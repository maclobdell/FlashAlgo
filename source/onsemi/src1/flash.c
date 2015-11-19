/**
 ******************************************************************************
 * @file    flash.c
 * @author  Stef Servaes, Pierre Lebas
 * $Rev: 152 $
 * $Date: 2012-02-01 12:26:50 +0100 (Wed, 01 Feb 2012) $
 * @brief Implementation of a Flash memory driver
 ******************************************************************************
 * @copyright (c) 2012 ON Semiconductor. All rights reserved.
 * @internal
 * ON Semiconductor is supplying this software for use with ON Semiconductor
 * processor based microcontrollers only.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ON SEMICONDUCTOR SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 * @endinternal
 *
 * @ingroup FLASH
 *
 * @details
 *
 * <h1> Reference document(s) </h1>
 * <p>
 * </p>
 *
 * <h1> Functional description (internal) </h1>
 * <p>
 * </p>
 */
#include "types.h"                     
#include <stdio.h>
//#include "Interface.h" 

#include "flash_map.h"
#include "flash.h"
//#include "assert.h"
#include "heap.h"
//#include "dma.h"
#include <string.h>
//#include "print.h"

extern int debug_flag;   
extern uint32_t firmwareAddress;
/** This integer indicates the number of devices connected to the device driver.
 *  In this case the number of devices is 2, which denotes FLASH A and FLASH B
 */
extern uint8_t numDev;

#define ERROR_FLASH_ILLEGAL_SRC_ADDR 	                (uint8_t) 0x01
#define ERROR_FLASH_ILLEGAL_DST_ADDR 	                (uint8_t) 0x02
#define ERROR_FLASH_UNSUPPORTED_LEN		        (uint8_t) 0x03

#define IRQ_ACCESS_ERROR				(uint8_t) 0x04
#define IRQ_ERASE_COMPLETE				(uint8_t) 0x02
#define IRQ_PROGRAM_COMPLETE			        (uint8_t) 0x01
#define IRQ_NO_INTERRUPT				(uint8_t) 0x00
#define FLAG_FLASH_RMW					(uint8_t) 0x01

#define CTRL_POWER_UP					(uint8_t) 0x00
#define CTRL_POWER_DOWN					(uint8_t) 0x01
#define CTRL_STALL_ON_WRITE				(uint8_t) 0x01

#define CMD_NOP						(uint8_t) 0x00
#define CMD_PAGE_ERASE					(uint8_t) 0x01
#define CMD_MASS_ERASE					(uint8_t) 0x02

#define CTRL_UNLOCK1					(uint32_t)0xBB781AE9
#define CTRL_UNLOCK2					(uint32_t)0xB56D9099

#define MAX_FLASH_DEV                                   2 // 2 flash devices supported
/**< Global storage to store flash page for read-modify-write operation */
/* Needs 32bit alignement to make sure DMA can handle the data */


#if 0
block_driver_t flash_driver = {
		{
		&fFlashCreate,
		&fFlashOpen,
		&fFlashClose,
		&fFlashIoctl,
		Null // device list
		},
		&fFlashRead,
		&fFlashWrite };
#endif
//uint8_t GlobFlashPageCache[FLASH_PAGE_SIZE ] __attribute__((aligned (4)));

//#pragma data_alignment=4
uint8_t GlobFlashPageCache[FLASH_PAGE_SIZE ];   

/* Two flash memory devices are present */
flash_device_t flashDev[MAX_FLASH_DEV];

boolean fFlashCreate(device_pt *device);
boolean fFlashOpen(device_pt device, void *options);
void fFlashClose(device_pt device);
boolean fFlashRead(device_pt device, uint8_t **address, uint8_t * const buf, uint32_t len);
boolean fFlashWrite(device_pt device, uint8_t **address, const uint8_t *buf, uint32_t len);
boolean fFlashIoctl(device_pt device, uint32_t request, void *argument);


/** Unlock first page for write or erases
 * Note that TEST pin needs to be 1 to unlock.
 * Write of unlock for flash A or B needs to be done within 20clock
 * after writing the overall unlock1 register.
 *
 *  @param device pointer to the flash device
 */
void fFlashUnlock(flash_device_pt device)
{

	device->membase->UNLOCK1 = CTRL_UNLOCK1;

	if (device->array_base_address && FLASH_B_OFFSET_MASK) {
		// Flash B
		device->membase->UNLOCKB = CTRL_UNLOCK2;
	} else {
		// Flash A
		device->membase->UNLOCKA = CTRL_UNLOCK2;
	}
}

/** Stalls execution until busy flag is cleared
 *
 * @param device pointer to flash device
 */
void fFlashStallUntilNotBusy(flash_device_pt device)
{
	if (device->array_base_address && FLASH_B_OFFSET_MASK) {
		/* Check flash B busy */
		while (device->membase->STATUS.BITS.FLASH_B_BUSY) {
		}
	} else
		while (device->membase->STATUS.BITS.FLASH_A_BUSY) {
		}
}

/** Power down the flash
 *
 * @param device pointer to flash device
 */
void fFlashPowerDown(flash_device_pt device)
{
	if (device->array_base_address && FLASH_B_OFFSET_MASK) {
		device->membase->CONTROL.BITS.FLASHB_PD = CTRL_POWER_DOWN;
	} else
		device->membase->CONTROL.BITS.FLASHA_PD = CTRL_POWER_DOWN;
}

/** Power up the flash
 *
 * @param device pointer to flash device
 */
void fFlashPowerUp(flash_device_pt device)
{
	if (device->array_base_address && FLASH_B_OFFSET_MASK) {
		device->membase->CONTROL.BITS.FLASHB_PD = CTRL_POWER_UP;
	} else
		device->membase->CONTROL.BITS.FLASHA_PD = CTRL_POWER_UP;
}

/** Erases a flash page
 * It is up to the caller to make sure busy flag is checked before continuing.
 *
 * @param device pointer to the flash device
 * @param address address, relative to flash, does not need to be page start
 */
void fFlashPageErase(flash_device_pt device, uint32_t address)
{
	flash_device_pt d = (flash_device_pt) device;

	// Only one page erase address is available for both flash banks.
	// Hardware is masking least significant bits to identify what page
	// in what flash needs to be erased.
	if (device->array_base_address && FLASH_B_OFFSET_MASK) {
		// Flash device is on flash bank B
		// Add bank B offset to address.
		address += FLASH_B_OFFSET_MASK;
	}

	fFlashUnlock(d);

	// Kick off erase cycle.
	device->membase->ADDR = address;
	device->membase->COMMAND.WORD = CMD_PAGE_ERASE;
}

/** Erases a flash page
 * It is up to the caller to make sure busy flag is checked before continuing.
 *
 * @param device pointer to the flash device
 * @param address address, relative to flash, does not need to be page start
 */
void fFlashMassErase(flash_device_pt device)
{
	flash_device_pt d = (flash_device_pt) device;

	// Only one mass erase control address is available for both flash banks.
	// Hardware is masking least significant bits to identify what page
	// in what flash needs to be erased.
	if (device->array_base_address && FLASH_B_OFFSET_MASK) {
		// Flash device is on flash bank B
		// Add bank B offset to address.
		device->membase->ADDR = FLASH_B_OFFSET_MASK;
	} else {
		device->membase->ADDR = 0x00000000;
	}

	fFlashUnlock(d);

	// Kick off erase cycle.
	device->membase->COMMAND.WORD = CMD_MASS_ERASE;
}

/**
 * see driver.h for details
 */
boolean fFlashCreate(device_pt *device) {

 
        //fprintf("Create device for Flash %d", numDev);
 
        ++ numDev;  

        
        // Same driver, new flash device added to the driver
        flash_device_pt d = (flash_device_pt)(fMalloc(sizeof(flash_device_t)));
        if (d == Null) {
		ASSERT(False);
		return False;
	}
	// a check to see if the compiler has aligned the
	ASSERT(&(d->device) == (device_pt) d);

	// add the device to the driver's device list
	*device = &(d->device);
//	(d->device).next = flash_driver.driver.deviceList;
//	flash_driver.driver.deviceList = &(d->device);

	// device is not active yet
	(d->device).open = False;

	return True;
}

/**
 * see driver.h for details
 */
boolean fFlashOpen(device_pt device, void *options) {
  
	flash_device_pt d = (flash_device_pt) device;
	flash_options_pt opt = options;

	/* Assign the flash bank base address */
	d->array_base_address = (uint32_t) opt->array_base_address;
	/* Set the device open flag */
	d->device.open = True;
	/* Assign the control register offset */
	d->membase = opt->base_address_control;

	return True;
}

/**
 * see driver.h for details
 */
void fFlashClose(device_pt device) {
}

/**
 * Read buffer from flash and increment address pointer
 *
 * Note that if flash is implemented on silicon, there's no need to call read function since read
 * is supported by regular memory read access.  Read function is to be used when large sections
 * of flash need to be copied in RAM, eg for write.
 *
 */
boolean fFlashRead(device_pt device, uint8_t **address, uint8_t * const buf, uint32_t len)
{
	uint8_t *destination = buf;
	uint8_t *source;

	if (!device->open) {
		return False;
	}

	source = (((flash_device_pt) (device))->array_base_address) + (uint8_t*) (*address);

	/** most stupid implementation.
	 * Better is to check alignment and do word or even block copy in asm.
	 * Best is to use DMA engine.
	 */
	while (len-- > 0) {
		*destination++ = *source++;
	}
	*address = source;

	return False;
}

/**
 * Note: bootloader section (first 8K) can only be flashed when the test pin is 1 and the flash is
 * unlocked.
 *
 * @todo add support for at least 1 write across page boundary
 */
boolean fFlashWrite(device_pt device, uint8_t **address, const uint8_t *buf, uint32_t len)
{
	uint8_t *destination;
	uint8_t *page_address;
	uint32_t page_size;
	uint8_t *in_page_offset;
          
 
	flash_device_pt d = (flash_device_pt) device;
        if (d == Null) {
          //fprintf("Device is null");
        }
	if ((device->open) != True) {
                //fprintf("device not open");
		return False;
	}
        
        destination = (((flash_device_pt) (device))->array_base_address) + (uint8_t*) (*address);
        
        //fprintf("Destination = %0x", destination);
        page_address = (uint8_t*) ((uint32_t) destination & FLASH_PAGE_MASK );
	page_size = FLASH_PAGE_SIZE;
       
        
	if ((uint32_t) (*address) < (FLASH_NR_INFO_BLOCK_PAGES * FLASH_PAGE_SIZE_INFO_BLOCK )) {
		// First page write requested, info block has FLASH_NR_INFO_BLOCK_PAGES pages of size FLASH_PAGE_SIZE_INFO_BLOCK.

		page_address = (uint8_t*) ((uint32_t) destination & FLASH_FIRST_PAGE_MASK );
		page_size = FLASH_PAGE_SIZE_INFO_BLOCK;
		/* No support for crossing page boundary */
		if (((uint32_t) (*address) % FLASH_PAGE_SIZE_INFO_BLOCK ) == 0) {
			// Address is page aligned
			if (len > FLASH_PAGE_SIZE_INFO_BLOCK)
				return False;
		} else {
			if (len > ((uint32_t) (*address) % FLASH_PAGE_SIZE_INFO_BLOCK ))
				return False;
		}
	}

	/* No support for crossing page boundary */
	if (((uint32_t) (*address) % FLASH_PAGE_SIZE ) == 0) {
		// Address is page aligned
		if (len > FLASH_PAGE_SIZE)
			return False;
	} else {
		if (len > ((uint32_t) (*address) % FLASH_PAGE_SIZE ))
			return False;
	}

        /* Read full page into RAM */
	memcpy(GlobFlashPageCache, page_address, FLASH_PAGE_SIZE);

        /* Partially overwrite the content of the page in RAM */
	in_page_offset = (uint8_t*) ((uint32_t) (*address) & ~FLASH_PAGE_MASK );
	memcpy(GlobFlashPageCache + (uint32_t) in_page_offset, buf, len);

	/* Order page erase of the target page.  Just use the destination address
	 * to program page erase, hardware makes sure the correct page is erased
	 */
	fFlashPageErase(d, (uint32_t) *address);

	fFlashStallUntilNotBusy(d);

	/* Make sure the CM3 hangs while write is ongoing */
	d->membase->CONTROL.BITS.WRITE_BLOCK = CTRL_STALL_ON_WRITE;

	fFlashUnlock(d);

	/* Commit the page to flash */

//Implement without DMA / Interrupt??
//#error "Need to update this part"
//	fDmaStart((uint32_t) page_address, (uint32_t) GlobFlashPageCache, page_size);
//	fDmaStallUntilDone();
  
        *address += len;

	return True;
}

/**
 * Flash IO control function, supports call to erase, power on and off of flash
 * bank
 *
 * @param device pointer to the flash device descriptor
 * @param request the IOCTL
 * @param argument the optional arguments to the IOCTL
 */
boolean fFlashIoctl(device_pt device, uint32_t request, void *argument)
{
	flash_device_pt d = (flash_device_pt) device;
	if (!device->open) {
          //FlMessageBox("Flash not open");
          return False;
	}
	switch (request) {
	case FLASH_MASS_ERASE_REQUEST :
		/* Device instance dictates what bank will be mass erased */
		fFlashMassErase(d);
		fFlashStallUntilNotBusy(d);
		break;
	case FLASH_PAGE_ERASE_REQUEST :
		/* get the page from argument and erase */
		fFlashPageErase(d, *(uint32_t*) argument);
		fFlashStallUntilNotBusy(d);
		break;
	case FLASH_POWER_DOWN :
		/* No unlock required fFlashUnlock(d); */
		fFlashPowerDown(d);
		break;
	case FLASH_POWER_UP :
		/* No unlock required fFlashUnlock(d); */
		fFlashPowerUp(d);
		fFlashStallUntilNotBusy(d);
		break;
	case FLASH_WAIT_UNTIL_DONE :
		fFlashStallUntilNotBusy(d);
		break;
	default:
		break;
	}
	return True;
}

/**
 *
 */
void fFlashHandler(FlashReg_pt membase)
{
	if (membase->INT_STATUS.BITS.INT_PEND) {
		switch (membase->INT_STATUS.BITS.INT_TYPE) {
		case IRQ_ACCESS_ERROR :
			break;
		case IRQ_ERASE_COMPLETE :
			break;
		case IRQ_PROGRAM_COMPLETE :
			break;
		default:
			break;
		}
	}
}
