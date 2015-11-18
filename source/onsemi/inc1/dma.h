/**
 ******************************************************************************
 * @file dma.h
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
#ifndef _DMA_H_
#define _DMA_H_
#include <stdint.h>
#include "types.h"


/** Typedef for call back, called as soon as requested DMA has complete */
typedef void (*fCallBack_pt)();


typedef struct {
	volatile uint32_t lock;	/**< variable to avoid multiple parallel use of DMA engine */
	fCallBack_pt callback;	/**< function pointer to call when DMA interrupt occurs */
} DmaDescr_t;


/**
 * Kick off DMA sequence
 *
 * @param source_address address of the original data
 * @param destination_addres destination address
 * @param len the number of bytes to be DMA-ed
 * @return succes or failure indication
 */
boolean fDmaStart(const uint32_t destination_addres, const uint32_t source_address, const uint32_t len );

/**
 * Registers a call back function.
 * Function is called upon completion of the DMA
 *
 * @param cb function pointer to the call back function
 * @return success when valid (within range) function pointer
 */
boolean fDmaRegisterCompleteHandler(fCallBack_pt cb);

/**
 * Stall execution until DMA is completed
 */
void fDmaStallUntilDone();

/**
 * DMA interrupt handler
 */
void fIrqDmaHandler();

#endif //_DMA_H_
