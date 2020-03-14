

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"
#include "mtConfig.h"
#include "debugLog.h"

#include "mtExporterWAV.h"
#include "mtSequencer.h"

#include "fileManager.h"
#include "fileTransfer.h"

extern int16_t sdram_sampleBank[4*1024*1024];

cFileManager newFileManager;

SdDir sdLocation;
SdFile wavfile;

elapsedMillis autoSaveTimer;

//==========================================================================================================
//==========================================================================================================
//==========================================================================================================
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
	case fmBrowseFirmwares: 			updateBrowseFirmwares(); 				break;

	case fmImportSamplesToWorkspace:	updateImportSamplesToWorkspace(); 		break;
	case fmCopyInstrumentsInWorkspace:	updateCopyInstrumentsInWorkspace(); 	break;
	case fmPreviewSamplesFromSD:												break;
	case fmDeleteInstruments:			updateDeleteInstruments(); 				break;

	case fmLoadWorkspacePattern:		updateLoadWorkspacePattern(); 			break;

	case fmExportSound: 				updateExportSound();					break;
	case fmSaveRecordedSound: 			updateSaveRecordedSound();				break;

	case fmImportSampleFromSampleEditor:updateImportSampleFromSampleEditor(); break;


	default: break;
	}


}




//==========================================================================================================
//==========================     UPDATEs     ===============================================================
//==========================================================================================================
void cFileManager::updateLoadProjectFromWorkspace() // fmLoadWorkspaceProject - 1
{
	switch(currentOperationStep)
	{
		case 0:		loadProjectFromWorkspaceInit(); 		break;// faza przygotowania (obliczanie postepu, clear itp)
		case 1:		loadProjectFileFromWorkspace(); 		break; // projekt
		case 2:		loadPatternFromWorkspace(mtProject.values.actualPattern); break;// pattern
		case 3:		loadInstrumentsFromWorkspace(); 		break; // instruments
		case 4:		loadSamplesFromWorkspace();				break; // samples
		case 5:		loadProjectFromWorkspaceFinish(); 		break;// wykonczenie
		default:	stopOperationWithError(fmLoadError); 	break;
	}
}

void cFileManager::updateSaveProjectToWorkspace() // fmSaveWorkspaceProject - 2
{
	switch(currentOperationStep)
	{
		case 0:		saveProjectToWorkspaceInit(); 			break;
		case 1:		savePatternToWorkspace(); 				break;
		case 2:		saveInstrumentsToWorkspace(); 			break;
		case 3:		saveProjectToWorkspaceContinue();		break;
		case 4:		saveProjectFileToWorkspace(); 			break; // projekt przesunięty na koniec
		case 5:		saveProjectToWorkspaceFinish(); 		break;
		default:	stopOperationWithError(fmSaveError); 	break;
	}
}

void cFileManager::updateCopyProjectsToWorkspace() // fmCopyProjectsToWorkspace - 3
{
	switch(currentOperationStep)
	{
		case 0:		copyProjectsToWorkspaceInit();		 	break;
		case 1:		copyProjectFile(); 						break;
		case 2:		copyPaterns(); 							break;
		case 3:		copyInstruments(); 						break;
		case 4:		copySamples(); 							break;
		case 5:		copyProjectsToWorkspaceFinish(); 		break;
		default:	stopOperationWithError(fmCopyError); 	break;
	}
}

void cFileManager::updateCopyWorkspaceToProjects() // fmCopyWorkspaceToProjects - 4
{
	switch(currentOperationStep)
	{
		case 0:		copyWorkspaceToProjectsInit(); 			break;
		case 1:		copyProjectFile();						break;
		case 2:		copyPaterns(); 							break;
		case 3:		copyInstruments(); 						break;
		case 4:		copySamples(); 							break;
		case 5:		copyWorkspaceToProjectsFinish(); 		break;
		default:	stopOperationWithError(fmCopyError); 	break;
	}
}


void cFileManager::updateBrowseSamples() //fmBrowseSamples - 5
{
	switch(currentOperationStep)
	{
		case 0: 	browseCurrentLocation();  						break;
		case 1: 	listOnlyWavFromActualPath();					break;
		case 2: 	processDirFileSizes(); 							break;
		default:	stopOperationWithError(fmBrowseSamplesError); 	break;
	}
}


void cFileManager::updateBrowseProjects() //fmBrowseProjects - 6
{
	switch(currentOperationStep)
	{
		case 0: 	browseProjectsLocation();  						break;
		default:	stopOperationWithError(fmBrowseProjectsError); 	break;
	}
}

void cFileManager::updateBrowseFirmwares() //fmBrowseFirmwares - 7
{
	switch(currentOperationStep)
	{
		case 0: 	browseFirmwaresLocation();  					break;
		default:	stopOperationWithError(fmBrowseFirmwaresError); break;
	}
}

void cFileManager::updateImportSamplesToWorkspace()	//fmImportSamplesToWorkspace - 8
{
	switch(currentOperationStep)
	{
		case 0:		importSamplesToWorkspaceInit(); 						break;
		case 1:		copySamples();											break;
		case 2:		createEmptyInstrumentInWorkspace();						break;
		case 3:		importSamplesToWorkspaceContinue(); 					break;
		case 4:		moveSampleMemory();										break;
		case 5:		loadInstrumentsFromWorkspace(); 						break; // instruments
		case 6:		loadSamplesFromWorkspace();								break; // samples
		case 7:		importSamplesToWorkspaceFinish();						break;
		default:	stopOperationWithError(fmImportSamplesError); 			break;
	}
}


void cFileManager::updateCopyInstrumentsInWorkspace()	//fmCopyInstrumentsInWorkspace - 9
{
	switch(currentOperationStep)
	{
		case 0:		copyInstrumentsInWorkspaceInit(); 						break;
		case 1:		copySamples();											break;
		case 2:		createEmptyInstrumentInWorkspace();						break;
		case 3:		copyInstrumentsInWorkspaceContinue(); 					break;
		case 4:		moveSampleMemory();										break;
		case 5:		loadInstrumentsFromWorkspace(); 						break; // instruments
		case 6:		loadSamplesFromWorkspace();								break; // samples
		case 7:		copyInstrumentsInWorkspaceFinish();						break;
		default:	stopOperationWithError(fmImportSamplesError); 			break;
	}
}

void cFileManager::updateDeleteInstruments() //fmDeleteInstruments - 11
{
	switch(currentOperationStep)
	{
		case 0: 	deleteInstrumentsFromWorkspace();  					break;
		case 1:		deleteSamplesFromWorkspace();						break;
		case 2: 	moveSampleMemory();									break;
		case 3:		deleteInstrumentsFromWorkspaceFinish();				break;
		default:	stopOperationWithError(fmDeleteInstrumentsError); 	break;
	}
}

void cFileManager::updateLoadWorkspacePattern() //fmLoadWorkspacePattern - 12
{
	switch(currentOperationStep)
	{
		case 0: 	savePatternToWorkspace();								 	break;
		case 1: 	loadPatternFromWorkspace(currentPattern); 					break;
		case 2:		loadPatternFromWorkspaceFinish();							break;
		default:	stopOperationWithError(fmLoadPatternError); 				break;
	}
}

void cFileManager::updateExportSound() //fmExportSound - 13
{
	switch(currentOperationStep)
	{
		case 0: 	exportSoundGetStatus(); 									break;
		case 1:		exportSoundEnd();											break;
		default:	stopOperationWithError(fmExportSoundError); 				break;
	}
}

void cFileManager::updateSaveRecordedSound() //  //fmSaveRecordedSound - 14
{
	switch(currentOperationStep)
	{
		case 0: 	saveRecording(); 										break;
		case 1:		copySamples();											break;
		case 2:		createEmptyInstrumentInWorkspace();						break;
		case 3:		importSamplesToWorkspaceContinue(); 					break;
		case 4:		moveSampleMemory();										break;
		case 5:		loadInstrumentsFromWorkspace(); 						break;
		case 6:		loadSamplesFromWorkspace();								break;
		case 7:		saveRecordedSoundFinish();								break;
		default:	stopOperationWithError(fmSaveRecordedError); 			break;
	}
}

void cFileManager::updateImportSampleFromSampleEditor() //  //fmImportSampleFromSampleEditor - 15
{
	switch(currentOperationStep)
	{
		case 0: 	importSampleFromSampleEditorInit(); 							break;
		case 1: 	saveSamplesToWorkspace(); 										break;
		case 2:		moveSampleMemory();												break;
		case 3:		importSampleFromSampleEditorFinish();							break;
		default:	stopOperationWithError(fmImportSampleFromSampleEditorError); 	break;
	}
}



void cFileManager::autoSaveProjectToWorkspace()
{
	if(autoSaveTimer < 10000) return;
	autoSaveTimer = 0;

	saveDebugLogToSd();

	saveProjectToWorkspace();

}

void cFileManager::moveToNextOperationStep()
{
	currentOperationStep++;
}


void cFileManager::skipNextOperationStep()
{
	currentOperationStep+=2;
}


void cFileManager::stopOperationWithError(uint8_t error)
{
	status = error;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------loadProjectFromWorkspace
void cFileManager::loadProjectFromWorkspaceInit()
{
	// tu czysci flagi zmian plikow bo podczas ladowania moga one zostac ustawione
	// jezeli np ladowany jest starsza wersja pliku i struktura jest modyfikwoana
	clearChangeFlags();

	currentInstrument = 0;
	currentSample = 0;


	//todo to tu nie dziala bo ladowanie instrumentow nadpisuje to
	mtProject.instrument[0].sample.length = 0;
	mtProject.instrument[0].sample.address = sdram_sampleBank;
	//lastActiveInstrument = 0;

	mtProject.used_memory = 0;


	calcTotalMemoryToTransfer();
	moveToNextOperationStep();
}


void cFileManager::loadProjectFromWorkspaceFinish()
{
	if(currentInstrument < INSTRUMENTS_COUNT-1)
	{
		currentInstrument++;
		currentSample++;

		currentOperationStep = 3; //xxx najwazniejsze !
		return;
	}

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
		changesFlags.project = 0;
	}

	strcpy(mtConfig.startup.lastProjectName, currentProjectName);
	sprintf(currentProjectPatch,"Projects/%s", currentProjectName);

	sequencer.switchRamPatternsNow();

	// swiezo wczytany projekt jest zawsze zgodny z zapisanym - nieeee?!
	//mtProject.values.projectNotSavedFlag = 0;


	status = fmLoadEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//------------------------------------------------------------------------------------------copyProjectsToWorkspace

void cFileManager::copyProjectsToWorkspaceInit()
{
	// przygotuj workspace
	clearWorkspace();
	createWorkspaceDirs();

	currentInstrument = 0;
	currentSample = 0;

	//
	moveToNextOperationStep();
}

void cFileManager::copyProjectsToWorkspaceFinish()
{
	if(currentInstrument < INSTRUMENTS_COUNT-1)
	{
		currentInstrument++;
		currentSample++;

		currentOperationStep = 3; //xxx najwazniejsze !
		return;
	}


	status = fmIdle;					// takie cwaniactwo pozwala wywolac otwieranie
	currentOperation = fmNoOperation;	// projektu z workspace w tym miejscu

	if(!openProjectFromWorkspace())
	{
		// wrazie by sie nie dalo otworzyc projektu
		// to otworz nowy pusty
		createNewProjectInWorkspace();
		openProjectFromWorkspace();
	}

	status = fmLoadingProjectFromProjects; // wymuszenie statusu
}

//-------------------------------------------------------------------------------------------saveProjectToWorkspace
void cFileManager::saveProjectToWorkspaceInit()
{
	currentInstrument = 0;
	setCurrentInstrumentToFirstActiveAfterCurrent();

	moveToNextOperationStep();
}

void cFileManager::saveProjectToWorkspaceContinue()
{
	if(currentInstrument < INSTRUMENTS_COUNT-1)
	{
		currentInstrument++;
		currentSample++;

		currentOperationStep = 2; //xxx najwazniejsze !
		return;
	}

	moveToNextOperationStep();
}

void cFileManager::saveProjectToWorkspaceFinish()
{
	//clearChangeFlags(); // wykomentowane - czysci flagi na bierząco

	debugLog.setMaxLineCount(9);
	debugLog.addLine("autosave end");

	if(status == fmSavingProjectToProjects) // przejdz do kopiowania projektu
	{
		currentOperationStep = 0;
		currentOperation = fmCopyWorkspaceToProjects;
		return;
	}


	status = fmIdle; //xxx zamiast fmSaveEnd
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//-----------------------------------------------------------------------------------------copyWorkspaceToProjects
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

	currentInstrument = 0;
	currentSample = 0;

	moveToNextOperationStep();
}

void cFileManager::copyWorkspaceToProjectsFinish()
{
	if(currentInstrument < INSTRUMENTS_COUNT-1)
	{
		currentInstrument++;
		currentSample++;

		currentOperationStep = 3; //xxx najwazniejsze
		return;
	}

	// po stworzeniu aktualnej wersji projektu w Project czysci flage zmian
	mtProject.values.projectNotSavedFlag = 0;

	status = fmSaveEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}
//----------------------------------------------------------------------------------------importSamplesToWorkspace
void cFileManager::importSamplesToWorkspaceInit()
{
	// wielkosc nowych sampli importowanych bedzie naliczana przy kopiowaniu do worspace
	newSamplesSize = 0;
	oldSamplesSize = 0;

	// zapamietanie zajmowanej pamieci przez nadpisywane instrumenty
	// + kasowanie zuzycia pamieci przez te instrumenty
	for(uint8_t instr = importStartSlot; instr <= importEndSlot; instr++)
	{
		if(mtProject.instrument[instr].isActive == 1)
		{
			oldSamplesSize += mtProject.instrument[instr].sample.length*2;
			mtProject.used_memory -= mtProject.instrument[instr].sample.length*2;
		}

		// wykomentowane bo pewnie jakies dane potrzebna beda w dalszych krokach wiec lepiej nie zerowac
//		setDefaultActiveInstrument(&mtProject.instrument[instr]);
//		mtProject.instrument[instr].sample.file_name[0] = 0;
//		mtProject.instrument[instr].isActive = 0;
	}

	moveToNextOperationStep();
}

void cFileManager::importSamplesToWorkspaceContinue()
{
	importCurrentFile++;

	newSamplesSize +=  fileTransfer.getConvertedSampleSize();
	fileTransfer.resetConvertedSampleSize();

	if(currentSample < importEndSlot  && currentSample < INSTRUMENTS_COUNT && importCurrentFile < explorerListLength)
	{
		currentInstrument++;
		currentSample++;

		currentOperationStep = 1; //xxx najwazniejsze !
		return;
	}
	else
	{
		currentInstrument = importStartSlot;
		currentSample = importStartSlot;
	}

	moveToNextOperationStep();
}

void cFileManager::importSamplesToWorkspaceFinish()
{

	if(currentInstrument < importEndSlot && currentInstrument < INSTRUMENTS_COUNT-1)
	{
		currentInstrument++;
		currentSample++;

		currentOperationStep = 5; //xxx najwazniejsze !
		return;
	}

	status = fmImportSamplesEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}
//---------------------------------------------------------------------------------------- copyInstrumentsInWorkspace
void cFileManager::copyInstrumentsInWorkspaceInit()
{
	// obliczenie jaka wielkosc maja kopiowane sample
	newSamplesSize = mtProject.instrument[copySrcSlot].sample.length*2;
	oldSamplesSize = 0;

	// zwolnienie zajmowanej przez naspisywany instrument pamieci
	// + obliczenie wielksoci starych nadpisywanych  sampli
	if(mtProject.instrument[copyDestSlot].isActive == 1)
	{
		oldSamplesSize += mtProject.instrument[copyDestSlot].sample.length*2;
		mtProject.used_memory -= mtProject.instrument[copyDestSlot].sample.length*2;
	}

	moveToNextOperationStep();
}

void cFileManager::copyInstrumentsInWorkspaceContinue()
{
//	if(currentSample < importEndSlot  && currentSample < INSTRUMENTS_COUNT && importCurrentFile < explorerListLength)
//	{
//		currentInstrument++;
//		currentSample++;
//
//		currentOperationStep = 1; //xxx najwazniejsze !
//		return;
//	}
//	else
	{
		currentInstrument = copyDestSlot;
		currentSample = copyDestSlot;
	}

	moveToNextOperationStep();
}

void cFileManager::copyInstrumentsInWorkspaceFinish()
{
//	if(currentInstrument < importEndSlot && currentInstrument < INSTRUMENTS_COUNT-1)
//	{
//		currentInstrument++;
//		currentSample++;
//
//		currentOperationStep = 5; //xxx najwazniejsze !
//		return;
//	}

	status = fmCopyingInstrumentsEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//---------------------------------------------------------------------------------------- deleteInstrumentsFromWorkspace
void cFileManager::deleteInstrumentsFromWorkspaceFinish()
{
	// clear project struct - dopiero na koncu po przesówaniu pamieci
	for(uint8_t instr = currentInstrument; instr <= deleteEndInstrument; instr++)
	{
		if(mtProject.instrument[instr].isActive == 1)
		{
			mtProject.used_memory -= mtProject.instrument[instr].sample.length*2;
		}

		setDefaultActiveInstrument(&mtProject.instrument[instr]);
		mtProject.instrument[instr].sample.file_name[0] = 0;
		mtProject.instrument[instr].isActive = 0;
	}



	status = fmDeleteInstrumentsEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}

//---------------------------------------------------------------------------------------- loadPatternFromWorkspace
void cFileManager::loadPatternFromWorkspaceFinish()
{
	if(status == fmExportingSoundSong || status == fmExportingSoundSongStems)
	{
		currentOperationStep = 0;
		currentOperation = fmExportSound;
		return;
	}

	status = fmIdle;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
}



void cFileManager::importSampleFromSampleEditorInit()
{
	newSamplesSize = importFromSampleEditorLength;
	oldSamplesSize = mtProject.instrument[importStartSlot].sample.length*2;

	moveToNextOperationStep();
}

void cFileManager::importSampleFromSampleEditorFinish()
{
	mtProject.used_memory -= (mtProject.instrument[importStartSlot].sample.length*2 > mtProject.used_memory)
							 ? mtProject.used_memory
							 : mtProject.instrument[importStartSlot].sample.length*2;

	mtProject.used_memory += importFromSampleEditorLength;


	mtProject.instrument[importStartSlot].sample.length = importFromSampleEditorLength/2;
	memcpy(mtProject.instrument[importStartSlot].sample.address, importFromSampleEditorAddress, importFromSampleEditorLength);


	status = fmImportSampleFromSampleEditorEnd;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;
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

	if(projectsList[index] == nullptr) return false;
	//pobranie nazwy otwieranego projektu tu
	strcpy(currentProjectName, projectsList[index]);

	status = fmLoadingProjectFromProjects;
	currentOperationStep = 0;
	currentOperation = fmCopyProjectsToWorkspace;

	return true;
}

bool cFileManager::saveProjectToWorkspace(bool forceSaveAll) //xxx jak narazie nigdzie nie uzywane force=true
{
	if(status != fmIdle && status != fmSavingProjectToProjects) return false;
	if(currentOperation != fmNoOperation) return false;

	if(forceSaveAll)
	{
		setAllChangeFlags();
	}
	else
	{
		if(!isProjectChanged())	return false; // nie sejwuj jesli nic nie jest zmodyfikowane
	}

	debugLog.setMaxLineCount(9);
	debugLog.addLine("autosave start");


	if(status == fmSavingProjectToProjects)
	{
		// jezeli autozapis przed zapisaniem do Projects to finalnie wszsytko bedzie aktualne
		mtProject.values.projectNotSavedFlag = 0;
	}
	else
	{
		// jezeli typowy autozapis to workspace napewo sie rozni od zawartosci Projects
		mtProject.values.projectNotSavedFlag |= 1;
	}

	status = fmSavingProjectToWorkspace;
	currentOperationStep = 0;
	currentOperation = fmSaveWorkspaceProject;

	return true;
}

bool cFileManager::saveProjectToProjects(char* projectNameToSave)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	//zmien nazwe otwartego projektu w pamieci i w workspace
	if(projectNameToSave != nullptr)
	{
		strcpy(currentProjectName, projectNameToSave);
	}

	// wymus zapis projektu z nowa nazwa w workspace
	changesFlags.project = 1;

	status = fmSavingProjectToProjects;

	if(saveProjectToWorkspace(false))  // zapisuj workpace tlyko jesli sa flagi zmian
	{
		status = fmSavingProjectToProjects;
		currentOperationStep = 0;
		currentOperation = fmSaveWorkspaceProject;
		return true;
	}

	status = fmSavingProjectToProjects;
	currentOperationStep = 0;
	currentOperation = fmCopyWorkspaceToProjects;//fmSaveWorkspaceProject;
	return true;
}

bool cFileManager::importSamplesToProject(uint8_t fileFrom, uint8_t fileTo, uint8_t instrumentSlot)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	//sprawdzic czy zaznaczone wartosci / nazwy sa poprawne xxx

	importCurrentFile = fileFrom;

	currentInstrument = instrumentSlot;
	currentSample = instrumentSlot;
	importStartSlot = instrumentSlot;
	importEndSlot = instrumentSlot + ((fileTo>fileFrom) ? (fileTo-fileFrom) : 0);
	if(importEndSlot >= INSTRUMENTS_COUNT) importEndSlot = INSTRUMENTS_COUNT-1; //xxx
	//findLastActiveInstrumentBeforeCurrent();

	// oblicz od ktorego instrumentu trzeba bedzie przesunac pamiec
	// potem zostanie obliczony offset o jaki trzeba bedzie przesunąć
	// jesli 48 to znaczy ze nie ma sampli do przesuniecia
	calcFirstSlotToMoveInMemory(importEndSlot+1);

	// sprawdzenie czy wystarczy pamieci xxx

	status = fmImportingSamplesToWorkspace;
	currentOperationStep = 0;
	currentOperation = fmImportSamplesToWorkspace;
	return true;
}


// narazie kopiowanie dziala tylko na 1 instrument
bool cFileManager::copyInstrumentsInWorkspace(uint8_t copyInstrSrc, uint8_t copyInstrCount, uint8_t copyInstrDest)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	if(!mtProject.instrument[copyInstrSrc].isActive) return false;

	currentInstrument = copyInstrDest;
	currentSample = copyInstrDest;

	copySrcSlot = copyInstrSrc;
	copyDestSlot = copyInstrDest;
	copySlotsCount = copyInstrCount;

	calcFirstSlotToMoveInMemory(copyInstrDest+1);

	status = fmCopyingInstrumentsInWorkspace;
	currentOperationStep = 0;
	currentOperation = fmCopyInstrumentsInWorkspace;
	return true;
}



bool cFileManager::deleteInstruments(uint8_t instrumentSlotFrom, uint8_t instrumentSlotTo)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	currentInstrument = instrumentSlotFrom;
	currentSample = instrumentSlotFrom;
	deleteEndInstrument = instrumentSlotTo;
	deleteEndSample = instrumentSlotTo;

	// oblicz od ktorego instrumentu trzeba bedzie przesunac pamiec
	// potem zostanie obliczony offset o jaki trzeba bedzie przesunąć
	// jesli 48 to znaczy ze nie ma sampli do przesuniecia
	calcFirstSlotToMoveInMemory(instrumentSlotTo+1);


	status = fmDeleteingInstruments;
	currentOperationStep = 0;
	currentOperation = fmDeleteInstruments;
	return true;
}



bool cFileManager::loadWorkspacePattern(uint8_t index)
{
	// to wyjatek który moze byc wywolywany podczas exportu songa
	if(status != fmIdle
			&& status != fmExportingSoundSong
			&& status != fmExportingSoundSongStems
			&& status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation
			&& currentOperation != fmExportSound
			&& currentOperation != fmSaveWorkspaceProject) return false;

	currentPattern = index;

	if(status != fmExportingSoundSong && status != fmExportingSoundSongStems)
	{
		status = fmLoadingPatternFromWorkspace;
	}

	currentOperationStep = 0;
	currentOperation = fmLoadWorkspacePattern;
	return true;
}

bool cFileManager::deleteProject(uint8_t index)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;

	if(index >= projectsListLength) return false;
	if(strcmp(projectsList[index], currentProjectName) == 0) return false;

	char project_dir[255];
	sprintf(project_dir, cProjectsPathFormat, projectsList[index]);

	SD.removeDirWithFiles(project_dir);

	return true;
}

bool cFileManager::importSampleFromSampleEditor(int16_t* memoryAddres, uint32_t length, uint8_t instrumentSlot)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;


	currentInstrument = instrumentSlot;
	currentSample = instrumentSlot;
	importStartSlot = instrumentSlot;
	importEndSlot = instrumentSlot;

	importFromSampleEditorAddress = (uint8_t*)memoryAddres;
	importFromSampleEditorLength = length*2;

	calcFirstSlotToMoveInMemory(importEndSlot+1);

	status = fmImportingSampleFromSampleEditor;
	currentOperationStep = 0;
	currentOperation = fmImportSampleFromSampleEditor;
	return true;
}


//-----------------------------------------------------------------------------------------------------
//-------------------------------------   FLAGI ZMIAN   -----------------------------------------------
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


void cFileManager::clearProjectChangedFlag()
{
	changesFlags.project = 0;
}

void cFileManager::clearPatternChanged(uint8_t pattern)
{
	changesFlags.pattern[pattern] = 0;
}

void cFileManager::clearInstrumentChanged(uint8_t instrument)
{
	changesFlags.instrument[instrument] = 0;
}




//-----------------------------------------------------------------------------------------------------
//-------------------------------------                 -----------------------------------------------
//-----------------------------------------------------------------------------------------------------



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
//----------------------------------------   POSTEP   -------------------------------------------------
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


//-----------------------------------------------------------------------------------------------------
//-------------------------------------------     ERRORS     ------------------------------------------
//-----------------------------------------------------------------------------------------------------


#ifdef DEBUG
static char errorText[100];
#endif

void cFileManager::throwError(uint8_t source)
{
#ifdef DEBUG
	debugLog.setMaxLineCount(9);
	sprintf(errorText,  "File manager error (%d)(%d)(%d)", currentOperation, currentOperationStep, source);
	debugLog.addLine(errorText);
	debugLog.forceRefresh();
#endif

	currentSample = 0;
	currentInstrument = 0;
	currentPattern = 0;

	status = fmError;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;

}

void cFileManager::report(const char* text, uint8_t value)
{
#ifdef DEBUG
	debugLog.setMaxLineCount(9);
	if(value > 0) sprintf(errorText,  "File manager: %s (%d)", text, value);
	else sprintf(errorText,  "File manager: %s", text);
	debugLog.addLine(errorText);
	debugLog.forceRefresh();
#endif
}
