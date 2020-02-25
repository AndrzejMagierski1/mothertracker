
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

