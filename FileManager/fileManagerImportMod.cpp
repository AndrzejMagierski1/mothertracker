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
	char modFilePath[255];
	char byteBuffer[20];

	sprintf(modFilePath, "%s/%s", cModsPath, modToImportFilename);

	uint8_t loadStatus = fileTransfer.loadFileToMemory(
														modFilePath,
														(uint8_t*) byteBuffer,
														20,
														fileWholeOnce); // fileDivIntoParts



//	moveToNextOperationStep();
	if (loadStatus == fileTransferEnd)
	{


		moveToNextOperationStep();

	}
	else if (loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}
void cFileManager::importModFileInstruments()
{

	moveToNextOperationStep();
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
