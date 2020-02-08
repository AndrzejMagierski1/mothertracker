#include <mtRandomNameGenerator.h>
#include "mtSliceDetector.h"


#include "projectEditor/projectEditor.h"
#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "mtLED.h"
#include "mtExporterWAV.h"
#include "mtPadBoard.h"

#include "sampleRecorder/sampleRecorder.h"

#include "performanceMode/performanceMode.h"

#include "sdCardDetect.h"
#include "mtFileManager.h"



cProjectEditor projectEditor;
cProjectEditor* PE = &projectEditor;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static uint8_t functSdCard(uint8_t state);

//static uint8_t functShowProjectsList();
//static uint8_t functShowTemplatesList();
//static uint8_t functCancelList();
//static uint8_t functOpenTemplate();
//static uint8_t functCreateNewTemplate();
//****************************************************
//Nowe podejście - ekran główny
static uint8_t functNewProject();
static uint8_t functOpenProject();
static uint8_t functSaveProject();
static uint8_t functSaveAsProject();
static uint8_t functExport();
//****************************************************
//New project
static uint8_t functSaveChangesCancelNewProject();
static uint8_t functSaveChangesDontSaveNewProject();
static uint8_t functSaveChangesSaveNewProject();
//****************************************************
//Save As
static uint8_t functSaveAsCancel();
static uint8_t functSaveAsConfirm();
static uint8_t functSaveAsAutoName();
static uint8_t functSaveAsOverwriteYes();
static uint8_t functSaveAsOverwriteNo();
//****************************************************
//Open
static uint8_t functOpenProjectConfirm();
static uint8_t functSaveChangesCancelOpen();
static uint8_t functSaveChangesDontSaveOpen();
static uint8_t functSaveChangesSaveOpen();
static uint8_t functProjectListUp();
static uint8_t functProjectListDown();
//****************************************************
//Export
static uint8_t functExportSong();
static uint8_t functExportSongStems();
static uint8_t functExportPattern();
static uint8_t functExportPatternStems();
static uint8_t functExportToMOD();
static uint8_t functExportCancel();
static uint8_t functExportGoBack();
//****************************************************
static uint8_t functSwitchModule(uint8_t button);


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();
static  uint8_t functConfirmKey();

static uint8_t functDeleteBackspace(uint8_t state);

static  uint8_t functEncoder(int16_t value);


static uint8_t functDelete();
static uint8_t functDeleteConfirm();


static uint8_t functStartGameModule()
{
	if(PE->isBusyFlag) return 1;
	PE->eventFunct(eventActivateGameModule,PE,0,0);
	return 1;
}


void cProjectEditor::update()
{
	refreshProcessingPopup();

	if(openInProgressFlag || createNewProjectFlag)
	{
		uint8_t loadStatus = fileManager.getLoadingStatus();

		if(loadStatus)
		{
			fileManager.refreshProjectOpening();
		}
		else
		{
			loadProjectValues();

			hideProcessingPopup();

			showDefaultScreen();
			setDefaultScreenFunct();
			openInProgressFlag = 0;
			createNewProjectFlag = 0;
			isBusyFlag = 0;
		}
	}

	if(saveInProgressFlag)
	{
		uint8_t saveStatus = fileManager.getSavingStatus();

		if(saveStatus)
		{
			fileManager.refreshSaveProject();
		}
		else
		{
			saveInProgressFlag = 0;
			showDefaultScreen();
			hideProcessingPopup();

			isBusyFlag = 0;

			if(newProjectOnSaveEndFlag)
			{
				newProjectOnSaveEndFlag = 0;
				functNewProject();
			}
			if(openOnSaveEndFlag)
			{
				openOnSaveEndFlag = 0;
				functOpenProjectConfirm();
			}

			setDefaultScreenFunct();
		}
	}

	if(deleteInProgressFlag)
	{
		uint8_t deleteStatus = fileManager.getDeletingStatus();

		if(deleteStatus)
		{
			fileManager.refreshDeleting();
		}
		else
		{
			deleteInProgressFlag = 0;
			showDefaultScreen();
			hideProcessingPopup();

			isBusyFlag = 0;

			setDefaultScreenFunct();
		}
	}

	if(exportInProgress)
	{
		currentExportState = exporter.getState();
		uint8_t localProgress = exporter.getProgress();
		if(localProgress >  exportProgress )
		{
			exportProgress = localProgress;
		}
		showExportingHorizontalBar();
		if((currentExportState == 0) && (lastExportState != currentExportState))
		{
			isBusyFlag = 0;
			exportInProgress = 0;
			showExportWindow();
		}

		lastExportState = currentExportState;
	}

	if(newProjectPopupDelay > 200)
	{
		if(newProjectPopupFlag)
		{
			newProjectPopupFlag = 0;
			fileManager.openProjectStart((char*)"New",projectTypeExample);
			createNewProjectFlag = 1;
		}
	}

	if(savePopupFlag)
	{
		if(savePopupDelay > 200)
		{
			savePopupFlag = 0;
			fileManager.startSaveProject();
			saveInProgressFlag = 1;
		}
	}

	if(openPopupFlag)
	{
		if(openPopupDelay > 200)
		{
			openPopupFlag = 0;
			fileManager.openProjectStart(&PE->locationFilesList[PE->selectedLocation][0],projectTypeUserMade);
			openInProgressFlag = 1;
		}
	}

	if(newProjectPopupFlag)
	{
		if(newProjectPopupDelay > 200)
		{
			newProjectPopupFlag = 0;
			fileManager.openProjectStart((char*)"New",projectTypeExample);
			createNewProjectFlag = 1;
		}
	}

	if(deletePopupFlag)
	{
		if(deletePopupDelay > 200)
		{
			deletePopupFlag = 0;
			fileManager.deleteProjectStart(&PE->locationFilesList[PE->selectedLocation][0]);
			deleteInProgressFlag = 1;
		}
	}

	if(refreshCover)
	{
		if(refreshCover == 1 && coverDelay > 2000)
		{
			char path[PATCH_SIZE];
			sprintf(path,"Projects/%s/cover", projectCoverName);

			if(display.readImgFromSd(path,670000,400,400))
			{
				refreshCover = 0;
			}
			else refreshCover = 2;
		}
		else if(refreshCover == 2)
		{
			if(display.isImgLoaded())
			{
				display.setControlShow(coverImg);
				display.refreshControl(coverImg);
				refreshCover = 0;
			}
		}
	}

}

void cProjectEditor::start(uint32_t options)
{

	selectedLocation = 0;

	moduleRefresh = 1;

	keyboardManager.init(keyboardControl,editName);

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);


	if(!sdCardDetector.isCardInserted())
	{
		functSdCard(0);
		return;
	}

	showDefaultScreen();
	setDefaultScreenFunct();
}

void cProjectEditor::stop()
{
	keyboardManager.deinit();
	moduleRefresh = 0;
	refreshCover = 0;
}

void cProjectEditor::setDefaultScreenFunct()
{
	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();
/*
	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);

*/
	FM->setButtonObj(interfaceButton0, buttonPress, functNewProject);
	FM->setButtonObj(interfaceButton1, buttonPress, functOpenProject);
	FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject);
	FM->setButtonObj(interfaceButton5, buttonPress, functSaveAsProject);
	FM->setButtonObj(interfaceButton6, buttonPress, functExport);

	//FM->setButtonObj(interfaceButton7, buttonPress, functStartGameModule); // ARKANOID

	FM->setButtonObj(interfaceButtonInsert, buttonPress, functConfirmKey);
	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	FM->setButtonObj(interfaceButtonDelete, functDeleteBackspace);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setPadsGlobal(functPads);


	FM->setSdDetection(functSdCard);
}
//==============================================================================================================
//==============================================================================================================

uint8_t cProjectEditor::loadProjectValues()
{
	engine.setHeadphonesVolume(mtProject.values.volume);
	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
	engine.setReverbDamping(mtProject.values.reverbDamping);
	engine.setLimiterAttack(mtProject.values.limiterAttack);
	engine.setLimiterRelease(mtProject.values.limiterRelease);
	engine.setLimiterTreshold(mtProject.values.limiterTreshold);


	//----------------------------------------------------------------------------------------------------
	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote = 36);

//	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);
	mtPadBoard.configureInstrumentPlayer(8);

	//----------------------------------------------------------------------------------------------------
	sampleRecorder.recorderConfig.gainLineIn = mtProject.values.gainLineIn;
	sampleRecorder.recorderConfig.gainMicHigh = mtProject.values.gainMicHigh;
	sampleRecorder.recorderConfig.gainMicLow = mtProject.values.gainMicLow;
	sampleRecorder.recorderConfig.gainRadio = mtProject.values.gainRadio;
	sampleRecorder.recorderConfig.monitor = mtProject.values.monitor;
	sampleRecorder.recorderConfig.radioFreq = mtProject.values.radioFreq;
	sampleRecorder.recorderConfig.source = mtProject.values.source;

	//----------------------------------------------------------------------------------------------------
	// performance mode i inne
	for(uint8_t i = 0; i<8; i++)
	{
		// paterny na trakach w performance mode
		if(mtProject.values.perfTracksPatterns[i] < 1 || mtProject.values.perfTracksPatterns[i] > 255)
		{
			mtProject.values.perfTracksPatterns[i] = 1;
		}

		// globalne mute trackow
		if(mtProject.values.trackMute[i] >= trackMasterModeCount) mtProject.values.trackMute[i] = 0;

		engine.muteTrack(i, mtProject.values.trackMute[i]);
	}

	for(uint8_t i = 0; i<12; i++)
	{
		if(mtProject.values.perfFxPlaces[i] > performanceFxesCount-1)
		{
			mtProject.values.perfFxPlaces[i] = (i+1 < performanceFxesCount) ? i+1 : 0;
		}

		if(mtProject.values.perfSelectedValues[i] > 3) mtProject.values.perfSelectedValues[i] = 0;

		//if(mtProject.values.perfFxValues[place][0] > 255 || mtProject.values.perfFxValues[place][0] < -255)
		if(mtProject.values.perfFxValues[i][0] != 0)
			mtProject.values.perfFxValues[i][0] = 0;
		if(mtProject.values.perfFxValues[i][1] > 255 || mtProject.values.perfFxValues[i][1] < -255)
			mtProject.values.perfFxValues[i][1] = 0;
		if(mtProject.values.perfFxValues[i][2] > 255 || mtProject.values.perfFxValues[i][2] < -255)
			mtProject.values.perfFxValues[i][2] = 0;
		if(mtProject.values.perfFxValues[i][3] > 255 || mtProject.values.perfFxValues[i][3] < -255)
			mtProject.values.perfFxValues[i][3] = 0;
	}

	//----------------------------------------------------------------------------------------------------
	// song pattern
	if(mtProject.values.globalTempo > 1000) mtProject.values.globalTempo = DEFAULT_TEMPO;
	if(mtProject.values.patternLength > 255) mtProject.values.patternLength = 32;


	//----------------------------------------------------------------------------------------------------
	// midi instruments velocity
	for(uint8_t channel = 0; channel<16; channel++)
	{
		if(mtProject.values.midiInstrument[channel].velocity > 127) mtProject.values.midiInstrument[channel].velocity = 127;
	}





	return 1;

}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
//Nowe podejscie ekran główny
static uint8_t functNewProject()
{
	if(PE->isBusyFlag) return 1;
	if(mtProject.values.projectNotSavedFlag)
	{
		PE->functShowSaveLastWindow();
		return 1;
	}

	strcpy(fileManager.currentProjectName, "New Project");

/*	memset(fileManager.currentProjectPatch,0,PATCH_SIZE);
	memset(fileManager.currentProjectName,0,PROJECT_NAME_SIZE);*/


	char currentPatch[PATCH_SIZE];
	strcpy(currentPatch,"Templates/New/project.mt");

	fileManager.getDefaultProject(&mtProject);
	fileManager.createEmptyTemplateProject((char*)"New");

	strcpy(mtConfig.startup.lastProjectName, fileManager.currentProjectName);


	PE->isBusyFlag = 1;
	PE->newProjectNotSavedFlag = 1;

	PE->newProjectPopupDelay = 0;
	PE->newProjectPopupFlag = 1;

	PE->showProcessingPopup("Creating new project");
	PE->showDefaultScreen();

	return 1;
}
static uint8_t functOpenProject()
{
	if(PE->isBusyFlag) return 1;

	PE->listOnlyFolderNames("/Projects/");

	PE->showProjectsList();

	PE->refreshProjectCover(100);
	strcpy(PE->projectCoverName, &PE->locationFilesList[PE->selectedLocation][0]);


	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton7, buttonPress, functOpenProjectConfirm);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functDelete);
	PE->FM->setButtonObj(interfaceButton5, buttonPress, functSaveChangesCancelOpen);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functProjectListUp);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functProjectListDown);

	PE->projectListActiveFlag = 1;

	return 1;
}
static uint8_t functSaveProject()
{
	if(PE->isBusyFlag) return 1;
	if(PE->newProjectNotSavedFlag)
	{
		functSaveAsProject();
		return 1;
	}

	PE->isBusyFlag = 1;
	PE->savePopupFlag = 1;
	PE->savePopupDelay = 0;

	mtProject.values.projectNotSavedFlag = 0;

	if(fileManager.currentProjectName[0]) PE->keyboardManager.fillName(fileManager.currentProjectName);
	else PE->keyboardManager.fillName((char *)"New Project");

	PE->showProcessingPopup("Saving project");

	return 1;
}

static uint8_t functSaveAsProject()
{
	if(PE->isBusyFlag) return 1;
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton5, buttonPress, functSaveAsAutoName);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveAsCancel);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsConfirm);
	PE->FM->setButtonObj(interfaceButton0, buttonPress, functConfirmKey);
	PE->FM->setButtonObj(interfaceButtonInsert, buttonPress, functConfirmKey);

	PE->keyboardManager.fillName(fileManager.currentProjectName);
	PE->keyboardManager.activateKeyboard();

	PE->showSaveAsKeyboard();

	return 1;
}


static uint8_t functExport()
{
	if(PE->isBusyFlag) return 1;
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functExportSong);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functExportSongStems);
	PE->FM->setButtonObj(interfaceButton2, buttonPress, functExportPattern);
	PE->FM->setButtonObj(interfaceButton3, buttonPress, functExportPatternStems);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functExportToMOD);
	PE->FM->setButtonObj(interfaceButton5, buttonPress, functExportCancel);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportGoBack);

	PE->showExportWindow();
	return 1;
}

//===============================================================================================================
//New Project
void cProjectEditor::functShowSaveLastWindow()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton5, buttonPress, functSaveChangesCancelNewProject);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesDontSaveNewProject);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveChangesSaveNewProject);

	showSaveLastWindow();
}

static uint8_t functSaveChangesCancelNewProject()
{
	if(PE->isBusyFlag) return 1;
	PE->setDefaultScreenFunct();

	PE->showDefaultScreen();
	return 1;
}
static uint8_t functSaveChangesDontSaveNewProject()
{
	if(PE->isBusyFlag) return 1;

	PE->newProjectNotSavedFlag = 1;

	strcpy(fileManager.currentProjectName, "New Project");

/*	memset(fileManager.currentProjectPatch,0,PATCH_SIZE);
	memset(fileManager.currentProjectName,0,PROJECT_NAME_SIZE);*/

	char currentPatch[PATCH_SIZE];
	strcpy(currentPatch,"Templates/New/project.mt");

	fileManager.getDefaultProject(&mtProject);
	fileManager.createEmptyTemplateProject((char*)"New");

	strcpy(mtConfig.startup.lastProjectName, fileManager.currentProjectName);


	PE->newProjectPopupDelay = 0;
	PE->newProjectPopupFlag = 1;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Creating new project");

	return 1;
}
static uint8_t functSaveChangesSaveNewProject()
{
	if(PE->isBusyFlag) return 1;
	if(PE->newProjectNotSavedFlag)
	{
		PE->showDefaultScreen();
		PE->showSaveAsKeyboard();
		functSaveAsProject();
		PE->newProjectOnSaveEndFlag = 1; //zostanie skasowana w cancel saveAs jakbyco
		return 1;
	}

	mtProject.values.projectNotSavedFlag = 0;

	PE->newProjectOnSaveEndFlag = 1;
	PE->savePopupFlag = 1;
	PE->savePopupDelay = 0;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Saving project");

	return 1;
}
//===============================================================================================================
//Save AS

static uint8_t functSaveAsCancel()
{
	if(PE->isBusyFlag) return 1;
	PE->setDefaultScreenFunct();

	PE->showDefaultScreen();
	PE->newProjectOnSaveEndFlag = 0; // powiazane z new project
	return 1;
}

static uint8_t functSaveAsConfirm()
{
	if(PE->isBusyFlag) return 1;

	if(fileManager.prepareSaveAs(PE->keyboardManager.getName(),FileManager::saveAsChecking) == 0 )
	{
		PE->functShowOverwriteWindow();
		return 1;
	}

	PE->newProjectNotSavedFlag = 0;
	mtProject.values.projectNotSavedFlag = 0;


	strcpy(fileManager.currentProjectName, 	PE->keyboardManager.getName());

	PE->savePopupFlag = 1;
	PE->savePopupDelay = 0;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Saving project");

	return 1;
}

static uint8_t functSaveAsAutoName()
{

	PE->keyboardManager.setRandomName();
	return 1;
}

void cProjectEditor::functShowOverwriteWindow()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveAsOverwriteNo);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsOverwriteYes);

	PE->showOverwriteWindow();
}

static uint8_t functSaveAsOverwriteYes()
{
	if(PE->isBusyFlag) return 1;

	fileManager.prepareSaveAs(PE->keyboardManager.getName(),FileManager::saveAsOverwrite);

	mtProject.values.projectNotSavedFlag = 0;

	PE->newProjectNotSavedFlag = 0;
	PE->savePopupFlag = 1;
	PE->savePopupDelay = 0;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Saving project");

	return 1;
}

static uint8_t functSaveAsOverwriteNo()
{
	if(PE->isBusyFlag) return 1;
	PE->showDefaultScreen();

	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveAsCancel);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsConfirm);
	PE->FM->setButtonObj(interfaceButton0, buttonPress, functConfirmKey);
	PE->FM->setButtonObj(interfaceButtonInsert, buttonPress, functConfirmKey);

	PE->keyboardManager.activateKeyboard();

	PE->showSaveAsKeyboard();

	return 1;
}
//===============================================================================================================
//open
static uint8_t functOpenProjectConfirm()
{
	if(PE->isBusyFlag) return 1;
	if(mtProject.values.projectNotSavedFlag)
	{
		PE->projectListActiveFlag = 0;
		PE->functShowSaveLastWindowBeforeOpen();
		return 1;
	}

	mtProject.values.projectNotSavedFlag = 0;
	PE->newProjectNotSavedFlag = 0;
	PE->projectListActiveFlag = 0;

	PE->openPopupDelay = 0;
	PE->openPopupFlag = 1;
	PE->isBusyFlag = 1;


	PE->showDefaultScreen();
	PE->showProcessingPopup("Opening project");
	return 1;
}
void cProjectEditor::functShowSaveLastWindowBeforeOpen()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton5, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesDontSaveOpen);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveChangesSaveOpen);

	showSaveLastWindow();
}

static uint8_t functDelete()
{
	if(PE->isBusyFlag) return 1;
	if(strcmp(fileManager.currentProjectName, &PE->locationFilesList[PE->selectedLocation][0]) == 0) return 1; // nie mozna usunac aktualnie uzywanego projektu

	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functDeleteConfirm);

	PE->showDeleteLastWindow();

	return 1;
}

static uint8_t functSaveChangesCancelOpen()
{
	if(PE->isBusyFlag) return 1;
	PE->projectListActiveFlag = 0;
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();

	return 1;
}

static uint8_t functProjectListUp()
{
	if(PE->projectListActiveFlag)
	{
		if(PE->selectedLocation > 0 )
		{
			PE->selectedLocation--;

			PE->refreshProjectCover(300);
			strcpy(PE->projectCoverName, &PE->locationFilesList[PE->selectedLocation][0]);
		}
		display.setControlValue(PE->fileListControl,PE->selectedLocation);
		display.refreshControl(PE->fileListControl);
	}
	return 1;
}

static uint8_t functProjectListDown()
{
	if(PE->projectListActiveFlag)
	{
		if(PE->selectedLocation < PE->locationFilesCount-1 )
		{
			PE->selectedLocation++;

			PE->refreshProjectCover(300);
			strcpy(PE->projectCoverName, &PE->locationFilesList[PE->selectedLocation][0]);
		}
		display.setControlValue(PE->fileListControl,PE->selectedLocation);
		display.refreshControl(PE->fileListControl);
	}
	return 1;
}


static uint8_t functSaveChangesDontSaveOpen()
{
	if(PE->isBusyFlag) return 1;

	mtProject.values.projectNotSavedFlag = 0;

	PE->newProjectNotSavedFlag = 0;
	PE->projectListActiveFlag = 0;

	PE->openPopupDelay = 0;
	PE->openPopupFlag = 1;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Opening project");

	return 1;
}
static uint8_t functSaveChangesSaveOpen()
{
	if(PE->isBusyFlag) return 1;
	if(PE->newProjectNotSavedFlag)
	{
		PE->showDefaultScreen();
		PE->showSaveAsKeyboard();
		functSaveAsProject();
		PE->openOnSaveEndFlag = 1; //zostanie skasowana w cancel saveAs jakbyco
		return 1;
	}

	mtProject.values.projectNotSavedFlag = 0;

	PE->openOnSaveEndFlag = 1;
	PE->projectListActiveFlag = 0;

	PE->savePopupFlag = 1;
	PE->savePopupDelay = 0;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Saving project");

	return 1;
}

static uint8_t functDeleteConfirm()
{
	if(PE->isBusyFlag) return 1;

	PE->deletePopupFlag = 1;
	PE->deletePopupDelay = 0;
	PE->isBusyFlag = 1;


	PE->showDefaultScreen();
	PE->showProcessingPopup("Deleting project");
	return 1;
}
//===============================================================================================================
//export
char currentExportPath[PATCH_SIZE];

static uint8_t functExportSong()
{
	if(PE->isBusyFlag) return 1;
	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;

	PE->isBusyFlag = 1;
	PE->exportInProgress = 1;
	PE->exportProgress = 0;
	PE->currentExportType = (int)exportType::song;

	uint16_t fileCounter = 0;

	sprintf(currentExportPath,"Export/%s/%s_S.wav",fileManager.currentProjectName,fileManager.currentProjectName);
	while(SD.exists(currentExportPath))
	{
		sprintf(currentExportPath,"Export/%s/%s_S%d.wav",fileManager.currentProjectName,fileManager.currentProjectName,++fileCounter);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}
	if(fileCounter == 0 ) sprintf(currentExportPath,"Export/%s/%s_S",fileManager.currentProjectName,fileManager.currentProjectName);
	else sprintf(currentExportPath,"Export/%s/%s_S%d",fileManager.currentProjectName,fileManager.currentProjectName,fileCounter);

	PE->showLabelDuringExport();
	exporter.start(currentExportPath, mtExporter::exportType::song);
	return 1;
}
static uint8_t functExportSongStems()
{
	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;
	if(PE->isBusyFlag) return 1;

	PE->isBusyFlag = 1;
	PE->exportInProgress = 1;
	PE->exportProgress = 0;
	PE->currentExportType = (int)exportType::songStems;

	uint16_t fileCounter = 0;

	sprintf(currentExportPath,"Export/%s/%s_Song_S",fileManager.currentProjectName,fileManager.currentProjectName);
	while(SD.exists(currentExportPath))
	{
		sprintf(currentExportPath,"Export/%s/%s_Song_S%d",fileManager.currentProjectName,fileManager.currentProjectName,++fileCounter);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}
	if(fileCounter == 0 ) sprintf(currentExportPath,"%s/%s_Song_S",fileManager.currentProjectName,fileManager.currentProjectName);
	else sprintf(currentExportPath,"%s/%s_Song_S%d",fileManager.currentProjectName,fileManager.currentProjectName,fileCounter);

	PE->showLabelDuringExport();
	exporter.start(currentExportPath, mtExporter::exportType::songStems);
	return 1;
}
static uint8_t functExportPattern()
{
	if(PE->isBusyFlag) return 1;
	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;

	PE->isBusyFlag = 1;
	PE->exportInProgress = 1;
	PE->exportProgress = 0;
	PE->currentExportType = (int)exportType::pattern;

	uint16_t fileCounter = 0;
	uint16_t namePattern = mtProject.values.actualPattern;

	sprintf(currentExportPath,"Export/%s/%s_P%d.wav",fileManager.currentProjectName,fileManager.currentProjectName,namePattern);
	while(SD.exists(currentExportPath))
	{
		sprintf(currentExportPath,"Export/%s/%s_P%d_%d.wav",fileManager.currentProjectName,fileManager.currentProjectName,namePattern,++fileCounter);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}
	if(fileCounter == 0 ) sprintf(currentExportPath,"Export/%s/%s_P%d",fileManager.currentProjectName,fileManager.currentProjectName,namePattern);
	else sprintf(currentExportPath,"Export/%s/%s_P%d_%d",fileManager.currentProjectName,fileManager.currentProjectName,namePattern,fileCounter);

	PE->showLabelDuringExport();
	exporter.start(currentExportPath, mtExporter::exportType::pattern);
	return 1;
}
static uint8_t functExportPatternStems()
{
	if(PE->isBusyFlag) return 1;
	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;

	PE->isBusyFlag = 1;
	PE->exportInProgress = 1;
	PE->exportProgress = 0;
	PE->currentExportType = (int)exportType::patternStems;

	uint16_t fileCounter = 0;
	uint16_t namePattern = mtProject.values.actualPattern;

	sprintf(currentExportPath,"Export/%s/%s_P%d_S",fileManager.currentProjectName,fileManager.currentProjectName,namePattern);
	while(SD.exists(currentExportPath))
	{
		sprintf(currentExportPath,"Export/%s/%s_P%d_S%d",fileManager.currentProjectName,fileManager.currentProjectName,namePattern,++fileCounter);
		if(fileCounter > 9999) return 1; // jak ktos zapisze jeden projekt 10000 razy to należy mu się medal z ziemniaka todo: obsłużyć jakoś
	}
	if(fileCounter == 0 ) sprintf(currentExportPath,"%s/%s_P%d_S",fileManager.currentProjectName,fileManager.currentProjectName,namePattern);
	else sprintf(currentExportPath,"%s/%s_P%d_S%d",fileManager.currentProjectName,fileManager.currentProjectName,namePattern,fileCounter);

	PE->showLabelDuringExport();
	exporter.start(currentExportPath, mtExporter::exportType::patternStems);
	return 1;
}
static uint8_t functExportToMOD()
{
	if(PE->isBusyFlag) return 1;
	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;

	return 1;
}

static uint8_t functExportCancel()
{

	return 1;
}

static uint8_t functExportGoBack()
{

	if((PE->exportInProgress) || (PE->isBusyFlag))
	{
		exporter.cancel();
		PE->exportInProgress = 0;
		PE->isBusyFlag = 0;
		PE->showExportWindow();
		return 1;
	}

	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();
	return 1;
}
//===============================================================================================================
//uint8_t functShowProjectsList()
//{
//	PE->listOnlyFolderNames("/Projects/");
//
//
//	PE->showProjectsList();
//
//
//// funkcje
//	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
//	PE->FM->clearAllPots();
//
//	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);
//
//	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenProject);
//	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);
//
//
//	return 1;
//}


//uint8_t functShowTemplatesList()
//{
//	PE->listOnlyFolderNames("/Templates/");
//
//
//	PE->showTemplatesList();
//
//
//
//
//// funkcje
//	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
//	PE->FM->clearAllPots();
//
//	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);
//
//	PE->FM->setButtonObj(interfaceButton0, buttonPress, functEnterName);
//	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);
//	PE->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate);
//
//	return 1;
//}

//uint8_t functCancelList()
//{
//	PE->showDefaultScreen();
//	PE->setDefaultScreenFunct();
//	return 1;
//}
//uint8_t functOpenTemplate()
//{
//
//
//	PE->showDefaultScreen();
//	PE->setDefaultScreenFunct();
//
//	return 1;
//}

//uint8_t functCreateNewTemplate()
//{
//
//	functShowTemplatesList();
//
//	return 1;
//}

static uint8_t functSwitchModule(uint8_t button)
{

	if(PE->isBusyFlag) return 1;
	if(PE->openInProgressFlag || PE->saveInProgressFlag || PE->exportInProgress) return 1;
	PE->eventFunct(eventSwitchModule,PE,&button,0);

	return 1;
}
//======================================================================================================================


void cProjectEditor::listOnlyFolderNames(const char* folder)
{
	char filePath[256] = {0};
	strcpy(filePath, folder);
	strcat(filePath,"/");
	sdLocation.close();
	sdLocation.open(folder, O_READ); //"/"
	locationFilesCount = sdLocation.createFilesList(0,locationFilesList, files_list_length_max);
	sdLocation.close();


	uint8_t foundProjectsCount = 0;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		if(locationFilesList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(filePath, folder);
			strcat(filePath,&locationFilesList[i][0]); //doklej nazwe folderu

			sdLocation.open(filePath, O_READ);

			if(sdLocation.exists("project.mt"))	//tylko jesli w folderze jest plik projektu
			{
				strcpy(&locationFilesList[foundProjectsCount][0],&locationFilesList[i][1]);

				foundProjectsCount++;
			}


			sdLocation.close();
		}
	}

	char strBuff[40];

	for (uint8_t i = 0; i < (foundProjectsCount-1)/2; i++)
	{
		strcpy(strBuff, locationFilesList[i]);
		strcpy(locationFilesList[i],locationFilesList[foundProjectsCount-1-i]);
		strcpy(locationFilesList[foundProjectsCount-i], strBuff);
	}


	locationFilesCount = foundProjectsCount;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		filesNames[i] = &locationFilesList[i][0];
	}

}

//static uint8_t functEnterName()
//{
//
//	char localPatch[PATCH_SIZE];
//	uint16_t cnt=1;
//	strcpy(PE->name,"New Project");
//	sprintf(localPatch,"Projects/%s",PE->name);
//
//	while(SD.exists(localPatch))
//	{
//	   sprintf(PE->name,"New Project%d",cnt);
//	   sprintf(localPatch,"Projects/%s",PE->name);
//
//	   cnt++;
//	   if(cnt > 9999)
//	   {
//		   memset(PE->name,0,33);
//		   break;
//	   }
//	}
//
//	PE->editPosition = strlen(PE->name);
//	PE->keyboardPosition = BACKSPACE_PAD_1;
//	PE->lastPressedPad = BACKSPACE_PAD_1;
//	leds.setLED(BACKSPACE_PAD_1, 1, 31);
//	leds.setLED(BACKSPACE_PAD_2, 1, 31);
//
//
//	PE->showEnterNameKeyboard();
//	PE->keyboardActiveFlag = 1;
//
//
//// funkcje
//	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
//	PE->FM->clearAllPots();
//
////	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);
//
//	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenTemplate);
//	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);
//	PE->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate);
//	PE->FM->setButtonObj(interfaceButtonEnter, buttonPress, functConfirmKey);
//
//
//	return 1;
//}

static  uint8_t functLeft()
{
	PE->keyboardManager.makeMove('a');
	return 1;
}
static  uint8_t functRight()
{

	PE->keyboardManager.makeMove('d');
	return 1;
}
static  uint8_t functUp()
{
	PE->keyboardManager.makeMove('w');
	if(PE->keyboardManager.getState()) return 1;
	if(PE->projectListActiveFlag)
	{
		if(PE->selectedLocation > 0 )
		{
			PE->selectedLocation--;

			PE->refreshProjectCover(300);
			strcpy(PE->projectCoverName, &PE->locationFilesList[PE->selectedLocation][0]);
		}
		display.setControlValue(PE->fileListControl,PE->selectedLocation);
		display.refreshControl(PE->fileListControl);
		return 1;
	}

	return 1;
}
static  uint8_t functDown()
{
	PE->keyboardManager.makeMove('s');
	if(PE->keyboardManager.getState()) return 1;
	if(PE->projectListActiveFlag)
	{
		if(PE->selectedLocation < PE->locationFilesCount-1 )
		{
			PE->selectedLocation++;

			PE->refreshProjectCover(300);
			strcpy(PE->projectCoverName, &PE->locationFilesList[PE->selectedLocation][0]);
		}
		display.setControlValue(PE->fileListControl,PE->selectedLocation);
		display.refreshControl(PE->fileListControl);
		return 1;
	}
	return 1;
}

static uint8_t functConfirmKey()
{
	PE->keyboardManager.confirmKey();
	return 1;
}

static uint8_t functDeleteBackspace(uint8_t state)
{
	if((state == buttonPress) || (state == buttonHold))
	{
		PE->keyboardManager.makeBackspace();
	}
	return 1;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(PE->isBusyFlag) return 1;

	PE->keyboardManager.onPadChange(pad, state);
	return 1;
}

static  uint8_t functEncoder(int16_t value)
{
	if(PE->isBusyFlag) return 1;
	if(PE->projectListActiveFlag)
	{
		if(value > 0)
		{
			if(PE->selectedLocation < PE->locationFilesCount-1 ) PE->selectedLocation++;
		}
		else if (value < 0)
		{
			if(PE->selectedLocation > 0 ) PE->selectedLocation--;
		}

		PE->refreshProjectCover(300);
		strcpy(PE->projectCoverName, &PE->locationFilesList[PE->selectedLocation][0]);

		display.setControlValue(PE->fileListControl,PE->selectedLocation);
		display.refreshControl(PE->fileListControl);
	}
	return 1;
}



void cProjectEditor::refreshProjectCover(uint16_t delay_ms)
{
	if(delay_ms > 2000) delay_ms = 2000;

	PE->refreshCover = 1;
	PE->coverDelay = 2000-delay_ms;

	display.setControlHide(coverImg);
}

static uint8_t functSdCard(uint8_t state)
{
	if(state)
	{
		PE->start(0);
	}
	else
	{
		PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
		PE->FM->clearAllPots();
		PE->FM->setSdDetection(functSdCard);
		PE->deactivateGui();
	}

	return 1;
}
