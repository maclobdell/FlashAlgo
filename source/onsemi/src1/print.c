/**
 ******************************************************************************
 * @file print.c
 * @brief Implements printf like functions
 * @internal
 * @author Stef Servaes
 * $Rev: 2567 $
 * $Date: 2013-11-25 13:41:14 +0100 (Mon, 25 Nov 2013) $
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
#include "types.h"

#include <stdarg.h>
#include "print.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yfuns.h>

#include "interface.h"

#define FLOAT_NUMDEC 3			/** number of decimal for float */
#define FLOAT_MAG 1000			/** 10 ^ FLOAT_NUMDEC */

/** Global variable containing the string to be transmitted to the registered interface */
int8_t GlobPrintBuff[256];
/** Global index into the global print buffer */
uint32_t GlobBuff_i = 0;

/** Function pointer the the registered function that will send the string buffer to the registered interface */
fPrint_tp fPrintCharacter = 0;
/** Global pointer the the peripheral device structure that will print the strings */
device_pt GlobPrinter = 0;
/** Global constant containing the characters for hex number representation */
const int8_t GlobHexNum[17] = "0123456789ABCDEF";


/**
 * @brief
 * Bind a function to print characters
 *
 * @param fp function pointer to function that does the character print
 * @param device pointer to the printing device
 */
void fPrintBind(fPrint_tp fp, device_pt device) {
	fPrintCharacter = fp;
	GlobPrinter = device;
}

/**
 * @brief
 * Convert an integer to a string representation in decimal
 * @param int_src the integer to be converted
 * @param buf the string buffer
 * @param buf_i the variable parameter having the offset into the string buffer
 */
void fInteger2dec(int32_t int_src, int8_t *buf, uint32_t* buf_i) {
	int8_t neg = 0;
	int32_t tmp_i = 0;
	int8_t tmp[30];
	/* uint8_t stuff_i = 0; */

	if (int_src < 0) {
		int_src = -int_src;
		neg = 1;
	};

	do {
		tmp[tmp_i++] = GlobHexNum[int_src % 10];
		int_src /= 10;
	} while (int_src != 0);
	if (neg)
		tmp[tmp_i++] = '-';

	/* Don't Stuff '_' character before number to make it at least 3 digits
	if (tmp_i < 3) {
		stuff_i = (3 - tmp_i);
		while (stuff_i--)
			tmp[tmp_i++] = '_';
	}
    */
	tmp_i--;

	while (tmp_i >= 0) {
		buf[(*buf_i)++] = tmp[tmp_i--];
	};
}

/**
 * @brief
 * Convert a float to a decimal string representation.
 * @param float_src the float to be converted
 * @param buf
 * @param buf_i the variable parameter having the offset into the string buffer
 */
void fFloat2dec(float float_src, int8_t *buf, uint32_t* buf_i) {
	int32_t t = (int32_t) (FLOAT_MAG * float_src);
	int8_t neg = 0;
	int32_t tmp_i = 0;
	int8_t tmp[30];

	if (float_src > 0x100000) {
		float_src = 0x100000;
	}
	if (float_src < -0x100000) {
		float_src = -0x100000;
	}
	if (t < 0) {
		t = -t;
		neg = 1;
	};
	do {
		tmp[tmp_i++] = '0' + (t % 10);
		t /= 10;
	} while ((t != 0) || (tmp_i <= FLOAT_NUMDEC));	// at least FLOAT_NUMDEC decimal
	if (neg)
		tmp[tmp_i++] = '-';
	tmp_i--;
	neg = 1;
	while (tmp_i >= 0) {
		// 'neg' used as one shot
		if ((tmp_i == FLOAT_NUMDEC - 1) && (neg == 1)) {
			// insert dot before FLOAT_NUMDEC decimal
			buf[(*buf_i)++] = '.';
			neg = 0;
		} else
			buf[(*buf_i)++] = tmp[tmp_i--];
	};
}

/**
 * @brief
 * Convert a float to a scientific notation string representation.
 * @param float_src the float to be converted
 * @param buf
 * @param buf_i the variable parameter having the offset into the string buffer
 */
void fFloat2sci(float float_src, int8_t *buf, uint32_t* buf_i) {
	int32_t tmp_i = 0;
	int8_t tmp[30];
	int32_t exp = 0;
	int32_t i = 0;

	if (float_src < 0) {
		tmp[tmp_i++] = '-';
		float_src = -float_src;
	}

	if (float_src != 0) {
		if (float_src >= 1)
			while (((float_src >= 10) || (float_src < 1)) && (exp < 20)) {
				float_src /= 10.0;
				exp++;
			}
		else
			while (((float_src >= 10) || (float_src < 1)) && (exp > -20)) {
				float_src *= 10.0;
				exp--;
			}
	}
	tmp[tmp_i++] = '0' + (int32_t) float_src;
	tmp[tmp_i++] = '.';
	float_src -= (int32_t) float_src;
	float_src *= 10.0;
	do {
		tmp[tmp_i++] = '0' + (int32_t) float_src;
		float_src -= (int32_t) float_src;
		float_src *= 10.0;
	} while ((float_src != 0) && (tmp_i < 20));
	for (i = 0; i < tmp_i; i++) {
		buf[(*buf_i)++] = tmp[i];
	}
	buf[(*buf_i)++] = 'e';
	fInteger2dec(exp, buf, buf_i);
}

/**
 * @brief
 * Convert an integer to a hexadecimal string representation.
 * @param int_src the integer to be converted
 * @param buf
 * @param buf_i the variable parameter having the offset into the string buffer
 */
void fInteger2hex(int32_t int_src, int8_t *buf, uint32_t* buf_i) {
	int32_t tmp_i = 0;
	int8_t tmp[30];
	do {
		tmp[tmp_i++] = GlobHexNum[int_src & 0xF];
		int_src >>= 4;
	} while (int_src != 0);
    
    /* If the hex number is a single digit, pad a '0' in front of it */
    if (tmp_i == 1)
    {
      tmp[tmp_i++] = '0';
    }
    
	tmp_i--;
	while (tmp_i >= 0) {
		buf[(*buf_i)++] = tmp[tmp_i--];
	}
}

/**
* @brief 
* parse a string int printf format
*
* @param fmt  the printf format string,
* @param ...  variable parameter list
*
* @return the null terminated string containing the plain ascii interpretation 
* 			  of the printf string.
*/
int8_t* fPrintParseString(const int8_t *fmt, ...)
{
	va_list a;
	int32_t fmt_i = 0;
	int8_t *str = 0;
	int32_t str_i = 0;

	int32_t int32_t_val;
	float float_val;

	va_start(a, fmt);
	GlobBuff_i = 0;
	fmt_i = 0;
	while (fmt[fmt_i] != 0) {
		if (fmt[fmt_i] != '%') { // copy fmt -> buff
			if (fmt[fmt_i] != '\r') { // skip \r character
				if (fmt[fmt_i] == '\n') { // insert \r character in front of \n character (for MS-DOS addicts!)
					GlobPrintBuff[GlobBuff_i] = '\r';
					GlobBuff_i++;
				}
				GlobPrintBuff[GlobBuff_i] = fmt[fmt_i];
				GlobBuff_i++;
			}
			fmt_i++;
		} else // interprete %
		{
			fmt_i++;

			// discards numbers/format prefix
			// by ex : %.1f => %f   --> float with 1 decimal
			while (((fmt[fmt_i] >= '0') && (fmt[fmt_i] <= '9')) || (fmt[fmt_i] == '.'))
				fmt_i++;

			// decode argument and put it in buff
			switch (fmt[fmt_i]) {
			case 's': // string
				str = va_arg(a,int8_t *);
				str_i = 0;
				while (str[str_i] != 0) {
					GlobPrintBuff[GlobBuff_i++] = str[str_i++];
				}
				;
				break;
			case 'X': // integer in hex form
			case 'x':
				int32_t_val = va_arg(a, int32_t);
				fInteger2hex(int32_t_val, GlobPrintBuff, &GlobBuff_i);
				break;
			case 'd': // integer in decimal form
			case 'u':
			case 'i':
				int32_t_val = va_arg(a, int32_t);
				fInteger2dec(int32_t_val, GlobPrintBuff, &GlobBuff_i);
				break;
			case 'f': // float
				float_val = (float) (va_arg(a,double));
				fFloat2dec(float_val, GlobPrintBuff, &GlobBuff_i);
				break;
			case 'e': // float in scientific form
				float_val = (float) (va_arg(a,double));
				fFloat2sci(float_val, GlobPrintBuff, &GlobBuff_i);
				break;
			default:
				break;
			};
			fmt_i++;
		};
	};
	va_end(a);
	GlobPrintBuff[GlobBuff_i] = 0; // terminate string
    return GlobPrintBuff;
}

/**
* @brief 
* print a printf formatted string into a string.
*
* @param buffer character pointer, destination of the print action
* @param fmt the printf like format string
* @param ... variable parameter list
*/
void fSprintf(char *buffer, const int8_t *fmt, ...)
{
	fPrintParseString(fmt);
	memcpy (buffer, GlobPrintBuff, GlobBuff_i);
}

extern int messageLogFile;

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
void fPrintf(const int8_t *fmt, ...) {
  
        // parse the format string and make it ready for printing
	//fPrintParseString(fmt);
    
        // We can use either a message log or a message box to print our debug msgs
        //FlMessageLog((char*)GlobPrintBuff); 
        //FlMessageBox((char *)GlobPrintBuff);
}

