/**
 ******************************************************************************
 * @file Orion_Flash.c
 * @brief Flash Loader Program For IAR Workbench
 * @details
 * This file contains implementation related to initialization of flash banks and
 * powering of relevant flash banks. Target specific implementations of the 
 * FlashInit(), FlashErase() and FlashWrite() and FlashSignoff() provided by the 
 * framework are given here. 
 * @internal
 * @author Balaji Jalluri, Pradeep Kumar G R
 * $Rev: $
 * $Date: 2013-09-12 17:46:15 +0530 (Thu, 12 Sep 2013) $
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
 *
 * @mainpage 
 * The flashloader to be used with IAR workbench is based on the reference flash programming tools 
 * provided by IAR for ARM cortex based microcontrollers. Some of the files in this flash programming
 * application is based on the framework given by IAR. 
 *
 * <h1>  Reference documents </h1>
 * 
 * <p>
 * The following reference documents provide additional information about IAR flashloader:
 * - [1] IAR Systems, IAR Embedded Workbench flash loader - User Guide
 * - [2] IAR Systems, Flash Loader Development Guide for IAR Embedded Workbench
 * </p>
 * 
 *
 */

#include "types.h" 
#include "flash.h"
#include "flash_loader.h"
#include "dma.h"
#include "fib.h"
#include "Interface.h"
#include "print.h"
#include "flash_loader_extra.h"

/** The binary file that needs to be programmed in flash */
#ifndef FLASHLOADER_TEST
#define BINFILENAME "$TARGET_BPATH$.bin"
#else
#define BINFILENAME "D:/ORION_NEW/trunk/esw/workspaces/orion-revd-mac-zip/Debug/Exe/orion-revd-mac-zip.bin"
#endif

/** The linker file of the aplication placed the variable fib_table that contains the start address
 * of the binary like it was configured in the project compiler flags to be LOAD_ADDRESS, at a fixed
 * location, right after the vector table.  By adding some variables to that section for Silicon 
 * Revision C, this location has shifted from 0x88 to 0x98. For REV D this location has shifted from 0x98 to 0xA4
 */
#define FIB_OFFSET 0xA4   

extern block_driver_t flash_driver;
extern DmaDescr_t GlobDmaDescr;
extern void fDmaDefaultCallBack();
extern int bytes_read_to_process;

int32_t fUpdateFib(void);
int32_t fCalFibChecksum(fib_t *stfibptr);
int32_t fReadFwbaseAddVer(int8_t *binFileName,int32_t *fwverarrptr);
int32_t fCalBinarysize(int8_t *binFileName);
int32_t fCrcCalculate(uint8_t *binFilename, uint32_t len,uint32_t *checksum);
void fInitGobjects(void);

#define ORION_EXE_FILE_MAX_SIZE 320*1024+1

fib_t fib;
uint32_t firmwareAddress;

/** Global flash A device pointer*/
device_pt GlobFlashDeviceA;

/** Global flash B device pointer */
device_pt GlobFlashDeviceB;

/** Global flash A device options */
const flash_options_t GlobFlashOptionsA = {
		(uint32_t*)0x00000000,
		FLASHREG,
		Flash_IRQn

};
/** Global flash B device options */
const flash_options_t GlobFlashOptionsB = {
		(uint32_t*)0x00100000,
		FLASHREG,
		Flash_IRQn

};
/** Character buffer pointer that IAR uses to pass the data blocks that need to be stored in flash */
uint8_t *t1_dptr;
uint8_t numDev;

/**
 * This function performs Flash write operation
 *  
 *  @param    void * block_start -> block starting address 
 *  @param    uint32_t offset_into_block -> offset  
 *  @param    uint32_t count ->  No of Bytes  
 *  @param    char const *buffer -> contents to be written 
 *  @return   Returns RESULT_OK  on success
 *            Returns RESULT_ERROR on Failure  
*/                              
uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
    unsigned char success_flag;                                  
      
    t1_dptr= ((uint8_t *)block_start)+offset_into_block;   

    //fPrintf("FIB base = %0x", firmwareAddress);
    
    /* Write to flash A or Flash B depending on the flash bank in use */
    if ((firmwareAddress & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK) {
      
      t1_dptr =(uint8_t *)((uint32_t)block_start ^ FLASH_B_OFFSET_MASK);
      
      fPrintf("Write 0x%x bytes to flash B, 0x%x\n", count, t1_dptr);
      success_flag=flash_driver.write(GlobFlashDeviceB,(uint8_t **)&t1_dptr,
                                           (uint8_t const *)buffer,count);
    } else {
      
      fPrintf("Write 0x%x bytes to flash A, 0x%x\n", count, t1_dptr);
      success_flag=flash_driver.write(GlobFlashDeviceA,(uint8_t **)&t1_dptr,
                                           (uint8_t const *)buffer,count); 
    }
    
    if(success_flag==True)  
    {
      return RESULT_OK;
    }  
    
    return RESULT_ERROR;
}


/**
 *  This function performs Flash Erase operation
 *  
 *  @param         void * block_start  -> block starting address 
 *  @param         uint32_t block_size -> block size         
 *  @return        Returns RESULT_OK  on success
 *                 Returns RESULT_ERROR on Failure  
 *           
*/                              
uint32_t FlashErase(void *block_start, 
                    uint32_t block_size)
{
      // Write call is doing erase by itself        
      return RESULT_OK;
}
/**
 *  This function performs Flash Init operation.The FlashInit function is the
 *  first function called in the flash loader.
 *
 *  @param          void * base_of_flash  -> Base address of Flash Bank 
 *  @return         Returns RESULT_OK  on success
 *                  Returns RESULT_ERROR on Failure  
            
*/
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags)
{
     boolean retVal = True;
     int32_t successFlag = 0;
     int32_t addrVer[2];
     
     fInitGobjects();
     
     /* Read firmware base address and size */
     successFlag=fReadFwbaseAddVer(BINFILENAME,(int32_t *)addrVer);  
       
     if(successFlag != 0)
     {   
       FlMessageBox("Error Reading  FW Base address and Version Number\n");
       return RESULT_ERROR;
     }  
     firmwareAddress = (uint32_t)addrVer[0];
     
     retVal = flash_driver.driver.create(&GlobFlashDeviceA);
     if (retVal != True) {
        FlMessageBox("Create Flash A driver failed ");
        return(RESULT_ERROR);
     }
     
     retVal = flash_driver.driver.open(GlobFlashDeviceA, (void*) &GlobFlashOptionsA);
     if (retVal != True) {
      FlMessageBox("Open Flash A driver failed");
      return(RESULT_ERROR);
     }

     retVal = flash_driver.driver.create(&GlobFlashDeviceB);
     if (retVal != True) {
       FlMessageBox("Create Flash B driver failed ");
       return(RESULT_ERROR);
     }
     retVal = flash_driver.driver.open(GlobFlashDeviceB, (void*) &GlobFlashOptionsB);
     if (retVal != True) {
      FlMessageBox("Open Flash B driver failed");
      return(RESULT_ERROR);
     }
 /*    retVal = flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_UP, 0);
     if (retVal != True) 
     {
       FlMessageBox("Flash B power up failed");
       return(RESULT_ERROR);
     }
*/
     if ((firmwareAddress & FLASH_B_OFFSET_MASK) == FLASH_B_OFFSET_MASK)
     {
       fPrintf("Binary for flash B, power UP B and DOWN A.\n");
       flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_UP, 0);
       flash_driver.driver.ioctl(GlobFlashDeviceA, FLASH_POWER_DOWN, 0);
     } else {
       fPrintf("Binary for flash A, power UP A and power DOWN B.\n");
       flash_driver.driver.ioctl(GlobFlashDeviceA, FLASH_POWER_UP, 0);
	   flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_DOWN, 0);
     }
     return RESULT_OK;
}

/**
 *  This function initializes function pointers for read and write of flash 
 *  as well as driver related initializtion
 *  @param          void  
 *  @return         void  
*/
void fInitGobjects(void)
{
     flash_driver.driver.create=&fFlashCreate;
     flash_driver.driver.open= &fFlashOpen;
     flash_driver.driver.close=&fFlashClose;
     flash_driver.driver.ioctl= &fFlashIoctl;
     flash_driver.driver.deviceList=Null; 
     flash_driver.read=&fFlashRead;  
     flash_driver.write=&fFlashWrite;

     numDev = 0;
     
     GlobDmaDescr.lock=0; 
     GlobDmaDescr.callback=fDmaDefaultCallBack;
     
}

/**
 *  This function calculates the fib and programs the fib
 *  
 *  @param          void   
 *  @return         Returns RESULT_OK  on success
 *                  Returns RESULT_ERROR on Failure  
*/      
int32_t fUpdateFib(void)
{
   int32_t addrVer[2],success_flag;                      
            
   /* Reading fw base address and version number */
   success_flag=fReadFwbaseAddVer(BINFILENAME,(int32_t *)addrVer);  

   if(success_flag != 0)
   {   
     FlMessageBox("Error Reading  FW Base address and Version Number\n");
     return -1;
   }  
  
   fib.base=addrVer[0];
   fib.rev=addrVer[1];
    
   /*Calculating the binary size*/
   fib.size=fCalBinarysize(BINFILENAME); 
   
   if(fib.size <= 0) // negative number or zero means error
   {
     FlMessageBox("Error Reading Binary Size ....!");
     return -1;
   } 
   
   success_flag=fCrcCalculate(BINFILENAME,fib.size,&fib.crc);  
     
   if(success_flag != 0)
   {
     FlMessageBox("Error Claculating CRC of the Binary ....!");
     return -1; 
   }  
   fPrintf("fCrcCalculate\n");
   fib.checksum=fCalFibChecksum(&fib);      
    
   if(fib.checksum==-1)
   {
     FlMessageBox("Error Calculating Checksum of the FIB....!"); 
     return -1;
   }  
   
   fPrintf("Write FIB to flash A.  Power UP A, DOWN B.\n");
   flash_driver.driver.ioctl(GlobFlashDeviceB, FLASH_POWER_DOWN, 0);
   flash_driver.driver.ioctl(GlobFlashDeviceA, FLASH_POWER_UP, 0);
   
   success_flag=fFibWriteFib(GlobFlashDeviceA, &fib);  
   
   if(success_flag==True)
   {
     /*  FIB write is successful  */                    
     return 0; 
   }else  
   {
     FlMessageBox("Error writing FIB ...");
     return -1;
   }
}

/**
 *  This function calculates checksum of the fib
 *  
 *  @param        address of the fib structure    
 *  @return       Returns checksum of the fib
 *                  
*/   
int32_t fCalFibChecksum(fib_t *stfibptr)
{
   /* calculate fib checksum sum of the all fields */ 
  
  return stfibptr->checksum=stfibptr->base+stfibptr->size+
         stfibptr->crc+stfibptr->rev;  
}

/**
 *  This function reads the firmware base address and version number
 *  
 *  @param          Binary file full path,address of the variable for stroing output         
 *  @return         Returns RESULT_OK  on success
 *                  Returns RESULT_ERROR on Failure   
 *                  
*/ 
int32_t fReadFwbaseAddVer(int8_t *binFileName,int32_t *fwverarrptr)
{
     int32_t fd,count,i,itemp;
     int8_t *bptr;
    
     count=FIB_OFFSET;
     bytes_read_to_process=0;
     
     fd=FlFileOpen((char *)binFileName);
     
     if(fd==-1)
     {
       FlMessageBox("Error opening binary file...!\n");  
       return -1;
     }
     
     /* file seek to the position of the load address stored in the binary */
     for(i=count;i>0;i--)
     {
       itemp=FlFileReadByte(fd);
       
       if(itemp==-1)
       {
         FlFileClose(fd); 
         FlMessageBox("Looks Not a valid binary file...!\n");  
         return -1;
       }
     }
     
     bptr= (int8_t *)fwverarrptr;
     
     /* Read the load address */
     for(count=4;count>0;count--)
     {
       itemp=FlFileReadByte(fd);         
       
       if(itemp==-1)
       { 
          FlFileClose(fd);  
          FlMessageBox("Looks Not a valid binary file...!\n");
          return -1;
       }     
    
       *bptr=(int8_t)itemp; 
       bptr++;
     } 
     
     bptr=(int8_t *)((unsigned int *)fwverarrptr+1); 
     
     for(count=4;count>0;count--)
     {
       itemp=FlFileReadByte(fd);         
       
       if(itemp==-1)
       { 
         FlFileClose(fd);  
         FlMessageBox("Looks Not a valid binary file...!\n");
         return -1;
       }     
      
       *bptr=(int8_t)itemp;        
       bptr++;
     } 
     FlFileClose(fd); 
          
     return 0;
}

/**
 *  This function calculates the Crc of the application Binary
 *  
 *  @param          Binary file full path,
 *  @param          length of the binary file,
 *  @param          adress of the variable for stroing output         
 *  @return         Returns RESULT_OK  on success
 *                  Returns RESULT_ERROR on Failure   
 *                  
*/ 
int32_t fCrcCalculate(uint8_t *binFilename, uint32_t len,uint32_t *checksum)
{
  uint32_t c = 0, n = 0, k = 0;
  int32_t fd,itempch;
  /* Table of CRCs of all 8-bit messages. */
  uint32_t crc_table[256];
  uint8_t ch;  
  
  bytes_read_to_process=0;
  fd=FlFileOpen((char *)binFilename);     
  
  if(fd==-1)
  {
    return -1;
  }
  
  /** Build CRC table in stack */
  for (n = 0; n < 256; n++)
  {
    c = n;
    for (k = 0; k < 8; k++)
    {
      if (c & 1)
      {
        c = 0xedb88320L ^ (c >> 1);
      }
      else
      {
        c = c >> 1;
      }
    }
    crc_table[n] = c;
  }
  
  /** Calculate the CRC */
  c = 0xffffffffL;
  
  for (n = 0; n < len; n++)
  {
    
    itempch=FlFileReadByte(fd);      
    
    if(itempch==-1)
    {
      FlFileClose(fd); 
      fPrintf("ERROR in file while calculating CRC\n");
      return -1;  /*looks EOF reached, some thing wrong need to check*/          
    }  
    
    ch=(uint8_t)itempch;
    
    c = crc_table[(c ^ ch) & 0xff] ^ (c >> 8);   
  }
  
  *checksum =(c ^ 0xffffffffL);
  
  FlFileClose(fd); 
  
  return 0;    
}
#if 0
OPTIONAL_SIGNOFF
uint32_t FlashSignoff()
{       
//#if 1
  int32_t successflag;
  
  fPrintf("Writing FIB");
  successflag=fUpdateFib();
  
  if(successflag==0)  
  {  
    FlMessageLog("FIB Successfully Programmed...\n");
    
  }else{
    FlMessageBox("Error Programming FIB...!");
  }
//#endif
  return RESULT_OK;
}

/**
 *  This function calculates the size of the binary
 *  
 *  @param          Binary file full path,       
 *  @return         Returns Binary Size  on success
 *                  Returns RESULT_ERROR on Failure   
 *                  
*/ 
int32_t fCalBinarysize(int8_t *imgfilename)
{
    int32_t fd,itempch;
    uint32_t size=0;
    
    bytes_read_to_process=0; 
    fd=FlFileOpen((char *)imgfilename);           
  
    if(fd==-1)      
    {
      FlMessageBox("Error opening binary file...!");
      return -1;
    }
    
    while(size<ORION_EXE_FILE_MAX_SIZE)
    { 
      itempch=FlFileReadByte(fd);      
      if(itempch==-1)
      break;
      size++;
    }
    
    if(size>=ORION_EXE_FILE_MAX_SIZE) 
    {
      FlMessageBox("Error,binary file exceeded than the max code memory ...!\n");
      return -1;
    }
     
    FlFileClose(fd);  
    
    return size;  
}
#endif

#ifdef FLASHLOADER_TEST
main()
{
	Fl2FlashInitEntry();
	Fl2FlashSignoffEntry();
	
	uint8_t Buf[50] = {0x1, 0x2, 0x3};
	
	theFlashParams.block_size = 256 * 2048;
	theFlashParams.base_ptr = 0x100000;
	theFlashParams.offset_into_block = 0x0;
	theFlashParams.count = 50;
	theFlashParams.buffer = Buf;
	
	Fl2FlashEraseWriteEntry();
	Fl2FlashWriteEntry();
	
//	Fl2FlashChecksumEntry();
}
#endif /*FLASHLOADER_TEST*/