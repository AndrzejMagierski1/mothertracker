

#include <songEditor.h>

#include "mtSequencer.h"


static uint16_t framesPlaces[3][4] =
{
	{0+2, 31, 800/4-5, 387},
	{(800/8)*6+2, 31, 800/8-5, 387},
	{(800/8)*7+2, 31, 800/8-5, 387},
};


void cSongEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);

	// ramka
	strControlProperties prop;
	frameData.placesCount = 3;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	prop.style = 0;
	prop.value = 0;
	prop.colors = nullptr;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

	strControlProperties prop1;
	prop1.text = (char*)"";
	prop1.style = (controlStyleShow | controlStyleCenterY | controlStyleBackground | controlStyleCenterX );
	prop1.x = (800/8);
	prop1.w = 800/4-6;
	prop1.y = 452;
	prop1.h = 59;
	if(topLabel[0] == nullptr) topLabel[0] = display.createControl<cLabel>(&prop1);


	prop1.x = (800/8)*6+5;
	prop1.y = 30;
	prop1.w = 800/8-10;
	prop1.style =  controlStyleValue_0_100 | controlStyleShow;
	prop1.h = 389;
	if(barControl[0] == nullptr)  barControl[0] = display.createControl<cBar>(&prop1);
	prop1.x = (800/8)*7+5;
	if(barControl[1] == nullptr)  barControl[1] = display.createControl<cBar>(&prop1);


	prop1.style = (controlStyleShow | controlStyleBackground | controlStyleCenterY | controlStyleCenterX );
	// inicjalizacja kontrolek
	for(uint8_t i=2; i<8; i++)
	{
		prop1.x = (800/8)*i+(800/16);
		prop1.w = 800/8-6;
		prop1.y = 437;
		prop1.h = 28;

		if(topLabel[i-1] == nullptr) topLabel[i-1] = display.createControl<cLabel>(&prop1);

		prop1.y = 465;
		prop1.h = 30;
		if(bottomLabel[i-1] == nullptr) bottomLabel[i-1] = display.createControl<cLabel>(&prop1);
	}

	patternsList.linesCount = 20;
	patternsList.start = 0;
	patternsList.length = 255;
	patternsList.data = patternNames;
	prop.x = 0+8;
	prop.y = 37;
	prop.w = (800/4-16);
	prop.h = 25;
	prop.data = &patternsList;
	if(patternsListControl == nullptr)  patternsListControl = display.createControl<cList>(&prop);

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

	for(uint8_t i=0;i<7;i++)
	{
		display.destroyControl(topLabel[i]);
		display.destroyControl(bottomLabel[i]);
		topLabel[i] = nullptr;
		bottomLabel[i]=nullptr;
	}

	display.destroyControl(frameControl);
	frameControl = nullptr;
}

void cSongEditor::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Song");
	display.refreshControl(titleLabel);


	display.setControlText(topLabel[0], "Slot / Pattern");
	display.setControlText(topLabel[1], "Delete");
	display.setControlText(topLabel[2], "Add");
	display.setControlText(topLabel[3], "Dec");
	display.setControlText(topLabel[4], "Inc");

	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");

	display.setControlText(bottomLabel[1], "slot");
	display.setControlText(bottomLabel[2], "slot");
	display.setControlText(bottomLabel[3], "pattern");
	display.setControlText(bottomLabel[4], "pattern");

	display.setControlText(bottomLabel[5], "Tempo");
	display.setControlText(bottomLabel[6], "Pat. Length");

	showTempoValue();
	showPatternLengthValue();

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	showPatternsList();
	activateLabelsBorder();

	display.synchronizeRefresh();
}


//==============================================================================================================
void cSongEditor::showPatternsList()
{
	patternsList.length = songLength;
	patternsList.linesCount = 15;
	patternsList.data = patternNames;
	patternsList.start=selectedPattern;

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
	sprintf(globalTempoVal,"%.1f", mtProject.values.globalTempo);
	display.setControlText(topLabel[5], globalTempoVal);
	display.refreshControl(topLabel[5]);

	display.setControlValue(barControl[0], (mtProject.values.globalTempo*100)/Sequencer::MAX_TEMPO);
	display.refreshControl(barControl[0]);
}


void cSongEditor::showPatternLengthValue()
{
	sprintf(patternLengthVal,"%d", mtProject.values.patternLength+1);
	display.setControlText(topLabel[6], patternLengthVal);
	display.refreshControl(topLabel[6]);

	display.setControlValue(barControl[1], (mtProject.values.patternLength*100)/Sequencer::MAXSTEP);
	display.refreshControl(barControl[1]);
}

void cSongEditor::showIcon(icon_t iconType,uint8_t position)
{
	iconPos = position;

	patternsList.icon.useSpecialIcons = 1;

	patternsList.icon.iconPositionInList = position;
	patternsList.icon.iconNum = iconType;

	display.setControlValue(patternsListControl, selectedPattern);
	display.refreshControl(patternsListControl);
}

void cSongEditor::hideIcon()
{
	iconPos = -1;

	patternsList.icon.useSpecialIcons = 0;
	patternsList.icon.iconNum = 0;
	patternsList.icon.iconPositionInList = 0;

	display.setControlValue(patternsListControl, selectedPattern);
	display.refreshControl(patternsListControl);
}

