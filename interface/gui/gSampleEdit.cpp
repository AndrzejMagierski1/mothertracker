

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

	strControlProperties prop6;

	prop6.x = 190;
	prop6.y = 170;
	prop6.style = controlStyleValue_0_100;
	prop6.h = 100;
	prop6.w = 420;

	if(processHorizontalBarControl == nullptr)  processHorizontalBarControl = display.createControl<cHorizontalBar>(&prop6);

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

	for(uint8_t i = 0; i<6; i++)
	{
		prop2.text = (char*)"";
		//prop2.data =  &bottomValuesConfig;
		prop2.colors = interfaceGlobals.activeLabelsColors;

		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 452;
		prop2.h =  59;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.x = (800/4)*3+(800/8);
	prop2.w = 800/4-6;

	prop2.y = 452;
	prop2.h = 59;
	if(label[6] == nullptr) label[6] = display.createControl<cLabel>(&prop2);


	effectList.linesCount = 14;
	effectList.start = 0;
	effectList.length = effectsCount;
	effectList.data = effectNames;

	prop.x = (800/8)*6+8;
	prop.y = 37;
	prop.w = 800/4-16;
	prop.h = 25;
	prop.data = &effectList;
	if(effectListControl == nullptr)  effectListControl = display.createControl<cList>(&prop);

	// spectrum + points
	prop.x = 0;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	if(progressCursor == nullptr) progressCursor = display.createControl<cProgressCursor>(&prop);
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);
	prop.data = &effectSpectrum;
	if(effectSpectrumControl == nullptr)  effectSpectrumControl = display.createControl<cSpectrum>(&prop);
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

	display.destroyControl(effectSpectrumControl);
	effectSpectrumControl = nullptr;

	display.destroyControl(pointsControl);
	pointsControl = nullptr;

	display.destroyControl(effectListControl);
	effectListControl = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
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

	display.destroyControl(progressCursor);
	progressCursor  = nullptr;
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

void cSampleEditor::showTitleBar()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Sample Editor");
	display.refreshControl(titleLabel);

	showActualInstrument();
}


void cSampleEditor::showDefaultScreen()
{
	showTitleBar();

	display.setControlValue(label[0], 0);
	display.setControlValue(label[1], 0);
	display.setControlValue(label[2], 0);
	display.setControlValue(label[3], 1);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 0);

	//display.setControlText(label[0], "Process");
	display.setControlText(label[1], "Apply");
	display.setControlText(label[6], "Effect");
/*	display.setControlText(label[3], "Start");
	display.setControlText(label[4], "End");
	display.setControlText(label[5], "Zoom");*/

	//display.setControlText(bottomLabel[7], "");




	showEffectScreen(&effectControl[currSelEffect]);


	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);

		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	showEffectsList();

	hideHorizontalBar();
	activateLabelsBorder();


	display.synchronizeRefresh();

}

void cSampleEditor::showEffectScreen(effect_handle_t *screenCfg)
{
	if(screenCfg->screen == fullSpectrum)
	{
		frameChange(0);
		spectrum.width = 600;

		display.setControlData(spectrumControl, &spectrum);
		display.setControlShow(spectrumControl);
		display.setControlShow(pointsControl);
		display.setControlShow(progressCursor);
	}
	else
	{
		frameChange(1);
		display.setControlHide(spectrumControl);
		display.setControlHide(pointsControl);
		display.setControlHide(progressCursor);
	}

	display.refreshControl(pointsControl);
	display.refreshControl(spectrumControl);

	for(int i = 0; i < MAX_DATA_BARS; i++)
	{
		display.setControlHide(barControl[i]);
		display.setControlText(label[BAR_MIN_POS + i], "");
		display.setControlText2(label[BAR_MIN_POS + i], "");
	}

	undoDisplayControl(effectControl[currSelEffect].undoActive);

	for(int i = (MAX_DATA_BARS - screenCfg->barNum); i < MAX_DATA_BARS; i++)
	{
		display.setControlShow(barControl[i]);
	}

	for(int i = (MAX_DATA_BARS - screenCfg->paramNum); i < MAX_DATA_BARS; i++)
	{
		display.setControlText2(label[BAR_MIN_POS + i], screenCfg->bar[i].name);
	}

	for(int i = (MAX_DATA_BARS - screenCfg->paramNum); i < MAX_DATA_BARS; i++)
	{
		updateEffectValues(&effectControl[currSelEffect], i);
	}

	processOrPreview(effectControl[currSelEffect].effectStage);

	for(int i=0;i<4;i++)
	{
		display.refreshControl(barControl[i]);
		display.refreshControl(label[BAR_MIN_POS + i]);
	}

	display.synchronizeRefresh();
}

void cSampleEditor::showPreviewSpectrum()
{
	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	if(effectControl[currSelEffect].screen == fullSpectrum)
	{
		display.setControlShow(pointsControl);
	}
	else
	{
		display.setControlHide(pointsControl);
	}

	display.refreshControl(pointsControl);

	display.setControlShow(progressCursor);
	display.refreshControl(progressCursor);

	for(int i = 0; i < MAX_DATA_BARS; i++)
	{
		display.setControlHide(barControl[i]);
		display.refreshControl(barControl[i]);
	}

	display.setControlShow(effectSpectrumControl);
	display.refreshControl(effectSpectrumControl);

	display.synchronizeRefresh();
}

void cSampleEditor::hidePreviewSpectrum()
{
	showEffectScreen(&effectControl[currSelEffect]);

	display.setControlShow(frameControl);
	display.refreshControl(frameControl);

	display.setControlHide(effectSpectrumControl);
	display.refreshControl(effectSpectrumControl);

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
	effectList.linesCount = 14;
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

	if(i < INSTRUMENTS_COUNT)
	{
		sprintf(actualInstrName, "%d. ", i+1);
		strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);
	}
	else
	{
		//i = i-(INSTRUMENTS_COUNT-1);
		sprintf(actualInstrName, "%d. MIDI Channel %d", i+3, i-(INSTRUMENTS_COUNT-1));
	}

	display.setControlShow(instrumentLabel);
	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}

//==============================================================================================================

void cSampleEditor::showValueLabels(uint8_t whichBar)
{
	display.setControlText(label[BAR_MIN_POS + whichBar], &dataBarText[whichBar][0]);
	display.setControlShow(label[BAR_MIN_POS + whichBar]);
	display.refreshControl(label[BAR_MIN_POS + whichBar]);
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

void cSampleEditor::showQueueHorizontalBar(uint8_t progress, const char* text)
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

void cSampleEditor::processOrPreview(uint8_t x)
{
	if(x)
	{
		display.setControlText(label[0], "Preview");
	}
	else
	{
		display.setControlText(label[0], "Process");
	}

	display.refreshControl(label[0]);
}

void cSampleEditor::undoDisplayControl(uint8_t onOff)
{
	if(onOff)
	{
		display.setControlValue(label[2], 0);
		display.setControlText(label[2], "Undo");
	}
	else
	{
		display.setControlValue(label[2], 1);
		display.setControlText(label[2], "");
	}

	display.refreshControl(label[2]);
}


