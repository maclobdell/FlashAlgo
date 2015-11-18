/**
 ******************************************************************************
 * @file dma.c
 * @author Stef Servaes
 * $Rev: 346 $
 * $Date: 2012-04-23 14:47:52 +0200 (Mon, 23 Apr 2012) $
 * @brief DMA hw module api
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
 * @ingroup DMA
 *
 * @details
 */
#include "dma.h"
#include "dma_map.h"
#include "types.h"     
#include "Interface.h"
   
#define DMAREG_BASE         ((uint32_t)0x24000400)
#define DMAREG              ((DmaReg_t *)DMAREG_BASE)

#define DMA_STATUS_DONE			(uint8_t)0x00
#define DMA_ERROR_SOURCE		(uint8_t)0x01
#define DMA_ERROR_DESTINATION	(uint8_t)0x02
#define DMA_CMD_ENABLE			(uint8_t)0x01
#define DMA_CMD_DISABLE			(uint8_t)0x00
#define DMA_IRQ_DMA_DONE		(uint8_t)0x01
#define DMA_IRQ_ERROR_SRC		(uint8_t)0x02
#define DMA_IRQ_ERROR_DST		(uint8_t)0x04

void fDmaDefaultCallBack();

DmaDescr_t GlobDmaDescr = { 0, fDmaDefaultCallBack };

void fIrqDmaHandler() {
	NVIC_DisableIRQ(Dma_IRQn);
	/* Clear the interrupt source */
	DMAREG->INT_CLEAR_ENABLE = DMAREG->INT_STATUS;

	switch (DMAREG->INT_STATUS) {
	case DMA_STATUS_DONE:
		GlobDmaDescr.callback();
		break;
	case DMA_ERROR_SOURCE:
		DMAREG->CONTROL = DMA_CMD_DISABLE;
        FlMessageBox("Error DMA source\n");
		//fHardFault();
		while(1);       //jb
                break;
	case DMA_ERROR_DESTINATION:
		DMAREG->CONTROL = DMA_CMD_DISABLE;
        FlMessageBox("Error DMA destination\n");
		//fHardFault();
		while(1);       //jb
                break;
	default:
		break;
	}
	/** Unlock the loop that blocked on DMA request */
	GlobDmaDescr.lock = 0;
}

void fDmaDefaultCallBack()
{
}

boolean fDmaStart(const uint32_t destination_addres,
		const uint32_t source_address, const uint32_t len) 
{
	/* Get the lock, the lock will only be released if DMA action
	 * completed.
	 */
	//while (fGetLock(&(GlobDmaDescr.lock))) { //jb
	//}

	DMAREG->DESTINATION = destination_addres;
	DMAREG->SOURCE = source_address;
	DMAREG->SIZE = len;
	/* Enable the interrupt and start the DMA copy */
	NVIC_ClearPendingIRQ(Dma_IRQn);
	NVIC_EnableIRQ(Dma_IRQn);

	DMAREG->INT_ENABLE = DMA_IRQ_DMA_DONE | DMA_IRQ_ERROR_DST | DMA_IRQ_ERROR_SRC;
	DMAREG->CONTROL = 1;

	return True;
}

void fDmaStallUntilDone()
{
	while (GlobDmaDescr.lock == 1) {}
}

boolean fDmaRegisterCompleteHandler(fCallBack_pt cb)
{
	if (cb != 0) {
		GlobDmaDescr.callback = cb;
		return True;
	} else
		return False;
}
