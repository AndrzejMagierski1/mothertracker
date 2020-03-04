
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "debugLog.h"

#include "mtExporterWAV.h"

#include "fileManager.h"
//#include "fileTransfer.h"



static bool getSongExportFileName(char* path);
static bool getSongStemsExportFolderName(char* path);
static bool getPatternExportFileName(char* path);
static bool getPatternStemsExportFolderName(char* path);
static bool getRenderSelectionExportFileName(char* path);


bool cFileManager::exportSound(uint8_t mode)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	char currentExportPath[255];

	//make dirs boys
	if(!SD.exists("Export")) SD.mkdir(0,"Export");
	sprintf(currentExportPath, cExportProjectPathFormat, newFileManager.getCurrentProjectName());
	if(!SD.exists(currentExportPath)) SD.mkdir(0, currentExportPath);

	uint8_t temp_status = fmIdle;

	switch(mode)
	{
	case exportSong:
		temp_status = fmExportingSoundSong;
		if(!getSongExportFileName(currentExportPath)) return false;
		break;
	case exportSongStems:
		temp_status = fmExportingSoundSongStems;
		if(!getSongStemsExportFolderName(currentExportPath)) return false;
		if(!SD.exists(currentExportPath)) SD.mkdir(0, currentExportPath);
		break;
	case exportPattern:
		temp_status = fmExportingSoundPattern;
		if(!getPatternExportFileName(currentExportPath)) return false;
		break;
	case exportPatternStems:
		temp_status = fmExportingSoundPatternStems;
		if(!getPatternStemsExportFolderName(currentExportPath)) return false;
		if(!SD.exists(currentExportPath)) SD.mkdir(0, currentExportPath);
		break;
	case exportRenderSelection:
		temp_status = fmExportingSoundRenderSelection;
		if(!getRenderSelectionExportFileName(currentExportPath)) return false;
		//if(!SD.exists("Render")) SD.mkdir(0,"Render");
		break;
	default: return false;
	}

	exporter.start(currentExportPath, mode);

	status = temp_status;
	currentOperationStep = 0;
	currentOperation = fmExportSound;
	return true;
}

void cFileManager::exportSoundEnd()
{

	status = fmExportSoundEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}


void cFileManager::exportSoundGetStatus()
{
	if(exporter.getState() == 0)
	{
		exportSoundEnd();
	}
}



bool cFileManager::exportSoundCancel()
{
	exporter.cancel();
	exportSoundEnd();
	return true;
}

uint8_t cFileManager::getExportProgress()
{
	return exporter.getProgress();
}

static bool getSongExportFileName(char* path)
{
	uint16_t fileCounter = 0;

	sprintf(path, "Export/%s/%s_Song%4d.wav", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), fileCounter++);
	while(SD.exists(path))
	{
		sprintf(path, "Export/%s/%s_Song%4d.wav", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), fileCounter++);
		if(fileCounter > 9999) return false; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}

	return true;
}

static bool getSongStemsExportFolderName(char* path)
{
	uint16_t fileCounter = 0;

	sprintf(path, "Export/%s/%s_Song_S%4d", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), fileCounter++);
	while(SD.exists(path))
	{
		sprintf(path, "Export/%s/%s_Song_S%4d", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), fileCounter++);
		if(fileCounter > 9999) return false; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}

	return true;
}

static bool getPatternExportFileName(char* path)
{
	uint16_t namePattern = mtProject.values.actualPattern;
	uint16_t fileCounter = 0;

	sprintf(path, "Export/%s/%s_P%d_%4d.wav", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), namePattern, fileCounter++);
	while(SD.exists(path))
	{
		sprintf(path, "Export/%s/%s_P%d_%4d.wav", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), namePattern, fileCounter++);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}

	return true;
}

static bool getPatternStemsExportFolderName(char* path)
{
	uint16_t namePattern = mtProject.values.actualPattern;
	uint16_t fileCounter = 0;

	sprintf(path, "Export/%s/%s_P%d_S%4d", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), namePattern, fileCounter++);
	while(SD.exists(path))
	{
		sprintf(path, "Export/%s/%s_P%d_S%4d", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), namePattern, fileCounter++);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}

	return true;
}

static bool getRenderSelectionExportFileName(char* path)
{
	uint16_t fileCounter = 0;

	sprintf(path, "Export/%s/%s_Sel%4d", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), fileCounter++);
	while(SD.exists(path))
	{
		sprintf(path, "Export/%s/%s_Sel%4d", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), fileCounter++);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}

	return true;
}




