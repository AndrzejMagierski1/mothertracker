


#include "performanceMode.h"

#include "keyScanner.h"
#include "mtPadBoard.h"

#include "mtPadsBacklight.h"


#include "interfacePopups.h"


#include "mtFileManager.h"

#include "mtSequencer.h"




cPerformanceMode performanceMode;
static cPerformanceMode* PM = &performanceMode;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static uint8_t functShift(uint8_t value);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchMode(uint8_t button);




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
	if(refreshTrackState == 1)
	{
		refreshTrackState = 0;
		refreshTracksState();
	}
	if(refreshTrackPattern == 1)
	{
		refreshTrackPattern = 0;
		refreshTracksPatterns();
	}


}

void cPerformanceMode::start(uint32_t options)
{
	for (uint8_t q = 0; q < 8; q++)
	{
		mtProject.values.perfTracksPatterns[q] = mtProject.values.actualPattern;
	}

	for(uint8_t track = 0; track < 8; track++)
	{
		int8_t tracksPerformanceState[track] = 0; // ustaw na 0

		if(PM->tracksPerformanceState[track] == 1) // a teraz sprawdz czy moze jednak nie jest 1 :>
		{
			for(uint8_t i = 0; i < 12; i++)
			{
				refreshPerformanceValuesForTrack(track,  mtProject.values.perfFxPlaces[i]);
			}
		}
	}



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
	for(uint8_t track = 0; track < 8; track++)
	{
		for(uint8_t fx = 0; fx < performanceFxesCount; fx++)
		{
			clearPerformanceValues(track, fx);
		}
	}

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
		padsBacklight.setBackLayer(1, mtConfig.values.padsLightBackWeek, i);
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

uint8_t cPerformanceMode::wasPatternOntrackChenged(uint8_t track)
{
	return trackPatternChange[track] > 1;
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
		for(uint8_t i = 0; i < 12; i++)
		{
			refreshPerformanceValuesForTrack(track,  mtProject.values.perfFxPlaces[i]);
		}
	}
}

void cPerformanceMode::clearPerformanceValues(uint8_t track, uint8_t fx)
{
	switch(fx)
	{
	case mtPerfFxVolume:
	{
		instrumentPlayer[track].endVolumePerformanceMode();
		break;
	}
	case mtPerfPanning:
	{
		instrumentPlayer[track].endPanningPerformanceMode();
		break;
	}
	case mtPerfLowPass:
	{
		instrumentPlayer[track].endCutoffPerformanceMode();
		instrumentPlayer[track].endFilterTypePerformanceMode();
		break;
	}
	case mtPerfHighPass:
	{
		instrumentPlayer[track].endCutoffPerformanceMode();
		instrumentPlayer[track].endFilterTypePerformanceMode();
		break;
	}
	case mtPerfBandPass:
	{
		instrumentPlayer[track].endCutoffPerformanceMode();
		instrumentPlayer[track].endFilterTypePerformanceMode();
		break;
	}
	case mtPerfReverbSend:
	{
		instrumentPlayer[track].endReverbSendPerformanceMode();
		break;
	}
	case mtPerfSampleStart:
	{
		instrumentPlayer[track].endStartPointPerformanceMode();
		break;
	}
	case mtPerfSampleEnd:
	{
		instrumentPlayer[track].endEndPointPerformanceMode();
		break;
	}
	case mtPerfWavetablePos:
	{
		instrumentPlayer[track].endWavetableWindowPerformanceMode();
		break;
	}
	case mtPerfTune:
	{
		instrumentPlayer[track].endTunePerformanceMode();
		break;
	}
	case mtPerfSamplePlayback:
	{
		instrumentPlayer[track].endSamplePlaybackPerformanceMode();
		break;
	}
	case mtPerfStepStutter:
	{
		sequencer.setPerformanceStutter(track, 0);
		break;
	}
	case mtPerfPatternPlayMode:
	{
		sequencer.setPerformancePlayMode(track, 0);
		break;
	}
	case mtPerfPatternLength:
	{
		//fxValues[fx][] = -1;
		sequencer.setPerformancePatternLengthFromFxVal(0);
		break;
	}




	default: break;
	}


}


void cPerformanceMode::refreshPerformanceValuesForTrack(uint8_t track, uint8_t fx)
{
	if(fx == 0) return;

	switch(fx)
	{
	case mtPerfFxVolume:
	{
		instrumentPlayer[track].changeVolumePerformanceMode(FX_VALUE(fx));
		break;
	}
	case mtPerfPanning:
	{
		instrumentPlayer[track].changePanningPerformanceMode(FX_VALUE(fx));
		break;
	}
	case mtPerfLowPass:
	{
		if(FX_VALUE(fx) != 0)
		{
			instrumentPlayer[track].changeFilterTypePerformanceMode(1);
			instrumentPlayer[track].changeCutoffPerformanceMode(FX_VALUE(fx));
		}
		else if(FX_VALUE(mtPerfHighPass) != 0 && FX_VALUE(mtPerfBandPass) != 0)
		{
			instrumentPlayer[track].changeFilterTypePerformanceMode(0);
		}
		break;
	}
	case mtPerfHighPass:
	{
		if(FX_VALUE(fx) != 0)
		{
			instrumentPlayer[track].changeFilterTypePerformanceMode(2);
			instrumentPlayer[track].changeCutoffPerformanceMode(FX_VALUE(fx));
		}
		else if(FX_VALUE(mtPerfLowPass) != 0 && FX_VALUE(mtPerfBandPass) != 0)
		{
			instrumentPlayer[track].changeFilterTypePerformanceMode(0);
		}
		break;
	}
	case mtPerfBandPass:
	{
		if(FX_VALUE(fx) != 0)
		{
			instrumentPlayer[track].changeFilterTypePerformanceMode(3);
			instrumentPlayer[track].changeCutoffPerformanceMode(FX_VALUE(fx));
		}
		else if(FX_VALUE(mtPerfLowPass) != 0 && FX_VALUE(mtPerfHighPass) != 0)
		{
			instrumentPlayer[track].changeFilterTypePerformanceMode(0);
		}

		break;
	}
	case mtPerfReverbSend:
	{
		instrumentPlayer[track].changeReverbSendPerformanceMode(FX_VALUE(fx));
		break;
	}
	case mtPerfSampleStart:
	{
		instrumentPlayer[track].changeStartPointPerformanceMode(map(FX_VALUE(fx),-100,100,-MAX_16BIT,MAX_16BIT));
		break;
	}
	case mtPerfSampleEnd:
	{
		instrumentPlayer[track].changeEndPointPerformanceMode(map(FX_VALUE(fx),-100,100,-MAX_16BIT,MAX_16BIT));
		break;
	}
	case mtPerfWavetablePos:
	{
		instrumentPlayer[track].changeWavetableWindowPerformanceMode(map(FX_VALUE(fx),-100,100,-MAX_WAVETABLE_WINDOW,MAX_WAVETABLE_WINDOW));
		break;
	}
	case mtPerfTune:
	{
		instrumentPlayer[track].changeTunePerformanceMode(FX_VALUE(fx));
		break;
	}
	case mtPerfSamplePlayback:
	{
		instrumentPlayer[track].changeSamplePlaybackPerformanceMode(FX_VALUE(fx));
		break;
	}
	case mtPerfStepStutter:
	{
		sequencer.setPerformanceStutter(track, FX_VALUE(fx));
		break;
	}
	case mtPerfPatternPlayMode:
	{
		sequencer.setPerformancePlayMode(track, FX_VALUE(fx));
		break;
	}
	case mtPerfPatternLength:
	{
		sequencer.setPerformancePatternLengthFromFxVal(FX_VALUE(fx));
		break;
	}



	default: break;
	}
}


//=================================================================================
//
//=================================================================================
uint8_t cPerformanceMode::getFxFromPlace(uint8_t place)
{
	return mtProject.values.perfFxPlaces[place];
}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::savePlaceFxValue(uint8_t place, uint8_t slot, int16_t value)
{
	mtProject.values.perfFxValues[place][slot] = value;


	//mtProject.values.perfFxPlaces[place];
	//mtProject.values.perfFxValues[i][] = PM->fxValues[i][PM->activeFxValues[i]];
}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::clearHidedFx(uint8_t fx)
{

}

//=================================================================================
//
//=================================================================================
uint8_t fxAlredyTaken(uint8_t fx)
{
	for(uint8_t i = 0; i<12; i++)
	{
		if(mtProject.values.perfFxPlaces[i] == fx) return 1;
	}

	return 0;
}

//=================================================================================
//
//=================================================================================
static  uint8_t functEncoder(int16_t value)
{
	uint8_t patternChange = 0;

	for(uint8_t i = 0; i<8; i++)
	{
		if((PM->trackPatternChange[i] == 1 || PM->trackPatternChange[i] == 2) && tactButtons.isButtonPressed(i))
		{
			//if(PM->trackPatternChange[i] == 1 && mtProject.values.perfTracksPatterns[i] == 1 && value < 0) PM->trackPatternChange[i] = 1;
			//else if(PM->trackPatternChange[i] == 1 && mtProject.values.perfTracksPatterns[i] == 255 && value > 0) PM->trackPatternChange[i] = 1;
			//else
			{
				PM->trackPatternChange[i] = 2;

				if(mtProject.values.perfTracksPatterns[i] + value > 255) mtProject.values.perfTracksPatterns[i] = 255;
				else if(mtProject.values.perfTracksPatterns[i] + value < 1) mtProject.values.perfTracksPatterns[i] = 1;
				else  mtProject.values.perfTracksPatterns[i] += value;
			}

			PM->refreshTrackPattern = 1;
			patternChange = 1;
		}
	}

	if(patternChange) return 1;



	for(uint8_t place = 0; place < 12; place++)
	{
		if(PM->performancePadsState[place] == 1) continue;

		if(PM->performancePadsState[place])
		{
			int16_t mod_value = value;
			//if(PM->placePerformanceState[place] > 2) mod_value = -mod_value;

			uint8_t fx = mtProject.values.perfFxPlaces[place];
			//PM->activeFxValues[i] = PM->performancePadsState[place]-1;

			switch(fx)
			{
			case mtPerfFxVolume:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeVolumePerformanceMode(FX_VALUE(fx));
				}
				break;
			}
			case mtPerfPanning:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changePanningPerformanceMode(FX_VALUE(fx));
				}
				break;
			}
			case mtPerfLowPass:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;
				////PM->fxValues[mtPerfHighPass][] = 0;
				//PM->fxValues[mtPerfBandPass][] = 0;
				//PM->showPerformaceValue(mtPerfBandPass);
				//PM->showPerformaceValue(mtPerfHighPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(1);
						instrumentPlayer[j].changeCutoffPerformanceMode(FX_VALUE(fx));
					}
				}
				break;
			}
			case mtPerfHighPass:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;
				//PM->fxValues[mtPerfLowPass] = 0;
				//PM->fxValues[mtPerfBandPass] = 0;
				//PM->showPerformaceValue(mtPerfBandPass);
				//PM->showPerformaceValue(mtPerfLowPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(2);
						instrumentPlayer[j].changeCutoffPerformanceMode(FX_VALUE(fx));
					}
				}
				break;
			}
			case mtPerfBandPass:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;
				//PM->fxValues[mtPerfLowPass][] = 0;
				//PM->fxValues[mtPerfHighPass][] = 0;
				//PM->showPerformaceValue(mtPerfLowPass);
				//PM->showPerformaceValue(mtPerfHighPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(3);
						instrumentPlayer[j].changeCutoffPerformanceMode(FX_VALUE(fx));
					}
				}
				break;
			}
			case mtPerfReverbSend:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeReverbSendPerformanceMode(FX_VALUE(fx));
				}
				break;
			}
			case mtPerfSampleStart:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeStartPointPerformanceMode(map(FX_VALUE(fx),-100,100,-MAX_16BIT,MAX_16BIT));
				}
				break;

				break;
			}
			case mtPerfSampleEnd:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeEndPointPerformanceMode(map(FX_VALUE(fx),-100,100,-MAX_16BIT,MAX_16BIT));
				}
				break;

				break;
			}
			case mtPerfWavetablePos:
			{
				if(FX_VALUE(fx) + mod_value > 100) FX_VALUE(fx) = 100;
				else if(FX_VALUE(fx) + mod_value < -100) FX_VALUE(fx) = -100;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeWavetableWindowPerformanceMode(map(FX_VALUE(fx),-100,100,-MAX_WAVETABLE_WINDOW,MAX_WAVETABLE_WINDOW));
				}
				break;

				break;
			}
			case mtPerfTune:
			{
				if(FX_VALUE(fx) + mod_value > 48) FX_VALUE(fx) = 48;
				else if(FX_VALUE(fx) + mod_value < -48) FX_VALUE(fx) = -48;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeTunePerformanceMode(FX_VALUE(fx));
				}

				break;
			}
			case mtPerfSamplePlayback:
			{
				if(FX_VALUE(fx) + mod_value > 1) FX_VALUE(fx) = 1;
				else if(FX_VALUE(fx) + mod_value < 0) FX_VALUE(fx) = 0;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeSamplePlaybackPerformanceMode(FX_VALUE(fx));
					}
				}

				break;
			}
			case mtPerfStepStutter:
			{
				if(FX_VALUE(fx) + mod_value > 11) FX_VALUE(fx) = 11;
				else if(FX_VALUE(fx) + mod_value < 0) FX_VALUE(fx) = 0;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if (PM->tracksPerformanceState[j])
					{
						sequencer.setPerformanceStutter(j, FX_VALUE(fx));
					}
				}

				break;
			}
			case mtPerfPatternPlayMode:
			{
				if(FX_VALUE(fx) + mod_value > 2) FX_VALUE(fx) = 2;
				else if(FX_VALUE(fx) + mod_value < 0) FX_VALUE(fx) = 0;
				else FX_VALUE(fx) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						sequencer.setPerformancePlayMode(j, FX_VALUE(fx));
					}
				}

				break;
			}
			case mtPerfPatternLength:
				{

				if (FX_VALUE(fx) + mod_value > 8) FX_VALUE(fx) = 8;
				else if (FX_VALUE(fx) + mod_value < 0) FX_VALUE(fx) = 0;
				else FX_VALUE(fx) += mod_value;

				sequencer.setPerformancePatternLengthFromFxVal(FX_VALUE(fx));

				break;
			}
			default: break;
			}

			PM->showPerformaceValue(fx);



			if(PM->performanceEditState)
			{
				PM->fxValues[fx][mtProject.values.perfSelectedValues[place]] = PM->fxTempValues[fx];
				PM->savePlaceFxValue(place, mtProject.values.perfSelectedValues[place], PM->fxTempValues[fx]);

			}


		}
	}



	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	if(PM->performanceEditState && tactButtons.isButtonPressed(interfaceButtonShift))
	{
		uint8_t target_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];
		if(PM->performanceEditPlace > 0) PM->performanceEditPlace--;
		uint8_t dest_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];

		mtProject.values.perfFxPlaces[PM->performanceEditPlace] = target_fx;
		mtProject.values.perfFxPlaces[PM->performanceEditPlace+1] = dest_fx;

		PM->showEditFrame(PM->performanceEditPlace);
		PM->refreshFxNames(PM->performanceEditPlace);
		PM->refreshFxNames(PM->performanceEditPlace+1);
		PM->showPerformaceValue(target_fx);
		PM->showPerformaceValue(dest_fx);
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
		uint8_t target_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];
		if(PM->performanceEditPlace < 11) PM->performanceEditPlace++;
		uint8_t dest_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];

		mtProject.values.perfFxPlaces[PM->performanceEditPlace] = target_fx;
		mtProject.values.perfFxPlaces[PM->performanceEditPlace-1] = dest_fx;

		PM->showEditFrame(PM->performanceEditPlace);
		PM->refreshFxNames(PM->performanceEditPlace);
		PM->refreshFxNames(PM->performanceEditPlace-1);
		PM->showPerformaceValue(target_fx);
		PM->showPerformaceValue(dest_fx);
	}
	else if(PM->performanceEditState)
	{
		if(PM->performanceEditPlace < 11) PM->performanceEditPlace++;
		PM->showEditFrame(PM->performanceEditPlace);
	}

	return 1;
}

void cPerformanceMode::setPlaceNewFx(uint8_t newFx)
{
	uint8_t old_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];

	for(uint8_t track = 0; track < 8; track++)
	{
		clearPerformanceValues(track, old_fx);
	}

	mtProject.values.perfFxValues[PM->performanceEditPlace][0] = 0;
	mtProject.values.perfFxValues[PM->performanceEditPlace][1] = 0;
	mtProject.values.perfFxValues[PM->performanceEditPlace][2] = 0;
	mtProject.values.perfFxValues[PM->performanceEditPlace][3] = 0;

	fxValues[old_fx][0] = 0;
	fxValues[old_fx][1] = 0;
	fxValues[old_fx][2] = 0;
	fxValues[old_fx][3] = 0;

	mtProject.values.perfFxPlaces[PM->performanceEditPlace] = newFx;
}

static  uint8_t functUp()
{
	if(PM->performanceEditState)
	{
		uint8_t new_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace]+1;
		if(new_fx > performanceFxesCount-1) new_fx = 0;

		while(fxAlredyTaken(new_fx) &&  new_fx != 0)
		{
			new_fx++;
			if(new_fx >= performanceFxesCount) new_fx = 0;

		}

		PM->setPlaceNewFx(new_fx);


		PM->refreshFxNames(PM->performanceEditPlace);
		PM->showPerformaceValue(new_fx);


		return 1;
	}


	for(uint8_t i = 0; i<8; i++)
	{
		if((PM->trackPatternChange[i] == 1 || PM->trackPatternChange[i] == 2) && tactButtons.isButtonPressed(i))
		{
			PM->trackPatternChange[i] = 2;

			if(mtProject.values.perfTracksPatterns[i] < 255)
			{
				mtProject.values.perfTracksPatterns[i]++;			//zmiana o 1
			}
			//else PM->trackPatternChange[i] = 1;

			PM->refreshTrackPattern = 1;
		}
	}


	return 1;
}


static  uint8_t functDown()
{
	if(PM->performanceEditState)
	{
		int8_t new_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace]-1;

		if(new_fx < 0) new_fx = performanceFxesCount-1;
		while((fxAlredyTaken(new_fx) && new_fx != 0))
		{
			new_fx--;
			if(new_fx < 0) new_fx = performanceFxesCount-1;

		}

		PM->setPlaceNewFx(new_fx);

		PM->refreshFxNames(PM->performanceEditPlace);
		PM->showPerformaceValue(new_fx);

		return 1;
	}


	for(uint8_t i = 0; i<8; i++)
	{
		if((PM->trackPatternChange[i] == 1 || PM->trackPatternChange[i] == 2) && tactButtons.isButtonPressed(i))
		{
			PM->trackPatternChange[i] = 2;

			if(mtProject.values.perfTracksPatterns[i] > 1)
			{
				mtProject.values.perfTracksPatterns[i]--;			//zmiana o 1
			}
			//else PM->trackPatternChange[i] = 1;

			PM->refreshTrackPattern = 1;
		}
	}


	return 1;
}



//=========================================================================================================
static  uint8_t functPlayAction()
{

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

		//PTE->lastPlayedPattern = 0;
	}
	else
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
	if(sequencer.isPerformanceMode())
	{
		fileManager.loadPattern(mtProject.values.actualPattern);
		sequencer.switchRamPatternsNow();
		sequencer.exitPerformanceMode();
	}
	PM->eventFunct(eventSwitchModule,PM,&button,0);

	return 1;
}


static  uint8_t functSwitchMode(uint8_t button)
{
/*
	if(PM->mode == mtPerformanceMaster)
	{
		PM->mode = mtPerformanceFxes;
		PM->showPerformanceFxes();
		PM->setPerformanceFxes();
	}
	else
	{
		PM->mode = mtPerformanceMaster;
		PM->showPerformanceMaster();
		PM->setPerformanceMaster();
	}
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
			if(tactButtons.isButtonPressed(interfaceButtonShift))
			{
				if(mtProject.values.trackMute[button] == 0) mtProject.values.trackMute[button] = 1;
				else mtProject.values.trackMute[button] = 0;

				engine.muteTrack(button, mtProject.values.trackMute[button]);

				PM->refreshTrackState = 1;
			}
			else
			{
				PM->trackPatternChange[button] = 1;
			}
		}
		else if(state == buttonRelease)
		{
			if(!tactButtons.isButtonPressed(interfaceButtonShift))
			{
				if(PM->wasPatternOntrackChenged(button))
				{
					// button <= numer tracka
					// mtProject.values.perfTracksPatterns[button] <= pattern tracka
					sequencer.enterPerformanceMode();

					fileManager.loadTrack(
							mtProject.values.perfTracksPatterns[button],
							button);
				}
				else
				{
					PM->toggleTrackPerformanceState(button);

					PM->refreshTrackState = 1;
				}
			}

			PM->trackPatternChange[button] = 0;
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
		padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);

		PM->performancePadsState[pad%12] = pad/12 + 1;
		mtProject.values.perfSelectedValues[pad%12] =  pad/12+1;

		uint8_t fx = PM->getFxFromPlace(pad%12);

		PM->fxTempValues[fx] = PM->fxValues[fx][pad/12];
		//PM->activeFxValues[fx] = pad/12;
		PM->refreshActiveValueForFx(fx);

		if(pad >= 12)
		{
			for(uint8_t track = 0; track < 8; track++)
			{
				if(PM->tracksPerformanceState[track] == 1)
				{
					PM->refreshPerformanceValuesForTrack(track, mtProject.values.perfFxPlaces[pad%12]);
				}
			}
		}
		else
		{
			for(uint8_t track = 0; track < 8; track++)
			{
				if(PM->tracksPerformanceState[track] == 1)
				{
					PM->clearPerformanceValues(track, mtProject.values.perfFxPlaces[pad%12]);
				}
			}
		}
	}
	else if(state == 0)
	{

		uint8_t fx = PM->getFxFromPlace(pad%12);

		if(PM->performancePadsState[pad%12] == pad/12+1)
		{ // jesli ten sam slot konkretnego fx co wcisnieto ostatnio
			PM->fxTempValues[fx] = PM->fxValues[fx][pad/12];

			for(uint8_t track = 0; track < 8; track++)
			{
				if(PM->tracksPerformanceState[track] == 1)
				{
					PM->refreshPerformanceValuesForTrack(track, fx);
				}
			}

			PM->performancePadsState[pad%12] = 0;
		}

		PM->showPerformaceValue(fx);

		padsBacklight.setFrontLayer(0,0, pad);


	}

	return 1;
}


