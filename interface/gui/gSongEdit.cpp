

#include <songEditor.h>


static uint16_t framesPlaces[5][4] =
{
	{0, 0, 800/2, 480},
	{(800/8)*4, 0, 800/8, 480},
	{(800/8)*5, 0, 800/8, 480},
	{(800/8)*6, 0, 800/8, 480},
	{(800/8)*7, 0, 800/8, 480},
};


void cSongEditor::initDisplayControls()
{
	strControlProperties prop1;

	prop1.text = (char*)"";
	prop1.style = 	( controlStyleShow | controlStyleBackground | controlStyleCenterX);
	prop1.x = 400;
	prop1.y = 0;
	prop1.w = 800;
	prop1.h = 25;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop1);


	prop1.text = (char*)"";
	prop1.style = (controlStyleShow | controlStyleCenterY  | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop1.x = (800/8);
	prop1.w = 800/4-10;
	prop1.y = 415;
	prop1.h = 65;
	if(topLabel[0] == nullptr) topLabel[0] = display.createControl<cLabel>(&prop1);

	prop1.style = (controlStyleShow| controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	// inicjalizacja kontrolek
	for(uint8_t i=2; i<8; i++)
	{
		prop1.x = (800/8)*i+(800/16);
		prop1.y = 415;
		prop1.w = 800/8-10;
		prop1.h = 32;

		if(topLabel[i-1] == nullptr) topLabel[i-1] = display.createControl<cLabel>(&prop1);

		//prop2.x = (800/4)*i+(800/8);
		prop1.y = 450;
		//prop2.w = 800/4-10;
		prop1.h = 30;

		if(bottomLabel[i-1] == nullptr) bottomLabel[i-1] = display.createControl<cLabel>(&prop1);
	}

	patternsList.linesCount = 20;
	patternsList.start = 0;
	patternsList.length = 255;
	patternsList.data = patternNames;
	strControlProperties prop;
	prop.x = 0+5;
	prop.y = 30;
	prop.w = (800/4-10)-5;
	prop.h = 25;
	prop.data = &patternsList;
	if(patternsListControl == nullptr)  patternsListControl = display.createControl<cList>(&prop);
}


void cSongEditor::destroyDisplayControls()
{
	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	for(uint8_t i=0;i<7;i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	display.destroyControl(patternsListControl);
	patternsListControl = nullptr;
}

void cSongEditor::showDefaultScreen()
{
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
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	showPatternsList();

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
