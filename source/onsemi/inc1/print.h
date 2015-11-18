/**
 ******************************************************************************
 * @file print.h
 * @brief API definition for printf like functions
 * @internal
 * @author Stef Servaes
 * version $Rev: 2542 $
 * date $Date: 2013-11-19 12:39:14 +0100 (Tue, 19 Nov 2013) $
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
 * @details
 * @ingroup print
 */

#ifndef PRINT_H_
#define PRINT_H_

#include <stdint.h>
#include "device.h"

/** Defines function type for function pointer. The function will
 * have to implement streaming out len number characters contained in the
 * character buffer, buf, of the peripheral device pointed to by device.
 */
typedef void (*fPrint_tp)(device_pt device, uint8_t const *buf, uint32_t len);


/**
* @brief Perform same as fPrintf, but now the output is placed
* in a buffer that is returned
*
* @param buffer The output buffer, where is printed into
* @param fmt The variable [arameter list, input of print
* @param ...
*/
void fSprintf(char *buffer, const int8_t *fmt, ...);


/**
 * @brief
 * printf function
 * Use like regular C printf
 * The actual output is wired to whatever hw/sw function that was bound to this function
 * using fPrintBind.
 *
 * @param fmt format string
 * @param ... variable argument list
 */
void fPrintf(const int8_t *fmt, ...);

/**
 * @brief
 * Bind a function to print characters
 *
 * @param fp function pointer to function that does the character print
 * @param device pointer to the printing device
 */
void fPrintBind(fPrint_tp, device_pt device);

/**
 * @brief
 * Parse a formatted string
 *
 * @param fmt variable argument list containing the string to be parsed and variables to be 
 * formatted into the string
 * @return a pointer to a global result string.
 */
int8_t* fPrintParseString(const int8_t *fmt, ...);

#endif /* PRINT_H_ */
