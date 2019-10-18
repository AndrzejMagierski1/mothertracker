

#include <songEditor.h>
#include "mtFileManager.h"
#include "Encoder.h"



#include "mtAudioEngine.h"
#include "keyScanner.h"


cSongEditor songEditor;

static cSongEditor* SE = &songEditor;






static  uint8_t functPatternSlot();

static  uint8_t functIncPattern();
static  uint8_t functDecPattern();
static  uint8_t functDeleteSlot();
static  uint8_t functAddSlot();

static  uint8_t functTempo();
static  uint8_t functPatternLength();


static  uint8_t functUp();
static  uint8_t functDown();
static  uint8_t functRight();
static  uint8_t functLeft();

static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);

static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);





void cSongEditor::update()
{

	markCurrentPattern(0);

}

void cSongEditor::start(uint32_t options)
{
	moduleRefresh = 1;
	loopPosition = -1;


	readSong();


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
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	handleEntryIcon();

}

void cSongEditor::stop()
{


	moduleRefresh = 0;
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
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);



	FM->setButtonObj(interfaceButton0, buttonPress, functPatternSlot);
	FM->setButtonObj(interfaceButton1, buttonPress, functPatternSlot);
	FM->setButtonObj(interfaceButton2, buttonPress, functDeleteSlot);
	FM->setButtonObj(interfaceButton3, buttonPress, functAddSlot);
	FM->setButtonObj(interfaceButton4, buttonPress, functDecPattern);
	FM->setButtonObj(interfaceButton5, buttonPress, functIncPattern);
	FM->setButtonObj(interfaceButton7, buttonPress, functTempo);
	//FM->setButtonObj(interfaceButton7, buttonPress, functPatternLength);



	FM->setPadsGlobal(functPads);


}


//==============================================================================================================

static  uint8_t functPatternSlot()
{
	SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functIncPattern()
{
	if(mtProject.mtProjectRemote.song.playlist[SE->selectedPattern] < 99)
	{
		mtProject.mtProjectRemote.song.playlist[SE->selectedPattern] += 1;

		if(SE->selectedPattern == SE->loopPosition)
		{
			SE->switchToNewPattern();
		}
	}


	SE->selectedPlace = 0;

	SE->listPatterns();
	SE->showPatternsList();

	SE->activateLabelsBorder();

	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;
	return 1;
}

static  uint8_t functDecPattern()
{
	if(mtProject.mtProjectRemote.song.playlist[SE->selectedPattern]>1)
	{
		mtProject.mtProjectRemote.song.playlist[SE->selectedPattern] -= 1;

		if(SE->selectedPattern == SE->loopPosition)
		{
			SE->switchToNewPattern();
		}
	}

	SE->selectedPlace = 0;

	SE->listPatterns();
	SE->showPatternsList();

	SE->activateLabelsBorder();

	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;
	return 1;
}

static  uint8_t functAddSlot()
{
	//if(SE->songLength < 15)//if(SE->songLength < (PATTERNS_COUNT-1))// 15 do czasu poprawy obslugi listy w interfejsie
	if(SE->songLength < SONG_MAX)
	{
		for(int i = SE->songLength; i > SE->selectedPattern; i--)
		{
			mtProject.mtProjectRemote.song.playlist[i] = mtProject.mtProjectRemote.song.playlist[i-1];
		}

		mtProject.mtProjectRemote.song.playlist[SE->selectedPattern+1] =mtProject.mtProjectRemote.song.playlist[SE->selectedPattern];

		if(SE->songLength)
		{
			SE->selectedPattern++;
		}

		SE->songLength++;

		if(mtProject.mtProjectRemote.song.playlist[SE->selectedPattern]==0)
		{
			mtProject.mtProjectRemote.song.playlist[SE->selectedPattern]=1;
		}
	}

	SE->listPatterns();
	SE->showPatternsList();



	SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;

	return 1;
}

static  uint8_t functDeleteSlot()
{
	if(SE->songLength>1)
	{
		mtProject.mtProjectRemote.song.playlist[SE->songLength]=0;

		for(int i = SE->selectedPattern; i < SE->songLength ; i++)
		{
			mtProject.mtProjectRemote.song.playlist[i] = mtProject.mtProjectRemote.song.playlist[i+1];
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
				SE->showIcon(iconLoop,SE->loopPosition);
				SE->switchToNewPattern();
			}
		}
	}

	SE->listPatterns();
	SE->showPatternsList();


	SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;
	return 1;
}


static  uint8_t functTempo()
{
	SE->selectedPlace = 1;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functPatternLength()
{
	SE->selectedPlace = 2;
	SE->activateLabelsBorder();

	return 1;
}

//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	switch(SE->selectedPlace)
	{
	case 0: SE->changePatternsSelection(value); 	break;
	case 1: SE->changeGlobalTempo(value);			break;
	//case 2: SE->changeGlobalPatternLength(value); 	break;

	default: break;

	}

	return 1;
}

static  uint8_t functEnter()
{


	return 1;
}

static  uint8_t functShift(uint8_t state)
{



	return 1;
}


static  uint8_t functUp()
{
	switch(SE->selectedPlace)
	{
	case 0:	SE->changePatternsSelection(-1); 	break;
	case 1:	SE->changeGlobalTempo(1); 			break;
	//case 2:	SE->changeGlobalPatternLength(1);	break;
	}

	return 1;
}

static  uint8_t functDown()
{
	switch(SE->selectedPlace)
	{
	case 0:	SE->changePatternsSelection(1); 	break;
	case 1:	SE->changeGlobalTempo(-1); 			break;
	//case 2:	SE->changeGlobalPatternLength(-1); 	break;
	}

	return 1;
}

static  uint8_t functLeft()
{
	if(SE->selectedPlace == 0)
	{
		functDecPattern();
		return 1;
	}

	if(SE->selectedPlace > 0) SE->selectedPlace--;
	SE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(SE->selectedPlace == 0)
	{
		functIncPattern();
		return 1;
	}

	if(SE->selectedPlace < SE->frameData.placesCount-1) SE->selectedPlace++;
	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong(SE->selectedPattern);
			SE->showIcon(iconPlay,SE->selectedPattern);
		}
		else
		{
			SE->switchToNewPattern();

			sequencer.playPattern();
			SE->loopPosition = SE->selectedPattern;
			SE->showIcon(iconLoop,SE->selectedPattern);
		}
	}
	else if (sequencer.getSeqState() == 1)
	{
		SE->loopPosition = -1;
		sequencer.stop();
		SE->hideIcon();
	}

	return 1;
}

static  uint8_t functRecAction()
{


	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{

	SE->eventFunct(eventSwitchModule,SE,&button,0);

	return 1;
}

//======================================================================================================================
void cSongEditor::changePatternsSelection(int16_t value)
{

	if(selectedPattern + value < 0) selectedPattern = 0;
	else if(selectedPattern + value > songLength-1) selectedPattern = songLength-1;
	else  selectedPattern += value;

	listPatterns();

	display.setControlValue(patternsListControl, selectedPattern);
	display.refreshControl(patternsListControl);

}




//======================================================================================================================
//==============================================================================================
void cSongEditor::readSong()
{
	for(uint8_t i=0;i<SONG_MAX;i++)
	{
		if(mtProject.mtProjectRemote.song.playlist[i] == 0)
		{
			songLength = i;
			break;
		}
	}

	if((songLength == (SONG_MAX-1)) || (songLength == 0)) // nie znaleziono
	{
		songLength = 1;
		selectedPattern = 0;

		mtProject.mtProjectRemote.song.playlist[0] = 1;
	}

	if(selectedPattern >= songLength) selectedPattern = 0;

	listPatterns();
}

void cSongEditor::listPatterns()
{
	for(uint8_t i=0;i<songLength;i++)
	{
		if(i<9)
		{
			if(i == selectedPattern)
			{
				sprintf(&patternsNamesList[i][0],"   %u         < %u >   ",i+1,mtProject.mtProjectRemote.song.playlist[i]);
			}
			else
			{
				sprintf(&patternsNamesList[i][0],"   %u           %u     ",i+1,mtProject.mtProjectRemote.song.playlist[i]);
			}
		}

		patternNames[i] = &patternsNamesList[i][0];
	}
}

void cSongEditor::markCurrentPattern(uint8_t forceRefresh)
{
	if(sequencer.ptrPlayer->songMode == 1)
	{
		if((mtProject.mtProjectRemote.song.playlistPos != localSongPosition) || forceRefresh)
		{
			localSongPosition = mtProject.mtProjectRemote.song.playlistPos;

			if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
			{
				showIcon(iconPlay,localSongPosition);
			}
		}
	}
}

void cSongEditor::changeGlobalTempo(int16_t value)
{
	if(mtProject.values.globalTempo+value < Sequencer::MIN_TEMPO) mtProject.values.globalTempo = Sequencer::MIN_TEMPO;
	else if(mtProject.values.globalTempo+value > Sequencer::MAX_TEMPO) mtProject.values.globalTempo = Sequencer::MAX_TEMPO;
	else  mtProject.values.globalTempo += value;

	//TODO: podpiąć afektowanie aktulanego odtwarzania

	showTempoValue();
}

void cSongEditor::changeGlobalPatternLength(int16_t value)
{
	if(mtProject.values.patternLength+value < Sequencer::MINSTEP) mtProject.values.patternLength = Sequencer::MINSTEP;
	else if(mtProject.values.patternLength+value > Sequencer::MAXSTEP) mtProject.values.patternLength = Sequencer::MAXSTEP;
	else  mtProject.values.patternLength += value;

	//TODO: podpiąć afektowanie aktulanego odtwarzania



/*
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[0].length+value < 0) pattern->track[0].length = 0;
	else if(pattern->track[0].length+value > Sequencer::MAXSTEP) pattern->track[0].length = Sequencer::MAXSTEP;
	else  pattern->track[0].length += value;

	for(uint8_t i = 1;i < 8; i++)
		pattern->track[i].length = pattern->track[0].length;

*/

	showPatternLengthValue();
}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	fileManager.patternIsChangedFlag = 1;
	fileManager.storePatternUndoRevision();

	switch (SE->selectedPlace)
	{
	case 0:

		break;
	case 1:
		// TODO: zmiana tempa z pod padów

		//sequencer.setTempo(map((float) pad, 0, 47, 10, 480));

		SE->showTempoValue();
		break;
	case 2:
		// TODO: zmiana dlugosci paternu z pod padów


		SE->showPatternLengthValue();
		break;
	}

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
			markCurrentPattern(1);
		}
		else
		{
			loopPosition = findSlotWithPattern();

			if(loopPosition >= 0)
			{
				showIcon(iconLoop,loopPosition);
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
	fileManager.savePattern(mtProject.values.actualPattern);

	mtProject.values.actualPattern = constrain(
			mtProject.mtProjectRemote.song.playlist[SE->selectedPattern], PATTERN_INDEX_MIN,
			PATTERN_INDEX_MAX);

	fileManager.loadPattern(mtProject.values.actualPattern);
	sequencer.switchRamPatternsNow();
}

int16_t cSongEditor::findSlotWithPattern()
{
	int32_t slot = 0;
	uint8_t foundFlag = 0;

	for(slot = 0; slot < songLength; slot++)
	{
		if(mtProject.mtProjectRemote.song.playlist[slot] == mtProject.values.actualPattern)
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
