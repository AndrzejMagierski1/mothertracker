



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

//master tracks



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functSwitchMode(uint8_t button);




void cConfigEditor::update()
{



}

void cConfigEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mode = options;

//--------------------------------------------------------------------



//--------------------------------------------------------------------


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchMode);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchMode);

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
	case 0: 	 break;
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

static  uint8_t functSwitchMode(uint8_t button)
{
	switch(button)
	{
	case interfaceButtonConfig:
	{
		if(CE->mode != mtConfigModeDefault)
		{
			CE->mode = 0;
			CE->showDefaultConfigScreen();
			CE->setConfigScreenFunct();
		}
		break;
	}
	case interfaceButtonMaster:
	{
		if(CE->mode != mtConfigModeMaster)
		{
			CE->mode = 1;
			CE->showMasterScreen();
			CE->setMasterScreenFunct();
		}
		break;
	}



	}

	CE->activateLabelsBorder();

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

}


//======================================================================================================================
//master
void cConfigEditor::changeVolume(int16_t value)
{
/*	if(mtProject.values.volume + value < MASTER_VOLUME_MIN) mtProject.values.volume = MASTER_VOLUME_MIN;
	else if(mtProject.values.volume + value > MASTER_VOLUME_MAX) mtProject.values.volume = MASTER_VOLUME_MAX;
	else mtProject.values.volume += value;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
*/
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


