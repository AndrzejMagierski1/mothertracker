

#include "songTimer.h"

#include "display.h"

#include "mtSequencer.h"
#include "patternEditor/patternEditor.h"


cSongTimer songTimer;

static elapsedMillis refreshTimer;
static elapsedMillis songTimeCounter;

static uint32_t textLabelColors[] =
{
	0x000000, // tekst
	0xFFFFFF, // tło
	0xFF0000, // ramka
};



void cSongTimer::initControls()
{
	strControlProperties prop1;
	prop1.style = controlStyleFont4 | controlStyleCenterY;
	prop1.x = 280;
	prop1.y = 0;
	prop1.h = 26;
	prop1.colors = textLabelColors;
	if(timerLabel == nullptr) timerLabel = display.createControl<cLabel>(&prop1);



}


void cSongTimer::show()
{
	refresh();

	display.setControlShow(timerLabel);
	display.refreshControl(timerLabel);

	visible = true;
}

void cSongTimer::hide()
{
	display.setControlHide(timerLabel);
	display.refreshControl(timerLabel);

	visible = false;
}

void cSongTimer::start()
{
	songTimeCounter = 0;
	counting = true;
}

void cSongTimer::stop()
{
	actualData.minutes = 0;
	actualData.seconds = 0;
	counting = false;
}

void cSongTimer::refresh()
{
	if(refreshTimer < 100 || !visible) return;
	refreshTimer = 0;

	if(counting)
	{
		actualData.minutes = songTimeCounter/60000;
		actualData.seconds = (songTimeCounter - (actualData.minutes*60000))/1000;
	}

	if(sequencer.isPlay()) 	actualData.step = sequencer.ptrPlayer->track[0].actual_pos+1;
	else 					actualData.step = patternEditor.trackerPattern.actualStep+1;

	// przerwanie operacji jesli dane do wyswietlenia nie ulegly zmianie
	if(lastData.step == actualData.step
	&& lastData.seconds == actualData.seconds
	&& lastData.minutes == actualData.minutes
	&& lastData.pattern == mtProject.values.actualPattern) return;

	lastData.step = actualData.step;
	lastData.seconds = actualData.seconds;
	lastData.minutes = actualData.minutes;
	lastData.pattern = 	mtProject.values.actualPattern;

	sprintf(timerText, "%3im :%2is |%3iP %3iS", actualData.minutes, actualData.seconds, mtProject.values.actualPattern, actualData.step);

	display.setControlText(timerLabel, timerText);
	display.refreshControl(timerLabel);
}
