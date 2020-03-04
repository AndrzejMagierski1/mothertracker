
#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "debugLog.h"

#include "mtExporterWAV.h"

#include "fileManager.h"
//#include "fileTransfer.h"



static bool getSongExportFileName(char* path);
static bool getSongStemsExportFileName(char* path);
static bool getPatternExportFileName(char* path);
static bool getPatternStemsExportFileName(char* path);
static bool getRenderSelectionExportFileName(char* path);


bool cFileManager::exportSound(uint8_t mode)
{
	char currentExportPath[255];

	//make dirs boys
	if(!SD.exists("Export")) SD.mkdir(0,"Export");
	sprintf(currentExportPath, cExportProjectPathFormat, newFileManager.getCurrentProjectName());
	if(!SD.exists(currentExportPath)) SD.mkdir(0, currentExportPath);


	switch(mode)
	{
	case exportSong: 				if(!getSongExportFileName(currentExportPath)) return false; 		break;
	case exportSongStems:
	case exportPattern:
	case exportPatternStems:
	case exportRenderSelection:
	default: return false;
	}


	exporter.start(currentExportPath, mode);


	status = fmExportingSound;
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


static bool getSongExportFileName(char* path)
{
	uint16_t fileCounter = 0;

	sprintf(path, "Export/%s/%s_S.wav", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName());
	while(SD.exists(path))
	{
		sprintf(path, "Export/%s/%s_S%d.wav", newFileManager.getCurrentProjectName(), newFileManager.getCurrentProjectName(), ++fileCounter);
		if(fileCounter > 9999) return false; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}

	return true;
}

static bool getSongStemsExportFileName(char* path)
{

	return true;
}

static bool getPatternExportFileName(char* path)
{

	return true;
}

static bool getPatternStemsExportFileName(char* path)
{

	return true;
}

static bool getRenderSelectionExportFileName(char* path)
{

	return true;
}




