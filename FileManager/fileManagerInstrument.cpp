
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



//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     LOAD     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void cFileManager::loadInstrumentsFromWorkspace()
{
	char instrumentToLoad[PATCH_SIZE];
	sprintf(instrumentToLoad, cWorkspaceInstrumentFileFormat, currentInstrument+1); // numery plikow od 1

	uint8_t loadStatus = fileTransfer.loadFileToMemory(instrumentToLoad, (uint8_t*)&fileManagerInstrumentBuffer, sizeof(strInstrumentFile), fileWholeOnce); //fileDivIntoParts

	if(loadStatus == fileTransferEnd)
	{
		mtProject.instruments_count++;
		if(loadInstrumentFormFileStruct(&mtProject.instrument[currentInstrument], &fileManagerInstrumentBuffer))
		{
			continueInstrumentProcess();
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
		continueInstrumentProcess();
	}
	else if(loadStatus >= fileTransferError)
	{
		instrumentThrowError();
	}
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     COPY     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::copyInstruments()
{
	if(currentOperation == fmCopyWorkspaceToProjects)
	{
		sprintf(currentCopySrcPath, cWorkspaceInstrumentFileFormat, currentInstrument+1);
		sprintf(currentCopyDestPath, cProjectsInstrumentFileFormat, currentProjectName, currentInstrument+1);
	}
	else
	{
		sprintf(currentCopySrcPath, cProjectsInstrumentFileFormat, currentProjectName, currentInstrument+1);
		sprintf(currentCopyDestPath, cWorkspaceInstrumentFileFormat, currentInstrument+1);
	}

	uint8_t loadStatus = fileTransfer.copyFile(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		mtProject.instruments_count++;
		continueInstrumentProcess();
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		continueInstrumentProcess();
	}
	else if(loadStatus >= fileTransferError)
	{
		instrumentThrowError();
	}
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     SAVE     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::saveInstrumentsToWorkspace()
{
	//znajduje pirwszy pattern z flaga zmian i jednoczesnie aktywny
	while(changesFlags.instrument[currentInstrument] == 0 || mtProject.instrument[currentInstrument].isActive == 0)
	{
		if(!continueInstrumentProcess()) return; // jesli sprawdzilo wszystkie to koczny
	}

	if(!writeInstrumentToFileStruct(&mtProject.instrument[currentInstrument], &fileManagerInstrumentBuffer))
	{
		throwError(0);
	}

	char instrumentToSave[PATCH_SIZE];
	sprintf(instrumentToSave, cWorkspaceInstrumentFileFormat, currentInstrument+1); // nazwy instrumentow od numeru 1

	uint8_t saveStatus = fileTransfer.saveMemoryToFile((uint8_t*)&fileManagerInstrumentBuffer, instrumentToSave, sizeof(strInstrumentFile));

	if(saveStatus == fileTransferEnd)
	{
		continueInstrumentProcess();
	}
	else// if(saveStatus >= fileTransferError)
	{
		throwError(1);
	}
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     CREATE     ---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::createEmptyInstrumentInWorkspace(uint8_t slot, char* name)
{
	if(mtProject.instrument[slot].isActive == 0)
	{
		// jesli nie aktywny to tworzy domyslna strukture
		setDefaultActiveInstrument(&mtProject.instrument[slot]);
	}

	//nadaje nazwe i zapisuje z nowa nazwa zawsze
	strcpy(mtProject.instrument[slot].sample.file_name, name);


	if(!writeInstrumentToFileStruct(&mtProject.instrument[slot], &fileManagerInstrumentBuffer))
	{
		throwError(0);
	}

	char instrumentToSave[PATCH_SIZE];
	sprintf(instrumentToSave, cWorkspaceInstrumentFileFormat, slot+1); // nazwy instrumentow od numeru 1

	uint8_t saveStatus = fileTransfer.saveMemoryToFile((uint8_t*)&fileManagerInstrumentBuffer, instrumentToSave, sizeof(strInstrumentFile));

	if(saveStatus == fileTransferEnd)
	{
		moveToNextOperationStep();
	}
	else// if(saveStatus >= fileTransferError)
	{
		throwError(1);
	}

}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

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


bool cFileManager::writeInstrumentToFileStruct(strInstrument* instrument, strInstrumentFile* instrumentFile)
{
	FastCRC32 crcCalc;

	memcpy(&instrumentFile->instrumentDataAndHeader.instrument, instrument, sizeof(strInstrument));

	instrumentFile->instrumentDataAndHeader.instrHeader.id_file[0]='T';
	instrumentFile->instrumentDataAndHeader.instrHeader.id_file[1]='I';
	instrumentFile->instrumentDataAndHeader.instrHeader.type = fileTypeInstrument;
	instrumentFile->instrumentDataAndHeader.instrHeader.fwVersion[0] = FV_VER_1;
	instrumentFile->instrumentDataAndHeader.instrHeader.fwVersion[1] = FV_VER_2;
	instrumentFile->instrumentDataAndHeader.instrHeader.fwVersion[2] = FV_VER_3;
	instrumentFile->instrumentDataAndHeader.instrHeader.fwVersion[3] = FV_VER_1;
	instrumentFile->instrumentDataAndHeader.instrHeader.fileStructureVersion[0] = INSTRUMENT_FILE_VERSION;
	instrumentFile->instrumentDataAndHeader.instrHeader.fileStructureVersion[1] = INSTRUMENT_FILE_VERSION;
	instrumentFile->instrumentDataAndHeader.instrHeader.fileStructureVersion[2] = INSTRUMENT_FILE_VERSION;
	instrumentFile->instrumentDataAndHeader.instrHeader.fileStructureVersion[3] = INSTRUMENT_FILE_VERSION;
	instrumentFile->instrumentDataAndHeader.instrHeader.size = sizeof(strInstrument);

	instrumentFile->crc = crcCalc.crc32((uint8_t *)&instrumentFile->instrumentDataAndHeader,
							sizeof(instrumentFile->instrumentDataAndHeader));

	return true;
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool cFileManager::continueInstrumentProcess()
{
	currentInstrument++;
	if(currentInstrument >= INSTRUMENTS_COUNT)
	{
		currentInstrument = 0;
		moveToNextOperationStep();
		return false;
	}
	return true;
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


void cFileManager::getEmptyInstrument(struct strInstrument* source)
{
	memset((uint8_t*)source, 0, sizeof(strInstrument));

	source->sample.wavetable_window_size = 2048;
}


void cFileManager::setDefaultActiveInstrument(struct strInstrument* targetInstrument)
{
	memset((uint8_t*)targetInstrument, 0, sizeof(strInstrument));

	targetInstrument->isActive = 1;

	targetInstrument->startPoint=0;
	targetInstrument->loopPoint1=1;
	targetInstrument->loopPoint2=MAX_16BIT-1;
	targetInstrument->endPoint=MAX_16BIT;

	targetInstrument->wavetableCurrentWindow = 0;
	targetInstrument->sample.wavetable_window_size = 2048;
	targetInstrument->playMode = 0;

	targetInstrument->envelope[envAmp].delay = 0;
	targetInstrument->envelope[envAmp].attack = 0;
	targetInstrument->envelope[envAmp].hold = 0;
	targetInstrument->envelope[envAmp].decay = 0;
	targetInstrument->envelope[envAmp].sustain = 1.0;
	targetInstrument->envelope[envAmp].release = 1000;
	targetInstrument->envelope[envAmp].amount = 1.0;
	targetInstrument->envelope[envAmp].loop = 0;
	targetInstrument->envelope[envAmp].enable = 1;

	targetInstrument->envelope[envFilter].delay = 0;
	targetInstrument->envelope[envFilter].attack = 3000;
	targetInstrument->envelope[envFilter].hold = 0;
	targetInstrument->envelope[envFilter].decay = 0;
	targetInstrument->envelope[envFilter].sustain = 1.0;
	targetInstrument->envelope[envFilter].release = 1000;
	targetInstrument->envelope[envFilter].amount = 1.0;
	targetInstrument->envelope[envFilter].loop = 0;
	targetInstrument->envelope[envFilter].enable = 0;

	targetInstrument->envelope[envWtPos].delay = 0;
	targetInstrument->envelope[envWtPos].attack = 3000;
	targetInstrument->envelope[envWtPos].hold = 0;
	targetInstrument->envelope[envWtPos].decay = 0;
	targetInstrument->envelope[envWtPos].sustain = 1.0;
	targetInstrument->envelope[envWtPos].release = 1000;
	targetInstrument->envelope[envWtPos].amount = 1.0;
	targetInstrument->envelope[envWtPos].loop = 0;
	targetInstrument->envelope[envWtPos].enable = 0;

	targetInstrument->envelope[envGranPos].delay = 0;
	targetInstrument->envelope[envGranPos].attack = 3000;
	targetInstrument->envelope[envGranPos].hold = 0;
	targetInstrument->envelope[envGranPos].decay = 0;
	targetInstrument->envelope[envGranPos].sustain = 1.0;
	targetInstrument->envelope[envGranPos].release = 1000;
	targetInstrument->envelope[envGranPos].amount = 1.0;
	targetInstrument->envelope[envGranPos].loop = 0;
	targetInstrument->envelope[envGranPos].enable = 0;

	targetInstrument->envelope[envPan].delay = 0;
	targetInstrument->envelope[envPan].attack = 3000;
	targetInstrument->envelope[envPan].hold = 0;
	targetInstrument->envelope[envPan].decay = 0;
	targetInstrument->envelope[envPan].sustain = 1.0;
	targetInstrument->envelope[envPan].release = 1000;
	targetInstrument->envelope[envPan].amount = 1.0;
	targetInstrument->envelope[envPan].loop = 0;
	targetInstrument->envelope[envPan].enable = 0;

	targetInstrument->cutOff = 1.0;
	targetInstrument->filterEnable = 0;
	targetInstrument->filterType = lowPass;
	targetInstrument->resonance = 0;
	targetInstrument->panning = 50;
	targetInstrument->glide = 0;
	targetInstrument->volume = 50;
	targetInstrument->tune = 0;
	targetInstrument->fineTune = 0;

	targetInstrument->reverbSend = 0;

	targetInstrument->granular.grainLength = 441;


}