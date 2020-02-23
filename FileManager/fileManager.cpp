

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"


#include "mtSequencer.h"

#include "fileManager.h"


cFileManager newFileManager;

SdDir sdLocation;


//====================================================================================
//====================================================================================
//====================================================================================
void cFileManager::update()
{
	if(currentOperation == fmNoOperation) return;

	switch(currentOperation)
	{
	case fmOpenWorkspaceProject: 	updateOpenWorkspaceProject(); 		break;
	case fmCopyProjectToWorkspace: 	updateCopyProjectToWorkspace(); 	break;




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
			loadWorkspaceProjectInit(); break;
		case 1: // projekt
			loadProjectFromWorkspace(); break;
		case 2: // pattern
			loadPatternFromWorkspace(mtProject.values.actualPattern); break;
		case 3: // instruments
			loadInstrumentsFromWorkspace(); break;
		case 4: // samples
			loadSamplesFromWorkspace(); break;
		case 5: // wykonczenie
			loadWorkspaceProjectFinish(); break;
		default:
			status = fmLoadError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}


void cFileManager::updateCopyProjectToWorkspace()
{
	switch(currentOperationStep)
	{
		case 0: // faza przygotowania
			copyProjectToWorkspaceInit(); break;
		case 1: // projekt
			copyProjectToWorkspace(); break;
		case 2: // pattern
			copyPaternsToWorkspace(); break;
		case 3: // instruments
			copyInstrumentsToWorkspace(); break;
		case 4: // samples
			copySamplesToWorkspace(); break;
		case 5: // wykonczenie
			copyProjectToWorkspaceFinish(); break;
		default:
			status = fmCopyError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
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

	status = fmLoadingProjectfromWorkspace;
	currentOperationStep = 0;
	currentOperation = fmOpenWorkspaceProject;

	return true;
}


bool cFileManager::openProjectFromProjects(uint8_t index)
{
	if(status != fmIdle) return false;
	if(currentOperation != fmNoOperation) return false;
	if(projectsNames[index] == nullptr) return false;

	//pobranie nazwy otwieranego projektu tu
	strcpy(currentProjectName, projectsNames[index]);

	status = fmLoadingProjectFromProjects;
	currentOperationStep = 0;
	currentOperation = fmCopyProjectToWorkspace;

	return true;
}

uint8_t cFileManager::getProjectsList(char*** list)
{
	sdLocation.close();
	sdLocation.open(cProjectsPath, O_READ);
	uint8_t projectsfoundCount = sdLocation.createProjectsList(projectsNames, files_list_length_max, 3000);
	sdLocation.close();

	for (uint8_t i = 0; i < (projectsfoundCount/2); i++)
	{
		std::swap(projectsNames[i], projectsNames[projectsfoundCount-i-1]);
	}

	*list = projectsNames;

	return projectsfoundCount;
}


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

void cFileManager::loadWorkspaceProjectInit()
{
	calcTotalMemoryToTransfer();
	moveToNextOperationStep();
}


void cFileManager::loadWorkspaceProjectFinish()
{
	//projectEditor.loadProjectValues(); // jest odrazu po otwarciu pliku projektu, gdyz dalszy proces wymaga nie ktorych zmiennych
	clearChangeFlags();

	if(currentProjectName[0] == 0) // jezeli nie ma nazwyw w pliku na koniec ladowania to
	{

	}


	status = fmLoadEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}



void cFileManager::copyProjectToWorkspaceInit()
{
	// przygotuj workspace
	clearWorkspace();
	createWorkspaceDirs();

	//

	moveToNextOperationStep();
}

void cFileManager::copyProjectToWorkspaceFinish()
{
	status = fmIdle;					//
	currentOperation = fmNoOperation;	// takie cwaniactwo pozwala wywolac otwieranie z workspace w tym miejscu

	if(!openProjectFromWorkspace())
	{
		createNewProjectInWorkspace();
		openProjectFromWorkspace();
	}

	status = fmLoadingProjectFromProjects; // wymuszenie statusu
}



//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

void cFileManager::clearChangeFlags()
{
	projectChangeFlag = 0;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		patternIsChangedFlag[i] = 0;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		instrumentIsChangedFlag[i] = 0;
	}
}

bool cFileManager::isProjectChanged()
{
	if(projectChangeFlag) return true;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		if(patternIsChangedFlag[i]) return true;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		if(instrumentIsChangedFlag[i]) return true;
	}

	 return false;
}

void cFileManager::setProjectStructChanged()
{
	projectChangeFlag = 1;
}

void cFileManager::setPatternStructChanged(uint8_t pattern)
{
	patternIsChangedFlag[pattern] = 1;
}

void cFileManager::setInstrumentStructChanged(uint8_t instrument)
{
	instrumentIsChangedFlag[instrument] = 1;
}

















//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------


void cFileManager::calcTotalMemoryToTransfer()
{
	totalMemoryToTranfser = 0;
	totalMemoryToTranfser += sizeof(strProjectFile);
	totalMemoryToTranfser += sizeof(Sequencer::strPattern);
	totalMemoryToTranfser += calcWorkspaceInstrumentsSize();
	totalMemoryToTranfser += calcWorkspaceSamplesSize(); //xxx moze troche trwac, mozna zoptymalizwac

}

void cFileManager::calcActualMemoryTransfered()
{
	if(currentOperationStep > 0) actualMemoryTransfered = sizeof(strProjectFile);
	if(currentOperationStep > 1) actualMemoryTransfered += sizeof(Sequencer::strPattern);
	if(currentOperationStep > 2) actualMemoryTransfered += sizeof(strInstrumentFile)*mtProject.instruments_count;
	if(currentOperationStep > 3) actualMemoryTransfered += getActualSampleMemoryLoaded();
	if(currentOperationStep > 4) actualMemoryTransfered += getActualSampleMemoryLoaded();

}



uint8_t cFileManager::getProgress()
{
	if(status == fmLoadEnd) return 100;

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

