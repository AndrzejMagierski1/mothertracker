

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
		showTracksState();
	}
	if(refreshTrackPattern == 1)
	{
		refreshTrackPattern = 0;
		showTracksPatterns();
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
		tracksPerformanceState[track] = 0; // ustaw na 0

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

	lightUpPadBoard();
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

void cPerformanceMode::setProjectSaveFlags()
{
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
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
			refreshPerformanceValuesForTrack(track,  i);
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
		sequencer.alignToGlobalPos(track);
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

void cPerformanceMode::refreshPerformanceValuesForTrack(uint8_t track, uint8_t place)
{
	if(place > 11) return;
	uint8_t fx = mtProject.values.perfFxPlaces[place];
	if(fx == mtPerfFxNone) return;

	if(FX_VALUE(place) == 0)
	{
		// czyszczenie zamiast zmiany kiedy value == 0
		clearPerformanceValues(track, fx);
		return;
	}

	switch(fx)
	{
	case mtPerfFxVolume:
	{
		instrumentPlayer[track].changeVolumePerformanceMode(FX_VALUE(place));
		break;
	}
	case mtPerfPanning:
	{
		instrumentPlayer[track].changePanningPerformanceMode(FX_VALUE(place));
		break;
	}
	case mtPerfLowPass:
	{
		//if(FX_VALUE(place) != 0)
		//{
			instrumentPlayer[track].changeFilterTypePerformanceMode(1);
			instrumentPlayer[track].changeCutoffPerformanceMode(FX_VALUE(place));
		//}
		break;
	}
	case mtPerfHighPass:
	{
		//if(FX_VALUE(place) != 0)
		//{
			instrumentPlayer[track].changeFilterTypePerformanceMode(2);
			instrumentPlayer[track].changeCutoffPerformanceMode(FX_VALUE(place));
		//}
		break;
	}
	case mtPerfBandPass:
	{
		//if(FX_VALUE(place) != 0)
		//{
			instrumentPlayer[track].changeFilterTypePerformanceMode(3);
			instrumentPlayer[track].changeCutoffPerformanceMode(FX_VALUE(place));
		//}
		break;
	}
	case mtPerfReverbSend:
	{
		instrumentPlayer[track].changeReverbSendPerformanceMode(FX_VALUE(place));
		break;
	}
	case mtPerfSampleStart:
	{
		instrumentPlayer[track].changeStartPointPerformanceMode(map(FX_VALUE(place),-100,100,-MAX_16BIT,MAX_16BIT));
		break;
	}
	case mtPerfSampleEnd:
	{
		instrumentPlayer[track].changeEndPointPerformanceMode(map(FX_VALUE(place),-100,100,-MAX_16BIT,MAX_16BIT));
		break;
	}
	case mtPerfWavetablePos:
	{
		instrumentPlayer[track].changeWavetableWindowPerformanceMode(map(FX_VALUE(place),-100,100,-MAX_WAVETABLE_WINDOW,MAX_WAVETABLE_WINDOW));
		break;
	}
	case mtPerfTune:
	{
		instrumentPlayer[track].changeTunePerformanceMode(FX_VALUE(place));
		break;
	}
	case mtPerfSamplePlayback:
	{
		instrumentPlayer[track].changeSamplePlaybackPerformanceMode(FX_VALUE(place));
		break;
	}
	case mtPerfStepStutter:
	{
		sequencer.setPerformanceStutter(track, FX_VALUE(place));
		break;
	}
	case mtPerfPatternPlayMode:
	{
		sequencer.setPerformancePlayMode(track, FX_VALUE(place));
		break;
	}
	case mtPerfPatternLength:
	{
		sequencer.setPerformancePatternLengthFromFxVal(FX_VALUE(place));
		break;
	}



	default: break;
	}
}

//=================================================================================
//
//=================================================================================
void cPerformanceMode::setPlaceNewFx(uint8_t place, uint8_t newFx)
{
	uint8_t old_fx = mtProject.values.perfFxPlaces[place];

	for(uint8_t track = 0; track < 8; track++)
	{
		clearPerformanceValues(track, old_fx);
	}

	mtProject.values.perfFxValues[place][0] = 0;
	mtProject.values.perfFxValues[place][1] = 0;
	mtProject.values.perfFxValues[place][2] = 0;
	mtProject.values.perfFxValues[place][3] = 0;

	mtProject.values.perfFxPlaces[place] = newFx;

	setProjectSaveFlags();
}
//=================================================================================
//
//=================================================================================
uint8_t cPerformanceMode::fxAlredyTaken(uint8_t fx)
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

				PM->setProjectSaveFlags();
			}

			PM->refreshTrackPattern = 1;
			patternChange = 1;
		}
	}

	if(patternChange) return 1;

	//--------------------------------------------------------------------------------------------------
	for(uint8_t place = 0; place < 12; place++)
	{
		if(PM->performancePadsState[place] > 1) // nie zmienia jesli 1 slot
		{
			int16_t mod_value = value;
			uint8_t fx = mtProject.values.perfFxPlaces[place];

			switch(fx)
			{
			case mtPerfFxVolume:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeVolumePerformanceMode(FX_VALUE(place));
				}
				break;
			}
			case mtPerfPanning:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changePanningPerformanceMode(FX_VALUE(place));
				}
				break;
			}
			case mtPerfLowPass:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;
				////PM->fxValues[mtPerfHighPass][] = 0;
				//PM->fxValues[mtPerfBandPass][] = 0;
				//PM->showPerformaceValue(mtPerfBandPass);
				//PM->showPerformaceValue(mtPerfHighPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(1);
						instrumentPlayer[j].changeCutoffPerformanceMode(FX_VALUE(place));
					}
				}
				break;
			}
			case mtPerfHighPass:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;
				//PM->fxValues[mtPerfLowPass] = 0;
				//PM->fxValues[mtPerfBandPass] = 0;
				//PM->showPerformaceValue(mtPerfBandPass);
				//PM->showPerformaceValue(mtPerfLowPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(2);
						instrumentPlayer[j].changeCutoffPerformanceMode(FX_VALUE(place));
					}
				}
				break;
			}
			case mtPerfBandPass:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;
				//PM->fxValues[mtPerfLowPass][] = 0;
				//PM->fxValues[mtPerfHighPass][] = 0;
				//PM->showPerformaceValue(mtPerfLowPass);
				//PM->showPerformaceValue(mtPerfHighPass);

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeFilterTypePerformanceMode(3);
						instrumentPlayer[j].changeCutoffPerformanceMode(FX_VALUE(place));
					}
				}
				break;
			}
			case mtPerfReverbSend:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeReverbSendPerformanceMode(FX_VALUE(place));
				}
				break;
			}
			case mtPerfSampleStart:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeStartPointPerformanceMode(map(FX_VALUE(place),-100,100,-MAX_16BIT,MAX_16BIT));
				}
				break;

				break;
			}
			case mtPerfSampleEnd:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeEndPointPerformanceMode(map(FX_VALUE(place),-100,100,-MAX_16BIT,MAX_16BIT));
				}
				break;

				break;
			}
			case mtPerfWavetablePos:
			{
				if(FX_VALUE(place) + mod_value > 100) FX_VALUE(place) = 100;
				else if(FX_VALUE(place) + mod_value < -100) FX_VALUE(place) = -100;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeWavetableWindowPerformanceMode(map(FX_VALUE(place),-100,100,-MAX_WAVETABLE_WINDOW,MAX_WAVETABLE_WINDOW));
				}
				break;

				break;
			}
			case mtPerfTune:
			{
				if(FX_VALUE(place) + mod_value > 48) FX_VALUE(place) = 48;
				else if(FX_VALUE(place) + mod_value < -48) FX_VALUE(place) = -48;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j]) instrumentPlayer[j].changeTunePerformanceMode(FX_VALUE(place));
				}

				break;
			}
			case mtPerfSamplePlayback:
			{
				if(FX_VALUE(place) + mod_value > 1) FX_VALUE(place) = 1;
				else if(FX_VALUE(place) + mod_value < 0) FX_VALUE(place) = 0;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						instrumentPlayer[j].changeSamplePlaybackPerformanceMode(FX_VALUE(place));
					}
				}

				break;
			}
			case mtPerfStepStutter:
			{
				if(FX_VALUE(place) + mod_value > 8) FX_VALUE(place) = 8;
				else if(FX_VALUE(place) + mod_value < 0) FX_VALUE(place) = 0;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if (PM->tracksPerformanceState[j])
					{
						sequencer.setPerformanceStutter(j, FX_VALUE(place));
					}
				}

				break;
			}
			case mtPerfPatternPlayMode:
			{
				if(FX_VALUE(place) + mod_value > 2) FX_VALUE(place) = 2;
				else if(FX_VALUE(place) + mod_value < 0) FX_VALUE(place) = 0;
				else FX_VALUE(place) += mod_value;

				for(uint8_t j = 0; j < 8; j++)
				{
					if(PM->tracksPerformanceState[j])
					{
						sequencer.setPerformancePlayMode(j, FX_VALUE(place));
					}
				}

				break;
			}
			case mtPerfPatternLength:
				{

				if (FX_VALUE(place) + mod_value > 8) FX_VALUE(place) = 8;
				else if (FX_VALUE(place) + mod_value < 0) FX_VALUE(place) = 0;
				else FX_VALUE(place) += mod_value;

				sequencer.setPerformancePatternLengthFromFxVal(FX_VALUE(place));

				break;
			}
			default: break;
			}

			PM->showPerformaceValue(place);


			if(PM->performanceEditState) // zapis jesli edit mode
			{
				mtProject.values.perfFxValues[place][mtProject.values.perfSelectedValues[place]] = PM->placesTempValues[place];

				PM->setProjectSaveFlags();
			}
		}
	}

	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	if(PM->performanceEditState && tactButtons.isButtonPressed(interfaceButtonShift) && PM->performanceEditPlace > 0)
	{
		uint8_t target_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];
		PM->performanceEditPlace--;
		uint8_t dest_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];

		mtProject.values.perfFxPlaces[PM->performanceEditPlace] = target_fx;
		mtProject.values.perfFxPlaces[PM->performanceEditPlace+1] = dest_fx;

		PM->showEditFrame(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace+1);
		PM->showPerformaceValue(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace+1);

		PM->setProjectSaveFlags();
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
	if(PM->performanceEditState && tactButtons.isButtonPressed(interfaceButtonShift) && PM->performanceEditPlace < 11)
	{
		uint8_t target_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];
		PM->performanceEditPlace++;
		uint8_t dest_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace];

		mtProject.values.perfFxPlaces[PM->performanceEditPlace] = target_fx;
		mtProject.values.perfFxPlaces[PM->performanceEditPlace-1] = dest_fx;

		PM->showEditFrame(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace-1);
		PM->showPerformaceValue(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace-1);

		PM->setProjectSaveFlags();
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
	if(PM->performanceEditState)
	{
		uint8_t new_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace]+1;
		if(new_fx > performanceFxesCount-1) new_fx = 0;

		while(PM->fxAlredyTaken(new_fx) &&  new_fx != 0)
		{
			new_fx++;
			if(new_fx >= performanceFxesCount) new_fx = 0;

		}

		PM->setPlaceNewFx(PM->performanceEditPlace, new_fx);

		PM->showFxNames(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace);

		PM->setProjectSaveFlags();

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

				PM->setProjectSaveFlags();
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
		while((PM->fxAlredyTaken(new_fx) && new_fx != 0))
		{
			new_fx--;
			if(new_fx < 0) new_fx = performanceFxesCount-1;

		}

		PM->setPlaceNewFx(PM->performanceEditPlace, new_fx);

		PM->showFxNames(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace);

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

				PM->setProjectSaveFlags();
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
			 PM->setProjectSaveFlags();
		}
		else if(state == buttonRelease)
		{
			if(tactButtons.isButtonPressed(interfaceButtonShift))
			{
				 if(mtProject.values.trackMute[button] == 0) mtProject.values.trackMute[button] = 1;
				 else mtProject.values.trackMute[button] = 0;

				 engine.muteTrack(button, mtProject.values.trackMute[button]);

				 PM->refreshMaster = 1;
				 PM->setProjectSaveFlags();
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
				PM->setProjectSaveFlags();
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
					sequencer.setTrackToLoadOnSwitch(
							button,
							mtProject.values.perfTracksPatterns[button]);

//					fileManager.loadTrack(
//							mtProject.values.perfTracksPatterns[button],
//							button);
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
	//padsBacklight.clearAllPads(0, 1, 1);

	for(uint8_t pad = 0; pad < 48; pad++)
	{
		uint8_t place = pad%12;
		if(mtProject.values.perfSelectedValues[place] == pad/12 && mtProject.values.perfFxPlaces[place])
			padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, pad);
		else if(!(pad%2))
			padsBacklight.setBackLayer(1, mtConfig.values.padsLightBackWeek, pad);
		else
			 padsBacklight.setBackLayer(0, 0, pad);
	}

}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	uint8_t place = pad%12;

	if(state == 1)
	{
		padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);

		PM->performancePadsState[place] = pad/12+1;
		mtProject.values.perfSelectedValues[place] = pad/12;


		PM->placesTempValues[place] = mtProject.values.perfFxValues[place][pad/12];

		if(pad >= 12)
		{
			for(uint8_t track = 0; track < 8; track++)
			{
				if(PM->tracksPerformanceState[track] == 1)
				{
					PM->refreshPerformanceValuesForTrack(track, place);
				}
			}
		}
		else
		{
			for(uint8_t track = 0; track < 8; track++)
			{
				if(PM->tracksPerformanceState[track] == 1)
				{
					PM->clearPerformanceValues(track, mtProject.values.perfFxPlaces[place]);
				}
			}
		}
	}
	else if(state == 0)
	{
		if(PM->performancePadsState[place] == pad/12+1)
		{ // jesli ten sam slot konkretnego fx co wcisnieto ostatnio
			PM->placesTempValues[place] = mtProject.values.perfFxValues[place][pad/12];

			for(uint8_t track = 0; track < 8; track++)
			{
				if(PM->tracksPerformanceState[track] == 1)
				{
					PM->refreshPerformanceValuesForTrack(track, place);
				}
			}

			PM->performancePadsState[place] = 0;
		}

		padsBacklight.setFrontLayer(0,0, pad);
	}

	PM->showPerformaceValue(place);
	PM->lightUpPadBoard();

	return 1;
}


