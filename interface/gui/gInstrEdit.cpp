



#include <instrumentEditor.h>


static uint16_t framesPlaces[8][4] =
{
	{0, 		30, 800/8, 450},
	{(800/8)*1, 30, 800/8, 450},
	{(800/8)*2, 30, 800/8, 450},
	{(800/8)*3, 30, 800/8, 450},
	{(800/8)*4, 30, 800/8, 450},
	{(800/8)*5, 30, 800/8, 450},
	{(800/8)*6, 30, 800/8, 450},
	{(800/8)*7, 30, 800/8, 450},
};




void cInstrumentEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.text = (char*)"";
	prop2.style = 	( controlStyleShow | controlStyleBackground | controlStyleCenterX);
	prop2.x = 400;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);


		prop2.y = 415;
		prop2.h = 26;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);


		prop2.x = (800/8)*i+5;
		prop2.y = 30;
		prop2.w = 800/8-10;
		prop2.style =  controlStyleShow | controlStyleValue_0_100;
		prop2.h = 380;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);
	}


	// ramka
	strControlProperties prop;
	frameData.placesCount = 8;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];
	prop.style = controlStyleShow;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

}


void cInstrumentEditor::destroyDisplayControls()
{
	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		display.destroyControl(barControl[i]);
		barControl[i] = nullptr;
	}

	display.destroyControl(frameControl);
	frameControl = nullptr;
}

void cInstrumentEditor::showDefaultScreen()
{

	// bottom labels
	display.setControlText(bottomLabel[0], "Start");
	display.setControlText(bottomLabel[1], "Loop Start");
	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlText(bottomLabel[3], "End");
	display.setControlText(bottomLabel[4], "- Zoom");
	display.setControlText(bottomLabel[5], "+ Zoom");
	display.setControlText(bottomLabel[6], "Play Mode");
	//display.setControlText(bottomLabel[7], "End");

	display.refreshControl(titleLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(barControl[i]);
	}

	display.synchronizeRefresh();

}

void cInstrumentEditor::showInstrumentVolume()
{
	display.setControlText(titleLabel, "Instrument Volume");

	display.setControlText(bottomLabel[0], "Attack");
	display.setControlText(bottomLabel[1], "Decay");
	display.setControlText(bottomLabel[2], "Sustain");
	display.setControlText(bottomLabel[3], "Release");
	display.setControlText(bottomLabel[4], "Amount");
	display.setControlText(bottomLabel[5], "");
	display.setControlText(bottomLabel[6], "");
	display.setControlText(bottomLabel[7], "");


	display.setControlText(topLabel[0], "");
	display.setControlText(topLabel[1], "");
	display.setControlText(topLabel[2], "");
	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");
	display.setControlText(topLabel[7], "");


	display.refreshControl(titleLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(barControl[i]);
	}


	display.synchronizeRefresh();

}

void cInstrumentEditor::showInstrumentFilter()
{
	display.setControlText(titleLabel, "Instrument Filter");

	display.refreshControl(titleLabel);
}

void cInstrumentEditor::showInstrumentParams()
{
	display.setControlText(titleLabel, "Instrument Parameters");

	display.refreshControl(titleLabel);
}

//==============================================================================================================
void cInstrumentEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}





//==============================================================================================================
