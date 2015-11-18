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
#include "dma.h"
#include "fib.h"
#include "Interface.h"
#include "print.h"

#include "FlashOS.h"
#include "FlashPrg.h"

#define RESULT_OK                   0
#define RESULT_ERROR                1


uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags);

uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer);

uint32_t FlashErase(void *block_start,
                    uint32_t block_size);


//Need to remove all code derived from flash_loader template files and functions in the future.
//Note to OnSemi: Perhaps orion_flash.c can be bypassed and this file can call your
//                posix style flash driver in flash.c directly.  this is up to you.
//                this would eliminate extra layers and complexity.

uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    // Called to configure the SoC. Should enable clocks
    //  watchdogs, peripherals and anything else needed to
    //  access or program memory. Fnc parameter has meaning
    //  but currently isnt used in MSC programming routines
    
    uint32_t success_flag = RESULT_ERROR;
    
    /* Disable all interrupts */
    NVIC->ICER[0] = 0x1F;
    /* Clear all Pending interrupts */
    NVIC->ICPR[0] = 0x1F;
    /* Clear all pending SV and systick */
    SCB->ICSR = (uint32_t)0x0A000000;

    //Note to OnSemi - why relocate vectors?  Is this needed?
#if 0
    /* Relocate the exception vector table from default 0x0 to the location in RAM
     * from this download */
    __DSB();
    /* MSbit = 0 vector table in code region, 1 vector table in ram region */
    SCB->VTOR = (uint32_t)&__vector_table;
    __DSB();
#endif

    success_flag = FlashInit((void *)adr, 0, 0, 0);  //args not used

    return success_flag;  // 0 on success, an error code otherwise

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

    return 1;
}

uint32_t EraseSector(uint32_t adr)
{
    // Execute a sequence that erases the sector that adr resides in

    //Note to OnSemi: Please implement FlashErase in orion_flash.c
    //or maybe this is not needed because the flash driver is always erasing before programming
    return 1;
}

uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    // Program the contents of buf starting at adr for length of sz
    uint32_t success_flag = RESULT_ERROR;
    
    uint32_t block_start;
    uint32_t offset_into_block; 
    
    if ((adr & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK) {
      block_start = FLASH_B_OFFSET_MASK;
    }else{
      block_start = 0;
    }  

    offset_into_block = adr - block_start;
    //offset_into_block = adr & ~block_start;  //alternative
              
    success_flag = FlashWrite((void *) block_start,
                                 offset_into_block,
                                                sz,
                             (const char *) buf);
    
    return success_flag;
}

uint32_t Verify(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    // Given an adr and sz compare this against the content of buf

    //note to OnSemi - this function is optional 

    return 1;
}

