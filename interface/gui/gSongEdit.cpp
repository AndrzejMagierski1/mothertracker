

#include <songEditor.h>

#include "mtSequencer.h"


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

	prop2.text = (char*)"";
	prop2.colors = interfaceGlobals.activeLabelsColors;
	prop2.style = 	(controlStyleCenterX | controlStyleCenterY);

	for(uint8_t i = 0; i < 8; i++)
	{
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 452;
		prop2.h =  59;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.value = 252;
	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);

	patternsList.linesCount = 14;
	patternsList.start = 0;
	patternsList.length = 255;
	patternsList.data = patternNames;
	prop.x = 500+8;
	prop.y = 37;
	prop.w = (800/4-16);
	prop.h = 25;

	prop.data = &patternsList;
	if(patternsListControl == nullptr)  patternsListControl = display.createControl<cList>(&prop);


	prop.x = 12;
	prop.y = 40;
	prop.w = 600;
	prop.h = 300;

	if(songPlayerControl == nullptr)  songPlayerControl = display.createControl<cSongPlayer>(&prop);
}


void cSongEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(patternsListControl);
	patternsListControl = nullptr;

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

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 0);
	display.setControlValue(label[3], 0);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 1);


	display.setControlText(label[0], "Add");
	display.setControlText2(label[0], "Row");

	display.setControlText(label[1], "Delete");
	display.setControlText2(label[1], "Row");

	display.setControlText(label[2], "Copy");

	display.setControlText(label[3], "Paste");

	display.setControlText(label[4], "");

	display.setControlText(label[5], "+");
	display.setControlText2(label[5], "pattern");

	display.setControlText(label[6], "-");
	display.setControlText2(label[6], "pattern");

	display.setControlText2(label[7], "Tempo");

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
	patternsList.linesCount = 14;
	patternsList.data = patternNames;
	patternsList.start = selectedPattern;

    display.setControlData(patternsListControl,  &patternsList);
	//display.setControlValue(patternsListControl, selectedPattern);
	display.setControlShow(patternsListControl);
	display.refreshControl(patternsListControl);

}


//==============================================================================================================

void cSongEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}



void cSongEditor::showTempoValue()
{
	sprintf(globalTempoVal,"%.1f BPM", mtProject.values.globalTempo);
	display.setControlText(label[7], globalTempoVal);
	display.refreshControl(label[7]);

	display.setControlValue(barControl[1], (mtProject.values.globalTempo*100)/Sequencer::MAX_TEMPO);
	display.refreshControl(barControl[1]);
}

void cSongEditor::showIcon(icon_t iconType,uint8_t position)
{
	patternsList.icon.useSpecialIcons = 1;

	patternsList.icon.iconPositionInList = position;
	patternsList.icon.iconNum = displayPlayIcon+iconType;

	display.setControlValue(patternsListControl, selectedPattern);
	display.refreshControl(patternsListControl);
}

void cSongEditor::hideIcon()
{
	songPlayerData.progress.isPlaying = 0;

	patternsList.icon.useSpecialIcons = 0;
	patternsList.icon.iconNum = displayPlayIcon;
	patternsList.icon.iconPositionInList = 0;

	display.setControlValue(patternsListControl, selectedPattern);
	display.refreshControl(patternsListControl);
}

void cSongEditor::showCopyingBar()
{
	uint8_t progress = (copyElementMax / currentCopyElement);

	display.setControlValue(loadHorizontalBarControl, selectedPattern);
	display.setControlText(loadHorizontalBarControl, "Copying");
	display.setControlShow(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

void cSongEditor::hideCopyingBar()
{
	display.setControlHide(loadHorizontalBarControl);
	display.refreshControl(loadHorizontalBarControl);
}

