
#include <patternEditor.h>
#include "mtStructs.h"





void cPatternEditor::initDisplayControls()
{
	// inicjalizacja kontrolek

	for(uint8_t i = 0; i<4; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.data =  &bottomValuesConfig;
		prop2.style = 	( controlStyleShowValue | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/4)*i+(800/8);
		prop2.y = 450;
		prop2.w = 800/4-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}



	strControlProperties prop;
	//prop.text = (char*)"";
	prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop.x = 0;
	prop.y = 0;
	prop.w = 50;
	prop.h = 25;
	prop.data = &trackerPattern;
	if(patternControl == nullptr)  patternControl = display.createControl<cTracker>(&prop);
	//hTrackControl = display.createControl<cLabel>(&prop);
	//display.refreshControl(hTrackControl);
}


void cPatternEditor::destroyDisplayControls()
{
	display.destroyControl(patternControl);
	patternControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}
}


void cPatternEditor::showDefaultScreen()
{

	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	//display.setControlShow(patternControl);
	display.refreshControl(patternControl);

	// bottom labels
	display.setControlText(bottomLabel[0], "-     BPM:         +");
	display.setControlText(bottomLabel[1], "-    Pattern:      +");
	display.setControlText(bottomLabel[2], "-    Length:       +");
	display.setControlText(bottomLabel[3], "-      Step:       +");

	display.setControlValue(bottomLabel[0], pattern->tempo);
	display.setControlValue(bottomLabel[1], 1);
	display.setControlValue(bottomLabel[2], pattern->track[0].length);
	display.setControlValue(bottomLabel[3], mtProject.values.patternEditStep);


	display.setControlShow(bottomLabel[0]);
	display.setControlShow(bottomLabel[1]);
	display.setControlShow(bottomLabel[2]);
	display.setControlShow(bottomLabel[3]);

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();




}
