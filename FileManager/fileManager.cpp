

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"


#include "fileManager.h"


cFileManager newFileManager;



//====================================================================================
void cFileManager::update()
{
	if(currentOperation == fmNoOperation) return;


	switch(currentOperation)
	{
	case fmOpenWorkspaceProject:
	{
		updateOpenWorkspaceProject();
		break;
	}

	}







}




//====================================================================================
//==========================     UPDATEs     =========================================
//====================================================================================
void cFileManager::updateOpenWorkspaceProject()
{
	switch(currentOperationStep)
	{
		case 0: // projekt
		{
			loadProjectFromWorkspace();
			break;
		}
		case 1: // pattern
		{
			loadPatternFromWorkspace(mtProject.values.actualPattern);
			break;
		}
		case 2: // instruments
		{
			loadInstrumentsFromWorkspace();
			break;
		}
		case 3: // samples
		{
			loadSamplesFromWorkspace();
			break;
		}



		default:
		{
			status = fmLoadEnd;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
		}

	}



}




//====================================================================================
//====================================================================================
//====================================================================================
bool cFileManager::openProjectFromWorkspace()
{
	if(status != fmIdle) return false;
	if(currentOperation != fmNoOperation) return false;

	if(!SD.exists(cWorkspacePath)) return false;
	if(!SD.exists(cProjectFileNameInWorkspace)) return false;
	if(!SD.exists(cProjectFileNameInWorkspace)) return false;


	//todo dalej w nastepnych update

//	if(loadPattern(mtProject.values.actualPattern))
//	{
//		sequencer.switchRamPatternsNow();
//	}

//	memset(&loadFromWorkspaceHandle, 0, sizeof(save_load_handle_t));
//	moveToNextStage(&loadFromWorkspaceHandle);
//	loadingInProgress = 1;

	mtProject.used_memory = 0;

	currentOperationStep = 0;
	currentOperation = fmOpenWorkspaceProject;


	return true;
}




void cFileManager::moveToNextOperationStep()
{
	currentOperationStep++;
}


#ifdef DEBUG
static char errorText[50];
#endif

void cFileManager::throwError(uint8_t source)
{
#ifdef DEBUG
	debugLog.setMaxLineCount(10);
	sprintf(errorText,  "File manager error (%d)", source);
	debugLog.addLine(errorText);
	debugLog.forceRefresh();
#endif


	status = fmLoadError;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

