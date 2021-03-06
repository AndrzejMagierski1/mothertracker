
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

__NOINIT(EXTERNAL_RAM) Sequencer::strPattern fileManagerPatternBuffer {0};
__NOINIT(EXTERNAL_RAM) Sequencer::strPattern fileManagerPerformancePatternBuffer {0};




//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     LOAD     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void cFileManager::loadPatternFromWorkspace(uint8_t index)
{
	char patternToLoad[PATCH_SIZE];
	sprintf(patternToLoad, cWorkspacePatternFileFormat, index);
	loadedPattern = index;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(patternToLoad,  (uint8_t*)&fileManagerPatternBuffer, sizeof(Sequencer::strPattern), fileWholeOnce); //fileDivIntoParts

	if(loadStatus == fileTransferEnd)
	{
		if(loadPatternFormFileStruct(sequencer.getPatternToLoadFromFile(), (uint8_t*)&fileManagerPatternBuffer))
		{
			sequencer.loadFromFileOK();
			//sequencer.switchRamPatternsNow();
			report(" no blocking pattern load - success ",loadedPattern);
			moveToNextOperationStep();
		}
		else
		{
			throwError(0);
		}
	}
	else if(loadStatus == fileTransferFileNoExist) // brak pliku patternu
	{
		report(" no blocking pattern load - file no exist ", patternToLoad);
		sequencer.loadFromFileERROR();
		moveToNextOperationStep();
	}
	else if(loadStatus >= fileTransferError)
	{
		throwError(1);
	}

}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     COPY     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::copyPaterns()
{
	if(currentOperation == fmCopyWorkspaceToProjects)
	{
		sprintf(currentCopySrcPath, cWorkspacePatternFileFormat, currentPattern);
		sprintf(currentCopyDestPath, cProjectsPatternFileFormat, currentProjectName, currentPattern);
	}
	else
	{
		sprintf(currentCopySrcPath, cProjectsPatternFileFormat, currentProjectName, currentPattern);
		sprintf(currentCopyDestPath, cWorkspacePatternFileFormat, currentPattern);
	}

	uint8_t loadStatus = fileTransfer.copyFile(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		copiedPatternsCount++;
		report(" copy patterns - success ", currentPattern);
		continuePatternProcess();
	}
	else if(loadStatus == fileTransferFileNoExist)
	{
		continuePatternProcess();
	}
	else if(loadStatus >= fileTransferError)
	{
		currentPattern = 0;
		throwError(0);
	}
}




//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     SAVE     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::savePatternToWorkspace()
{
	if(changesFlags.pattern[mtProject.values.actualPattern] == 0) // jesli flaga zmian nie ustawiona omin zapis
	{

		report(" no blocking pattern save - no changes - ", mtProject.values.actualPattern);
		moveToNextOperationStep();
		return;
	}

	// sprawdz bitmaski traków patternu, moze cos sie zmienilo w ciagu jednego przejścia petli od zapisu projektu? ;>
	if(updatePatternBitmask(mtProject.values.actualPattern))
	{
		// jesli struktura bitow sie zminila wymus ponowna aktualizcje projektu = ponowny autosave po skonczeniu tego
		changesFlags.project = 1;
	}


	if(!writePatternToFileStruct(sequencer.getPatternToSaveToFile(), (uint8_t*)&fileManagerPatternBuffer))
	{
		throwError(0);
	}

	char patternToSave[PATCH_SIZE];
	sprintf(patternToSave, cWorkspacePatternFileFormat, mtProject.values.actualPattern);

	uint8_t saveStatus = fileTransfer.saveMemoryToFile((uint8_t*)&fileManagerPatternBuffer, patternToSave, sizeof(Sequencer::strPattern));

	if(saveStatus == fileTransferEnd)
	{
		report(" no blocking pattern save - success ", mtProject.values.actualPattern);
		clearPatternChanged(mtProject.values.actualPattern);
		sequencer.saveToFileDone();
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

bool cFileManager::loadPatternFormFileStruct(uint8_t* pattern, uint8_t* patternFile)
{
	FastCRC32 crcCalc;

	//if (((Sequencer::strPattern*)patternFile)->header.type != fileTypePattern)
	//	return false;

	uint32_t checkCRC = crcCalc.crc32(patternFile,
									  sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	if (checkCRC == (((Sequencer::strPattern*)patternFile)->crc))
	{
		memcpy(pattern, patternFile, sizeof(Sequencer::strPattern));
		return true;
		// ok
	}

	return false;
}


bool cFileManager::writePatternToFileStruct(uint8_t* pattern, uint8_t* patternFile)
{
	FastCRC32 crcCalc;

	memcpy(patternFile, pattern, sizeof(Sequencer::strPattern));

	sequencer.setPatternHeader((Sequencer::strPattern*)patternFile);


	((Sequencer::strPattern*) patternFile)->crc =
			crcCalc.crc32(patternFile, sizeof(Sequencer::strPattern) - sizeof(uint32_t));


	return true;
}



//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool cFileManager::continuePatternProcess()
{
	currentPattern++;
	if(currentPattern > PATTERN_INDEX_MAX)
	{
		currentPattern = 1;
		moveToNextOperationStep();
		return false;
	}
	return true;
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool cFileManager::loadWorkspacePatternNow(uint8_t index)
{
	bool status = false;
	char patternToLoad[PATCH_SIZE];

	sprintf(patternToLoad, cWorkspacePatternFileFormat, index);
	loadedPattern = index;

	if(!SD.exists(patternToLoad))
	{
		report(" load patern now - file no exist ", patternToLoad);
		status = false;
	}
	else
	{
		status = readPatternFile(patternToLoad, sequencer.getPatternToLoadFromFile());
	}

	if(status)
	{
		report(" load pattern now - success " , loadedPattern);
		sequencer.loadFromFileOK();
	}
	else
	{
		report(" load pattern now - failed " , loadedPattern);
		sequencer.loadFromFileERROR();
	}

	return status;
}


bool cFileManager::saveWorkspacePatternNow(uint8_t index)
{
	char patternToSave[PATCH_SIZE] { 0 };
	bool status = false;

	if(changesFlags.pattern[index] == 1)
	{
		changesFlags.pattern[index] = 0;
		sprintf(patternToSave, cWorkspacePatternFileFormat, index);
		status = writePatternFile(patternToSave, sequencer.getPatternToSaveToFile());
		sequencer.saveToFileDone();

		if(updatePatternBitmask(mtProject.values.actualPattern))
		{
			// jesli struktura bitow sie zminila wymus aktualizcje projektu
			changesFlags.project = 1;
		}
		if(status == true) report(" save pattern now - success " , mtProject.values.actualPattern);
		else  report(" save pattern now - failed " , mtProject.values.actualPattern);
	}
	else
	{
		report(" save pattern now - no changes " , mtProject.values.actualPattern);
	}

	return status;
}



//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     XXXX     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------



bool cFileManager::writePatternFile(const char* filePath, uint8_t* sourcePattern)
{
	SD.remove(filePath);

	SdFile file;
	FastCRC32 crcCalc;
	bool saveStatus = false;

	sequencer.setPatternHeader((Sequencer::strPattern*)sourcePattern);

	((Sequencer::strPattern*) sourcePattern)->crc =
			crcCalc.crc32(sourcePattern,
							sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	saveStatus = file.open(filePath, FILE_WRITE);

	if(saveStatus)
	{
		if(file.write(sourcePattern, sizeof(Sequencer::strPattern)) < 0) saveStatus = false;
		file.close();
	}

	if(saveStatus == false)
	{
		report(" write patern file - write error ", filePath);
		return false;
	}


	return true;
}


bool cFileManager::readPatternFile(const char * filePath, uint8_t *destPattern)
{
	SdFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC = 0;
	bool loadStatus = false;

	// na końcu struktury jest crc
	loadStatus = file.open(filePath);

	if(loadStatus)
	{
		if(file.read(destPattern, sizeof(Sequencer::strPattern)) < 0) loadStatus = false;
		file.close();
	}

	if(loadStatus == false)
	{
		report(" load patern file - read error ", filePath);
		return false;
	}


	checkCRC = crcCalc.crc32(
			destPattern, sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	if (checkCRC != (((Sequencer::strPattern *) destPattern)->crc))
	{
		report(" load patern file - crc error ", filePath);
		return false;
	}

	return true;
}


/// tu tylko ladujemy do bufora, podmieniamy w sequ
bool cFileManager::loadPerformanceTrackToBuffer(uint8_t pattIndex, uint8_t trackIndex)
{
	report(" load perf track to buff");

	if(trackIndex > 7) trackIndex = 7;

	char patternToLoad[PATCH_SIZE] { 0 };
	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", pattIndex);


	Sequencer::strPattern * patternFrom = (Sequencer::strPattern*) sequencer.getPatternToLoadFromFile();
	Sequencer::strPattern * patternTo = &fileManagerPerformancePatternBuffer;



	if (SD.exists(patternToLoad))
	{
		readPatternFile(patternToLoad, sequencer.getPatternToLoadFromFile());
	}
	else
	{
		sequencer.clearPattern(patternFrom);
	}



	patternTo->track[trackIndex] = patternFrom->track[trackIndex];

	// lengthy tracka na potrzeby performance przepisuje do konretnego tracka
	patternTo->track[trackIndex].length = patternFrom->track[0].length;

	mtProject.values.perfTracksPatterns[trackIndex] = pattIndex;

	return true;
}

uint8_t cFileManager::getLoadedPatternIndex()
{
	return loadedPattern;
}


uint32_t cFileManager::calcProjectPatternsSize()
{
	char currentPatch[PATCH_SIZE];
	uint32_t size = 0;
	projectPatternsCount = 0;

	for(uint16_t i = 1; i < PATTERN_INDEX_MAX+1; i++)
	{
		sprintf(currentPatch, cProjectsPatternFileFormat, currentProjectName, i);
		if(SD.exists(currentPatch))
		{
			size += sizeof(Sequencer::strPattern);
			projectPatternsCount++;
		}
	}

	return size;
}

