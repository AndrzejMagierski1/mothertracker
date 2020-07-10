

#include "mtTest.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
//#include "mtFileManager.h"
#include "fileManager.h"
#include "configEditor/configEditor.h"
#include "mtSequencer.h"
#include "mtConfig.h"
#include "keyScanner.h"

#include "MTP.h"

#include "patternEditor/patternEditor.h"

#include "mtAudioEngine.h"


#include "Si4703.h"

#include "core/interfacePopups.h"
#include "debugLog.h"
#include "configEditor.h"
#include "mtGridEditor.h"
#include "mtPadsBacklight.h"

cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;
extern AudioControlSGTL5000 audioShield;

//GRID SCREEN
static 	uint8_t	functSaveGridScreen();
static 	uint8_t	functCancelGridScreen();
static 	uint8_t	functConfirmGridScreen();
static 	uint8_t	functUpGridScreen();
static 	uint8_t	functDownGridScreen();
static 	uint8_t	functLeftGridScreen();
static 	uint8_t	functRightGridScreen();
static  uint8_t functActionButtonGridScreen(uint8_t button, uint8_t state);
static  uint8_t functPadsGridScreen(uint8_t pad, uint8_t state, int16_t velo);
static 	uint8_t functEncoderGridScreen(int16_t value);
//PAD SCREEN
static 	uint8_t	functConfirmPadScreen();
static 	uint8_t	functUpPadScreen();
static 	uint8_t	functDownPadScreen();
static 	uint8_t	functLeftPadScreen();
static 	uint8_t	functRightPadScreen();
static  uint8_t functPadsPadScreen(uint8_t pad, uint8_t state, int16_t velo);
static  uint8_t functActionButtonPadScreen(uint8_t button, uint8_t state);
static 	uint8_t functEncoderPadScreen(int16_t value);
//////////////
static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

//static  uint8_t functAction0();
//static  uint8_t functAction5();

static  uint8_t functActionButton(uint8_t button, uint8_t state);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


// config

static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);


uint8_t checkIfFirmwareValid(char *name);
//static uint8_t selectFirmware();

static uint8_t prepareAndFlash();
static uint8_t hideFlashingWarning();

static uint8_t closeCreditsButton();

void cConfigEditor::update()
{
	uint8_t managerStatus = newFileManager.getStatus();


	if(managerStatus == fmBrowseFirmwaresEnd)
	{
		firmwareFoundNum = newFileManager.getFirmwaresList(&ptrfirmwareNamesList);
		CE->listAllFirmwares();
		CE->flashingState = 1;
		CE->changeLabelText(7, "Update");
		CE->showConfigList5(3, 0, firmwareFoundNum, ptrfirmwareNamesList);
		CE->selectConfigList(3);
		newFileManager.clearStatus();
	}
	else if(managerStatus >=  fmError)
	{
		debugLog.addLine("Opretion Error");
		newFileManager.clearStatus();
		//setDefaultScreenFunct();
	}




	if(processUpdate)
	{
		processUpdate = 0;
		prepareAndFlash();
	}

}

void cConfigEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mode = options;
	exitOnButtonRelease = 0;

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);

	//FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
	//FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);


	// powraca tylko do 1 i 2 kolumny
	if(selectedPlace > 1) selectedPlace = 1;

	showDefaultConfigScreen();
	setConfigScreenFunct();


	//--------------------------------------------------------------------

	createConfigMenu();


	//--------------------------------------------------------------------


	activateLabelsBorder();
}


void cConfigEditor::stop()
{
	eepromUpdate(true);

	exitOnButtonRelease = 0;
	moduleRefresh = 0;
}


void cConfigEditor::setConfigScreenFunct()
{
	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);


//	FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
//	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButton0, functActionButton);
	FM->setButtonObj(interfaceButton1, functActionButton);
	FM->setButtonObj(interfaceButton2, functActionButton);
	FM->setButtonObj(interfaceButton3, functActionButton);
	FM->setButtonObj(interfaceButton4, functActionButton);
	FM->setButtonObj(interfaceButton5, functActionButton);
	FM->setButtonObj(interfaceButton6, functActionButton);
	FM->setButtonObj(interfaceButton7, functActionButton);

	FM->setPadsGlobal(functPads);

}

void cConfigEditor::setGridScreenFunction()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);
	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearAllPads();
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton0, functActionButtonGridScreen);
	FM->setButtonObj(interfaceButton1, functActionButtonGridScreen);
	FM->setButtonObj(interfaceButton2, functActionButtonGridScreen);

	FM->setButtonObj(interfaceButton5, buttonPress, functSaveGridScreen);
	FM->setButtonObj(interfaceButton6, buttonPress, functCancelGridScreen);
	FM->setButtonObj(interfaceButton7, buttonPress, functConfirmGridScreen);
	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeftGridScreen);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRightGridScreen);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUpGridScreen);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDownGridScreen);

	FM->setPotObj(interfacePot0, functEncoderGridScreen, nullptr);

	FM->setPadsGlobal(functPadsGridScreen);
}

void cConfigEditor::setPadScreenFunction()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);
	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearAllPads();
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton0, functActionButtonPadScreen);
	FM->setButtonObj(interfaceButton1, functActionButtonPadScreen);
	FM->setButtonObj(interfaceButton2, functActionButtonPadScreen);

	FM->setButtonObj(interfaceButton7, buttonPress, functConfirmPadScreen);
	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeftPadScreen);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRightPadScreen);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUpPadScreen);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDownPadScreen);

	FM->setPotObj(interfacePot0, functEncoderPadScreen, nullptr);

	FM->setPadsGlobal(functPadsPadScreen);
}

void cConfigEditor::reloadPadScreenDisplayedValue(uint8_t value)
{
	switch(value)
	{
	case 0:
		padScreenDisplayedValue[value] = map(mtGrid.pad[gridEditor.getSelectedPad()].note,MIN_NOTE,MAX_NOTE, 0, 100);
		interfaceGlobals.padNamesPointer[gridEditor.getSelectedPad()] = (char*)mtNotes[mtGrid.pad[gridEditor.getSelectedPad()].note];
		break;
	case 1:
		padScreenDisplayedValue[value] = mtGrid.pad[gridEditor.getSelectedPad()].microtune;
		sprintf(interfaceGlobals.padFinetuneNames[gridEditor.getSelectedPad()], "%d", mtGrid.pad[gridEditor.getSelectedPad()].microtune);
		break;
	case 2:
		padScreenDisplayedValue[value] = mtGrid.pad[gridEditor.getSelectedPad()].ledEnable ? 0 : 1;
		break;
	default: break;
	}
}

//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################
static uint8_t functActionButton(uint8_t button, uint8_t state)
{
	if(CE->updatePopupShown)
	{
		if(state != buttonPress) return 1;

		if(button == 6)
		{
			CE->updateFirmware();
		}
		else if(button == 7)
		{
			CE->cancelUpdateFirmware();
		}

		return 1;
	}


	if(state == buttonPress)
	{
		switch(button)
		{
		case 0:
		{
			if(CE->selectedPlace == 0)
			{
				CE->changeMenuListPosition(0, -1);
			}
			else
			{
				CE->selectedPlace = 0;
			}
			break;
		}
		case 1:
		{
			if(CE->selectedPlace == 0)
			{
				CE->changeMenuListPosition(0, 1);
			}
			else
			{
				CE->selectedPlace = 0;
			}
			break;
		}
		case 2:
		{
			if(CE->selectedPlace == 1)
			{
				CE->changeMenuListPosition(1, -1, 1);
			}
			else
			{
				CE->selectedPlace = 1;
			}
			break;
		}
		case 3:
		{
			if(CE->selectedPlace == 1)
			{
				CE->changeMenuListPosition(1, 1, 1);
			}
			else
			{
				CE->selectedPlace = 1;
			}
			break;
		}
		case 4:
		{
			if(CE->selectedPlace == 1)
			{
				CE->executeSelectedListItem(1);
			}
			else
			{
				CE->selectedPlace = 1;
			}
			break;
		}
		case 5:
		{
			if(CE->selectedPlace == 2 || CE->selectedPlace == 3)
			{
				if(CE->configListShown) CE->changeConfigListPosition(-1);
				else CE->changeMenuListPosition(2, -1, 1);
			}
			else
			{
				if(CE->configListShown) CE->selectedPlace = 2;
				else if(CE->secondSubmenuShown) CE->selectedPlace = 3;
			}
			break;
		}
		case 6:
		{
			if(CE->selectedPlace == 2 || CE->selectedPlace == 3)
			{
				if(CE->configListShown) CE->changeConfigListPosition(1);
				else CE->changeMenuListPosition(2, 1, 1);
			}
			else
			{
				if(CE->configListShown) CE->selectedPlace = 2;
				else if(CE->secondSubmenuShown) CE->selectedPlace = 3;
			}
			break;
		}
		case 7:
		{
			if(CE->configListShown)
			{
				if(CE->selectedPlace == 2)
				{
					if(CE->flashingState) CE->showFirmwareUpdatePopout();
					else CE->configListConfirm(CE->selectedConfigListPosition);
				}
				else
				{
					CE->selectedPlace = 2;
				}
			}
			else if(CE->selectedPlace == 3)
			{
				CE->executeSelectedListItem(2);
			}
			else
			{
				if(CE->secondSubmenuShown) CE->selectedPlace = 3;
			}

			break;
		}

		default:
			break;
		}
	}
	else if(state == buttonRelease)
	{

	}

	CE->activateLabelsBorder();

	return 1;
}

//==============================================================================================================


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(CE->updatePopupShown) return 1;
	if(state != buttonPress) return 1;


	if(mtTest.runTestByCombinaion(pad)) 	CE->eventFunct(eventActivateTestingProcedure,CE,0,0);


	if(pad == 0 && DEBUG_LOG_FROM_CONFIG_PAD)
	{
		mtPopups.config(4, (strPopupStyleConfig*)&popupDebugConfig);
		if(debugLog.toggleState())
		{
			mtPopups.show(4, "Debug log enabled");
		}
		else
		{
			mtPopups.show(4, "Debug log disabled");
		}
		CE->saveConfigToEeprom();
	}

	else if(pad == 11 && FILE_TRANS_FROM_CONFIG_PAD)
	{
		mtPopups.config(4, (strPopupStyleConfig*)&popupDebugConfig);
		if(mtpd.toggleState())
		{
			mtPopups.show(4, "File transfer enabled","This is an experimental feature, use it carefully");
		}
		else
		{
			mtPopups.show(4, "File transfer disabled");
		}
		CE->saveConfigToEeprom();
	}



	return 1;
}


static  uint8_t functEncoder(int16_t value)
{
	if(CE->updatePopupShown) return 1;

	switch(CE->selectedPlace)
	{
	case 0:
	{
		CE->changeMenuListPosition(0, value);
		break;
	}
	case 1:
	{
		CE->changeMenuListPosition(1, value);
		break;
	}
	case 2:
	{
		//if(CE->configListShown) CE->changeConfigListPosition(value);
		//else CE->changeMenuListPosition(2, value);

		if(CE->configListShown) CE->changeConfigListPosition(value);
		break;
	}
	case 3:
	{
		//if(CE->configListShown) CE->changeConfigListPosition(value);
		//else CE->changeMenuListPosition(2, value);

		CE->changeMenuListPosition(2, value);
		break;
	}


	default:
		break;
	}



	return 1;
}






static  uint8_t functLeft()
{
	if(CE->updatePopupShown) return 1;


	if(CE->selectedPlace == 3) 		CE->selectedPlace = 1;
	else if(CE->selectedPlace > 0) 	CE->selectedPlace--;


	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(CE->updatePopupShown) return 1;


	if(CE->selectedPlace == 0 && CE->submenuShown)
	{
		CE->selectedPlace = 1;
	}
	else if(CE->selectedPlace == 1 && CE->secondSubmenuShown)
	{
		CE->selectedPlace = 3;
	}
	else if(CE->selectedPlace == 1 && CE->configListShown)
	{
		CE->selectedPlace = 2;
	}
	else if(CE->selectedPlace == 1)
	{
		CE->executeSelectedListItem(1);
	}
	else if(CE->selectedPlace == 2)
	{
		if(CE->configListShown)
		{
			//CE->configListConfirm(CE->selectedConfigListPosition);
		}
	}
	else if(CE->selectedPlace == 3)
	{
		if(CE->secondSubmenuShown)
		{
			CE->executeSelectedListItem(2);
		}
	}

	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(CE->updatePopupShown) return 1;


	switch(CE->selectedPlace)
	{
	case 0:
	{
		CE->changeMenuListPosition(0, -1);
		break;
	}
	case 1:
	{
		CE->changeMenuListPosition(1, -1);
		break;
	}
	case 2:
	{
		//if(CE->configListShown) CE->changeConfigListPosition(value);
		//else CE->changeMenuListPosition(2, value);
		if(CE->configListShown) CE->changeConfigListPosition(-1);
		break;
	}
	case 3:
	{
		//if(CE->configListShown) CE->changeConfigListPosition(value);
		//else CE->changeMenuListPosition(2, value);
		CE->changeMenuListPosition(2, -1);
		break;
	}
	default:
		break;
	}


	return 1;
}


static  uint8_t functDown()
{
	if(CE->updatePopupShown) return 1;

	switch(CE->selectedPlace)
	{
	case 0:
	{
		CE->changeMenuListPosition(0, 1);
		break;
	}
	case 1:
	{
		CE->changeMenuListPosition(1, 1);
		break;
	}
	case 2:
	{
		//if(CE->configListShown) CE->changeConfigListPosition(value);
		//else CE->changeMenuListPosition(2, value);
		if(CE->configListShown) CE->changeConfigListPosition(1);
		break;
	}
	case 3:
	{
		//if(CE->configListShown) CE->changeConfigListPosition(value);
		//else CE->changeMenuListPosition(2, value);
		CE->changeMenuListPosition(2, 1);
		break;
	}
	default:
		break;
	}


	return 1;
}
////////////////////////////////////////////////GRID SCREEN
static 	uint8_t	functSaveGridScreen()
{
	return 1;
}

static 	uint8_t	functCancelGridScreen()
{
	gridEditor.close();
	CE->setConfigScreenFunct();

	CE->hideGridScreen();
	display.setControlShow(CE->configBasemenuListControl);
	display.refreshControl(CE->configBasemenuListControl);
	display.setControlShow(CE->configSubmenuListControl);
	display.refreshControl(CE->configSubmenuListControl);
	CE->showDefaultConfigScreen();
	CE->hideSecondSubmenu();
	padsBacklight.clearAllPads(0, 1, 0);
	return 1;
}
static 	uint8_t	functConfirmGridScreen()
{
	gridEditor.confirmSelectedPad();
	CE->setPadScreenFunction();

	CE->hideGridScreen();
	CE->showPadScreen();
	return 1;
}
static 	uint8_t	functUpGridScreen()
{
	gridEditor.changeSelectedPad(mtGridEditor::enChangePadDirectionUp);
	CE->refreshGridScreen();
	return 1;
}
static 	uint8_t	functDownGridScreen()
{
	gridEditor.changeSelectedPad(mtGridEditor::enChangePadDirectionDown);
	CE->refreshGridScreen();
	return 1;
}
static 	uint8_t	functLeftGridScreen()
{
	gridEditor.changeSelectedPad(mtGridEditor::enChangePadDirectionLeft);
	CE->refreshGridScreen();
	return 1;
}
static 	uint8_t	functRightGridScreen()
{
	gridEditor.changeSelectedPad(mtGridEditor::enChangePadDirectionRight);
	CE->refreshGridScreen();
	return 1;
}
static  uint8_t functActionButtonGridScreen(uint8_t button, uint8_t state)
{
	if(state == buttonPress)
	{
		CE->selectedPlaceGridScreen = button;
		CE->refreshGridScreenFrame();
	}

	return 1;
}
static  uint8_t functPadsGridScreen(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == buttonPress)
	{
		padsBacklight.setFrontLayer(1, mtConfig.values.padsLightFront, pad);
		gridEditor.changeSelectedPad(pad);
		CE->refreshGridScreen();
	}
	else if(state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0, 0, pad);
	}

	return 1;
}

static 	uint8_t functEncoderGridScreen(int16_t value)
{

	switch(CE->selectedPlaceGridScreen)
	{
		case 0: gridEditor.changeNote(value);			break;
		case 1: gridEditor.changeMicrotune(value);		break;
		case 2: gridEditor.changeLedEnable(-value);		break;
		default: break;
	}
	CE->reloadPadScreenDisplayedValue(CE->selectedPlaceGridScreen);
	CE->refreshPadValue(CE->selectedPlaceGridScreen,enScreenType::screenTypeGridScreen);
	display.setControlValue(CE->gridPadsControl,gridEditor.getSelectedPad());
	display.refreshControl(CE->gridPadsControl);


	return 1;
}
//////////////////////////////////////////////////////////
//PAD SCREEN
static 	uint8_t	functConfirmPadScreen()
{
	CE->setGridScreenFunction();
	CE->hidePadScreen();
	CE->showGridScreen();
	gridEditor.cancelSelectedPad();
	return 1;
}
static 	uint8_t	functUpPadScreen()
{
	switch(CE->selectedPlacePadScreen)
	{
		case 0: gridEditor.changeNote(1); 			break;
		case 1: gridEditor.changeMicrotune(1);		break;
		case 2: gridEditor.changeLedEnable(1);		break;
		default: break;
	}
	CE->reloadPadScreenDisplayedValue(CE->selectedPlacePadScreen);
	CE->refreshPadValue(CE->selectedPlacePadScreen, enScreenType::screenTypePadScreen);
	return 1;
}
static 	uint8_t	functDownPadScreen()
{
	switch(CE->selectedPlacePadScreen)
	{
		case 0: gridEditor.changeNote(-1);			break;
		case 1: gridEditor.changeMicrotune(-1);		break;
		case 2: gridEditor.changeLedEnable(-1);		break;
		default: break;
	}
	CE->reloadPadScreenDisplayedValue(CE->selectedPlacePadScreen);
	CE->refreshPadValue(CE->selectedPlacePadScreen, enScreenType::screenTypePadScreen);
	return 1;
}
static 	uint8_t	functLeftPadScreen()
{
	if(CE->selectedPlacePadScreen > 0) CE->selectedPlacePadScreen--;
	CE->refreshPadScreenFrame();
	return 1;
}
static 	uint8_t	functRightPadScreen()
{
	if(CE->selectedPlacePadScreen < 2) CE->selectedPlacePadScreen++;
	CE->refreshPadScreenFrame();
	return 1;
}
static  uint8_t functPadsPadScreen(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == buttonPress)
	{
		padsBacklight.setFrontLayer(1, mtConfig.values.padsLightFront, pad);
		gridEditor.changeSelectedPad(pad);
		for(uint8_t i = 0 ; i < 3; i++)
		{
			CE->reloadPadScreenDisplayedValue(i);
			CE->refreshPadValue(i,enScreenType::screenTypePadScreen);
		}
		sprintf(CE->padScreenTitleLabelName,"Grid Editor Pad %d", gridEditor.getSelectedPad() + 1);

		display.setControlText(CE->titleLabel, CE->padScreenTitleLabelName);
		display.refreshControl(CE->titleLabel);
	}
	else if(state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0, 0, pad);
	}

	return 1;
}
static  uint8_t functActionButtonPadScreen(uint8_t button, uint8_t state)
{
	if(state == buttonPress)
	{
		CE->selectedPlacePadScreen = button;
	}
	CE->refreshPadScreenFrame();
	return 1;
}
static 	uint8_t functEncoderPadScreen(int16_t value)
{
	switch(CE->selectedPlacePadScreen)
	{
		case 0: gridEditor.changeNote(value);			break;
		case 1: gridEditor.changeMicrotune(value);		break;
		case 2: gridEditor.changeLedEnable(-value);		break;
		default: break;
	}
	CE->reloadPadScreenDisplayedValue(CE->selectedPlacePadScreen);
	CE->refreshPadValue(CE->selectedPlacePadScreen,enScreenType::screenTypePadScreen);
	return 1;
}
//////////////


static  uint8_t functPlayAction()
{
	if(CE->updatePopupShown) return 1;


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



static  uint8_t functRecAction()
{
	if(CE->updatePopupShown) return 1;

	////////////////////////TEST GRID EDITOR//////////////////////


//	CE->setGridScreenFunction();
//	CE->showGridScreen();
//	gridEditor.open();


	////////////////////////TEST GRID EDITOR//////////////////////

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(CE->updatePopupShown) return 1;

//	if(button != interfaceButtonPerformance) CE->turnOffPerformanceMode();
//	if(button != interfaceButtonSampleRec) CE->turnOffRadio();

	CE->eventFunct(eventSwitchModule,CE,&button,0);

	return 1;
}

//======================================================================================================================


void firmwareUpgradeActivate()
{
	newFileManager.browseFirmwares();


}

void firmwareUpgradeDeactivate()
{
	CE->hideConfigList();

	CE->changeLabelText(7, "");


}

void cConfigEditor::changeConfigListPosition(int16_t value)
{
	if(selectedConfigListPosition + value < 0) selectedConfigListPosition  = 0;
	else if(selectedConfigListPosition + value > configList.length-1) selectedConfigListPosition = configList.length-1;
	else selectedConfigListPosition += value;


	display.setControlValue(configListControl, selectedConfigListPosition);
	display.refreshControl(configListControl);


}


void cConfigEditor::cancelUpdateFirmware()
{
	firmwareUpgradeDeactivate();

	CE->updatePopupShown = 0;

	hideFlashingWarning();
}


void cConfigEditor::updateFirmware()
{
	if(firmwareFoundNum == 0) return;
	if(selectedConfigListPosition > firmwareFoundNum) return;

	processUpdate = 1;
}


//======================================================================================================================
//master
void cConfigEditor::listAllFirmwares()
{
	uint8_t locationFileCount=0;
	//uint8_t validFilesCount=0;

	sdLocation.close();
	if(sdLocation.open("/Firmware", O_READ))
	{
		locationFileCount = sdLocation.createFilesList(0, ptrfirmwareNamesList, firmware_list_max, 4000, 3);

		sdLocation.close();


//		if(locationFileCount > firmware_list_max)
//		{
//			locationFileCount = firmware_list_max;
//		}

//		for(uint8_t i = 0; i < locationFileCount; i++)
//		{
//			if(!checkIfFirmwareValid(ptrfirmwareNamesList[i]))
//			{
//				std::swap(ptrfirmwareNamesList[i]);
//
//				strncpy(&firmwareNamesList[validFilesCount][0], &firmwareNamesList[i][0], firmware_name_length);
//				ptrfirmwareNamesList[validFilesCount] = &firmwareNamesList[validFilesCount][0];
//				validFilesCount++;
//			}
//		}
//
//		firmwareFoundNum = locationFileCount;

		// sortowanie

		bool notSorted = 1;
//		char strBuff[40];
		while (notSorted)
		{
			notSorted = 0;
			for (uint8_t a = 0; a < locationFileCount - 1; a++)
			{
				if (strcasecmp(ptrfirmwareNamesList[a],
							   ptrfirmwareNamesList[a + 1]) > 0)
				{
					std::swap(ptrfirmwareNamesList[a], ptrfirmwareNamesList[a+1]);

//					strcpy(strBuff,
//							firmwareNamesList[a]);
//
//					strcpy(firmwareNamesList[a],
//							firmwareNamesList[a + 1]);
//
//					strcpy(firmwareNamesList[a + 1],
//							strBuff);
					notSorted = 1;
				}
			}
		}
		///




		firmwareFoundNum = locationFileCount;

	}
	else
	{
		firmwareFoundNum=0;
	}
}

uint8_t checkIfFirmwareValid(char *name)
{
	uint8_t nameLength;

	nameLength=strlen(name);

	if(name[nameLength-1] == 'f' && name[nameLength-2] == 't' && name[nameLength-3] == 'p' && name[nameLength-4] == '.')
	{
		if(name[0] == 'P' && name[1] == 'o' && name[2] == 'l' && name[3] == 'y' && name[4] == 'e' && name[5] == 'n'&& name[6] == 'd')
		{
			return 1;
		}
	}

	if(name[nameLength-1] == 'x' && name[nameLength-2] == 'e' && name[nameLength-3] == 'h' && name[nameLength-4] == '.')
	{
		if(name[0] == 'm' && name[1] == 't')
		{
			return 1;
		}
	}



	return 0;
}

static uint8_t prepareAndFlash()
{
	if(SD.exists("/Firmware/_fwinfo")) // plik nie powinien istniec, bootloader sam go usunie
	{
		SD.remove("/Firmware/_fwinfo");
	}

	saveStartState(1); // po zakonczwniu wgrywania firmaware zapewnia prawidlowy restart

	SdFile fwinfo;

	fwinfo = SD.open("/Firmware/_fwinfo", FILE_WRITE);
	fwinfo.write(CE->ptrfirmwareNamesList[CE->selectedConfigListPosition], strlen(CE->ptrfirmwareNamesList[CE->selectedConfigListPosition]));
	fwinfo.close();

	pinMode(BOOTLOADER_PIN,OUTPUT);
	digitalWrite(BOOTLOADER_PIN,LOW);

	while(1)
	{
		delay(1);
	}
}

void cConfigEditor::showFlashingWarning()
{
	if(firmwareFoundNum)
	{
		//FM->clearButtonsRange(interfaceButton0,interfaceButton7);
		//FM->setButtonObj(interfaceButton7, buttonPress, prepareAndFlash);
		//FM->setButtonObj(interfaceButton6, buttonPress, hideFlashingWarning);
		updatePopupShown=1;
		showFirmwareUpdatePopout();
	}
}

static uint8_t hideFlashingWarning()
{
	CE->updatePopupShown = 0;
	CE->start(0);

	return 1;
}


//==============================================================================================================
// credits

void cConfigEditor::openCredits()
{
	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);

	FM->setButtonObj(interfaceButton7, buttonPress, closeCreditsButton);

	FM->setPadsGlobal(functPads);

	hideConfigMenu();
	showCreditsControls();
}

void cConfigEditor::closeCredits()
{
	hideCreditsControls();
	start(0);
}


static uint8_t closeCreditsButton()
{
	CE->closeCredits();

	return 1;
}

void openCreditsAction()
{
	CE->openCredits();
}


//==============================================================================================================

//==============================================================================================================

//==============================================================================================================

void cConfigEditor::saveConfigToEeprom()
{
	saveConfig();
}

//==============================================================================================================

//==============================================================================================================

//==============================================================================================================

void openManual1Action()
{
	uint8_t folderIndex = 0;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual2Action()
{
	uint8_t folderIndex = 1;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual3Action()
{
	uint8_t folderIndex = 2;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual4Action()
{
	uint8_t folderIndex = 3;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual5Action()
{
	uint8_t folderIndex = 4;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual6Action()
{
	uint8_t folderIndex = 5;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual7Action()
{
	uint8_t folderIndex = 6;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual8Action()
{
	uint8_t folderIndex = 7;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual9Action()
{
	uint8_t folderIndex = 8;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual10Action()
{
	uint8_t folderIndex = 9;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual11Action()
{
	uint8_t folderIndex = 10;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual12Action()
{
	uint8_t folderIndex = 11;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual13Action()
{
	uint8_t folderIndex = 12;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual14Action()
{
	uint8_t folderIndex = 13;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}

void openManual15Action()
{
	uint8_t folderIndex = 14;
	CE->eventFunct(eventActivateImageViewer,CE,&folderIndex,0);
}



