/**
 ******************************************************************************
 * @file fib.h
 * @author Stef Servaes
 * $Rev: 1285 $
 * $Date: 2012-02-01 11:58:05 +0100 (Wed, 01 Feb 2012) $
 * @brief Defines firmware information block
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
 * @details
 * A linked list of firmware information blocks (fibs) are stored in flash bank A.
 * A Fib contains location, size, revision number and size of the stored firmware.
 * Whenever a new firmware is downloaded, either over the air (OTA) or
 * using the UART download protocol, a new fib needs to be written into the flash.
 * All fibs are stored, one after the other in the first available not protected page of flash A.
 * Since flash write code is, in all cases, running from RAM, we assume only one linked
 * list of fibs is required to be stored in flash A.  Hence the fib structure does not
 * need a pointer to the next fib to construct a linked list.
 *
 @verbatim

  |                      |
  |                      |
  | Firmware             |
  |                      |
  ........................ 0x3000
  |                      |
  |                      |
  | FIB table            |
  ........................ 0x2000
  |                      |
  |    TRIM Values       |
  |                      |  8K protected page
  | This bootloader code |
  |  Bootloader vect.    |
  +----------------------+ 0x0
 @endverbatim
 * A fib is 20 bytes long and nicely word aligned.  We assume that the 4096bytes
 * flash page is reserved for storing fibs, that these are written one after the other
 * and that we'll never have to overwrite them because 4096bytes support 204 firmware upgrades.
 *
 *
 * @ingroup app
 */
#ifndef _FIB_H_
#define _FIB_H_

#include "types.h"
#include "device.h"
#include <stdint.h>

/** Firmware Information Block (fib)
 */
typedef struct fib{
	uint32_t base;		/**< Base offset of firmware, indicating what flash the firmware is in. (will never be 0x11111111) */
	uint32_t size;		/**< Size of the firmware */
	uint32_t crc;		/**< CRC32 for firmware correctness check */
	uint32_t rev;		/**< Revision number */
	uint32_t checksum;	/**< Check-sum of information block */
}fib_t, *fib_pt;

/** Traverse the fib list and return the newest FIB
 *
 * @param none
 * @return pointer to the most recent fib
 */
fib_pt fFibGetLatest();

/** Write a fib to flash
 *
 * @param flash pointer to flash device
 * @param fib pointer to the fib that needs to be written.
 * @return True if success.
 */
boolean fFibWriteFib(device_pt flash, fib_pt fib);


#endif //_FIB_H_
