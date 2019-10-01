



#include <performanceMode.h>


static uint16_t framesPlaces[12][4] =
{
	{0,			 31, 800/12, 387},
	{(800/12)*1, 31, 800/12, 387},
	{(800/12)*2, 31, 800/12, 387},
	{(800/12)*3, 31, 800/12, 387},
	{(800/12)*4, 31, 800/12, 387},
	{(800/12)*5, 31, 800/12, 387},
	{(800/12)*6, 31, 800/12, 387},
	{(800/12)*7, 31, 800/12, 387},
	{(800/12)*8, 31, 800/12, 387},
	{(800/12)*9, 31, 800/12, 387},
	{(800/12)*10, 31, 800/12, 387},
	{(800/12)*11, 31, 800/12, 387},
};

static uint32_t textLabelsColors[] =
{
	0xFFFFFF, // tekst
	0x111111, // tło
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


	strControlProperties prop;
	// ramka
	//strControlProperties prop;
	frameData.placesCount = 12;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];
	frameData.places[8] = &framesPlaces[8][0];
	frameData.places[9] = &framesPlaces[9][0];
	frameData.places[10] = &framesPlaces[10][0];
	frameData.places[11] = &framesPlaces[11][0];
	prop.style = 0;
	prop.value = 0;
	prop.colors = nullptr;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	//strControlProperties prop;

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<8; i++)
	{
		prop.text = (char*)"";
		prop.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop.x = (800/8)*i+(800/16);
		prop.y = 465;
		prop.w = 800/8-6;
		prop.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop);

		prop.y = 437;
		prop.h = 28;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop);
	}


	for(uint8_t i = 0; i<12; i++)
	{
		prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleVerticalText);
		prop.x = (800/12)*i+(800/24);
		prop.y = 220;
		prop.colors = textLabelsColors;

		if(textLabel[i] == nullptr) textLabel[i] = display.createControl<cLabel>(&prop);

		prop.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleFont2);
		if(i%2 == 0) prop.style |= controlStyleBackground;
		prop.x = (800/12)*i+(800/24);
		prop.y = 280;
		prop.colors = textLabelsColors;
		prop.w = (800/12);
		prop.h = 510;
		if(value1Label[i] == nullptr) value1Label[i] = display.createControl<cLabel>(&prop);
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

		display.destroyControl(value1Label[i]);
		value1Label[i] = nullptr;
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

	display.setControlText(bottomLabel[0], "Track 1");
	display.setControlText(bottomLabel[1], "Track 2");
	display.setControlText(bottomLabel[2], "Track 3");
	display.setControlText(bottomLabel[3], "Track 4");
	display.setControlText(bottomLabel[4], "Track 5");
	display.setControlText(bottomLabel[5], "Track 6");
	display.setControlText(bottomLabel[6], "Track 7");
	display.setControlText(bottomLabel[7], "Track 8");


	//-------------------------------------

	for(uint8_t i = 0; i<performanceFxesCount; i++)
	{
		display.setControlText(textLabel[i], &performanceFxesLabels[fxPlaces[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);

		showPerformaceValue(i);
	}


	for(uint8_t i = 0; i<8; i++)
	{
		refreshTracksState();

		display.setControlPosition(bottomLabel[i], -1, 465);
		display.setControlSize(bottomLabel[i], -1, 30);

		//display.setControlShow(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
		//	display.refreshControl(topLabel[i]);
	}

	display.synchronizeRefresh();
}


//====================================================================================================

void cPerformanceMode::showEditFrame(uint8_t place)
{
	display.setControlValue(frameControl, place);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cPerformanceMode::hideEditFrame()
{
	display.setControlHide(frameControl);
	display.refreshControl(frameControl);
}


void cPerformanceMode::refreshFxNames()
{
	for(uint8_t i = 0; i<performanceFxesCount; i++)
	{
		display.setControlText(textLabel[i], &performanceFxesLabels[fxPlaces[i]][0]);
		display.setControlShow(textLabel[i]);
		display.refreshControl(textLabel[i]);
	}
}


void cPerformanceMode::refreshTracksState()
{
	for(uint8_t i = 0; i<8; i++)
	{
		if(tracksPerformanceState[i] == 1)
		{
			display.setControlText(topLabel[i], "[x]");
		}
		else
		{
			display.setControlText(topLabel[i], "[ ]");
		}

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

}


void cPerformanceMode::showPerformaceValue(uint8_t fx)
{
	if(fx >= performanceFxesCount) return;

	switch(fx)
	{
	case mtPerfSamplePlayback:
	{
		if(fxValues[fx] == 1) display.setControlText(value1Label[fx], "<<<");
		else 					 display.setControlText(value1Label[fx], ">>>");
		display.setControlShow(value1Label[fx]);
		display.refreshControl(value1Label[fx]);
		return;
	}
	case mtPerfStepStutter:
	{
		display.setControlText(value1Label[fx], &performanceStutterLabels[fxValues[fx]][0]);
		display.setControlShow(value1Label[fx]);
		display.refreshControl(value1Label[fx]);
		return;
	}
	case mtPerfPatternPlayMode:
	{
		if(fxValues[fx] == 1) 		display.setControlText(value1Label[fx], "Back");
		else if(fxValues[fx] == 2) 	display.setControlText(value1Label[fx], "Rnd");
		else 							display.setControlText(value1Label[fx], "Fwd");
		display.setControlShow(value1Label[fx]);
		display.refreshControl(value1Label[fx]);
		return;
	}

	}


	sprintf(&fxValuesText[fx][0],"%d", fxValues[fx]);

	display.setControlText(value1Label[fx], &fxValuesText[fx][0]);
	display.setControlShow(value1Label[fx]);
	display.refreshControl(value1Label[fx]);
}
