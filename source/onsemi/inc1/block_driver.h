/**
 ******************************************************************************
 * @file block_driver.h
 * @author Michiel Verschueren
 * $Rev: 1617 $
 * $Date: 2012-09-10 18:26:47 +0200 (Mon, 10 Sep 2012) $
 * @brief Defines a block driver data type.
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
 * The block driver is intended for devices that allow read and write operations
 * on "blocks" of memory, such as DMA controllers or flash memory.
 *
 * The block driver derives from the generic driver template (see driver.h). It
 * does so by including an element of the generic driver_t type.
 *
 * The read and write operation both take an address that points to the start of
 * the region where the operation should take place. After the operation, it will
 * point to the next region.
 *
 * @ingroup block_drivers
 */

#ifndef BLOCK_DRIVER_H_
#define BLOCK_DRIVER_H_

#include "driver.h"

/** A block driver structure. */
typedef struct block_driver {
	/** The parent generic driver. */
	driver_t driver;

	/** Reads bytes from a memory region into a buffer.
	 * @param device The block device to read from.
	 * @param address Pointer to the address where the driver starts reading; pointer to the next location to read from.
	 * @param buf The buffer to read into.
	 * @return True if reading was successful.
	 */
	boolean (*read)(device_pt device, uint8_t **address, uint8_t *const buf, uint32_t len);

	/** Writes bytes to a memory region from a buffer.
	 * @param device The block device to write to.
	 * @param address Pointer to the address where the driver starts writing; pointer to the next location to write.
	 * @param buf The buffer to read from.
	 * @return True if writing was successful.
	 */
	boolean (*write)(device_pt device, uint8_t **address, const uint8_t *buf, uint32_t len);
} block_driver_t, *block_driver_pt;

#endif /* BLOCK_DRIVER_H_ */
