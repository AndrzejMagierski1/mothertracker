

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

	markCurrentPattern();

}

void cSongEditor::start(uint32_t options)
{
	moduleRefresh = 1;


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

	 //mtProject.mtProjectRemote.song.


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
	FM->setButtonObj(interfaceButton6, buttonPress, functTempo);
	FM->setButtonObj(interfaceButton7, buttonPress, functPatternLength);



	FM->setPadsGlobal(functPads);


}


//==============================================================================================================

static  uint8_t functPatternSlot()
{
	SE->selectedPlace = 0;
	SE->activateLabelsBorder();
}

static  uint8_t functIncPattern()
{
	if(mtProject.mtProjectRemote.song.playlist[SE->selectedPattern] < 99)
	{
		mtProject.mtProjectRemote.song.playlist[SE->selectedPattern] += 1;
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
}

static  uint8_t functPatternLength()
{
	SE->selectedPlace = 2;
	SE->activateLabelsBorder();
}

//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	switch(SE->selectedPlace)
	{
	case 0: SE->changePatternsSelection(value); 	break;
	case 1: SE->changeGlobalTempo(value);			break;
	case 2: SE->changeGlobalPatternLength(value); 	break;

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
	case 2:	SE->changeGlobalPatternLength(1);	break;
	}

	return 1;
}

static  uint8_t functDown()
{
	switch(SE->selectedPlace)
	{
	case 0:	SE->changePatternsSelection(1); 	break;
	case 1:	SE->changeGlobalTempo(-1); 			break;
	case 2:	SE->changeGlobalPatternLength(-1); 	break;
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
	if (SE->songLength == 0)
	{
//		sequencer.playPattern();

		if (sequencer.getSeqState() == 0)
		{
			sequencer.playPattern();
		}
		else if (sequencer.getSeqState() == 1)
		{
			sequencer.stop();
		}
	}
	else if (sequencer.getSeqState() == 0)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
		}
		else
		{
			sequencer.playSong(SE->selectedPattern);
		}

	}
	else if (sequencer.getSeqState() == 1)
	{
		sequencer.stop();

	}

//	display.refreshControl(patternsListControl);

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
		uint8_t breakFlag = 0;
		if(mtProject.mtProjectRemote.song.playlist[i] == 0)
		{
			songLength = i;
			breakFlag = 1;
		}
		if((i == SONG_MAX) || (i == 0)) // nie znaleziono
		{
			songLength = 1;
			selectedPattern = 0;

			if((i == 0) && (mtProject.mtProjectRemote.song.playlist[i] == 0))
			{
				mtProject.mtProjectRemote.song.playlist[i] = 1;
			}
		}

		if(breakFlag)
		{
			break;
		}
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
		else if(i<99)
		{
			if(i == selectedPattern)
			{
				sprintf(&patternsNamesList[i][0],"  %u         < %u >   ",i+1,mtProject.mtProjectRemote.song.playlist[i]);
			}
			else
			{
				sprintf(&patternsNamesList[i][0],"  %u           %u     ",i+1,mtProject.mtProjectRemote.song.playlist[i]);
			}
		}

		patternNames[i] = &patternsNamesList[i][0];
	}

	clearPatternMark();
	patternsNamesList[mtProject.mtProjectRemote.song.playlistPos][CURR_PATTERN_MARK_POS]='*';
}

void cSongEditor::clearPatternMark()
{
	for(uint32_t i = 0; i < songLength; i++)
	{
		patternsNamesList[i][CURR_PATTERN_MARK_POS] = ' ';
	}
}

void cSongEditor::markCurrentPattern()
{
	if(mtProject.mtProjectRemote.song.playlistPos != localSongPosition)
	{
		clearPatternMark();
		patternsNamesList[mtProject.mtProjectRemote.song.playlistPos][CURR_PATTERN_MARK_POS] = '*';

		display.setControlValue(patternsListControl, selectedPattern);
		display.refreshControl(patternsListControl);
	}
}

void cSongEditor::changeGlobalTempo(int16_t value)
{
	if(mtProject.values.globalTempo+value < Sequencer::MIN_TEMPO) mtProject.values.globalTempo = Sequencer::MIN_TEMPO;
	else if(mtProject.values.globalTempo+value > Sequencer::MAX_TEMPO) mtProject.values.globalTempo = Sequencer::MAX_TEMPO;
	else  mtProject.values.globalTempo += value;

	showTempoValue();
}

void cSongEditor::changeGlobalPatternLength(int16_t value)
{
	if(mtProject.values.patternLength+value < Sequencer::MINSTEP) mtProject.values.patternLength = Sequencer::MINSTEP;
	else if(mtProject.values.patternLength+value > Sequencer::MAXSTEP) mtProject.values.patternLength = Sequencer::MAXSTEP;
	else  mtProject.values.patternLength += value;

	showPatternLengthValue();
}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{

	// obsługa przycisków pod ekranem

	if (SE->selectedPlace >= 0)
	{
		fileManager.patternIsChangedFlag = 1;
		fileManager.storePatternUndoRevision();

		switch (SE->selectedPlace)
		{
		case 0:

			break;
		case 1:
			sequencer.setTempo(map((float) pad, 0, 47, 10, 480));
			SE->showTempoValue();
			break;
		case 2:
			SE->changeGlobalPatternLength(map(pad, 0, 47, 3, 191));
			SE->showPatternLengthValue();
			break;
		}

		return 1;
	}

	return 1;
}



