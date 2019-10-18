

#include "sampleEditor.h"


static uint16_t framesPlacesNoBars[7][4] =
{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{(800/8)*3+1, 421, 800/8-1, 65},
		{(800/8)*4+1, 421, 800/8-1, 65},
		{(800/8)*5+1, 421, 800/8-1, 65},
		{(800/8)*6+2, 31, 800/4-5, 387},
};

static uint16_t framesPlacesWithBars[7][4] =
{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{(800/8)*2+2, 31, 800/8-5, 387},
		{(800/8)*3+2, 31, 800/8-5, 387},
		{(800/8)*4+2, 31, 800/8-5, 387},
		{(800/8)*5+2, 31, 800/8-5, 387},
		{(800/8)*6+2, 31, 800/4-5, 387},
};




void cSampleEditor::initDisplayControls()
{
	// inicjalizacja kontrolek
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
	prop.style = 0;
	prop.value = 0;
	//prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

	strControlProperties prop6;

	prop6.x = 190;
	prop6.y = 170;
	prop6.style = controlStyleValue_0_100;
	prop6.h = 100;
	prop6.w = 420;

	if(processHorizontalBarControl == nullptr)  processHorizontalBarControl = display.createControl<cHorizontalBar>(&prop6);

	for(uint8_t i = 0; i<6; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.w = 800/8-6;
		prop2.h = 30;

		prop2.x = (800/8)*i+(800/16);
		if(i < 2)
		{
			prop2.y = 452;
			prop2.h = 58;
		}
		else
		{
			prop2.y = 437;
			prop2.h = 28;
		}

		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.x = (800/4)*3+(800/8);
	prop2.w = 800/4-6;

	prop2.y = 452;
	prop2.h = 58;
	if(topLabel[6] == nullptr) topLabel[6] = display.createControl<cLabel>(&prop2);

	prop2.y = 465;
	prop2.h = 30;
	if(bottomLabel[6] == nullptr) bottomLabel[6] = display.createControl<cLabel>(&prop2);


	effectList.linesCount = 8;
	effectList.start = 0;
	effectList.length = effectsCount;
	effectList.data = effectNames;

	prop2.style = 	( controlStyleShow );
	prop.x = (800/8)*6+8;
	prop.y = 140;
	prop.w = 800/4-16;
	prop.h = 25;
	prop.data = &effectList;
	if(effectListControl == nullptr)  effectListControl = display.createControl<cList>(&prop);

	// spectrum + points
	prop.x = 0;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);

	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);


	for(int i = 2; i < 6; i++)
	{
		prop2.x = (800/8)*i+5;
		prop2.y = 30;
		prop2.w = 800/8-10;
		prop2.style =  controlStyleValue_0_100;
		prop2.h = 389;
		if(barControl[i-2] == nullptr)  barControl[i-2] = display.createControl<cBar>(&prop2);
	}



}


void cSampleEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;

	display.destroyControl(spectrumControl);
	spectrumControl = nullptr;

	display.destroyControl(pointsControl);
	pointsControl = nullptr;

	display.destroyControl(effectListControl);
	effectListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;

	for(uint8_t i = 0; i < 4 ; i++)
	{
		display.destroyControl(barControl[i]);
		barControl[i]=nullptr;
	}

	display.destroyControl(processHorizontalBarControl);
	processHorizontalBarControl = nullptr;
}

void cSampleEditor::frameChange(uint8_t control)
{
	frameData.placesCount = 7;

	if(control)// bars
	{
		for(int i = 0; i< frameData.placesCount ; i++)
		{
			frameData.places[i] = &framesPlacesWithBars[i][0];
		}
	}
	else// no bars
	{
		for(int i = 0; i< frameData.placesCount ; i++)
		{
			frameData.places[i] = &framesPlacesNoBars[i][0];
		}
	}

	display.setControlData(frameControl, &frameData);
}

void cSampleEditor::showDefaultScreen()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Sample Editor");
	display.refreshControl(titleLabel);

	showActualInstrument();

	// bottom labels

	display.setControlText(topLabel[0], "Preview");
	display.setControlText(topLabel[1], "Apply");
	display.setControlText(bottomLabel[3], "Start");
	display.setControlText(bottomLabel[4], "End");
	display.setControlText(bottomLabel[5], "Zoom");

	//display.setControlText(bottomLabel[7], "");


	display.setControlText(topLabel[6], "Effect");

	showEffectScreen(&effectScreen[currSelEffect]);


	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	showEffectsList();

	hideHorizontalBar();

	display.synchronizeRefresh();

}

void cSampleEditor::resizeUndo(uint8_t control)//1 - center 0 - top
{
	if(control)
	{
		display.setControlPosition(topLabel[2], (800/8)*2+(800/16), 452);
		display.setControlSize(topLabel[2], 800/8-6, 58);
	}
	else
	{
		display.setControlPosition(topLabel[2], (800/8)*2+(800/16), 437);
		display.setControlSize(topLabel[2], 800/8-6, 28);
	}
}

void cSampleEditor::showEffectScreen(effect_screen_t *screenCfg)
{
	if(screenCfg->screen == fullSpectrum)
	{
		frameChange(0);
		spectrum.width = 600;

		display.setControlData(spectrumControl, &spectrum);
		display.setControlShow(spectrumControl);
		display.setControlShow(pointsControl);
	}
	else
	{
		frameChange(1);
		display.setControlHide(spectrumControl);
		display.setControlHide(pointsControl);
	}

	display.refreshControl(pointsControl);
	display.refreshControl(spectrumControl);

	for(int i = 0; i < MAX_DATA_BARS; i++)
	{
		display.setControlHide(barControl[i]);
		display.setControlText(topLabel[BAR_MIN_POS + i], "");
		display.setControlText(bottomLabel[BAR_MIN_POS + i], "");
	}

	undoDisplayControl(effectScreen[currSelEffect].undoActive);

	for(int i = (MAX_DATA_BARS - screenCfg->barNum); i < MAX_DATA_BARS; i++)
	{
		display.setControlShow(barControl[i]);
	}

	for(int i = (MAX_DATA_BARS - screenCfg->paramNum); i < MAX_DATA_BARS; i++)
	{
		display.setControlText(bottomLabel[BAR_MIN_POS + i], screenCfg->bar[i].name);
	}

	for(int i = (MAX_DATA_BARS - screenCfg->paramNum); i < MAX_DATA_BARS; i++)
	{
		updateEffectValues(&effectScreen[currSelEffect], i);
	}

	for(int i=0;i<4;i++)
	{
		display.refreshControl(barControl[i]);
		display.refreshControl(bottomLabel[BAR_MIN_POS + i]);
		display.refreshControl(topLabel[BAR_MIN_POS + i]);
	}

	display.synchronizeRefresh();
}



//==============================================================================================================
void cSampleEditor::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================

void cSampleEditor::showEffectsList()
{
	effectList.start = currSelEffect;
	effectList.length = effectsCount;
	effectList.linesCount = 6;
	effectList.data = effectNames;

	display.setControlData(effectListControl,  &effectList);
	display.setControlShow(effectListControl);
	display.refreshControl(effectListControl);
}

//==============================================================================================================
void cSampleEditor::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	sprintf(actualInstrName, "%d. ", i+1);

	strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);

	display.setControlShow(instrumentLabel);
	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}

//==============================================================================================================

void cSampleEditor::showValueLabels(uint8_t whichBar)
{
	display.setControlText(topLabel[BAR_MIN_POS + whichBar], &dataBarText[whichBar][0]);
	display.setControlShow(topLabel[BAR_MIN_POS + whichBar]);
	display.refreshControl(topLabel[BAR_MIN_POS + whichBar]);
}

void cSampleEditor::refreshBarsValue(uint8_t whichBar, uint8_t newValue)
{
	display.setControlValue(barControl[whichBar], newValue);
	display.refreshControl(barControl[whichBar]);
}

void cSampleEditor::showHorizontalBar(uint8_t progress , const char* text)
{
	display.setControlValue(processHorizontalBarControl, progress);
	display.setControlText(processHorizontalBarControl, text);
	display.setControlShow(processHorizontalBarControl);
	display.refreshControl(processHorizontalBarControl);
}

void cSampleEditor::hideHorizontalBar()
{
	display.setControlValue(processHorizontalBarControl, 0);
	display.setControlHide(processHorizontalBarControl);
	display.refreshControl(processHorizontalBarControl);
}

void cSampleEditor::undoDisplayControl(uint8_t onOff)
{
	resizeUndo(onOff);

	if(onOff)
	{
		display.setControlText(topLabel[2], "Undo");
	}
	else
	{
		display.setControlText(topLabel[2], "");
	}

	display.refreshControl(topLabel[2]);
}


