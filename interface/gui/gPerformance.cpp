



#include <performanceMode.h>


static uint16_t framesPlaces[8][4] =
{
	{0+2, 		31, 800/8-5, 387},
	{(800/8)*1+2, 31, 800/8-5, 387},
	{(800/8)*2+2, 31, 800/8-5, 387},
	{(800/8)*3+2, 31, 800/8-5, 387},
	{(800/8)*4+2, 31, 800/8-5, 387},
	{(800/8)*5+2, 31, 800/8-5, 387},
	{(800/8)*6+2, 31, 800/8-5, 387},
	{(800/8)*7+2, 31, 800/8-5, 387},
};

static uint32_t textLabelsColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};


void cPerformanceMode::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


//  	strControlProperties prop;
/*
	// ramka
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
	prop.style = 0;
	prop.value = 0;
	prop.colors = nullptr;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);
*/

	strControlProperties prop;
	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;
		prop2.w = 800/8-6;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.y = 437;
		prop2.h = 28;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);


	}


	for(uint8_t i = 0; i<12; i++)
	{
		prop2.text = (char*)"";
		prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
		prop.x = (800/8)*i+(800/16);
		prop.y = 240;
		prop.colors = textLabelsColors;
		//prop2.w = 800/8-6;
		//prop2.h = 30;
		if(textLabel[i] == nullptr) textLabel[i] = display.createControl<cLabel>(&prop);
	}

}


void cPerformanceMode::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;


	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	for(uint8_t i = 0; i<12; i++)
	{
		display.destroyControl(textLabel[i]);
		textLabel[i] = nullptr;
	}



	display.destroyControl(frameControl);
	frameControl = nullptr;


}

/*
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
*/

void cPerformanceMode::showPerformanceMaster()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Performance Master");
	display.refreshControl(titleLabel);

	//showActualInstrument();


	display.setControlText(bottomLabel[0], "Track 1");
	display.setControlText(bottomLabel[1], "Track 2");
	display.setControlText(bottomLabel[2], "Track 3");
	display.setControlText(bottomLabel[3], "Track 4");
	display.setControlText(bottomLabel[4], "Track 5");
	display.setControlText(bottomLabel[5], "Track 6");
	display.setControlText(bottomLabel[6], "Track 7");
	display.setControlText(bottomLabel[7], "Track 8");


//-------------------------------------

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlPosition(bottomLabel[i], -1, 452);
		display.setControlSize(bottomLabel[i], -1, 59);

		display.setControlHide(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
		//display.refreshControl(topLabel[i]);

		//TO DO: nie powinno tu tego być tylko tma gdzie zarzadzanie zmiennymi projektu
		if(mtProject.values.trackMute[i] >= trackMasterModeCount) mtProject.values.trackMute[i] = 0;

		display.setControlText(textLabel[i], &trackMasterLabels[mtProject.values.trackMute[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);
	}

	display.setControlHide(textLabel[8]);
	display.setControlHide(textLabel[9]);
	display.setControlHide(textLabel[10]);
	display.setControlHide(textLabel[11]);


	display.synchronizeRefresh();

}


void cPerformanceMode::showPerformanceFxes()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Performance Fxes");
	display.refreshControl(titleLabel);

	//showActualInstrument();


	display.setControlText(bottomLabel[0], "Track 1");
	display.setControlText(bottomLabel[1], "Track 2");
	display.setControlText(bottomLabel[2], "Track 3");
	display.setControlText(bottomLabel[3], "Track 4");
	display.setControlText(bottomLabel[4], "Track 5");
	display.setControlText(bottomLabel[5], "Track 6");
	display.setControlText(bottomLabel[6], "Track 7");
	display.setControlText(bottomLabel[7], "Track 8");

	display.setControlText(topLabel[0], "[ ]");
	display.setControlText(topLabel[1], "[ ]");
	display.setControlText(topLabel[2], "[ ]");
	display.setControlText(topLabel[3], "[ ]");
	display.setControlText(topLabel[4], "[ ]");
	display.setControlText(topLabel[5], "[ ]");
	display.setControlText(topLabel[6], "[ ]");
	display.setControlText(topLabel[7], "[ ]");

//-------------------------------------

	for(uint8_t i = 0; i<trackFxesCount; i++)
	{
		display.setControlText(textLabel[i], &trackFxesLabels[i][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);
	}


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlPosition(bottomLabel[i], -1, 465);
		display.setControlSize(bottomLabel[i], -1, 30);

		display.setControlShow(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}


	display.synchronizeRefresh();
}



//==============================================================================================================
void cPerformanceMode::activateLabelsBorder()
{
	if(selectedPlace[mode] > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace[mode]);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================
