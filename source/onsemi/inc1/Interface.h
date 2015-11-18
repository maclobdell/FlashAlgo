/**************************************************
 * Copyright 2004-2005 IAR Systems. All rights reserved.
 *
 * $Revision: 21621 $
 **************************************************/

#include "Config.h"

// Defines the function pointer type of the write function that
// must be defined in the flash loader driver.
typedef void (*WriteFunctionType)(unsigned long address, int byte);

//
// Initialize functions.
//

// This function must be defined by the flash loader driver and is
// used by the flash loader framework to initialize the flash loader driver.
// The number of flash arguments are passed in argc.
// The flash arguments are passed in the argv array.
// Flash arguments allow parameters to be passed from the C-SPY
// flash options dialog to the flash loader. A typical example of
// when this can be used, is passing CPU clock speed to the flash
// loader driver.
void FlashDriverInitialize(int argc, char const* argv[]);

// Used by the flash loader driver to register the write function
// with the flash loader framework. The argument write_func is the
// function pointer to the write function. The framework will call this
// function for every byte to be flashed. When the framework calls
// the write function, it will pass the byte and the address of where to
// write the byte as parameters. The sequence of addresses is guaranteed
// to be increasing but not contiguous (gaps may be present).
// The flash loader driver must call this function from FlashDriverInitialize().
void FlRegisterWriteFunction(WriteFunctionType write_func);

// Optional function to get the flash base address set by the user in the IDE.
// Returns 0xffffffff if no flash base address is set by the user.
// For example, AMD compatible flash devices do not rely on any control registers
// for program and erase procedures. Instead, bus write sequences using various address
// offsets from the flash base address are used. In this case it will be necessary
// to know the flash base address to adapt to possible flash remapping.
unsigned long FlGetBaseAddress();


//
// Argument functions.
//

// The function looks for the specified option in the argument array argv.
// The with_value parameter specifies if the function is used to see if an option
// exists in argv or if the value of the option should be returned. The value
// of an option is the next argument after the matching argument in argv.
// Set with_value to 0 when checking for a flag option like --small_ram.
// Set with_value to 1 when checking for an option with value like --speed 14600.
// The argc parameter is the number of arguments in the argv array.
// The argv parameter is an array of string pointers.
// The argc/argv parameters in FlashDriverInitialize() can be used directly
// when calling this function.
// The function returns a null pointer if the option is not found in argv.
// The function returns a pointer to the argument after the matching option if with_value is set to 1.
// The function returns a pointer to the entry in argv that matches the option if with_value is set to 0.
const char* FlFindOption(char* option, int with_value, int argc, char const* argv[]);

// Takes a string with space/tab separated options and makes an argv string array
// with one array element per option. The argv character pointer array must be large
// enough to accommodate all options in the args string.
// The function returns the resulting number of strings of the argv array (number of arguments).
int FlMakeArgs(char* args, char const* argv[]);


//
// C-SPY user interface functions.
//

// C-SPY will display a message box window with the text given by the msg parameter.
// Text may be split on multiple lines by embedding newlines (\n) in the message string.
// The flash loader execution will halt until the message box OK button is pressed.
void FlMessageBox(char* msg);

// C-SPY will display a log message given by msg in the debug log window.
// Text may be split on multiple lines by embedding newlines (\n) in the message string.
void FlMessageLog(char* msg);

// C-SPY will create a progress bar window. The title parameter string will be displayed
// above the progress bar.
void FlProgressBarCreate(char* title);

// C-SPY will close the progress bar window.
void FlProgressBarDestroy();

// C-SPY will update the progress bar to reflect the value of the progress parameter.
// The valid range of progress is 0..100.
// The progress bar must be created for this function to succeed.
// The number of calls to FlProgressBarUpdate() must be kept as low as possible (<= 10),
// this is to reduce the number of (slow) transactions on the JTAG bus.
void FlProgressBarUpdate(int progress);

// Overrides the default progress bar implemented by the flash loader framework.
// In most cases the flash loader driver will not need to handle the progress bar
// as this is handled by the input file read routines in the framework.
// If the flash loader driver implements its own progress bar, it must disable
// the default implementation by calling this function.
void FlOverrideProgressBar();

// Terminates the flash loader and signals C-SPY that the flash download succeeded.
void FlExit();

// Terminates the flash loader and signals C-SPY that the flash download failed.
void FlErrorExit();

// C-SPY will display a message box window with the text given by the msg parameter.
// Text may be split on multiple lines by embedding newlines (\n) in the message string.
// The flash loader execution will halt until the message box OK button is pressed.
// The flash loader will then terminate and notify C-SPY that the flash download failed.
void FlFatalError(char* msg);

//
// File functions.
//

// Opens a file for binary mode reading.
// Returns a file handle if the open succeeded.
// Returns -1 if the file could not be opened.
int FlFileOpen(char* name);

// Reads one byte from the open file associated with file handle fd.
// Returns the byte read, or -1 if end of file is reached.
int FlFileReadByte(int fd);

// Closes the open file associated with file handle fd.
void FlFileClose(int fd);

