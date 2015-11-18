/**
 ******************************************************************************
 * @file Interface.c
 * @brief File operations and message log functions
 * @detail Functions in this file are given by the IAR flashloader framework.
 * Functions in this file handle file operations and logging,display of messages
 * while flashloader is executed. 
 * @internal
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
 * @ingroup orion_flash
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yfuns.h>

#include "Config.h"
#include "Interface.h"
#include "Private.h"

WriteFunctionType writeByteFunction; // Write byte function pointer.
int overrideProgressBar = 0;         // To disable framework progress bar.

int messageLogFile = -1;
#if TINY_FLASHLOADER == 0
static int progressBarFile = -1;
#endif




void FlRegisterWriteFunction(void (*write_func)(unsigned long address, int byte))
{
  writeByteFunction = write_func;
}


const char* FlFindOption(char* option, int with_value, int argc, char const* argv[])
{
  int i;

  for (i = 0; i < argc; i++)
  {
    if (strcmp(option, argv[i]) == 0)
    {
      if (with_value)
      {
        if (i + 1 < argc)
          return argv[i + 1]; // The next argument is the value.
        else
          return 0; // The option was found but there is no value to return.
      }
      else
      {
        return argv[i]; // Return the flag argument itself just to get a non-zero pointer.
      }
    }
  }
  return 0;
}


int FlMakeArgs(char* args, char const* argv[])
{
  int i = 0;
  char* argp;

  // Split the argument string into individual arguments.
  // The arguments are separated by spaces and/or tabs.
  while (argp = strtok(args, " \t"))
  {
    argv[i++] = argp;
    args = 0; // For subsequent strtok calls.
  }
  return i;
}


void FlMessageBox(char* msg)
{
  int log_fd;

  log_fd = __open("$MESSAGE_BOX$", _LLIO_CREAT | _LLIO_TRUNC | _LLIO_WRONLY);
  if (log_fd == -1)
  {
    FlErrorExit(); // Something went really wrong, give up.
  }

  __write(log_fd, (unsigned char *)msg, strlen(msg));
  __close(log_fd);
}


void FlMessageLog(char* msg)
{
  if (messageLogFile == -1)
  {
    messageLogFile = __open("$DEBUG_LOG$", _LLIO_CREAT | _LLIO_TRUNC | _LLIO_WRONLY);
    if (messageLogFile == -1)
    {
      FlFatalError(ERR_IF_MESSAGE_LOG);
    }
  }

  __write(messageLogFile, (unsigned char *)msg, strlen(msg));
}


#if TINY_FLASHLOADER == 0
void FlProgressBarCreate(char* title)
{
  char name[40];

  if (strlen(title) > 24)
  {
    FlFatalError(ERR_IF_LONG_PROGRESS_TITLE);
  }

  strcpy(name, "$PROGRESS_BAR$");
  strcat(name, title);
  if (progressBarFile == -1)
  {
    progressBarFile = __open(name, _LLIO_CREAT | _LLIO_TRUNC | _LLIO_WRONLY);
    if (progressBarFile == -1)
    {
      FlFatalError(ERR_IF_PROGRESS_OPEN_FAIL);
    }
  }
}


void FlProgressBarDestroy()
{
  if (progressBarFile != -1)
  {
    __close(progressBarFile);
    progressBarFile = -1;
  }
}


void FlProgressBarUpdate(int progress)
{
  unsigned char value = progress;

  if (progressBarFile == -1)
  {
    FlFatalError(ERR_IF_PROGRESS_NOT_OPEN);
  }

  __write(progressBarFile, &value, 1);
}


void FlOverrideProgressBar()
{
  overrideProgressBar = 1;
}
#endif


static void CloseHandles()
{
#if TINY_FLASHLOADER == 0
  // Close the progress bar if active.
  FlProgressBarDestroy();
#endif

  if (messageLogFile != -1)
  {
    __close(messageLogFile);
  }
}


void FlExit()
{
  CloseHandles();

  exit(0);
}


void FlErrorExit()
{
  CloseHandles();

  exit(1);
}


void FlFatalError(char* msg)
{
    FlMessageBox(msg);
    FlErrorExit();
}

int FlFileOpen(char* name)
{
  return __open(name, _LLIO_RDONLY | _LLIO_BINARY);;
}


void FlFileClose(int fd)
{
  __close(fd);
}

int bytes_read_to_process = 0;
int FlFileReadByte(int fd)
{
  int c;
  static unsigned char* bufp;
  static unsigned char readBuffer[512];

  if (bytes_read_to_process == 0)
  {
    bytes_read_to_process = __read(fd, readBuffer, 512);
    
    if (bytes_read_to_process == 0)
    {
      return -1;
    }
    bufp = readBuffer;
  }
  c = (int)(*bufp++);
  --bytes_read_to_process;

  return c;
}


