



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
static  uint8_t functSelectVolume(uint8_t state);
static  uint8_t functSelectReverbSize(uint8_t state);
static  uint8_t functSelectReverbDamping(uint8_t state);
static  uint8_t functSelectLimiterAttack(uint8_t state);
static  uint8_t functSelectLimiterRelease(uint8_t state);
static  uint8_t functSelectLimiterTreshold(uint8_t state);
static  uint8_t functSelectBitDepth(uint8_t state);



//master tracks



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);


static  uint8_t functSwitchModeConfig(uint8_t state);
static  uint8_t functSwitchModeMaster(uint8_t state);

uint8_t checkIfFirmwareValid(char *name);
static uint8_t selectFirmware();
uint8_t flashFirmware();


// MASTER EDIT FUNCTIONS
void changeVolume(int16_t value);
void changeReverbRoomSize(int16_t value);
void changeReverbDamping(int16_t value);
void changeLimiterAttack(int16_t value);
void changeLimiterRelease(int16_t value);
void changeLimiterTreshold(int16_t value);
void changeBitDepth(int16_t value);



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

	clearAllNodes();
	cancelMultiFrame();


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





	FM->setButtonObj(interfaceButton0, functSelectVolume);

	FM->setButtonObj(interfaceButton1, functSelectReverbSize);
	FM->setButtonObj(interfaceButton2, functSelectReverbDamping);

	FM->setButtonObj(interfaceButton3, functSelectBitDepth);
	FM->setButtonObj(interfaceButton4, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton5, functSelectLimiterRelease);
	FM->setButtonObj(interfaceButton6, functSelectLimiterTreshold);




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

	if(CE->frameData.multiSelActiveNum != 0)
	{
		CE->stepThroughNodes(value);
	}
	else
	{
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

		case 10: changeVolume(value);			break;
		case 11: changeReverbRoomSize(value);	break;
		case 12: changeReverbDamping(value);	break;
		case 13: changeBitDepth(value);			break;
		case 14: changeLimiterAttack(value);	break;
		case 15: changeLimiterRelease(value);	break;
		case 16: changeLimiterTreshold(value);	break;
		case 17: 	break;

		}
	}

	return 1;
}






static  uint8_t functLeft()
{
	if(CE->selectionActive) return 1;
	if(CE->frameData.multiSelActiveNum != 0) return 1;

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
	if(CE->frameData.multiSelActiveNum != 0) return 1;

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

	if(CE->frameData.multiSelActiveNum != 0)
	{
		CE->stepThroughNodes(1);
	}
	else
	{
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

		case 10: changeVolume(1);			break;
		case 11: changeReverbRoomSize(1);	break;
		case 12: changeReverbDamping(1);	break;
		case 13: changeBitDepth(1);			break;
		case 14: changeLimiterAttack(1);	break;
		case 15: changeLimiterRelease(1);	break;
		case 16: changeLimiterTreshold(1);	break;
		case 17: 	break;

		}
	}

	return 1;
}


static  uint8_t functDown()
{
	if(CE->selectionActive) return 1;

	if(CE->frameData.multiSelActiveNum != 0)
	{
		CE->stepThroughNodes(-1);
	}
	else
	{
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

		case 10: changeVolume(-1);			break;
		case 11: changeReverbRoomSize(-1);	break;
		case 12: changeReverbDamping(-1);	break;
		case 13: changeBitDepth(-1);		break;
		case 14: changeLimiterAttack(-1);	break;
		case 15: changeLimiterRelease(-1);	break;
		case 16: changeLimiterTreshold(-1);	break;
		case 17: 	break;

		}
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

static  uint8_t functSelectVolume(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 0;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeVolume, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbSize(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 1;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeReverbRoomSize, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbDamping(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 2;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeReverbDamping, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectBitDepth(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 3;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeBitDepth, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterAttack(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 4;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeLimiterAttack, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterRelease(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 5;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeLimiterRelease, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterTreshold(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 6;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeLimiterTreshold, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

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
			CE->clearAllNodes();
			CE->cancelMultiFrame();

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
		CE->selectedPlace[mtConfigModeMaster] = 0;

		CE->clearAllNodes();
		CE->cancelMultiFrame();

		if(CE->mode != mtConfigModeMaster)
		{
			CE->clearAllNodes();
			CE->cancelMultiFrame();

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
		//CE->selectedPlace[mtConfigModeMaster] = 0;
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
void changeVolume(int16_t value)
{
	if(mtProject.mtProjectRemote.values.volume + value < MASTER_VOLUME_MIN) mtProject.mtProjectRemote.values.volume = MASTER_VOLUME_MIN;
	else if(mtProject.mtProjectRemote.values.volume + value > MASTER_VOLUME_MAX) mtProject.mtProjectRemote.values.volume = MASTER_VOLUME_MAX;
	else mtProject.mtProjectRemote.values.volume += value;


	engine.setHeadphonesVolume(mtProject.mtProjectRemote.values.volume);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showVolume();
}

void changeReverbRoomSize(int16_t value)
{
	if(mtProject.mtProjectRemote.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.mtProjectRemote.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.mtProjectRemote.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.mtProjectRemote.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.mtProjectRemote.values.reverbRoomSize += value;

	engine.setReverbRoomsize(mtProject.mtProjectRemote.values.reverbRoomSize);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showReverbSize();
}

void changeReverbDamping(int16_t value)
{
	if(mtProject.mtProjectRemote.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.mtProjectRemote.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.mtProjectRemote.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.mtProjectRemote.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.mtProjectRemote.values.reverbDamping += value;

	engine.setReverbDamping(mtProject.mtProjectRemote.values.reverbDamping);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showReverbDamping();
}

void changeLimiterAttack(int16_t value)
{
	value *= LIMITER_ATTACK_MAX/100;

	if(mtProject.mtProjectRemote.values.limiterAttack + value < LIMITER_ATTACK_MIN) mtProject.mtProjectRemote.values.limiterAttack = LIMITER_ATTACK_MIN;
	else if(mtProject.mtProjectRemote.values.limiterAttack + value > LIMITER_ATTACK_MAX) mtProject.mtProjectRemote.values.limiterAttack = LIMITER_ATTACK_MAX;
	else mtProject.mtProjectRemote.values.limiterAttack += value;

	engine.setLimiterAttack(mtProject.mtProjectRemote.values.limiterAttack);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;

	CE->showLimiterAttack();
}

void changeLimiterRelease(int16_t value)
{
	float fvalue = value * (LIMITER_RELEASE_MAX/100);

	if(mtProject.mtProjectRemote.values.limiterRelease + fvalue < LIMITER_RELEASE_MIN) mtProject.mtProjectRemote.values.limiterRelease = LIMITER_RELEASE_MIN;
	else if(mtProject.mtProjectRemote.values.limiterRelease + fvalue > LIMITER_RELEASE_MAX) mtProject.mtProjectRemote.values.limiterRelease = LIMITER_RELEASE_MAX;
	else mtProject.mtProjectRemote.values.limiterRelease += fvalue;


	engine.setLimiterRelease(mtProject.mtProjectRemote.values.limiterRelease);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showLimiterRelease();
}

void changeLimiterTreshold(int16_t value)
{
	value *= LIMITER_TRESHOLD_MAX/100;

	if(mtProject.mtProjectRemote.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.mtProjectRemote.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.mtProjectRemote.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.mtProjectRemote.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.mtProjectRemote.values.limiterTreshold += value;

	engine.setLimiterTreshold(mtProject.mtProjectRemote.values.limiterTreshold);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showLimiterTreshold();
}

void changeBitDepth(int16_t value)
{
	float localValf = value * (BIT_DEPTH_MAX - BIT_DEPTH_MIN)/100.0;
	int8_t localVal;

	if(localValf < 1.0f && localValf > 0.0f ) localVal = 1;
	else if(localValf > -1.0f && localValf < 0.0f ) localVal = -1;
	else localVal = round(localValf);

	if(mtProject.mtProjectRemote.values.bitDepth + localVal < BIT_DEPTH_MIN) mtProject.mtProjectRemote.values.bitDepth = BIT_DEPTH_MIN;
	else if(mtProject.mtProjectRemote.values.bitDepth + localVal > BIT_DEPTH_MAX) mtProject.mtProjectRemote.values.bitDepth = BIT_DEPTH_MAX;
	else mtProject.mtProjectRemote.values.bitDepth += localVal;

	engine.setBitDepth(mtProject.mtProjectRemote.values.bitDepth);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showBitDepth();
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

		if(locationFileCount > firmware_list_max)
		{
			locationFileCount = firmware_list_max;
		}

		for(uint8_t i = 0; i < locationFileCount; i++)
		{
			if(checkIfFirmwareValid(&firmwareNamesList[i][0]))
			{
				strcpy(&firmwareNamesList[validFilesCount][0], &firmwareNamesList[i][0]);
				validFilesCount++;
			}
		}

		for(uint8_t i = 0; i < validFilesCount; i++)
		{
			firmwareNames[i] = &firmwareNamesList[i][0];
		}

		firmwareFoundNum = validFilesCount;
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


// MULTISEL
void cConfigEditor::addNode(editFunct_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cConfigEditor::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cConfigEditor::stepThroughNodes(int16_t value)
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		if(selectNodes[node].isActive)
		{
			if(selectNodes[node].editFunct != NULL)
			{
				selectNodes[node].editFunct(value);
			}
		}
	}
}

void cConfigEditor::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}

void cConfigEditor::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		CE->frameData.multisel[i].isActive = 0;
	}

	CE->frameData.multiSelActiveNum = 0;
}



