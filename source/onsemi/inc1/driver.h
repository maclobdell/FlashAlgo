/**
 ******************************************************************************
 * @file driver.h
 * @author Michiel Verschueren
 * $Rev: 1617 $
 * $Date: 2012-09-10 18:26:47 +0200 (Mon, 10 Sep 2012) $
 * @brief Defines a driver data type
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
 * A generic driver template is defined as a structure with a list of function
 * pointers. Any driver implementation must instantiate a variable of this
 * structure type, and assign functions to one or more of the function pointers.
 * This driver variable can then be made public (extern) to make it accessible
 * to other files. The functions are the only interface to the driver.
 *
 * The create() function is intended to create a new instance of a device type
 * (see device.h). The device type is to be defined by the driver implementation.
 * The other functions take a pointer to this instance as an argument, to act
 * on the appropriate device. This implies that multiple devices can be used
 * with the same driver. Any create() function must take care of allocating the
 * necessary amount of memory for the device.
 *
 * After creating the device, it has to be opened with open() before using it.
 * Once opened, it can be closed again using close(). When opening a device, the
 * user can pass options to configure the device. The structure of the options
 * is to be defined by the driver implementation. Use a NULL pointer for passing
 * no options.
 *
 * Ioctl() is intended for device-specific operations (other than read or write),
 * such as controlling I/O pins on the device.
 *
 * Finally, the driver keeps track of all devices that it has created in the
 * device list, which is implemented as a linked list. Adding a newly created
 * device to the list must be included in the create() function of any driver
 * implementation.
 *
 * The generic driver template can be extended to create more specific driver
 * templates that are dedicated to a certain type of device. For example, the
 * generic driver template can be extended to a character driver template, which
 * can in turn be extended to a driver for a particular UART device. This is
 * shown in the diagram below.
 *
 * @image html "../../../../rtos/drivers/doc/architecture.png"
 *
 * The organization of the folders that correspond to the architecture shown
 * above, is displayed in the next image. This organization should also be
 * reflected in the hierarchy of the source code documentation.
 *
 * @image html "../../../../rtos/drivers/doc/organization.png"
 *
 * @ingroup drivers
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include <stdint.h>

#include "types.h"
#include "device.h"

/** A generic driver structure. */
typedef struct driver {
	/** Creates a new device instance.
	 * @details
	 * An implementation of create must allocate the necessary memory for the device.
	 * It must also initialize the device's ID and any other driver-specific fields
	 * that the device may have.
	 *
	 * @param device A placeholder for the newly created device.
	 * @return True if creation was successful.
	 */
	boolean (*create)(device_pt *device);

	/** Opens a device (after it has been created).
	 * @details
	 * An implementation of open must enable the necessary interrupts and set the correct
	 * register values. The registers to set may depend on the device opened, which can
	 * be identified by it's ID or other fields.
	 *
	 * @param device The device instance to open.
	 * @param options An optional pointer to a collection of options, depending on the implementation.
	 * @return True if opening the device was successful.
	 */
	boolean (*open)(device_pt device, void *options);

	/** Closes a device.
	 * @details
	 * An implementation of close must disable any necessary interrupts.
	 *
	 * @param device The device instance to close.
	 */
	void (*close)(device_pt device);

	/** Performs a generic I/O operation on a device.
	 * @param device The device to perform an operation on.
	 * @param request A driver-dependent request code.
	 * @param argument An optional pointer to a structure of arguments.
	 * @return True if the operation was successful.
	 */
	boolean (*ioctl)(device_pt device, uint32_t request, void *argument);

	/** The head of the linked list of all devices created by the driver. */
	device_pt deviceList;
} driver_t, *driver_pt;

#endif /* DRIVER_H_ */
