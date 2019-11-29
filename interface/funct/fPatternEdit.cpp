
#include <patternEditor.h>
#include "mtStructs.h"

#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "mtPadBoard.h"
#include "mtPadsBacklight.h"

#include "interfacePopups.h"

#include "keyScanner.h"


cPatternEditor patternEditor;
static  cPatternEditor* PTE = &patternEditor;

extern strMtProject mtProject;
extern uint32_t patternTrackerSelectionColor;


// pod ekranem
static  uint8_t functChangePattern(uint8_t state);
static  uint8_t functChangePatternLength(uint8_t state);
static  uint8_t functChangePatternEditStep(uint8_t state);
static  uint8_t functFill();
static  uint8_t functPreview();
static  uint8_t functTranspose();
static  uint8_t functUndo();
uint8_t functInvert();


// step buttons
static  uint8_t functNote(uint8_t state);
static  uint8_t functInstrument(uint8_t state);
static  uint8_t functFx1(uint8_t state);
static  uint8_t functFx2(uint8_t state);


// modules
static  uint8_t functPattern(uint8_t state);

static  uint8_t functSwitchModule(uint8_t button);


// fill
static  uint8_t functFillCancel();
static  uint8_t functFillApply();
static  uint8_t functFillChangeType();
static  uint8_t functFillChangeParam1();
static  uint8_t functFillChangeParam2();
static  uint8_t functFillChangeParam3();
static  uint8_t functFillChangeParam4();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functPlayAction();
static  uint8_t functRecAction();

static  uint8_t functShift(uint8_t state);

static  uint8_t functInsertHome(uint8_t state);
static  uint8_t functCopyPaste(uint8_t state);
static  uint8_t functDeleteBackspace(uint8_t state);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


//muty
static uint8_t functActionButton(uint8_t button, uint8_t state);

//--------------------------------------------
static uint8_t getSelectedElement();


char getHexFromInt(int16_t val, uint8_t index);




elapsedMillis patternRefreshTimer;

void cPatternEditor::update()
{
	if(patternRefreshTimer < 20) return;

	patternRefreshTimer = 0;

	if(sequencer.isStop())
	{
		return;
	}

	readPatternState();

	if(lastPlayedPattern != mtProject.values.actualPattern)
	{
		lastPlayedPattern = mtProject.values.actualPattern;

		refreshPatternParams();
	}

	if(trackerPattern.playheadPosition == lastPatternPosition || (!isPleyheadOnScreen() && editMode))  return;


	if(mtPopups.getStepPopupState() == stepPopupNone) // zatrzymaj odswiezanie kiedy wyswietlany popup
	{
		refreshPattern();
		lastPatternPosition = trackerPattern.playheadPosition;
	}

}



void cPatternEditor::start(uint32_t options)
{
	moduleRefresh = 1;
	patternButtonReleaseActive = 0;

	mtProject.values.padBoardScale = 0;
	mtProject.values.padBoardNoteOffset = 12;
	mtProject.values.padBoardRootNote = 36;


	readPatternState();
	refreshPattern();



	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	refreshEditState();

	focusOnPattern();

	PTE->eventFunct(eventActivateTestingProcedure,PTE,0,0);
}

void cPatternEditor::stop()
{
//	setPatternChangeFlag();

	if(fillState) fillState = 0;

	padsBacklight.clearAllPads(1, 1, 1);

	moduleRefresh = 0;
}

void cPatternEditor::setDefaultScreenFunct()
{
	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonCopy, functCopyPaste);
	FM->setButtonObj(interfaceButtonInsert, functInsertHome);
	FM->setButtonObj(interfaceButtonDelete, functDeleteBackspace);


	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);
	FM->setButtonObj(interfaceButtonShift, functShift);


	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);



	FM->setButtonObj(interfaceButtonNote,functNote);
	FM->setButtonObj(interfaceButtonInstr, functInstrument);
	//FM->setButtonObj(interfaceButtonVol, functVolume);
	FM->setButtonObj(interfaceButtonVol, functFx1);
	FM->setButtonObj(interfaceButtonFx, functFx2);

	FM->setButtonObj(interfaceButtonPattern, functPattern);


	FM->setButtonObj(interfaceButton0, functChangePattern);
	FM->setButtonObj(interfaceButton1, functChangePatternLength);
	FM->setButtonObj(interfaceButton2, functChangePatternEditStep);
//	FM->setButtonObj(interfaceButton2, functChangePatternEditStep);


	//FM->setButtonObj(interfaceButton4, buttonPress, functFill);
	//FM->setButtonObj(interfaceButton5, buttonPress, functRandom);
	//FM->setButtonObj(interfaceButton6, buttonPress, functInvert);
	//FM->setButtonObj(interfaceButton7, buttonPress, functUndo);




	FM->setPotObj(interfacePot0, functEncoder, nullptr);


	FM->setPadsGlobal(functPads);

}

//==============================================================================================================
// przeniesienie danych z sekewncji  do struktury wyswietlania
void cPatternEditor::refreshPattern()
{
	seq = sequencer.getPatternToUI();
	trackerPattern.patternLength = seq->track[0].length+1;

	trackerPattern.popupMode = 0;

	if(editMode == 0)
	{
		trackerPattern.selectState = 0;

		if(!sequencer.isStop())
		{
			trackerPattern.actualStep = trackerPattern.playheadPosition;

			if(sequencer.isRec())
			{
				playheadRecMode();
			}
			else
			{
				playheadNormalMode();
			}
		}
	}


	if(trackerPattern.actualStep > seq->track[0].length) trackerPattern.actualStep = seq->track[0].length;


	trackerPattern.selectedParam = editParam;

	uint8_t steps_down = trackerPattern.patternLength - trackerPattern.actualStep;
	steps_down = (steps_down < 8) ? steps_down : 8;

	uint8_t steps_up = (trackerPattern.actualStep < 7) ? trackerPattern.actualStep : 7;

	int16_t patternPosition = trackerPattern.actualStep;

	//mtPopups.show(0, &mtNotes[seq->track[0].step[patternPosition].note][0]);

	for(uint8_t i = 0; i < 8; i++) //track
	{
		// sprawdzanie mutow
		trackerPattern.inactive[i] = mtProject.values.trackMute[i];

		for(uint8_t j = 0; j < 15; j++) // step
		{
			strTrackerPattern::strTracks::strRow *row = &trackerPattern.track[i].row[j];

			if(j-6 > steps_down || j<7-steps_up)
			{
				row->note[0] = 0;
				row->note[1] = 0;
				row->note[2] = 0;
				row->note[3] = 0;
				row->note[4] = 0;
				row->instr[0] = 0;
				row->instr[1] = 0;
				row->instr[2] = 0;
				row->instr[3] = 0;
				row->fx[0][0] = 0;
				row->fx[0][1] = 0;
				row->fx[0][2] = 0;
				row->fx[0][3] = 0;
				row->fx[0][4] = 0;
				row->fx[1][0] = 0;
				row->fx[1][1] = 0;
				row->fx[1][2] = 0;
				row->fx[1][3] = 0;
				row->fx[1][4] = 0;
				continue;
			}

//			if(seq->track[i].step[patternPosition-7+j].isOn)
//			{
			bool showInstrument = 0;
			// jest nuta
			if(seq->track[i].step[patternPosition-7+j].note >= 0)
			{
				trackerPattern.track[i].row[j].note[0] = mtNotes[seq->track[i].step[patternPosition-7+j].note][0];
				trackerPattern.track[i].row[j].note[1] = mtNotes[seq->track[i].step[patternPosition-7+j].note][1];
				trackerPattern.track[i].row[j].note[2] = mtNotes[seq->track[i].step[patternPosition-7+j].note][2];
				trackerPattern.track[i].row[j].note[3] = mtNotes[seq->track[i].step[patternPosition-7+j].note][3];
				trackerPattern.track[i].row[j].note[4] = mtNotes[seq->track[i].step[patternPosition-7+j].note][4];
				showInstrument = 1;
			}
			// pusta nuta
			else if (seq->track[i].step[patternPosition - 7 + j].note == Sequencer::STEP_NOTE_EMPTY)
			{
				trackerPattern.track[i].row[j].note[0] = '-';
				trackerPattern.track[i].row[j].note[1] = '-';
				trackerPattern.track[i].row[j].note[2] = '-';
				trackerPattern.track[i].row[j].note[3] = 0;
			}
			// note off
			else if (seq->track[i].step[patternPosition - 7 + j].note == Sequencer::STEP_NOTE_OFF)
			{
				trackerPattern.track[i].row[j].note[0] = 'O';
				trackerPattern.track[i].row[j].note[1] = 'F';
				trackerPattern.track[i].row[j].note[2] = 'F';
				trackerPattern.track[i].row[j].note[3] = 0;
				showInstrument = 0;
			}
			else if (seq->track[i].step[patternPosition - 7 + j].note == Sequencer::STEP_NOTE_CUT)
			{
				trackerPattern.track[i].row[j].note[0] = 'C';
				trackerPattern.track[i].row[j].note[1] = 'U';
				trackerPattern.track[i].row[j].note[2] = 'T';
				trackerPattern.track[i].row[j].note[3] = 0;
				showInstrument = 0;
			}
			else if (seq->track[i].step[patternPosition - 7 + j].note == Sequencer::STEP_NOTE_FADE)
			{
				trackerPattern.track[i].row[j].note[0] = 'F';
				trackerPattern.track[i].row[j].note[1] = 'A';
				trackerPattern.track[i].row[j].note[2] = 'D';
				trackerPattern.track[i].row[j].note[3] = 0;
				showInstrument = 0;
			}

			//--------------------------------------------------------------------------------------------
			if (showInstrument)
			{
				if(seq->track[i].step[patternPosition - 7 + j].instrument > INSTRUMENTS_MAX)  // midi instr
				{
					trackerPattern.track[i].row[j].instr[3] = 1;

					char inst0 = (seq->track[i].step[patternPosition - 7 + j].instrument + 3) / 10;
					char inst1 = (seq->track[i].step[patternPosition - 7 + j].instrument + 3) % 10;

					trackerPattern.track[i].row[j].instr[0] = inst0 + 48;
					trackerPattern.track[i].row[j].instr[1] = inst1 + 48;
					trackerPattern.track[i].row[j].instr[2] = 0;
				}
				else // normal instr
				{
					trackerPattern.track[i].row[j].instr[3] = 0;

					char inst0 = (seq->track[i].step[patternPosition - 7 + j].instrument + 1) / 10;
					char inst1 = (seq->track[i].step[patternPosition - 7 + j].instrument + 1) % 10;

					trackerPattern.track[i].row[j].instr[0] = inst0 + 48;
					trackerPattern.track[i].row[j].instr[1] = inst1 + 48;
					trackerPattern.track[i].row[j].instr[2] = 0;
				}
			}
			else
			{
				trackerPattern.track[i].row[j].instr[0] = '-';
				trackerPattern.track[i].row[j].instr[1] = '-';
				trackerPattern.track[i].row[j].instr[2] = 0;
				trackerPattern.track[i].row[j].instr[3] = 0;
			}

			//--------------------------------------------------------------------------------------------

			uint8_t type_temp  = interfaceGlobals.fxIdToName(seq->track[i].step[patternPosition-7+j].fx[1].type);
			if(type_temp > 0 && type_temp < FX_MAX)
			{
				trackerPattern.track[i].row[j].fx[0][0] = 0;
				trackerPattern.track[i].row[j].fx[0][1] = 0;
				trackerPattern.track[i].row[j].fx[0][2] = 0;

				strncat(&trackerPattern.track[i].row[j].fx[0][0],
						&interfaceGlobals.fxNames[type_temp][0],
						1);

				int16_t val = sequencer.getFxValueToView(
						1, i, patternPosition - 7 + j);

				if (val >= 0)
				{
					sprintf(&trackerPattern.track[i].row[j].fx[0][1],
							"%.3i",
							val
							);
				}
				else
				{
					sprintf(&trackerPattern.track[i].row[j].fx[0][1],
							"%.2i",
							val
							);
				}

				//trackerPattern.track[i].row[j].fx[0][3] = '0';
			}
			else
			{
				trackerPattern.track[i].row[j].fx[0][0] = '-';
				trackerPattern.track[i].row[j].fx[0][1] = '-';
				trackerPattern.track[i].row[j].fx[0][2] = '-';
				trackerPattern.track[i].row[j].fx[0][3] = '-';
			}
			//--------------------------------------------------------------------------------------------
			type_temp  = interfaceGlobals.fxIdToName(seq->track[i].step[patternPosition-7+j].fx[0].type);
			if(type_temp > 0 && type_temp < FX_MAX)
			{
				trackerPattern.track[i].row[j].fx[1][0] = 0;
				trackerPattern.track[i].row[j].fx[1][1] = 0;
				trackerPattern.track[i].row[j].fx[1][2] = 0;

				strncat(&trackerPattern.track[i].row[j].fx[1][0],
						&interfaceGlobals.fxNames[type_temp][0],
						1);

				sprintf(&trackerPattern.track[i].row[j].fx[1][1],
						"%.3u",
						sequencer.getFxValueToView(0,i,patternPosition - 7 + j));

				//trackerPattern.track[i].row[j].fx[1][3] = '0';
			}
			else
			{
				trackerPattern.track[i].row[j].fx[1][0] = '-';
				trackerPattern.track[i].row[j].fx[1][1] = '-';
				trackerPattern.track[i].row[j].fx[1][2] = '-';
				trackerPattern.track[i].row[j].fx[1][3] = '-';
			}
		}
	}

	display.refreshControl(patternControl);
}

char getHexFromInt(int16_t val, uint8_t index)
{

	int16_t retVal;
	if (index == 0)
	{
		retVal = val % 16;
	}
	else
	{
		retVal = val / (16 * index);
	}

	if (retVal < 10)
	{
		return retVal + 48;
	}
	else if (retVal < 16)
	{
		return retVal + 97 - 10;
	}
	else
	{
		return 0;
	}

}

// odczytywanie dlugosci i pozycji odtwarzania aktualnej sekewncji
void cPatternEditor::readPatternState()
{
	seq = sequencer.getPatternToUI();


	trackerPattern.patternLength = seq->track[0].length+1;
	trackerPattern.playheadPosition = sequencer.ptrPlayer->track[0].actual_pos;


}

uint8_t cPatternEditor::getStepFx()
{
	uint8_t selectedFx = 0;

	uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

	uint8_t fx_type =  interfaceGlobals.fxIdToName(
			sequencer.getPatternToUI()->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[fx_index].type);

	if(fx_type < FX_COUNT) selectedFx = fx_type;

	if(selectedFx == 0) selectedFx = mtProject.values.lastUsedFx;

	return selectedFx;
}

int8_t cPatternEditor::getStepVol()
{
//	int8_t show_vol = sequencer.getPatternToUI()->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity;
	int8_t show_vol = -1;

	if(show_vol < 0) return -1;

	return map(show_vol,0,127,0,47);
}


// focusowanie trackow na kursorze, jesli jest poza ekranem
void cPatternEditor::focusOnActual()
{
	if(patternViewMode > 0)
	{
		trackerPattern.firstVisibleTrack = 0;
		return;
	}

	if(trackerPattern.actualTrack > trackerPattern.firstVisibleTrack+3)
	{
		trackerPattern.firstVisibleTrack = trackerPattern.actualTrack - 3;
	}
	else if(trackerPattern.actualTrack < trackerPattern.firstVisibleTrack)
	{
		trackerPattern.firstVisibleTrack = trackerPattern.actualTrack;
	}
}

void cPatternEditor::moveCursorByStep()
{
	if(mtProject.values.patternEditStep <= 0 ) return;

	int16_t patternLength  = sequencer.getPatternToUI()->track[0].length;

	if(trackerPattern.actualStep + mtProject.values.patternEditStep <= patternLength)
	{
		trackerPattern.actualStep += mtProject.values.patternEditStep;
	}
	else
	{
		trackerPattern.actualStep = mtProject.values.patternEditStep - ((patternLength+1)-trackerPattern.actualStep);
	}
}



void cPatternEditor::cancelPopups()
{
	uint8_t popup_type = mtPopups.getStepPopupState();
	if(popup_type != stepPopupNone)
	{
		fileManager.storePatternUndoRevision();

		if(PTE->editMode == 1 && !insertOnPopupHideDisabled)
		{
			switch (popup_type)
			{
			case stepPopupNote:
	//			if (!isMultiSelection())
	//			{
					//sendSelection();
					//sequencer.setSelectionNote(mtProject.values.lastUsedNote);
	//			}
				break;
			case stepPopupFx:
				if (!isMultiSelection())
				{
					uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

					uint8_t fx_name = interfaceGlobals.fxIdToName(
							sequencer.getPatternToUI()->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[fx_index].type);
					if (fx_name > 0 && fx_name < FX_COUNT)
					{

						sendSelection();
						sequencer.setSelectionFxType(
								fx_index,
								interfaceGlobals.fxIDs[mtProject.values.lastUsedFx]);
					}
				}
				break;
			case stepPopupInstr:
	//			if (!isMultiSelection())
	//			{
					//sendSelection();
					//sequencer.setSelectionInstrument(mtProject.values.lastUsedInstrument);
	//			}
				break;

			default:
				break;
			}
		}

		mtPopups.hideStepPopups();
		setDefaultScreenFunct();
		showDefaultScreen();
		refreshEditState();

		if(fillState)
		{
			if(popup_type == stepPopupInstr)
			{
				 if(editParam == 1)
				 {
					if (PTE->fillPlace == 1) 		PTE->fillData[1].from = mtProject.values.lastUsedInstrument;
					else if (PTE->fillPlace == 2) 	PTE->fillData[1].to = mtProject.values.lastUsedInstrument;
				 }
			}
			functFill();
		}

		refreshPattern();
	}
}




// sprawdza czy w danym mencie playhead sekwencji jest na ktoryms ze stepow wyswietlanych na ekranie
uint8_t cPatternEditor::isPleyheadOnScreen()
{
	if(lastPatternPosition == trackerPattern.patternLength-1) return 1; //
	if(lastPatternPosition == trackerPattern.patternLength) return 1;   //

	if((trackerPattern.playheadPosition > trackerPattern.actualStep-8
		&&  trackerPattern.playheadPosition <= trackerPattern.actualStep+8))
		return 1;

	return 0;
}




void cPatternEditor::changeActualTempo(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->tempo+value < 10) pattern->tempo = 10;
	else if(pattern->tempo+value > 1000) pattern->tempo = 400;
	else  pattern->tempo += value;

//	showTempo();
}

void cPatternEditor::changeActualPattern(int16_t value)
{
	if(sequencer.getSeqState() != sequencer.SEQ_STATE_PLAY_PERFORMANCE)
	{

		fileManager.savePattern(mtProject.values.actualPattern);

	}

	mtProject.values.actualPattern = constrain(
			mtProject.values.actualPattern + value, PATTERN_INDEX_MIN,
			PATTERN_INDEX_MAX);


	// Zapis aktualnego appternu za 10s od ostatniej zmiany
	fileManager.configChangedRefresh = 0;
	fileManager.configIsChangedFlag = 1;


	fileManager.loadPattern(mtProject.values.actualPattern);
	sequencer.switchRamPatternsNow();

	readPatternState();
	refreshPatternParams();
	refreshPattern();

}
void cPatternEditor::setActualPattern(int16_t value)
{

	mtProject.values.actualPattern = constrain(value,
												PATTERN_INDEX_MIN,
												PATTERN_INDEX_MAX);

	fileManager.loadPattern(mtProject.values.actualPattern);
	sequencer.switchRamPatternsNow();

	readPatternState();
	refreshPatternParams();
	refreshPattern();

}

void cPatternEditor::changeActualPatternLength(int16_t value)
{
	fileManager.setPatternChangeFlag(mtProject.values.actualPattern);

	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[0].length+value < 0) pattern->track[0].length = 0;
	else if(pattern->track[0].length+value > Sequencer::MAXSTEP) pattern->track[0].length = Sequencer::MAXSTEP;
	else  pattern->track[0].length += value;

	for(uint8_t i = 1;i < 8; i++)
		pattern->track[i].length = pattern->track[0].length;

	trackerPattern.patternLength = pattern->track[0].length+1;

	if(trackerPattern.actualStep > trackerPattern.patternLength-1) trackerPattern.actualStep = trackerPattern.patternLength-1;


	showLength();

	refreshPattern();
}

void cPatternEditor::setActualPatternLength(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	pattern->track[0].length = constrain(value, 0, Sequencer::MAXSTEP);

	for(uint8_t i = 1;i < 8; i++)
		pattern->track[i].length = pattern->track[0].length;

	trackerPattern.patternLength = pattern->track[0].length+1;

	if(trackerPattern.actualStep > trackerPattern.patternLength-1) trackerPattern.actualStep = trackerPattern.patternLength-1;


	showLength();

	refreshPattern();
}

void cPatternEditor::changeActualPatternEditStep(int16_t value)
{
	fileManager.setPatternChangeFlag(mtProject.values.actualPattern);

	mtProject.values.patternEditStep = constrain(
			mtProject.values.patternEditStep + value,
			0,
			PATTERN_EDIT_STEP_MAX);

	showStep();
}

void cPatternEditor::setActualPatternEditStep(int16_t value)
{

	mtProject.values.patternEditStep = constrain(value,
													0,
													PATTERN_EDIT_STEP_MAX);

	showStep();
}

//======================================================================================================================
static uint8_t functActionButton(uint8_t button, uint8_t state)
{
	if(state == buttonPress)
	{
		 if(mtProject.values.trackMute[button] == 0) mtProject.values.trackMute[button] = 1;
		 else mtProject.values.trackMute[button] = 0;

		 engine.muteTrack(button, mtProject.values.trackMute[button]);
		 PTE->trackerPattern.inactive[button] = mtProject.values.trackMute[button];

		 display.refreshControl(PTE->patternControl);


		PTE->shiftAction = 1;

		PTE->setProjectSaveFlags();
	}


	return 1;
}

/*
void cPatternEditor::changeSelectedFx(int16_t value, uint8_t type)
{
//	Serial.println("change fx");

	uint8_t oldList = selectedFx/12;

	if(selectedFx + value < 0) selectedFx = 0;
	else if(selectedFx + value > FX_MAX) selectedFx = FX_MAX;
	else selectedFx += value;

	uint8_t newList = selectedFx/12;

	mtProject.values.lastUsedFx = selectedFx;

	if(oldList != newList)
	{
		if(oldList < newList)
		{
			fxList[newList].start = 0;
			if(type) fxList[newList].start = selectedFx%12;
			//intrumentsList[oldList].start = 11;
			fxList[oldList].start = -1;
		}
		else
		{
			//intrumentsList[oldList].start = 0;
			fxList[oldList].start = -1;

			fxList[newList].start = 11;
			if(type) fxList[newList].start = selectedFx%12;
		}

		display.setControlData(fxListControl[newList], &fxList[newList]);
		display.setControlData(fxListControl[oldList], &fxList[oldList]);

		showFxList(oldList);
	}

	showFxList(newList);

	showActualInstrument();

	lightUpPadBoard();
}
*/


void cPatternEditor::refreshEditState()
{
	if(editMode)
	{
		if(trackerPattern.selectState == 0) trackerPattern.selectState = 1;

		trackerPattern.selectedParam = editParam;

		focusOnActual();

		focusOnPattern();

		showEditModeLabels();

		playheadNormalMode();

		FM->setButtonObj(interfaceButton3, buttonPress, functFill);
		FM->setButtonObj(interfaceButton4, buttonPress, functPreview);
		FM->setButtonObj(interfaceButton5, buttonPress, functInvert);
		FM->setButtonObj(interfaceButton6, buttonPress, functTranspose);
		FM->setButtonObj(interfaceButton7, buttonPress, functUndo);

		lightUpPadBoard();
	}
	else
	{
		trackerPattern.selectState = 0;

		hideEditModeLabels();

		FM->clearButtonsRange(interfaceButton3, interfaceButton7);

		padsBacklight.clearAllPads(0, 1, 0);

	}


	display.refreshControl(patternControl);

}

void cPatternEditor::changeFillData(int16_t value)
{
	if(fillPlace < 0 && fillPlace > 4)
	{
		return;
	}

	int16_t* ptrVal;
	int16_t min = 0, max;

	switch(fillPlace)
	{
	case 1:
		ptrVal = &fillData[editParam].type;
		max = fillTypeList.length - 1;
		//value = value*(-1);
		break;
	case 2:
		ptrVal = &fillData[editParam].from;
		max = (trackerPattern.selectedParam == 1 ? 63 : 127);
		break;
	case 3:
		ptrVal = &fillData[editParam].to;
		max = (editParam == 1 ? 63 : 127);
		break;
	case 4:
		ptrVal = &fillData[editParam].param;
		min = (editParam == 0 ? 0 : -1);
		max = (editParam == 0 ? 1 : FX_COUNT);
		//value = value*(-1);
		break;
	case 0:
		ptrVal = &fillStep;
		min = -2;
		max = PATTERN_EDIT_STEP_MAX;
		break;

	default: return;

	}

	if(*ptrVal + value < min) *ptrVal = min;
	else if(*ptrVal + value > max) *ptrVal = max;
	else *ptrVal += value;

	switch(fillPlace)
	{
		case 1:  refreshFillType(); break;
		case 2:  refreshFillFrom(); break;
		case 3:  refreshFillTo(); break;
		case 4:  refreshFillParam(); break;
		case 0:  refreshFillStep(); break;
		default: return;
	}
}

void cPatternEditor::changneFillDataByPad(uint8_t pad)
{
	switch(fillPlace)
	{
	case 2:
	{
		if(editParam == 0) 		fillData[editParam].from = mtPadBoard.getNoteFromPad(pad);
		else if(editParam == 1) fillData[editParam].from = pad;
		else 					fillData[editParam].from = map(pad,0,47,0,127);
		break;
	}
	case 3:
	{
		if(editParam == 0) 		fillData[editParam].to = mtPadBoard.getNoteFromPad(pad);
		else if(editParam == 1) fillData[editParam].to = pad;
		else 					fillData[editParam].to = map(pad,0,47,0,127);
		break;
	}
	case 4:
	{
		if(editParam == 3) fillData[editParam].param = pad;
		break;
	}
	case 0:
	{
		if(pad-2 > PATTERN_EDIT_STEP_MAX) fillStep = PATTERN_EDIT_STEP_MAX;
		else fillStep = pad-2;
		break;
	}
	default: break;
	}

	switch(fillPlace)
	{
		case 1:  refreshFillType(); break;
		case 2:  refreshFillFrom(); break;
		case 3:  refreshFillTo(); break;
		case 4:  refreshFillParam(); break;
		case 0:  refreshFillStep(); break;
		default: return;
	}
}


void cPatternEditor::setFillPlace(uint8_t place, int8_t dir)
{
	if(place < 0 || place > 4) return;

	if(place == 3)
	{
		if(fillData[editParam].type == 0)
		{
			if(dir<0) fillPlace = 2;
			else if(dir>0) fillPlace = (editParam==1) ? fillPlace : 4;
			else if(fillPlace == 3) fillPlace = 0;
		}
		else fillPlace = place;
	}
	else if(place == 4)
	{
		if(editParam==1)
		{
			if(dir>0)
			{

			}
			else
			{
				fillPlace = 0;
			}
		}
		else
		{
			fillPlace = place;
		}
	}
	else
	{
		fillPlace = place;
	}
}

void cPatternEditor::changeFillPlace(int8_t diff)
{
	//int8_t temp_place = fillPlace+diff;
	setFillPlace(fillPlace+diff, diff);
//
//	if(PTE->fillPlace > 0)
//	{
//		if(PTE->fillPlace == 5)
//		{
//			if(PTE->editParam != 1) PTE->fillPlace -= 2;
//			else if(PTE->fillData[PTE->editParam].type == 0) PTE->fillPlace -= 4;
//			else PTE->fillPlace -= 3;
//		}
//		else if(PTE->fillPlace == 3 && PTE->fillData[PTE->editParam].type == 0)
//		{
//			PTE->fillPlace -= 2;
//		}
//		else PTE->fillPlace--;
//		PTE->activateFillPopupBorder();
//	}
}

void cPatternEditor::setMuteFunct(uint8_t state)
{
	if(state == 0)
	{
		FM->clearButtonsRange(interfaceButton0,interfaceButton7);
		FM->setButtonObj(interfaceButton0, functChangePattern);
		FM->setButtonObj(interfaceButton1, functChangePatternLength);
		FM->setButtonObj(interfaceButton2, functChangePatternEditStep);

		if(editMode)
		{
			FM->setButtonObj(interfaceButton3, buttonPress, functFill);
			FM->setButtonObj(interfaceButton4, buttonPress, functPreview);
			FM->setButtonObj(interfaceButton5, buttonPress, functInvert);
			FM->setButtonObj(interfaceButton6, buttonPress, functTranspose);
			FM->setButtonObj(interfaceButton7, buttonPress, functUndo);
		}
	}
	else
	{
		FM->setButtonObj(interfaceButton0, functActionButton);
		FM->setButtonObj(interfaceButton1, functActionButton);
		FM->setButtonObj(interfaceButton2, functActionButton);
		FM->setButtonObj(interfaceButton3, functActionButton);
		FM->setButtonObj(interfaceButton4, functActionButton);
		FM->setButtonObj(interfaceButton5, functActionButton);
		FM->setButtonObj(interfaceButton6, functActionButton);
		FM->setButtonObj(interfaceButton7, functActionButton);

	}
}

uint8_t cPatternEditor::isCursorInSelection()
{
	uint8_t x1 = 0, x2 = 0;
	uint16_t y1 = 0, y2 = 0;

	if(trackerPattern.selectEndStep > trackerPattern.selectStartStep)
	{
		y1 = trackerPattern.selectStartStep;
		y2 = trackerPattern.selectEndStep;
	}
	else
	{
		y2 = trackerPattern.selectStartStep;
		y1 = trackerPattern.selectEndStep;
	}

	if(trackerPattern.selectEndTrack > trackerPattern.selectStartTrack)
	{
		x1 = trackerPattern.selectStartTrack;
		x2 = trackerPattern.selectEndTrack;
	}
	else
	{
		x2 = trackerPattern.selectStartTrack;
		x1 = trackerPattern.selectEndTrack;
	}


	if(trackerPattern.actualStep < y1 || trackerPattern.actualStep > y2
		|| trackerPattern.actualTrack < x1 || trackerPattern.actualTrack > x2) return 0;

	return 1;
}


void cPatternEditor::setProjectSaveFlags()
{
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
}



//==============================================================================================================
//==============================================================================================================


uint8_t functEncoder(int16_t value)
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillData(value);
		return 1;
	}

	if(PTE->selectedPlace >= 0)
	{
		fileManager.storePatternUndoRevision();

		switch(PTE->selectedPlace)
		{
		//case 0: PTE->changeActualTempo(value); break;
		case 0: PTE->changeActualPattern(value); break;
		case 1: PTE->changeActualPatternLength(value); break;
		case 2: PTE->changeActualPatternEditStep(value); break;
		}

		PTE->lightUpPadBoard();

		return 1;
	}

	if(PTE->editMode == 0)
	{
		if(PTE->trackerPattern.actualTrack + value < 0) PTE->trackerPattern.actualTrack = 0;
		else if(PTE->trackerPattern.actualTrack + value > 7) PTE->trackerPattern.actualTrack = 7;
		else  PTE->trackerPattern.actualTrack += value;

		PTE->focusOnActual();
		display.refreshControl(PTE->patternControl);

		return 1;
	}

	//czy parametr widoczny jesli np. pokazywane tylko 2 parametry 8 trackow
	if(PTE->patternViewMode > 0 && !(PTE->patternViewMode & (1 << PTE->editParam))) return 1;


	sendSelection();
	if(tactButtons.isButtonPressed(interfaceButton6) || !isMultiSelection())
	{
		fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
		fileManager.storePatternUndoRevision();
		switch(PTE->editParam)
		{
		case 0: sequencer.changeSelectionNote(value); break;
		case 1: sequencer.changeSelectionInstrument(value); break;
		case 2:// sequencer.changeSelectionVolume(value); break;
		case 3:
		{
			uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

			if (tactButtons.isButtonPressed(interfaceButtonFx1) || tactButtons.isButtonPressed(interfaceButtonFx2))
			{
				sequencer.changeSelectionFxType(fx_index, value);
			}
			else
			{
				sequencer.changeSelectionFxValue(fx_index, value);
			}
			break;
		}
		}
	}

	PTE->lightUpPadBoard();
	PTE->refreshPattern();

	return 1;
}


static  uint8_t functShift(uint8_t state)
{
	if(state == buttonPress)
	{
		if(mtPopups.getStepPopupState() != stepPopupNone) // ukrywa popup nuta...fx jesli jest wyswietlany
		{
			PTE->insertOnPopupHideDisabled = 1;  // dezaktywuje wstawianie danych na wyjecie z popupow
			PTE->cancelPopups();
			PTE->insertOnPopupHideDisabled = 0;  // a tu aktywuje spowrotem
		}
		else
		{
			PTE->setMuteFunct(1);
		}
	}
	else if(state == buttonRelease)
	{
		if(PTE->isSelectingNow == 1) // konczenie zaznaczania po puszczeniu shifta
		{
			PTE->isSelectingNow = 0;
			PTE->trackerPattern.selectColumn = 0;
		}

		if(PTE->editMode && !PTE->isCursorInSelection())
		{
			PTE->trackerPattern.selectState = 1;
			display.refreshControl(PTE->patternControl);
		}

//		if(PTE->editMode && !isMultiSelection() && !PTE->shiftAction )
//		{
//			sendSelection();
//			sequencer.blinkSelectedStep();
//		}

		PTE->setMuteFunct(0);

		PTE->shiftAction = 0;
	}

	return 1;
}


static  uint8_t functLeft()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillPlace(-1);
		PTE->activateFillPopupBorder();
		return 1;
	}

	if(PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
//		if(PTE->selectedPlace > 0)
//		{
//			PTE->selectedPlace--;
//			PTE->activateLabelsBorder();
//		}
		return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);

	if(PTE->editMode)
	{
		if(shiftPressed && PTE->isSelectingNow == 0)// pierwsze wcisniecie kierunku po wcisnieciu shift
		{
			PTE->trackerPattern.selectStartStep = PTE->trackerPattern.actualStep;
			PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
			//PTE->trackerPattern.selectState = 1;
			PTE->isSelectingNow = 1;
		}

		if(PTE->trackerPattern.actualTrack > 0) PTE->trackerPattern.actualTrack--;

		PTE->focusOnActual();

		if(shiftPressed && PTE->trackerPattern.selectColumn == 1) // zaznaczanie kolumny
		{
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.patternLength-1;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
			PTE->isSelectingNow = 1;
		}
		else if(shiftPressed && PTE->trackerPattern.selectColumn == 0)
		{
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
		}
		else if(!shiftPressed)
		{
			PTE->trackerPattern.selectState = 1;
			PTE->trackerPattern.selectColumn = 0;
			PTE->isSelectingNow = 0;
			//display.refreshControl(PTE->patternControl);
		}
	}
	else
	{
		if(PTE->patternViewMode == 0 && PTE->trackerPattern.firstVisibleTrack > 0 ) PTE->trackerPattern.firstVisibleTrack--;
	}

	PTE->lightUpPadBoard();
	display.refreshControl(PTE->patternControl);

	return 1;
}

static  uint8_t functRight()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillPlace(1);
		PTE->activateFillPopupBorder();
		return 1;
	}

	if(PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
//		if(PTE->selectedPlace < 3)
//		{
//			PTE->selectedPlace++;
//			PTE->activateLabelsBorder();
//		}
		return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);

	if(PTE->editMode)
	{
		if(shiftPressed && PTE->isSelectingNow == 0)// pierwsze wcisniecie kierunku po wcisnieciu shift
		{
			PTE->trackerPattern.selectStartStep = PTE->trackerPattern.actualStep;
			PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
			//PTE->trackerPattern.selectState = 1;
			PTE->isSelectingNow = 1;
		}

		if(PTE->trackerPattern.actualTrack < 7) PTE->trackerPattern.actualTrack++;

		PTE->focusOnActual();

		if(shiftPressed && PTE->trackerPattern.selectColumn == 1) // zaznaczanie kolumny
		{
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.patternLength-1;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
			PTE->isSelectingNow = 1;
		}
		else if(shiftPressed && PTE->trackerPattern.selectColumn == 0)
		{
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
		}
		else if(!shiftPressed)
		{
			PTE->trackerPattern.selectState = 1;
			PTE->trackerPattern.selectColumn = 0;
			PTE->isSelectingNow = 0;
			//display.refreshControl(PTE->patternControl);
		}
	}
	else
	{
		if(PTE->patternViewMode == 0 && PTE->trackerPattern.firstVisibleTrack < 4 ) PTE->trackerPattern.firstVisibleTrack++;
	}

	PTE->lightUpPadBoard();
	display.refreshControl(PTE->patternControl);

	return 1;
}


static  uint8_t functUp()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillData((PTE->fillPlace == 2 || PTE->fillPlace == 3) ? 1 : -1);
		return 1;
	}

	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		fileManager.storePatternUndoRevision();
		switch(PTE->selectedPlace)
		{
		//case 0: PTE->changeActualTempo(1); 			 return 1;
		case 0: PTE->changeActualPattern(1); 		 return 1;
		case 1: PTE->changeActualPatternLength(1); 	 return 1;
		case 2: PTE->changeActualPatternEditStep(1); return 1;
		}
		return 1;
	}

	if(PTE->editMode == 0)
	{
		if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP) return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);


	if(PTE->editMode == 1 && shiftPressed && PTE->trackerPattern.actualStep == 0) // zaznaczanie calej kolumny
	{
		if(PTE->trackerPattern.selectColumn == 1)  // zaznacz wszystko
		{
			PTE->trackerPattern.selectStartStep = 0;
			PTE->trackerPattern.selectStartTrack = 0;
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.patternLength-1;
			PTE->trackerPattern.selectEndTrack = Sequencer::MAXTRACK;
			PTE->trackerPattern.selectState = 2;
			PTE->isSelectingNow = 1;
		}
		else // zaznacz kolumne
		{
			PTE->trackerPattern.selectColumn = 1;

			PTE->trackerPattern.selectStartStep = 0;
			PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.patternLength-1;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
			PTE->isSelectingNow = 1;
		}

	}

	if(PTE->editMode == 1 && shiftPressed && PTE->isSelectingNow == 0) // poczatek zaznaczenia
	{
		PTE->trackerPattern.selectStartStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
		//PTE->trackerPattern.selectState = 1;
		PTE->isSelectingNow = 1;
	}

	if(PTE->trackerPattern.actualStep > 0 ) PTE->trackerPattern.actualStep--; // zmiana pozycji kursora
	else if(!shiftPressed) PTE->trackerPattern.actualStep = PTE->trackerPattern.patternLength;

	if(PTE->editMode == 1 && shiftPressed && PTE->trackerPattern.selectColumn == 0) // kontynuacja zaznaczania
	{
		PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
		PTE->trackerPattern.selectState = 2;
	}
	else if(!shiftPressed)
	{
		PTE->trackerPattern.selectColumn = 0;
		PTE->trackerPattern.selectState = 1;
		PTE->isSelectingNow = 0;
		//display.refreshControl(PTE->patternControl);
	}

	PTE->refreshPattern();
	PTE->lightUpPadBoard();

	return 1;
}


static  uint8_t functDown()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillData((PTE->fillPlace == 2 || PTE->fillPlace == 3)? -1 : 1);
		return 1;
	}

	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		fileManager.storePatternUndoRevision();
		switch(PTE->selectedPlace)
		{
		//case 0: PTE->changeActualTempo(-1); 			return 1;
		case 0: PTE->changeActualPattern(-1); 		 	return 1;
		case 1: PTE->changeActualPatternLength(-1); 	return 1;
		case 2: PTE->changeActualPatternEditStep(-1);	return 1;
		}

		return 1;
	}

	if(PTE->editMode == 0)
	{
		if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP) return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);


//	if(PTE->editMode == 1 && shiftPressed && PTE->trackerPattern.selectColumn == 1)
//	{
//
//	}

	if(PTE->editMode == 1 && shiftPressed && PTE->isSelectingNow == 0)
	{
		PTE->trackerPattern.selectStartStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
		//PTE->trackerPattern.selectState = 1;
		PTE->isSelectingNow = 1;
	}

	if(PTE->trackerPattern.actualStep < PTE->trackerPattern.patternLength-1) PTE->trackerPattern.actualStep++;
	else if(!shiftPressed) PTE->trackerPattern.actualStep = 0;

	if(PTE->editMode == 1 && shiftPressed)
	{
		PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
		PTE->trackerPattern.selectState = 2;
		PTE->trackerPattern.selectColumn = 0;
	}
	else
	{
		PTE->trackerPattern.selectState = 1;
		PTE->trackerPattern.selectColumn = 0;
		PTE->isSelectingNow = 0;
		//display.refreshControl(PTE->patternControl);
	}

	PTE->refreshPattern();
	PTE->lightUpPadBoard();

	return 1;
}


//=========================================================================================================

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
static  uint8_t functNote(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->wasNotesEditBefore = 0;
		PTE->editParam = 0;
		PTE->trackerPattern.selectedParam = 0;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(1);
		}
	}
	else if(state == buttonHold
			&& mtPopups.getStepPopupState() == stepPopupNone
			&& !tactButtons.isButtonPressed(interfaceButtonShift)
			//&& !tactButtons.isButtonPressed(interfaceButtonCopy)
			&& !tactButtons.isButtonPressed(interfaceButtonPattern)
			&& !PTE->dontShowPopupsUntilButtonRelease)
	{

		int8_t show_note = sequencer.getPatternToUI()->track[PTE->trackerPattern.actualTrack].step[PTE->trackerPattern.actualStep].note;

		if(show_note >= 0 && mtPadBoard.getPadsWithNote(show_note, PTE->padsTempData))
		{
			for(uint8_t i = 0; i < 48; i++)
			{
				if(PTE->padsTempData[i])
				{
					show_note = i;
				}
			}
		}
		else
		{
			show_note = -1;
		}

		mtPopups.showStepPopup(stepPopupNote, show_note);
		//PTE->lightUpPadBoard();
	}
	else if(state == buttonRelease)
	{
		PTE->cancelPopups();
		PTE->dontShowPopupsUntilButtonRelease = 0;
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functInstrument(uint8_t state)
{
	if(state == buttonPress)
	{
		if(PTE->editParam  == 0) PTE->wasNotesEditBefore = 1;
		PTE->editParam = 1;
		PTE->trackerPattern.selectedParam = 1;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->wasNotesEditBefore = 0;
			PTE->showFillPopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(2);
		}
	}
	else if(state == buttonHold
			&& mtPopups.getStepPopupState() == stepPopupNone
			&& !tactButtons.isButtonPressed(interfaceButtonShift)
			&& !tactButtons.isButtonPressed(interfaceButtonPattern)
			&& !PTE->dontShowPopupsUntilButtonRelease)
	{
		mtPopups.showStepPopup(stepPopupInstr, mtProject.values.lastUsedInstrument);

		//PTE->lightUpPadBoard();

		// odswiezenie paternu bez danych zakrytych przez popup
		PTE->trackerPattern.popupMode = 1;
		display.refreshControl(PTE->patternControl);
	}
	else if(state == buttonRelease)
	{
		// powrot do nuty po wybraniu instrumentu
		if(PTE->wasNotesEditBefore && mtPopups.getStepPopupState() != stepPopupNone)
		{
			PTE->wasNotesEditBefore = 0;
			PTE->editParam = 0;
			PTE->trackerPattern.selectedParam = 0;
			display.refreshControl(PTE->patternControl);

			PTE->focusOnPattern();
			PTE->lightUpPadBoard();
		}

		PTE->cancelPopups();
		PTE->dontShowPopupsUntilButtonRelease = 0;
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functFx1(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->wasNotesEditBefore = 0;
		PTE->editParam = 2;
		PTE->trackerPattern.selectedParam = 2;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(3);
		}
	}
	else if(state == buttonHold
			&& mtPopups.getStepPopupState() == stepPopupNone
			&& !tactButtons.isButtonPressed(interfaceButtonShift)
			//&& !tactButtons.isButtonPressed(interfaceButtonCopy)
			&& !tactButtons.isButtonPressed(interfaceButton6)
			&& !tactButtons.isButtonPressed(interfaceButtonPattern)
			&& !PTE->dontShowPopupsUntilButtonRelease)
	{
		if(PTE->fillState == 1) return 1;

		PTE->FM->clearButton(interfaceButtonFx1);

		mtProject.values.lastUsedFx = PTE->getStepFx();
		mtPopups.showStepPopup(stepPopupFx, mtProject.values.lastUsedFx); //PTE->getStepFx()

		//PTE->lightUpPadBoard();

		// odswiezenie paternu bez danych zakrytych przez popup
		PTE->trackerPattern.popupMode = 1;
		display.refreshControl(PTE->patternControl);
	}
	else if(state == buttonRelease)
	{
		PTE->cancelPopups();
		PTE->dontShowPopupsUntilButtonRelease = 0;
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functFx2(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->wasNotesEditBefore = 0;
		PTE->editParam = 3;
		PTE->trackerPattern.selectedParam = 3;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(4);
		}
	}
	else if(state == buttonHold
			&& mtPopups.getStepPopupState() == stepPopupNone
			&& !tactButtons.isButtonPressed(interfaceButtonShift)
			//&& !tactButtons.isButtonPressed(interfaceButtonCopy)
			&& !tactButtons.isButtonPressed(interfaceButton6)
			&& !tactButtons.isButtonPressed(interfaceButtonPattern)
			&& !PTE->dontShowPopupsUntilButtonRelease)
	{
		if(PTE->fillState == 1) return 1;

		PTE->FM->clearButton(interfaceButtonFx2);

		mtProject.values.lastUsedFx = PTE->getStepFx();
		mtPopups.showStepPopup(stepPopupFx, mtProject.values.lastUsedFx); //PTE->getStepFx()

		//PTE->lightUpPadBoard();

		// odswiezenie paternu bez danych zakrytych przez popup
		PTE->trackerPattern.popupMode = 1;
		display.refreshControl(PTE->patternControl);
	}
	else if(state == buttonRelease)
	{
		PTE->cancelPopups();
		PTE->dontShowPopupsUntilButtonRelease = 0;
	}

	return 1;
}

static  uint8_t functPattern(uint8_t state)
{
	if(state == buttonPress)
	{
		if(PTE->fillState == 1) return 1;

		PTE->cancelPopups();

	}
	else if(state == buttonRelease && PTE->patternButtonReleaseActive)
	{
			PTE->setPatternViewMode(0);
	}
	else if(state == buttonRelease)
	{
		PTE->patternButtonReleaseActive = 1;
	}

	return 1;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
static  uint8_t functPlayAction()
{
	if (sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{

		if (tactButtons.isButtonPressed(interfaceButtonRec))
		{
			sequencer.rec();
			PTE->editMode = 0;
			Serial.println("rec");

			strPopupStyleConfig popupConfig {
					2,					// time
					800 / 2 - 150,		// x
					480 / 2 - 50,		// y
					300,				// w
					100,				// h
					0xff0000,			// lineColor[4];
					0xffffff,
					0xffffff,
					0xffffff,
					controlStyleCenterX,			//lineStyle[4];
					controlStyleCenterX,
					controlStyleCenterX,
					controlStyleCenterX };

			mtPopups.config(0, &popupConfig);
			mtPopups.show(0, "REC");


			// todo: popup REC
		}
		else if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
			PTE->refreshPatternParams();
			PTE->shiftAction = 1;
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

		PTE->trackerPattern.playheadPosition = 0;
		//PTE->trackerPattern.actualStep = 0;
		PTE->refreshPattern();
	}

	return 1;
}


static  uint8_t functRecAction()
{
	if(sequencer.isRec())
	{
		sequencer.stop();
	}

	if(PTE->fillState == 1) return 1;

	PTE->editMode = !PTE->editMode;



	PTE->refreshEditState();



	return 1;
}



static uint8_t functInsertHome(uint8_t state)
{
	if (state == buttonPress)
	{

		if (PTE->editMode == 1)
		{
			fileManager.storePatternUndoRevision();
			fileManager.setPatternChangeFlag(mtProject.values.actualPattern);

			// HOME
			if(tactButtons.isButtonPressed(interfaceButtonShift))
			{
				if (isMultiSelection())
				{
					PTE->trackerPattern.selectStartStep=0;
					PTE->trackerPattern.selectEndStep=0;
					PTE->trackerPattern.actualStep=0; // zmiana pozycji kursora
				}
				else
				{
					PTE->trackerPattern.actualStep=0; // zmiana pozycji kursora
				}
			}
			// INSERT
			else
			{
				sendSelection();
				sequencer.insert(&sequencer.selection);
			}
		}

		PTE->refreshPattern();

	}

	return 1;
}


static uint8_t getSelectedElement()
{
	if (tactButtons.isButtonPressed(interfaceButtonNote))
	{
		return Sequencer::ELEMENTS_NOTES;
	}
	else if (tactButtons.isButtonPressed(interfaceButtonInstr))
	{
		return Sequencer::ELEMENTS_INSTRUMENTS;
	}
	else if (tactButtons.isButtonPressed(interfaceButtonFx1))
	{
		return Sequencer::ELEMENTS_FX1;
	}
	else if (tactButtons.isButtonPressed(interfaceButtonFx2))
	{
		return Sequencer::ELEMENTS_FX2;
	}
	else if (tactButtons.isButtonPressed(interfaceButtonPattern))
	{
		return Sequencer::ELEMENTS_ALL_WITH_PREFERENCES;
	}
	else
	{
		return Sequencer::ELEMENTS_ALL_NO_PREFERENCES;
	}
	return Sequencer::ELEMENTS_ALL_NO_PREFERENCES;
}





static uint8_t functCopyPaste(uint8_t state)
{

	if (state == buttonPress)
	{

		if (PTE->editMode == 1)
		{
			fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
			fileManager.storePatternUndoRevision();

			if (tactButtons.isButtonPressed(interfaceButtonShift))
			{
				sendPasteSelection();
				sequencer.pasteFromBuffer(getSelectedElement());
				PTE->moveCursorByStep();

			}
			else
			{
				sendCopySelection();
				sequencer.copyToBuffer();
				PTE->shiftAction = 1;
			}

		}

		PTE->refreshPattern();
		PTE->showPattern();
		PTE->showLength();
	}

	return 1;
}
static uint8_t functDeleteBackspace(uint8_t state)
{

	if (state == buttonPress)
	{

		if (PTE->editMode == 1)
		{
			fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
			fileManager.storePatternUndoRevision();

			// backspace
			if (tactButtons.isButtonPressed(interfaceButtonShift))
			{
				sendSelection();
				sequencer.backspace();
				if (isMultiSelection())
				{
					if (PTE->trackerPattern.selectStartStep > 0)
					{
						PTE->trackerPattern.selectStartStep--;
						PTE->trackerPattern.selectEndStep--;
						PTE->trackerPattern.actualStep--; // zmiana pozycji kursora
					}
				}
				else
				{
					if (PTE->trackerPattern.actualStep > 0)
						PTE->trackerPattern.actualStep--; // zmiana pozycji kursora
				}
			}
			// DELETE
			else
			{
				PTE->dontShowPopupsUntilButtonRelease = 1;	// blokuje ponowne pojawianie sie popupu
															// jesli ciagle wcisniety przycisk popupu
															// do czasu release note...fx
				if(mtPopups.getStepPopupState() != stepPopupNone) // ukrywa popup nuta...fx jesli jest wyswietlany
				{
					PTE->insertOnPopupHideDisabled = 1;  // dezaktywuje wstawianie danych na wyjecie z popupow
					PTE->cancelPopups();
					PTE->insertOnPopupHideDisabled = 0;  // a tu aktywuje spowrotem
				}

				sendSelection();
				sequencer.clearSelected(getSelectedElement());
				PTE->shiftAction = 1;
				PTE->moveCursorByStep();
			}

		}

		PTE->refreshPattern();
	}

	return 1;
}

void sendSelection()
{
 	if (isMultiSelection())
	{
		sequencer.setSelection(PTE->trackerPattern.selectStartStep,
								PTE->trackerPattern.selectStartTrack,
								PTE->trackerPattern.selectEndStep,
								PTE->trackerPattern.selectEndTrack);
	}
	else
	{
		sequencer.setSelection(PTE->trackerPattern.actualStep,
								PTE->trackerPattern.actualTrack,
								PTE->trackerPattern.actualStep,
								PTE->trackerPattern.actualTrack);
	}
}
void sendCopySelection()
{
	if (isMultiSelection())
	{
		sequencer.setCopySelection(PTE->trackerPattern.selectStartStep,
									PTE->trackerPattern.selectStartTrack,
									PTE->trackerPattern.selectEndStep,
									PTE->trackerPattern.selectEndTrack);
	}
	else
	{
		sequencer.setCopySelection(PTE->trackerPattern.actualStep,
									PTE->trackerPattern.actualTrack,
									PTE->trackerPattern.actualStep,
									PTE->trackerPattern.actualTrack);
	}
}
 void sendPasteSelection()
{
	if (isMultiSelection())
	{
		sequencer.setPasteSelection(PTE->trackerPattern.selectStartStep,
									PTE->trackerPattern.selectStartTrack,
									PTE->trackerPattern.selectEndStep,
									PTE->trackerPattern.selectEndTrack);
	}
	else
	{
		sequencer.setPasteSelection(PTE->trackerPattern.actualStep,
									PTE->trackerPattern.actualTrack,
									PTE->trackerPattern.actualStep,
									PTE->trackerPattern.actualTrack);
	}
}

uint8_t isMultiSelection()
{
	return PTE->trackerPattern.selectState == 2;

}
uint8_t isEditMode()
{
	return PTE->editMode == 1;

}

//##############################################################################################
//##########################             PATTERN PARAMS             ############################
//##############################################################################################
static  uint8_t functChangePattern(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->selectedPlace = 0;
		PTE->unfocusPattern();
		PTE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(PTE->selectedPlace == 0)
		{
			PTE->focusOnPattern();
		}
	}

	return 1;
}

static  uint8_t functChangePatternLength(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->selectedPlace = 1;
		PTE->unfocusPattern();
		PTE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(PTE->selectedPlace == 1)
		{
			PTE->focusOnPattern();
		}
	}

	return 1;
}
static  uint8_t functChangePatternEditStep(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->selectedPlace = 2;
		PTE->unfocusPattern();
		PTE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(PTE->selectedPlace == 2)
		{
			PTE->focusOnPattern();
		}
	}



	return 1;
}
//##############################################################################################
//###############################             PREVIEW          #################################
//##############################################################################################

static  uint8_t functPreview()
{
	sendSelection();
	sequencer.playSelection();
}

//##############################################################################################
//###############################             FILL             #################################
//##############################################################################################

static  uint8_t functFill()
{
	PTE->fillState = 1;

	PTE->focusOnPattern();

	PTE->showFillPopup();


	PTE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PTE->FM->setButtonObj(interfaceButton6, buttonPress, functFillCancel);
	PTE->FM->setButtonObj(interfaceButton7, buttonPress, functFillApply);


	PTE->FM->setButtonObj(interfaceButton0, buttonPress, functFillChangeParam4);
	PTE->FM->setButtonObj(interfaceButton1, buttonPress, functFillChangeType);
	PTE->FM->setButtonObj(interfaceButton2, buttonPress, functFillChangeParam1);
	PTE->FM->setButtonObj(interfaceButton3, buttonPress, functFillChangeParam2);
	PTE->FM->setButtonObj(interfaceButton4, buttonPress, functFillChangeParam3);


	return 1;
}

static  uint8_t functFillCancel()
{
	PTE->fillState = 0;
	PTE->selectedPlace = -1;

	PTE->hideFillPopup();

	PTE->start(0);

	return 1;
}

static  uint8_t functFillApply()
{
	// zatwierdzanie wypelnienia
	if(PTE->fillState)
	{
		fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
		fileManager.storePatternUndoRevision();
		cPatternEditor::strFill * fillData = &PTE->fillData[PTE->editParam];
		//(void) PTE->fillData[PTE->editParam];
		//(void) PTE->fillStep;
		// PTE->fillData[x]		<= przechowuje dane do konfiguracji fill
		// x - (0-3)(PTE->editParam) - co wypelnia - nuta , instr , vol , fx
		// 							type  	 = typ wypelninia (0-3);
		//							from	 = pierwsza wartosc (0-127 - nuty/vol/fx_val , 0-48 instr)
		//							to		 = druga wartosc (jesli potrzebna)
		//							param	 = scala yes/no (0-1) albo typ efektu (0-xx)
		//
		// PTE->fillStep 		<= krok dla filowania
		// PTE->editParam		<= nuta / instr / vol / fx
		//--------------------------------------------------------
		//TU
		sendSelection();
		switch (PTE->editParam)
		{
		case 0:
			if (fillData->type == 2)
			{
				sequencer.fillRandomNotes(PTE->fillStep,
											fillData->param,
											fillData->from,
											fillData->to);
			}
			else if (fillData->type == 1)
			{
				sequencer.fillLinearNotes(PTE->fillStep,
											fillData->param,
											fillData->from,
											fillData->to);
			}
			else if (fillData->type == 0)
			{
				sequencer.fillLinearNotes(PTE->fillStep,
											fillData->param,
											fillData->from,
											fillData->from);
			}

			break;
		case 1:
			if (fillData->type == 0)
			{
				sequencer.fillLinearInstruments(PTE->fillStep,
												fillData->from,
												fillData->from);
			}
			else if (fillData->type == 1)
			{
				sequencer.fillLinearInstruments(PTE->fillStep,
												fillData->from,
												fillData->to);
			}
			else if (fillData->type == 2)
			{
				sequencer.fillRandomInstruments(PTE->fillStep,
												fillData->from,
												fillData->to);
			}

			break;

		case 2:
		case 3:
		{
			uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

			if (fillData->type == 0)
			{
				sequencer.fillLinearFx(fx_index,
									   PTE->fillStep,
									   fillData->param,
										fillData->from,
										fillData->from);
			}
			else if (fillData->type == 1)
			{
				sequencer.fillLinearFx(fx_index,
									   PTE->fillStep,
									   fillData->param,
										fillData->from,
										fillData->to);
			}
			else if (fillData->type == 2)
			{
				sequencer.fillRandomFx(fx_index,
									   PTE->fillStep,
									   fillData->param ,
										fillData->from,
										fillData->to);
			}

			break;
		}
		default:
			break;
		}


		//--------------------------------------------------------
		functFillCancel();
	}


	return 1;
}

static  uint8_t functFillChangeType()
{
	//PTE->fillPlace = 0;
	PTE->setFillPlace(1);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam1()
{
	//PTE->fillPlace = 1;
	PTE->setFillPlace(2);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam2()
{
	//PTE->fillPlace = 2;
	PTE->setFillPlace(3);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam3()
{
	//PTE->fillPlace = 3;
	PTE->setFillPlace(4);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam4()
{
	//PTE->fillPlace = 5;
	PTE->setFillPlace(0);
	PTE->activateFillPopupBorder();

	return 1;
}


//##############################################################################################
//###############################            INVERT            #################################
//##############################################################################################
uint8_t functInvert()
{
	//--------------------------------------------------------
	//TU

	fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
	fileManager.storePatternUndoRevision();

	sendSelection();
	sequencer.invertSelectedSteps(getSelectedElement());

	PTE->refreshPattern();

	//--------------------------------------------------------
	return 1;
}
//##############################################################################################
//###############################            TRANSPOSE		   #################################
//##############################################################################################
static uint8_t functTranspose()
{
	//--------------------------------------------------------
	//TU



	//--------------------------------------------------------
	return 1;
}
//##############################################################################################
//###############################            UNDO		   #################################
//##############################################################################################
static uint8_t functUndo()
{
	if (tactButtons.isButtonPressed(interfaceButtonShift))
	{
		fileManager.redoPattern();
	}
	else
	{
		fileManager.undoPattern();
	}
	PTE->refreshPattern();
	PTE->showPattern();
	PTE->showLength();
	return 1;
}






//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################


void cPatternEditor::lightUpPadBoard()
{
	padsBacklight.clearAllPads(0, 1, 0);


	//--------------------------------------------

	if(selectedPlace >= 0)
	{
		//padsBacklight.clearAllPads(0, 1, 0);
		switch(selectedPlace)
		{
			case 0:
			{
				if(mtProject.values.actualPattern <= 48)
				{
					padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, mtProject.values.actualPattern-1);
				}
				break;
			}
			case 1:
			{
				uint8_t temp_length = sequencer.getPatternToUI()->track[0].length;
				if(temp_length < 3 || temp_length > 191) break;
				padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, map(temp_length,3,191,0,47));
				break;
			}
			case 2:
			{
				padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack,  mtProject.values.patternEditStep);
				break;
			}
		}

		return;
	}

	//--------------------------------------------

	if(editMode > 0)
	{
		Sequencer::strPattern * seq = sequencer.getPatternToUI();

		int8_t show_note = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].note;

		switch(editParam)
		{
		case 0:
		{
			if(show_note < 0) break;

			if(mtPadBoard.getPadsWithNote(show_note, padsTempData))
			{
				for(uint8_t i = 0; i < 48; i++)
				{
					if(padsTempData[i])
					{
						padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, i);
					}
				}
			}

			break;
		}

		case 1:
		{
			if(show_note < 0) break;

			int8_t show_instr = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].instrument;

			if(show_instr >= 0 && show_instr <= 47)
			{
				padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, show_instr);
			}

			break;
		}

		case 2:
		case 3:
		{
			uint8_t show_fx = 0;


			if(mtPopups.getStepPopupState() != stepPopupNone)
			{
				show_fx = mtPopups.getStepPopupValue();

				if(show_fx < 0 || show_fx > FX_MAX) break;

				padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, show_fx);
			}
			else
			{
				uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

				if(seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[fx_index].type != 0)
				{
					// co pokazywac na padach:
					if(tactButtons.isButtonPressed(interfaceButtonFx1) || tactButtons.isButtonPressed(interfaceButtonFx2))
					{
						show_fx = interfaceGlobals.fxIdToName(
								seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[fx_index].type);    // typ
						if(show_fx > FX_MAX) break;
					}
					else
					{
						show_fx = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[fx_index].value; // wartosc
						if(show_fx > FX_VALUE_MAX) break;
						show_fx = map(show_fx,0,127,0,47); // przeskalowanie - dla wartosci!!
					}


					padsBacklight.setBackLayer(1, mtConfig.values.padsLightBack, show_fx);

				}
			}





			break;
		}


		default: break;

		}


	}




}



static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	// obsługa przycisków pod ekranem (na poczatku bo dziala tez bez editmode)
	if (PTE->selectedPlace >= 0)
	{
		fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
		fileManager.storePatternUndoRevision();

		switch (PTE->selectedPlace)
		{
		case 0:
			PTE->setActualPattern(pad+1);
			PTE->showPattern();
			break;
		case 1:
			PTE->setActualPatternLength(map(pad, 0, 47, 3, 191));
			PTE->showLength();
			break;
		case 2:
			PTE->setActualPatternEditStep(pad);
			PTE->showStep();
			break;
		}

		PTE->lightUpPadBoard();

		return 1;
	}

	// dalej tylko jesli edit (rec)
	if (PTE->editMode != 1)
	{
		sendSelection();
		if (state == buttonPress)
		{
			uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
			sequencer.handleNote(Sequencer::MIDI_CHANNEL_GRID, noteFromPad,
									127);
		}
		else if (state == buttonRelease)
		{
			uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
			sequencer.handleNote(Sequencer::MIDI_CHANNEL_GRID, noteFromPad, 0);
		}
		return 1;
	}

	// obsluga podswietlenia
	if (state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0, 0, pad);
	}
	else if (state == buttonPress)
	{
		padsBacklight.setFrontLayer(1, mtConfig.values.padsLightFront, pad);
	}
	else
	{
		return 1;
	}

	//obluga popupow fill/randomise
	if(PTE->fillState > 0)
	{
		if(state == buttonPress) PTE->changneFillDataByPad(pad);
		return 1;
	}

	if(PTE->patternViewMode > 0 && !(PTE->patternViewMode & (1 << PTE->editParam))) return 1;

	// wprowadzanie danych
	if (PTE->editMode == 1)
	{
		fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
		fileManager.storePatternUndoRevision();

		switch (PTE->editParam)
		{
		case 0: // nuta
		{
			sendSelection();
			if (state == buttonPress)
			{
				uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
				sequencer.handleNote(Sequencer::MIDI_CHANNEL_GRID, noteFromPad, 127);
			}
			else if (state == buttonRelease)
			{
				uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
				sequencer.handleNote(Sequencer::MIDI_CHANNEL_GRID, noteFromPad, 0);
			}
			break;
		}

		case 1: // instrument
		{
			if (state == buttonPress)
			{
				sendSelection();
				sequencer.setSelectionInstrument(pad);
			}
			else if (state == buttonRelease)
			{
				sequencer.handleNote(Sequencer::MIDI_CHANNEL_GRID, 0, 0);
			}
			break;
		}

		case 2: // fx1
		case 3: // fx2
		{
			if (state == buttonPress)
			{
				uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

				sendSelection();
				fileManager.setPatternChangeFlag(mtProject.values.actualPattern);
				fileManager.storePatternUndoRevision();

				if((tactButtons.isButtonPressed(interfaceButtonFx1) || tactButtons.isButtonPressed(interfaceButtonFx2))
						)//&& mtPopups.getStepPopupState() == stepPopupNone)
				{
					PTE->dontShowPopupsUntilButtonRelease = 1;
					sequencer.setSelectionFxType(fx_index, interfaceGlobals.fxIDs[pad]);
				}
				else
				{
					sequencer.setSelectionFxValue(fx_index, map(pad, 0, 47, 0, 127));
				}
			}
			break;
		}

		default:
			break;

		}

		if (state == buttonPress)
		{
			PTE->moveCursorByStep();
			PTE->lightUpPadBoard();
			PTE->refreshPattern();
		}

	}


	return 1;
}

//##############################################################################################
//###############################                              #################################
//##############################################################################################

void cPatternEditor::focusOnPattern()
{
	PTE->selectedPlace = -1;
	PTE->activateLabelsBorder();

	PTE->lightUpPadBoard();

	activateSelection();

	display.refreshControl(patternControl);
}

void cPatternEditor::unfocusPattern()
{
	lightUpPadBoard();

	deactivateSelection();

	display.refreshControl(patternControl);
}





void cPatternEditor::setPatternViewMode(uint8_t param)
{
	if(param == 0) // wroc do starego widoku domyslnego
	{
		if(PTE->patternViewChanged == 0)
		{
			PTE->patternViewMode = 0;
			//PTE->focusOnPattern();
			PTE->focusOnActual();
			display.setControlValue(PTE->patternControl, PTE->patternViewMode);
			display.refreshControl(PTE->patternControl);
		}
		else PTE->patternViewChanged = 0;

		selectedParams[0] = 0;
		selectedParams[1] = 0;

		return;
	}

	if(selectedParams[0] == 0 || (selectedParams[0] > 0 && tactButtons.isButtonPressed((interfaceButtonNote-1)+selectedParams[0]) == 0))
	{
		selectedParams[0] = param;
		selectedParams[1] = 0;
	}
	else if(selectedParams[1] == 0 && selectedParams[0] != param)
	{
		selectedParams[1] = param;
	}
	else if(selectedParams[1] != param)
	{
		selectedParams[0] = selectedParams[1];
		selectedParams[1] = param;
	}

	PTE->patternViewMode = 0;
	if(selectedParams[0] > 0) PTE->patternViewMode = (1<<(selectedParams[0]-1));
	if(selectedParams[1] > 0) PTE->patternViewMode |= (1<<(selectedParams[1]-1));


	PTE->patternViewChanged = 1;
	PTE->trackerPattern.firstVisibleTrack = 0;

	display.setControlValue(PTE->patternControl, PTE->patternViewMode);
	display.refreshControl(PTE->patternControl);
}


static uint8_t functSwitchModule(uint8_t button)
{
//	if(tactButtons.isButtonPressed(interfaceButtonShift) && PTE->editMode == 1)
	if(PTE->editMode == 1)
	{
		Sequencer::strPattern* seq = sequencer.getPatternToUI();
		Sequencer::strPattern::strTrack::strStep *actualStep = &seq->track[PTE->trackerPattern.actualTrack].
				step[PTE->trackerPattern.actualStep];

		if (actualStep->note >= 0)
		{
			mtProject.values.lastUsedNote = actualStep->note;
			mtProject.values.lastUsedInstrument = constrain(
					actualStep->instrument,
					0,
					INSTRUMENTS_MAX+16);
		}
		/*
		// fx znacza cos tylko w paternie wiec po co je zmieniac prxzyz wychodzeniu do innych modolow
		if (actualStep->fx[0].type > 0)
		{
			mtProject.values.lastUsedFx = actualStep->fx[0].type;
		}
		else if (actualStep->fx[1].type > 0)
		{
			mtProject.values.lastUsedFx = actualStep->fx[1].type;
		}
		*/
	}

	PTE->eventFunct(eventSwitchModule,PTE,&button,0);

	return 1;
}


//======================================================================================================================

