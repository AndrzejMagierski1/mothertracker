



#include "mtPadBoard.h"
#include "mtAudioEngine.h"


#include "configEditor.h"



cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


// config
static  uint8_t functConfigGroup(uint8_t button);




//master
static  uint8_t functSelectVolume();
static  uint8_t functSelectReverbSize();
static  uint8_t functSelectReverbDamping();
static  uint8_t functSelectLimiterAttack();
static  uint8_t functSelectLimiterRelease();
static  uint8_t functSelectLimiterTreshold();

static  uint8_t functMasterHold();

//master tracks



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);


static  uint8_t functSwitchModeConfig(uint8_t state);
static  uint8_t functSwitchModeMaster(uint8_t state);

uint8_t checkIfFirmwareValid(char *name);
static uint8_t selectFirmware();
uint8_t prepareDataForBootloader();




void cConfigEditor::update()
{



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
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchModule);
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
		showDefaultConfigScreen();
		setConfigScreenFunct();
		break;
	}
	case mtConfigModeMaster:
	{
		showMasterScreen();
		setMasterScreenFunct();
		break;
	}
	case mtConfigModeMasterTracks:
	{


		showMasterTracksScreen();
		setMasterTracksScreenFunct();
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

	FM->setButtonObj(interfaceButton3, buttonPress, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton4, buttonPress, functSelectLimiterRelease);
	FM->setButtonObj(interfaceButton5, buttonPress, functSelectLimiterTreshold);




	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

void cConfigEditor::setMasterTracksScreenFunct()
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


}
//==============================================================================================================






//==============================================================================================================





static  uint8_t functConfigGroup(uint8_t button)
{
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
	uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

	switch(mode_places)
	{
	case 0: CE->changeSelectionInGroup(value);		 break;
	case 1:		 break;
	case 2: 	 break;
	case 3: 	 break;
	case 4: 	 break;
	case 5: 	 break;
	case 6: CE->changeConfigGroupSelection(value);	 break;
	case 7: 	 break;

	case 10: CE->changeVolume(value);			break;
	case 11: CE->changeReverbRoomSize(value);	break;
	case 12: CE->changeReverbDamping(value);	break;
	case 13: CE->changeLimiterAttack(value);	break;
	case 14: CE->changeLimiterRelease(value);	break;
	case 15: CE->changeLimiterTreshold(value);	break;
	case 16: 	break;
	case 17: 	break;

	}






	return 1;
}






static  uint8_t functLeft()
{
	if(CE->selectedPlace[CE->mode] > 0) CE->selectedPlace[CE->mode]--;

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


static  uint8_t functRight()
{
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
	switch(CE->selectedPlace[CE->mode])
	{
	case 0: 	break;
	case 1: 	break;
	case 2: 	break;
	case 3: 	break;
	case 5: 	break;
	case 6: CE->changeConfigGroupSelection(-1);	break;
	}

	return 1;
}


static  uint8_t functDown()
{
	switch(CE->selectedPlace[CE->mode])
	{
	case 0: 	break;
	case 1: 	break;
	case 2: 	break;
	case 3: 	break;
	case 5: 	break;
	case 6: CE->changeConfigGroupSelection(1);	break;
	}

	return 1;
}




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0)
	{
		sequencer.play();
	}
	else if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	return 1;
}



static  uint8_t functRecAction()
{


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
	CE->selectedPlace[mtConfigModeMaster] = 3;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterRelease()
{
	CE->selectedPlace[mtConfigModeMaster] = 4;
	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterTreshold()
{
	CE->selectedPlace[mtConfigModeMaster] = 5;
	CE->activateLabelsBorder();

	return 1;
}


static uint8_t functSwitchModule(uint8_t button)
{
	CE->eventFunct(eventSwitchModule,CE,&button,0);

	return 1;
}

static  uint8_t functSwitchModeConfig(uint8_t state)
{
	if(state == buttonPress)
	{

		if(CE->mode != mtConfigModeDefault)
		{
			CE->mode = 0;
			CE->showDefaultConfigScreen();
			CE->setConfigScreenFunct();
		}

		CE->activateLabelsBorder();
	}

	return 1;
}

static  uint8_t functSwitchModeMaster(uint8_t state)
{
	if(state == buttonPress)
	{

		if(CE->mode != mtConfigModeMaster)
		{
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

//======================================================================================================================


void cConfigEditor::changeConfigGroupSelection(int16_t value)
{
	if(selectedConfigGroup + value < 0) selectedConfigGroup = 0;
	else if(selectedConfigGroup + value > groupCount-1) selectedConfigGroup = groupCount-1;
	else  selectedConfigGroup += value;


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

	if(selectedConfigGroup != configDefaultFirmware)
	{
		hideFirmwareMenu();
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
/*
	if(mtProject.values.volume + value < MASTER_VOLUME_MIN) mtProject.values.volume = MASTER_VOLUME_MIN;
	else if(mtProject.values.volume + value > MASTER_VOLUME_MAX) mtProject.values.volume = MASTER_VOLUME_MAX;
	else mtProject.values.volume += value;
*/
	float fValue = value*0.01;

	if(mtConfig.audioCodecConfig.headphoneVolume + fValue < MASTER_VOLUME_MIN) mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_MIN;
	else if(mtConfig.audioCodecConfig.headphoneVolume + fValue > MASTER_VOLUME_MAX) mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_MAX;
	else mtConfig.audioCodecConfig.headphoneVolume += fValue;

	mtConfig.audioCodecConfig.changeFlag = 1;

	mtConfig.audioCodecConfig.outSelect = outputSelectHeadphones;

	showVolume();
}

void cConfigEditor::changeReverbRoomSize(int16_t value)
{
	if(mtProject.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.values.reverbRoomSize += value;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);

	showReverbSize();
}

void cConfigEditor::changeReverbDamping(int16_t value)
{
	if(mtProject.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.values.reverbDamping += value;

	engine.setReverbDamping(mtProject.values.reverbDamping);

	showReverbDamping();
}

void cConfigEditor::changeLimiterAttack(int16_t value)
{
	if(mtProject.values.limiterAttack + value < LIMITER_ATTACK_MIN) mtProject.values.limiterAttack = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterAttack + value > LIMITER_ATTACK_MAX) mtProject.values.limiterAttack = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterAttack += value;

	engine.setLimiterAttack(mtProject.values.limiterAttack);

	showLimiterAttack();
}

void cConfigEditor::changeLimiterRelease(int16_t value)
{
/*	if(mtProject.values.limiterRelease + value < LIMITER_ATTACK_MIN) mtProject.values.limiterRelease = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterRelease + value > LIMITER_ATTACK_MAX) mtProject.values.limiterRelease = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterRelease += value;

	engine.setLimiterRelease(mtProject.values.limiterRelease);
*/
	showLimiterRelease();
}

void cConfigEditor::changeLimiterTreshold(int16_t value)
{
	value = value*100;

	if(mtProject.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.values.limiterTreshold += value;

	engine.setLimiterTreshold(mtProject.values.limiterTreshold);

	showLimiterTreshold();
}

void cConfigEditor::showFirmwareMenu()
{
	FM->setButtonObj(interfaceButton0, buttonPress, selectFirmware);
	FM->setButtonObj(interfaceButton1, buttonPress, selectFirmware);
	FM->setButtonObj(interfaceButton2, buttonPress, prepareDataForBootloader);

	listAllFirmwares();
	createFirmwareList();

	showFirmwareList();
}

void cConfigEditor::listAllFirmwares()
{
	uint8_t locationFileCount=0;
	uint8_t validFilesCount=0;
	uint8_t invalidFileCount=0;

	sdLocation.close();
	if(sdLocation.open("/firmware", O_READ))
	{
		locationFileCount = sdLocation.createFilesList(0,firmwareNamesList, firmware_list_max,0);
	}

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

uint8_t checkIfFirmwareValid(char *name)
{
	uint8_t nameLength;

	nameLength=strlen(name);

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

void cConfigEditor:: hideFirmwareMenu()
{
/*
	FM->setButtonObj(interfaceButton0, buttonPress, nullptr);
	FM->setButtonObj(interfaceButton1, buttonPress, nullptr);
*/

	hideFirmwareList();

}

static uint8_t selectFirmware()
{
	CE->selectedPlace[mtConfigModeDefault]=0;
	CE->activateLabelsBorder();

	return 1;
}

uint8_t prepareDataForBootloader()
{
	FsFile fwinfo;

	if(SD.exists("/firmware/_fwinfo")) // plik nie powinien istniec, bootloader sam go usunie
	{
		SD.remove("/firmware/_fwinfo");
	}

	fwinfo = SD.open("/firmware/_fwinfo", FILE_WRITE);
	fwinfo.write(&CE->firmwareNamesList[CE->firmwareSelect][0], 13);
	fwinfo.close();

	//TODO: popup potwierdzenia czy na pewno zflashowac

	pinMode(BOOTLOADER_PIN,OUTPUT);
	digitalWrite(BOOTLOADER_PIN,LOW);

	while(1);// program sie zatrzymuje do czasu przejecia kontroli przez bootloader

	return 1;
}

void cConfigEditor::changeFirmwareSelection(int16_t value)
{
	if(firmwareSelect + value < 0) firmwareSelect = 0;
	else if(firmwareSelect + value > firmwareFoundNum) firmwareSelect = firmwareFoundNum-1;
	else  firmwareSelect+= value;

	display.setControlValue(firmwareListControl, firmwareSelect);
	display.refreshControl(firmwareListControl);
}



