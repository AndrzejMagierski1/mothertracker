

#include "songEditor/songEditor.h"

#include "mtSequencer.h"
#include "keyScanner.h"

static uint16_t framesPlaces[3][4] =
{
	{(800/8)*0+1, 29, 500-3, 391},
	{(800/8)*5+1, 29, 800/4-3, 391},
	{(800/8)*7+1, 29, 800/8-3, 391},
	//{(800/8)*7+1, 29, 800/8-3, 391},
};


void cSongEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);

	// ramka
	strControlProperties prop;
	frameData.placesCount = 10;
	frameData.startPlace = 0;

	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[0][0];
	frameData.places[2] = &framesPlaces[0][0];
	frameData.places[3] = &framesPlaces[0][0];
	frameData.places[4] = &framesPlaces[0][0];
	frameData.places[5] = &framesPlaces[0][0];
	frameData.places[6] = &framesPlaces[0][0];
	frameData.places[7] = &framesPlaces[0][0];
	frameData.places[8] = &framesPlaces[1][0];
	frameData.places[9] = &framesPlaces[2][0];

	prop.style = 0;
	prop.value = 0;
	prop.colors = nullptr;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

	prop.x = 190;
	prop.y = 170;
	prop.style = controlStyleValue_0_100;
	prop.h = 100;
	prop.w = 420;
	if(loadHorizontalBarControl == nullptr)  loadHorizontalBarControl = display.createControl<cHorizontalBar>(&prop);

	strControlProperties prop1;

	prop1.x = (800/8)*6+1;
	prop1.y = 29;
	prop1.w = 800/8-3;
	prop1.style =  controlStyleValue_0_100 | controlStyleShow | controlStyleBackground;
	prop1.h = 394;
	if(barControl[0] == nullptr)  barControl[0] = display.createControl<cBar>(&prop1);
	prop1.x = (800/8)*7+1;
	if(barControl[1] == nullptr)  barControl[1] = display.createControl<cBar>(&prop1);


	doubleArrow[0].bitmaps[0].bitmapIndex = displayDoubleArrowL;
	doubleArrow[0].bitmaps[0].xValue =  (800/8)*5+(800/16);
	doubleArrow[0].bitmaps[0].yValue = 460;
	doubleArrow[1].bitmaps[0].bitmapIndex = displayDoubleArrowR;
	doubleArrow[1].bitmaps[0].xValue =  (800/8)*6+(800/16);
	doubleArrow[1].bitmaps[0].yValue = 460;

	prop2.text = (char*)"";
	prop2.colors = interfaceGlobals.activeButtonLabelsColors;
	prop2.style = 	(controlStyleCenterX | controlStyleFont1);

	for(uint8_t i = 0; i < 8; i++)
	{
		prop2.x = (800/8)*i+(800/16);
		if(i == 5) prop2.x = (800/8)*i+(800/8);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		prop2.value =  1;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.value = 191;
	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);

//	prop.x = 12;
//	prop.y = 40 + 27;
//	prop.w = 600;
//	prop.h = 300;

	prop.data = nullptr;
	prop.x = 0+1;
	prop.y = 29;
	prop.w = ((800/8)*7)-3;
	prop.h = 394;
	prop.style = controlStyleBackground;

	if(songPlayerControl == nullptr)  songPlayerControl = display.createControl<cSongPlayer>(&prop);
}


void cSongEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;


	display.destroyControl(barControl[0]);
	barControl[0] = nullptr;
	display.destroyControl(barControl[1]);
	barControl[1] = nullptr;

	for(uint8_t i=0;i<8;i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(songPlayerControl);
	songPlayerControl = nullptr;

	display.destroyControl(loadHorizontalBarControl);
	loadHorizontalBarControl = nullptr;
}

void cSongEditor::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Song");
	display.refreshControl(titleLabel);


	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont1);
	}
	display.setControlStyle2(label[7], controlStyleCenterX | controlStyleFont2);

//	display.setControlValue(label[0], 1);
//	display.setControlValue(label[1], 1);
//	display.setControlValue(label[2], 0);
//	display.setControlValue(label[3], 0);
//	display.setControlValue(label[4], 0);
//	display.setControlValue(label[5], 1);
//	display.setControlValue(label[6], 1);

	display.setControlData(label[5], &doubleArrow[0]);
	display.setControlData(label[6], &doubleArrow[1]);
	display.setAddControlStyle(label[5], controlStyleShowBitmap);
	display.setAddControlStyle(label[6], controlStyleShowBitmap);

	byttonsState = 0;

	display.setControlText(label[0], "Play");
	display.setControlText2(label[0], "Pattern");

	display.setControlText(label[1], "Play");
	display.setControlText2(label[1], "Song");

	display.setControlText(label[2], "Undo");

	display.setControlText(label[3], "Add");
	display.setControlText2(label[3], "Slot");

	display.setControlText(label[4], "Delete");
	display.setControlText2(label[4], "Slot");

	display.setControlText2(label[5], "");
	display.setControlText(label[5], "Pattern");

	display.setControlText(label[6], "");
	display.setControlText2(label[6], "");

	display.setControlText(label[7], "Tempo");
	if (sequencer.isInternalClock())
	{
		display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);
	}
	else
	{
		display.setControlColors(label[7],
									interfaceGlobals.inactiveLabelsColors);
	}
	showTempoValue();
	//showPatternLengthValue();

	for(uint8_t i = 0; i< 8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.refreshControl(bgLabel);

	showPatternsList();
	activateLabelsBorder();

	display.setControlShow(songPlayerControl);
	display.refreshControl(songPlayerControl);

	display.synchronizeRefresh();
}


//==============================================================================================================
void cSongEditor::showPatternsList()
{
	patternsList.length = songLength;
	patternsList.linesCount = 13;
	patternsList.data = patternNames;
	patternsList.start = selectedPattern;

    display.setControlData(songPlayerControl, &songPlayerData);
	display.setControlValue(songPlayerControl, selectedPattern);
}


void cSongEditor::refreshButtonsLabels()
{

	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		if(tactButtons.isButtonPressed(interfaceButtonShift) && selectedPlace < 8
			&& songPlayerData.selection.trackSelectionLength == 1
			&& songPlayerData.selection.patternSelectionLength == 1)
		{
			if(byttonsState != 2)
			{
				display.setControlText(label[0], "Preview");
				display.setControlText2(label[0], "Track");

				display.setControlText(label[1], "Play");
				display.setControlText2(label[1], "Song");

				display.refreshControl(label[0]);
				display.refreshControl(label[1]);
				byttonsState = 2;
			}
		}
		else
		{
			if(byttonsState != 0)
			{
				display.setControlText(label[0], "Play");
				display.setControlText2(label[0], "Pattern");

				display.setControlText(label[1], "Play");
				display.setControlText2(label[1], "Song");

				display.refreshControl(label[0]);
				display.refreshControl(label[1]);
				byttonsState = 0;
			}
		}

	}
	else
	{
		if(byttonsState != 1)
		{
			display.setControlText(label[0], "Stop");
			display.setControlText2(label[0], "");

			display.setControlText(label[1], "Stop");
			display.setControlText2(label[1], "");

			display.refreshControl(label[0]);
			display.refreshControl(label[1]);
			byttonsState = 1;
		}
	}



}


//==============================================================================================================

void cSongEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	if(selectedPlace > 7)
	{
		songPlayerData.selection.isActive = 0;
		display.refreshControl(songPlayerControl);
	}
	else
	{
		display.setControlHide(frameControl);
		display.refreshControl(frameControl);
		return;
	}

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}



void cSongEditor::showTempoValue()
{
	if (sequencer.isInternalClock())
	{
		sprintf(globalTempoVal,"%.1f BPM", mtProject.values.globalTempo);
	}
	else
	{
		sprintf(globalTempoVal,"EXT");
	}
	display.setControlText2(label[7], globalTempoVal);
	display.refreshControl(label[7]);

	display.setControlValue(barControl[1], (mtProject.values.globalTempo*100)/Sequencer::MAX_TEMPO);
	display.refreshControl(barControl[1]);
}

void cSongEditor::showIcon(icon_t iconType,uint8_t position)
{
	patternsList.icon.useSpecialIcons = 1;
	patternsList.icon.iconPositionInList = position;
	patternsList.icon.iconNum = displayPlayIcon+iconType;

	display.setControlValue(songPlayerControl, selectedPattern);
	display.refreshControl(songPlayerControl);
}

void cSongEditor::hideIcon()
{
	songPlayerData.progress.isPlaying = 0;

	patternsList.icon.useSpecialIcons = 0;
	patternsList.icon.iconNum = displayPlayIcon;
	patternsList.icon.iconPositionInList = 0;

	display.setControlValue(songPlayerControl, selectedPattern);
	display.refreshControl(songPlayerControl);
}

void cSongEditor::showHorizontalBar(const char *text, uint32_t max, uint32_t current)
{
	uint8_t progress = max/current;

	display.setControlValue(loadHorizontalBarControl, progress);
	display.setControlText(loadHorizontalBarControl, text);
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSongEditor::hideHorizontalBar()
{
	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

