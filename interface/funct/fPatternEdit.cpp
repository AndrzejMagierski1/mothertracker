
#include <patternEditor.h>
#include "mtStructs.h"

#include "mtFileManager.h"


#include "keyScanner.h"
extern keyScanner tactButtons; // dla isButtonPressed()

cPatternEditor patternEditor;
static  cPatternEditor* PTE = &patternEditor;

extern strMtProject mtProject;

static  uint8_t functPlayAction();
static  uint8_t functRecAction();
static  uint8_t functPasteInsert();
static  uint8_t functCopyDelete();

static  uint8_t functChangeTempo();
static  uint8_t functChangePattern();
static  uint8_t functChangePatternLength();
static  uint8_t functChangePatternEditStep();

static  uint8_t functFill();
static  uint8_t functRandom();
static  uint8_t functInvert();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functNote();
static  uint8_t functInstrument();
static  uint8_t functVolume();
static  uint8_t functFx();



static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);




char getHexFromInt(int16_t val, uint8_t index);



elapsedMillis patternRefreshTimer;

void cPatternEditor::update()
{
	if(patternRefreshTimer < 20) return;

	uint8_t sequencerState = sequencer.getSeqState();

	if(sequencerState != 1 ) return;

	readPatternState();


	if(trackerPattern.playheadPosition == lastPatternPosition || (!isPleyheadOnScreen() && editMode))  return;



	refreshPattern();

	lastPatternPosition = trackerPattern.playheadPosition;

	patternRefreshTimer = 0;
}



void cPatternEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	//trackerPattern.selectState = 0;

	readPatternState();
	refreshPattern();


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchModule);
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


}

void cPatternEditor::stop()
{
	if(fillState) fillState = 0;


	moduleRefresh = 0;
}

void cPatternEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonPaste, buttonPress, functPasteInsert);
	FM->setButtonObj(interfaceButtonCopy, buttonPress, functCopyDelete);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

/*
	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);
*/

	FM->setButtonObj(interfaceButtonNote, buttonPress, functNote);
	FM->setButtonObj(interfaceButtonInstr, buttonPress, functInstrument);
	FM->setButtonObj(interfaceButtonVol, buttonPress, functVolume);
	FM->setButtonObj(interfaceButtonFx, buttonPress, functFx);



	FM->setButtonObj(interfaceButton0, buttonPress, functChangeTempo);
	FM->setButtonObj(interfaceButton1, buttonPress, functChangePattern);
	FM->setButtonObj(interfaceButton2, buttonPress, functChangePatternLength);
	FM->setButtonObj(interfaceButton3, buttonPress, functChangePatternEditStep);


	FM->setButtonObj(interfaceButton4, buttonPress, functFill);
	FM->setButtonObj(interfaceButton5, buttonPress, functRandom);
	FM->setButtonObj(interfaceButton6, buttonPress, functInvert);
	//FM->setButtonObj(interfaceButton7, buttonPress, );


	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);


	FM->setPotObj(interfacePot0, functEncoder, nullptr);




}
//==============================================================================================================
// przeniesienie danych z sekewncji  do struktury wyswietlania
void cPatternEditor::refreshPattern()
{
	seq = sequencer.getPatternToUI();

	if(editMode == 0)
	{
		trackerPattern.selectState = 0;

		trackerPattern.actualStep = trackerPattern.playheadPosition;
	}


	uint8_t steps_down = trackerPattern.patternLength - trackerPattern.actualStep;
	steps_down = (steps_down < 8) ? steps_down : 8;

	uint8_t steps_up = (trackerPattern.actualStep < 7) ? trackerPattern.actualStep : 7;

	int16_t patternPosition = trackerPattern.actualStep;

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
				showInstrument = 1;
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



			if(seq->track[i].step[patternPosition-7+j].fx[0].isOn)
			{
				trackerPattern.track[i].row[j].fx[0] = seq->track[i].step[patternPosition-7+j].fx[0].type + 59;
				trackerPattern.track[i].row[j].fx[1] = '0';
				trackerPattern.track[i].row[j].fx[2] = '0';
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
	display.refreshControl(bottomLabel[0]);
	display.setControlValue(bottomLabel[0], seq->tempo);
	display.refreshControl(bottomLabel[1]);
	display.setControlValue(bottomLabel[1], mtProject.values.actualPattern+1);
	display.refreshControl(bottomLabel[2]);
	display.setControlValue(bottomLabel[2], seq->track[0].length+1);
	display.refreshControl(bottomLabel[3]);
	display.setControlValue(bottomLabel[3], mtProject.values.patternEditStep);

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


void cPatternEditor::changeActualStepNote(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();
	Sequencer::strPattern::strTrack::strStep *step = &(pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep]);

	uint16_t dupa =pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].note;
	Serial.println(dupa);


	step->note =
			constrain(step->note + value,
						Sequencer::MIN_NOTE_STEP,
						Sequencer::MAX_NOTE_STEP);

}

void cPatternEditor::changeActualStepInstrument(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();
	Sequencer::strPattern::strTrack::strStep *step = &(pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep]);

	if (pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].note >= 0)
	{
		step->instrument = constrain(step->instrument + value,
								0,
								INSTRUMENTS_COUNT - 1);

		mtProject.values.lastUsedInstrument = step->instrument;
	}
	else
	{
		step->note = 24;
		step->instrument = mtProject.values.lastUsedInstrument;
		step->velocity = -1;
	}

}

void cPatternEditor::changeActualStepVolume(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

//	if(pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].isOn)
//	{
		int8_t step_volume = pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity;

		if(step_volume + value > Sequencer::MAX_VELO_STEP)
			pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity = Sequencer::MAX_VELO_STEP;
		else if(step_volume + value < Sequencer::MIN_VELO_STEP-1)
			pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity = Sequencer::MIN_VELO_STEP-1;
		else
			pattern->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].velocity += value;
//	}
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
	showPattern();

}

void cPatternEditor::changeActualPatternLength(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[0].length+value < 0) pattern->track[0].length = 0;
	else if(pattern->track[0].length+value > PATTERN_LENGTH_MAX) pattern->track[0].length = PATTERN_LENGTH_MAX;
	else  pattern->track[0].length += value;

	for(uint8_t i = 1;i < 8; i++)
		pattern->track[i].length = pattern->track[0].length;

	trackerPattern.patternLength = pattern->track[0].length+1;

	if(trackerPattern.actualStep > trackerPattern.patternLength-1) trackerPattern.actualStep = trackerPattern.patternLength-1;


	showLength();

	refreshPattern();

}

void cPatternEditor::changeActualPatternEditStep(int16_t value)
{
	if(mtProject.values.patternEditStep+value < 0) mtProject.values.patternEditStep = 0;
	else if(mtProject.values.patternEditStep+value > PATTERN_EDIT_STEP_MAX) mtProject.values.patternEditStep = PATTERN_EDIT_STEP_MAX;
	else  mtProject.values.patternEditStep += value;

	showStep();
}


void cPatternEditor::changeFillData(int16_t value)
{
	if(fillState)
	{

		uint16_t max;
		if(fillPopup.column[fillPlace].type == 1) // zmiana pozycji listy
		{
			max = ((strList*)(fillPopup.column[fillPlace].data))->length - 1;

			value = value*(-1);

			uint16_t* ptrVal = &lastFillValues[trackerPattern.selectedParam][fillPlace];

			if(*ptrVal + value < 0) *ptrVal = 0;
			else if(*ptrVal + value > max) *ptrVal = max;
			else *ptrVal += value;

		}
		else if(fillPopup.column[fillPlace].type == 2) // zmiana wartosci labela
		{
			if(trackerPattern.selectedParam == 0)
			{ // zmiana nuty
				uint16_t* ptrVal = &lastFillValues[trackerPattern.selectedParam][fillPlace];

				if(*ptrVal + value < 0) *ptrVal  = 0;
				else if(*ptrVal + value > 127) *ptrVal = 127;
				else *ptrVal += value;

				fillPopup.column[fillPlace].text = (char*)&mtNotes[*ptrVal][0];
			}
			else if (trackerPattern.selectedParam == 1 || trackerPattern.selectedParam == 2 || trackerPattern.selectedParam == 3)
			{ // zmiana reszty
				max = (trackerPattern.selectedParam == 1 ? 48 : 127);

				uint16_t* ptrVal = &lastFillValues[trackerPattern.selectedParam][fillPlace];

				if(*ptrVal + value < 0) *ptrVal  = 0;
				else if(*ptrVal + value > max) *ptrVal  = max;
				else *ptrVal += value;
			}
		}


		refreshFillPopup();
	}

}

void cPatternEditor::changeFillPlace(int16_t value)
{
	if(fillState)
	{
		if(fillPlace+value < 0) fillPlace = 0;
		else if(fillPlace+value >= fillPopup.columnsCount) fillPlace = fillPopup.columnsCount-1;
		else fillPlace += value;

		display.setControlValue(patternPopupControl, fillPlace);
		display.refreshControl(patternPopupControl);
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
	if(PTE->selectedPlace >= 0)
	{
		switch(PTE->selectedPlace)
		{
		case 0: PTE->changeActualTempo(value); break;
		case 1: PTE->changeActualPattern(value); break;
		case 2: PTE->changeActualPatternLength(value); break;
		case 3: PTE->changeActualPatternEditStep(value); break;

		case 4: PTE->changeFillData(value); break;

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
	switch(PTE->trackerPattern.selectedParam)
	{
	case 0: sequencer.changeSelectionNote(value); break;
	case 1: sequencer.changeSelectionInstrument(value); break;
	case 2: sequencer.changeSelectionVolume(value); break;
	case 3: break;
	}

	PTE->refreshPattern();

	return 1;
}


static  uint8_t functEnter()
{
	// zatwierdzanie wypelnienia
	if(PTE->fillState)
	{
		(void) PTE->lastFillValues[0][0];
		(void) mtProject.values.patternEditStep;
		// PTE->lastFillValues[x][y] <= przechowuje dane do konfiguracji fill
		// x - (0-3) - co wypelnia - nuta , instr , vol , fx
		// y - (0-3) - kolumna popupu fill - 	0 = typ wypelninia (0-3);
		//										1 = pierwsza wartosc (0-127 - nuty/vol/fx_val , 0-48 instr)
		//										2 = druga wartosc (jesli potrzebna)
		//										3 = scala yes/no (0-1) albo typ efektu (0-xx)
		//
		// mtProject.values.patternEditStep <= pattern step



		PTE->hideFillPopup();

		PTE->selectedPlace = -1;
		PTE->activateLabelsBorder();
	}



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
		}

		if(!PTE->isCursorInSelection())
		{
			PTE->trackerPattern.selectState = 1;
			display.refreshControl(PTE->patternControl);
		}


	}

	return 1;
}


static  uint8_t functLeft()
{
	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		if(PTE->fillState > 0 && PTE->selectedPlace == 4)
		{
			PTE->changeFillPlace(-1);
			return 1;
		}

		if(PTE->selectedPlace > 0)
		{
			PTE->selectedPlace--;
			PTE->activateLabelsBorder();
		}

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

		if(shiftPressed)
		{
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
		}
	}
	else
	{
		if(PTE->trackerPattern.firstVisibleTrack > 0 ) PTE->trackerPattern.firstVisibleTrack--;
	}

	display.refreshControl(PTE->patternControl);

	return 1;
}

static  uint8_t functRight()
{
	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		if(PTE->fillState > 0 && PTE->selectedPlace == 4)
		{
			PTE->changeFillPlace(1);
			return 1;
		}

		if(PTE->selectedPlace < 7)
		{
			PTE->selectedPlace++;
			PTE->activateLabelsBorder();
		}

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

		if(shiftPressed)
		{
			PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
			PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
			PTE->trackerPattern.selectState = 2;
		}
	}
	else
	{
		if(PTE->trackerPattern.firstVisibleTrack < 4 ) PTE->trackerPattern.firstVisibleTrack++;
	}

	display.refreshControl(PTE->patternControl);

	return 1;
}


static  uint8_t functUp()
{
	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		switch(PTE->selectedPlace)
		{
		case 0: PTE->changeActualTempo(1); 			 return 1;
		case 1: PTE->changeActualPattern(1); 		 return 1;
		case 2: PTE->changeActualPatternLength(1); 	 return 1;
		case 3: PTE->changeActualPatternEditStep(1); return 1;
		}

		if(PTE->fillState > 0)
		{
			PTE->changeFillData(1);
			return 1;
		}
	}

	if(PTE->editMode == 0)
	{
		if(sequencer.getSeqState() == 1) return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);


	if(PTE->editMode == 1 && shiftPressed && PTE->isSelectingNow == 0)
	{
		PTE->trackerPattern.selectStartStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
		//PTE->trackerPattern.selectState = 1;
		PTE->isSelectingNow = 1;
	}

	if(PTE->trackerPattern.actualStep > 0 ) PTE->trackerPattern.actualStep--;

	if(PTE->editMode == 1 && shiftPressed)
	{
		PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
		PTE->trackerPattern.selectState = 2;
	}



	PTE->refreshPattern();

	return 1;
}


static  uint8_t functDown()
{
	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
		switch(PTE->selectedPlace)
		{
		case 0: PTE->changeActualTempo(-1); 			return 1;
		case 1: PTE->changeActualPattern(-1); 		 	return 1;
		case 2: PTE->changeActualPatternLength(-1); 	return 1;
		case 3: PTE->changeActualPatternEditStep(-1);	return 1;
		}

		if(PTE->fillState > 0)
		{
			PTE->changeFillData(-1);
			return 1;
		}
	}

	if(PTE->editMode == 0)
	{
		if(sequencer.getSeqState() == 1) return 1;
	}

	uint8_t shiftPressed = tactButtons.isButtonPressed(interfaceButtonShift);


	if(PTE->editMode == 1 && shiftPressed && PTE->isSelectingNow == 0)
	{
		PTE->trackerPattern.selectStartStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
		//PTE->trackerPattern.selectState = 1;
		PTE->isSelectingNow = 1;
	}

	if(PTE->trackerPattern.actualStep <  PTE->trackerPattern.patternLength-1) PTE->trackerPattern.actualStep++;

	if(PTE->editMode == 1 && shiftPressed)
	{
		PTE->trackerPattern.selectEndStep = PTE->trackerPattern.actualStep;
		PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;
		PTE->trackerPattern.selectState = 2;
	}

	PTE->refreshPattern();

	return 1;
}

static  uint8_t functNote()
{
	PTE->trackerPattern.selectedParam = 0;

	if(PTE->fillState > 0)
	{
		PTE->showFillPopup();
		return 1;
	}

	PTE->selectedPlace = -1;
	PTE->activateLabelsBorder();
	PTE->refreshPattern();

	return 1;
}


static  uint8_t functInstrument()
{
	PTE->trackerPattern.selectedParam = 1;

	if(PTE->fillState > 0)
	{
		PTE->showFillPopup();
		return 1;
	}


	PTE->selectedPlace = -1;
	PTE->activateLabelsBorder();
	PTE->refreshPattern();

	return 1;
}

static  uint8_t functVolume()
{
	PTE->trackerPattern.selectedParam = 2;

	if(PTE->fillState > 0)
	{
		PTE->showFillPopup();
		return 1;
	}

	PTE->selectedPlace= -1;
	PTE->activateLabelsBorder();
	PTE->refreshPattern();

	return 1;
}

static  uint8_t functFx()
{
	PTE->trackerPattern.selectedParam = 3;

	if(PTE->fillState > 0)
	{
		PTE->showFillPopup();
		return 1;
	}

	PTE->selectedPlace= -1;
	PTE->activateLabelsBorder();
	PTE->refreshPattern();

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

		PTE->trackerPattern.playheadPosition = 0;
		PTE->trackerPattern.actualStep = 0;
		PTE->refreshPattern();
	}

	return 1;
}


static  uint8_t functRecAction()
{
	PTE->editMode = !PTE->editMode;

	if(PTE->editMode)
	{
		PTE->trackerPattern.selectState = 1;

		PTE->focusOnActual();
	}
	else  PTE->trackerPattern.selectState = 0;



	display.refreshControl(PTE->patternControl);

	return 1;
}



static  uint8_t functPasteInsert()
{
//	TODO: podstawić aktualne zaznaczenie - ZROBIONE


//	// test I cwiartki
//	sequencer.setSelection(0, 2, 7, 4);
//	sequencer.setPasteSelection(4, 0, 11, 2);

//	// test II cwiartki
//	sequencer.setSelection(0, 0, 7, 2);
//	sequencer.setPasteSelection(4, 2, 11, 4);

//	// test III cwiartki
//	sequencer.setSelection(4, 0, 11, 2);
//	sequencer.setPasteSelection(0, 2, 7, 4);

	// test IV cwiartki
//	sequencer.setSelection(4, 2, 11, 4);
//	sequencer.setPasteSelection(0, 0, 7, 2);

	if (PTE->editMode == 1)
	{
		// INSERT
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sendSelection();
			sequencer.insert(&sequencer.selection);
		}
		// PASTE
		else
		{

			sendPasteSelection();
			sequencer.copy();
		}

	}

	PTE->refreshPattern();

	return 1;
}

static uint8_t functCopyDelete()
{

//	sequencer.copy();

	if (PTE->editMode == 1)
	{
		// DELETE
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sendSelection();
			sequencer.clearSelected();

		}
		// COPY
		else
		{
			sendSelection();
		}

	}
//	else if (PTE->editMode == 1)
//	{
//		// SHIFT
//		if (tactButtons.isButtonPressed(interfaceButtonShift))
//		{
//			sendSelection();
//			sequencer.transposeSelection(1);
//
//		}
//		// NO SHIFT
//		else
//		{
//			sendSelection();
//			sequencer.transposeSelection(-1);
//
//		}
//
//	}

	PTE->refreshPattern();

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

static  uint8_t functChangeTempo()
{
	PTE->selectedPlace = 0;
	PTE->activateLabelsBorder();

	return 1;
}

static  uint8_t functChangePattern()
{
	PTE->selectedPlace = 1;
	PTE->activateLabelsBorder();

	//display.setControlValue(PTE->bottomLabel[1], mtProject.values.actualPattern+1);
	//display.refreshControl(PTE->bottomLabel[1]);

	//PTE->refreshPattern();

	return 1;
}

static  uint8_t functChangePatternLength()
{
	PTE->selectedPlace = 2;
	PTE->activateLabelsBorder();

	return 1;
}
static  uint8_t functChangePatternEditStep()
{
	PTE->selectedPlace = 3;
	PTE->activateLabelsBorder();

	return 1;
}

static  uint8_t functFill()
{
	if(PTE->fillState && PTE->selectedPlace!= 4) // tylko zaznacz przycisk kiedy popop jest wyswietlony
	{
		PTE->selectedPlace = 4;
		PTE->activateLabelsBorder();

		return 1;
	}

	PTE->fillState = !PTE->fillState;

	if(PTE->fillState)
	{
		PTE->showFillPopup();

		PTE->selectedPlace = 4;
		PTE->activateLabelsBorder();
	}
	else
	{
		PTE->hideFillPopup();


		PTE->selectedPlace = -1;
		PTE->activateLabelsBorder();
	}




	return 1;
}

static  uint8_t functRandom()
{
	PTE->selectedPlace = 5;
	PTE->activateLabelsBorder();

	return 1;
}

static  uint8_t functInvert()
{
	PTE->selectedPlace = 6;
	PTE->activateLabelsBorder();

	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{

	PTE->eventFunct(eventSwitchModule,PTE,&button,0);

	return 1;
}


//======================================================================================================================


