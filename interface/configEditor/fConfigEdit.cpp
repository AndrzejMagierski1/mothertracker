

#include "mtTest.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"
#include "configEditor/configEditor.h"
#include "mtSequencer.h"
#include "mtConfig.h"

#include "patternEditor/patternEditor.h"

#include "mtAudioEngine.h"

#include "SI4703.h"

#include "core/interfacePopups.h"
#include "debugLog.h"


cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;
extern AudioControlSGTL5000 audioShield;


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


void cConfigEditor::update()
{

	if(processUpdate)
	{
		processUpdate = 0;
		prepareAndFlash();
	}

	eepromUpdate();

}

void cConfigEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mode = options;
	exitOnButtonRelease = 0;
//--------------------------------------------------------------------

	createConfigMenu();


//--------------------------------------------------------------------

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





	selectedPlace = 0;

	showDefaultConfigScreen();
	setConfigScreenFunct();


	activateLabelsBorder();
}


void cConfigEditor::stop()
{
	exitOnButtonRelease = 0;
	moduleRefresh = 0;
}

void cConfigEditor::turnOffPerformanceMode()
{
	if(sequencer.isPerformanceMode())
	{
		fileManager.loadPattern(mtProject.values.actualPattern);
		sequencer.switchRamPatternsNow();
		sequencer.exitPerformanceMode();
	}

	engine.performanceModeEndAll();

}

void cConfigEditor::turnOffRadio()
{
	audioShield.headphoneSourceSelect(0);
	radio.clearRDS();
	radio.resetSeekCallback();
	engine.setHeadphonesVolume(mtProject.values.volume);
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

//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################
//CE->eventFunct(eventActivateImageViewer,CE,0,0);

static uint8_t functActionButton(uint8_t button, uint8_t state)
{
	uint8_t updateFrame = 1;
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
				CE->changeMenuListPosition(1, -1);
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
				CE->changeMenuListPosition(1, 1);
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
				else CE->changeMenuListPosition(2, -1);
			}
			else
			{
				if(CE->configListShown) CE->selectedPlace = 2;
				else CE->selectedPlace = 3;
			}
			break;
		}
		case 6:
		{
			if(CE->selectedPlace == 2 || CE->selectedPlace == 3)
			{
				if(CE->configListShown) CE->changeConfigListPosition(1);
				else CE->changeMenuListPosition(2, 1);
			}
			else
			{
				if(CE->configListShown) CE->selectedPlace = 2;
				else CE->selectedPlace = 3;
			}
			break;
		}
		case 7:
		{
			if(CE->configListShown)
			{
				if(CE->flashingState) CE->updateFirmware();
				else CE->configListConfirm(CE->selectedConfigListPosition);
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

	if(updateFrame)
	{
		CE->activateLabelsBorder();
	}

	return 1;
}

//==============================================================================================================


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state != buttonPress) return 1;

	if(mtTest.runTestByCombinaion(pad)) 	CE->eventFunct(eventActivateTestingProcedure,CE,0,0);


	if(pad == 0)
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

	}




	return 1;
}


static  uint8_t functEncoder(int16_t value)
{
	if(CE->selectionActive) return 1;







		switch(CE->selectedPlace)
		{
		case 0: 	 break;
		case 1:		 break;
		case 2: 	 break;
		case 3: 	 break;

		}


	return 1;
}






static  uint8_t functLeft()
{
	if(CE->selectionActive) return 1;
	if(CE->frameData.multiSelActiveNum != 0) return 1;


	if(CE->selectedPlace > 0) CE->selectedPlace--;


	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(CE->selectionActive) return 1;
	if(CE->frameData.multiSelActiveNum != 0) return 1;


	uint8_t maxPlaces;
	maxPlaces = 3;
	if(CE->selectedPlace < (maxPlaces-1)) CE->selectedPlace++;


	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(CE->selectionActive) return 1;




		switch(CE->selectedPlace)
		{
		case 0: 	 break;
		case 1:		 break;
		case 2: 	 break;
		case 3: 	 break;

		case 17: 	break;

		}


	return 1;
}


static  uint8_t functDown()
{
	if(CE->selectionActive) return 1;




		switch(CE->selectedPlace)
		{
		case 0: 	 break;
		case 1:		 break;
		case 2: 	 break;
		case 3: 	 break;

		case 17: 	break;

		}


	return 1;
}




static  uint8_t functPlayAction()
{
	if(CE->selectionActive) return 1;

	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		sequencer.play();
	}
	else
	{
		sequencer.stop();
	}

	return 1;
}



static  uint8_t functRecAction()
{
	if(CE->selectionActive) return 1;

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(CE->selectionActive) return 1;

//	if(button != interfaceButtonPerformance) CE->turnOffPerformanceMode();
//	if(button != interfaceButtonSampleRec) CE->turnOffRadio();

	CE->eventFunct(eventSwitchModule,CE,&button,0);

	return 1;
}

//======================================================================================================================


void firmwareUpgradeActivate()
{
	CE->listAllFirmwares();

	CE->flashingState = 1;

	CE->changeLabelText(7, "Update");
	CE->showConfigList5(0, CE->firmwareFoundNum, CE->ptrfirmwareNamesList);

}

void firmwareUpgradeDeactivate()
{


	CE->changeLabelText(7, "");
	CE->hideConfigList();



}

void cConfigEditor::changeConfigListPosition(int16_t value)
{
	if(selectedConfigListPosition + value < 0) selectedConfigListPosition  = 0;
	else if(selectedConfigListPosition + value > configList.length-1) selectedConfigListPosition = configList.length-1;
	else selectedConfigListPosition += value;


	display.setControlValue(configListControl, selectedConfigListPosition);
	display.refreshControl(configListControl);


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
	uint8_t validFilesCount=0;

	sdLocation.close();
	if(sdLocation.open("/firmware", O_READ))
	{
		locationFileCount = sdLocation.createFilesListShort(0,&firmwareNamesList[0][0], firmware_list_max,firmware_name_length);

		sdLocation.close();

		if(locationFileCount > firmware_list_max)
		{
			locationFileCount = firmware_list_max;
		}

		for(uint8_t i = 0; i < locationFileCount; i++)
		{
			if(checkIfFirmwareValid(&firmwareNamesList[i][0]))
			{
				strncpy(&firmwareNamesList[validFilesCount][0], &firmwareNamesList[i][0], firmware_name_length);
				ptrfirmwareNamesList[validFilesCount] = &firmwareNamesList[validFilesCount][0];
				validFilesCount++;
			}
		}

		firmwareFoundNum = validFilesCount;
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

	SdFile fwinfo;

	fwinfo = SD.open("/firmware/_fwinfo", FILE_WRITE);
	fwinfo.write(&CE->firmwareNamesList[CE->selectedConfigListPosition][0], strlen(&CE->firmwareNamesList[CE->selectedConfigListPosition][0]));
	fwinfo.close();

	pinMode(BOOTLOADER_PIN,OUTPUT);
	digitalWrite(BOOTLOADER_PIN,LOW);

	while(1);
}

void cConfigEditor::showFlashingWarning()
{
	if(firmwareFoundNum)
	{
		FM->clearButtonsRange(interfaceButton0,interfaceButton7);

		FM->setButtonObj(interfaceButton7, buttonPress, prepareAndFlash);
		FM->setButtonObj(interfaceButton6, buttonPress, hideFlashingWarning);
		selectionActive=1;
		showFirmwareUpdatePopout();
	}
}

static uint8_t hideFlashingWarning()
{
	CE->selectionActive = 0;
	CE->start(0);

	return 1;
}




//==============================================================================================================


void cConfigEditor::saveConfigToEeprom()
{
	saveConfig();
}




