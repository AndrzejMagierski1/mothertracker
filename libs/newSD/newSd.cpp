
#include <stdio.h>
#include <string.h>
#include "fsl_sd.h"
//#include "fsl_debug_console.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
//#include "board.h"

#include "debugLog.h"

#include "newSd.h"


//#include "fsl_sysmpu.h"
//#include "pin_mux.h"
//#include "clock_config.h"


// buffer size (in byte) for read/write operations
#define BUFFER_SIZE (100U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief wait card insert function.
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
SDK_ALIGN(uint8_t g_bufferWrite[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};


static status_t sdcardWaitCardInsert()
{
    /* Save host information. */
    g_sd.host.base           = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    /* card detect type */
    g_sd.usrParam.cd = &s_sdCardDetect;

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        //PRINTF("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }
    /* power off card */
    SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);


    /* wait card insert */



    SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);


    return kStatus_Success;
}


int8_t sdCardInit()
{
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

	if (sdcardWaitCardInsert() != kStatus_Success)
	{

		return -1;
	}

	if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
	{

		return -1;
	}


    FRESULT error;

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        //PRINTF("Change drive failed.\r\n");
        return -1;
    }
#endif



    error = f_mkdir(_T("/dir_1"));
    if (error)
    {
        if (error == FR_EXIST)
        {
          //  PRINTF("Directory exists.\r\n");
        }
        else
        {
          //  PRINTF("Make directory failed.\r\n");
            return -1;
        }
    }

    error = f_open(&g_fileObject, _T("/dir_1/f_1.dat"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    if (error)
    {
        if (error == FR_EXIST)
        {
           // PRINTF("File exists.\r\n");
        }
        else
        {
           // PRINTF("Open file failed.\r\n");
            return -1;
        }
    }



    if (f_close(&g_fileObject))
    {
      //  PRINTF("\r\nClose file failed.\r\n");
        return -1;
    }




	return kStatus_Success;
}







