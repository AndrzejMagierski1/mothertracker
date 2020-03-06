
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "debugLog.h"


#include "fileManager.h"

#include "mtRecorder.h"



static char recordedFileName[35];

void cFileManager::saveRecording()
{
	if(recorder.getSaveState() == 0)
	{
		if(status == fmSavingImportingRecordedSound)
		{
			importSamplesSize = 0;

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







char* cFileManager::getRecordingFileName()
{
	return recordedFileName;
}

bool cFileManager::saveRecordedSound(char* fileName, int8_t importSlot)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	if(!SD.exists("Recorded")) SD.mkdir(0,"Recorded");

	char filePath[70];
	sprintf(filePath, "Recorded/%s.wav", fileName);
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
		strcpy(recordedFileName, fileName);
		recordingImportSlot = importSlot;
		status = fmSavingImportingRecordedSound;
		currentOperationStep = 0;
		currentOperation = fmSaveRecordedSound;
	}

	return true;
}




