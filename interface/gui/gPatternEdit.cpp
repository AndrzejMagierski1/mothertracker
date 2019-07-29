
#include <patternEditor.h>
#include "mtStructs.h"





void cPatternEditor::initDisplayControls()
{
	// inicjalizacja kontrolek

	for(uint8_t i = 0; i<8; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.data =  &bottomValuesConfig;
		prop2.style = 	( controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 460;
		prop2.w = 800/8-10;
		prop2.h = 20;


		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 431;
		prop2.h = 20;

		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
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

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}
}


void cPatternEditor::showDefaultScreen()
{

	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	//display.setControlShow(patternControl);
	display.refreshControl(patternControl);

	// bottom labels
	display.setControlText(bottomLabel[0], "Tempo");
	display.setControlText(bottomLabel[1], "Pattern");
	display.setControlText(bottomLabel[2], "Length");
	display.setControlText(bottomLabel[3], "Step");
	display.setControlText(bottomLabel[4], "Fill");
	display.setControlText(bottomLabel[5], "Random");
	display.setControlText(bottomLabel[6], "Invert");
	display.setControlText(bottomLabel[7], "");

	display.setControlText(topLabel[0], "");
	display.setControlText(topLabel[1], "");
	display.setControlText(topLabel[2], "");
	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");
	display.setControlText(topLabel[7], "");

//	display.setControlValue(bottomLabel[0], pattern->tempo);
//	display.setControlValue(bottomLabel[1], 1);
//	display.setControlValue(bottomLabel[2], pattern->track[0].length+1);
//	display.setControlValue(bottomLabel[3], mtProject.values.patternEditStep);




	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	display.synchronizeRefresh();




}

// ustawianie stylu ramki do wybranego labela przycisku (bottomLabel[selectedLabel])
void cPatternEditor::activateLabelsBorder()
{
	for(uint8_t i = 0; i<4; i++)
	{
		display.setRemoveControlStyle(bottomLabel[i], controlStyleBorder);
		display.refreshControl(bottomLabel[i]);
	}

	if(selectedLabel >= 0)
	{
		display.setAddControlStyle(bottomLabel[selectedLabel], controlStyleBorder);
		//trackerPattern.selectActive = 0;
	}
}

