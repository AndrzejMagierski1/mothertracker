

#include "keyScanner.h"
#include "mtPadBoard.h"
#include "mtPadsBacklight.h"
#include "core/interfacePopups.h"
#include "fileManager.h"
#include "mtConfig.h"
#include "mtSequencer.h"


#include "performanceMode.h"

#include "performanceEngine.h"


cPerformanceMode performanceMode;
static cPerformanceMode* PM = &performanceMode;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static  uint8_t functCopyPaste();

static uint8_t functShift(uint8_t value);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);


static uint8_t functActionButton(uint8_t button, uint8_t state);


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);




static elapsedMillis refreshTime;

void cPerformanceMode::update()
{
	if(refreshTime < 100) return;
	refreshTime = 0;

	if(refreshBlinkingTrack == 1)
	{
		blinkTrackUntilSwitch();
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

	eepromUpdate(false);
}

void cPerformanceMode::start(uint32_t options)
{
	// ustawienie trackow
	if(!dontClearPerformanceMode)
	{
		for (uint8_t q = 0; q < 8; q++)
		{
			mtProject.values.perfTracksPatterns[q] = mtProject.values.actualPattern;
		}
	}

	// zaladowanie danych do performencu
	if(mtConfig.general.performanceSource < performanceDataSourceCount)
	{
		performance.readPerfData(mtConfig.general.performanceSource);
	}

	// czyszczenie afektowania traków
	for(uint8_t track = 0; track < 8; track++)
	{
		if(!dontClearPerformanceMode) performance.setTrackState(track, 0); // ustaw na 0

		// to jest ladowanie fxow na wejscie ale czy to potrzebne?
//		if(performance.getTrackState(track) == 1) // a teraz sprawdz czy moze jednak nie jest 1 :>
//		{
//			for(uint8_t slot = 0; slot < 12; slot++)
//			{
//				performance.loadFxValuesOnTrack(slot, track);
//			}
//		}
	}

	// prezepisanie aktualnie zaznaczonych wartosci do tablicy wartosci chwilowych
	performance.loadSelectedValuesToTempValues();


	if(dontClearPerformanceMode)
	{
		refreshTrackPattern = 1;
		refreshTrackState = 1;
	}


	moduleRefresh = 1;
	performanceEditState = 0;
	refreshBlinkingTrack = 0;
	dontClearPerformanceMode = 0;

	// ustawienie funkcji
	//FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchMode);
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

	PM->showPerformanceFxes();
	PM->setPerformanceFxes();

}

void cPerformanceMode::stop()
{
	if(!dontClearPerformanceMode)
	{
		performance.turnOff();

		mtPadBoard.releaseAllInstrument();
	}

	eepromUpdate(true);

	moduleRefresh = 0;
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

	FM->setButtonObj(interfaceButtonCopy, buttonPress, functCopyPaste);


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

	performance.setEditState(performanceEditState);

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
	newFileManager.setProjectStructChanged();
//	mtProject.values.projectNotSavedFlag = 1;
//	fileManager.projectChangeFlag = 1;
}


void cPerformanceMode::clearPerformanceValues(uint8_t track, uint8_t fx)
{



}


//=================================================================================
//
//=================================================================================

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
		performance.changeSlotFxValue(place, value);
		if(performance.slotChanged(place)) PM->showPerformaceValue(place);
	}

	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	if(PM->performanceEditState && tactButtons.isButtonPressed(interfaceButtonShift) && PM->performanceEditPlace > 0)
	{
		performance.swapSlotFx(PM->performanceEditPlace, PM->performanceEditPlace-1);
		PM->performanceEditPlace--;

		PM->showEditFrame(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace+1);
		PM->showPerformaceValue(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace+1);
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
		performance.swapSlotFx(PM->performanceEditPlace, PM->performanceEditPlace+1);
		PM->performanceEditPlace++;

		PM->showEditFrame(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace);
		PM->showFxNames(PM->performanceEditPlace-1);
		PM->showPerformaceValue(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace-1);
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

//		uint8_t new_fx = mtProject.values.perfFxPlaces[PM->performanceEditPlace]+1;
//		if(new_fx > performance.getFxCount()-1) new_fx = 0;
//
//		while(PM->fxAlredyTaken(new_fx) &&  new_fx != 0)
//		{
//			new_fx++;
//			if(new_fx >= performance.getFxCount()) new_fx = 0;
//
//		}

		performance.changeSlotFx(PM->performanceEditPlace, 1);


		PM->showFxNames(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace);
		PM->lightUpPadBoard();

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
		performance.changeSlotFx(PM->performanceEditPlace, -1);

		PM->showFxNames(PM->performanceEditPlace);
		PM->showPerformaceValue(PM->performanceEditPlace);
		PM->lightUpPadBoard();

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

static  uint8_t functCopyPaste()
{

	if(PM->performanceEditState)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			performance.pasteSlotFx(PM->performanceEditPlace);
		}
		else
		{
			performance.copySlotFx(PM->performanceEditPlace);
		}
	}

	PM->showFxNames(PM->performanceEditPlace);
	PM->showPerformaceValue(PM->performanceEditPlace);
	PM->lightUpPadBoard();

	return 1;
}


static uint8_t functSwitchModule(uint8_t button)
{
	if(button != interfaceButtonMaster && button != interfaceButtonSong && sequencer.isPerformanceMode())
	{
		//PM->dontClearPerformanceMode = 0;
	}
	else if(button == interfaceButtonMaster || button == interfaceButtonSong)
	{
		PM->dontClearPerformanceMode = 1;
	}

	PM->eventFunct(eventSwitchModule,PM,&button,0);

	return 1;
}




//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################



static uint8_t functActionButton(uint8_t button, uint8_t state)
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
//				sequencer.enterPerformanceMode();
				sequencer.setTrackToLoadOnSwitch(
						button,
						mtProject.values.perfTracksPatterns[button]);

//					fileManager.loadTrack(
//							mtProject.values.perfTracksPatterns[button],
//							button);
				PM->refreshBlinkingTrack = 1;
			}
			else
			{
				performance.toggleTrackState(button);
				//PM->toggleTrackPerformanceState(button);

				PM->refreshTrackState = 1;
			}
		}

		PM->trackPatternChange[button] = 0;
	}




	return 1;
}

void cPerformanceMode::blinkTrackUntilSwitch()
{
	blinkInterval++;
	if(blinkInterval >= 4)
	{
		blinkState = !blinkState;
		blinkInterval = 0;
	}

	uint8_t continueBlink = 0;

	for(uint8_t track = 0; track < 8; track++)
	{
		if(sequencer.isPerformanceTrackChange(track))
		{
			colorTracksLabel(track, blinkState);
			continueBlink = 1;
		}
		else
		{
			colorTracksLabel(track, 0);
		}
	}

	if(!continueBlink)
	{
//		for(uint8_t track = 0; track < 8; track++)
//		{
//			colorTracksLabel(track, 0);
//		}
		refreshTrackState = 1;

		refreshTrackPattern = 1;

		refreshBlinkingTrack = 0;
	}

	display.synchronizeRefresh();
}



//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################


void cPerformanceMode::lightUpPadBoard()
{
	//padsBacklight.clearAllPads(0, 1, 1);
	uint8_t slot_state[4];

	for(uint8_t slot = 0; slot < 12; slot++)
	{
		slot_state[0] = 0;
		slot_state[1] = 0;
		slot_state[2] = 0;
		slot_state[3] = 0;

		int8_t row = performance.getSelectedRow(slot);

		if(row >= 0 && row < 4) slot_state[row] = 1; // ponizej 0 i powyzej 3 nie zaswieci zadnego pada

		for(uint8_t r = 0; r < 4; r++)
		{
			if(slot_state[r] == 0) 	padsBacklight.setBackLayer(0, 0, slot+(12*r));
			else 					padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, slot+(12*r));
		}
	}

}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state > 1) return 1;

	uint8_t slot = pad%12;

	if(state == buttonPress)
	{
		padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);

		performance.setSelectedRow(slot, pad/12);
		performance.setSlotState(slot, 1);

		performance.loadFxValuesOnAffectedTracks(slot);
	}
	else if(state == buttonRelease)
	{
		// jesli puszczany ostatnio wcisniety pad w danym slocie
		// to przeladuj wartosc
		performance.loadFxValuesOnAffectedTracks(slot);

		performance.setSlotState(slot, 0);

		padsBacklight.setFrontLayer(0,0, pad);
	}

	// odwiez zmienione sloty
	for(uint8_t place = 0; place < 12; place++)
	{
		if(performance.slotChanged(place))
		{
			PM->showPerformaceValue(place);
		}
	}

	PM->lightUpPadBoard();

	return 1;
}


