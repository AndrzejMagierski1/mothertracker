
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "mtSequencer.h"

#include "fileTransfer.h"
#include "fileManager.h"


//SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
//          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

__NOINIT(EXTERNAL_RAM) strInstrumentFile fileManagerInstrumentBuffer  {0};



void cFileManager::loadInstrumentsFromWorkspace()
{
	char instrumentToLoad[PATCH_SIZE];
	sprintf(instrumentToLoad, cWorkspaceInstrumentFileFormat, currentInstrument);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(instrumentToLoad, (uint8_t*)&fileManagerInstrumentBuffer, sizeof(strInstrumentFile), fileDivIntoParts);

	if(loadStatus == fileTransferEnd)
	{
		mtProject.instruments_count++;
		if(loadInstrumentFormFileStruct(&mtProject.instrument[currentInstrument], &fileManagerInstrumentBuffer))
		{
			continueInstrumentLoad();
		}
		else
		{
			instrumentThrowError();
		}
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		memset(mtProject.instrument[currentInstrument].sample.file_name, 0, SAMPLE_NAME_SIZE);
		mtProject.instrument[currentInstrument].isActive = 0;
		continueInstrumentLoad();
	}
	else if(loadStatus >= fileTransferError)
	{
		instrumentThrowError();
	}
}


void cFileManager::continueInstrumentLoad()
{
	currentInstrument++;
	if(currentInstrument >= INSTRUMENTS_COUNT)
	{
		currentInstrument = 0;
		moveToNextOperationStep();
	}
}


bool cFileManager::loadInstrumentFormFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile)
{
	if(((strInstrumentFile*)instrumentFile)->instrumentDataAndHeader.instrHeader.type != fileTypeInstrument) return false;

	FastCRC32 crcCalc;
	uint32_t checkCRC = crcCalc.crc32((uint8_t*)&instrumentFile->instrumentDataAndHeader,
									  sizeof(strInstrumentFile::strInstrumentDataAndHeader));

	if(checkCRC == ((strInstrumentFile*)instrumentFile)->crc)
	{
		memcpy(instrument, &instrumentFile->instrumentDataAndHeader.instrument, sizeof(strInstrument));

		return true;
	}


	return false;
}


void cFileManager::instrumentThrowError()
{
	currentInstrument = 0;

	throwError(0);
}


uint32_t cFileManager::calcWorkspaceInstrumentsSize()
{
	char currentPatch[PATCH_SIZE];
	uint32_t size = 0;

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(currentPatch, cWorkspaceInstrumentFileFormat, i);
		if(SD.exists(currentPatch)) size += sizeof(strInstrumentFile);
	}

	return size;
}

