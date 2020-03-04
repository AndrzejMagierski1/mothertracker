

#include <stdio.h>
#include <string.h>
#include "fsl_sd.h"


#include "SD.h"
#include "ctype.h"


#include "diskio.h"
#include "fsl_sd_disk.h"

#include "debugLog.h"

void enableGPIO(bool enable);
void initSDHC();



SdCard SD;

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FATFS g_fileSystem; /* File system object */

/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */

#define BUFFER_SIZE (100U)

//SDK_ALIGN(uint8_t g_bufferWrite[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
//          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
//SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
//          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
//

/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByHostCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};


const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};


bool SdCard::init()
{
	initSDHC();
	//enableGPIO(true);
	//pinsInit();

//	for(uint32_t i = 0; i< 1000; i++)
//	{
//		__asm__ volatile("nop");
//	}

    /* Save host information. */
    g_sd.host.base           = SDHC;
    g_sd.host.sourceClock_Hz = 180000000;//SD_HOST_CLK_FREQ;
    /* card detect type */
    g_sd.usrParam.cd = &s_sdCardDetect;


//  for(uint8_t i = 0; i<2; i++)
//  {

    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        //PRINTF("\r\nSD host init fail\r\n");
        return false;
    }

    SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
    SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);

	FRESULT result = f_mount(&g_fileSystem, driverNumberBuffer, 1U);
	if (result)
	{
		//if(i > 0)
			return false;
	}

//	else break;
//	}

#if (FF_FS_RPATH >= 2U)
	FRESULT error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        return false;
    }
#endif

    return true;
}

void SdCard::stop()
{
    f_mount(0, driverNumberBuffer, 1);
    memset(&g_fileSystem, 0, sizeof(g_fileSystem));

  // SDMMCHOST_Reset(g_sd.host.base);
  // SDMMCHOST_Deinit(&(g_sd.host));


}


#ifdef DEBUG
static char errorText[100] = "SD report: ";
static char errorNr[9];
#endif

void reportSdError(const char* text, uint16_t value)
{
#ifdef DEBUG
	debugLog.setMaxLineCount(10);
	strncpy(&errorText[11], text, 80);
	if(value > 0)
	{
		sprintf(errorNr, " (%d)", value);
		strcat(errorText, errorNr);
	}
	debugLog.addLine(errorText);

	debugLog.forceRefresh();

#endif
}





void SdCard::pinsInit()
{
    CLOCK_EnableClock(kCLOCK_PortE);

    const port_pin_config_t porte0_pin1_config = {/* Internal pull-up resistor is enabled */
                                                  kPORT_PullUp,
                                                  /* Fast slew rate is configured */
                                                  kPORT_FastSlewRate,
                                                  /* Passive filter is disabled */
                                                  kPORT_PassiveFilterDisable,
                                                  /* Open drain is disabled */
                                                  kPORT_OpenDrainDisable,
                                                  /* Low drive strength is configured */
                                                  kPORT_LowDriveStrength,
                                                  /* Pin is configured as SDHC0_D1 */
                                                  kPORT_MuxAlt4,
                                                  /* Pin Control Register fields [15:0] are not locked */
                                                  kPORT_UnlockRegister};
    /* PORTE0 (pin 1) is configured as SDHC0_D1 */
    PORT_SetPinConfig(PORTE, 0U, &porte0_pin1_config);

    const port_pin_config_t porte1_pin2_config = {/* Internal pull-up resistor is enabled */
                                                  kPORT_PullUp,
                                                  /* Fast slew rate is configured */
                                                  kPORT_FastSlewRate,
                                                  /* Passive filter is disabled */
                                                  kPORT_PassiveFilterDisable,
                                                  /* Open drain is disabled */
                                                  kPORT_OpenDrainDisable,
                                                  /* Low drive strength is configured */
                                                  kPORT_LowDriveStrength,
                                                  /* Pin is configured as SDHC0_D0 */
                                                  kPORT_MuxAlt4,
                                                  /* Pin Control Register fields [15:0] are not locked */
                                                  kPORT_UnlockRegister};
    /* PORTE1 (pin 2) is configured as SDHC0_D0 */
    PORT_SetPinConfig(PORTE, 1U, &porte1_pin2_config);

    const port_pin_config_t porte2_pin3_config = {/* Internal pull-up resistor is enabled */
                                                  kPORT_PullUp,
                                                  /* Fast slew rate is configured */
                                                  kPORT_FastSlewRate,
                                                  /* Passive filter is disabled */
                                                  kPORT_PassiveFilterDisable,
                                                  /* Open drain is disabled */
                                                  kPORT_OpenDrainDisable,
                                                  /* Low drive strength is configured */
                                                  kPORT_LowDriveStrength,
                                                  /* Pin is configured as SDHC0_DCLK */
                                                  kPORT_MuxAlt4,
                                                  /* Pin Control Register fields [15:0] are not locked */
                                                  kPORT_UnlockRegister};
    /* PORTE2 (pin 3) is configured as SDHC0_DCLK */
    PORT_SetPinConfig(PORTE, 2U, &porte2_pin3_config);

    const port_pin_config_t porte3_pin4_config = {/* Internal pull-up resistor is enabled */
                                                  kPORT_PullUp,
                                                  /* Fast slew rate is configured */
                                                  kPORT_FastSlewRate,
                                                  /* Passive filter is disabled */
                                                  kPORT_PassiveFilterDisable,
                                                  /* Open drain is disabled */
                                                  kPORT_OpenDrainDisable,
                                                  /* Low drive strength is configured */
                                                  kPORT_LowDriveStrength,
                                                  /* Pin is configured as SDHC0_CMD */
                                                  kPORT_MuxAlt4,
                                                  /* Pin Control Register fields [15:0] are not locked */
                                                  kPORT_UnlockRegister};
    /* PORTE3 (pin 4) is configured as SDHC0_CMD */
    PORT_SetPinConfig(PORTE, 3U, &porte3_pin4_config);

    const port_pin_config_t porte4_pin7_config = {/* Internal pull-up resistor is enabled */
                                                  kPORT_PullUp,
                                                  /* Fast slew rate is configured */
                                                  kPORT_FastSlewRate,
                                                  /* Passive filter is disabled */
                                                  kPORT_PassiveFilterDisable,
                                                  /* Open drain is disabled */
                                                  kPORT_OpenDrainDisable,
                                                  /* Low drive strength is configured */
                                                  kPORT_LowDriveStrength,
                                                  /* Pin is configured as SDHC0_D3 */
                                                  kPORT_MuxAlt4,
                                                  /* Pin Control Register fields [15:0] are not locked */
                                                  kPORT_UnlockRegister};
    /* PORTE4 (pin 7) is configured as SDHC0_D3 */
    PORT_SetPinConfig(PORTE, 4U, &porte4_pin7_config);

    const port_pin_config_t porte5_pin8_config = {/* Internal pull-up resistor is enabled */
                                                  kPORT_PullUp,
                                                  /* Fast slew rate is configured */
                                                  kPORT_FastSlewRate,
                                                  /* Passive filter is disabled */
                                                  kPORT_PassiveFilterDisable,
                                                  /* Open drain is disabled */
                                                  kPORT_OpenDrainDisable,
                                                  /* Low drive strength is configured */
                                                  kPORT_LowDriveStrength,
                                                  /* Pin is configured as SDHC0_D2 */
                                                  kPORT_MuxAlt4,
                                                  /* Pin Control Register fields [15:0] are not locked */
                                                  kPORT_UnlockRegister};
    /* PORTE5 (pin 8) is configured as SDHC0_D2 */
    PORT_SetPinConfig(PORTE, 5U, &porte5_pin8_config);



}





SdFile SdCard::open(const char* path, uint8_t oflag)
{
	SdFile tmpFile;
	tmpFile.open(path, oflag);
	return tmpFile;
}


bool SdCard::mkdir(uint8_t hidden, const char *path, bool pFlag)
{
	FRESULT error = f_mkdir(path);
    if (error)
    {
        if (error == FR_EXIST)
        {
        	reportSdError("create dir - exist", error);

        }
        else
        {
        	reportSdError("create dir - failed", error);
            return false;
        }
    }

    if(hidden)
    {
    	f_chmod (path,AM_HID,AM_HID);
    }

    return true;
}

bool SdCard::remove(const char* path)
{
	FRESULT error = f_unlink (path);
    if (error)
    {
    	if(error == FR_NO_FILE)
    	{
            return false;
    	}
    	else if (error == FR_LOCKED)
        {
        	reportSdError("remove failed - file locked", error);
            return false;
        }
        else
        {
        	reportSdError("remove failed", error);
            return false;
        }
    }
    return true;
}

bool SdCard::removeDirWithFiles(const char* path)
{
    char buff[256];
    FILINFO fno;
    strcpy(buff, path);

    FRESULT error = delete_node(buff, 255, &fno);
    if(error)
    {
    	reportSdError("remove dir with files failed", error);
        return false;
    }
    return true;
}

FRESULT SdCard::delete_node(
    TCHAR* path,    /* Path name buffer with the sub-directory to delete */
    UINT sz_buff,   /* Size of path name buffer (items) */
    FILINFO* fno    /* Name read buffer */
)
{
    UINT i, j;
    FRESULT fr;
    DIR dir;


    fr = f_opendir(&dir, path); /* Open the directory */
    if (fr != FR_OK) return fr;

    for (i = 0; path[i]; i++) ; /* Get current path length */
    path[i++] = _T('/');

    for (;;) {
        fr = f_readdir(&dir, fno);  /* Get a directory item */
        if (fr != FR_OK || !fno->fname[0]) break;   /* End of directory? */
        j = 0;
        do {    /* Make a path name */
            if (i + j >= sz_buff) { /* Buffer over flow? */
                fr = (FRESULT)100; break;    /* Fails with 100 when buffer overflow */
            }
            path[i + j] = fno->fname[j];
        } while (fno->fname[j++]);
        if (fno->fattrib & AM_DIR) {    /* Item is a directory */
            fr = delete_node(path, sz_buff, fno);
        } else {                        /* Item is a file */
            fr = f_unlink(path);
        }
        if (fr != FR_OK) break;
    }

    path[--i] = 0;  /* Restore the path name */
    f_closedir(&dir);

    if (fr == FR_OK) fr = f_unlink(path);  /* Delete the empty directory */
    return fr;
}

uint32_t  SdCard::clusterCount()
{
	return (g_fileSystem.n_fatent-2);
}

uint16_t  SdCard::sectorsPerCluster()
{
	return g_fileSystem.csize;
}

uint32_t  SdCard::freeClusterCount()
{
    FATFS *fs;
    DWORD fre_clust;//, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 1 */
    FRESULT res = f_getfree("2:", &fre_clust, &fs);
    if (res) return 0;

    /* Get total sectors and free sectors */
    //tot_sect = (fs->n_fatent - 2) * fs->csize ;
	//fre_sect = fre_clust * fs->csize;
	return fre_clust;
}

// filter: 0-all, 1-folders only, 2-supported wav only, 3-ptf // always ignore hidden files // max_used_memory must be higher than 255
uint16_t SdDir::createFilesList(uint8_t start_line, char** list, uint8_t list_length, uint16_t max_used_memory, uint8_t chooseFilter)
{
	for(uint8_t i = 0; i<list_length; i++) // wyczyszczenie uzytej wczesniej pamieci
	{
		if(list[i] != nullptr)
		{
			delete list[i];
			list[i] = nullptr;
		}
	}

	uint8_t n = start_line;

	if(max_used_memory < 255)
	{
		reportSdError("create list - memory max used", max_used_memory);
		return n;
	}

	uint16_t memory_used = 0;
	FILINFO fno;
	FRESULT error;
	SdFile local_file;

	f_readdir(directory, nullptr);

	while (1)
	{
		if (n >= list_length) break;
		if (memory_used > max_used_memory-255) break;

		error = f_readdir(directory, &fno);
		if(error)
		{
			reportSdError("create list - read dir item - failed", error);
			break;
		}
		else if(!fno.fname[0]) // koniec folderu
		{
			break;
		}

		if (fno.fattrib & AM_HID) // ukryty
		{
			continue;
		}


		uint8_t filenameLen = strlen(fno.fname);
		switch (chooseFilter)
		{
		case enListFileFilter_all:
			// do nothing
			break;

		case enListFileFilter_folder:
			if ((fno.fattrib & AM_DIR) == 0) continue;
			break;

		case enListFileFilter_wave:
			if (filenameLen < 5) continue;

			if (((fno.fname[filenameLen - 1] != 'V') && (fno.fname[filenameLen - 1] != 'v'))
					|| ((fno.fname[filenameLen - 2] != 'A') && (fno.fname[filenameLen - 2] != 'a'))
					|| ((fno.fname[filenameLen - 3] != 'W') && (fno.fname[filenameLen - 3] != 'w'))
					|| (fno.fname[filenameLen - 4] != '.')) continue;

			if ((strlen(dir_path) + filenameLen) > 253) continue;

			/*

			 char wav_file[255];
			 strcpy(wav_file, dir_path);
			 strcat(wav_file, "/");
			 strcat(wav_file, fno.fname);

			 if(local_file.open(wav_file))
			 {
			 strWavFileHeader localHeader;
			 readHeader(&localHeader, &local_file);
			 local_file.close();

			 if ((localHeader.sampleRate != 44100)
			 || ((localHeader.AudioFormat != 1) && (localHeader.AudioFormat != 3))
			 || ((localHeader.bitsPerSample != 16) && (localHeader.bitsPerSample != 24) && (localHeader.bitsPerSample != 32)))
			 continue;
			 }
			 else continue;
			 */
		case enListFileFilter_firmware:
			if (filenameLen < 5) continue;

			if (fno.fname[filenameLen - 1] == 'f' && fno.fname[filenameLen - 2] == 't' && fno.fname[filenameLen - 3] == 'p' && fno.fname[filenameLen - 4] == '.')
			{
				if (fno.fname[0] == 'P' && fno.fname[1] == 'o' && fno.fname[2] == 'l' && fno.fname[3] == 'y' && fno.fname[4] == 'e' && fno.fname[5] == 'n' && fno.fname[6] == 'd')
				{

				}
				else
					continue;
			}
			else if (fno.fname[filenameLen - 1] == 'x' && fno.fname[filenameLen - 2] == 'e' && fno.fname[filenameLen - 3] == 'h' && fno.fname[filenameLen - 4] == '.')
			{
				if (fno.fname[0] == 'm' && fno.fname[1] == 't')
				{

				}
				else
					continue;
			}
			else
				continue;

		case enListFileFilter_mod:
			char lowCase[255];
			strcpy(lowCase, fno.fname);
			for (uint16_t a = 0; a < 255; a++)
			{
				lowCase[a] = tolower(lowCase[a]);
				if (lowCase[a] == 0) break;
			}

			if (strstr(lowCase, ".mod\0") == NULL) continue;
			break;
		}

		uint16_t len = strlen(fno.fname);

		if (fno.fattrib & AM_DIR)	len+=2;
		else 						len+=1;

		list[n] = new char[len];

		if (fno.fattrib & AM_DIR) // folder
		{
			strcpy(list[n], "/");
			strcat(list[n], fno.fname);
		}
		else
		{
			strcpy(list[n], fno.fname);
		}

		memory_used += len;

        n++;
	}


	return n;
}



uint16_t SdDir::createProjectsList(char** list, uint8_t list_length, uint16_t max_used_memory)
{
	for(uint8_t i = 0; i<list_length; i++) // wyczyszczenie uzytej wczesniej pamieci
	{
		if(list[i] != nullptr)
		{
			delete list[i];
			list[i] = nullptr;
		}
	}

	uint8_t n = 0;

	if(max_used_memory < 255)
	{
		reportSdError("create list - memory max used", max_used_memory);
		return n;
	}

	uint16_t memory_used = 0;
	FILINFO fno;
	FRESULT error;
	SdFile local_file;

	f_readdir(directory, nullptr);

	while (1)
	{
		if (n >= list_length) break;
		if (memory_used > max_used_memory-255) break;

		error = f_readdir(directory, &fno);
		if(error)
		{
			reportSdError("create list - read dir item - failed", error);
			break;
		}
		else if(!fno.fname[0]) // koniec folderu
		{
			break;
		}

		if (fno.fattrib & AM_HID) // ukryty
		{
			continue;
		}


        if((fno.fattrib & AM_DIR) == 0) continue; // nie folder
        else // folder
        {
        	char filePath[256];
			strcpy(filePath, dir_path);
			strcat(filePath, "/");
			strcat(filePath, fno.fname);
			strcat(filePath,"/project.mt");

			if(!SD.exists(filePath))	//tylko jesli w folderze jest plik projektu
			{
				continue;
			}
        }

		uint16_t len = strlen(fno.fname)+1;

		if(len > 32) continue; // max dlugosc nazwy projektu

		list[n] = new char[len];
		strcpy(list[n], fno.fname);
		memory_used += len;

        n++;
	}


	return n;
}

