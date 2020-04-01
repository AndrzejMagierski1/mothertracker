
#include <mtRandomNameGenerator.h>
#include "mtSliceDetector.h"

#include "core/interfacePopups.h"

#include "projectEditor/projectEditor.h"
///#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "keyScanner.h"
#include "mtLED.h"
#include "mtExporterWAV.h"
#include "mtPadBoard.h"
#include "sampleRecorder/sampleRecorder.h"

#include "performanceMode/performanceMode.h"
#include "sdCardDetect.h"
#include "fileManager.h"
#include "debugLog.h"

cProjectEditor projectEditor;
cProjectEditor* PE = &projectEditor;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

//****************************************************
//Nowe podejście - ekran główny
static uint8_t functNewProject();
static uint8_t functOpenProject();
static uint8_t functImportMod();
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
static uint8_t functImportModConfirm();
static uint8_t functSaveChangesCancelOpen();
static uint8_t functSaveChangesDontSaveOpen();
static uint8_t functSaveChangesDontSaveOpenMod();
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


static uint8_t functStopPatternYes();
static uint8_t functStopPatternNo();

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();
static  uint8_t functConfirmKey();
static  uint8_t makeMoveOnActiveList(int16_t moveDir);
static uint8_t functDeleteBackspace(uint8_t state);
static uint8_t functEncoder(int16_t value);
static uint8_t functDeleteProject();
static uint8_t functDeleteMod();
static uint8_t functDeleteProject_Confirm();
static uint8_t functDeleteMod_Confirm();
static  uint8_t functPlayAction();
static uint8_t functStartGameModule()
{
	PE->eventFunct(eventActivateGameModule,PE,0,0);
	return 1;
}

void cProjectEditor::update()
{
	uint8_t managerStatus = newFileManager.getStatus();

	if(managerStatus == fmBrowseProjectsEnd)
	{
		processProjectList();
		FM->unblockAllInputs();
		newFileManager.clearStatus();
	}
	else if(managerStatus == fmExportSoundEnd)
	{
		PE->showExportWindow();
		PE->setExportWindowFuncts();
		FM->unblockAllInputs();
		newFileManager.clearStatus();
		interfaceGlobals.refreshFileExplorer = true;
	}
	else if(managerStatus == fmSaveEnd)
	{
		FM->unblockAllInputs();
		newFileManager.clearStatus();
		interfaceGlobals.refreshFileExplorer = true;
	}
	else if (managerStatus == fmBrowseModsEnd)
	{
		processModsList();
		FM->unblockAllInputs();
		newFileManager.clearStatus();
	}
	else if(managerStatus >= fmLoadEnd && managerStatus < fmError) // 	lapie cala reszte Endow
	{
		setDefaultScreenFunct();
		showDefaultScreen();
		FM->unblockAllInputs();
		newFileManager.clearStatus();
	}
	else if(managerStatus >=  fmError) // a tu wszelakie errory
	{
		debugLog.addLine("File Manager Opretion Error");
		setDefaultScreenFunct();
		showDefaultScreen();
		FM->unblockAllInputs();
		newFileManager.clearStatus();
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
	selectedProject = 0;

	moduleRefresh = 1;

	keyboardManager.init(keyboardControl,editName);


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

	FM->setButtonObj(interfaceButton0, buttonPress, functNewProject);
	FM->setButtonObj(interfaceButton1, buttonPress, functOpenProject);
	FM->setButtonObj(interfaceButton2, buttonPress, functImportMod);
	FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject);
	FM->setButtonObj(interfaceButton5, buttonPress, functSaveAsProject);
	FM->setButtonObj(interfaceButton6, buttonPress, functExport);
	//FM->setButtonObj(interfaceButton7, buttonPress, functStartGameModule); // ARKANOID
	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);

	FM->setButtonObj(interfaceButtonInsert, buttonPress, functConfirmKey);
	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	FM->setButtonObj(interfaceButtonDelete, functDeleteBackspace);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setPadsGlobal(functPads);

}


void cProjectEditor::setExportWindowFuncts()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functExportSong);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functExportSongStems);
	PE->FM->setButtonObj(interfaceButton2, buttonPress, functExportPattern);
	PE->FM->setButtonObj(interfaceButton3, buttonPress, functExportPatternStems);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functExportToMOD);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportGoBack);


	PE->FM->clearButton(interfaceButtonPlay);

}

//==============================================================================================================
//==============================================================================================================
static  uint8_t functPlayAction()
{
	if (sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
		}
		else
		{
			sequencer.playPattern();
		}
	}
	else
	{
		sequencer.stop();
	}

	return 1;
}

uint8_t cProjectEditor::loadProjectValues()
{
	engine.setHeadphonesVolume(mtProject.values.volume);
	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
	engine.setReverbDamping(mtProject.values.reverbDamping);
	engine.setLimiterAttack(mtProject.values.limiterAttack);
	engine.setLimiterRelease(mtProject.values.limiterRelease);
	engine.setLimiterTreshold(mtProject.values.limiterTreshold);
	//----------------------------------------------------------------------------------------------------
//	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
//			mtProject.values.padBoardNoteOffset,
//			mtProject.values.padBoardRootNote = 36);


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

void cProjectEditor::processProjectList()
{
	PE->projectsListLength = newFileManager.getProjectsList(&PE->projectsList);

	PE->selectedProject = 0;

	PE->showProjectsList();

	PE->refreshProjectCover(100);
	strcpy(PE->projectCoverName, PE->projectsList[PE->selectedProject]);

	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton7, buttonPress, functOpenProjectConfirm);
	PE->FM->setButtonObj(interfaceButton2, buttonPress, functDeleteProject);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton0, buttonPress, functProjectListUp);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functProjectListDown);

	PE->projectListActiveFlag = 1;
}
void cProjectEditor::processModsList()
{

	PE->modsListLength = newFileManager.getModsList(&PE->modsList);

	PE->selectedMod = 0;

	PE->showModsList();

//	PE->refreshProjectCover(100);
//	strcpy(PE->projectCoverName, PE->projectsList[PE->selectedProject]);


	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton7, buttonPress, functImportModConfirm);
	PE->FM->setButtonObj(interfaceButton2, buttonPress, functDeleteMod);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton0, buttonPress, functProjectListUp);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functProjectListDown);

	PE->modsListActiveFlag = 1;

}

//==============================================================================================================
//==============================================================================================================

void cProjectEditor::setStopPatternFunct()
{
	FM->clearAllButtons();
	FM->clearAllPots();
	FM->clearAllPads();

	FM->setButtonObj(interfaceButton6, buttonPress, functStopPatternNo);
	FM->setButtonObj(interfaceButton7, buttonPress, functStopPatternYes);
}

static uint8_t functStopPatternYes()
{

	sequencer.stop();
	PE->setDefaultScreenFunct();
	switch (PE->stopAction)
	{
		case cProjectEditor::stopActionOpen: 		functOpenProjectConfirm();		break;
		case cProjectEditor::stopActionNewProject: 	functNewProject();				break;
		case cProjectEditor::stopActionExport: 		functExport();					break;
		case cProjectEditor::stopActionImportMod: 	functImportModConfirm();		break;
		default: break;
	}


	return 1;
}

static uint8_t functStopPatternNo()
{
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();
	return 1;
}



//==============================================================================================================
//==============================================================================================================
//==============================================================================================================

void cProjectEditor::createNewProject()
{
	if(newFileManager.isProjectChanged()  || (mtProject.values.projectNotSavedFlag & 1))
	{
		PE->functShowSaveLastWindow();
		return;
	}

	newFileManager.createNewProjectInWorkspace();

	if(newFileManager.openProjectFromWorkspace())
	{
		PE->FM->blockAllInputs();
	}
}

//----------------------------------------------------------------------------------------
void cProjectEditor::openProject()
{
	if(newFileManager.isProjectChanged() || (mtProject.values.projectNotSavedFlag & 1))
	{
		PE->functShowSaveLastWindowBeforeOpen();
		return;
	}

	if(newFileManager.openProjectFromProjects(PE->selectedProject))
	{
		PE->FM->blockAllInputs();
	}
}

//----------------------------------------------------------------------------------------
void cProjectEditor::saveProject()
{
	if(mtProject.values.projectNotSavedFlag & 2)
	{
		functSaveAsProject();
		return;
	}

	if(newFileManager.saveProjectToProjects())
	{
		PE->FM->blockAllInputs();
	}
}

//----------------------------------------------------------------------------------------
void cProjectEditor::saveAsProject()
{

	if(newFileManager.saveProjectToProjects(PE->keyboardManager.getName()))
	{
		PE->FM->blockAllInputs();
	}
}


//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
//Nowe podejscie ekran główny
static uint8_t functNewProject()
{
	if(sequencer.isPlay())
	{
		PE->showStopPatternWindow();
		PE->setStopPatternFunct();
		PE->stopAction = cProjectEditor::stopActionNewProject;
		return 1;
	}

	PE->showDefaultScreen();

	PE->createNewProject();

	return 1;
}
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================

static uint8_t functImportMod()
{
//	if (PE->isBusyFlag) return 1;

	newFileManager.browseMods();

	return 1;
}

static uint8_t functOpenProject()
{
	newFileManager.browseProjects();
	return 1;
}

static uint8_t functSaveProject()
{
	PE->saveProject();

	return 1;
}

static uint8_t functSaveAsProject()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton5, buttonPress, functSaveAsAutoName);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveAsCancel);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsConfirm);
	PE->FM->setButtonObj(interfaceButton0, buttonPress, functConfirmKey);
	PE->FM->setButtonObj(interfaceButtonInsert, buttonPress, functConfirmKey);

	PE->keyboardManager.fillName(newFileManager.getCurrentProjectName());
	PE->keyboardManager.activateKeyboard();

	PE->showSaveAsKeyboard();

	return 1;
}


static uint8_t functExport()
{
	if(sequencer.isPlay())
	{
		PE->showStopPatternWindow();
		PE->setStopPatternFunct();
		PE->stopAction = cProjectEditor::stopActionExport;
		return 1;
	}

	PE->setExportWindowFuncts();
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

//anuluj tworzenie nowego projektu
static uint8_t functSaveChangesCancelNewProject()
{
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();

	return 1;
}

// nie zapisuj starego projektu i otworz nowy
static uint8_t functSaveChangesDontSaveNewProject()
{
	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();

	// ignorowanie neizapisanego
	mtProject.values.projectNotSavedFlag = 0;
	newFileManager.clearChangeFlags();

	PE->createNewProject();

	return 1;
}

// zapisz stary projekt przy otwieraniu nowego
static uint8_t functSaveChangesSaveNewProject()
{
	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();

	PE->saveProject();

	return 1;
}
//===============================================================================================================
//Save AS
static uint8_t functSaveAsCancel()
{
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();

	return 1;
}

static uint8_t functSaveAsConfirm()
{
	if(newFileManager.projectExist(PE->keyboardManager.getName()))
	{
		PE->functShowOverwriteWindow();
		return 1;
	}
	PE->saveAsProject();

	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();
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
	PE->saveAsProject();
	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();
	return 1;
}

static uint8_t functSaveAsOverwriteNo()
{
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
	if(PE->projectsList[PE->selectedProject] == nullptr || PE->projectsList[PE->selectedProject][0] == '0') return 1;

	if(sequencer.isPlay())
	{
		PE->showStopPatternWindow();
		PE->setStopPatternFunct();
		PE->stopAction = cProjectEditor::stopActionOpen;
		return 1;
	}


	PE->showDefaultScreen();
	PE->openProject();

	PE->projectListActiveFlag = 0;

	return 1;
}
static uint8_t functImportModConfirm()
{
	if(PE->modsList[PE->selectedMod] == nullptr || PE->modsList[PE->selectedMod][0] == '0') return 1;


	if(sequencer.isPlay())
	{
		PE->showStopPatternWindow();
		PE->setStopPatternFunct();
		PE->stopAction = cProjectEditor::stopActionImportMod;
		return 1;
	}

	if(newFileManager.isProjectChanged() || (mtProject.values.projectNotSavedFlag & 1))
	{
		PE->projectListActiveFlag = 0;
		PE->functShowSaveLastWindowBeforeImportMod();
		return 1;
	}

	debugLog.setMaxLineCount(5);
	debugLog.addLine("Import Started");
	debugLog.forceRefresh();

	PE->modsListActiveFlag = 0;

	newFileManager.createNewProjectInWorkspace();
	newFileManager.openProjectFromWorkspace();


	newFileManager.importModAfterLoadNewProject(PE->selectedMod);

	PE->showDefaultScreen();

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
void cProjectEditor::functShowSaveLastWindowBeforeImportMod()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	PE->FM->setButtonObj(interfaceButton5, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesDontSaveOpenMod);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveChangesSaveOpen);
	showSaveLastWindow();
}

static uint8_t functDeleteProject()
{
	if(PE->projectsList[PE->selectedProject] == nullptr || PE->projectsList[PE->selectedProject][0] == '0') return 1;

	if(strcmp(newFileManager.getCurrentProjectName(), PE->projectsList[PE->selectedProject]) == 0)
	{
		// nie mozna usunac aktualnie uzywanego projektu
		mtPopups.config(4, (strPopupStyleConfig*)&popupDebugConfig);
		mtPopups.show(4, "You cannot delete active project.");

		return 1;	}

	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functDeleteProject_Confirm);
	PE->showDeleteProjectLastWindow();
	return 1;
}
static uint8_t functDeleteMod()
{
	if(PE->modsList[PE->selectedMod] == nullptr || PE->modsList[PE->selectedMod][0] == '0') return 1;

	PE->FM->setButtonObj(interfaceButton6, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functDeleteMod_Confirm);
	PE->showDeleteModLastWindow();
	return 1;
}

static uint8_t functSaveChangesCancelOpen()
{
	PE->projectListActiveFlag = 0;
	PE->modsListActiveFlag= 0;

	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();
	return 1;
}

static uint8_t functProjectListUp()
{
	makeMoveOnActiveList(-1);
	return 1;
}

static uint8_t functProjectListDown()
{
	makeMoveOnActiveList(1);
	return 1;
}


static uint8_t functSaveChangesDontSaveOpen()
{
	PE->projectListActiveFlag = 0;

	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();

	// ignorowanie neizapisanego
	mtProject.values.projectNotSavedFlag = 0;
	newFileManager.clearChangeFlags();

	PE->openProject();

	return 1;
}

static uint8_t functSaveChangesDontSaveOpenMod()
{
	PE->projectListActiveFlag = 0;


	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();


	// ignorowanie neizapisanego
	mtProject.values.projectNotSavedFlag = 0;
	newFileManager.clearChangeFlags();

	functImportModConfirm();


	return 1;
}

static uint8_t functSaveChangesSaveOpen()
{
	if(mtProject.values.projectNotSavedFlag)
	{
		PE->showDefaultScreen();
		PE->showSaveAsKeyboard();
		functSaveAsProject();
		return 1;
	}

	//mtProject.values.projectNotSavedFlag = 0;

	PE->showDefaultScreen();
	return 1;
}

static uint8_t functDeleteProject_Confirm()
{
	newFileManager.deleteProject(PE->selectedProject);

	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();

	return 1;
}

static uint8_t functDeleteMod_Confirm()
{
//	if(PE->isBusyFlag) return 1;
//	PE->deletePopupFlag = 1;
//	PE->deletePopupDelay = 0;
//	PE->isBusyFlag = 1;

	newFileManager.deleteMod(PE->selectedMod);

	PE->showDefaultScreen();
	PE->setDefaultScreenFunct();
//	PE->showProcessingPopup("Deleting project");
	return 1;
}



//===============================================================================================================
//export
char currentExportPath[PATCH_SIZE];

static uint8_t functExportSong()
{
	if(newFileManager.exportSound(exportSong))
	{
		PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportCancel);
		PE->FM->blockAllInputsExcept(interfaceButton7);

		PE->showLabelDuringExport();
	}

	return 1;
}

static uint8_t functExportSongStems()
{
	if(newFileManager.exportSound(exportSongStems))
	{
		PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportCancel);
		PE->FM->blockAllInputsExcept(interfaceButton7);

		PE->showLabelDuringExport();
	}

	return 1;
}
static uint8_t functExportPattern()
{
	if(newFileManager.exportSound(exportPattern))
	{
		PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportCancel);
		PE->FM->blockAllInputsExcept(interfaceButton7);

		PE->showLabelDuringExport();
	}

	return 1;
}
static uint8_t functExportPatternStems()
{
	if(newFileManager.exportSound(exportPatternStems))
	{
		PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportCancel);
		PE->FM->blockAllInputsExcept(interfaceButton7);

		PE->showLabelDuringExport();
	}

	return 1;
}
static uint8_t functExportToMOD()
{


	return 1;
}

static uint8_t functExportCancel()
{
	newFileManager.exportSoundCancel();

	PE->showExportWindow();
	return 1;
}

static uint8_t functExportGoBack()
{
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();
	return 1;
}
//===============================================================================================================

static uint8_t functSwitchModule(uint8_t button)
{

	PE->eventFunct(eventSwitchModule,PE,&button,0);

	return 1;
}
//======================================================================================================================


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

static uint8_t functUp()
{
	PE->keyboardManager.makeMove('w');
	if (PE->keyboardManager.getState()) return 1;

	makeMoveOnActiveList(-1);

	return 1;
}

static uint8_t functDown()
{
	PE->keyboardManager.makeMove('s');
	if (PE->keyboardManager.getState()) return 1;

	makeMoveOnActiveList(1);

	return 1;
}

static  uint8_t makeMoveOnActiveList(int16_t moveDir)
{
	moveDir = constrain(moveDir, -5, 5);
	if (PE->modsListActiveFlag)
	{
		PE->selectedMod = constrain(int16_t (PE->selectedMod) + moveDir,
									0,
									PE->modsListLength - 1);

		display.setControlValue(PE->fileListControl, PE->selectedMod);
		display.refreshControl(PE->fileListControl);
	}
	if (PE->projectListActiveFlag)
	{
		PE->selectedProject = constrain(PE->selectedProject + moveDir,
											0,
											PE->projectsListLength - 1);

		PE->refreshProjectCover(300);
		strcpy(PE->projectCoverName, PE->projectsList[PE->selectedProject]);

		display.setControlValue(PE->fileListControl, PE->selectedProject);
		display.refreshControl(PE->fileListControl);
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
	PE->keyboardManager.onPadChange(pad, state);
	return 1;
}

static  uint8_t functEncoder(int16_t value)
{
//	if(PE->isBusyFlag) return 1;
	makeMoveOnActiveList(value);
	return 1;
}

void cProjectEditor::refreshProjectCover(uint16_t delay_ms)
{
	if(delay_ms > 2000) delay_ms = 2000;
	PE->refreshCover = 1;
	PE->coverDelay = 2000-delay_ms;
	display.setControlHide(coverImg);
}
