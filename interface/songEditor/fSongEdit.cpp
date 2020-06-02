

#include "songEditor/songEditor.h"
//#include "mtFileManager.h"
#include "Encoder.h"
#include "fileManager.h"

#include "patternEditor/patternEditor.h" // todo zrobic cos z tym

#include "mtAudioEngine.h"
#include "keyScanner.h"
#include "mtPadsBacklight.h"
#include "mtPadBoard.h"

cSongEditor songEditor;

static cSongEditor* SE = &songEditor;


static  uint8_t functPlayPattern();
static  uint8_t functPlaySong();
static  uint8_t functIncPattern();
static  uint8_t functDecPattern();
static  uint8_t functDeleteSlot();
static  uint8_t functAddSlot();

static  uint8_t functTempo();


static  uint8_t functUp();
static  uint8_t functDown();
static  uint8_t functRight();
static  uint8_t functLeft();

static  uint8_t functShift(uint8_t state);

static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchModeSong(uint8_t state);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static uint8_t functCopyPaste();
static void refreshCopyPasting();
static void updateBitmaskAfterCopy(uint8_t *src, uint8_t *dest, uint8_t startSrc, uint8_t startDest, uint8_t length);

static uint8_t functCopy();
static uint8_t functPaste();

static uint8_t functUndo();

static uint8_t functDelete();
static void refreshDeleting();
static void updateBitmaskAfterDelete(uint8_t *src, uint8_t startSrc, uint8_t length);

void cSongEditor::update()
{
	if(songPlayerRefreshTimer > 100)
	{
		songPlayerRefreshTimer = 0;

		if(songPlayerData.progress.isPlaying)
		{
			markCurrentPattern(0);
		}
		refreshButtonsLabels();
	}

	refreshCopyPasting();
	refreshDeleting();
}

void cSongEditor::start(uint32_t options)
{
	moduleRefresh = 1;
	loopPosition = -1;
	selectedPlace = 9;
	isSomethingCopied = 0;

	exitOnButtonRelease = 0;



	readSong();
	songPlayerData.list = &patternsList;
	songPlayerData.songData = mtProject.song.playlist;
	songPlayerData.patternsBitmask = mtProject.values.allPatternsBitmask;

	refreshSongPlayerControl();
	showPatternsList();


	//selectedPlace = 0;
	activateLabelsBorder();

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
	FM->setButtonObj(interfaceButtonSong, functSwitchModeSong);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	handleEntryIcon();

	display.setControlData(songPlayerControl, &songPlayerData);
	display.refreshControl(songPlayerControl);

}

void cSongEditor::stop()
{
	sequencer.stopManualNotes();

	moduleRefresh = 0;
}

void cSongEditor::turnOffPerformanceMode()
{
	if(sequencer.isPerformanceMode())
	{
		newFileManager.loadWorkspacePatternNow(mtProject.values.actualPattern);
		sequencer.switchRamPatternsNow();
		sequencer.exitPerformanceMode();
	}

	engine.performanceModeEndAll();

}




void cSongEditor::setDefaultScreenFunct()
{
	//funkcje
	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

//	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);

	FM->setButtonObj(interfaceButton0, buttonPress, functPlayPattern);
	FM->setButtonObj(interfaceButton1, buttonPress, functPlaySong);
	FM->setButtonObj(interfaceButton2, buttonPress, functUndo);

	FM->setButtonObj(interfaceButton3, buttonPress, functAddSlot);
	FM->setButtonObj(interfaceButton4, buttonPress, functDeleteSlot);

	FM->setButtonObj(interfaceButton5, buttonPress, functDecPattern);
	FM->setButtonObj(interfaceButton6, buttonPress, functIncPattern);

	FM->setButtonObj(interfaceButton7, buttonPress, functTempo);

	FM->setButtonObj(interfaceButtonCopy, buttonPress, functCopyPaste);
	FM->setButtonObj(interfaceButtonDelete, buttonPress, functDelete);
	//FM->setButtonObj(interfaceButton7, buttonPress, functPatternLength);



	FM->setPadsGlobal(functPads);


}


//==============================================================================================================

static  uint8_t functPlayPattern()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		if(tactButtons.isButtonPressed(interfaceButtonShift) && SE->selectedPlace < 8
				&& SE->songPlayerData.selection.patternSelectionLength == 1
				&& SE->songPlayerData.selection.trackSelectionLength == 1)//xxx
		{
			// tlylko zaznacozny track

			SE->switchToNewPattern();

			sequencer.setSelection(0,
								SE->songPlayerData.selection.startTrack,
								sequencer.getPatternLength()-1,
								SE->songPlayerData.selection.startTrack);

			sequencer.playSelection();

			SE->startSongPlayer();
			// wlacza tryb preview tracku \/
			SE->songPlayerData.progress.trackPreview = SE->songPlayerData.selection.startTrack;

			display.refreshControl(SE->songPlayerControl);
			return 1;
		}

		// caly pattern

		SE->switchToNewPattern();

		sequencer.playPattern();
		SE->loopPosition = SE->selectedPattern;
		SE->showIcon(iconLoop,SE->selectedPattern);

		SE->startSongPlayer();

		display.refreshControl(SE->songPlayerControl);


	}
	else if (sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		SE->loopPosition = -1;
		sequencer.stop();
		SE->hideIcon();
	}
	return 1;
}

static  uint8_t functPlaySong()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		sequencer.playSong(SE->selectedPattern);
		SE->showIcon(iconPlay,SE->selectedPattern);

		SE->startSongPlayer();
	}
	else if (sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		SE->loopPosition = -1;
		sequencer.stop();
		SE->hideIcon();
	}
	return 1;
}


static  uint8_t functIncPattern()
{
	if(SE->isBusy) return 1;
	if(SE->exitOnButtonRelease) return 1;

	if(SE->selectedPlace == 8)
	{
		if(mtProject.song.playlist[SE->selectedPattern] < PATTERN_INDEX_MAX)
		{
			mtProject.song.playlist[SE->selectedPattern] += 1;

			if(SE->selectedPattern == SE->loopPosition)
			{
				SE->switchToNewPattern();
			}
		}

		SE->refreshSongPlayerControl();

		SE->listPatterns();
		SE->showPatternsList();

		//fileManager.projectChangeFlag = 1;
		//mtProject.values.projectNotSavedFlag = 1;
		newFileManager.setProjectStructChanged();
	}

	SE->selectedPlace = 8;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functDecPattern()
{
	if(SE->isBusy) return 1;
	if(SE->exitOnButtonRelease) return 1;

	if(SE->selectedPlace == 8)
	{
		if(mtProject.song.playlist[SE->selectedPattern]>1)
		{
			mtProject.song.playlist[SE->selectedPattern] -= 1;

			if(SE->selectedPattern == SE->loopPosition)
			{
				SE->switchToNewPattern();
			}
		}

		SE->refreshSongPlayerControl();

		SE->listPatterns();
		SE->showPatternsList();

		//fileManager.projectChangeFlag = 1;
		//mtProject.values.projectNotSavedFlag = 1;
		newFileManager.setProjectStructChanged();
	}

	SE->selectedPlace = 8;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functAddSlot()
{
	if(SE->isBusy) return 1;
	if(SE->exitOnButtonRelease) return 1;

	//if(SE->songLength < 15)//if(SE->songLength < (PATTERNS_COUNT-1))// 15 do czasu poprawy obslugi listy w interfejsie
	if(SE->songLength < SONG_MAX)
	{
		for(int i = SE->songLength; i > SE->selectedPattern; i--)
		{
			mtProject.song.playlist[i] = mtProject.song.playlist[i-1];
		}

		mtProject.song.playlist[SE->selectedPattern+1] =mtProject.song.playlist[SE->selectedPattern];

		SE->songLength++;

		if(SE->songLength)
		{
			SE->selectedPattern++;
		}

		if(mtProject.song.playlist[SE->selectedPattern]==0)
		{
			mtProject.song.playlist[SE->selectedPattern]=1;
		}
	}

	SE->songPlayerData.songLength = SE->songLength;

	SE->listPatterns();
	SE->showPatternsList();

	SE->refreshSongPlayerControl();


	SE->selectedPlace = 8;
	SE->activateLabelsBorder();

	//fileManager.projectChangeFlag = 1;
	//mtProject.values.projectNotSavedFlag = 1;
	newFileManager.setProjectStructChanged();

	return 1;
}

static  uint8_t functDeleteSlot()
{
	if(SE->isBusy) return 1;
	if(SE->exitOnButtonRelease) return 1;

	if(SE->songLength>1)
	{
		mtProject.song.playlist[SE->songLength]=0;

		for(int i = SE->selectedPattern; i < SE->songLength ; i++)
		{
			mtProject.song.playlist[i] = mtProject.song.playlist[i+1];
		}

		SE->songLength--;

		if(SE->songLength)
		{
			if(SE->selectedPattern)
			{
				SE->selectedPattern--;
			}

			if((SE->selectedPattern != SE->loopPosition) && (SE->loopPosition> SE->selectedPattern))
			{
				SE->loopPosition--;
				SE->showIcon(iconLoop, SE->loopPosition);
				SE->switchToNewPattern();

				SE->startSongPlayer();
			}
		}
	}

	SE->songPlayerData.songLength = SE->songLength;

	SE->listPatterns();
	SE->showPatternsList();


	SE->refreshSongPlayerControl();

	SE->selectedPlace = 8;
	SE->activateLabelsBorder();

	//fileManager.projectChangeFlag = 1;
	//mtProject.values.projectNotSavedFlag = 1;
	newFileManager.setProjectStructChanged();
	return 1;
}


static  uint8_t functTempo()
{
	if(SE->isBusy) return 1;

	SE->selectedPlace = 9;
	SE->activateLabelsBorder();

	return 1;
}

//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	if(SE->isBusy) return 1;

	switch(SE->selectedPlace)
	{
	case 8: SE->changePatternsSelection(value); 	break;
	case 9: SE->changeGlobalTempo(value);			break;
	//case 2: SE->changeGlobalPatternLength(value); 	break;

	default: break;

	}

	return 1;
}


static  uint8_t functShift(uint8_t state)
{
	if(SE->isBusy) return 1;


	return 1;
}


static  uint8_t functUp()
{
	if(SE->isBusy) return 1;

	switch(SE->selectedPlace)
	{
	case 0: SE->walkOnSongPlayer(playerUp);		break;
	case 1: SE->walkOnSongPlayer(playerUp);		break;
	case 2: SE->walkOnSongPlayer(playerUp);		break;
	case 3: SE->walkOnSongPlayer(playerUp);		break;
	case 4: SE->walkOnSongPlayer(playerUp);		break;
	case 5: SE->walkOnSongPlayer(playerUp);		break;
	case 6: SE->walkOnSongPlayer(playerUp);		break;
	case 7: SE->walkOnSongPlayer(playerUp);		break;

	case 8:	SE->changePatternsSelection(-1); 	break;
	case 9:	SE->changeGlobalTempo(1); 			break;
	//case 2:	SE->changeGlobalPatternLength(1);	break;
	}

	return 1;
}

static  uint8_t functDown()
{
	if(SE->isBusy) return 1;

	switch(SE->selectedPlace)
	{
	case 0: SE->walkOnSongPlayer(playerDown);		break;
	case 1: SE->walkOnSongPlayer(playerDown);		break;
	case 2: SE->walkOnSongPlayer(playerDown);		break;
	case 3: SE->walkOnSongPlayer(playerDown);		break;
	case 4: SE->walkOnSongPlayer(playerDown);		break;
	case 5: SE->walkOnSongPlayer(playerDown);		break;
	case 6: SE->walkOnSongPlayer(playerDown);		break;
	case 7: SE->walkOnSongPlayer(playerDown);		break;

	case 8:	SE->changePatternsSelection(1); 		break;
	case 9:	SE->changeGlobalTempo(-1); 				break;
	//case 2:	SE->changeGlobalPatternLength(-1); 	break;
	}

	return 1;
}

static  uint8_t functLeft()
{
	if(SE->isBusy) return 1;

	if(SE->exitOnButtonRelease) return 1;

	if(SE->selectedPlace == 8)
	{
		if(SE->songPlayerData.selection.isActive != 1)
		{
			SE->selectSpecificTrack(7, SE->selectedPattern);
		}
	}

	if(SE->selectedPlace < 8)
	{
		SE->walkOnSongPlayer(playerLeft);
		SE->selectedPlace = SE->songPlayerData.selection.startTrack;
	}
	else
	{
		SE->selectedPlace--;

		if(SE->selectedPlace == 7)
		{
			SE->selectedPlace = SE->songPlayerData.selection.startTrack;
		}
	}

	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	if(SE->isBusy) return 1;
	if(SE->exitOnButtonRelease) return 1;

	uint8_t skip = 0;

	if(SE->selectedPlace < 7)
	{
		SE->walkOnSongPlayer(playerRight);
	}

	if(SE->selectedPlace == 7 && tactButtons.isButtonPressed(interfaceButtonShift))
	{
		skip = 1;
	}

	if(skip == 0)
	{
		if(SE->selectedPlace < SE->frameData.placesCount-1) SE->selectedPlace++;
	}

	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functPlayAction()
{
	if(SE->isBusy) return 1;


	if (tactButtons.isButtonPressed(interfaceButtonShift))
	{
		functPlayPattern();
	}
	else
	{
		functPlaySong();

		if (tactButtons.isButtonPressed(interfaceButtonSong))
		{
			SE->exitOnButtonRelease = 1;
		}
	}


	return 1;
}

static  uint8_t functRecAction()
{
	if(SE->isBusy) return 1;

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(SE->isBusy) return 1;

	if(button != interfaceButtonPerformance) SE->turnOffPerformanceMode();
	SE->eventFunct(eventSwitchModule,SE,&button,0);

	return 1;
}

//======================================================================================================================
void cSongEditor::startSongPlayer()
{
	songPlayerData.progress.isPlaying = 1;
	SE->songPlayerData.progress.trackPreview = 255; // wylacza tryb preview tracku
	songPlayerData.progress.currentSongPosition = selectedPattern;
	songPlayerData.progress.patternLength = sequencer.getPatternLength();
	songPlayerData.progress.positionInPattern = sequencer.getActualPos();

	display.refreshControl(SE->songPlayerControl);
}

void cSongEditor::stopSongPlayer()
{
	songPlayerData.progress.isPlaying = 0;

	display.refreshControl(SE->songPlayerControl);
}

void cSongEditor::stopPlayingSelection()
{
	if(sequencer.ptrPlayer->selectionMode)
	{
		sequencer.stop();
		songPlayerData.progress.isPlaying = 0;
		display.refreshControl(SE->songPlayerControl);
	}
}

void cSongEditor::refreshSongPlayer(uint8_t source)
{
	if(sequencer.isStop())
	{
		songPlayerData.progress.isPlaying = 0;
		display.refreshControl(songPlayerControl);
		return;
	}

	songPlayerData.progress.currentSongPosition = source;
	songPlayerData.progress.patternLength = sequencer.getPatternLength();
	songPlayerData.progress.positionInPattern = sequencer.getActualPos();

	display.refreshControl(songPlayerControl);
}

void cSongEditor::changePatternsSelection(int16_t value)
{
	if(selectedPattern + value < 0) selectedPattern = 0;
	else if(selectedPattern + value > songLength-1) selectedPattern = songLength-1;
	else  selectedPattern += value;

	listPatterns();
	display.setControlValue(songPlayerControl, selectedPattern);
	refreshSongPlayerControl();

	//display.refreshControl(songPlayerControl);
}

//======================================================================================================================
//==============================================================================================
void cSongEditor::refreshSongPlayerControl()
{
	//getUsage();
	display.refreshControl(songPlayerControl);
	display.synchronizeRefresh();
}

void cSongEditor::readSong()
{
	uint8_t i;
	uint8_t isAnyActive = 0;
	for(i = 0; i < SONG_MAX; i++)
	{
		if(mtProject.song.playlist[i] == 0)
		{
			break;
		}
		else
		{
			isAnyActive = 1;
		}
	}

	songLength = i;

	if((isAnyActive == 0) || (songLength == 0)) // nie znaleziono
	{
		songLength = 1;
		selectedPattern = 0;

		mtProject.song.playlist[0] = 1;
	}

	songPlayerData.songLength = songLength;

	if(selectedPattern >= songLength) selectedPattern = 0;

	listPatterns();
}

void cSongEditor::listPatterns()
{
	for(uint8_t i=0;i<songLength;i++)
	{
		if(i < 9)
		{
			if(i == selectedPattern)
			{
				sprintf(&patternsNamesList[i][0], "   %u         < %u >   ", i+1, mtProject.song.playlist[i]);
			}
			else
			{
				sprintf(&patternsNamesList[i][0], "   %u           %u     ", i+1, mtProject.song.playlist[i]);
			}
		}
		else if(i < 99)
		{
			if(i == selectedPattern)
			{
				sprintf(&patternsNamesList[i][0], "   %u        < %u >   ", i+1, mtProject.song.playlist[i]);
			}
			else
			{
				sprintf(&patternsNamesList[i][0], "   %u          %u     ", i+1, mtProject.song.playlist[i]);
			}
		}
		else
		{
			if(i == selectedPattern)
			{
				sprintf(&patternsNamesList[i][0], "   %u       < %u >   ", i+1, mtProject.song.playlist[i]);
			}
			else
			{
				sprintf(&patternsNamesList[i][0], "   %u         %u     ", i+1, mtProject.song.playlist[i]);
			}
		}

		patternNames[i] = &patternsNamesList[i][0];
	}
}

void cSongEditor::markCurrentPattern(uint8_t forceRefresh)
{
	if(sequencer.ptrPlayer->songMode == true)
	{
		if((mtProject.song.playlistPos != localSongPosition) || forceRefresh)
		{
			localSongPosition = mtProject.song.playlistPos;

			if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
			{
				showIcon(iconPlay,localSongPosition);
			}
		}

		refreshSongPlayer(mtProject.song.playlistPos);
	}
	else if(sequencer.ptrPlayer->selectionMode)
	{
		refreshSongPlayer(selectedPattern);
	}
	else
	{
		refreshSongPlayer(loopPosition);
	}
}

void cSongEditor::changeGlobalTempo(int16_t value)
{
//	nie zmieniaj jak external clock
	if(!sequencer.isInternalClock())
	{
		return;
	}
	if(mtProject.values.globalTempo+value < Sequencer::MIN_TEMPO) mtProject.values.globalTempo = Sequencer::MIN_TEMPO;
	else if(mtProject.values.globalTempo+value > Sequencer::MAX_TEMPO) mtProject.values.globalTempo = Sequencer::MAX_TEMPO;
	else  mtProject.values.globalTempo += value;

	//TODO: podpiąć afektowanie aktulanego odtwarzania

	showTempoValue();
}

void cSongEditor::setDefaultGlobalTempo()
{
//	nie zmieniaj jak external clock
	if(!sequencer.isInternalClock())
	{
		return;
	}

	mtProject.values.globalTempo = DEFAULT_TEMPO;
	//TODO: podpiąć afektowanie aktulanego odtwarzania

	showTempoValue();
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(SE->isBusy) return 1;

//	fileManager.storePatternUndoRevision();

//	switch (SE->selectedPlace)
//	{
//	case 9:
//		// TODO: zmiana tempa z pod padów
//		//sequencer.setTempo(map((float) pad, 0, 47, 10, 480));
//
//		SE->showTempoValue();
//		break;
//	default:
//		break;
//	}

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
	//newFileManager.setPatternStructChanged(mtProject.values.actualPattern); // todo odkomentowac jesli potrzebne

	return 1;

}

void cSongEditor::handleEntryIcon()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		hideIcon();
	}
	else
	{
		if(sequencer.ptrPlayer->songMode == 1)
		{
			startSongPlayer();
			markCurrentPattern(1);
		}
		else
		{
			loopPosition = findSlotWithPattern();

			if(loopPosition >= 0)
			{
				selectedPattern = loopPosition;
				showIcon(iconLoop, loopPosition);
				startSongPlayer();
			}
			else
			{
				hideIcon();
			}
		}
	}
}

void cSongEditor::switchToNewPattern()
{
	newFileManager.saveWorkspacePatternNow(mtProject.values.actualPattern);


	mtProject.values.actualPattern = constrain(
			mtProject.song.playlist[SE->selectedPattern], PATTERN_INDEX_MIN,
			PATTERN_INDEX_MAX);

	newFileManager.loadWorkspacePatternNow(mtProject.values.actualPattern);
	sequencer.switchRamPatternsNow();
}

int16_t cSongEditor::findSlotWithPattern()
{
	int32_t slot = 0;
	uint8_t foundFlag = 0;

	for(slot = 0; slot < songLength; slot++)
	{
		if(mtProject.song.playlist[slot] == mtProject.values.actualPattern)
		{
			foundFlag = 1;
			break;
		}
	}

	if(!foundFlag)
	{
		slot = -1;
	}

	return slot;
}

void cSongEditor::selectSpecificTrack(uint8_t track, uint8_t pattern)
{
	songPlayerData.selection.isActive = 1;
	songPlayerData.selection.patternSelectionLength = 1;
	songPlayerData.selection.trackSelectionLength = 1;
	songPlayerData.selection.startPattern = pattern;
	songPlayerData.selection.startTrack = track;

	refreshSongPlayerControl();
}

void cSongEditor::walkOnSongPlayer(player_direction_t dir)
{
	uint8_t isShiftPressed = 1;
	songPlayerData.selection.isActive = 1;

	if(!tactButtons.isButtonPressed(interfaceButtonShift))
	{
		if(songPlayerData.selection.patternSelectionLength > 1)
		{
			songPlayerData.selection.startPattern += (songPlayerData.selection.patternSelectionLength - 1);
		}

		if(songPlayerData.selection.trackSelectionLength > 1)
		{
			songPlayerData.selection.startTrack += (songPlayerData.selection.trackSelectionLength - 1);
		}

		songPlayerData.selection.patternSelectionLength = 1;
		songPlayerData.selection.trackSelectionLength = 1;
		isShiftPressed = 0;
	}
	else
	{
		if(songPlayerData.selection.patternSelectionLength == 0)
		{
			songPlayerData.selection.patternSelectionLength = 1;
		}

		if(songPlayerData.selection.trackSelectionLength == 0)
		{
			songPlayerData.selection.trackSelectionLength = 1;
		}
	}

	switch(dir)
	{
	case playerUp:

		if(isShiftPressed)
		{
			if((currSelDirection2 & selDirDown) || (currSelDirection2 == 0))
			{
				if(songPlayerData.selection.patternSelectionLength > 1)
				{
					songPlayerData.selection.patternSelectionLength--;
				}
				else
				{
					currSelDirection2 &= ~selDirDown;
					currSelDirection2 |= selDirUp;
				}
			}

			if((currSelDirection2 & selDirUp) || (currSelDirection2 == 0))
			{
				if(songPlayerData.selection.startPattern)
				{
					songPlayerData.selection.patternSelectionLength++;
					songPlayerData.selection.startPattern--;
				}
			}
		}
		else
		{
			if(songPlayerData.selection.startPattern > 0)
			{
				songPlayerData.selection.startPattern--;
			}
		}

		break;
	case playerDown:

		if(isShiftPressed)
		{
			if((currSelDirection2 & selDirUp) || (currSelDirection2 == 0))
			{
				if(songPlayerData.selection.patternSelectionLength > 1)
				{
					songPlayerData.selection.patternSelectionLength--;
					songPlayerData.selection.startPattern++;
				}
				else
				{
					currSelDirection2 &= ~selDirUp;
					currSelDirection2 |= selDirDown;
				}
			}

			if((currSelDirection2 & selDirDown) || (currSelDirection2 == 0))
			{
				if((songPlayerData.songLength > songPlayerData.selection.patternSelectionLength + songPlayerData.selection.startPattern))
				{
					songPlayerData.selection.patternSelectionLength++;
				}
			}
		}
		else
		{
			if(songPlayerData.selection.startPattern < (songPlayerData.songLength - 1))
			{
				songPlayerData.selection.startPattern++;
			}
		}

		break;
	case playerLeft:

		if(isShiftPressed)
		{
			if((currSelDirection & selDirRight) || (currSelDirection == 0))
			{
				if(songPlayerData.selection.trackSelectionLength > 1)
				{
					songPlayerData.selection.trackSelectionLength--;
				}
				else
				{
					currSelDirection &= ~selDirRight;
					currSelDirection |= selDirLeft;
				}
			}

			if((currSelDirection & selDirLeft) || (currSelDirection == 0))
			{
				if(songPlayerData.selection.startTrack)
				{
					songPlayerData.selection.trackSelectionLength++;
					songPlayerData.selection.startTrack--;
				}
			}
		}
		else
		{
			if(songPlayerData.selection.startTrack > 0)
			{
				songPlayerData.selection.startTrack--;
			}
		}

		break;
	case playerRight:

		if(isShiftPressed)
		{
			if((currSelDirection & selDirLeft) || (currSelDirection == 0))
			{
				if(songPlayerData.selection.trackSelectionLength > 1)
				{
					if(songPlayerData.selection.startTrack < 8)
					{
						songPlayerData.selection.trackSelectionLength--;
						songPlayerData.selection.startTrack++;
					}
				}
				else
				{
					currSelDirection &= ~selDirLeft;
					currSelDirection |= selDirRight;
				}
			}

			if((currSelDirection & selDirRight) || (currSelDirection == 0))
			{
				if(songPlayerData.selection.trackSelectionLength < 8)
				{
					songPlayerData.selection.trackSelectionLength++;
				}
			}
		}
		else
		{
			if(songPlayerData.selection.startTrack < 7)
			{
				songPlayerData.selection.startTrack++;
			}
		}

		break;

	default:
		break;
	}

	if(isShiftPressed)//xxx
	{
		if(currSelDirection2 & selDirUp)
		{
			selectedPattern = songPlayerData.selection.startPattern;
		}
		else if(currSelDirection2 & selDirDown)
		{
			selectedPattern = songPlayerData.selection.startPattern + songPlayerData.selection.patternSelectionLength - 1;
		}
	}
	else
	{
		selectedPattern = songPlayerData.selection.startPattern;
	}

	stopPlayingSelection();
	listPatterns();
	display.setControlValue(songPlayerControl, selectedPattern);
	refreshSongPlayerControl();
}

static  uint8_t functSwitchModeSong(uint8_t state)
{
	if(state == buttonPress)
	{
		//SE->activateLabelsBorder();
	}
	else if(state == buttonHold)
	{
		SE->selectedPlace = 9;
		SE->exitOnButtonRelease = 1;
		SE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(SE->exitOnButtonRelease) SE->eventFunct(eventSwitchToPreviousModule,SE,0,0);
		else
		{
			SE->selectedPlace = 8;
			SE->activateLabelsBorder();
		}
	}

	return 1;
}

static void updateBitmaskAfterCopy(uint8_t *src, uint8_t *dest, uint8_t startSrc, uint8_t startDest, uint8_t length)
{
	while((startSrc + length > 8) || (startDest + length > 8))
	{
		length--;
	}

	for(uint8_t bit = 0; bit < length; bit++)
	{
		if(*src & (1 << (bit + startSrc)))
		{
			*dest |= (1 << (bit + startDest));
		}
		else
		{
			*dest &= ~(1 << (bit + startDest));
		}
	}

	//fileManager.projectChangeFlag = 1;
	//mtProject.values.projectNotSavedFlag = 1;
	newFileManager.setProjectStructChanged();
}

static void updateBitmaskAfterDelete(uint8_t *src, uint8_t startSrc, uint8_t length)
{
	for(uint8_t bit = 0; bit < length; bit++)
	{
		*src &= ~(1 << (bit + startSrc));
	}

	//fileManager.projectChangeFlag = 1;
	//mtProject.values.projectNotSavedFlag = 1;
	newFileManager.setProjectStructChanged();
}

static void refreshCopyPasting()
{
	if(SE->isCopyingInProgress)
	{
		uint8_t source = mtProject.song.playlist[SE->copyCurrentData.startPattern + SE->currentCopyElement];
		uint8_t destination =  mtProject.song.playlist[SE->songPlayerData.selection.startPattern + SE->currentCopyElement];

		newFileManager.storeSongUndoRevision(destination);

		newFileManager.copySongTracks((char*) "Workspace", source, destination,
				SE->copyCurrentData.startTrack,
				SE->songPlayerData.selection.startTrack,
				SE->copyCurrentData.trackSelectionLength);


		updateBitmaskAfterCopy(
				&mtProject.values.allPatternsBitmask[source-1],
				&mtProject.values.allPatternsBitmask[destination-1],
				SE->copyCurrentData.startTrack,
				SE->songPlayerData.selection.startTrack,
				SE->copyCurrentData.trackSelectionLength);

		SE->currentCopyElement++;

		SE->showHorizontalBar("Copying", SE->copyElementMax, SE->currentCopyElement);

		if(SE->currentCopyElement == SE->copyElementMax)
		{
			SE->refreshSongPlayerControl();
			SE->hideHorizontalBar();
			SE->isBusy = 0;
			SE->isCopyingInProgress = 0;

			patternEditor.changeActualPattern(0);// to przeladuje wyswietlany pattern w patternEdicie jesli cos zmienialismy w aktualnym
		}
	}
}

static uint8_t functCopy()
{
	if(SE->isBusy) return 1;

	if(SE->selectedPlace < 8)
	{
		SE->isSomethingCopied = 1;
		memcpy(&SE->copyCurrentData, &SE->songPlayerData.selection, sizeof(song_selection_t));
	}

	return 1;
}

static uint8_t functPaste()
{
	if(SE->isBusy) return 1;

	if(SE->selectedPlace < 8)
	{
		if(SE->isSomethingCopied)
		{
			SE->showHorizontalBar("Copying", SE->copyElementMax, SE->currentCopyElement);
			SE->currentCopyElement = 0;

			SE->isBusy = 1;
			SE->isCopyingInProgress = 1;
			SE->copyElementMax = SE->copyCurrentData.patternSelectionLength;

			while((SE->songPlayerData.selection.startPattern + SE->copyElementMax) > SE->songPlayerData.songLength)
			{
				SE->copyElementMax--;
			}
		}
	}

	return 1;
}

static uint8_t functUndo()
{
	if(SE->isBusy) return 1;

	newFileManager.undoSongPattern(); //todo brak save projektu?
	SE->refreshSongPlayerControl();

	return 1;
}

static uint8_t functCopyPaste()
{
	if(!(tactButtons.isButtonPressed(interfaceButtonShift)))
	{
		/*COPY*/
		functCopy();
	}
	else
	{
		/*PASTE*/
		functPaste();
	}

	return 1;
}

static uint8_t functDelete()
{
	if(SE->isBusy) return 1;

	if(SE->selectedPlace < 8)
	{
		SE->currentDeleteElement = 0;

		SE->isBusy = 1;
		SE->isDeletingInProgress = 1;
		SE->deleteElementMax = SE->songPlayerData.selection.patternSelectionLength;

		SE->showHorizontalBar("Deleting", SE->deleteElementMax, SE->currentDeleteElement);
	}
	else if(SE->selectedPlace == 9) SE->setDefaultGlobalTempo();

	return 1;
}

static void refreshDeleting()
{
	if(SE->isDeletingInProgress)
	{
		uint8_t source = mtProject.song.playlist[SE->songPlayerData.selection.startPattern + SE->currentDeleteElement];

		newFileManager.deleteTracks((char*) "Workspace", source, SE->songPlayerData.selection.startTrack, SE->songPlayerData.selection.trackSelectionLength);

		updateBitmaskAfterDelete(&mtProject.values.allPatternsBitmask[source-1], SE->songPlayerData.selection.startTrack, SE->songPlayerData.selection.trackSelectionLength);

		SE->currentDeleteElement++;

		SE->showHorizontalBar("Deleting", SE->deleteElementMax, SE->currentDeleteElement);

		if(SE->currentDeleteElement == SE->deleteElementMax)
		{
			SE->hideHorizontalBar();
			SE->refreshSongPlayerControl();
			SE->isBusy = 0;
			SE->isDeletingInProgress = 0;

			patternEditor.changeActualPattern(0);// to przeladuje wyswietlany pattern w patternEdicie jesli cos usuwalismy z aktualnie tam zaladowanego
		}
	}
}
