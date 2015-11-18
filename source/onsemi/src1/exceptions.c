/**
 ******************************************************************************
 * @file exceptions.c
 * @brief Exception Handlers
 * @internal
 * @author Balaji Jalluri
 * $Rev: 2074 $
 * $Date: 2013-09-12 17:46:15 +0530 (Thu, 12 Sep 2013) $
 ******************************************************************************
 * @copyright (c) 2012 ON Semiconductor. All rights reserved.
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
 * @ingroup exceptions
 *
 * @details
 * <p>
 * </p>
 *
 */
#include "types.h"
#include "print.h"

static uint32_t __get_MSP(void)
{
  asm("mrs r0, msp");
}

/** Hardware fault interrupt handler */
void HardFault_Handler(void) {

  fPrintf("MSP: 0x%x", __get_MSP());
  
  fPrintf("Fatal Error, hard fault at 0x%x\n", *(int*)(__get_MSP()+24));
  /*fPrintf("Stack trace:\n");
  fPrintf("0x%x:8 0x%x:8 0x%x8 0x%x:8",  *(int*)(__get_MSP()+0),  *(int*)(__get_MSP()+4),  *(int*)(__get_MSP()+8),  *(int*)(__get_MSP()+12));
  fPrintf("0x%x:8 0x%x:8 0x%x8 0x%x:8",  *(int*)(__get_MSP()+16),  *(int*)(__get_MSP()+20),  *(int*)(__get_MSP()+24),  *(int*)(__get_MSP()+28));
  fPrintf("0x%x:8 0x%x:8 0x%x8 0x%x:8",  *(int*)(__get_MSP()+32),  *(int*)(__get_MSP()+36),  *(int*)(__get_MSP()+40),  *(int*)(__get_MSP()+44));  
  */  
  asm("bkpt 0");
}

/** Call the FreeRTOS SVC IRQ handler */
void SVC_Handler() {
	
}

/** Call the FreeRTOS pending SVC IRQ handler */
void PendSV_Handler() {
	
}

/** Call the CM3 System Tick interrupt IRQ handler */
void SysTick_Handler() {
	
}

/** Call the MacHw IRQ handler */
void fIrqMacHwHandler() {
	
}

/** Call the Timer0 IRQ handler */
void fIrqTim0Handler() {
	
}

/** Call the Timer1 IRQ handler */
void fIrqTim1Handler() {
	
}

/** Call the Timer2 IRQ handler */
void fIrqTim2Handler() {
       
}

/** Call the Gpio IRQ handler */
void fIrqGpioHandler() {
	
}

/** Call the Spi IRQ handler */
void fIrqSpiHandler() {
	
}

/** Call the Uart 1 IRQ handler */
void fIrqUart1Handler(void) {
	
}

/** Call the Uart 2 IRQ handler */
void fIrqUart2Handler(void) {
	
}

void fIrqAdcHandler(void) {
	
}

void NotImplemented_Handler(void)
{
        
}
