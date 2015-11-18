/**
 ******************************************************************************
 * @file dma_map.h
 * @author Stef Servaes, Pierre Lebas
 * $Rev: 346 $
 * $Date: 2012-04-23 14:47:52 +0200 (Mon, 23 Apr 2012) $
 * @brief DMA hw module register map
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

//#ifdef CM3
#include "system_ARMCM3.h"
#include "core_cm3.h"
//#endif
#ifdef CM0
#include <system_ARMCM0.h>
#include <core_cm0.h>
#endif

#ifndef DMA_MAP_H_
#define DMA_MAP_H_

typedef struct {
	__IO uint32_t CONTROL;				/**< Write 1 to enable DMA, write 0 to disable */
	__IO uint32_t SOURCE;				/**< Address of source, read to get the number of bytes written */
	__IO uint32_t DESTINATION;			/**< Address of destination, read to get the number of bytes written  */
	__IO uint32_t SIZE;					/**< Lenght of the entire transfer */
	__IO uint32_t STATUS;				/**< To be debined */
	__IO uint32_t INT_ENABLE;			/**< Enable interrupt source by writing 1. Bit 0: DMA done, Bit 1: Source Error, Bit 2: Destination Error */
	__IO uint32_t INT_CLEAR_ENABLE;		/**< Clear Interrupt source by writing 1. Bit 0: DMA done, Bit 1: Source Error, Bit 2: Destination Error */
	__I  uint32_t INT_STATUS;			/**< Current interrupt status. Bit 0: DMA done, Bit 1: Source Error, Bit 2: Destination Error */
} DmaReg_t, *DmaReg_pt;

#endif /* DMA_MAP_H_ */
