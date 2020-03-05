#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "mtSequencer.h"

#include "fileTransfer.h"
#include "fileManager.h"

void cFileManager::importModFileInit()
{

	char byteBuffer[4];

	sprintf(modFilePath, "%s/%s", cModsPath, modToImportFilename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFilePath,
														(uint8_t*) byteBuffer,
														4,
														1080,
														fileWholeOnce); // fileDivIntoParts

//	moveToNextOperationStep();
	if (loadStatus == fileTransferEnd)
	{

		if (strstr(byteBuffer, "M.K.") != NULL)
		{
			modFileInstrumentsCount = 31;
			modFileChannelsCount = 4;
			moveToNextOperationStep();
		}
		else
		{
			throwError(2);
		}

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
//	moveToNextOperationStep();
}
void cFileManager::importModFileInstruments()
{

	char byteBuffer[modSampleInfoSize];
	uint8_t buffOffset = 0;

	sprintf(modFilePath, "%.10s/%.200s", cModsPath, modToImportFilename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFilePath,
														(uint8_t*) byteBuffer,
														modSampleInfoSize, // memo
														20+modFileInstruments_actualIndex*modSampleInfoSize, // offset
														fileWholeOnce); // fileDivIntoParts

	if (loadStatus == fileTransferEnd)
	{

		//sampleName
//		uint8_t a = 0;
		memset(modSampleData.sampleName,0, sizeof(modSampleData.sampleName));
		for (uint8_t a = 0; a < modSampleNameSize; a++)
		{
			if (byteBuffer[a] != 0)
			{
				modSampleData.sampleName[a] = byteBuffer[a];
			}
			else
				break;
		}
		buffOffset = modSampleNameSize-1;

		//sampleLengthInWords
		modSampleData.sampleLengthInWords = byteBuffer[buffOffset+1] << 8 | byteBuffer[buffOffset+2];
		buffOffset+=2;
		modSampleData.finetune = byteBuffer[buffOffset++] & 0b00001111;
		modSampleData.volume = byteBuffer[buffOffset++];

		modSampleData.repeatPointInWords = byteBuffer[buffOffset+1] << 8 | byteBuffer[buffOffset+2];
		buffOffset+=2;
		modSampleData.repeatLengthInWords = byteBuffer[buffOffset+1] << 8 | byteBuffer[buffOffset+2];
		buffOffset+=2;

		//;TUTAJ UTRORZYC PLIK INSTRUMENTU


		modFileInstruments_actualIndex++;
		if (modFileInstruments_actualIndex >= modFileInstrumentsCount)
		{
			moveToNextOperationStep();
		}

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}

//	moveToNextOperationStep();
}

void cFileManager::importModFilePatterns()
{

	moveToNextOperationStep();
}
void cFileManager::importModFileWaves()
{

	moveToNextOperationStep();
}
void cFileManager::importModFileFinish()
{
	status = fmLoadEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

void cFileManager::importModFileError()
{
	importModFileAfterNewProject = 0;

}
