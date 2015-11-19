/* Flash OS Routines
 * Copyright (c) 2009-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file FlashPrg.c */


#include "system_ARMCM3.h"
#include "core_cm3.h"
#include "types.h" 
#include "flash.h"
//#include "dma.h"
//#include "fib.h"
//#include "Interface.h"
//#include "print.h"

#include "FlashOS.h"
#include "FlashPrg.h"

#define RESULT_OK                   0
#define RESULT_ERROR                1

void fInitGobjects(void);
void fInitRam(void);  

/** Global flash A device pointer*/
device_pt GlobFlashDeviceA;

/** Global flash B device pointer */
device_pt GlobFlashDeviceB;

/** Global flash A device options */
const flash_options_t GlobFlashOptionsA = {
		(uint32_t*)0x00000000,
		FLASHREG,
		Flash_IRQn

};
/** Global flash B device options */
const flash_options_t GlobFlashOptionsB = {
		(uint32_t*)0x00100000,
		FLASHREG,
		Flash_IRQn

};

uint8_t numDev;

uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    // Called to configure the SoC. Should enable clocks
    //  watchdogs, peripherals and anything else needed to
    //  access or program memory. Fnc parameter has meaning
    //  but currently isnt used in MSC programming routines
  

	/* Disable all interrupts */
    NVIC->ICER[0] = 0x1F;
    /* Clear all Pending interrupts */
    NVIC->ICPR[0] = 0x1F;
    /* Clear all pending SV and systick */
    SCB->ICSR = (uint32_t)0x0A000000;

	#if 0
    /* Relocate the exception vector table from default 0x0 to the location in RAM
     * from this download */
    __DSB();
    /* MSbit = 0 vector table in code region, 1 vector table in ram region */
    //SCB->VTOR = (uint32_t)&__vector_table;
			SCB->VTOR = (uint32_t)&__Vectors;   
	 __DSB();
#endif

/*REFACTORING AGAIN TO MAKE POSITION INDEPENDENT */
		 
	    boolean retVal = True;
   //  int32_t successFlag = 0;
   //  int32_t addrVer[2];
     
   //  fInitGobjects();
     
     /* Read firmware base address and size */
     //successFlag=fReadFwbaseAddVer(BINFILENAME,(int32_t *)addrVer);  
       
  //   if(successFlag != 0)
  //   {   
  //     FlMessageBox("Error Reading  FW Base address and Version Number\n");
  //     return RESULT_ERROR;
  //   }  
   //  firmwareAddress = (uint32_t)addrVer[0];
     
     //retVal = flash_driver.driver.create(&GlobFlashDeviceA);
     retVal = fFlashCreate(&GlobFlashDeviceA);

		if (retVal != True) {
     //   FlMessageBox("Create Flash A driver failed ");
        return(RESULT_ERROR);
     }
     
     //retVal = flash_driver.driver.open(GlobFlashDeviceA, (void*) &GlobFlashOptionsA);
     retVal = fFlashOpen(GlobFlashDeviceA, (void*) &GlobFlashOptionsA);
		 
		 if (retVal != True) {
      //FlMessageBox("Open Flash A driver failed");
      return(RESULT_ERROR);
     }

     //retVal = flash_driver.driver.create(&GlobFlashDeviceB);
		 retVal = fFlashCreate(&GlobFlashDeviceB);
     if (retVal != True) {
       //FlMessageBox("Create Flash B driver failed ");
       return(RESULT_ERROR);
     }
     //retVal = flash_driver.driver.open(GlobFlashDeviceB, (void*) &GlobFlashOptionsB);
     retVal = fFlashOpen(GlobFlashDeviceB, (void*) &GlobFlashOptionsB);
		 if (retVal != True) {
      //FlMessageBox("Open Flash B driver failed");
      return(RESULT_ERROR);
     }
 /*    retVal = flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_UP, 0);
     if (retVal != True) 
     {
       FlMessageBox("Flash B power up failed");
       return(RESULT_ERROR);
     }
*/
     //if ((firmwareAddress & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK)
		 if ((adr & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK)
     {
     //  fPrintf("Binary for flash B, power UP B and DOWN A.\n");
       //flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_UP, 0);
			 fFlashIoctl(GlobFlashDeviceB, FLASH_POWER_UP, 0);
       //flash_driver.driver.ioctl(GlobFlashDeviceA, FLASH_POWER_DOWN, 0);
			 fFlashIoctl(GlobFlashDeviceA, FLASH_POWER_DOWN, 0);
     } else {
      // fPrintf("Binary for flash A, power UP A and power DOWN B.\n");
       //flash_driver.driver.ioctl(GlobFlashDeviceA, FLASH_POWER_UP, 0);
	     fFlashIoctl(GlobFlashDeviceA, FLASH_POWER_UP, 0);
			 //flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_DOWN, 0);
			 fFlashIoctl(GlobFlashDeviceB, FLASH_POWER_DOWN, 0);
     }
     return RESULT_OK;
	 
}

uint32_t UnInit(uint32_t fnc)
{
    // When a session is complete this is called to powerdown
    //  communication channels and clocks that were enabled
    //  Fnc parameter has meaning but isnt used in MSC program
    //  routines

   //Note to OnSemi: Anything to be done here?

    return 0;  //success
}

uint32_t BlankCheck(uint32_t adr, uint32_t sz, uint8_t pat)
{
    // Check that the memory at address adr for length sz is 
    //  empty or the same as pat
    
   //note to OnSemi - this function is optional 
      
    return 1;
}

uint32_t EraseChip(void)
{
    // Execute a sequence that erases the entire of flash memory region 

  //Note to OnSemi: Please implement FlashMassErase in orion_flash.c
  //FlashMassErase should call fFlashMassErase or ioctl command that calls it

	// Write call is doing erase by itself        
      return RESULT_OK;
    
}

uint32_t EraseSector(uint32_t adr)
{
    // Execute a sequence that erases the sector that adr resides in

    //Note to OnSemi: Please implement FlashErase in orion_flash.c
    //or maybe this is not needed because the flash driver is always erasing before programming
    
	// Write call is doing erase by itself        
      return RESULT_OK;
}

uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
	//unsigned char success_flag;                                  
    boolean retVal = True;
		
    //t1_dptr= ((uint8_t *)block_start)+offset_into_block;   

    //fPrintf("FIB base = %0x", firmwareAddress);
    
    /* Write to flash A or Flash B depending on the flash bank in use */
    //if ((firmwareAddress & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK) {
      if ((adr & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK) {
				
      //t1_dptr =(uint8_t *)((uint32_t)block_start ^ FLASH_B_OFFSET_MASK);
      
      //fPrintf("Write 0x%x bytes to flash B, 0x%x\n", count, t1_dptr);
      //success_flag=flash_driver.write(GlobFlashDeviceB,(uint8_t **)&t1_dptr,
      //                                     (uint8_t const *)buffer,count);
			//retVal =flash_driver.write(GlobFlashDeviceB,(uint8_t **)&adr,
      //                                     (uint8_t const *)buf,sz);
				
				retVal =fFlashWrite(GlobFlashDeviceB,(uint8_t **)&adr,
                                           (uint8_t const *)buf,sz);
				
    } else {
      
      //fPrintf("Write 0x%x bytes to flash A, 0x%x\n", count, t1_dptr);
      //success_flag=flash_driver.write(GlobFlashDeviceA,(uint8_t **)&t1_dptr,
      //                                     (uint8_t const *)buffer,count); 
      //retVal=flash_driver.write(GlobFlashDeviceA,(uint8_t **)&adr,
      //                                     (uint8_t const *)buf,sz); 
        retVal=fFlashWrite(GlobFlashDeviceA,(uint8_t **)&adr,
                                           (uint8_t const *)buf,sz); 
    }
    
    if(retVal==True)  
    {
      return RESULT_OK;
    }  
    
    return RESULT_ERROR;		
		
}

uint32_t Verify(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    // Given an adr and sz compare this against the content of buf

    //note to OnSemi - this function is optional 

    return 1;
}
