


#include <projectEditor.h>
#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "mtLED.h"

#include "mtPadBoard.h"

#include "sampleRecorder.h"

#include "performanceMode.h"

enum valueMapDirecion
{
	valueMapDirectionLeft,
	valueMapDirectionRight,
	valueMapDirectionUp,
	valueMapDirectionDown
};

constexpr char smallKeyboard[KEYBOARD_SIZE] =
{
		 	 '1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' , 0 ,
		 	 'q', 'w', 'e' ,'r' ,'t' ,'y' ,'u' ,'i' ,'o' ,'p' ,'-' ,'+',
			 'a', 's', 'd' ,'f' ,'g' ,'h' ,'j' ,'k' ,'l' ,'@' , 1 ,
			 'z', 'x', 'c' ,'v' ,'b' ,'n' ,'m' , ' '
};

constexpr char bigKeyboard[KEYBOARD_SIZE] =
{
			 '1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' , 0 ,
		 	 'Q', 'W', 'E' ,'R' ,'T' ,'Y' ,'U' ,'I' ,'O' ,'P' ,'-' ,'+',
			 'A', 'S', 'D' ,'F' ,'G' ,'H' ,'J' ,'K' ,'L', '@', 1 ,
			 'Z', 'X', 'C' ,'V' ,'B' ,'N' ,'M' , ' '
};

//constexpr uint8_t valueMap[4][42] = przechodzenie przez sciany
//{
//		{
//			10,0,1,2,3,4,5,6,7,8,9,
//			22,11,12,13,14,15,16,17,18,19,20,21,
//			33,23,24,25,26,27,28,29,30,31,32,
//			41,34,35,36,37,38,39,40
//		},
//
//		{
//			1,2,3,4,5,6,7,8,9,10,0,
//			12,13,14,15,16,17,18,19,20,21,22,11,
//			24,25,26,27,28,29,30,31,32,33,23,
//			35,36,37,38,39,40,41,34
//		},
//
//		{
//			34,35,36,37,38,39,40,41,41,41,41,
//			0,1,2,3,4,5,6,7,8,9,10,10,
//			11,12,13,14,15,16,17,18,19,20,21,
//			23,24,25,26,27,28,29,30,
//		},
//
//		{
//			11,12,13,14,15,16,17,18,19,20,21,
//			23,24,25,26,27,28,29,30,31,32,33,33,
//			34,35,36,37,38,39,40,41,41,41,41,
//			0,1,2,3,4,5,6,7
//		},
//};

constexpr uint8_t valueMap[4][42] =
{
		{
			0,0,1,2,3,4,5,6,7,8,9,
			11,11,12,13,14,15,16,17,18,19,20,21,
			23,23,24,25,26,27,28,29,30,31,32,
			34,34,35,36,37,38,39,40
		},

		{
			1,2,3,4,5,6,7,8,9,10,10,
			12,13,14,15,16,17,18,19,20,21,22,22,
			24,25,26,27,28,29,30,31,32,33,33,
			35,36,37,38,39,40,41,41
		},

		{
			0,1,2,3,4,5,6,7,8,9,10,
			0,1,2,3,4,5,6,7,8,9,10,10,
			11,12,13,14,15,16,17,18,19,20,21,
			23,24,25,26,27,28,29,30,
		},

		{
			11,12,13,14,15,16,17,18,19,20,21,
			23,24,25,26,27,28,29,30,31,32,33,33,
			34,35,36,37,38,39,40,41,41,41,41,
			34,35,36,37,38,39,40,41
		},
};

constexpr uint8_t valueMapPads[48] =
{
	0,1,2,3,4,5,6,7,8,9,10,10,
	11,12,13,14,15,16,17,18,19,20,21,22,
	23,24,25,26,27,28,29,30,31,32,33,33,
	34,35,36,37,38,39,40,41,41,41,41,41
};

constexpr uint8_t keyPositionToPads[42] =
{
	0,1,2,3,4,5,6,7,8,9,10,
	12,13,14,15,16,17,18,19,20,21,22,23,
	24,25,26,27,28,29,30,31,32,33,34,
	36,37,38,39,40,41,42,43
};

constexpr uint8_t BACKSPACE_PAD_1 = 10;
constexpr uint8_t BACKSPACE_PAD_2 = 11;

constexpr uint8_t CAPS_LOCK_PAD_1 = 34;
constexpr uint8_t CAPS_LOCK_PAD_2 = 35;

constexpr uint8_t SPACE_PAD_1 = 43;
constexpr uint8_t SPACE_PAD_2 = 44;
constexpr uint8_t SPACE_PAD_3 = 45;
constexpr uint8_t SPACE_PAD_4 = 46;
constexpr uint8_t SPACE_PAD_5 = 47;

constexpr uint8_t F_PAD = 27;

constexpr uint8_t J_PAD = 30;

cProjectEditor projectEditor;
cProjectEditor* PE = &projectEditor;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

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
static uint8_t functSaveAsOverwriteYes();
static uint8_t functSaveAsOverwriteNo();
//****************************************************
//Open
static uint8_t functOpenProjectConfirm();
static uint8_t functSaveChangesCancelOpen();
static uint8_t functSaveChangesDontSaveOpen();
static uint8_t functSaveChangesSaveOpen();
//****************************************************
//Export
static uint8_t functExportSong();
static uint8_t functExportSongStems();
static uint8_t functExportPattern();
static uint8_t functExportPatternStems();
static uint8_t functExportToMOD();
static uint8_t functExportGoBack();

//****************************************************
static uint8_t functEnterName();
static uint8_t functSwitchModule(uint8_t button);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();
static  uint8_t functConfirmKey();

static  uint8_t functEncoder(int16_t value);

static uint8_t functStartGameModule()
{
	if(PE->isBusyFlag) return 1;
	PE->eventFunct(eventActivateGameModule,PE,0,0);
}

void cProjectEditor::update()
{
	refreshProcessingPopup();

/*	if(projectOptions > 0)
	{
		switch(projectOptions)
		{
			case mtProjectStartModeOpenLast:
			{
				listOnlyFolderNames("/Projects/");

				fileManager.openProjectStart(&locationFilesList[selectedLocation][0],projectTypeUserMade);

				loadProjectValues();

				functSwitchModule(interfaceButtonPattern);
				break;
			}
			default: break;
		}

		projectOptions = 0;
	}*/

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

	FM->setPadsGlobal(functPads);

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


	showDefaultScreen();
	setDefaultScreenFunct();
}

void cProjectEditor::stop()
{
	moduleRefresh = 0;
	projectOptions = 0;
	refreshCover = 0;
}

void cProjectEditor::setDefaultScreenFunct()
{


	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();
	keyboardActiveFlag = 0;
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

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);


	FM->setButtonObj(interfaceButton7, buttonPress, functStartGameModule);
	FM->setPotObj(interfacePot0, functEncoder, nullptr);



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



	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote = 36);

//	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);
	mtPadBoard.configureInstrumentPlayer(8);

	sampleRecorder.recorderConfig.gainLineIn = mtProject.values.gainLineIn;
	sampleRecorder.recorderConfig.gainMicHigh = mtProject.values.gainMicHigh;
	sampleRecorder.recorderConfig.gainMicLow = mtProject.values.gainMicLow;
	sampleRecorder.recorderConfig.gainRadio = mtProject.values.gainRadio;
	sampleRecorder.recorderConfig.monitor = mtProject.values.monitor;
	sampleRecorder.recorderConfig.radioFreq = mtProject.values.radioFreq;
	sampleRecorder.recorderConfig.source = mtProject.values.source;


	for(uint8_t i = 0; i<8; i++)
	{
		// paterny na trakach w performance mode
		if(mtProject.values.perfTracksPatterns[i] < 1 || mtProject.values.perfTracksPatterns[i] > 255)
		{
			mtProject.values.perfTracksPatterns[i] = 1;
		}

		// globalne mute trackow
		if(mtProject.values.trackMute[i] >= trackMasterModeCount) mtProject.values.trackMute[i] = 0;
	}

	for(uint8_t i = 0; i<12; i++)
	{
		if(mtProject.values.perfFxPlaces[i] > performanceFxesCount-1)
		{
			mtProject.values.perfFxPlaces[i] = (i+1 < performanceFxesCount) ? i+1 : 0;
		}
	}


	if(mtProject.values.globalTempo > 1000) mtProject.values.globalTempo = DEFAULT_TEMPO;

	if(mtProject.values.patternLength > 255) mtProject.values.patternLength = 32;




	//performanceMode.
	//uint8_t fxPlaces[12]

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
	strcpy(currentPatch,"Templates/New/project.bin");

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

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenProjectConfirm);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functSaveChangesCancelOpen);

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

	mtProject.mtProjectRemote.values.projectNotSavedFlag = 0;
	mtProject.values.projectNotSavedFlag = 0;

	if(fileManager.currentProjectName[0]) strcpy(PE->name, fileManager.currentProjectName);
	else strcpy(PE->name,"New Project");

	PE->showProcessingPopup("Saving project");

	return 1;
}

static uint8_t functSaveAsProject()
{
	if(PE->isBusyFlag) return 1;
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functSaveAsCancel);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsConfirm);
	PE->FM->setButtonObj(interfaceButtonShift, buttonPress, functConfirmKey);

	strcpy(PE->name,fileManager.currentProjectName);


	PE->newProjectNotSavedFlag = 0;
	PE->editPosition = strlen(PE->name);
	PE->keyboardPosition = BACKSPACE_PAD_1;
	PE->lastPressedPad = BACKSPACE_PAD_1;
	leds.setLED(BACKSPACE_PAD_1, 1, 31);
	leds.setLED(BACKSPACE_PAD_2, 1, 31);

	PE->keyboardActiveFlag = 1;

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
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportGoBack);

	PE->showExportWindow();
	return 1;
}

//===============================================================================================================
//New Project
void cProjectEditor::functShowSaveLastWindow()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functSaveChangesCancelNewProject);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functSaveChangesDontSaveNewProject);
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
	strcpy(currentPatch,"Templates/New/project.bin");

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

	mtProject.mtProjectRemote.values.projectNotSavedFlag = 0;
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

	if(fileManager.prepareSaveAs(PE->name,FileManager::saveAsChecking) == 0 )
	{
		PE->functShowOverwriteWindow();
		return 1;
	}


	mtProject.values.projectNotSavedFlag = 0;
	mtProject.mtProjectRemote.values.projectNotSavedFlag = 0;

	strcpy(fileManager.currentProjectName, PE->name);

	PE->savePopupFlag = 1;
	PE->savePopupDelay = 0;
	PE->isBusyFlag = 1;

	PE->showDefaultScreen();
	PE->showProcessingPopup("Saving project");

	return 1;
}

void cProjectEditor::functShowOverwriteWindow()
{
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functSaveAsOverwriteYes);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsOverwriteNo);

	PE->showOverwriteWindow();
}

static uint8_t functSaveAsOverwriteYes()
{
	if(PE->isBusyFlag) return 1;

	fileManager.prepareSaveAs(PE->name,FileManager::saveAsOverwrite);

	mtProject.values.projectNotSavedFlag = 0;
	mtProject.mtProjectRemote.values.projectNotSavedFlag = 0;

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

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functSaveAsCancel);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveAsConfirm);
	PE->FM->setButtonObj(interfaceButtonShift, buttonPress, functConfirmKey);

	PE->editPosition = strlen(PE->name);
	PE->keyboardPosition = BACKSPACE_PAD_1;
	PE->lastPressedPad = BACKSPACE_PAD_1;
	leds.setLED(BACKSPACE_PAD_1, 1, 31);
	leds.setLED(BACKSPACE_PAD_2, 1, 31);

	PE->keyboardActiveFlag = 1;

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

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functSaveChangesCancelOpen);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functSaveChangesDontSaveOpen);
	PE->FM->setButtonObj(interfaceButton7, buttonPress, functSaveChangesSaveOpen);

	showSaveLastWindow();
}

static uint8_t functSaveChangesCancelOpen()
{
	if(PE->isBusyFlag) return 1;
	PE->projectListActiveFlag = 0;
	PE->setDefaultScreenFunct();
	PE->showDefaultScreen();


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

	mtProject.mtProjectRemote.values.projectNotSavedFlag = 0;
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
//===============================================================================================================
//export
static uint8_t functExportSong()
{
	if(PE->isBusyFlag) return 1;

	return 1;
}
static uint8_t functExportSongStems()
{
	if(PE->isBusyFlag) return 1;

	return 1;
}
static uint8_t functExportPattern()
{
	if(PE->isBusyFlag) return 1;

	return 1;
}
static uint8_t functExportPatternStems()
{
	if(PE->isBusyFlag) return 1;

	return 1;
}
static uint8_t functExportToMOD()
{
	if(PE->isBusyFlag) return 1;

	return 1;
}
static uint8_t functExportGoBack()
{
	if(PE->isBusyFlag) return 1;
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

			if(sdLocation.exists("project.bin"))	//tylko jesli w folderze jest plik projektu
			{
				strcpy(&locationFilesList[foundProjectsCount][0],&locationFilesList[i][1]);

				foundProjectsCount++;
			}


			sdLocation.close();
		}
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
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionLeft][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
	return 1;
}
static  uint8_t functRight()
{
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionRight][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
	return 1;
}
static  uint8_t functUp()
{
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionUp][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
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
	if(PE->keyboardActiveFlag)
	{
		PE->keyboardPosition = valueMap[valueMapDirectionDown][PE->keyboardPosition];
		PE->showKeyboard();
		return 1;
	}
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
	if(PE->keyboardActiveFlag)
	{
		if(PE->editPosition > 31) return 1;

		//****************************************************ledy
		if(PE->lastPressedPad == BACKSPACE_PAD_1 || PE->lastPressedPad == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 0, 0);
			leds.setLED(BACKSPACE_PAD_2, 0, 0);
		}
		else if(PE->lastPressedPad == CAPS_LOCK_PAD_1 || PE->lastPressedPad == CAPS_LOCK_PAD_2) //capslock
		{
			if(PE->keyboardShiftFlag)
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, 10);
				leds.setLED(CAPS_LOCK_PAD_2, 1, 10);
			}
			else
			{
				leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
				leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
			}

		}
		else if(PE->lastPressedPad >= SPACE_PAD_1 && PE->lastPressedPad <= SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 0, 0);
			}
		}
		else
		{
			if(PE->lastPressedPad != F_PAD && PE->lastPressedPad != J_PAD) leds.setLED(PE->lastPressedPad,0,0);
			else leds.setLED(PE->lastPressedPad,1,10);
		}


		PE->lastPressedPad = keyPositionToPads[PE->keyboardPosition];

		if(keyPositionToPads[PE->keyboardPosition] == BACKSPACE_PAD_1 || keyPositionToPads[PE->keyboardPosition] == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 1, 31);
			leds.setLED(BACKSPACE_PAD_2, 1, 31);
		}
		else if(keyPositionToPads[PE->keyboardPosition] == CAPS_LOCK_PAD_1 || keyPositionToPads[PE->keyboardPosition] == CAPS_LOCK_PAD_2) //capslock
		{
			leds.setLED(CAPS_LOCK_PAD_1, 1, 31);
			leds.setLED(CAPS_LOCK_PAD_2, 1, 31);
		}
		else if(keyPositionToPads[PE->keyboardPosition] >= SPACE_PAD_1 && keyPositionToPads[PE->keyboardPosition] <=SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 1, 31);
			}
		}
		else
		{
			leds.setLED(keyPositionToPads[PE->keyboardPosition],1,31);
		}
		//////////////////////////////////////
		if(smallKeyboard[PE->keyboardPosition] > 1)
		{
			if(PE->editPosition == 31) return 1;
			uint8_t localNameLen = strlen(PE->name);
			if(PE->editPosition < localNameLen)
			{
				for(uint8_t i = localNameLen; i >= PE->editPosition ; i-- )
				{
					PE->name[i+1] = PE->name[i];
				}
			}

			PE->name[PE->editPosition] = PE->keyboardShiftFlag ? bigKeyboard[PE->keyboardPosition] : smallKeyboard[PE->keyboardPosition];

			PE->editPosition++;
		}
		else if(smallKeyboard[PE->keyboardPosition] == 0)
		{
			if(PE->editPosition == 0 ) return 1;

			PE->name[PE->editPosition-1] = 0;
			PE->editPosition--;


		}
		else if(smallKeyboard[PE->keyboardPosition] == 1)
		{
			PE->keyboardShiftFlag = ! PE->keyboardShiftFlag;
			PE->showKeyboard();

		}

		PE->showKeyboardEditName();
		return 1;
	}
	return 0;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if((state == 1) || (state == 2))
	{
		if(PE->keyboardActiveFlag)
		{
			if(PE->lastPressedPad == BACKSPACE_PAD_1 || PE->lastPressedPad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 0, 0);
				leds.setLED(BACKSPACE_PAD_2, 0, 0);
			}
			else if(PE->lastPressedPad == CAPS_LOCK_PAD_1 || PE->lastPressedPad == CAPS_LOCK_PAD_2) //capslock
			{
				if(PE->keyboardShiftFlag)
				{
					leds.setLED(CAPS_LOCK_PAD_1, 1, 10);
					leds.setLED(CAPS_LOCK_PAD_2, 1, 10);
				}
				else
				{
					leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
					leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
				}

			}
			else if(PE->lastPressedPad >= SPACE_PAD_1 && PE->lastPressedPad <= SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 0, 0);
				}
			}
			else
			{
				if(PE->lastPressedPad != F_PAD && PE->lastPressedPad != J_PAD) leds.setLED(PE->lastPressedPad,0,0);
				else leds.setLED(PE->lastPressedPad,1,10);
			}


			PE->lastPressedPad = pad;

			if(pad == BACKSPACE_PAD_1 || pad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 1, 31);
				leds.setLED(BACKSPACE_PAD_2, 1, 31);
			}
			else if(pad == CAPS_LOCK_PAD_1 || pad == CAPS_LOCK_PAD_2) //capslock
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, 31);
				leds.setLED(CAPS_LOCK_PAD_2, 1, 31);
			}
			else if(pad >= SPACE_PAD_1 && pad <=SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 1, 31);
				}
			}
			else
			{
				leds.setLED(pad,1,31);
			}

			PE->keyboardPosition = valueMapPads[pad];


			if(PE->editPosition > 31) return 1;
			if(smallKeyboard[PE->keyboardPosition] > 1)
			{
				if(PE->editPosition == 31) return 1;
				uint8_t localNameLen = strlen(PE->name);
				if(PE->editPosition < localNameLen)
				{
					for(uint8_t i = localNameLen; i >= PE->editPosition ; i-- )
					{
						PE->name[i+1] = PE->name[i];
					}
				}

				PE->name[PE->editPosition] = PE->keyboardShiftFlag ? bigKeyboard[PE->keyboardPosition] : smallKeyboard[PE->keyboardPosition];
				PE->name[PE->editPosition + 1] = 0;
				PE->editPosition++;
			}
			else if(smallKeyboard[PE->keyboardPosition] == 0)
			{
				if(PE->editPosition == 0 ) return 1;

				PE->name[PE->editPosition-1] = 0;
				PE->editPosition--;


			}
			else if(smallKeyboard[PE->keyboardPosition] == 1)
			{
				PE->keyboardShiftFlag = ! PE->keyboardShiftFlag;
//				PE->showKeyboard();
			}
			PE->showKeyboard();
			PE->showKeyboardEditName();
			return 1;
		}

		return 1;
	}
	else if(state == 0)
	{
		if(PE->keyboardActiveFlag)
		{

		}
	}




	return 1;
}

static  uint8_t functEncoder(int16_t value)
{

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


