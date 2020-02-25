

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"


#include "mtSequencer.h"

#include "fileManager.h"


cFileManager newFileManager;



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

	case fmBrowseSamples: 				updateBrowseSamples(); 					break;
	case fmBrowseProjects: 				updateBrowseProjects(); 				break;



	}


}




//====================================================================================
//==========================     UPDATEs     =========================================
//====================================================================================
void cFileManager::updateLoadProjectFromWorkspace() // fmLoadWorkspaceProject - 1
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

void cFileManager::updateSaveProjectToWorkspace() // fmSaveWorkspaceProject - 2
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
			status = fmSaveError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}

void cFileManager::updateCopyProjectsToWorkspace() // fmCopyProjectsToWorkspace - 3
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

void cFileManager::updateCopyWorkspaceToProjects() // fmCopyWorkspaceToProjects - 4
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


void cFileManager::updateBrowseSamples() //fmBrowseSamples - 5
{
	switch(currentOperationStep)
	{
		case 0: browseCurrentLocation();  break;
		case 1: processDirFileSizes(); break;

		default:
			status = fmBrowseError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}

void cFileManager::updateBrowseProjects() //fmBrowseProjects - 6
{
	switch(currentOperationStep)
	{
		//case 0: browseCurrentLocation();  break;
		//case 1: processDirFileSizes(); break;

		default:
			status = fmBrowseError;
			currentOperationStep = 0;
			currentOperation = fmNoOperation;
			break;
	}
}

void cFileManager::autoSaveProjectToWorkspace()
{
	if(autoSaveTimer < 10000) return;
	autoSaveTimer = 0;

	saveProjectToWorkspace();

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
	if(status != fmIdle) return false;

	if(forceSaveAll)
	{
		setAllChangeFlags();
	}
	else
	{
		if(!isProjectChanged())	return false; // nie sejwuj jesli nic nie jest zmodyfikowane
	}

	report("autosave");

	status = fmSavingProjectToWorkspace;
	currentOperationStep = 0;
	currentOperation = fmSaveWorkspaceProject;

	return true;
}

bool cFileManager::saveProjectToProjects(char* projectNameToSave)
{
	if(status != fmIdle) return false;

	if(projectNameToSave != nullptr)
	{
		strcpy(currentProjectName, projectNameToSave);
	}


	saveProjectToWorkspace(true);

	status = fmSavingProjectToProjects;
//	currentOperationStep = 0;
//	currentOperation = fmSaveWorkspaceProject;
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
		changesFlags.project = 1;
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

	if(status == fmSavingProjectToProjects)
	{
		currentOperationStep = 0;
		currentOperation = fmCopyWorkspaceToProjects;
		return;
	}

	status = fmSaveEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//-------------------------------------------------------
void cFileManager::copyWorkspaceToProjectsInit()
{
	char projectSavePath[PATCH_SIZE];

	if(!SD.exists(cProjectsPath))
	{
		SD.mkdir(0, cProjectsPath);
	}

	sprintf(projectSavePath, cProjectsPathFormat, currentProjectName);
	if(!SD.exists(projectSavePath))
	{
		SD.mkdir(0, projectSavePath);
	}

	sprintf(projectSavePath, cProjectsPatternsPathFormat, currentProjectName);
	if(!SD.exists(projectSavePath))
	{
		SD.mkdir(0, projectSavePath);
	}

	sprintf(projectSavePath, cProjectsInstrumentsPathFormat, currentProjectName);
	if(!SD.exists(projectSavePath))
	{
		SD.mkdir(0, projectSavePath);
	}

	sprintf(projectSavePath, cProjectsSamplesPathFormat, currentProjectName);
	if(!SD.exists(projectSavePath))
	{
		SD.mkdir(0, projectSavePath);
	}

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
	changesFlags.project = 0;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		changesFlags.pattern[i] = 0;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		changesFlags.instrument[i] = 0;
	}
}

bool cFileManager::isProjectChanged()
{
	if(changesFlags.project) return true;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		if(changesFlags.pattern[i]) return true;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		if(changesFlags.instrument[i]) return true;
	}

	 return false;
}

void cFileManager::setAllChangeFlags()
{
	changesFlags.project = 1;
	for(uint8_t i = 0 ; i< PATTERN_INDEX_MAX; i++)
	{
		changesFlags.pattern[i] = 1;
	}
	for(uint8_t i = 0 ; i< INSTRUMENTS_COUNT; i++)
	{
		changesFlags.instrument[i] = 1;
	}
}


void cFileManager::setProjectStructChanged()
{
	changesFlags.project = 1;
}

void cFileManager::setPatternStructChanged(uint8_t pattern)
{
	changesFlags.pattern[pattern] = 1;
}

void cFileManager::setInstrumentStructChanged(uint8_t instrument)
{
	changesFlags.instrument[instrument] = 1;
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


	status = fmError;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

void cFileManager::report(const char* text, uint8_t value)
{
#ifdef DEBUG
	debugLog.setMaxLineCount(10);
	if(value > 0) sprintf(errorText,  "File manager: %s (%d)", text, value);
	else sprintf(errorText,  "File manager: %s", text);
	debugLog.addLine(errorText);
	debugLog.forceRefresh();
#endif
}
