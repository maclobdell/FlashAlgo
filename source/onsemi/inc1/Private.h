/**************************************************
 * Copyright 2004-2005 IAR Systems. All rights reserved.
 *
 * $Revision: 26029 $
 **************************************************/


#if TINY_FLASHLOADER == 0

#define ERR_MAIN_NO_WRITE_FUNC "Flasher must call FlSetWriteByteFunction()."
#define ERR_MAIN_OPEN_SIM \
                 "Failed to open file $TARGET_BPATH$.sim"
#define ERR_SIM_EOF "Found unexpected EOF in input file."
#define ERR_SIM_BAD_FORMAT "The input file is not in the simple-code format."
#define ERR_SIM_BAD_RECORD "Unknown record in simple-code input file."
#define ERR_SIM_CHECKSUM "Checksum error in simple-code input file."
#define ERR_IF_MESSAGE_LOG "Failed to open message log."
#define ERR_IF_LONG_PROGRESS_TITLE "Progress bar title is too long (max 240)."
#define ERR_IF_PROGRESS_OPEN_FAIL "Failed to open progress bar."
#define ERR_IF_PROGRESS_NOT_OPEN "Progress bar not open."
#define ERR_IF_SMALL_READ_BUFFER "The read buffer is too small."

#else

#define ERR_MAIN_NO_WRITE_FUNC        "\v51"
#define ERR_MAIN_OPEN_SIM             "\v52"
#define ERR_SIM_EOF                   "\v61"
#define ERR_SIM_BAD_FORMAT            "\v62"
#define ERR_SIM_BAD_RECORD            "\v63"
#define ERR_SIM_CHECKSUM              "\v64"
#define ERR_IF_MESSAGE_LOG            "\v71"
#define ERR_IF_LONG_PROGRESS_TITLE    "\v72"
#define ERR_IF_PROGRESS_OPEN          "\v73"
#define ERR_IF_PROGRESS_NOT_OPEN      "\v74"
#define ERR_IF_SMALL_READ_BUFFER      "\v75"

#endif

extern WriteFunctionType writeByteFunction;  // Write byte function pointer.
extern int overrideProgressBar;              // To disable framework progress bar.
extern unsigned long baseAddress;            // The flash base address to start flashing at
extern int overrideBaseAddress;              // if default given by input file is overridden.

