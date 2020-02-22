
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

__NOINIT(EXTERNAL_RAM) Sequencer::strPattern fileManagerPatternBuffer  {0};



void cFileManager::loadPatternFromWorkspace(uint8_t index)
{
	char patternToLoad[PATCH_SIZE];
	sprintf(patternToLoad, cWorkspacePatternFileFormat, index);
	mtProject.values.actualPattern = index;

	uint8_t loadStatus = fileTransfer.loadFileToMemory(patternToLoad,  (uint8_t*)&fileManagerPatternBuffer, sizeof(Sequencer::strPattern), fileDivIntoParts);

	if(loadStatus == fileTransferEnd)
	{
		if(loadPatternFormFileStruct(sequencer.getPatternToLoadFromFile(), (uint8_t*)&fileManagerPatternBuffer))
		{
			sequencer.loadFromFileOK();
			sequencer.switchRamPatternsNow();
			moveToNextOperationStep();
		}
		else
		{
			throwError(0);
		}
	}
	else if(loadStatus == fileTransferFileNoExist) // brak pliku patternu
	{
		sequencer.loadFromFileERROR();
	}
	else if(loadStatus >= fileTransferError)
	{
		throwError(1);
	}

}

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

bool cFileManager::readPatternFile(const char * filePath, uint8_t *destPattern)
{
	SdFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC = 0;
	bool loadStatus = false;

	// na końcu struktury jest crc
	file.open(filePath);

	if(file)
	{
		file.read(destPattern, sizeof(Sequencer::strPattern));
		file.close();

		checkCRC = crcCalc.crc32(
				destPattern, sizeof(Sequencer::strPattern) - sizeof(uint32_t));

		if (checkCRC == (((Sequencer::strPattern *) destPattern)->crc))
		{
			loadStatus = true;
			// ok
		}
		else
		{
			loadStatus = false;
			// not ok
		}
	}

	return loadStatus;
}

bool cFileManager::writePatternFile(const char* filePath, uint8_t* sourcePattern)
{
	SD.remove(filePath);

	SdFile file;
	FastCRC32 crcCalc;

	sequencer.setPatternHeader((Sequencer::strPattern*)sourcePattern);

	((Sequencer::strPattern*) sourcePattern)->crc =
			crcCalc.crc32(sourcePattern,
							sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	file.open(filePath, FILE_WRITE);
	file.write(sourcePattern, sizeof(Sequencer::strPattern));
	file.close();

	return true;
}


bool cFileManager::saveActualPattern(const char* path, uint8_t index)
{
	char patternToSave[PATCH_SIZE];
	sprintf(patternToSave, "%s/pattern_%02d.mtp", path, index);
	return writePatternFile(patternToSave, sequencer.getPatternToSaveToFile());
}





bool cFileManager::loadPattern(const char* path, uint8_t index)
{
	bool status = false;
	char patternToLoad[PATCH_SIZE];

	sprintf(patternToLoad, "%s/pattern_%02d.mtp", path, index);
	mtProject.values.actualPattern = index;

	if(!SD.exists(patternToLoad))
	{
		status = false;
	}
	else
	{
		status = readPatternFile(patternToLoad, sequencer.getPatternToLoadFromFile());
	}

	if(status)
	{
		sequencer.loadFromFileOK();
	}
	else
	{
		sequencer.loadFromFileERROR();
	}

	return status;
}


bool cFileManager::loadTrack(uint8_t pattIndex, uint8_t trackIndex)
{
	if(trackIndex > 7) trackIndex = 7;

	char patternToLoad[PATCH_SIZE] { 0 };
	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", pattIndex);


	Sequencer::strPattern * patternFrom = (Sequencer::strPattern*) sequencer.getPatternToLoadFromFile();
	Sequencer::strPattern * patternTo = sequencer.getActualPattern();



	if (SD.exists(patternToLoad))
	{
		readPatternFile(patternToLoad, sequencer.getPatternToLoadFromFile());
	}
	else
	{
		sequencer.clearPattern(patternFrom);
	}



	patternTo->track[trackIndex] = patternFrom->track[trackIndex];

	mtProject.values.perfTracksPatterns[trackIndex] = pattIndex;

	return true;
}
