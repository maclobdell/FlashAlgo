/**
 ******************************************************************************
 * @file heap.c
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
#include "heap.h"
#include "types.h"
//#include "print.h"

#define HEAP_SIZE			(uint8_t)0xFF
#define NUM_HEAPVECTORS		(uint8_t)0x05
#define HEAP_START			(uint32_t)GlobHeap
#define HEAP_END			(uint32_t)GlobHeap + HEAP_SIZE

/**< Global chunk of memory, size down as soon as final allocation requirement is known. */
uint8_t 	GlobHeap[HEAP_SIZE];
/**< Global value representing the pointer to next available memory */
uint32_t 	GlobHeapVector;

uint8_t* fMalloc(uint16_t size)
{
	uint32_t retval = 0;
    
	//Initialize the vector
	if (GlobHeapVector == 0) GlobHeapVector = HEAP_START;

	//Make size multiple of 32bit to avoid alignment issues
	if (size & 0x3) size |= 0x04;

	//Check if the heap has enough room left.
	if (size > (HEAP_END - GlobHeapVector))
		return 0;

	retval = GlobHeapVector;
    
	//Move the pointer ahead
	GlobHeapVector += size;

	return (uint8_t*) retval;
}


boolean fFree(uint8_t* address)
{
	// Don't free
	return True;
}
