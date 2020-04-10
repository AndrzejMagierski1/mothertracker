#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "mtSequencer.h"

#include "Arduino.h"

#include "fileTransfer.h"
#include "fileManager.h"

char file[255];

SdFile exportedFile;
// wystartuj operację
uint8_t cFileManager::exportItFile()
{
	if (status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if (currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject)
		return false;

	currentOperationStep = 0;
	status = fmExportingMod;
	currentOperation = fmExportModFile;

	return true;
}

void cFileManager::exportItFile_Init()
{
	Serial.println("exportItFile_Init");

	if (!SD.exists("Export")) SD.mkdir(0, "Export");

	sprintf(file,
			cExportModFilePathFormat,
			newFileManager.getCurrentProjectName());

	if (SD.exists(file))
	{
		SD.remove(file);
	}

	if (exportedFile.open(file, FILE_WRITE))
	{
		moveToNextOperationStep();
	}
	else
	{
		exportItFile_Error();
	}
}

uint8_t* writeLE(uint8_t *dest, uint32_t val, uint8_t size)
{

	for (uint8_t a = 0; a < size; a++)
	{

	}
	dest += size;

	return dest;

}
void cFileManager::exportItFile_InitHeader()
{
	Serial.println("exportItFile_InitHeader");

	uint8_t buff[0x20] { 0 };

	sprintf((char*) buff, "IMPM%.26s",
			newFileManager.getCurrentProjectName());
	exportedFile.write(buff, sizeof(buff));

	memset(buff, 0, sizeof(buff));

	moveToNextOperationStep();
}

void cFileManager::exportItFile_Finish()
{
	Serial.println("exportItFile_Finish");
	if (exportedFile.isOpen())
	{
		exportedFile.close();
	}

	moveToNextOperationStep();
}
void cFileManager::exportItFile_Error()
{
	Serial.println("exportItFile_Error");
	throwError(1);
	if (exportedFile.isOpen())
	{
		exportedFile.close();
	}

}
