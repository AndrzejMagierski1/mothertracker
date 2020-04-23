
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "debugLog.h"


#include "fileManager.h"

#include "mtRecorder.h"


static char recordingsDirPath[30];
static char recordedFileName[35];

void cFileManager::saveRecording()
{
	if(recorder.getSaveState() == 0)
	{
		if(status == fmSavingImportingRecordedSound)
		{
			newSamplesSize = 0;

			// kasowanie zuzycia pamieci przez nadpisywany instrument
			if(mtProject.instrument[recordingImportSlot].isActive == 1)
			{
				mtProject.used_memory -= mtProject.instrument[recordingImportSlot].sample.length*2;
			}

			currentInstrument = recordingImportSlot;
			currentSample = recordingImportSlot;
			importStartSlot = recordingImportSlot;
			importEndSlot = recordingImportSlot;
			calcFirstSlotToMoveInMemory(importEndSlot+1);

			// do progressu
			samplesMemoryCopied = 0;

			moveToNextOperationStep();
		}
		else
		{
			status = fmSaveRecordedSoundEnd;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
		}
	}
}

void cFileManager::saveRecordedSoundFinish()
{
	status = fmSaveRecordedSoundEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}






char* cFileManager::getRecordingDirPath()
{
	return recordingsDirPath;
}

char* cFileManager::getRecordingFileName()
{
	return recordedFileName;
}

bool cFileManager::saveRecordedSound(char* dirPath, char* fileName, int8_t importSlot)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;


	// utworz folder jesli nie istnieje
	if(!SD.exists(dirPath)) SD.mkdir(0, dirPath);

	// tworzy sciezke do pliku
	char filePath[70];
	sprintf(filePath, "%s/%s.wav", dirPath, fileName);
	if(SD.exists(filePath)) SD.remove(filePath);

	recorder.startSave(filePath);

	if(importSlot < 0)
	{
		status = fmSavingRecordedSound;
		currentOperationStep = 0;
		currentOperation = fmSaveRecordedSound;
	}
	else
	{
		strcpy(recordingsDirPath, dirPath);
		strcpy(recordedFileName, fileName);
		recordingImportSlot = importSlot;
		status = fmSavingImportingRecordedSound;
		currentOperationStep = 0;
		currentOperation = fmSaveRecordedSound;
	}

	return true;
}




