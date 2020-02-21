
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "mtSequencer.h"

#include "fileTransfer.h"
#include "fileManager.h"

extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];

//SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
//          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

///__NOINIT(EXTERNAL_RAM) strInstrumentFile fileManagerInstrumentBuffer  {0};



void cFileManager::loadSamplesFromWorkspace()
{
	startSampleLoad();

	char sampleToLoad[PATCH_SIZE];
	sprintf(sampleToLoad, cWorkspaceSamplesFilesFormat, processSample);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(sampleToLoad, (uint8_t*)&fileManagerInstrumentBuffer, sizeof(strInstrument), fileDivIntoParts);

	if(loadStatus == fileTransferEnd)
	{
		if(loadInstrumentFormFileStruct(&mtProject.instrument[processInstrument], &fileManagerInstrumentBuffer))
		{
			continueSampleLoad();
		}
		else
		{
			sampleThrowError();
		}
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		//memset(mtProject.instrument[processInstrument].sample.file_name, 0, SAMPLE_NAME_SIZE);
		//mtProject.instrument[processInstrument].isActive = 0;
		continueSampleLoad();
	}
	else if(loadStatus >= fileTransferError)
	{
		instrumentThrowError();
	}
}

void cFileManager::startSampleLoad()
{
	if(sampleLoadPhase != 0) return;
	sampleLoadPhase = 1;

	//to do bebechy z :
	//void SamplesLoader::start(uint8_t startIndex, char * projectPatch, uint8_t firstLoad  )


}

void cFileManager::continueSampleLoad()
{
	processSample++;
	if(processSample >= INSTRUMENTS_COUNT)
	{
		processSample = 0;
		moveToNextOperationStep();
	}
}




void cFileManager::sampleThrowError()
{
	processSample = 0;

	throwError(3);
}


