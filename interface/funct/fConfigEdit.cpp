



#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"
#include "configEditor.h"
#include "mtSequencer.h"

cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static  uint8_t functAction0();
static  uint8_t functAction5();

static  uint8_t functActionButton(uint8_t button, uint8_t state);

// config
static  uint8_t functConfigGroup(uint8_t button);




//master
static  uint8_t functSelectVolume();
static  uint8_t functSelectReverbSize();
static  uint8_t functSelectReverbDamping();
static  uint8_t functSelectLimiterAttack();
static  uint8_t functSelectLimiterRelease();
static  uint8_t functSelectLimiterTreshold();
static  uint8_t functSelectBitDepth();



//master tracks



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);


static  uint8_t functSwitchModeConfig(uint8_t state);
static  uint8_t functSwitchModeMaster(uint8_t state);

uint8_t checkIfFirmwareValid(char *name);
static uint8_t selectFirmware();
uint8_t flashFirmware();




void cConfigEditor::update()
{

	if(processUpdate)
	{
		processUpdate = 0;

		pinMode(BOOTLOADER_PIN,OUTPUT);
		digitalWrite(BOOTLOADER_PIN,LOW);

		while(1);
	}

}

void cConfigEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mode = options;
	exitOnButtonRelease = 0;
//--------------------------------------------------------------------

//--------------------------------------------------------------------


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);




	switch(mode)
	{
	case mtConfigModeDefault:
	{
		resizeLabelConfigDefault();

		showDefaultConfigScreen();
		setConfigScreenFunct();
		if(selectedConfigGroup == configDefaultFirmware)
		{
			showFirmwareMenu();
		}



		break;
	}
	case mtConfigModeMaster:
	{
		selectedPlace[1] = 0;
		resizeLabelConfigMaster();

		showMasterScreen();
		setMasterScreenFunct();


		break;
	}
	}


	activateLabelsBorder();
}


void cConfigEditor::stop()
{
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

	FM->setButtonObj(interfaceButton6, buttonPress, functConfigGroup);
	FM->setButtonObj(interfaceButton7, buttonPress, functConfigGroup);

	FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButton0, functActionButton);
	FM->setButtonObj(interfaceButton1, functActionButton);
	FM->setButtonObj(interfaceButton2, functActionButton);
	FM->setButtonObj(interfaceButton3, functActionButton);
	FM->setButtonObj(interfaceButton4, functActionButton);
	FM->setButtonObj(interfaceButton5, functActionButton);
	FM->setButtonObj(interfaceButton6, functActionButton);

}

void cConfigEditor::setMasterScreenFunct()
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





	FM->setButtonObj(interfaceButton0, buttonPress, functSelectVolume);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectReverbSize);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectReverbDamping);
	FM->setButtonObj(interfaceButton3, buttonPress, functSelectBitDepth);
	FM->setButtonObj(interfaceButton4, buttonPress, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton5, buttonPress, functSelectLimiterRelease);
	FM->setButtonObj(interfaceButton6, buttonPress, functSelectLimiterTreshold);




	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################



static uint8_t functActionButton(uint8_t button, uint8_t state)
{

	if(state == buttonPress)
	{

		switch(button)
		{
		case 0:
		{
			break;
		}
		case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
		case 4:
		{
			break;
		}
		case 5:
		{

			CE->eventFunct(eventActivateImageViewer,CE,0,0);

			break;
		}


		default: break;
		}


	}
	else if(state == buttonRelease)
	{

	}





	return 1;
}

//==============================================================================================================

static  uint8_t functConfigGroup(uint8_t button)
{
	if(CE->selectionActive) return 1;

	if(button == interfaceButton6)
	{
		CE->changeConfigGroupSelection(-1);
	}
	else //if(button == interfaceButton7)
	{
		CE->changeConfigGroupSelection(1);
	}

	CE->selectedPlace[0] = 6;
	CE->activateLabelsBorder();


	return 1;
}

static  uint8_t functEncoder(int16_t value)
{
	if(CE->selectionActive) return 1;

	uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

	switch(mode_places)
	{
	case 0: CE->changeSelectionInGroup(value);		 break;
	case 1:	CE->changeSelectionInGroup(value);	 	 break;
	case 2: 	 break;
	case 3: 	 break;
	case 4: 	 break;
	case 5: 	 break;
	case 6: CE->changeConfigGroupSelection(value);	 break;
	case 7: 	 break;

	case 10: CE->changeVolume(value);			break;
	case 11: CE->changeReverbRoomSize(value);	break;
	case 12: CE->changeReverbDamping(value);	break;
	case 13: CE->changeBitDepth(value);			break;
	case 14: CE->changeLimiterAttack(value);	break;
	case 15: CE->changeLimiterRelease(value);	break;
	case 16: CE->changeLimiterTreshold(value);	break;
	case 17: 	break;

	}






	return 1;
}






static  uint8_t functLeft()
{
	if(CE->selectionActive) return 1;

	if(CE->selectedPlace[CE->mode] > 0) CE->selectedPlace[CE->mode]--;

	uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

	switch(mode_places)
	{
		case 0:	CE->selectedPlace[CE->mode] = 1; break;
		case 1:	break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6:
		{
		CE->selectedPlace[CE->mode] = 6;
		break;
		}


		case 10: 	break;
		case 11: 	break;
		case 12: 	break;
		case 13: 	break;
		case 14: 	break;
		case 15: 	break;
		case 16: 	break;
		case 17: 	break;

	}

	CE->activateLabelsBorder();


	return 1;
}


static  uint8_t functRight()
{
	if(CE->selectionActive) return 1;

	if(CE->selectedPlace[CE->mode] < CE->frameData.placesCount-1) CE->selectedPlace[CE->mode]++;

	uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

	switch(mode_places)
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6:
		{
		CE->selectedPlace[CE->mode] = 6;
		break;
		}


		case 10: 	break;
		case 11: 	break;
		case 12: 	break;
		case 13: 	break;
		case 14: 	break;
		case 15: 	break;
		case 16: 	break;
		case 17: 	break;

	}

	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(CE->selectionActive) return 1;

	uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

	switch(mode_places)
	{
	case 0:
		if(CE->selectedConfigGroup == configDefaultFirmware)
		{
			CE->changeFirmwareSelection(-1);
		}
		break;

	case 1:
		if(CE->selectedConfigGroup == configDefaultFirmware)
		{
			CE->changeFirmwareSelection(-1);
		}
		break;
	case 2: 	 break;
	case 3: 	 break;
	case 4: 	 break;
	case 5: 	 break;
	case 6: CE->changeConfigGroupSelection(-1);	 break;
	case 7: 	 break;

	case 10: CE->changeVolume(1);			break;
	case 11: CE->changeReverbRoomSize(1);	break;
	case 12: CE->changeReverbDamping(1);	break;
	case 13: CE->changeBitDepth(1);			break;
	case 14: CE->changeLimiterAttack(1);	break;
	case 15: CE->changeLimiterRelease(1);	break;
	case 16: CE->changeLimiterTreshold(1);	break;
	case 17: 	break;

	}

	return 1;
}


static  uint8_t functDown()
{
	if(CE->selectionActive) return 1;

	uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

	switch(mode_places)
	{
	case 0:
		if(CE->selectedConfigGroup == configDefaultFirmware)
		{
			CE->changeFirmwareSelection(1);

		}
		break;
	case 1:
		if(CE->selectedConfigGroup == configDefaultFirmware)
		{
			CE->changeFirmwareSelection(1);

		}
		break;
	case 2: 	 break;
	case 3: 	 break;
	case 4: 	 break;
	case 5: 	 break;
	case 6: CE->changeConfigGroupSelection(1);	 break;
	case 7: 	 break;

	case 10: CE->changeVolume(-1);			break;
	case 11: CE->changeReverbRoomSize(-1);	break;
	case 12: CE->changeReverbDamping(-1);	break;
	case 13: CE->changeBitDepth(-1);		break;
	case 14: CE->changeLimiterAttack(-1);	break;
	case 15: CE->changeLimiterRelease(-1);	break;
	case 16: CE->changeLimiterTreshold(-1);	break;
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

static  uint8_t functSelectVolume()
{
	CE->selectedPlace[mtConfigModeMaster] = 0;
	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functSelectReverbSize()
{
	CE->selectedPlace[mtConfigModeMaster] = 1;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbDamping()
{
	CE->selectedPlace[mtConfigModeMaster] = 2;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterAttack()
{
	CE->selectedPlace[mtConfigModeMaster] = 4;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterRelease()
{
	CE->selectedPlace[mtConfigModeMaster] = 5;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterTreshold()
{
	CE->selectedPlace[mtConfigModeMaster] = 6;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectBitDepth()
{
	CE->selectedPlace[mtConfigModeMaster] = 3;
	CE->activateLabelsBorder();

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(CE->selectionActive) return 1;

	CE->eventFunct(eventSwitchModule,CE,&button,0);

	return 1;
}

static  uint8_t functSwitchModeConfig(uint8_t state)
{
	if(CE->selectionActive) return 1;

	if(state == buttonPress)
	{

		if(CE->mode != mtConfigModeDefault)
		{
			CE->mode = 0;

			CE->resizeLabelConfigDefault();
			CE->showDefaultConfigScreen();
			CE->setConfigScreenFunct();

			if(CE->selectedConfigGroup == configDefaultFirmware)
			{
				CE->showFirmwareMenu();
			}
		}

		CE->activateLabelsBorder();
	}

	return 1;
}

static  uint8_t functSwitchModeMaster(uint8_t state)
{
	if(CE->selectionActive) return 1;

	if(state == buttonPress)
	{

		if(CE->mode != mtConfigModeMaster)
		{
			CE->resizeLabelConfigMaster();

			if(CE->selectedConfigGroup == configDefaultFirmware)
			{
				CE->hideFirmwareMenu();
			}

			CE->mode = 1;
			CE->showMasterScreen();
			CE->setMasterScreenFunct();
			//FM->clearButtonsRange(interfaceButton0,interfaceButton7);

			CE->activateLabelsBorder();




			return 0;
		}



	}
	else if(state == buttonHold)
	{
//		CE->resizeLabelConfigMaster();

		CE->exitOnButtonRelease = 1;
		CE->selectedPlace[mtConfigModeMaster] = 0;
		CE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(CE->exitOnButtonRelease) CE->eventFunct(eventSwitchToPreviousModule,CE,0,0);
	}

	return 1;
}

static uint8_t functAction0()
{
	if(CE->selectedConfigGroup == configDefaultFirmware)
	{
		if(CE->selectionActive)
		{
			CE->processUpdate = 1;



			//while(1);// program sie zatrzymuje do czasu przejecia kontroli przez bootloader
		}
	}

	return 1;
}

static uint8_t functAction5()
{
	if(CE->selectedConfigGroup == configDefaultFirmware)
	{
		if(CE->selectionActive)
		{
			CE->hideWarning();
		}
	}

	return 1;
}


//======================================================================================================================


void cConfigEditor::changeConfigGroupSelection(int16_t value)
{


	if(selectedConfigGroup + value < 0) selectedConfigGroup = 0;
	else if(selectedConfigGroup + value > mtConfigGroupsCount-1) selectedConfigGroup = mtConfigGroupsCount-1;
	else  selectedConfigGroup += value;

	if(selectedConfigGroup != previousSelectedConfigGroup)
	{
		if(previousSelectedConfigGroup == configDefaultFirmware && value < 0)
		{
			hideFirmwareMenu();
			resizeFirmwareLabel(0);
		}

		display.setControlValue(configGroupsListControl, selectedConfigGroup);
		display.refreshControl(configGroupsListControl);

		switch(selectedConfigGroup)
		{
		case configDefaultGeneral: break;
		case configDefaultAudio: break;
		case configDefaultMIDI: break;
		case configDefaultInterface: break;
		case configDefaultSD: break;
		case configDefaultFirmware: showFirmwareMenu(); break;
		}

		previousSelectedConfigGroup = selectedConfigGroup;
	}
}

void cConfigEditor::changeSelectionInGroup(int16_t value)
{
	switch(selectedConfigGroup)
	{
	case configDefaultGeneral: break;
	case configDefaultAudio: break;
	case configDefaultMIDI: break;
	case configDefaultInterface: break;
	case configDefaultSD: break;
	case configDefaultFirmware: changeFirmwareSelection(value); break;
	}
}


//======================================================================================================================
//master
void cConfigEditor::changeVolume(int16_t value)
{
	if(mtProject.values.volume + value < MASTER_VOLUME_MIN) mtProject.values.volume = MASTER_VOLUME_MIN;
	else if(mtProject.values.volume + value > MASTER_VOLUME_MAX) mtProject.values.volume = MASTER_VOLUME_MAX;
	else mtProject.values.volume += value;


	engine.setHeadphonesVolume(mtProject.values.volume);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	showVolume();
}

void cConfigEditor::changeReverbRoomSize(int16_t value)
{
	if(mtProject.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.values.reverbRoomSize += value;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	showReverbSize();
}

void cConfigEditor::changeReverbDamping(int16_t value)
{
	if(mtProject.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.values.reverbDamping += value;

	engine.setReverbDamping(mtProject.values.reverbDamping);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	showReverbDamping();
}

void cConfigEditor::changeLimiterAttack(int16_t value)
{
	value *= LIMITER_ATTACK_MAX/100;

	if(mtProject.values.limiterAttack + value < LIMITER_ATTACK_MIN) mtProject.values.limiterAttack = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterAttack + value > LIMITER_ATTACK_MAX) mtProject.values.limiterAttack = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterAttack += value;

	engine.setLimiterAttack(mtProject.values.limiterAttack);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;

	showLimiterAttack();
}

void cConfigEditor::changeLimiterRelease(int16_t value)
{
	float fvalue = value * (LIMITER_RELEASE_MAX/100);

	if(mtProject.values.limiterRelease + fvalue < LIMITER_RELEASE_MIN) mtProject.values.limiterRelease = LIMITER_RELEASE_MIN;
	else if(mtProject.values.limiterRelease + fvalue > LIMITER_RELEASE_MAX) mtProject.values.limiterRelease = LIMITER_RELEASE_MAX;
	else mtProject.values.limiterRelease += fvalue;


	engine.setLimiterRelease(mtProject.values.limiterRelease);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	showLimiterRelease();
}

void cConfigEditor::changeLimiterTreshold(int16_t value)
{
	value *= LIMITER_TRESHOLD_MAX/100;

	if(mtProject.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.values.limiterTreshold += value;

	engine.setLimiterTreshold(mtProject.values.limiterTreshold);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	showLimiterTreshold();
}

void cConfigEditor::changeBitDepth(int16_t value)
{
	float localValf = value * (BIT_DEPTH_MAX - BIT_DEPTH_MIN)/100.0;
	int8_t localVal;

	if(localValf < 1.0f && localValf > 0.0f ) localVal = 1;
	else if(localValf > -1.0f && localValf < 0.0f ) localVal = -1;
	else localVal = round(localValf);

	if(mtProject.values.bitDepth + localVal < BIT_DEPTH_MIN) mtProject.values.bitDepth = BIT_DEPTH_MIN;
	else if(mtProject.values.bitDepth + localVal > BIT_DEPTH_MAX) mtProject.values.bitDepth = BIT_DEPTH_MAX;
	else mtProject.values.bitDepth += localVal;

	engine.setBitDepth(mtProject.values.bitDepth);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	showBitDepth();
}

void cConfigEditor::showFirmwareMenu()
{
	//firmwareSelect=0;
	//selectedPlace[0] -=1;

	FM->setButtonObj(interfaceButton0, buttonPress, selectFirmware);
	FM->setButtonObj(interfaceButton1, buttonPress, selectFirmware);
	FM->setButtonObj(interfaceButton2, buttonPress, flashFirmware);

	listAllFirmwares();

	showFirmwareUpdateLabels();
}

void cConfigEditor:: hideFirmwareMenu()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton2);
	hideFirmwareUpdateLabels();
}

void cConfigEditor::listAllFirmwares()
{
	uint8_t locationFileCount=0;
	uint8_t validFilesCount=0;
	uint8_t invalidFileCount=0;

	sdLocation.close();
	if(sdLocation.open("/firmware", O_READ))
	{
		locationFileCount = sdLocation.createFilesListShort(0,&firmwareNamesList[0][0], firmware_list_max,firmware_name_length);

		sdLocation.close();

		for(uint8_t i = 0; i < locationFileCount;)
		{
			if(checkIfFirmwareValid(&firmwareNamesList[i][0]))
			{
				validFilesCount++;
				i++;
			}
			else
			{
				invalidFileCount++;
				strcpy(&firmwareNamesList[i][0],&firmwareNamesList[invalidFileCount][0]);
				memset(&firmwareNamesList[invalidFileCount][0],0,sizeof(firmwareNamesList[invalidFileCount][0]));

				if(invalidFileCount == (firmware_list_max-1))
				{
					break;
				}
			}
		}

		locationFileCount = validFilesCount;

		for(uint8_t i = 0; i < locationFileCount; i++)
		{
			firmwareNames[i] = &firmwareNamesList[i][0];
		}

		firmwareFoundNum=locationFileCount;
	}
	else
	{
		firmwareFoundNum=0;
		firmwareSelect=0;
	}
}

uint8_t checkIfFirmwareValid(char *name)
{
	uint8_t nameLength;

	nameLength=strlen(name);

	if(nameLength>12)//8.3 convetion
	{
		return 0;
	}

	if(name[0] != 'm' || name[1] != 't')
	{
		return 0;
	}

	if(name[nameLength-1] != 'x' || name[nameLength-2] != 'e' || name[nameLength-3] != 'h')
	{
		return 0;
	}

	return 1;

}

static uint8_t selectFirmware()
{
	CE->selectedPlace[mtConfigModeDefault]=1;
	CE->activateLabelsBorder();

	return 1;
}

uint8_t flashFirmware()
{
	if(CE->firmwareFoundNum)
	{
		FsFile fwinfo;

		if(SD.exists("/firmware/_fwinfo")) // plik nie powinien istniec, bootloader sam go usunie
		{
			SD.remove("/firmware/_fwinfo");
		}

		fwinfo = SD.open("/firmware/_fwinfo", FILE_WRITE);
		fwinfo.write(&CE->firmwareNamesList[CE->firmwareSelect][0], 13);
		fwinfo.close();


		CE->showWarning();
	}


	return 1;
}

void cConfigEditor::showWarning()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton5);

	FM->setButtonObj(interfaceButton0, buttonRelease, functAction0);
	FM->setButtonObj(interfaceButton5, buttonPress, functAction5);

	CE->selectionActive=1;
	CE->showFirmwareUpdatePopout();
}

void cConfigEditor::hideWarning()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton5);

	FM->setButtonObj(interfaceButton0, buttonPress, selectFirmware);
	FM->setButtonObj(interfaceButton1, buttonPress, selectFirmware);
	FM->setButtonObj(interfaceButton2, buttonPress, flashFirmware);

	CE->selectionActive=0;
	CE->hideFirmwareUpdatePopout();
}

void cConfigEditor::changeFirmwareSelection(int16_t value)
{
	if(firmwareSelect + value < 0) firmwareSelect = 0;
	else if(firmwareSelect + value >= firmwareFoundNum) firmwareSelect = (firmwareFoundNum - 1);
	else  firmwareSelect+= value;

	display.setControlValue(firmwareListControl, firmwareSelect);
	display.refreshControl(firmwareListControl);
}



