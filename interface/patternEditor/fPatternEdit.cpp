
#include "patternEditor/patternEditor.h"
#include "mtStructs.h"

#include "fileManager.h"
#include "mtAudioEngine.h"
#include "mtPadBoard.h"
#include "mtPadsBacklight.h"

#include "core/interfacePopups.h"
#include "core/songTimer.h"

#include "keyScanner.h"
#include "sdramTest.h"

#include "debugLog.h"


cPatternEditor patternEditor;
static  cPatternEditor* PTE = &patternEditor;

extern strMtProject mtProject;


// pod ekranem
static  uint8_t functChangePattern(uint8_t state);
static  uint8_t functChangePatternLength(uint8_t state);
static  uint8_t functChangePatternEditStep(uint8_t state);
static  uint8_t functRamTest(uint8_t state);
static  uint8_t functFill();
static  uint8_t functPreview();
static  uint8_t functTranspose(uint8_t state);
static  uint8_t functUndo(uint8_t state);
static  uint8_t functUndoRedo(int8_t dir);
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
static  uint8_t functFillAction1();
static  uint8_t functFillAction2();
static  uint8_t functFillAction3();
static  uint8_t functFillAction4();
static  uint8_t functFillAction5();
static  uint8_t functFillAction6();
// export selection
static 	uint8_t	functExportSelection();
static  uint8_t functAutoNameExport();
static  uint8_t functCancelExport();
static  uint8_t functConfirmExport();
static  uint8_t functConfirmExportLoad();
static  uint8_t functEnterKeyExport();
static 	uint8_t	functLeftKeyboard();
static 	uint8_t	functRightKeyboard();
static 	uint8_t	functUpKeyboard();
static 	uint8_t	functDownKeyboard();
static 	uint8_t	functDeleteKeyboard();
static 	uint8_t	functInsertKeyboard();

static 	uint8_t functOverwriteExportFile();
static 	uint8_t functCancelOverwriteExportFile();

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
static uint8_t functMuteTrack(uint8_t button, uint8_t state);

//--------------------------------------------
static uint8_t getSelectedElement();


char getHexFromInt(int16_t val, uint8_t index);




elapsedMillis patternRefreshTimer;

void cPatternEditor::update()
{
	// zarzadznie renderowaniem zaznaczenia ---------------------------
	uint8_t managerStatus = newFileManager.getStatus();

	if(managerStatus == fmExportSoundEnd)
	{
		newFileManager.clearStatus();

		endExportSelection();

		interfaceGlobals.refreshFileExplorer = true;
	}
	else if(managerStatus == fmImportSamplesEnd)
	{
		FM->unblockAllInputs();
		newFileManager.clearStatus();

		interfaceGlobals.refreshFileExplorer = true;

		uint8_t button = interfaceButtonSampleLoad;
		PTE->firstFreeInstrumentSlotFound ++;
		PTE->eventFunct(eventSwitchModule, PTE, &button, &PTE->firstFreeInstrumentSlotFound);
	}
	else if(managerStatus >= fmLoadEnd && managerStatus < fmError) // 	lapie cala reszte Endow
	{
		FM->unblockAllInputs();
		newFileManager.clearStatus();
		start(0);
	}
	else if(managerStatus >=  fmError) // a tu wszelakie errory
	{
		debugLog.addLine("File Manager Opretion Error (pattern)");

		FM->unblockAllInputs();
		newFileManager.clearStatus();
		start(0);
	}


	// cala reszta  wymagajaca odwiezania -----------------------------------
	if(patternRefreshTimer < 20) return;
	patternRefreshTimer = 0;

	if(refreshAfterReleaseFx == true)
	{
		refreshAfterReleaseFx = false;
		lightUpPadBoard();
	}

	// opozniania pokazywania step popupow
	for(uint8_t i = 0; i<4; i++)
	{
		if(mtPopups.getStepPopupState() == stepPopupNone && PTE->stepButtonsState[i] == 1 && PTE->stepButtonsTimer[i] > 180) // << wartosc w ms
		{
			switch(i)
			{
			case 0: showNotePopup(); 		break;
			case 1: showInstrumentPopup(); 	break;
			case 2: showFx1Popup(); 		break;
			case 3: showFx2Popup(); 		break;
			}
		}
	}

		showPattern();


	if(sequencer.isStop())
	{
		// jesli ostatnio bylo odtwarzane zaznaczenie to cofnij playhed na 0
		if(trackerPattern.playheadSelectMode == 1 || trackerPattern.playheadRecMode == 1)
		{
			trackerPattern.playheadSelectMode = 0;
			trackerPattern.playheadPosition = 0;
			trackerPattern.playheadRecMode = 0;
			refreshPattern();
		}
		return;
	}

	readPatternState();

//	showPattern();
	showLength();

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

	keyboardManager.init(keyboardControl,editName);
	trackerPattern.stepDevider = (mtConfig.general.patternDiv + 1);


	readPatternState();
	refreshPattern();



	setSwitchModuleFunct();
	//FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	refreshEditState();

	focusOnPattern();
}

void cPatternEditor::stop()
{
//	if(sequencer.getSeqState() != sequencer.SEQ_STATE_PLAY_PERFORMANCE)
//	{
//		newFileManager.savePattern(mtProject.values.actualPattern);
//
//	}

	undoRedoOpen = 0; // na wszelki

	keyboardManager.deinit();
	sequencer.stopManualNotes();
	sequencer.sequencialSwitch_Reset();

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
	FM->setButtonObj(interfaceButton6, functRamTest);
//	FM->setButtonObj(interfaceButton2, functChangePatternEditStep);


	//FM->setButtonObj(interfaceButton4, buttonPress, functFill);
	//FM->setButtonObj(interfaceButton5, buttonPress, functRandom);
	//FM->setButtonObj(interfaceButton6, buttonPress, functInvert);
	//FM->setButtonObj(interfaceButton7, buttonPress, functUndo);


	FM->setPotObj(interfacePot0, functEncoder, nullptr);


	FM->setPadsGlobal(functPads);

}

void cPatternEditor::setSwitchModuleFunct()
{
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
}

//==============================================================================================================
// przeniesienie danych z sekewncji  do struktury wyswietlania
void cPatternEditor::refreshPattern()
{
	seq = sequencer.getPatternToUI();
	trackerPattern.patternLength = seq->track[0].length+1;

	// odpowiednie wylaczenie generowania zalsanianych czesci trackow
	if(fillState)  trackerPattern.popupMode &= ~(2 | 4);
	else trackerPattern.popupMode = 0;

	if(editMode == 0)
	{
		trackerPattern.selectState = 1; // xxx zielona ramka mod

		if(!sequencer.isStop())
		{
			trackerPattern.actualStep = trackerPattern.playheadPosition;

			if(sequencer.isRec()) trackerPattern.selectState = 1;
				//trackerPattern.playheadRecMode = 1;
			//else 					trackerPattern.playheadRecMode = 0;
		}
	}

	if(sequencer.isPreview())
	{
		trackerPattern.playheadSelectMode = 1;
	}
	else trackerPattern.playheadSelectMode = 0;


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
					char inst0 = (seq->track[i].step[patternPosition - 7 + j].instrument - INSTRUMENTS_COUNT+1) / 10;
					char inst1 = (seq->track[i].step[patternPosition - 7 + j].instrument - INSTRUMENTS_COUNT+1) % 10;

					trackerPattern.track[i].row[j].instr[0] = 'M';
					trackerPattern.track[i].row[j].instr[1] = inst0 + 48;
					trackerPattern.track[i].row[j].instr[2] = inst1 + 48;
					trackerPattern.track[i].row[j].instr[3] = 0;
				}
				else // normal instr
				{
					char inst0 = (seq->track[i].step[patternPosition - 7 + j].instrument + 1) / 10;
					char inst1 = (seq->track[i].step[patternPosition - 7 + j].instrument + 1) % 10;

					trackerPattern.track[i].row[j].instr[0] = inst0 + 48;
					trackerPattern.track[i].row[j].instr[1] = inst1 + 48;
					trackerPattern.track[i].row[j].instr[2] = 0;
					trackerPattern.track[i].row[j].instr[3] = 0;
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

//				int16_t val = sequencer.getFxValueToView(
//						1, i, patternPosition - 7 + j);
//
//				if (val >= 0)
//				{
//					sprintf(&trackerPattern.track[i].row[j].fx[0][1],
//							"%.3i",
//							val
//							);
//				}
//				else
//				{
//					sprintf(&trackerPattern.track[i].row[j].fx[0][1],
//							"%.2i",
//							val
//							);
//				}

				sequencer.makeFxValLabel(
						&trackerPattern.track[i].row[j].fx[0][1],
						1,
						i,
						patternPosition - 7 + j);


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

//				sprintf(&trackerPattern.track[i].row[j].fx[1][1],
//						"%.3u",
//						sequencer.getFxValueToView(0,i,patternPosition - 7 + j));

				sequencer.makeFxValLabel(
						&trackerPattern.track[i].row[j].fx[1][1],
						0,
						i,
						patternPosition - 7 + j);

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


void cPatternEditor::showFxInfo()
{
	if(editParam < 2) return;
	if(editMode == 0) return;

	debugLog.setMaxLineCount(1);

	seq = sequencer.getPatternToUI();
	uint8_t type_temp  = interfaceGlobals.fxIdToName(seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[1-(editParam-2)].type);
	if(type_temp > 0 && type_temp < FX_MAX)
	{
		debugLog.addLine(&interfaceGlobals.fxNames[type_temp][0]);
	}
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

	if(fx_type < FX_COUNT-FX_COUNT_HIDDEN_FXes) selectedFx = fx_type;

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
	moveCursorByStep(0);
}

void cPatternEditor::moveCursorByStep(uint8_t val)
{
	if(mtProject.values.patternEditStep + val <= 0 ) return;

	int16_t patternLength  = sequencer.getPatternToUI()->track[0].length;

	if(trackerPattern.actualStep + mtProject.values.patternEditStep + val <= patternLength)
	{
		trackerPattern.actualStep += mtProject.values.patternEditStep + val;
	}
	else
	{
		trackerPattern.actualStep = mtProject.values.patternEditStep  + val - ((patternLength+1)-trackerPattern.actualStep);
	}

	PTE->trackerPattern.selectState = 1;
	PTE->trackerPattern.selectColumn = 0;
	PTE->isSelectingNow = 0;

}

void cPatternEditor::moveCursorByStepParallel( )
{
	if (mtProject.values.patternEditStep <= 0) return;

	int16_t patternLength = sequencer.getPatternToUI()->track[0].length;

	if (trackerPattern.actualStep + mtProject.values.patternEditStep <= patternLength)
	{
		trackerPattern.actualStep += mtProject.values.patternEditStep;
	}
	else
	{
		trackerPattern.actualStep = mtProject.values.patternEditStep - ((patternLength + 1) - trackerPattern.actualStep);
	}
	PTE->trackerPattern.selectStartStep = trackerPattern.actualStep;
	PTE->trackerPattern.selectEndStep = trackerPattern.actualStep;

	PTE->trackerPattern.selectState = 2;
	PTE->trackerPattern.selectColumn = 0;
	PTE->isSelectingNow = 0;

}


void cPatternEditor::cancelPopups()
{
	uint8_t popup_type = mtPopups.getStepPopupState();
	if(popup_type != stepPopupNone)
	{


		if(PTE->editMode == 1 && !insertOnPopupHideDisabled)
		{
			switch (popup_type)
			{
			case stepPopupNote:
				if (!isMultiSelection())
				{
					newFileManager.storePatternUndoRevision();
					sendSelection();
					sequencer.setSelectionNote(mtProject.values.lastUsedNote);
				}
				break;
			case stepPopupFx:
				if (!isMultiSelection())
				{
					uint8_t fx_index = PTE->editParam == 2 ? 1 : 0;

					//uint8_t fx_name = interfaceGlobals.fxIdToName(
					//		sequencer.getPatternToUI()->track[trackerPattern.actualTrack].step[trackerPattern.actualStep].fx[fx_index].type);
					//if (fx_name > 0 && fx_name < FX_COUNT-FX_COUNT_HIDDEN_FXes) // tylko jesli na stepie jest juz jakis fx
					//{
						newFileManager.storePatternUndoRevision();
						sendSelection();
						sequencer.setSelectionFxType(
								fx_index,
								interfaceGlobals.fxIDs[mtProject.values.lastUsedFx]);
					//}
				}
				break;
			case stepPopupInstr:
				if (!isMultiSelection())
				{
					newFileManager.storePatternUndoRevision();
					sendSelection();
					sequencer.setSelectionInstrument(mtProject.values.lastUsedInstrument);
				}
				break;

			default:
				break;
			}
		}

		mtPopups.hideStepPopups();
		setDefaultScreenFunct();
		showDefaultScreen();
		refreshEditState();

		// schowaj w przypadku wychodzenia z note popup
		songTimer.hide();

		PTE->trackerPattern.popupMode  &= ~(2 | 4);

		if(fillState)
		{
			if(popup_type == stepPopupInstr)
			{
				 if(editParam == 1)
				 {
					if (PTE->fillPlace == 2) 		PTE->fillData[1].from = mtProject.values.lastUsedInstrument;
					else if (PTE->fillPlace == 3) 	PTE->fillData[1].to = mtProject.values.lastUsedInstrument;
				 }
			}
			functFill();
		}

		refreshPattern();
	}
}

void cPatternEditor::setKeyboardExportFunctions()
{
	//funkcje
	FM->clearAllButtons();
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeftKeyboard);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRightKeyboard);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUpKeyboard);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDownKeyboard);

	FM->setButtonObj(interfaceButton0,buttonPress, functEnterKeyExport);
	FM->setButtonObj(interfaceButton4,buttonPress, functAutoNameExport);
	FM->setButtonObj(interfaceButton5,buttonPress, functCancelExport);
	FM->setButtonObj(interfaceButton6,buttonPress, functConfirmExportLoad);
	FM->setButtonObj(interfaceButton7,buttonPress, functConfirmExport);

	FM->setButtonObj(interfaceButtonDelete,buttonPress, functDeleteKeyboard);
	FM->setButtonObj(interfaceButtonInsert,buttonPress, functInsertKeyboard);

}

void cPatternEditor::setOverwriteRenderFunct()
{
	FM->clearButton(interfaceButton6);
	FM->clearButton(interfaceButton7);

	FM->setButtonObj(interfaceButton6,buttonPress, functCancelOverwriteExportFile);
	FM->setButtonObj(interfaceButton7,buttonPress, functOverwriteExportFile);

	FM->blockAllInputsExcept(interfaceButton6,interfaceButton7);
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




//void cPatternEditor::changeActualTempo(int16_t value)
//{
//	Sequencer::strPattern * pattern = sequencer.getPatternToUI();
//
//	if(pattern->tempo+value < 10) pattern->tempo = 10;
//	else if(pattern->tempo+value > 1000) pattern->tempo = 400;
//	else  pattern->tempo += value;
//
////	showTempo();
//}

void cPatternEditor::changeActualPattern(int16_t value)
{
	sequencer.sequencialSwitch_Reset();

	if(sequencer.getSeqState() != sequencer.SEQ_STATE_PLAY_PERFORMANCE)
	{
		newFileManager.saveWorkspacePatternNow(mtProject.values.actualPattern);
	}

	mtProject.values.actualPattern = constrain(mtProject.values.actualPattern + value,
											   PATTERN_INDEX_MIN, PATTERN_INDEX_MAX);

	newFileManager.loadWorkspacePatternNow(mtProject.values.actualPattern);
	sequencer.switchRamPatternsNow();

	readPatternState();
	refreshPatternParams();
	refreshPattern();

}
void cPatternEditor::setActualPattern(int16_t value)
{
	if (sequencer.getSeqState() != sequencer.SEQ_STATE_PLAY_PERFORMANCE)
	{
		newFileManager.saveWorkspacePatternNow(mtProject.values.actualPattern);
	}

	mtProject.values.actualPattern = constrain( value,
												PATTERN_INDEX_MIN,
												PATTERN_INDEX_MAX);

	newFileManager.loadWorkspacePatternNow(mtProject.values.actualPattern);
	sequencer.switchRamPatternsNow();

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

	newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
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

	newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
}

void cPatternEditor::changeActualPatternEditStep(int16_t value)
{
	mtProject.values.patternEditStep = constrain(
			mtProject.values.patternEditStep + value,
			0,
			PATTERN_EDIT_STEP_MAX);

	showStep();

	newFileManager.setProjectStructChanged();
	//newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
}

void cPatternEditor::setActualPatternEditStep(int16_t value)
{

	mtProject.values.patternEditStep = constrain(value,
													0,
													PATTERN_EDIT_STEP_MAX);

	showStep();
}

//======================================================================================================================
static uint8_t functMuteTrack(uint8_t button, uint8_t state)
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
		trackerPattern.playheadRecMode = 0;

		focusOnActual();

		focusOnPattern();

		showEditModeLabels();

		FM->setButtonObj(interfaceButton3, buttonPress, functFill);
		FM->setButtonObj(interfaceButton4, buttonPress, functPreview);
		FM->setButtonObj(interfaceButton5, buttonPress, functInvert);
		FM->setButtonObj(interfaceButton6, buttonPress, functExportSelection);
		FM->setButtonObj(interfaceButton7, functUndo);

		lightUpPadBoard();
	}
	else
	{
		trackerPattern.selectState = 1; //xxx zielona ramka mod
		focusOnPattern();

		hideEditModeLabels();

		FM->clearButtonsRange(interfaceButton3, interfaceButton7);
		FM->setButtonObj(interfaceButton4, buttonPress, functPreview);

		padsBacklight.clearAllPads(0, 1, 0);

	}


	display.refreshControl(patternControl);

}

void cPatternEditor::changeFillData(int16_t value)
{
	if(fillPlace < 0 && fillPlace > 5) return;

	int16_t* ptrVal;
	int16_t min = 0, max;
	uint8_t fillParam = fillPlace;

	if(editParam != 1)
	{
		switch(fillPlace)
		{
		case 1: fillParam = 4; break;// param
		case 2: fillParam = 4; break;// param
		case 3: fillParam = 1; break;// type
		case 4: fillParam = 2; break;// from
		case 5: fillParam = 3; break;// to
		}
	}

	switch(fillParam)
	{
	case 0: // step
		ptrVal = &fillStep;
		min = -4;
		max = PATTERN_EDIT_STEP_MAX;
		break;
	case 1: // type
		ptrVal = &fillData[editParam].type;
		max = fillTypeList.length - 1;
		break;
	case 2: // from
		ptrVal = &fillData[editParam].from;
		min = (editParam < 2 ? 0 : sequencer.getFxMin(interfaceGlobals.fxIDs[fillData[editParam].param]));
		max = (editParam == 0 ? 127 : (editParam == 1 ? 63 : sequencer.getFxMax(interfaceGlobals.fxIDs[fillData[editParam].param])));
		break;
	case 3: // to
		if(fillData[editParam].type == 0) return;
		ptrVal = &fillData[editParam].to;
		min = (editParam < 2 ? 0 : sequencer.getFxMin(interfaceGlobals.fxIDs[fillData[editParam].param]));
		max = (editParam == 0 ? 127 : (editParam == 1 ? 63 : sequencer.getFxMax(interfaceGlobals.fxIDs[fillData[editParam].param])));
		break;
	case 4: // param
		ptrVal = &fillData[editParam].param;
		min = (editParam == 0 ? 0 : -1);
		max = (editParam == 0 ? MAX_SCALE : FX_COUNT-1-FX_COUNT_HIDDEN_FXes);
		break;

	default: return;

	}

	if(*ptrVal + value < min) *ptrVal = min;
	else if(*ptrVal + value > max) *ptrVal = max;
	else *ptrVal += value;

	switch(fillParam)
	{
		case 0:  refreshFillStep(); break;
		case 1:  refreshFillType(); break;
		case 2:  refreshFillFrom(); break;
		case 3:  refreshFillTo(); 	break;
		case 4:  refreshFillParam(); break;
		default: return;
	}
}

void cPatternEditor::changneFillDataByPad(uint8_t pad)
{
	uint8_t fillParam = fillPlace;

	if(editParam != 1)
	{
		switch(fillPlace)
		{
		case 1: fillParam = 4; break;// param
		case 2: fillParam = 4; break;// param
		case 3: fillParam = 1; break;// type
		case 4: fillParam = 2; break;// from
		case 5: fillParam = 3; break;// to
		}
	}

	switch(fillParam)
	{
	case 0: // step
	{
		if(pad-4 < PATTERN_EDIT_STEP_MAX) fillStep = pad-4;
		break;
	}
	case 1: //type
	{
		if(pad < 3) fillData[editParam].type = pad;
		break;
	}
	case 2: // from
	{
		if(editParam == 0) 		fillData[editParam].from = mtPadBoard.getNoteFromPad(pad);
		else if(editParam == 1) fillData[editParam].from = pad;
		else
		{
			int16_t min_fx = sequencer.getFxMin(interfaceGlobals.fxIDs[fillData[editParam].param]);
			int16_t max_fx = sequencer.getFxMax(interfaceGlobals.fxIDs[fillData[editParam].param]);

			fillData[editParam].from = map(pad,0,47,min_fx,max_fx);

			if(fillData[editParam].from < min_fx) fillData[editParam].from = min_fx;
			if(fillData[editParam].from > max_fx) fillData[editParam].from = max_fx;
		}
		break;
	}
	case 3: // to
	{
		if(editParam == 0) 		fillData[editParam].to = mtPadBoard.getNoteFromPad(pad);
		else if(editParam == 1) fillData[editParam].to = pad;
		else
		{
			int16_t min_fx = sequencer.getFxMin(interfaceGlobals.fxIDs[fillData[editParam].param]);
			int16_t max_fx = sequencer.getFxMax(interfaceGlobals.fxIDs[fillData[editParam].param]);

			fillData[editParam].to = map(pad,0,47,min_fx,max_fx);

			if(fillData[editParam].to < min_fx) fillData[editParam].to = min_fx;
			if(fillData[editParam].to > max_fx) fillData[editParam].to = max_fx;
		}
		break;
	}
	case 4: // param
	{
		if(editParam == 0)
		{
			if(pad < MAX_SCALE) fillData[editParam].param = pad;
		}
		else if(editParam >= 2)
		{
			if(pad < FX_COUNT-FX_COUNT_HIDDEN_FXes) fillData[editParam].param = pad;
		}
		break;
	}
	default: break;
	}

	switch(fillParam)
	{
		case 0:  refreshFillStep(); break;
		case 1:  refreshFillType(); break;
		case 2:  refreshFillFrom(); break;
		case 3:  refreshFillTo(); 	break;
		case 4:  refreshFillParam(); break;
		default: return;
	}
}


void cPatternEditor::setFillPlace(uint8_t place, int8_t dir)
{
	if(place < 0 || place > 5) return;

	if(editParam == 0)
	{
		if(dir == 0)
		{
			if(fillPlace == 1 && (place == 1 || place == 2))
			{
				changeFillData(place == 1 ? -1 : 1);
			}
			else
			{
				if(place == 2) fillPlace = 1;
				else  fillPlace = place;
			}
			return;
		}
		else if(dir > 0)
		{
			if((fillData[editParam].type == 0) && (place > 4)) fillPlace = 4;
			else if(place == 2) fillPlace = 3;
			else  fillPlace = place;
		}
		else if(dir < 0)
		{
			if(place == 2) fillPlace = 1;
			else  fillPlace = place;
		}
	}
	else if(editParam == 1)
	{
		if(dir == 0)
		{
			if(place < 4) fillPlace = place;
			return;
		}
		else if(dir > 0)
		{
			if((fillData[editParam].type == 0) && (place > 2)) fillPlace = 2;
			else if(place > 3) fillPlace = 3;
			else  fillPlace = place;
		}
		else if(dir < 0)
		{
			if(place > 3) fillPlace = 3;
			else  fillPlace = place;
		}
	}
	else if(editParam == 2 || editParam == 3)
	{
		if(dir == 0)
		{
			if(fillPlace == 1 && (place == 1 || place == 2))
			{
				changeFillData(place == 1 ? -1 : 1);
			}
			else
			{
				if(place == 2) fillPlace = 1;
				else  fillPlace = place;
			}
			return;
		}
		else if(dir > 0)
		{
			if((fillData[editParam].type == 0) && (place > 4)) fillPlace = 4;
			else if(place == 2) fillPlace = 3;
			else  fillPlace = place;
		}
		else if(dir < 0)
		{
			if(place == 2) fillPlace = 1;
			else  fillPlace = place;
		}
	}
}

void cPatternEditor::changeFillPlace(int8_t diff)
{
	setFillPlace(fillPlace+diff, diff);
}


void cPatternEditor::refreshFillPlace()
{
	if(editParam == 0 || editParam == 2 || editParam == 3)
	{
		if(fillPlace == 2) fillPlace = 1;
	}
}

void cPatternEditor::setMuteFunct(uint8_t state)
{
	if(state == 0)
	{
		FM->clearButtonsRange(interfaceButton0,interfaceButton7);
		FM->setButtonObj(interfaceButton0, functChangePattern);
		FM->setButtonObj(interfaceButton1, functChangePatternLength);
		FM->setButtonObj(interfaceButton2, functChangePatternEditStep);

		FM->setButtonObj(interfaceButton4, buttonPress, functPreview);

		if(editMode)
		{
			FM->setButtonObj(interfaceButton3, buttonPress, functFill);
			FM->setButtonObj(interfaceButton5, buttonPress, functInvert);
			FM->setButtonObj(interfaceButton6, buttonPress, functExportSelection);
			FM->setButtonObj(interfaceButton7, functUndo);
		}
	}
	else
	{
		FM->setButtonObj(interfaceButton0, functMuteTrack);
		FM->setButtonObj(interfaceButton1, functMuteTrack);
		FM->setButtonObj(interfaceButton2, functMuteTrack);
		FM->setButtonObj(interfaceButton3, functMuteTrack);
		FM->setButtonObj(interfaceButton4, functMuteTrack);
		FM->setButtonObj(interfaceButton5, functMuteTrack);
		FM->setButtonObj(interfaceButton6, functMuteTrack);
		FM->setButtonObj(interfaceButton7, functMuteTrack);

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
	newFileManager.setProjectStructChanged();
//	mtProject.values.projectNotSavedFlag = 1;
//	newFileManager.projectChangeFlag = 1;
}

void cPatternEditor::startExportSelection()
{
	char localPatch[PATCH_SIZE];

//	sprintf(localPatch, "Export/%s/Selection/%s.wav",newFileManager.getCurrentProjectName(), PTE->keyboardManager.getName());
//	exporter.start(localPatch, exportRenderSelection);
//	PTE->showExportProgress();

	keyboardManager.deactivateKeyboard();

	if(newFileManager.exportSoundRenderSelection(PTE->keyboardManager.getName()))
	{
		//PE->FM->setButtonObj(interfaceButton7, buttonPress, functExportCancel);
		//PE->FM->blockAllInputsExcept(interfaceButton7);
		PTE->FM->blockAllInputs();

	}


}
void cPatternEditor::endExportSelection()
{
	//hideExportProgress();
	//showDefaultScreen();
	//setDefaultScreenFunct();
	//refreshEditState();
	//setSwitchModuleFunct();

	if(isLoadAfterSave)
	{
		isLoadAfterSave = false;

		// sprawdz czy wystarczy pamieci
		if((mtProject.used_memory + exporter.getRenderLength() * 2) > mtProject.max_memory)
		{
			showFullMemoryInBank();
		}
		else
		{
			// sprawdz czy jest wolny slot instrumentu
			PTE->firstFreeInstrumentSlotFound = -1;
			for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++ )
			{
				if(!mtProject.instrument[i].isActive)
				{
					PTE->firstFreeInstrumentSlotFound = i;
					//mtProject.values.lastUsedInstrument = i;
					break;
				}
			}

			if(PTE->firstFreeInstrumentSlotFound == -1)
			{
				showFullInstrumentInBank();
			}
			else
			{
				// dopeiro teraz importuj
				char localPatch[255];
				sprintf(localPatch, "Export/%s/Selection", newFileManager.getCurrentProjectName());
				if(newFileManager.importSampleToProject(localPatch, PTE->keyboardManager.getName(), PTE->firstFreeInstrumentSlotFound))
				{
					return;
				}
			}
		}
	}

	// jesli nei importuje albo blad to oodblokuj modul i zakoncz
	FM->unblockAllInputs();
	start(0);
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

		switch(PTE->selectedPlace)
		{
		//case 0: PTE->changeActualTempo(value); break;
		case 0: PTE->changeActualPattern(value); break;
		case 1: PTE->changeActualPatternLength(value); break;
		case 2: PTE->changeActualPatternEditStep(value); break;
		case 7: functUndoRedo(value); 						break;
		}

		PTE->lightUpPadBoard();

		return 1;
	}

	if(PTE->editMode == 0)
	{
		if(!sequencer.isStop()) return 1;
		// przesuwanie lewo-prawo
		//if(PTE->trackerPattern.actualTrack + value < 0) PTE->trackerPattern.actualTrack = 0;
		//else if(PTE->trackerPattern.actualTrack + value > 7) PTE->trackerPattern.actualTrack = 7;
		//else  PTE->trackerPattern.actualTrack += value;

		// przesuwanie gora-dol
		if(PTE->trackerPattern.actualStep + value < 0) PTE->trackerPattern.actualStep = 0;
		else if(PTE->trackerPattern.actualStep + value > PTE->trackerPattern.patternLength-1) PTE->trackerPattern.actualStep = PTE->trackerPattern.patternLength-1;
		else  PTE->trackerPattern.actualStep += value;

		PTE->focusOnActual();
		PTE->refreshPattern();

		return 1;
	}

	//czy parametr widoczny jesli np. pokazywane tylko 2 parametry 8 trackow
	if(PTE->patternViewMode > 0 && !(PTE->patternViewMode & (1 << PTE->editParam))) return 1;


	sendSelection();
//	if(tactButtons.isButtonPressed(interfaceButtonShift) || !isMultiSelection())
	if(1)
	{
		newFileManager.storePatternUndoRevision();
		switch(PTE->editParam)
		{
		case 0: sequencer.changeSelectionNote(value); break;
		case 1: sequencer.changeSelectionInstrument(value); break;
		case 2:
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

		newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
	}

	PTE->lightUpPadBoard();
	PTE->refreshPattern();

	return 1;
}


static  uint8_t functShift(uint8_t state)
{
	if(PTE->fillState) return 1;

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

	if (PTE->selectedPlace >= 0 && PTE->selectedPlace < 8)
	{
		switch (PTE->selectedPlace)
		{
		case 0:
			sequencer.sequencialSwitch_changeNextPattern(-1);
			return 1;
		case 7:
			functUndoRedo(-1);
			return 1;
		}
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
		//if(PTE->patternViewMode == 0 && PTE->trackerPattern.firstVisibleTrack > 0 ) PTE->trackerPattern.firstVisibleTrack--;

		if(PTE->trackerPattern.actualTrack > 0) PTE->trackerPattern.actualTrack--;
		PTE->focusOnActual();
	}

	PTE->showFxInfo();


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

	if (PTE->selectedPlace >= 0 && PTE->selectedPlace < 8)
	{
		switch (PTE->selectedPlace)
		{
		case 0:
			sequencer.sequencialSwitch_changeNextPattern(1);
			return 1;
		case 7:
			functUndoRedo(1);
			return 1;
		}
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
		//if(PTE->patternViewMode == 0 && PTE->trackerPattern.firstVisibleTrack < 4 ) PTE->trackerPattern.firstVisibleTrack++;

		if(PTE->trackerPattern.actualTrack < 7) PTE->trackerPattern.actualTrack++;
		PTE->focusOnActual();
	}


	PTE->showFxInfo();

	PTE->lightUpPadBoard();
	display.refreshControl(PTE->patternControl);

	return 1;
}


static  uint8_t functUp()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillData( (PTE->fillPlace == 0 || PTE->fillPlace == 1 ||
							 (PTE->editParam != 1 && PTE->fillPlace == 3))  ? -1 : 1);
		return 1;
	}

	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
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

	if (tactButtons.isButtonPressed(interfaceButtonPattern))
	{
		PTE->changeActualPattern(1);
		return 1;
	}


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


	PTE->showFxInfo();

	PTE->refreshPattern();
	PTE->lightUpPadBoard();

	return 1;
}


static  uint8_t functDown()
{
	if(PTE->fillState > 0)
	{
		PTE->changeFillData( (PTE->fillPlace == 0 || PTE->fillPlace == 1 ||
							 (PTE->editParam != 1 && PTE->fillPlace == 3))  ? 1 : -1);
		return 1;
	}

	if(	PTE->selectedPlace >= 0 &&  PTE->selectedPlace < 8)
	{
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


	if (tactButtons.isButtonPressed(interfaceButtonPattern))
	{
		PTE->changeActualPattern(-1);
		return 1;
	}

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


	PTE->showFxInfo();

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
		PTE->setStepButtonState(0,1);
//		PTE->wasNotesEditBefore = 0;
		PTE->previousEditParam = PTE->editParam;
		PTE->editParam = 0;
		PTE->trackerPattern.selectedParam = 0;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(1);
		}
		else PTE->changePatternViewMode(1);

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			PTE->trackerPattern.popupMode |= 1;
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonHold)
	{

	}
	else if(state == buttonRelease)
	{
		PTE->setStepButtonState(0,0);
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
		PTE->setStepButtonState(1,1);
//		if(PTE->editParam  == 0) PTE->wasNotesEditBefore = 1;
		PTE->previousEditParam = PTE->editParam;
		PTE->editParam = 1;
		PTE->trackerPattern.selectedParam = 1;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(2);
		}
		else PTE->changePatternViewMode(2);

		if(PTE->fillState > 0)
		{
//			PTE->wasNotesEditBefore = 0;
			PTE->showFillPopup();
			PTE->trackerPattern.popupMode |= 1;
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonHold)
	{
	}
	else if(state == buttonRelease)
	{
		PTE->setStepButtonState(1,0);
		// powrot do nuty po wybraniu instrumentu
//		if(PTE->wasNotesEditBefore && mtPopups.getStepPopupState() != stepPopupNone)
//		{
//			PTE->wasNotesEditBefore = 0;
//			PTE->editParam = 0;
//			PTE->trackerPattern.selectedParam = 0;
//			display.refreshControl(PTE->patternControl);
//
//			PTE->focusOnPattern();
//			PTE->lightUpPadBoard();
//		}

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
		PTE->setStepButtonState(2,1);
//		PTE->wasNotesEditBefore = 0;
		PTE->previousEditParam = PTE->editParam;
		PTE->editParam = 2;
		PTE->trackerPattern.selectedParam = 2;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(3);
		}
		else PTE->changePatternViewMode(3);

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			PTE->trackerPattern.popupMode |= 1;
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonHold)
	{
	}
	else if(state == buttonRelease)
	{
		PTE->setStepButtonState(2,0);
		PTE->cancelPopups();
		PTE->dontShowPopupsUntilButtonRelease = 0;

		PTE->showFxInfo();
		PTE->refreshAfterReleaseFx = true;
	}

	return 1;
}

//-----------------------------------------------------------------------------------
static  uint8_t functFx2(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->setStepButtonState(3,1);
//		PTE->wasNotesEditBefore = 0;
		PTE->previousEditParam = PTE->editParam;
		PTE->editParam = 3;
		PTE->trackerPattern.selectedParam = 3;
		display.refreshControl(PTE->patternControl);

		PTE->cancelPopups();

		if(tactButtons.isButtonPressed(interfaceButtonPattern)) //zmiana widoku na 8 trackow
		{
			PTE->setPatternViewMode(4);
		}
		else PTE->changePatternViewMode(4);

		if(PTE->fillState > 0)
		{
			PTE->showFillPopup();
			PTE->trackerPattern.popupMode |= 1;
			return 1;
		}

		PTE->focusOnPattern();
		PTE->lightUpPadBoard();
	}
	else if(state == buttonHold)
	{
	}
	else if(state == buttonRelease)
	{
		PTE->setStepButtonState(3,0);
		PTE->cancelPopups();
		PTE->dontShowPopupsUntilButtonRelease = 0;

		PTE->showFxInfo();
		PTE->refreshAfterReleaseFx = true;
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

			PTE->refreshEditState();
		}
		else if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playPattern(getActualStep());
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

		//newFileManager.autoSaveWorkspace(1);
	}

	PTE->focusOnPattern();

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
	if (state == buttonPress || state == buttonHold)
	{
		if(tactButtons.isButtonPressed(interfaceButtonShift))
		{
			// HOME
			if (isMultiSelection())
			{
				PTE->trackerPattern.selectColumn = 0;
				PTE->trackerPattern.selectState = 1;
				PTE->isSelectingNow = 0;

				PTE->trackerPattern.selectStartStep = 0;
				PTE->trackerPattern.selectEndStep = 0;
				PTE->trackerPattern.selectStartTrack = PTE->trackerPattern.actualTrack;
				PTE->trackerPattern.selectEndTrack = PTE->trackerPattern.actualTrack;

				PTE->trackerPattern.actualStep = 0; // zmiana pozycji kursora
			}
			else
			{
				PTE->trackerPattern.actualStep = 0; // zmiana pozycji kursora
			}
		}
		else
		{
			if(PTE->editMode == 1)
			{
				// INSERT
				newFileManager.storePatternUndoRevision();

				sendSelection();
				sequencer.insert(&sequencer.selection);

				newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
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
			newFileManager.storePatternUndoRevision();

			if (tactButtons.isButtonPressed(interfaceButtonShift))
			{
				sendPasteSelection();
				sequencer.pasteFromBuffer(getSelectedElement());
				PTE->moveCursorByStep(sequencer.getCopySelectionHeight()-1);

				newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
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
	if (state == buttonPress || state == buttonHold)
	{
		if (PTE->editMode == 1)
		{
			newFileManager.storePatternUndoRevision();

			// backspace
			if (tactButtons.isButtonPressed(interfaceButtonShift))
			{
				if(state == buttonPress)
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
			}
			// DELETE
			else
			{
				// wykomentowane bo chyba juz nie aktualne a robi problem
//				PTE->dontShowPopupsUntilButtonRelease = 1;	// blokuje ponowne pojawianie sie popupu
															// jesli ciagle wcisniety przycisk popupu
															// do czasu release note...fx
//				if(mtPopups.getStepPopupState() != stepPopupNone) // ukrywa popup nuta...fx jesli jest wyswietlany
//				{
//					PTE->insertOnPopupHideDisabled = 1;  // dezaktywuje wstawianie danych na wyjecie z popupow
//					PTE->cancelPopups();
//					PTE->insertOnPopupHideDisabled = 0;  // a tu aktywuje spowrotem
//				}

				sendSelection();
				if (PTE->editParam == 3 )
				{
					sequencer.clearSelected(Sequencer::ELEMENTS_FX1);
				}
				else if (PTE->editParam == 2 )
				{
					sequencer.clearSelected(Sequencer::ELEMENTS_FX2);
				}
				else
				{
					sequencer.clearSelected(getSelectedElement());
				}
				PTE->shiftAction = 1;
				PTE->moveCursorByStep();
			}

			newFileManager.setPatternStructChanged(mtProject.values.actualPattern);

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
uint8_t isMultiSelectionOnOneLine()
{
	if (isMultiSelection() && PTE->trackerPattern.selectStartStep == PTE->trackerPattern.selectEndStep)
	{
		return 1;
	}
	return 0;
}
int16_t getActualStep()
{
	return PTE->trackerPattern.actualStep;
}
int16_t getActualTrack()
{
	return PTE->trackerPattern.actualTrack;
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
		display.synchronizeRefresh();
	}
	else if(state == buttonRelease)
	{
		if(PTE->selectedPlace == 0)
		{
			PTE->focusOnPattern();
		}

		if (sequencer.sequencialSwitch_GetNext() > 0)
		{
			sequencer.sequencialSwitch_SetReady();
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
		display.synchronizeRefresh();
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
		display.synchronizeRefresh();
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

static  uint8_t functRamTest(uint8_t state)
{
	if(state == buttonPress)
	{
//		sdramTester.test();
		engine.setPassEnvelope(1);
	}
	else if(state == buttonRelease)
	{
		engine.setPassEnvelope(0);
	}

	return 1;
}
//##############################################################################################
//###############################             PREVIEW          #################################
//##############################################################################################

static  uint8_t functPreview()
{
	PTE->refreshEditState();

	sendSelection();
	sequencer.playSelection();

	return 1;
}

//##############################################################################################
//###############################             FILL             #################################
//##############################################################################################


static  uint8_t functFill()
{
	PTE->fillState = 1;
	PTE->fillButton7ReleaseBlocker = 1;
	PTE->trackerPattern.popupMode |= 1;
	PTE->focusOnPattern();

	PTE->showFillPopup();


	PTE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	PTE->FM->setButtonObj(interfaceButton6, buttonPress, functFillCancel);
	PTE->FM->setButtonObj(interfaceButton7, buttonPress, functFillApply);


	PTE->FM->setButtonObj(interfaceButton0, buttonPress, functFillAction1);
	PTE->FM->setButtonObj(interfaceButton1, buttonPress, functFillAction2);
	PTE->FM->setButtonObj(interfaceButton2, buttonPress, functFillAction3);
	PTE->FM->setButtonObj(interfaceButton3, buttonPress, functFillAction4);
	PTE->FM->setButtonObj(interfaceButton4, buttonPress, functFillAction5);
	PTE->FM->setButtonObj(interfaceButton5, buttonPress, functFillAction6);


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
		newFileManager.storePatternUndoRevision();
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
		//newFileManager.storePatternUndoRevision(); // xxx chyba tu nie potrzebne wiec wykomentowane
		newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
	}


	return 1;
}

static  uint8_t functFillAction1()
{
	//PTE->fillPlace = 0;
	PTE->setFillPlace(0);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillAction2()
{
	//PTE->fillPlace = 1;
	PTE->setFillPlace(1);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillAction3()
{
	//PTE->fillPlace = 2;
	PTE->setFillPlace(2);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillAction4()
{
	//PTE->fillPlace = 3;
	PTE->setFillPlace(3);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillAction5()
{
	//PTE->fillPlace = 5;
	PTE->setFillPlace(4);
	PTE->activateFillPopupBorder();

	return 1;
}

static  uint8_t functFillAction6()
{
	//PTE->fillPlace = 5;
	PTE->setFillPlace(5);
	PTE->activateFillPopupBorder();

	return 1;
}
//##############################################################################################
//###############################       EXPORT SELECTION       #################################
//##############################################################################################
//Eksport do pliku zaznaczenia - wywolanie klawiatury i podpiecie funkcji klawiatury i exportu
static 	uint8_t	functExportSelection()
{
	PTE->setKeyboardExportFunctions();
	char localPatch[128];
	uint16_t cnt = 1;
	char keyboardName[MAX_NAME_LENGTH];

	do
	{
		sprintf(keyboardName, "Selection%04d", cnt);
		sprintf(localPatch, "Export/%s/Selection/%s.wav",newFileManager.getCurrentProjectName(), keyboardName);
		cnt++;
		if(cnt > 9999)
		{
			break;
		}
	} while(SD.exists(localPatch));


	PTE->keyboardManager.fillName(keyboardName);
	PTE->keyboardManager.activateKeyboard();
	PTE->showKeyboardExport();
	return 1;
}
//Auto name klawiatury exportu
static  uint8_t functAutoNameExport()
{
	PTE->keyboardManager.setRandomName();
	return 1;
}
//Cancel Export - wraca do podstawowego okna
static  uint8_t functCancelExport()
{
	PTE->setDefaultScreenFunct();
	PTE->setSwitchModuleFunct();
	PTE->refreshEditState();
	PTE->hideKeyboardExport();
	PTE->keyboardManager.deactivateKeyboard();
	return 1;
}
//Zatwierdzenie nazwy - rozpoczęcie exportu
static  uint8_t functConfirmExport()
{
	char localPatch[255];

	sprintf(localPatch, "Export/%s/Selection",newFileManager.getCurrentProjectName());

	if(!SD.exists(localPatch)) SD.mkdir(0, localPatch);

	sprintf(localPatch, "Export/%s/Selection/%s.wav",newFileManager.getCurrentProjectName(), PTE->keyboardManager.getName());
	if(SD.exists(localPatch))
	{
		PTE->setOverwriteRenderFunct();
		PTE->showOverwriteExportDialog();
	}
	else
	{
		PTE->startExportSelection();
	}

	return 1;
}
//Zatwierdzenie nazwy - rozpoczęcie exportu - po nim zaladowanie instrumentu
static  uint8_t functConfirmExportLoad()
{
	functConfirmExport();
	PTE->isLoadAfterSave = true;
	return 1;
}
//Zatwierdzenie znaku w klawiaturze
static  uint8_t functEnterKeyExport()
{
	PTE->keyboardManager.confirmKey();
	return 1;
}
//Modyfikacja zaznaczenia znaku w klawiaturze
static 	uint8_t	functLeftKeyboard()
{
	PTE->keyboardManager.makeMove('a');
	return 1;
}
//Modyfikacja zaznaczenia znaku w klawiaturze
static 	uint8_t	functRightKeyboard()
{
	PTE->keyboardManager.makeMove('d');
	return 1;
}
//Modyfikacja zaznaczenia znaku w klawiaturze
static 	uint8_t	functUpKeyboard()
{
	PTE->keyboardManager.makeMove('w');
	return 1;
}
//Modyfikacja zaznaczenia znaku w klawiaturze
static 	uint8_t	functDownKeyboard()
{
	PTE->keyboardManager.makeMove('s');
	return 1;
}
//Backspace znaku z przycisku delete
static 	uint8_t	functDeleteKeyboard()
{
	PTE->keyboardManager.makeBackspace();
	return 1;
}
//Zatwierdzenie znaku z przycisku insert
static 	uint8_t	functInsertKeyboard()
{
	PTE->keyboardManager.confirmKey();
	return 1;
}
//Nadpisz istniejący plik
static 	uint8_t functOverwriteExportFile()
{
	PTE->hideOverwriteExportDialog();
	PTE->startExportSelection();
	return 1;
}
//Nie nadpisuj istniejącego pliku - zmien nazwę
static 	uint8_t functCancelOverwriteExportFile()
{
	char localPatch[PATCH_SIZE];
	char keyboardName[33];
	uint16_t cnt = 1;
	strcpy(keyboardName,PTE->keyboardManager.getName());
	if(strlen(keyboardName) <= 27) // 33 - 1(na zero) - 5 znakow na _%04d
	{
		do
		{
			sprintf(keyboardName,"%s_%04d",PTE->keyboardManager.getName(),cnt);
			sprintf(localPatch, "Export/%s/Selection/%s.wav",newFileManager.getCurrentProjectName(), keyboardName);
			if(++cnt > 9999) break;
		}while(SD.exists(localPatch));
	}

	PTE->isLoadAfterSave = false;
	PTE->keyboardManager.fillName(keyboardName);
	PTE->keyboardManager.activateKeyboard();

	PTE->hideOverwriteExportDialog();
	PTE->FM->unblockAllInputs();
	PTE->setKeyboardExportFunctions();
	return 1;
}
//##############################################################################################
//###############################            INVERT            #################################
//##############################################################################################
uint8_t functInvert()
{
	//--------------------------------------------------------
	//TU

	newFileManager.storePatternUndoRevision();

	sendSelection();
	sequencer.invertSelectedSteps(getSelectedElement());

	PTE->refreshPattern();

	newFileManager.setPatternStructChanged(mtProject.values.actualPattern);

	//--------------------------------------------------------
	return 1;
}
//##############################################################################################
//###############################            TRANSPOSE		   #################################
//##############################################################################################
static uint8_t functTranspose(uint8_t state)
{
	//--------------------------------------------------------
	//TU
//	if(state == buttonPress) functRamTest(1);
//	else if(state == buttonRelease) functRamTest(0);


	//--------------------------------------------------------
	return 1;
}
//##############################################################################################
//###############################            UNDO		   #################################
//##############################################################################################
static uint8_t functUndo(uint8_t state)
{
	if(state == buttonPress)
	{
		PTE->selectedPlace = 7;
		PTE->unfocusPattern();
		PTE->activateLabelsBorder();
		display.synchronizeRefresh();
	}
	else if (state == buttonRelease)
	{

		if(PTE->fillButton7ReleaseBlocker)
		{
			PTE->fillButton7ReleaseBlocker = 0;
			return 1;
		}
		if (PTE->selectedPlace == 7)
		{
			PTE->focusOnPattern();
		}
		if (PTE->undoRedoOpen)
		{
			PTE->undoRedoOpen = 0; // zerujemy na release
		}
		else
		{
			newFileManager.undoPattern();
		}

		PTE->refreshPattern();
		PTE->showPattern();
		PTE->showLength();
	}
	return 1;
}

static uint8_t functUndoRedo(int8_t dir)
{
	if (dir > 0)
	{
		newFileManager.redoPattern();
		PTE->undoRedoOpen = 1;
	}
	else if (dir < 0)
	{
		newFileManager.undoPattern();
		PTE->undoRedoOpen = 1;
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
						Sequencer::strPattern::strTrack::strStep & tempStep = seq->track[trackerPattern.actualTrack].step[trackerPattern.actualStep];

						show_fx = tempStep.fx[fx_index].value; // wartosc
						if (show_fx > FX_VALUE_MAX) break;

						float showFxTemp =  map(
								(float)show_fx,
								sequencer.getFxMin(tempStep.fx[fx_index].type),
								sequencer.getFxMax(tempStep.fx[fx_index].type),
								0,
								47);

						show_fx = showFxTemp + 0.5f;

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
	if(PTE->keyboardManager.getState())
	{
		PTE->keyboardManager.onPadChange(pad, state);
		return 1;
	}

	// obsługa przycisków pod ekranem (na poczatku bo dziala tez bez editmode)
	if (PTE->selectedPlace >= 0)
	{

		switch (PTE->selectedPlace)
		{
		case 0:
			PTE->setActualPattern(pad+1);
			PTE->showPattern();
			break;
		case 1:
			PTE->setActualPatternLength(map(pad, 0, 47, 3, 191));
			newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
			PTE->showLength();
			break;
		case 2:
			PTE->setActualPatternEditStep(pad);
			newFileManager.setProjectStructChanged();
			PTE->showStep();
			break;
		}

		PTE->lightUpPadBoard();

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

	// dalej tylko jesli edit (rec)
	if (PTE->editMode != 1 && !sequencer.isRec())
	{
		sendSelection();
		uint8_t noteToPlay = mtPadBoard.getNoteFromPad(pad);
		if (PTE->editParam == 1)
		{
			mtProject.values.lastUsedInstrument = pad;
			noteToPlay = sequencer.getSelectionFirstNote();
		}

		if (state == buttonPress)
		{
			sequencer.handleNoteOn(
					Sequencer::GRID_INSIDE_PATTERN,
					noteToPlay,
					-1,
					pad);
		}
		else if (state == buttonRelease)
		{
//			uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
			sequencer.handleNoteOff(Sequencer::GRID_INSIDE_PATTERN,
									noteToPlay,
									0,
									pad);
		}
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
	if (PTE->editMode == 1 || sequencer.isRec())
	{
		if (state == buttonPress)
		{
			newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
			newFileManager.storePatternUndoRevision();
		}

		switch (PTE->editParam)
		{
		case 0: // nuta
		{
			if (sequencer.isRec())
			{
				sendSelection();
				if (state == buttonPress)
				{
					uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
					sequencer.handleNoteOn(
							Sequencer::GRID_OUTSIDE_PATTERN,
							noteFromPad,
							sequencer.getInstrumentVelo(
									mtProject.values.lastUsedInstrument),
							Sequencer::handleNoteSource_irrelevant);
				}
				else if (state == buttonRelease)
				{
					uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
					sequencer.handleNoteOff(
							Sequencer::GRID_OUTSIDE_PATTERN,
							noteFromPad,
							0,
							Sequencer::handleNoteSource_irrelevant);
				}
			}
			else
			{

				sendSelection();
				if (isMultiSelectionOnOneLine())
				{
					if (state == buttonPress)
					{
						uint8_t noteFromPad = mtPadBoard.getNoteFromPad(
																		pad);

						sequencer.handleNoteOn(
								Sequencer::GRID_OUTSIDE_PATTERN,
								noteFromPad,
								sequencer.getInstrumentVelo(
										mtProject.values.lastUsedInstrument),
								pad);
					}
					else if (state == buttonRelease)
					{
						uint8_t noteFromPad = mtPadBoard.getNoteFromPad(
																		pad);

						sequencer.handleNoteOff(
												Sequencer::GRID_OUTSIDE_PATTERN,
												noteFromPad,
												0,
												pad);
					}
				}
				else
				{
					if (state == buttonPress)
					{

						uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
						sequencer.setSelectionNote(noteFromPad);
					}
				}

			}
			break;
		}

		case 1: // instrument
		{

			if (sequencer.isRec())
			{
				sendSelection();
				if (state == buttonPress)
				{
//					sequencer.setSelectionInstrument(pad);

					sequencer.handleNoteOn(
							Sequencer::GRID_OUTSIDE_PATTERN,
							Sequencer::STEP_NOTE_DEFAULT,
							sequencer.getInstrumentVelo(
									mtProject.values.lastUsedInstrument),
							pad);
				}
				else if (state == buttonRelease)
				{

					sequencer.handleNoteOff(Sequencer::GRID_OUTSIDE_PATTERN,
											Sequencer::STEP_NOTE_DEFAULT,
											0,
											pad);
				}
			}
			else
			{
				if (state == buttonPress)
				{
					sendSelection();
					sequencer.setSelectionInstrument(pad);
				}
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
				newFileManager.setPatternStructChanged(mtProject.values.actualPattern);
				newFileManager.storePatternUndoRevision();

				if((tactButtons.isButtonPressed(interfaceButtonFx1) || tactButtons.isButtonPressed(interfaceButtonFx2))
						)//&& mtPopups.getStepPopupState() == stepPopupNone)
				{
					PTE->dontShowPopupsUntilButtonRelease = 1;
					sequencer.setSelectionFxType(fx_index, interfaceGlobals.fxIDs[pad]);
				}
				else
				{
					sequencer.setSelectionFxValueByPad(fx_index, pad);
				}
			}
			break;
		}

		default:
			break;

		}

		if (state == buttonPress)
		{
			if (!isMultiSelection()) PTE->moveCursorByStep();

			PTE->lightUpPadBoard();
			PTE->refreshPattern();
		}
		else if (state == buttonRelease)
		{
			if (isMultiSelectionOnOneLine() && sequencer.noMoreRecOpen())
			{
				PTE->moveCursorByStepParallel();
			}

			PTE->lightUpPadBoard();
			PTE->refreshPattern();
		}

	}


	return 1;
}

//##############################################################################################
//###############################       STEP POPUPS            #################################
//##############################################################################################

void cPatternEditor::setStepButtonState(uint8_t button, uint8_t state)
{
	if(state)
	{
		if(mtPopups.getStepPopupState() != stepPopupNone
			|| tactButtons.isButtonPressed(interfaceButtonShift)
			|| tactButtons.isButtonPressed(interfaceButtonPattern)
			|| PTE->dontShowPopupsUntilButtonRelease)
		{
			return;
		}

		if(stepButtonsState[0] || stepButtonsState[1] || stepButtonsState[2] || stepButtonsState[3]) return;

		stepButtonsState[button] = 1;
		stepButtonsTimer[button] = 0;
	}
	else
	{
		stepButtonsState[button] = 0;
	}
}

void cPatternEditor::showNotePopup()
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

}

void cPatternEditor::showInstrumentPopup()
{
	// odswiezenie paternu bez danych zakrytych przez popup
	PTE->trackerPattern.popupMode |= 2;
	display.refreshControl(PTE->patternControl);

	mtPopups.showStepPopup(stepPopupInstr, mtProject.values.lastUsedInstrument);

}

void cPatternEditor::showFx1Popup()
{
	if(PTE->fillState == 1) return;

	PTE->FM->clearButton(interfaceButtonFx1);

	// odswiezenie paternu bez danych zakrytych przez popup
	PTE->trackerPattern.popupMode |= 2;
	if(mtConfig.interface.fxPopupDescription) PTE->trackerPattern.popupMode |= 4;
	display.refreshControl(PTE->patternControl);

	mtProject.values.lastUsedFx = PTE->getStepFx();
	mtPopups.showStepPopup(stepPopupFx, mtProject.values.lastUsedFx); //PTE->getStepFx()
}

void cPatternEditor::showFx2Popup()
{
	if(PTE->fillState == 1) return;

	PTE->FM->clearButton(interfaceButtonFx2);

	// odswiezenie paternu bez danych zakrytych przez popup
	PTE->trackerPattern.popupMode |= 2;
	if(mtConfig.interface.fxPopupDescription) PTE->trackerPattern.popupMode |= 4;
	display.refreshControl(PTE->patternControl);

	mtProject.values.lastUsedFx = PTE->getStepFx();
	mtPopups.showStepPopup(stepPopupFx, mtProject.values.lastUsedFx); //PTE->getStepFx()
}

void cPatternEditor::focusOnPattern()
{
	PTE->selectedPlace = -1;
	PTE->activateLabelsBorder();

	PTE->lightUpPadBoard();

	if(!PTE->editMode)
	{
		if(sequencer.isRec()) activateRedSelection();
		else activateGreenSelection();
	}
	else activateRedSelection();

	display.refreshControl(patternControl);
}

void cPatternEditor::unfocusPattern()
{
	lightUpPadBoard();

	deactivateSelection();

	display.refreshControl(patternControl);
}



//##############################################################################################
//###############################                              #################################
//##############################################################################################



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



void cPatternEditor::changePatternViewMode(uint8_t param)
{
	if(patternViewMode == 0 || param == 0) return;  // standardowy widok 4 parametrow
	if(patternViewMode & (1 << (param-1))) return; // juz widac wybrany parametr to nic nie rob

	uint8_t param_count = (patternViewMode & 1) +  ((patternViewMode>>1) & 1) + ((patternViewMode>>2) & 1) + ((patternViewMode>>3) & 1);

	if(param_count > 1) patternViewMode = (1<<(param-1)) | (1<<(previousEditParam));
	else if(param_count == 1) patternViewMode = (1<<(param-1));
	else return;

	display.setControlValue(PTE->patternControl, PTE->patternViewMode);
	display.refreshControl(PTE->patternControl);
}

static uint8_t functSwitchModule(uint8_t button)
{
//	if(tactButtons.isButtonPressed(interfaceButtonShift) && PTE->editMode == 1)

	Sequencer::strPattern *seq = sequencer.getPatternToUI();
	Sequencer::strPattern::strTrack::strStep *actualStep = &seq->track[PTE->trackerPattern.actualTrack].
			step[PTE->trackerPattern.actualStep];

	if (actualStep->note >= 0)
	{
		mtProject.values.lastUsedNote = actualStep->note;
		mtProject.values.lastUsedInstrument = constrain(
														actualStep->instrument,
														0,
														INSTRUMENTS_MAX + 16);
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

	PTE->eventFunct(eventSwitchModule,PTE,&button,0);

	return 1;
}


//======================================================================================================================

