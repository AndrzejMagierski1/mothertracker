

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
//#include "mtFileManager.h"
#include "mtSequencer.h"
#include "mtConfig.h"
#include "Si4703.h"
#include "keyScanner.h"
#include "mtPadsBacklight.h"

#include "patternEditor/patternEditor.h"


#include "performanceMode/performanceEngine.h"

#include "masterParams/masterParams.h"
#include "mtGainLevelLogarithmicTab.h"


#include "fileManager.h"

cMasterParams masterParams;
static cMasterParams* MP = &masterParams;

extern strMtProject mtProject;
extern AudioControlSGTL5000 audioShield;
extern AudioMixer10	mixerReverb;

static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();
static 	uint8_t functDelete();



//master
static  uint8_t functSelectVolume(uint8_t state);
static  uint8_t functSelectLimiterAttack(uint8_t state);
static  uint8_t functSelectLimiterRelease(uint8_t state);
static  uint8_t functSelectLimiterTreshold(uint8_t state);
static  uint8_t functSelectBitDepth(uint8_t state);

//delay
static  uint8_t functSwitchToDelayWindow();
static 	uint8_t functSelectDelaySyncEnable();
static 	uint8_t functSelectDelayPingpongEnable();
static 	uint8_t functSelectDelaySyncRate();
static 	uint8_t functSelectDelayTime();
static 	uint8_t functSelectDelayFeedback();
static 	uint8_t functDelayCancel();
//reverb
static 	uint8_t functSwitchToReverbWindow();
static 	uint8_t functSelectReverbSize();
static 	uint8_t functSelectReverbDamp();
static	uint8_t functSelectReverbPredelay();
static	uint8_t functSelectReverbDiffusion();
static  uint8_t functReverbCancel();

static  uint8_t functEncoder(int16_t value);
static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchModeMaster(uint8_t state);

//mixer
static uint8_t functSoloMute(uint8_t state);
static uint8_t functSoloMuteTrack(uint8_t n,uint8_t state);

// MASTER EDIT FUNCTIONS
void changeVolume(int16_t value);
void changeLimiterAttack(int16_t value);
void changeLimiterRelease(int16_t value);
void changeLimiterTreshold(int16_t value);
void changeBitDepth(int16_t value);

void setDefaultMasterVolume();
void setDefaultLimiterAttack();
void setDefaultLimiterRelease();
void setDefaultLimiterTreshold();
void setDefaultBithDepth();

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

void cMasterParams::update()
{
	if(displayType == display_t::mixer)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			calcTrackLevel(i);
			if(trackLevel[i].value != trackLevel[i].lastValue)
			{
				showLevelBar(i);
			}
			trackLevel[i].lastValue = trackLevel[i].value;
		}
	}

}

// parametr options - przycisk modulu z ktorego nastapilo wejscie do mastera
void cMasterParams::start(uint32_t options)
{
	moduleRefresh = 1;

	displayType = display_t::masterValues;

	sourceModuleButton = options;
	//mode = options;
	exitOnButtonRelease = 0;
//--------------------------------------------------------------------


//--------------------------------------------------------------------
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
	FM->setPadsGlobal(functPads);



	isDelayScreen = false;
	isReverbScreen = false;
	selectedPlace = 0;

	showMasterScreen();

	setMasterScreenFunct();


	activateLabelsBorder();
}


void cMasterParams::stop()
{
	sequencer.stopManualNotes();
	exitOnButtonRelease = 0;
	moduleRefresh = 0;
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
	FM->setButtonObj(interfaceButtonDelete, buttonPress, functDelete);

	FM->setButtonObj(interfaceButton0, functSelectVolume);
//	FM->setButtonObj(interfaceButton1, functSelectReverbSize);
//	FM->setButtonObj(interfaceButton2, functSelectReverbDamping);
	FM->setButtonObj(interfaceButton1, buttonPress, functSwitchToReverbWindow);
	FM->setButtonObj(interfaceButton2, buttonPress, functSwitchToDelayWindow);
	FM->setButtonObj(interfaceButton3, functSelectBitDepth);
	FM->setButtonObj(interfaceButton4, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton5, functSelectLimiterRelease);
	FM->setButtonObj(interfaceButton6, functSelectLimiterTreshold);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);

}

void cMasterParams::setDelayScreenFunct()
{

}

//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################
//==============================================================================================================



static  uint8_t functEncoder(int16_t value)
{
	if(MP->displayType == cMasterParams::display_t::mixer) return 1;

	if(MP->isDelayScreen)
	{
		switch(MP->selectedPlaceDelay)
		{
			case 0: 	MP->changeDelayPingPongEnable(value);		break;
			case 1: 	MP->changeDelaySyncEnable(value);			break;
			case 2: 	MP->changeDelayRate(value);					break;
			case 3: 	MP->changeDelayTime(value);					break;
			case 4: 	MP->changeDelayFeedback(value);				break;
			default: break;
		}
	}
	else if(MP->isReverbScreen)
	{
		switch(MP->selectedPlaceReverb)
		{
			case 0: 	MP->changeReverbSize(value);				break;
			case 1: 	MP->changeReverbDamp(value);				break;
			case 2: 	MP->changeReverbPredelay(value);			break;
			case 3: 	MP->changeReverbDiffusion(value);			break;
			default: break;
		}
	}
	else
	{
		switch(MP->selectedPlace)
		{
			case 0: changeVolume(value);			break;
			case 3: changeBitDepth(value);			break;
			case 4: changeLimiterAttack(value);		break;
			case 5: changeLimiterRelease(value);	break;
			case 6: changeLimiterTreshold(value);	break;
			default: break;
		}
	}



	return 1;
}






static  uint8_t functLeft()
{
	if(MP->displayType == cMasterParams::display_t::mixer) return 1;

	if(MP->isDelayScreen)
	{
		if(MP->selectedPlaceDelay > 0) MP->selectedPlaceDelay--;

		if( (MP->selectedPlaceDelay == 2) && !(mtProject.values.delayParams & 0b01000000)) MP->selectedPlaceDelay = 1;
		else if ( (MP->selectedPlaceDelay == 3) && (mtProject.values.delayParams & 0b01000000)) MP->selectedPlaceDelay = 2;

		MP->refreshDelayFrame();
	}
	else if(MP->isReverbScreen)
	{
		if(MP->selectedPlaceReverb > 0) MP->selectedPlaceReverb--;
		MP->refreshReverbFrame();
	}
	else
	{
		if(MP->selectedPlace > 0)
		{
			if(MP->selectedPlace == 3) MP->selectedPlace = 0;
			else MP->selectedPlace--;
		}
		MP->activateLabelsBorder();
	}



	return 1;
}


static  uint8_t functRight()
{
	if(MP->displayType == cMasterParams::display_t::mixer) return 1;

	if(MP->isDelayScreen)
	{
		if(MP->selectedPlaceDelay < MP->frameData.placesCount-1) MP->selectedPlaceDelay++;

		if( (MP->selectedPlaceDelay == 2) && !(mtProject.values.delayParams & 0b01000000)) MP->selectedPlaceDelay = 3;
		else if ( (MP->selectedPlaceDelay == 3) && (mtProject.values.delayParams & 0b01000000)) MP->selectedPlaceDelay = 4;

		MP->refreshDelayFrame();
	}
	else if(MP->isReverbScreen)
	{
		if(MP->selectedPlaceReverb < MP->frameData.placesCount-1) MP->selectedPlaceReverb++;
		MP->refreshReverbFrame();
	}
	else
	{
		if(MP->selectedPlace < MP->frameData.placesCount-1)
		{
			if(MP->selectedPlace == 0 ) MP->selectedPlace = 3;
			else MP->selectedPlace++;
		}
		MP->activateLabelsBorder();
	}

	return 1;
}


static  uint8_t functUp()
{
	if(MP->displayType == cMasterParams::display_t::mixer) return 1;

	if(MP->isDelayScreen)
	{
		switch(MP->selectedPlaceDelay)
		{
			case 0: 	MP->changeDelayPingPongEnable(-1);		break;
			case 1: 	MP->changeDelaySyncEnable(-1);			break;
			case 2: 	MP->changeDelayRate(-1);				break;
			case 3: 	MP->changeDelayTime(1);					break;
			case 4: 	MP->changeDelayFeedback(1);				break;
			default: break;
		}
	}
	else if(MP->isReverbScreen)
	{
		switch(MP->selectedPlaceReverb)
		{
			case 0: 	MP->changeReverbSize(1);				break;
			case 1: 	MP->changeReverbDamp(1);				break;
			case 2: 	MP->changeReverbPredelay(1);			break;
			case 3: 	MP->changeReverbDiffusion(1);			break;
			default: break;
		}
	}
	else
	{
		switch(MP->selectedPlace)
		{
			case 0: changeVolume(1);			break;
			case 3: changeBitDepth(1);			break;
			case 4: changeLimiterAttack(1);		break;
			case 5: changeLimiterRelease(1);	break;
			case 6: changeLimiterTreshold(1);	break;
			default: break;
		}
	}



	return 1;
}


static  uint8_t functDown()
{
	if(MP->displayType == cMasterParams::display_t::mixer) return 1;

	if(MP->isDelayScreen)
	{
		switch(MP->selectedPlaceDelay)
		{
			case 0: 	MP->changeDelayPingPongEnable(1);		break;
			case 1: 	MP->changeDelaySyncEnable(1);			break;
			case 2: 	MP->changeDelayRate(1);					break;
			case 3: 	MP->changeDelayTime(-1);				break;
			case 4: 	MP->changeDelayFeedback(-1);			break;
			default: break;
		}
	}
	else if(MP->isReverbScreen)
	{
		switch(MP->selectedPlaceReverb)
		{
			case 0: 	MP->changeReverbSize(-1);				break;
			case 1: 	MP->changeReverbDamp(-1);				break;
			case 2: 	MP->changeReverbPredelay(-1);			break;
			case 3: 	MP->changeReverbDiffusion(-1);			break;
			default: break;
		}
	}
	else
	{
		switch(MP->selectedPlace)
		{
			case 0: changeVolume(-1);			break;
			case 3: changeBitDepth(-1);			break;
			case 4: changeLimiterAttack(-1);	break;
			case 5: changeLimiterRelease(-1);	break;
			case 6: changeLimiterTreshold(-1);	break;
			default: break;
		}
	}

	return 1;
}


static 	uint8_t functDelete()
{
	if(MP->displayType == cMasterParams::display_t::mixer) return 1;

	if(MP->isDelayScreen)
	{
		switch(MP->selectedPlaceDelay)
		{
		case 0: 	MP->setDefaultDelayPingPongEnable();		break;
		case 1: 	MP->setDefaultDelaySyncEnable();			break;
		case 2: 	MP->setDefaultDelayRate();					break;
		case 3: 	MP->setDefaultDelayTime();					break;
		case 4: 	MP->setDefaultDelayFeedback();				break;
		default: break;
		}
	}
	else if(MP->isReverbScreen)
	{
		switch(MP->selectedPlaceReverb)
		{
			case 0: 	MP->setDefaultReverbSize();				break;
			case 1: 	MP->setDefaultReverbDamp();				break;
			case 2: 	MP->setDefaultReverbPredelay();			break;
			case 3: 	MP->setDefaultReverbDiffusion();		break;
			default: break;
		}
	}
	else
	{
		switch(MP->selectedPlace)
		{
			case 0: setDefaultMasterVolume();		break;
			case 3: setDefaultBithDepth();			break;
			case 4: setDefaultLimiterAttack();		break;
			case 5: setDefaultLimiterRelease();		break;
			case 6: setDefaultLimiterTreshold();	break;
			default: break;
		}
	}
	return 1;
}

static  uint8_t functPlayAction()
{
	if(MP->sourceModuleButton == interfaceButtonSampleRec) 	return 1;

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

	return 1;
}

static  uint8_t functSelectVolume(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 0;


	if(state == buttonPress)
	{
		MP->selectedPlace = node;
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
	}
	MP->activateLabelsBorder();

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(button != interfaceButtonPerformance) performance.turnOff();
	//if(button != interfaceButtonSampleRec) MP->turnOffRadio();

	if(MP->sourceModuleButton == interfaceButtonSampleRec)
	{
		MP->eventFunct(eventSwitchModule,MP, &MP->sourceModuleButton, &button);
	}
	else
	{
		MP->eventFunct(eventSwitchModule,MP,&button,0);
	}


	return 1;
}
void cMasterParams::switchToMaster()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearButton(interfaceButtonShift);

	FM->setButtonObj(interfaceButton0, functSelectVolume);

//	FM->setButtonObj(interfaceButton1, functSelectReverbSize);
//	FM->setButtonObj(interfaceButton2, functSelectReverbDamping);
	FM->setButtonObj(interfaceButton1, buttonPress, functSwitchToReverbWindow);
	FM->setButtonObj(interfaceButton2, buttonPress, functSwitchToDelayWindow);
	FM->setButtonObj(interfaceButton3, functSelectBitDepth);
	FM->setButtonObj(interfaceButton4, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton5, functSelectLimiterRelease);
	FM->setButtonObj(interfaceButton6, functSelectLimiterTreshold);


	showMasterScreen();
	displayType = display_t::masterValues;
	isDelayScreen = false;
	isReverbScreen = false;
}
void cMasterParams::switchToMixer()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);


	FM->setButtonObj(interfaceButton0, functSoloMuteTrack);

	FM->setButtonObj(interfaceButton1, functSoloMuteTrack);
	FM->setButtonObj(interfaceButton2, functSoloMuteTrack);

	FM->setButtonObj(interfaceButton3, functSoloMuteTrack);
	FM->setButtonObj(interfaceButton4, functSoloMuteTrack);
	FM->setButtonObj(interfaceButton5, functSoloMuteTrack);
	FM->setButtonObj(interfaceButton6, functSoloMuteTrack);
	FM->setButtonObj(interfaceButton6, functSoloMuteTrack);
	FM->setButtonObj(interfaceButton7, functSoloMuteTrack);

	FM->setButtonObj(interfaceButtonShift, functSoloMute);

	showMixerScreen();
	displayType = display_t::mixer;
}

void cMasterParams::switchToDelayScreen()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearButton(interfaceButtonShift);

	FM->setButtonObj(interfaceButton0, buttonPress, functSelectDelayPingpongEnable);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectDelaySyncEnable);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectDelaySyncRate);
	FM->setButtonObj(interfaceButton3, buttonPress, functSelectDelayTime);
	FM->setButtonObj(interfaceButton4, buttonPress, functSelectDelayFeedback);
	FM->setButtonObj(interfaceButton7, buttonPress, functDelayCancel);

	showDelayScreen();

	displayType = display_t::masterValues;
	isDelayScreen = true;
}

void cMasterParams::switchToReverbScreen()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearButton(interfaceButtonShift);

	FM->setButtonObj(interfaceButton0, buttonPress, functSelectReverbSize);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectReverbDamp);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectReverbPredelay);
	FM->setButtonObj(interfaceButton3, buttonPress, functSelectReverbDiffusion);

	FM->setButtonObj(interfaceButton7, buttonPress, functReverbCancel);

	showReverbScreen();

	displayType = display_t::masterValues;
	isReverbScreen = true;
}

//delay
static  uint8_t functSwitchToDelayWindow()
{
	MP->switchToDelayScreen();
	return 1;
}
static 	uint8_t functSelectDelayPingpongEnable()
{
	MP->selectedPlaceDelay = 0;
	MP->refreshDelayFrame();
	return 1;
}

static 	uint8_t functSelectDelaySyncEnable()
{
	MP->selectedPlaceDelay = 1;
	MP->refreshDelayFrame();
	return 1;
}

static 	uint8_t functSelectDelaySyncRate()
{
	if(!(mtProject.values.delayParams & 0b01000000)) return 1;

	MP->selectedPlaceDelay = 2;
	MP->refreshDelayFrame();
	return 1;
}
static 	uint8_t functSelectDelayTime()
{
	if(mtProject.values.delayParams & 0b01000000) return 1;

	MP->selectedPlaceDelay = 3;
	MP->refreshDelayFrame();
	return 1;
}
static 	uint8_t functSelectDelayFeedback()
{
	MP->selectedPlaceDelay = 4;
	MP->refreshDelayFrame();
	return 1;
}

static 	uint8_t functDelayCancel()
{
	MP->switchToMaster();
	return 1;
}

//reverb
static 	uint8_t functSwitchToReverbWindow()
{
	MP->switchToReverbScreen();
	return 1;
}
static 	uint8_t functSelectReverbSize()
{
	MP->selectedPlaceReverb = 0;
	MP->refreshReverbFrame();
	return 1;
}
static 	uint8_t functSelectReverbDamp()
{
	MP->selectedPlaceReverb = 1;
	MP->refreshReverbFrame();
	return 1;
}
static	uint8_t functSelectReverbPredelay()
{
	MP->selectedPlaceReverb = 2;
	MP->refreshReverbFrame();
	return 1;
}
static	uint8_t functSelectReverbDiffusion()
{
	MP->selectedPlaceReverb = 3;
	MP->refreshReverbFrame();
	return 1;
}
static  uint8_t functReverbCancel()
{
	MP->switchToMaster();
	return 1;
}


void cMasterParams::calcTrackLevel(uint8_t n)
{
	float localTrackLevel = instrumentPlayer[n].getRMSValue();
	if(localTrackLevel == - 1.0f) return;

	trackLevel[n].measureSum += localTrackLevel;
	trackLevel[n].measureCounter++;

	if(trackLevel[n].measureCounter == 10)
	{
		trackLevel[n].measureCounter = 0;

		uint16_t localMeasureSum = trackLevel[n].measureSum * 100;

		if(localMeasureSum < 1) localMeasureSum = 1;

		uint8_t localLevel = masterLevelTab[localMeasureSum - 1];
/*
 		bardziej rozbudowane obliczenia dla zrozumienia 0.0085 =
		//float localMeasureSum = (measureSum/10)/0.85;
//		if(localMeasureSum < 0.001f) localMeasureSum = 0.001f;
//
//		uint8_t localLevel = logarithmicLevelTab[ (uint8_t)(localMeasureSum * LOGHARITMIC_LEVEL_TAB_SIZE) - 1];
*/
		//if(n == 0) Serial.printf("MeasureSum: %0.03f, localLevel: %d, trackLevel: %d\n",trackLevel[n].measureSum, localLevel, trackLevel[n].value);

		if(((trackLevel[n].timer > 500 )) && (trackLevel[n].value != 0 ))
		{
			trackLevel[n].timer = 0;
			trackLevel[n].value--;
		}
		if(localLevel > (trackLevel[n].value + 1 )) trackLevel[n].value = localLevel;

		trackLevel[n].measureSum = 0;
	}

}

//mixer
static uint8_t functSoloMute(uint8_t state)
{
	if(MP->displayType == cMasterParams::display_t::masterValues) return 1;

	MP->isSolo = state;
	MP->showMixerScreen();
	return 1;
}
static uint8_t functSoloMuteTrack(uint8_t n,uint8_t state)
{
	if(MP->displayType == cMasterParams::display_t::masterValues) return 1;

	if(state == buttonPress)
	{
		if(MP->isSolo)
		{
			if((uint8_t)MP->soloTrack == n)
			{
				MP->soloTrack = -1;
				for(uint8_t i = 0; i < 8; i++)
				{
					mtProject.values.trackMute[i] = 0;
				}
			}
			else
			{
				for(uint8_t i = 0; i < 8; i++)
				{
					mtProject.values.trackMute[i] = 1;
				}
				mtProject.values.trackMute[n] = 0;
				MP->soloTrack = n;
			}
		}
		else
		{
			mtProject.values.trackMute[n] = !mtProject.values.trackMute[n];
			MP->soloTrack = -1;
		}

		for(uint8_t i = 0; i < 8 ; i++)
		{
			engine.muteTrack(i, mtProject.values.trackMute[i]);
			MP->showLevelBar(i);
		}
		MP->showMixerScreen();

	}
	return 1;
}
static  uint8_t functSwitchModeMaster(uint8_t state)
{
	if(state == buttonPress)
	{
		MP->selectedPlace = 0;

		if(MP->displayType == cMasterParams::display_t::masterValues)
		{
			MP->switchToMixer();
		}
		else if(MP->displayType == cMasterParams::display_t::mixer)
		{
			if(MP->isDelayScreen) MP->switchToDelayScreen();
			else if(MP->isReverbScreen) MP->switchToReverbScreen();
			else MP->switchToMaster();
		}
/*		CE->clearAllNodes();
		CE->cancelMultiFrame();*/

	}
	else if(state == buttonHold)
	{
//		CE->resizeLabelConfigMaster();

		MP->exitOnButtonRelease = 1;
		//CE->selectedPlace[mtConfigModeMaster] = 0;
//		MP->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(MP->sourceModuleButton == interfaceButtonSampleRec)
		{
			if(MP->exitOnButtonRelease) MP->eventFunct(eventSwitchToPreviousModule, MP, &MP->sourceModuleButton, 0);
		}
		else
		{
			if(MP->exitOnButtonRelease) MP->eventFunct(eventSwitchToPreviousModule, MP, 0, 0);
		}
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

	newFileManager.setProjectStructChanged();

	MP->showVolume();
}

void changeLimiterAttack(int16_t value)
{
	value *= LIMITER_ATTACK_MAX/100;

	if(mtProject.values.limiterAttack + value < LIMITER_ATTACK_MIN) mtProject.values.limiterAttack = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterAttack + value > LIMITER_ATTACK_MAX) mtProject.values.limiterAttack = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterAttack += value;

	engine.setLimiterAttack(mtProject.values.limiterAttack);

	newFileManager.setProjectStructChanged();

	MP->showLimiterAttack();
}

void changeLimiterRelease(int16_t value)
{
	float fvalue = value * (LIMITER_RELEASE_MAX/100);

	if(mtProject.values.limiterRelease + fvalue < LIMITER_RELEASE_MIN) mtProject.values.limiterRelease = LIMITER_RELEASE_MIN;
	else if(mtProject.values.limiterRelease + fvalue > LIMITER_RELEASE_MAX) mtProject.values.limiterRelease = LIMITER_RELEASE_MAX;
	else mtProject.values.limiterRelease += fvalue;


	engine.setLimiterRelease(mtProject.values.limiterRelease);

	newFileManager.setProjectStructChanged();

	MP->showLimiterRelease();
}

void changeLimiterTreshold(int16_t value)
{
	int32_t val = value * LIMITER_TRESHOLD_MAX/100; // mnoznik jest na tyle duzy ze przy mocnym pokreceniu iloczyn przekracza 16bit

	if(val> MAX_SIGNED_16BIT) value = MAX_SIGNED_16BIT;
	else if(val < MIN_SIGNED_16BIT) value = MIN_SIGNED_16BIT;
	else value = val;

	if(mtProject.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.values.limiterTreshold += value;

	engine.setLimiterTreshold(mtProject.values.limiterTreshold);

	newFileManager.setProjectStructChanged();

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

	newFileManager.setProjectStructChanged();
	MP->showBitDepth();
}

void setDefaultMasterVolume()
{
	mtProject.values.volume = 50;
	engine.setHeadphonesVolume(mtProject.values.volume);

	newFileManager.setProjectStructChanged();

	MP->showVolume();
}
void setDefaultLimiterAttack()
{
	mtProject.values.limiterAttack = 100;
	engine.setLimiterAttack(mtProject.values.limiterAttack);

	newFileManager.setProjectStructChanged();

	MP->showLimiterAttack();
}
void setDefaultLimiterRelease()
{
	mtProject.values.limiterRelease = 0.512;

	engine.setLimiterRelease(mtProject.values.limiterRelease);

	newFileManager.setProjectStructChanged();

	MP->showLimiterRelease();
}
void setDefaultLimiterTreshold()
{
	mtProject.values.limiterTreshold = 16384;
	engine.setLimiterTreshold(mtProject.values.limiterTreshold);

	newFileManager.setProjectStructChanged();

	MP->showLimiterTreshold();
}
void setDefaultBithDepth()
{
	mtProject.values.bitDepth = 16;
	engine.setBitDepth(mtProject.values.bitDepth);

	newFileManager.setProjectStructChanged();
	MP->showBitDepth();
}



void cMasterParams::changeDelayPingPongEnable(int16_t val)
{
	if(val > 0) mtProject.values.delayParams &= 0b01111111;
	else if( val < 0) mtProject.values.delayParams |= 0b10000000;

	engine.setDelayParams(mtProject.values.delayParams);

	newFileManager.setProjectStructChanged();

	showDelayPingPongEnable();

}
void cMasterParams::changeDelaySyncEnable(int16_t val)
{
	if(val > 0) mtProject.values.delayParams &= 0b10111111;
	else if( val < 0) mtProject.values.delayParams |= 0b01000000;

	engine.setDelayParams(mtProject.values.delayParams);
	if(val > 0) engine.setDelayTime(mtProject.values.delayTime);

	newFileManager.setProjectStructChanged();

	showDelaySyncEnable();
}
void cMasterParams::changeDelayRate(int16_t val)
{
	uint8_t temp = mtProject.values.delayParams & 0b00111111;

	if(temp + val < 0) temp = 0;
	else if(temp + val > 18) temp = 18;
	else temp+=val;

	mtProject.values.delayParams &= 0b11000000;
	mtProject.values.delayParams |= temp;

	engine.setDelayParams(mtProject.values.delayParams);

	newFileManager.setProjectStructChanged();

	showDelayRate();
}
void cMasterParams::changeDelayTime(int16_t val)
{
	if(mtProject.values.delayTime < 500) val *= 10;
	else val *= 30;

	if(mtProject.values.delayTime + val < DELAY_TIME_MIN) mtProject.values.delayTime = DELAY_TIME_MIN;
	else if(mtProject.values.delayTime + val > DELAY_TIME_MAX) mtProject.values.delayTime = DELAY_TIME_MAX;
	else mtProject.values.delayTime += val;

	engine.setDelayTime(mtProject.values.delayTime);

	newFileManager.setProjectStructChanged();

	showDelayTime();

	newFileManager.setProjectStructChanged();
}
void cMasterParams::changeDelayFeedback(int16_t val)
{
	if(mtProject.values.delayFeedback + val < DELAY_FEEDBACK_MIN) mtProject.values.delayFeedback = DELAY_FEEDBACK_MIN;
	else if(mtProject.values.delayFeedback + val > DELAY_FEEDBACK_MAX) mtProject.values.delayFeedback = DELAY_FEEDBACK_MAX;
	else mtProject.values.delayFeedback += val;

	engine.setDelayFeedback(mtProject.values.delayFeedback);

	newFileManager.setProjectStructChanged();

	showDelayFeedback();
}

void cMasterParams::changeReverbSize(int16_t val)
{
	float v = 0.01 * val;

	if(mtProject.values.reverb.size + v > REVERB_SIZE_MAX) mtProject.values.reverb.size  = REVERB_SIZE_MAX;
	else if(mtProject.values.reverb.size + v < REVERB_SIZE_MIN) mtProject.values.reverb.size  = REVERB_SIZE_MIN;
	else  mtProject.values.reverb.size  += v;

	engine.setReverbSize(mtProject.values.reverb.size);

	showReverbSize();
}
void cMasterParams::changeReverbDamp(int16_t val)
{
	float v = 0.01 * val;

	if(mtProject.values.reverb.damp + v > REVERB_DAMP_MAX) mtProject.values.reverb.damp = REVERB_DAMP_MAX;
	else if(mtProject.values.reverb.damp + v < REVERB_DAMP_MIN) mtProject.values.reverb.damp = REVERB_DAMP_MIN;
	else  mtProject.values.reverb.damp += v;

	engine.setReverbDamp(mtProject.values.reverb.damp);

	showReverbDamp();

	newFileManager.setProjectStructChanged();
}
void cMasterParams::changeReverbPredelay(int16_t val)
{
	float v = 0.01 * val;

	if(mtProject.values.reverb.predelay + v > REVERB_PREDELAY_MAX) mtProject.values.reverb.predelay = REVERB_PREDELAY_MAX;
	else if(mtProject.values.reverb.predelay + v < REVERB_PREDELAY_MIN) mtProject.values.reverb.predelay = REVERB_PREDELAY_MIN;
	else  mtProject.values.reverb.predelay += v;

	engine.setReverbPredelay(mtProject.values.reverb.predelay);

	showReverbPredelay();

	newFileManager.setProjectStructChanged();
}
void cMasterParams::changeReverbDiffusion(int16_t val)
{
	float v = 0.01 * val;

	if(mtProject.values.reverb.diffusion + v > REVERB_DIFFUSION_MAX) mtProject.values.reverb.diffusion = REVERB_DIFFUSION_MAX;
	else if(mtProject.values.reverb.diffusion + v < REVERB_DIFFUSION_MIN) mtProject.values.reverb.diffusion = REVERB_DIFFUSION_MIN;
	else  mtProject.values.reverb.diffusion += v;

	engine.setReverbDiffusion(mtProject.values.reverb.diffusion);

	showReverbDiffusion();

	newFileManager.setProjectStructChanged();
}

void cMasterParams::setDefaultReverbSize()
{
	mtProject.values.reverb.size = DEFAULT_REVERB_SIZE;
	engine.setReverbSize(mtProject.values.reverb.size);

	showReverbSize();

	newFileManager.setProjectStructChanged();
}
void cMasterParams::setDefaultReverbDamp()
{
	mtProject.values.reverb.damp = DEFAULT_REVERB_DAMP;

	engine.setReverbDamp(mtProject.values.reverb.damp);

	showReverbDamp();

	newFileManager.setProjectStructChanged();
}
void cMasterParams::setDefaultReverbPredelay()
{
	mtProject.values.reverb.predelay = DEFAULT_REVERB_PREDELAY;

	engine.setReverbPredelay(mtProject.values.reverb.predelay);

	showReverbPredelay();

	newFileManager.setProjectStructChanged();
}
void cMasterParams::setDefaultReverbDiffusion()
{
	mtProject.values.reverb.diffusion = DEFAULT_REVERB_DIFFUSION;

	engine.setReverbDiffusion(mtProject.values.reverb.diffusion);

	showReverbDiffusion();

	newFileManager.setProjectStructChanged();
}

void cMasterParams::setDefaultDelayPingPongEnable()
{
	mtProject.values.delayParams &= 0b01111111;
	engine.setDelayParams(mtProject.values.delayParams);
	newFileManager.setProjectStructChanged();
	showDelayPingPongEnable();
}
void cMasterParams::setDefaultDelaySyncEnable()
{
	mtProject.values.delayParams &= 0b10111111;
	engine.setDelayParams(mtProject.values.delayParams);
	newFileManager.setProjectStructChanged();
	showDelaySyncEnable();
}
void cMasterParams::setDefaultDelayRate()
{
	mtProject.values.delayParams &= 0b11000000;
	engine.setDelayParams(mtProject.values.delayParams);
	newFileManager.setProjectStructChanged();
	showDelayRate();
}
void cMasterParams::setDefaultDelayTime()
{
	mtProject.values.delayTime = 500;
	engine.setDelayTime(mtProject.values.delayTime);
	newFileManager.setProjectStructChanged();
	showDelayTime();
}
void cMasterParams::setDefaultDelayFeedback()
{
	mtProject.values.delayFeedback = 50;
	engine.setDelayFeedback(mtProject.values.delayFeedback);

	newFileManager.setProjectStructChanged();

	showDelayFeedback();
}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == buttonPress)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;

		padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);
		uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
		sequencer.handleNoteOn(
							Sequencer::GRID_OUTSIDE_PATTERN,
							noteFromPad,
							-1,
							pad);
//		mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);

	}
	else if(state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0,0, pad);
//		mtPadBoard.stopInstrument(pad);
		uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
		sequencer.handleNoteOff(Sequencer::GRID_OUTSIDE_PATTERN, noteFromPad, 0, pad);
	}

	return 1;

}

