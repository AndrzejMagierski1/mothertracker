

#include <songEditor.h>
#include "mtFileManager.h"
#include "Encoder.h"



#include "mtAudioEngine.h"


cSongEditor songEditor;

static cSongEditor* SE = &songEditor;







static  uint8_t functIncPattern();
static  uint8_t functDecPattern();
static  uint8_t functDeleteSlot();
static  uint8_t functAddSlot();

static  uint8_t functUp();
static  uint8_t functDown();

static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);

static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);



void cSongEditor::update()
{

	markCurrentPattern();

}

void cSongEditor::start(uint32_t options)
{
	moduleRefresh = 1;


	readSong();


	showPatternsList();

	Encoder.setResolution(12);
	Encoder.setAcceleration(0);


	//selectedPlace = 0;
	activateLabelsBorder();

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

	 //mtProject.mtProjectRemote.song.


}

void cSongEditor::stop()
{
	Encoder.setResolution(48);
	Encoder.setAcceleration(3);

	moduleRefresh = 0;
}

void cSongEditor::setDefaultScreenFunct()
{
	//funkcje
	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functDecPattern);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functIncPattern);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);

	FM->setButtonObj(interfaceButton2, buttonPress, functDeleteSlot);
	FM->setButtonObj(interfaceButton3, buttonPress, functAddSlot);
	FM->setButtonObj(interfaceButton4, buttonPress, functDecPattern);
	FM->setButtonObj(interfaceButton5, buttonPress, functIncPattern);






}


//==============================================================================================================

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

	return 1;
}

static  uint8_t functDeleteSlot()
{
	mtProject.mtProjectRemote.song.playlist[SE->songLength]=0;

	for(int i = SE->selectedPattern; i < SE->songLength ; i++)
	{
		mtProject.mtProjectRemote.song.playlist[i] = mtProject.mtProjectRemote.song.playlist[i+1];
	}

	if(SE->songLength)
	{
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
	return 1;
}




//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	switch(SE->selectedPlace)
	{
	case 0: SE->changePatternsSelection(value); break;
	case 1: break;
	case 2: break;
	case 3: break;
	case 4: break;

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
	case 0:SE->changePatternsSelection(-1); break;
	case 1: break;
	case 2: break;
	case 3: break;
	case 4: break;
	}

	return 1;
}

static  uint8_t functDown()
{
	switch(SE->selectedPlace)
	{
	case 0:SE->changePatternsSelection(1); break;
	case 1: break;
	case 2: break;
	case 3: break;
	case 4: break;
	}

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
		if(i==SONG_MAX) // nie znaleziono
		{
			songLength = 1;
			selectedPattern = 0;
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

	patternsNamesList[mtProject.mtProjectRemote.song.playlistPos][1]='*';
}

void cSongEditor::markCurrentPattern()
{
	if(mtProject.mtProjectRemote.song.playlistPos != localSongPosition)
	{
		patternsNamesList[mtProject.mtProjectRemote.song.playlistPos][1]='*';

		if(mtProject.mtProjectRemote.song.playlistPos == 0)
		{
			patternsNamesList[songLength-1][1]=' ';
		}
		else
		{
			patternsNamesList[mtProject.mtProjectRemote.song.playlistPos-1][1]=' ';
		}

		display.setControlValue(patternsListControl, selectedPattern);
		display.refreshControl(patternsListControl);
	}
}



