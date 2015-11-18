/**
 ******************************************************************************
 * @file heap.h
 * @author Stef Servaes
 * $Rev: 1343 $
 * $Date: 2012-03-09 16:56:35 +0100 (Fri, 09 Mar 2012) $
 * @brief Implements simple dynamic memory allocation
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
 * Simple memory allocation for use without OS
 * Beware:
 * No protection on the ring descriptor values is implemented.
 * No re-entrant code.
 *
 *
 * @ingroup util
 */
#ifndef _HEAP_H_
#define _HEAP_H_
#include "types.h"

/** Dynamic memory allocation function
 *	To be used in systems without OS
 *	Not re-entrant, do not use from ISR.
 */
uint8_t* fMalloc(uint16_t size);

/** Dummy free
 *
 */
boolean fFree(uint8_t* address);

#endif // _HEAP_H_
