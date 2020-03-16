
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

__NOINIT(EXTERNAL_RAM)  strInstrumentFile fileManagerInstrumentBuffer  {0};



//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     LOAD     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::loadInstrumentsFromWorkspace()
{
	if(currentInstrument >= INSTRUMENTS_COUNT) { skipNextOperationStep(); return; }//zabiezpeiczenie

	char instrumentToLoad[PATCH_SIZE];
	sprintf(instrumentToLoad, cWorkspaceInstrumentFileFormat, currentInstrument+1); // numery plikow od 1

	uint8_t loadStatus = fileTransfer.loadFileToMemory(instrumentToLoad, (uint8_t*)&fileManagerInstrumentBuffer, sizeof(strInstrumentFile), fileWholeOnce); //fileDivIntoParts

	if(loadStatus == fileTransferEnd)
	{
		mtProject.instruments_count++;
		if(loadInstrumentFormFileStruct(&mtProject.instrument[currentInstrument], &fileManagerInstrumentBuffer))
		{
			mtProject.instrument[currentInstrument].isActive = 1;
			moveToNextOperationStep();
		}
		else
		{
			memset(mtProject.instrument[currentInstrument].sample.file_name, 0, SAMPLE_NAME_SIZE);
			mtProject.instrument[currentInstrument].isActive = 0;
			skipNextOperationStep(); //xxx pomija nastepny krok czyli ladowanie sampla <uważać>
		}
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		memset(mtProject.instrument[currentInstrument].sample.file_name, 0, SAMPLE_NAME_SIZE);
		mtProject.instrument[currentInstrument].isActive = 0;
		skipNextOperationStep(); //xxx pomija nastepny krok czyli ladowanie sampla <uważać>
	}
	else if(loadStatus >= fileTransferError)
	{
		memset(mtProject.instrument[currentInstrument].sample.file_name, 0, SAMPLE_NAME_SIZE);
		mtProject.instrument[currentInstrument].isActive = 0;
		skipNextOperationStep();
		//uszkodzony plik wav - pomin uszkodzony instrument ale nie wyrzucaj bledu
		//instrumentThrowError();
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
	else if(currentOperation == fmCopyProjectsToWorkspace)
	{
		sprintf(currentCopySrcPath, cProjectsInstrumentFileFormat, currentProjectName, currentInstrument+1);
		sprintf(currentCopyDestPath, cWorkspaceInstrumentFileFormat, currentInstrument+1);
	}
//	else //import
//	{
//		sprintf(currentCopySrcPath, "%s/%s", explorerCurrentPath, explorerList[importCurrentFile]); // nazwa pliku od 1
//		sprintf(currentCopyDestPath, cWorkspaceInstrumentFileFormat, currentInstrument+1);
//	}

	uint8_t loadStatus = fileTransfer.copyFile(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		mtProject.instruments_count++;
		//continueInstrumentProcess();
		moveToNextOperationStep();
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		//continueInstrumentProcess();
		moveToNextOperationStep();
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
	if(changesFlags.instrument[currentInstrument] == 0 || mtProject.instrument[currentInstrument].isActive == 0) // jesli flaga zmian nie ustawiona omin zapis
	{
		skipNextOperationStep();
		return;
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
		clearInstrumentChanged(currentInstrument);
		skipNextOperationStep();   // ominac sample bo nie zapisuje sie sampli
	}
	else// if(saveStatus >= fileTransferError)
	{
		throwError(1);
	}
}

// na potrzeby innych modułów
uint8_t cFileManager::saveInstrument(uint8_t idx)
{
	if (!writeInstrumentToFileStruct(&mtProject.instrument[idx],
										&fileManagerInstrumentBuffer))
	{
		return 0;
	}

	char instrumentToSave[PATCH_SIZE];
	sprintf(instrumentToSave,
			cWorkspaceInstrumentFileFormat,
			idx + 1); // nazwy instrumentow od numeru 1

	uint8_t saveStatus = fileTransfer.saveMemoryToFile(
			(uint8_t*) &fileManagerInstrumentBuffer,
			instrumentToSave,
			sizeof(strInstrumentFile));

	if (saveStatus == fileTransferEnd)
	{
		return 1;
	}
	else   // if(saveStatus >= fileTransferError)
	{
		return 0;
	}

	return 0;
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     CREATE     ---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::createEmptyInstrumentInWorkspace()
{
	if(mtProject.instrument[currentInstrument].isActive == 0)
	{
		// jesli nie aktywny to tworzy domyslna strukture
		// jezeli jest aktywny to tylko zmienia nazwe sampla w instrumecie
		setDefaultActiveInstrument(&mtProject.instrument[currentInstrument]);
	}

	if(currentOperation == fmImportSamplesToWorkspace)
	{
		//nadaje nazwe i zapisuje z nowa nazwa zawsze
		strncpy(mtProject.instrument[currentInstrument].sample.file_name, explorerList[importCurrentFile], SAMPLE_NAME_SIZE);
	}
	else if(currentOperation == fmCopyInstrumentsInWorkspace)
	{
		//kopiuje cala strukturę z instrumentu zrodlowego <= wyjatek od pozostalych przypadkow
		memcpy(&mtProject.instrument[currentInstrument], &mtProject.instrument[copySrcSlot], sizeof(strInstrument));
		//strncpy(mtProject.instrument[currentInstrument].sample.file_name, mtProject.instrument[copySrcSlot].sample.file_name, SAMPLE_NAME_SIZE);
	}
	else //(currentOperation == fmSaveRecordedSound)
	{
		strncpy(mtProject.instrument[currentInstrument].sample.file_name, getRecordingFileName(), SAMPLE_NAME_SIZE);
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
		moveToNextOperationStep();
	}
	else// if(saveStatus >= fileTransferError)
	{
		throwError(1);
	}

}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     DELETE     ---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::deleteInstrumentsFromWorkspace()
{
	for(uint8_t instr = currentInstrument; instr <= deleteEndInstrument; instr++)
	{
		char file_path[WORKSPACE_INSTRUMENTS_FORMAT_MAX_LENGTH];
		sprintf(file_path, cWorkspaceInstrumentFileFormat, instr+1);

		if(SD.exists(file_path))
		{
			SD.remove(file_path);
		}

		// clear project struct - nie tu bo potrzebne sa te dane w dalszej czesci procesu np. do przesówania pamięci
		//setDefaultActiveInstrument(&mtProject.instrument[instr]);
		//mtProject.instrument[instr].sample.file_name[0] = 0;
		//mtProject.instrument[instr].isActive = 0;
	}
	moveToNextOperationStep();
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


void cFileManager::calcFirstSlotToMoveInMemory(uint8_t calcStartSlot)
{
	// oblicz od ktorego instrumentu trzeba bedzie przesunac pamiec
	// potem zostanie obliczony offset o jaki trzeba bedzie przesunąć
	// jesli 48 to znaczy ze nie ma sampli do przesuniecia
	firstSlotToMoveInMemory = calcStartSlot;

	while(firstSlotToMoveInMemory < INSTRUMENTS_COUNT && mtProject.instrument[firstSlotToMoveInMemory].isActive == 0)
	{
		firstSlotToMoveInMemory++;

		if(firstSlotToMoveInMemory >= INSTRUMENTS_COUNT)
		{
			break;
		}
	}
}




//
//void cFileManager::findLastActiveInstrumentBeforeCurrent()
//{
//	// znajduje ostanti aktywny istrument poprzedzajacy currentInstrument
//	// jezli nie znadzie ustawia: lastActiveInstrument = 0
//	if(currentInstrument > 0)  lastActiveInstrument = currentInstrument-1;
//	else { lastActiveInstrument = 0; return; }
//
//	while(mtProject.instrument[lastActiveInstrument].isActive == 0)
//	{
//		lastActiveInstrument--; // jesli sprawdzilo wszystkie to koczny
//		if(lastActiveInstrument == 0) break;
//	}
//}


void cFileManager::setCurrentInstrumentToFirstActiveAfterCurrent()
{
	//znajduje pierwszy instrument z flaga zmian i jednoczesnie aktywny
	while(changesFlags.instrument[currentInstrument] == 0 || mtProject.instrument[currentInstrument].isActive == 0)
	{
		currentInstrument++; // jesli sprawdzilo wszystkie to koczny
		if(currentInstrument >= INSTRUMENTS_COUNT) break;
	}
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

	targetInstrument->envelope[envCutoff].delay = 0;
	targetInstrument->envelope[envCutoff].attack = 3000;
	targetInstrument->envelope[envCutoff].hold = 0;
	targetInstrument->envelope[envCutoff].decay = 0;
	targetInstrument->envelope[envCutoff].sustain = 1.0;
	targetInstrument->envelope[envCutoff].release = 1000;
	targetInstrument->envelope[envCutoff].amount = 1.0;
	targetInstrument->envelope[envCutoff].loop = 0;
	targetInstrument->envelope[envCutoff].enable = 0;

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

	for(uint8_t i = 0;  i < envMax;  i++)
	{
		targetInstrument->lfo[i].amount = 0.5;
		targetInstrument->lfo[i].shape = 0;
		targetInstrument->lfo[i].speed = 0;
	}



}
