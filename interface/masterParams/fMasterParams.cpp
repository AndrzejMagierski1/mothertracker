

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"
#include "mtSequencer.h"
#include "mtConfig.h"
#include "SI4703.h"


#include "patternEditor/patternEditor.h"

#include "masterParams/masterParams.h"



cMasterParams masterParams;
static cMasterParams* MP = &masterParams;

extern strMtProject mtProject;
extern AudioControlSGTL5000 audioShield;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();



//master
static  uint8_t functSelectVolume(uint8_t state);
static  uint8_t functSelectReverbSize(uint8_t state);
static  uint8_t functSelectReverbDamping(uint8_t state);
static  uint8_t functSelectLimiterAttack(uint8_t state);
static  uint8_t functSelectLimiterRelease(uint8_t state);
static  uint8_t functSelectLimiterTreshold(uint8_t state);
static  uint8_t functSelectBitDepth(uint8_t state);


static  uint8_t functEncoder(int16_t value);
static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchModeMaster(uint8_t state);


// MASTER EDIT FUNCTIONS
void changeVolume(int16_t value);
void changeReverbRoomSize(int16_t value);
void changeReverbDamping(int16_t value);
void changeLimiterAttack(int16_t value);
void changeLimiterRelease(int16_t value);
void changeLimiterTreshold(int16_t value);
void changeBitDepth(int16_t value);


void cMasterParams::update()
{

}

void cMasterParams::start(uint32_t options)
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
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);





	selectedPlace = 0;

	showMasterScreen();
	setMasterScreenFunct();

	activateLabelsBorder();
}


void cMasterParams::stop()
{
	exitOnButtonRelease = 0;
	moduleRefresh = 0;
}

void cMasterParams::turnOffPerformanceMode()
{
	if(sequencer.isPerformanceMode())
	{
		fileManager.loadPattern(mtProject.values.actualPattern);
		sequencer.switchRamPatternsNow();
		sequencer.exitPerformanceMode();
	}

	engine.performanceModeEndAll();

}

void cMasterParams::turnOffRadio()
{
	audioShield.headphoneSourceSelect(0);
	radio.clearRDS();
	radio.resetSeekCallback();
	engine.setHeadphonesVolume(mtProject.values.volume);
}

void cMasterParams::setMasterScreenFunct()
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
//==============================================================================================================



static  uint8_t functEncoder(int16_t value)
{
	if(MP->frameData.multiSelActiveNum != 0)
	{
		MP->stepThroughNodes(value);
	}
	else
	{
		switch(MP->selectedPlace)
		{
		case 0: changeVolume(value);			break;
		case 1: changeReverbRoomSize(value);	break;
		case 2: changeReverbDamping(value);		break;
		case 3: changeBitDepth(value);			break;
		case 4: changeLimiterAttack(value);		break;
		case 5: changeLimiterRelease(value);	break;
		case 6: changeLimiterTreshold(value);	break;
		case 7: break;
		}
	}
	return 1;
}






static  uint8_t functLeft()
{
	if(MP->frameData.multiSelActiveNum != 0) return 1;

	if(MP->selectedPlace > 0) MP->selectedPlace--;

	MP->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(MP->frameData.multiSelActiveNum != 0) return 1;

	if(MP->selectedPlace < MP->frameData.placesCount-1) MP->selectedPlace++;

	MP->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(MP->frameData.multiSelActiveNum != 0)
	{
		MP->stepThroughNodes(1);
	}
	else
	{
		switch(MP->selectedPlace)
		{
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
	if(MP->frameData.multiSelActiveNum != 0)
	{
		MP->stepThroughNodes(-1);
	}
	else
	{
		switch(MP->selectedPlace)
		{
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

	return 1;
}

static  uint8_t functSelectVolume(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 0;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeVolume, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbSize(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 1;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeReverbRoomSize, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum  -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbDamping(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 2;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeReverbDamping, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum  -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectBitDepth(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 3;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeBitDepth, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum  -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterAttack(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 4;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeLimiterAttack, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum  -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterRelease(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 5;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeLimiterRelease, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum  -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterTreshold(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 6;

	if(state == buttonPress)
	{
		MP->selectedPlace = node;
		MP->addNode(changeLimiterTreshold, node);
		MP->frameData.multisel[node].frameNum = node;
		MP->frameData.multisel[node].isActive = 1;
		MP->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		MP->removeNode(node);

		if(MP->frameData.multiSelActiveNum)
		{
			if(MP->frameData.multisel[node].isActive)
			{
				MP->removeNode(node);
				MP->frameData.multiSelActiveNum  -= 1;

				MP->frameData.multisel[node].isActive = 0;

				if(MP->frameData.multiSelActiveNum == 0)
				{
					MP->selectedPlace = node;
				}
			}
		}
	}

	MP->activateLabelsBorder();

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(button != interfaceButtonPerformance) MP->turnOffPerformanceMode();
	if(button != interfaceButtonSampleRec) MP->turnOffRadio();

	MP->eventFunct(eventSwitchModule,MP,&button,0);

	return 1;
}

static  uint8_t functSwitchModeMaster(uint8_t state)
{
	if(state == buttonPress)
	{
		MP->selectedPlace = 0;

/*		CE->clearAllNodes();
		CE->cancelMultiFrame();*/

	}
	else if(state == buttonHold)
	{
//		CE->resizeLabelConfigMaster();

		MP->exitOnButtonRelease = 1;
		//CE->selectedPlace[mtConfigModeMaster] = 0;
		MP->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(MP->exitOnButtonRelease) MP->eventFunct(eventSwitchToPreviousModule,MP,0,0);
	}

	return 1;
}




//======================================================================================================================
//master
void changeVolume(int16_t value)
{
	if(mtProject.values.volume + value < MASTER_VOLUME_MIN) mtProject.values.volume = MASTER_VOLUME_MIN;
	else if(mtProject.values.volume + value > MASTER_VOLUME_MAX) mtProject.values.volume = MASTER_VOLUME_MAX;
	else mtProject.values.volume += value;


	engine.setHeadphonesVolume(mtProject.values.volume);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	MP->showVolume();
}

void changeReverbRoomSize(int16_t value)
{
	if(mtProject.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.values.reverbRoomSize += value;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	MP->showReverbSize();
}

void changeReverbDamping(int16_t value)
{
	if(mtProject.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.values.reverbDamping += value;

	engine.setReverbDamping(mtProject.values.reverbDamping);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	MP->showReverbDamping();
}

void changeLimiterAttack(int16_t value)
{
	value *= LIMITER_ATTACK_MAX/100;

	if(mtProject.values.limiterAttack + value < LIMITER_ATTACK_MIN) mtProject.values.limiterAttack = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterAttack + value > LIMITER_ATTACK_MAX) mtProject.values.limiterAttack = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterAttack += value;

	engine.setLimiterAttack(mtProject.values.limiterAttack);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;

	MP->showLimiterAttack();
}

void changeLimiterRelease(int16_t value)
{
	float fvalue = value * (LIMITER_RELEASE_MAX/100);

	if(mtProject.values.limiterRelease + fvalue < LIMITER_RELEASE_MIN) mtProject.values.limiterRelease = LIMITER_RELEASE_MIN;
	else if(mtProject.values.limiterRelease + fvalue > LIMITER_RELEASE_MAX) mtProject.values.limiterRelease = LIMITER_RELEASE_MAX;
	else mtProject.values.limiterRelease += fvalue;


	engine.setLimiterRelease(mtProject.values.limiterRelease);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	MP->showLimiterRelease();
}

void changeLimiterTreshold(int16_t value)
{
	value *= LIMITER_TRESHOLD_MAX/100;

	if(mtProject.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.values.limiterTreshold += value;

	engine.setLimiterTreshold(mtProject.values.limiterTreshold);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	MP->showLimiterTreshold();
}

void changeBitDepth(int16_t value)
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
	MP->showBitDepth();
}


// MULTISEL
void cMasterParams::addNode(editFunct_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cMasterParams::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cMasterParams::stepThroughNodes(int16_t value)
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

void cMasterParams::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}

void cMasterParams::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		frameData.multisel[i].isActive = 0;
	}

	frameData.multiSelActiveNum = 0;
}


