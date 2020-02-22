

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"


#include "mtSequencer.h"

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
		case 0: // faza przygotowania (obliczanie postepu, clear itp)
		{
			loadWorkspaceProjectInit();
			break;
		}
		case 1: // projekt
		{
			loadProjectFromWorkspace();
			break;
		}
		case 2: // pattern
		{
			loadPatternFromWorkspace(mtProject.values.actualPattern);
			break;
		}
		case 3: // instruments
		{
			loadInstrumentsFromWorkspace();
			break;
		}
		case 4: // samples
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


	mtProject.used_memory = 0;
	mtProject.instruments_count = 0;

	currentOperationStep = 0;
	currentOperation = fmOpenWorkspaceProject;


	return true;
}

void cFileManager::loadWorkspaceProjectInit()
{
	calcTotalMemoryToTransfer();
	moveToNextOperationStep();
}

void cFileManager::calcTotalMemoryToTransfer()
{
	totalMemoryToTranfser = 0;
	totalMemoryToTranfser += sizeof(strProjectFile);
	totalMemoryToTranfser += sizeof(Sequencer::strPattern);
	totalMemoryToTranfser += calcWorkspaceInstrumentsSize();
	totalMemoryToTranfser += calcWorkspaceSamplesSize();

}

void cFileManager::calcActualMemoryTransfered()
{
	if(currentOperationStep > 0) actualMemoryTransfered = sizeof(strProjectFile);
	if(currentOperationStep > 1) actualMemoryTransfered += sizeof(Sequencer::strPattern);
	if(currentOperationStep > 2) actualMemoryTransfered += sizeof(strInstrumentFile)*mtProject.instruments_count;
	if(currentOperationStep > 3) actualMemoryTransfered += getActualSampleMemoryLoaded();

}



uint8_t cFileManager::getProgress()
{
	calcActualMemoryTransfered();
	return (actualMemoryTransfered < totalMemoryToTranfser) ? (actualMemoryTransfered*100)/totalMemoryToTranfser: 100;
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
	sprintf(errorText,  "File manager error (%d)(%d)(%d)", currentOperation, currentOperationStep, source);
	debugLog.addLine(errorText);
	debugLog.forceRefresh();
#endif


	status = fmLoadError;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

