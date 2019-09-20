
#include <patternEditor.h>
#include "mtStructs.h"

#include "mtFileManager.h"

#include "mtPadBoard.h"
#include "mtPadsBacklight.h"

#include "interfacePopups.h"

#include "keyScanner.h"

extern keyScanner tactButtons; // dla isButtonPressed()

cPatternEditor patternEditor;
static  cPatternEditor* PTE = &patternEditor;

extern strMtProject mtProject;
extern uint32_t patternTrackerSelectionColor;


static  uint8_t functChangeTempo(uint8_t state);
static  uint8_t functChangePattern(uint8_t state);
static  uint8_t functChangePatternLength(uint8_t state);
static  uint8_t functChangePatternEditStep(uint8_t state);

static  uint8_t functNote(uint8_t state);
static  uint8_t functInstrument(uint8_t state);
static  uint8_t functVolume(uint8_t state);
static  uint8_t functFx(uint8_t state);

static  uint8_t functFill();
static  uint8_t functFillCancel();
static  uint8_t functFillApply();
static  uint8_t functFillChangeType();
static  uint8_t functFillChangeParam1();
static  uint8_t functFillChangeParam2();
static  uint8_t functFillChangeParam3();
static  uint8_t functFillChangeParam4();


static  uint8_t functRandomise();
static  uint8_t functRandomiseCancel();
static  uint8_t functRandomiseApply();
static  uint8_t functRandomiseChangeParam1();
static  uint8_t functRandomiseChangeParam2();
static  uint8_t functRandomiseChangeParam3();
static  uint8_t functRandomiseChangeParam4();

static  uint8_t functInvert();
static  uint8_t functTranspose();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();





static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);


static  uint8_t functPlayAction();
static  uint8_t functRecAction();
static  uint8_t functPasteInsert(uint8_t state);
static  uint8_t functCopyDelete(uint8_t state);

static uint8_t getSelectedElement();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


char getHexFromInt(int16_t val, uint8_t index);



elapsedMillis patternRefreshTimer;

void cPatternEditor::update()
{
	if(patternRefreshTimer < 20) return;

	uint8_t sequencerState = sequencer.getSeqState();

	if(sequencerState != 1 ) return;

	readPatternState();

	if(lastPlayedPattern != mtProject.mtProjectRemote.song.playlist[mtProject.mtProjectRemote.song.playlistPos])
	{
		lastPlayedPattern = mtProject.mtProjectRemote.song.playlist[mtProject.mtProjectRemote.song.playlistPos];

		mtProject.values.actualPattern = mtProject.mtProjectRemote.song.playlist[mtProject.mtProjectRemote.song.playlistPos];

		refreshPatternParams();
	}

	if(trackerPattern.playheadPosition == lastPatternPosition || (!isPleyheadOnScreen() && editMode))  return;


	refreshPattern();

	lastPatternPosition = trackerPattern.playheadPosition;

	patternRefreshTimer = 0;
}



void cPatternEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mtProject.values.padBoardScale = 0;
	mtProject.values.padBoardNoteOffset = 12;
	mtProject.values.padBoardRootNote = 36;

	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote);
	mtPadBoard.configureInstrumentPlayer(8);

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
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	refreshEditState();

	focusOnPattern();
}

void cPatternEditor::stop()
{
	if(fillState) fillState = 0;
	if(randomiseState) randomiseState = 0;

	padsBacklight.clearAllPads(1, 1, 1);

	moduleRefresh = 0;
}

void cPatternEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonPaste, functPasteInsert);
//	FM->setButtonObj(interfaceButtonPaste, buttonRelease, functPasteInsertRelease);
	FM->setButtonObj(interfaceButtonCopy, functCopyDelete);
//	FM->setButtonObj(interfaceButtonCopy, buttonRelease, functCopyDeleteRelease);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);



	FM->setButtonObj(interfaceButtonNote,functNote);
	FM->setButtonObj(interfaceButtonInstr, functInstrument);
	FM->setButtonObj(interfaceButtonVol, functVolume);
	FM->setButtonObj(interfaceButtonFx, functFx);



	FM->setButtonObj(interfaceButton0, functChangeTempo);
	FM->setButtonObj(interfaceButton1, functChangePattern);
	FM->setButtonObj(interfaceButton2, functChangePatternLength);
	FM->setButtonObj(interfaceButton3, functChangePatternEditStep);


	//FM->setButtonObj(interfaceButton4, buttonPress, functFill);
	//FM->setButtonObj(interfaceButton5, buttonPress, functRandom);
	//FM->setButtonObj(interfaceButton6, buttonPress, functInvert);
	//FM->setButtonObj(interfaceButton7, buttonPress, );


	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);


	FM->setPotObj(interfacePot0, functEncoder, nullptr);


	FM->setPadsGlobal(functPads);


}

//==============================================================================================================
// przeniesienie danych z sekewncji  do struktury wyswietlania
void cPatternEditor::refreshPattern()
{
	seq = sequencer.getPatternToUI();

	if(editMode == 0)
	{
		trackerPattern.selectState = 0;

		if(sequencer.getSeqState() == 1)
		{
			trackerPattern.actualStep = trackerPattern.playheadPosition;
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
		for(uint8_t j = 0; j < 15; j++) // step
		{
			strTrackerPattern::strTracks::strRow *row = &trackerPattern.track[i].row[j];

			if(j-6 > steps_down || j<7-steps_up)
			{
				row->note[0] = 0;
				row->note[1] = 0;
				row->note[2] = 0;
				row->note[3] = 0;
				row->instr[0] = 0;
				row->instr[1] = 0;
				row->instr[2] = 0;
				row->vol[0] = 0;
				row->vol[1] = 0;
				row->vol[2] = 0;
				row->fx[0] = 0;
				row->fx[1] = 0;
				row->fx[2] = 0;
				row->fx[3] = 0;
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

			if (showInstrument)
			{
				char inst0 = (seq->track[i].step[patternPosition - 7 + j].instrument + 1) / 10;
				char inst1 = (seq->track[i].step[patternPosition - 7 + j].instrument + 1) % 10;

				trackerPattern.track[i].row[j].instr[0] = inst0 + 48;
				trackerPattern.track[i].row[j].instr[1] = inst1 + 48;
				trackerPattern.track[i].row[j].instr[2] = 0;
			}
			else
			{
				trackerPattern.track[i].row[j].instr[0] = '-';
				trackerPattern.track[i].row[j].instr[1] = '-';
				trackerPattern.track[i].row[j].instr[2] = 0;
			}

			if (seq->track[i].step[patternPosition - 7 + j].velocity >= 0)
			{

				row->vol[0] = getHexFromInt(
						seq->track[i].step[patternPosition - 7 + j].velocity,
						1);
				row->vol[1] = getHexFromInt(
						seq->track[i].step[patternPosition - 7 + j].velocity,
						0);
				row->vol[2] = 0;
			}
			else
			{
				row->vol[0] = '-';
				row->vol[1] = '-';
				row->vol[2] = 0;
			}



			if(seq->track[i].step[patternPosition-7+j].fx[0].type)
			{
				trackerPattern.track[i].row[j].fx[0] = 0;
				trackerPattern.track[i].row[j].fx[1] = 0;
				trackerPattern.track[i].row[j].fx[2] = 0;

				strncat(&trackerPattern.track[i].row[j].fx[0],
						&interfaceGlobals.fxNames[seq->track[i].step[patternPosition - 7 + j].fx[0].type][0],
						1);

				sprintf(&trackerPattern.track[i].row[j].fx[1],
						"%2.2x",
						seq->track[i].step[patternPosition - 7 + j].fx[0].value);

				trackerPattern.track[i].row[j].fx[3] = 0;
			}
			else
			{
				trackerPattern.track[i].row[j].fx[0] = '-';
				trackerPattern.track[i].row[j].fx[1] = '-';
				trackerPattern.track[i].row[j].fx[2] = '-';
				trackerPattern.track[i].row[j].fx[3] = 0;
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
	trackerPattern.playheadPosition = sequencer.ptrPlayer->row[0].actual_pos;


}

uint8_t cPatternEditor::getStepFx()
{
	uint8_t selectedFx = 0;

	uint8_t fx_type =  sequencer.getPatternToUI()->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[0].type;

	if(fx_type < FX_COUNT) selectedFx = fx_type;

	return selectedFx;
}

int8_t cPatternEditor::getStepVol()
{
	int8_t show_vol = sequencer.getPatternToUI()->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity;

	if(show_vol < 0) return -1;

	return map(show_vol,0,127,0,47);
}


// focusowanie trackow na kursorze, jesli jest poza ekranem
void cPatternEditor::focusOnActual()
{
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
	if(mtPopups.getStepPopupState() != stepPopupNone)
	{
		mtPopups.hideStepPopups();
		setDefaultScreenFunct();
		showDefaultScreen();
		refreshEditState();

		if(fillState)
		{
			functFill();
		}

		if(randomiseState)
		{
			functRandomise();
		}
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

	showTempo();
}

void cPatternEditor::changeActualPattern(int16_t value)
{

	fileManager.savePattern(mtProject.values.actualPattern);

	mtProject.values.actualPattern = constrain(
			mtProject.values.actualPattern + value, PATTERN_INDEX_MIN,
			PATTERN_INDEX_MAX);

	fileManager.loadPattern(mtProject.values.actualPattern);
	sequencer.switchNextPatternNow();


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
	sequencer.switchNextPatternNow();

	readPatternState();
	refreshPatternParams();
	refreshPattern();

}

void cPatternEditor::changeActualPatternLength(int16_t value)
{
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

		FM->setButtonObj(interfaceButton4, buttonPress, functFill);
		FM->setButtonObj(interfaceButton5, buttonPress, functRandomise);
		FM->setButtonObj(interfaceButton6, buttonPress, functInvert);
		FM->setButtonObj(interfaceButton7, buttonPress, functTranspose);

		lightUpPadBoard();

		strPopupStyleConfig config;
		config.time = 2;
		config.w = 300;
		config.h = 100;
		config.x = 800/2-150;
		config.y = 480/2-50;
		config.lineColor[0] = 0xff0000;
		config.lineColor[1] = 0x00ff00;
		config.lineStyle[0] = controlStyleCenterX;
		config.lineStyle[1] = controlStyleCenterX;


		mtPopups.config(0, &config);
		mtPopups.show(0, "Warning!", "Edit mode enabled");

	}
	else
	{
		trackerPattern.selectState = 0;

		hideEditModeLabels();

		FM->clearButtonsRange(interfaceButton4, interfaceButton7);

		padsBacklight.clearAllPads(0, 1, 0);

	}


	display.refreshControl(patternControl);

}

void cPatternEditor::changeFillData(int16_t value)
{
	if(fillPlace < 0 && fillPlace > 5)
	{
		return;
	}

	uint16_t* ptrVal;
	uint16_t min = 0, max;

	switch(fillPlace)
	{
	case 0:
		ptrVal = &fillData[editParam].type;
		max = fillTypeList.length - 1;
		//value = value*(-1);
		break;
	case 1:
		ptrVal = &fillData[editParam].from;
		max = (trackerPattern.selectedParam == 1 ? 48 : 127);
		break;
	case 2:
		ptrVal = &fillData[editParam].to;
		max = (editParam == 1 ? 48 : 127);
		break;
	case 3:
		ptrVal = &fillData[editParam].param;
		max = (editParam == 0 ? interfaceGlobals.fillScaleFilterCount-1 : FX_COUNT-1);
		//value = value*(-1);
		break;
	case 5:
		ptrVal = &fillStep;
		min = 0;
		max = PATTERN_EDIT_STEP_MAX;
		break;

	default: return;

	}

	if(*ptrVal + value < min) *ptrVal = min;
	else if(*ptrVal + value > max) *ptrVal = max;
	else *ptrVal += value;

	switch(fillPlace)
	{
		case 0:  refreshFillType(); break;
		case 1:  refreshFillFrom(); break;
		case 2:  refreshFillTo(); break;
		case 3:  refreshFillParam(); break;
		case 5:  refreshFillStep(); break;
		default: return;
	}
}


void cPatternEditor::changeRandomiseData(int16_t value)
{
	if(randomisePlace < 1 && randomisePlace > 5)
	{
		return;
	}

	uint16_t* ptrVal;
	uint16_t min = 0, max;

	switch(randomisePlace)
	{
	case 1:
		ptrVal = &randomiseData[editParam].from;
		max = (trackerPattern.selectedParam == 1 ? 48 : 127);
		break;
	case 2:
		ptrVal = &randomiseData[editParam].to;
		max = (editParam == 1 ? 48 : 127);
		break;
	case 3:
		ptrVal = &randomiseData[editParam].param;
		max = (editParam == 0 ? interfaceGlobals.fillScaleFilterCount-1 : FX_COUNT-1);
		//value = value*(-1);
		break;
/*	case 5:
		ptrVal = &randomiseStep;
		min = 1;
		max = PATTERN_EDIT_STEP_MAX;
		break;
*/
	default: return;

	}

	if(*ptrVal + value < min) *ptrVal = min;
	else if(*ptrVal + value > max) *ptrVal = max;
	else *ptrVal += value;

	switch(randomisePlace)
	{
		case 1:  refreshRandomiseFrom(); 	break;
		case 2:  refreshRandomiseTo(); 		break;
		case 3:  refreshRandomiseParam();	break;
		case 5:  refreshRandomiseStep(); 	break;
		default: return;
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
		|| trackerPattern.actualTrack < x1 || trackerPattern.actualTrack > x2)
	return 0;


	return 1;
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
	if(PTE->randomiseState > 0)
	{
		PTE->changeRandomiseData(value);
		return 1;
	}

	if(PTE->selectedPlace >= 0)
	{

		switch(PTE->selectedPlace)
		{
		case 0: PTE->changeActualTempo(value); break;
		case 1: PTE->changeActualPattern(value); break;
		case 2: PTE->changeActualPatternLength(value); break;
		case 3: PTE->changeActualPatternEditStep(value); break;
		}

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


	sendSelection();
	if(tactButtons.isButtonPressed(interfaceButton7) || !isMultiSelection())
	{
		switch(PTE->editParam)
		{
		case 0: sequencer.changeSelectionNote(value); break;
		case 1: sequencer.changeSelectionInstrument(value); break;
		case 2: sequencer.changeSelectionVolume(value); break;
		case 3:
			if (tactButtons.isButtonPressed(interfaceButtonFx))
			{
				//sequencer.changeSelectionFxType(value);
			}
			else
			{
				sequencer.changeSelectionFxValue(value);
			}
			break;
		}
	}

	PTE->lightUpPadBoard();
	PTE->refreshPattern();

	return 1;
}



static  uint8_t functEnter()
{

	return 1;
}

static  uint8_t functShift(uint8_t state)
{

	if(state == buttonPress)
	{


	}
	else if(state == buttonRelease)
	{
		if(PTE->isSelectingNow == 1) // konczenie zaznaczania po puszczeniu shifta
		{
			PTE->isSelectingNow = 0;
			PTE->trackerPattern.selectColumn = 0;
		}

		if(!PTE->isCursorInSelection())
		{
			PTE->trackerPattern.selectState = 1;
			display.refreshControl(PTE->patternControl);
		}

		if(PTE->editMode && !isMultiSelection() && !PTE->shiftAction )
		{
			sendSelection();
			sequencer.blinkSelectedStep();
		}


		PTE->shiftAction = 0;
	}

	return 1;
}


static  uint8_t functLeft()
{
	if(PTE->fillState > 0)
	{
		if(PTE->fillPlace > 0)
		{
			if(PTE->fillPlace == 5) PTE->fillPlace -= 2;
			else PTE->fillPlace--;
			PTE->activateFillPopupBorder();
		}
		return 1;
	}
	if(PTE->randomiseState > 0)
	{
		if(PTE->randomisePlace > 1)
		{
			if(PTE->randomisePlace == 5) PTE->randomisePlace -= 2;
			else PTE->randomisePlace--;
			PTE->activateRandomisePopupBorder();
		}
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
		if(shiftPressed && PTE->isSelectingNow == 0)// pierwsze wcisnienie kierunku po wcisnieniu shift
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
		if(PTE->trackerPattern.firstVisibleTrack > 0 ) PTE->trackerPattern.firstVisibleTrack--;
	}

	PTE->lightUpPadBoard();
	display.refreshControl(PTE->patternControl);

	return 1;
}

static  uint8_t functRight()
{
	if(PTE->fillState > 0)
	{
		if(PTE->fillPlace < 5)
		{
			if(PTE->fillPlace == 3) PTE->fillPlace += 2;
			else PTE->fillPlace++;
			PTE->activateFillPopupBorder();
		}
		return 1;
	}
	if(PTE->randomiseState > 0)
	{
		if(PTE->randomisePlace < 5)
		{
			if(PTE->randomisePlace == 3) PTE->randomisePlace += 2;
			else PTE->randomisePlace++;
			PTE->activateRandomisePopupBorder();
		}
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
		if(shiftPressed && PTE->isSelectingNow == 0)// pierwsze wcisnienie kierunku po wcisnieniu shift
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
		if(PTE->trackerPattern.firstVisibleTrack < 4 ) PTE->trackerPattern.firstVisibleTrack++;
	}

	PTE->lightUpPadBoard();
	display.refreshControl(PTE->patternControl);

	return 1;
}


static  uint8_t functUp()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillData((PTE->fillPlace == 0 || PTE->fillPlace == 3)? -1 : 1);
		return 1;
	}
	if(PTE->randomiseState > 0)
	{
		PTE->changeRandomiseData((PTE->randomisePlace == 3)? -1 : 1);
		return 1;
	}

	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		switch(PTE->selectedPlace)
		{
		case 0: PTE->changeActualTempo(1); 			 return 1;
		case 1: PTE->changeActualPattern(1); 		 return 1;
		case 2: PTE->changeActualPatternLength(1); 	 return 1;
		case 3: PTE->changeActualPatternEditStep(1); return 1;
		}

		return 1;
	}

	if(PTE->editMode == 0)
	{
		if(sequencer.getSeqState() == 1) return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);


	if(PTE->editMode == 1 && shiftPressed && PTE->trackerPattern.actualStep == 0) // zaznaczanie calej kolumny
	{
		if(PTE->trackerPattern.selectColumn == 1)  // zaznacz wszystko
		{
			PTE->trackerPattern.selectStartStep = 0;
			PTE->trackerPattern.selectStartTrack = 0;
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.patternLength-1;
			PTE->trackerPattern.selectEndTrack = 8;
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
		PTE->changeFillData((PTE->fillPlace == 0 || PTE->fillPlace == 3)? 1 : -1);
		return 1;
	}
	if(PTE->randomiseState > 0)
	{
		PTE->changeRandomiseData((PTE->randomisePlace == 3)? 1 : -1);
		return 1;
	}

	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		switch(PTE->selectedPlace)
		{
		case 0: PTE->changeActualTempo(-1); 			return 1;
		case 1: PTE->changeActualPattern(-1); 		 	return 1;
		case 2: PTE->changeActualPatternLength(-1); 	return 1;
		case 3: PTE->changeActualPatternEditStep(-1);	return 1;
		}

		return 1;
	}

	if(PTE->editMode == 0)
	{
		if(sequencer.getSeqState() == 1) return 1;
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
		PTE->editParam = 0;
		PTE->trackerPattern.selectedParam = 0;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}
		if(PTE->randomiseState > 0)
		{
			PTE->showRandomisePopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonDouble
			&& !tactButtons.isButtonPressed(interfaceButtonShift)
			&& !tactButtons.isButtonPressed(interfaceButtonCopy))
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
		PTE->lightUpPadBoard();
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functInstrument(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->editParam = 1;
		PTE->trackerPattern.selectedParam = 1;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}
		if(PTE->randomiseState > 0)
		{
			PTE->showRandomisePopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonDouble && !tactButtons.isButtonPressed(interfaceButtonShift))
	{
		mtPopups.showStepPopup(stepPopupInstr, mtProject.values.lastUsedInstrument);
		PTE->lightUpPadBoard();
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functVolume(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->editParam = 2;
		PTE->trackerPattern.selectedParam = 2;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}
		if(PTE->randomiseState > 0)
		{
			PTE->showRandomisePopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonDouble && !tactButtons.isButtonPressed(interfaceButtonShift))
	{
		mtPopups.showStepPopup(stepPopupVol, PTE->getStepVol());
		PTE->lightUpPadBoard();
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functFx(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->editParam = 3;
		PTE->trackerPattern.selectedParam = 3;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			return 1;
		}
		if(PTE->randomiseState > 0)
		{
			PTE->showRandomisePopup();
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonDouble
			&& !tactButtons.isButtonPressed(interfaceButtonShift)
			&& !tactButtons.isButtonPressed(interfaceButtonCopy))
	{
		//if(mtPopups.getStepPopupState() == stepPopupNone)
		//{
			mtPopups.showStepPopup(stepPopupFx, PTE->getStepFx());
			PTE->lightUpPadBoard();
		//}
	}

	return 1;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
static  uint8_t functPlayAction()
{
	if (sequencer.getSeqState() == 0)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
			PTE->shiftAction = 1;
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

		PTE->trackerPattern.playheadPosition = 0;
		//PTE->trackerPattern.actualStep = 0;
		PTE->refreshPattern();
	}

	return 1;
}


static  uint8_t functRecAction()
{
	if(PTE->fillState == 1 || PTE->randomiseState == 1) return 1;

	PTE->editMode = !PTE->editMode;

	PTE->refreshEditState();



	return 1;
}



static uint8_t functPasteInsert(uint8_t state)
{
	if (state == buttonPress)
	{

		if (PTE->editMode == 1)
		{
			if(tactButtons.isButtonPressed(interfaceButtonShift) && tactButtons.isButtonPressed(interfaceButtonEnter))
			{
				sendSelection();
				sequencer.insertReversed(&sequencer.selection);
			}
			else if(tactButtons.isButtonPressed(interfaceButtonShift))
			{
				sendPasteSelection();

				sequencer.pasteFromBuffer(getSelectedElement());
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
//	else if (state == buttonRelease)
//	{
//		if (PTE->editMode == 1)
//		{
//			// PASTE
//			if (tactButtons.isButtonPressed(interfaceButtonShift))
//			{
//
//			}
//		}
//
//		PTE->refreshPattern();
//	}

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
	else if (tactButtons.isButtonPressed(interfaceButtonVol))
	{
		return Sequencer::ELEMENTS_VELO;
	}
	else if (tactButtons.isButtonPressed(interfaceButtonFx))
	{
		return Sequencer::ELEMENTS_FXes;
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





static uint8_t functCopyDelete(uint8_t state)
{

	if (state == buttonPress)
	{

		if (PTE->editMode == 1)
		{
			// COPY
			if (tactButtons.isButtonPressed(interfaceButtonShift))
			{
				sendCopySelection();
				sequencer.copyToBuffer();
				PTE->shiftAction = 1;
			}
			// DELETE
			else
			{
				sendSelection();
				sequencer.clearSelected(getSelectedElement());
				PTE->shiftAction = 1;
			}

		}

		PTE->refreshPattern();
	}
	else if (state == buttonRelease)
	{
		if (PTE->editMode == 1)
		{
			// COPY
			if (tactButtons.isButtonPressed(interfaceButtonShift))
			{
				//			sendCopySelection();
				//			sequencer.copyToBuffer();
			}
			// DELETE
			else
			{
//				sendSelection();
//				sequencer.clearSelected(getSelectedElement());
//				PTE->shiftAction = 1;
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

//##############################################################################################
//##########################             PATTERN PARAMS             ############################
//##############################################################################################
static  uint8_t functChangeTempo(uint8_t state)
{
	if(state == buttonPress)
	{
		if(PTE->selectedPlace == 0)
		{
			PTE->focusOnPattern();
			return 1;
		}
		else
		{
			PTE->unfocusPattern();
		}

		PTE->selectedPlace = 0;
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

static  uint8_t functChangePattern(uint8_t state)
{
	if(state == buttonPress)
	{
		if(PTE->selectedPlace == 1)
		{
			PTE->focusOnPattern();
			return 1;
		}
		else
		{
			PTE->unfocusPattern();
		}

		PTE->selectedPlace = 1;
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

static  uint8_t functChangePatternLength(uint8_t state)
{
	if(state == buttonPress)
	{
		if(PTE->selectedPlace == 2)
		{
			PTE->focusOnPattern();
			return 1;
		}
		else
		{
			PTE->unfocusPattern();
		}

		PTE->selectedPlace = 2;
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
static  uint8_t functChangePatternEditStep(uint8_t state)
{
	if(state == buttonPress)
	{
		if(PTE->selectedPlace == 3)
		{
			PTE->focusOnPattern();
			return 1;
		}
		else
		{
			PTE->unfocusPattern();
		}

		PTE->selectedPlace = 3;
		PTE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(PTE->selectedPlace == 3)
		{
			PTE->focusOnPattern();
		}
	}



	return 1;
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

	PTE->FM->setButtonObj(interfaceButton0, buttonPress, functFillChangeType);
	PTE->FM->setButtonObj(interfaceButton1, buttonPress, functFillChangeParam1);
	PTE->FM->setButtonObj(interfaceButton2, buttonPress, functFillChangeParam2);
	PTE->FM->setButtonObj(interfaceButton3, buttonPress, functFillChangeParam3);
	PTE->FM->setButtonObj(interfaceButton5, buttonPress, functFillChangeParam4);


	return 1;
}

static  uint8_t functFillCancel()
{
	PTE->fillState = 0;
	PTE->selectedPlace = -1;

	PTE->hideFillPopup();

	PTE->start(0);

//	PTE->readPatternState();
//	PTE->refreshPattern();
//
//	PTE->showDefaultScreen();
//	PTE->setDefaultScreenFunct();
//
//	PTE->refreshEditState();
//
//	PTE->focusOnPattern();

	return 1;
}

static  uint8_t functFillApply()
{
	// zatwierdzanie wypelnienia
	if(PTE->fillState)
	{
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
											fillData->from,
											fillData->to);
			}
			else if (fillData->type == 1)
			{
				sequencer.fillLinearNotes(PTE->fillStep,
											fillData->from,
											fillData->to);
			}
			else if (fillData->type == 0)
			{
				sequencer.fillLinearNotes(PTE->fillStep,
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
			if (fillData->type == 0)
			{
				sequencer.fillLinearVelocity(PTE->fillStep,
											fillData->from,
											fillData->from);
			}
			else if (fillData->type == 1)
			{
				sequencer.fillLinearVelocity(PTE->fillStep,
											fillData->from,
											fillData->to);
			}
			else if (fillData->type == 2)
			{
				sequencer.fillRandomVelocity(PTE->fillStep,
											fillData->from,
											fillData->to);
			}

			break;
		case 3:
			if (fillData->type == 0)
			{
				sequencer.fillLinearFx(PTE->fillStep,
									   fillData->param,
										fillData->from,
										fillData->from);
			}
			else if (fillData->type == 1)
			{
				sequencer.fillLinearFx(PTE->fillStep,
									   fillData->param,
										fillData->from,
										fillData->to);
			}
			else if (fillData->type == 2)
			{
				sequencer.fillRandomFx(PTE->fillStep,
									   fillData->param ,
										fillData->from,
										fillData->to);
			}

			break;
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
	PTE->fillPlace = 0;
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam1()
{
	PTE->fillPlace = 1;
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam2()
{
	PTE->fillPlace = 2;
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam3()
{
	PTE->fillPlace = 3;
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillChangeParam4()
{
	PTE->fillPlace = 5;
	PTE->activateFillPopupBorder();

	return 1;
}


//##############################################################################################
//###############################            RANDOM            #################################
//##############################################################################################
static  uint8_t functRandomise()
{
	PTE->randomiseState = 1;

	PTE->focusOnPattern();

	PTE->showRandomisePopup();

	PTE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PTE->FM->setButtonObj(interfaceButton6, buttonPress, functRandomiseCancel);
	PTE->FM->setButtonObj(interfaceButton7, buttonPress, functRandomiseApply);

	PTE->FM->setButtonObj(interfaceButton1, buttonPress, functRandomiseChangeParam1);
	PTE->FM->setButtonObj(interfaceButton2, buttonPress, functRandomiseChangeParam2);
	PTE->FM->setButtonObj(interfaceButton3, buttonPress, functRandomiseChangeParam3);
	//PTE->FM->setButtonObj(interfaceButton5, buttonPress, functRandomiseChangeParam4);

	return 1;
}

static  uint8_t functRandomiseCancel()
{
	PTE->randomiseState = 0;
	PTE->selectedPlace = -1;

	PTE->hideRandomisePopup();

	PTE->start(0);

	return 1;
}

static  uint8_t functRandomiseApply()
{
	// zatwierdzanie wypelnienia
	if(PTE->randomiseState)
	{
		cPatternEditor::strRandomise * randomiseData = &PTE->randomiseData[PTE->editParam];
		//(void) PTE->randomiseData[PTE->editParam];
		//(void) PTE->randomiseStep;
		// PTE->randomiseData[x]	<= przechowuje dane do konfiguracji randomise
		// x - (0-3)(PTE->editParam) - co wypelnia - nuta , instr , vol , fx
		//							from	 = pierwsza wartosc (0-127 - nuty/vol/fx_val , 0-47 instr)
		//							to		 = druga wartosc (jesli potrzebna)
		//							param	 = scala yes/no (0-1) albo typ efektu (0-xx)
		//
		// PTE->randomiseStep 		<= krok dla filowania
		// PTE->editParam			<= nuta / instr / vol / fx
		//--------------------------------------------------------
		//TU
		sendSelection();
		switch (PTE->editParam)
		{
		case 0:
			sequencer.randomSelectedNotes(randomiseData->from,
											randomiseData->to,
											randomiseData->param);
			break;
		case 1:
			sequencer.randomSelectedInstruments(randomiseData->from,
												randomiseData->to);
			break;
		case 2:
			sequencer.randomSelectedVelo(randomiseData->from,
											randomiseData->to);
			break;

		break;
		}




		//--------------------------------------------------------
		functRandomiseCancel();
	}
	return 1;
}

static  uint8_t functRandomiseChangeParam1()
{
	PTE->randomisePlace = 1;
	PTE->activateRandomisePopupBorder();

	return 1;
}

static  uint8_t functRandomiseChangeParam2()
{
	PTE->randomisePlace = 2;
	PTE->activateRandomisePopupBorder();

	return 1;
}

static  uint8_t functRandomiseChangeParam3()
{
	PTE->randomisePlace = 3;
	PTE->activateRandomisePopupBorder();

	return 1;
}

/*
static  uint8_t functRandomiseChangeParam4()
{
	PTE->randomisePlace = 5;
	PTE->activateRandomisePopupBorder();

	return 1;
}
*/

//##############################################################################################
//###############################            INVERT            #################################
//##############################################################################################
static uint8_t functInvert()
{
	//--------------------------------------------------------
	//TU

	sendSelection();
	sequencer.invertSelectedSteps();

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
//###############################          PAD BOARD           #################################
//##############################################################################################


void cPatternEditor::lightUpPadBoard()
{
	padsBacklight.clearAllPads(0, 1, 0);

	Sequencer::strPattern * seq = sequencer.getPatternToUI();


	if(editMode > 0)
	{
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
						padsBacklight.setBackLayer(1, 20, i);
					}
				}
			}

			break;
		}

		case 1:
		{
			if(show_note < 0) break;

			int8_t show_instr = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].instrument;

			if(show_instr >= 0 && show_instr <= 48)
			{
				padsBacklight.setBackLayer(1, 20, show_instr);
			}

			break;
		}

		case 2:
		{
			int8_t show_vol = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity;

			if(show_vol < 0) break;

			show_vol = map(show_vol,0,127,0,47);

			padsBacklight.setBackLayer(1, 20, show_vol);

			break;
		}

		case 3:
		{
			uint8_t show_fx = 0;

			if(mtPopups.getStepPopupState() != stepPopupNone)
			{
				show_fx = mtPopups.getStepPopupValue();

				if(show_fx < 0 || show_fx > FX_MAX) break;

				padsBacklight.setBackLayer(1, 20, show_fx);
			}
			else
			{
				if(seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[0].type != 0)
				{
					// co pokazywac na padach:
					show_fx = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[0].type;    // typ
					//show_fx = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[0].value; // wartosc

					if(show_fx > FX_VALUE_MAX) break;

					show_fx = map(show_fx,0,127,0,47);

					padsBacklight.setBackLayer(1, 20, show_fx);

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
	if (PTE->editMode != 1) return 1;

	// obsluga podswietlenia
	if (state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0, 0, pad);
	}
	else if (state == buttonPress)
	{
		padsBacklight.setFrontLayer(1, 31, pad);
	}
	else
	{
		return 1;
	}


	//obluga popupow fill/randomise
	if(PTE->fillState > 0)
	{
		return 1;
	}
	if(PTE->randomiseState > 0)
	{
		return 1;
	}


	// obsługa przycisków pod ekranem

	if (PTE->selectedPlace >= 0)
	{

		switch (PTE->selectedPlace)
		{
		case 0:
			sequencer.setTempo(map((float) pad, 0, 47, 10, 480));
			PTE->showTempo();
			break;
		case 1:
			PTE->setActualPattern(pad+1);
			PTE->showPattern();
			break;
		case 2:
			PTE->setActualPatternLength(map(pad, 0, 47, 3, 191));
			PTE->showLength();
			break;
		case 3:
			PTE->setActualPatternEditStep(pad);
			PTE->showStep();
			break;
		}

		return 1;
	}

	// wprowadzanie danych
	if (PTE->editMode == 1)
	{

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

		case 2: // volume
		{
			if (state == buttonPress)
			{
				sendSelection();
				sequencer.setSelectionVelocity(map(pad, 0, 47, 0, 127));
			}
			break;
		}

		case 3: //fx
		{
			if (state == buttonPress)
			{
				sendSelection();
				sequencer.changeSelectionFxValue(map(pad, 0, 47, 0, 127));
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

	//if(PTE->editMode) PTE->trackerPattern.selectState = 1;

	PTE->trackerPattern.selectColor = 0xff5860;
	display.refreshControl(patternControl);
	//PTE->refreshPattern();
}

void cPatternEditor::unfocusPattern()
{
	//PTE->trackerPattern.selectState = 0;

	PTE->trackerPattern.selectColor = patternTrackerSelectionColor;
	display.refreshControl(patternControl);
	//PTE->refreshPattern();
}


static uint8_t functSwitchModule(uint8_t button)
{
	if(button == interfaceButtonPattern)
	{
		if(PTE->fillState == 1 || PTE->randomiseState == 1) return 1;

		PTE->cancelPopups();


		return 1;
	}


//	if(tactButtons.isButtonPressed(interfaceButtonShift) && PTE->editMode == 1)
	if(PTE->editMode == 1)
	{
		Sequencer::strPattern* seq = sequencer.getPatternToUI();

		mtProject.values.lastUsedNote =
				seq->track[PTE->trackerPattern.actualTrack].
				step[PTE->trackerPattern.actualStep].note;
		mtProject.values.lastUsedInstrument =
				seq->track[PTE->trackerPattern.actualTrack].
				step[PTE->trackerPattern.actualStep].instrument;
		mtProject.values.lastUsedVolume =
				seq->track[PTE->trackerPattern.actualTrack].
				step[PTE->trackerPattern.actualStep].velocity;
		mtProject.values.lastUsedFx =
				seq->track[PTE->trackerPattern.actualTrack].
				step[PTE->trackerPattern.actualStep].fx[0].type;
	}


	PTE->eventFunct(eventSwitchModule,PTE,&button,0);

	return 1;
}


//======================================================================================================================

