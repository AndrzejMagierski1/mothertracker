

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"


#include "mtSequencer.h"

#include "fileManager.h"


cFileManager newFileManager;

SdDir sdLocation;

elapsedMillis autoSaveTimer;

//====================================================================================
//====================================================================================
//====================================================================================
void cFileManager::update()
{
	if(currentOperation == fmNoOperation)
	{
		autoSaveProjectToWorkspace();


		return;
	}

	switch(currentOperation)
	{
	case fmLoadWorkspaceProject: 		updateLoadProjectFromWorkspace(); 		break;
	case fmSaveWorkspaceProject: 		updateSaveProjectToWorkspace(); 		break;
	case fmCopyProjectsToWorkspace: 	updateCopyProjectsToWorkspace(); 		break;
	case fmCopyWorkspaceToProjects: 	updateCopyWorkspaceToProjects(); 		break;




	}


}




//====================================================================================
//==========================     UPDATEs     =========================================
//====================================================================================
void cFileManager::updateLoadProjectFromWorkspace()
{
	switch(currentOperationStep)
	{
		case 0: // faza przygotowania (obliczanie postepu, clear itp)
			loadProjectFromWorkspaceInit(); break;
		case 1: // projekt
			loadProjectFileFromWorkspace(); break;
		case 2: // pattern
			loadPatternFromWorkspace(mtProject.values.actualPattern); break;
		case 3: // instruments
			loadInstrumentsFromWorkspace(); break;
		case 4: // samples
			loadSamplesFromWorkspace(); break;
		case 5: // wykonczenie
			loadProjectFromWorkspaceFinish(); break;
		default:
			status = fmLoadError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}

void cFileManager::updateSaveProjectToWorkspace()
{
	switch(currentOperationStep)
	{
		case 0: // faza przygotowania
			saveProjectToWorkspaceInit(); break;
		case 1: // projekt
			saveProjectFileToWorkspace(); break;
		case 2: // pattern
			savePatternToWorkspace(); break;
		case 3: // instruments
			saveInstrumentsToWorkspace(); break;
		case 4: // samples
			saveSamplesToWorkspace(); break;
		case 5: // wykonczenie
			saveProjectToWorkspaceFinish(); break;
		default:
			status = fmCopyError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}

void cFileManager::updateCopyProjectsToWorkspace()
{
	switch(currentOperationStep)
	{
		case 0: // faza przygotowania
			copyProjectsToWorkspaceInit(); break;
		case 1: // projekt
			copyProjectFile(); break;
		case 2: // pattern
			copyPaterns(); break;
		case 3: // instruments
			copyInstruments(); break;
		case 4: // samples
			copySamples(); break;
		case 5: // wykonczenie
			copyProjectsToWorkspaceFinish(); break;
		default:
			status = fmCopyError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}

void cFileManager::updateCopyWorkspaceToProjects()
{
	switch(currentOperationStep)
	{
		case 0: // faza przygotowania
			copyWorkspaceToProjectsInit(); break;
		case 1: // projekt
			copyProjectFile(); break;
		case 2: // pattern
			copyPaterns(); break;
		case 3: // instruments
			copyInstruments(); break;
		case 4: // samples
			copySamples(); break;
		case 5: // wykonczenie
			copyWorkspaceToProjectsFinish(); break;
		default:
			status = fmCopyError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}


void cFileManager::autoSaveProjectToWorkspace()
{
	if(autoSaveTimer < 10000) return;
	autoSaveTimer = 0;

	if(isProjectChanged())
	{
		saveProjectToWorkspace();
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
	currentOperation = fmLoadWorkspaceProject;

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
	currentOperation = fmCopyProjectsToWorkspace;

	return true;
}

bool cFileManager::saveProjectToWorkspace(bool forceSaveAll)
{
	if(forceSaveAll)
	{
		setAllChangeFlags();
	}
	else
	{
		if(!isProjectChanged())	return false; // nie zapisuj jesli nic nie jest zmodyfikowane
	}

	status = fmSavingProjectToWorkspace;
	currentOperationStep = 0;
	currentOperation = fmSaveWorkspaceProject;

	return true;
}

bool cFileManager::saveProjectToProjects()
{
	status = fmSavingProjectToProjects;
	currentOperationStep = 0;
	currentOperation = fmSaveWorkspaceProject;

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

void cFileManager::loadProjectFromWorkspaceInit()
{
	// tu czysci flagi zmian plikow bo podczas ladowania moga one zostac ustawione
	// jezeli np ladowany jest starsza wersja pliku i struktura jest modyfikwoana
	clearChangeFlags();

	calcTotalMemoryToTransfer();
	moveToNextOperationStep();
}


void cFileManager::loadProjectFromWorkspaceFinish()
{
	//projectEditor.loadProjectValues(); // jest odrazu po otwarciu pliku projektu, gdyz dalszy proces wymaga nie ktorych zmiennych

	// jezeli projekt byl potwierany z Projects i kopiwoany do workspace to
	// nadpisz nazwe projektu w pliku projektu na taka jak folderu z ktorego byl kopiwoany
	if(currentProjectName[0] == 0)
	{
		//	jezeli projekt jest otwierany z workspace na start urzadzenia to pobierz nazwe z pliku projektu
		strcpy(currentProjectName, projectNamefromProjectFile);
	}
	else
	{
		// jezeli jest otwierany z folderu Projects to wymys nazwe otwieranego projektu
		saveProjectFileToWorkspace();
		//writeProjectFile(cProjectFileNameInWorkspace, &mtProject);
	}

	strcpy(mtConfig.startup.lastProjectName, currentProjectName);
	sprintf(currentProjectPatch,"Projects/%s", currentProjectName);



	status = fmLoadEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//-------------------------------------------------------

void cFileManager::copyProjectsToWorkspaceInit()
{
	// przygotuj workspace
	clearWorkspace();
	createWorkspaceDirs();

	//
	moveToNextOperationStep();
}

void cFileManager::copyProjectsToWorkspaceFinish()
{
	status = fmIdle;					//
	currentOperation = fmNoOperation;	// takie cwaniactwo pozwala wywolac otwieranie projektu z workspace w tym miejscu

	if(!openProjectFromWorkspace())
	{
		createNewProjectInWorkspace();
		openProjectFromWorkspace();
	}

	status = fmLoadingProjectFromProjects; // wymuszenie statusu
}

//-------------------------------------------------------
void cFileManager::saveProjectToWorkspaceInit()
{

	moveToNextOperationStep();
}

void cFileManager::saveProjectToWorkspaceFinish()
{
	clearChangeFlags();

	status = fmSaveEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//-------------------------------------------------------
void cFileManager::copyWorkspaceToProjectsInit()
{

	moveToNextOperationStep();
}

void cFileManager::copyWorkspaceToProjectsFinish()
{

	status = fmSaveEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

void cFileManager::clearChangeFlags()
{
	chengesFlags.project = 0;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		chengesFlags.pattern[i] = 0;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		chengesFlags.instrument[i] = 0;
	}
}

bool cFileManager::isProjectChanged()
{
	if(chengesFlags.project) return true;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		if(chengesFlags.pattern[i]) return true;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		if(chengesFlags.instrument[i]) return true;
	}

	 return false;
}

void cFileManager::setAllChangeFlags()
{
	chengesFlags.project = 1;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		chengesFlags.pattern[i] = 1;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		chengesFlags.instrument[i] = 1;
	}
}


void cFileManager::setProjectStructChanged()
{
	chengesFlags.project = 1;
}

void cFileManager::setPatternStructChanged(uint8_t pattern)
{
	chengesFlags.pattern[pattern] = 1;
}

void cFileManager::setInstrumentStructChanged(uint8_t instrument)
{
	chengesFlags.instrument[instrument] = 1;
}










bool cFileManager::projectExist(char* name)
{
	char path[255];
	sprintf(path,cProjectsPathFormat,name);

	if(SD.exists(path))
	{
		return true;
	}

	return false;
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

