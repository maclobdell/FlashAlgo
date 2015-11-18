/**
 ******************************************************************************
 * @file device.h
 * @author Michiel Verschueren
 * $Rev: 1617 $
 * $Date: 2012-09-10 18:26:47 +0200 (Mon, 10 Sep 2012) $
 * @brief Defines a generic device.
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
 * A device is implemented as a very simple structure, with only an ID. The actions
 * that are performed on the physical device are defined in a driver implementation
 * (see driver.h).
 *
 * An actual device may need additional fields, which can be easily done by creating
 * a new structure data type, of which the first element is a device_t variable. The
 * driver instance may then cast variables of the generic device_t type to instances
 * of the new data type, to access the additional fields. For example, additional
 * fields may be send and receive queues.
 *
 * @ingroup drivers
 */

#ifndef DEVICE_H_
#define DEVICE_H_

/** A generic device structure. */
typedef struct device {
	boolean open;
	struct device *next; /**< A pointer to the next device in the driver's linked list of devices. */
} device_t, *device_pt;

#endif /* DEVICE_H_ */
