


#include "performanceMode.h"

#include "keyScanner.h"
#include "mtPadBoard.h"

#include "mtPadsBacklight.h"

//#include "mtAudioEngine.h"

#include "interfacePopups.h"

#include "mtSequencer.h"



cPerformanceMode performanceMode;
static cPerformanceMode* PM = &performanceMode;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functCopy();
static uint8_t functShift(uint8_t value);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchMode(uint8_t button);



static  uint8_t functInstrument(uint8_t state);
static uint8_t functStepNote(uint8_t value);



static uint8_t functActionButton(uint8_t button, uint8_t state);



static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


static elapsedMillis refreshTime;

void cPerformanceMode::update()
{
	if(refreshTime < 100) return;
	refreshTime = 0;

	if(refreshMaster == 1)
	{
		refreshMaster = 0;
		if(mode == mtPerformanceMaster) showPerformanceMaster();
	}



}

void cPerformanceMode::start(uint32_t options)
{
	moduleRefresh = 1;
	performanceEditState = 0;

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchMode);


	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	setDefaultScreenFunct();





	if(mode == mtPerformanceMaster)
	{
		PM->showPerformanceMaster();
		PM->setPerformanceMaster();
	}
	else
	{
		PM->showPerformanceFxes();
		PM->setPerformanceFxes();
	}

}

void cPerformanceMode::stop()
{
	moduleRefresh = 0;
	mtPadBoard.releaseAllInstrument();
	padsBacklight.clearAllPads(1, 1, 1);
}

void cPerformanceMode::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButtonShift, functShift);

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



//==============================================================================================================
void cPerformanceMode::setPerformanceMaster()
{

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);




	padsBacklight.clearAllPads(0, 1, 1);
}

void cPerformanceMode::setPerformanceFxes()
{

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	padsBacklight.clearAllPads(0, 1, 1);
	for(uint8_t i = 0; i < 48; i+=2)
	{
		padsBacklight.setBackLayer(1, 8, i);
	}


}

void cPerformanceMode::toggleEditState()
{
	performanceEditState = !performanceEditState;

	if(performanceEditState)
	{
		showEditFrame(performanceEditPlace);
	}
	else
	{
		hideEditFrame();
	}

}


void cPerformanceMode::toggleTrackPerformanceState(uint8_t track)
{
	tracksPerformanceState[track] = !tracksPerformanceState[track];


	if(tracksPerformanceState[track] == 0)
	{
		for(uint8_t i = 0; i < performanceFxesCount; i++)
		{
			clearPerformanceValues(track, i);
		}
	}
	else
	{
		refreshPerformanceValuesForTrack(track);
	}
}


void cPerformanceMode::clearPerformanceValues(uint8_t track, uint8_t fx)
{
	switch(fx)
	{
	case mtPerfFxVolume:
	{
		instrumentPlayer[track].changeVolumePerformanceMode(0);
		break;
	}
	case mtPerfPanning:
	{
		instrumentPlayer[track].changePanningPerformanceMode(0);
		break;
	}
	case mtPerfLowPass:
	{
		instrumentPlayer[track].changeCutoffPerformanceMode(0);
		instrumentPlayer[track].changeFilterTypePerformanceMode(0);
		break;
	}
	case mtPerfHighPass:
	{
		instrumentPlayer[track].changeCutoffPerformanceMode(0);
		instrumentPlayer[track].changeFilterTypePerformanceMode(0);
		break;
	}
	case mtPerfBandPass:
	{
		instrumentPlayer[track].changeCutoffPerformanceMode(0);
		instrumentPlayer[track].changeFilterTypePerformanceMode(0);
		break;
	}
	case mtPerfReverbSend:
	{
		instrumentPlayer[track].changeReverbSendPerformanceMode(0);
		break;
	}
	case mtPerfSampleStart:
	{
		instrumentPlayer[track].changeStartPointPerformanceMode(0);
		break;
	}
	case mtPerfTune:
	{
		instrumentPlayer[track].changeTunePerformanceMode(0);
		break;
	}
	case mtPerfSamplePlayback:
	{

		break;
	}
	case mtPerfStepStutter:
	{

		break;
	}
	case mtPerfPatternPlayMode:
	{

	}
	case mtPerfFx11:
	{

	}
	default: break;
	}


}


void cPerformanceMode::refreshPerformanceValuesForTrack(uint8_t track)
{
	for(uint8_t fx = 0; fx < performanceFxesCount; fx++)
	{
		switch(fx)
		{
		case mtPerfFxVolume:
		{
			instrumentPlayer[track].changeVolumePerformanceMode(fxValues[fx]);
			break;
		}
		case mtPerfPanning:
		{
			instrumentPlayer[track].changePanningPerformanceMode(fxValues[fx]);
			break;
		}
		case mtPerfLowPass:
		{
			if(fxValues[fx] != 0)
			{
				instrumentPlayer[track].changeFilterTypePerformanceMode(1);
				instrumentPlayer[track].changeCutoffPerformanceMode(fxValues[fx]);
			}
			else if(fxValues[mtPerfHighPass] != 0 && fxValues[mtPerfBandPass] != 0)
			{
				instrumentPlayer[track].changeFilterTypePerformanceMode(0);
			}
			break;
		}
		case mtPerfHighPass:
		{
			if(fxValues[fx] != 0)
			{
				instrumentPlayer[track].changeFilterTypePerformanceMode(2);
				instrumentPlayer[track].changeCutoffPerformanceMode(fxValues[fx]);
			}
			else if(fxValues[mtPerfLowPass] != 0 && fxValues[mtPerfBandPass] != 0)
			{
				instrumentPlayer[track].changeFilterTypePerformanceMode(0);
			}
			break;
		}
		case mtPerfBandPass:
		{
			if(fxValues[fx] != 0)
			{
				instrumentPlayer[track].changeFilterTypePerformanceMode(3);
				instrumentPlayer[track].changeCutoffPerformanceMode(fxValues[fx]);
			}
			else if(fxValues[mtPerfLowPass] != 0 && fxValues[mtPerfHighPass] != 0)
			{
				instrumentPlayer[track].changeFilterTypePerformanceMode(0);
			}

			break;
		}
		case mtPerfReverbSend:
		{
			instrumentPlayer[track].changeReverbSendPerformanceMode(fxValues[fx]);
			break;
		}
		case mtPerfSampleStart:
		{
			instrumentPlayer[track].changeStartPointPerformanceMode(fxValues[fx]);
			break;
		}
		case mtPerfTune:
		{
			instrumentPlayer[track].changeTunePerformanceMode(fxValues[fx]);
			break;
		}
		case mtPerfSamplePlayback:
		{

			break;
		}
		case mtPerfStepStutter:
		{

			break;
		}
		case mtPerfPatternPlayMode:
		{

		}
		case mtPerfFx11:
		{

		}
		default: break;
		}

	}
}


//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{

	for(uint8_t i = 0; i<performanceFxesCount; i++)
	{
		if(PM->fxPerformanceState[i])
		{
			switch(i)
			{
			case mtPerfFxVolume:
			{
				if(PM->fxValues[i] + value > 100) PM->fxValues[i] = 100;
				else if(PM->fxValues[i] + value < -100) PM->fxValues[i] = -100;
				else PM->fxValues[i] += value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeVolumePerformanceMode(PM->fxValues[i]);
				}
				break;
			}
			case mtPerfPanning:
			{
				if(PM->fxValues[i] + value > 100) PM->fxValues[i] = 100;
				else if(PM->fxValues[i] + value < -100) PM->fxValues[i] = -100;
				else PM->fxValues[i] += value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changePanningPerformanceMode(PM->fxValues[i]);
				}
				break;
			}
			case mtPerfLowPass:
			{
				if(PM->fxValues[i] + value > 100) PM->fxValues[i] = 100;
				else if(PM->fxValues[i] + value < -100) PM->fxValues[i] = -100;
				else PM->fxValues[i] += value;
				PM->fxValues[mtPerfHighPass] = 0;
				PM->fxValues[mtPerfBandPass] = 0;
				PM->showPerformaceValue(mtPerfBandPass);
				PM->showPerformaceValue(mtPerfHighPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(1);
						instrumentPlayer[j].changeCutoffPerformanceMode(PM->fxValues[i]);
					}
				}
				break;
			}
			case mtPerfHighPass:
			{
				if(PM->fxValues[i] + value > 100) PM->fxValues[i] = 100;
				else if(PM->fxValues[i] + value < -100) PM->fxValues[i] = -100;
				else PM->fxValues[i] += value;
				PM->fxValues[mtPerfLowPass] = 0;
				PM->fxValues[mtPerfBandPass] = 0;
				PM->showPerformaceValue(mtPerfBandPass);
				PM->showPerformaceValue(mtPerfLowPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(2);
						instrumentPlayer[j].changeCutoffPerformanceMode(PM->fxValues[i]);
					}
				}
				break;
			}
			case mtPerfBandPass:
			{
				if(PM->fxValues[i] + value > 100) PM->fxValues[i] = 100;
				else if(PM->fxValues[i] + value < -100) PM->fxValues[i] = -100;
				else PM->fxValues[i] += value;
				PM->fxValues[mtPerfLowPass] = 0;
				PM->fxValues[mtPerfHighPass] = 0;
				PM->showPerformaceValue(mtPerfLowPass);
				PM->showPerformaceValue(mtPerfHighPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(3);
						instrumentPlayer[j].changeCutoffPerformanceMode(PM->fxValues[i]);
					}
				}
				break;
			}
			case mtPerfReverbSend:
			{
				if(PM->fxValues[i] + value > 100) PM->fxValues[i] = 100;
				else if(PM->fxValues[i] + value < -100) PM->fxValues[i] = -100;
				else PM->fxValues[i] += value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeReverbSendPerformanceMode(PM->fxValues[i]);
				}
				break;
			}
			case mtPerfSampleStart:
			{


				break;
			}
			case mtPerfTune:
			{
				if(PM->fxValues[i] + value > 48) PM->fxValues[i] = 48;
				else if(PM->fxValues[i] + value < -48) PM->fxValues[i] = -48;
				else PM->fxValues[i] += value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeTunePerformanceMode(PM->fxValues[i]);
				}

				break;
			}
			case mtPerfSamplePlayback:
			{
				if(PM->fxValues[i] + value > 1) PM->fxValues[i] = 1;
				else if(PM->fxValues[i] + value < 0) PM->fxValues[i] = 0;
				else PM->fxValues[i] += value;

				break;
			}
			case mtPerfStepStutter:
			{
				if(PM->fxValues[i] + value > 11) PM->fxValues[i] = 11;
				else if(PM->fxValues[i] + value < 0) PM->fxValues[i] = 0;
				else PM->fxValues[i] += value;

				break;
			}
			case mtPerfPatternPlayMode:
			{
				if(PM->fxValues[i] + value > 2) PM->fxValues[i] = 2;
				else if(PM->fxValues[i] + value < 0) PM->fxValues[i] = 0;
				else PM->fxValues[i] += value;

				break;
			}
			case mtPerfFx11:
			{

			}
			default: break;
			}

			PM->showPerformaceValue(i);
		}
	}



	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	if(PM->performanceEditState && tactButtons.isButtonPressed(interfaceButtonShift))
	{
		uint8_t target_fx = PM->fxPlaces[PM->performanceEditPlace];
		if(PM->performanceEditPlace > 0) PM->performanceEditPlace--;
		uint8_t dest_fx = PM->fxPlaces[PM->performanceEditPlace];

		PM->fxPlaces[PM->performanceEditPlace] = target_fx;
		PM->fxPlaces[PM->performanceEditPlace+1] = dest_fx;

		PM->refreshFxNames();
		PM->showEditFrame(PM->performanceEditPlace);
	}
	else if(PM->performanceEditState)
	{
		if(PM->performanceEditPlace > 0) PM->performanceEditPlace--;
		PM->showEditFrame(PM->performanceEditPlace);
	}

	return 1;
}


static  uint8_t functRight()
{
	if(PM->performanceEditState && tactButtons.isButtonPressed(interfaceButtonShift))
	{
		uint8_t target_fx = PM->fxPlaces[PM->performanceEditPlace];
		if(PM->performanceEditPlace < 11) PM->performanceEditPlace++;
		uint8_t dest_fx = PM->fxPlaces[PM->performanceEditPlace];

		PM->fxPlaces[PM->performanceEditPlace] = target_fx;
		PM->fxPlaces[PM->performanceEditPlace-1] = dest_fx;

		PM->refreshFxNames();
		PM->showEditFrame(PM->performanceEditPlace);
	}
	else if(PM->performanceEditState)
	{
		if(PM->performanceEditPlace < 11) PM->performanceEditPlace++;
		PM->showEditFrame(PM->performanceEditPlace);
	}

	return 1;
}


static  uint8_t functUp()
{


	return 1;
}


static  uint8_t functDown()
{



	return 1;
}



//=========================================================================================================
static  uint8_t functPlayAction()
{

	if (sequencer.getSeqState() == 0)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
		}
		else
		{
			sequencer.playPattern();
		}

		//PTE->lastPlayedPattern = 0;
	}
	else if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	return 1;
}




static  uint8_t functRecAction()
{
	PM->toggleEditState();

	return 1;
}

static uint8_t functShift(uint8_t value)
{




	return 1;
}


static uint8_t functSwitchModule(uint8_t button)
{

	PM->eventFunct(eventSwitchModule,PM,&button,0);

	return 1;
}


static  uint8_t functSwitchMode(uint8_t button)
{
/*
	if(PM->mode == mtPerformanceMaster)
	{
		PM->mode = 1;
		PM->showPerformanceFxes();
		PM->setPerformanceFxes();

	}
	else
	{
		PM->mode = 0;
		PM->showPerformanceMaster();
		PM->setPerformanceMaster();
	}


	PM->activateLabelsBorder();
*/



	return 1;
}





//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################



static uint8_t functActionButton(uint8_t button, uint8_t state)
{
	if(PM->mode == mtPerformanceMaster)
	{
		if(state == buttonPress)
		{
			 if(mtProject.values.trackMute[button] == 0) mtProject.values.trackMute[button] = 1;
			 else mtProject.values.trackMute[button] = 0;

			 engine.muteTrack(button, mtProject.values.trackMute[button]);

			 PM->refreshMaster = 1;
		}
		else if(state == buttonRelease)
		{
			if(tactButtons.isButtonPressed(interfaceButtonShift))
			{
				 if(mtProject.values.trackMute[button] == 0) mtProject.values.trackMute[button] = 1;
				 else mtProject.values.trackMute[button] = 0;

				 engine.muteTrack(button, mtProject.values.trackMute[button]);

				 PM->refreshMaster = 1;
			}
		}
	}
	else
	{
		if(state == buttonPress)
		{
			PM->toggleTrackPerformanceState(button);
			PM->refreshTracksState();
		}
		else if(state == buttonRelease)
		{

		}
	}



	return 1;
}


//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################


void cPerformanceMode::lightUpPadBoard()
{
	padsBacklight.clearAllPads(0, 1, 1);

	if(mtProject.values.lastUsedInstrument >= 0 && mtProject.values.lastUsedInstrument <= 48)
	{
		padsBacklight.setBackLayer(1, 20, mtProject.values.lastUsedInstrument);
	}
}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{

	if(state == 1)
	{
		padsBacklight.setFrontLayer(1,20, pad);

		PM->fxPerformanceState[pad%12] = pad/12 + 1;
	}
	else if(state == 0)
	{

		if(pad/12 == 0)
		{
			PM->fxValues[pad%12] = 0;
			for(uint8_t j = 0; j < 8; j++)
			{

				PM->clearPerformanceValues(j, pad%12);
			}
			PM->showPerformaceValue(pad%12);
		}

		padsBacklight.setFrontLayer(0,0, pad);

		PM->fxPerformanceState[pad%12] = 0;


	}

	return 1;
}


