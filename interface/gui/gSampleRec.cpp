

#include "sampleRecorder.h"
#include "mtRecorder.h"

static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};

static  uint16_t framesPlacesS1[8][4] =
{
	{0, 		0, 800/8, 480},
	{(800/8)*1, 0, 800/8, 480},
	{(800/8)*2, 412, 800/8, 68},
	{(800/8)*3, 412, 800/8, 68},
	{(800/8)*4, 0, 800/8, 480},
	{(800/8)*5, 0, 800/8, 480},
	{(800/8)*6, 0, 800/8, 480},
	{(800/8)*7, 412, 800/8, 68}
};

static  uint16_t framesPlacesS2[8][4] =
{
	{0, 		412, 800/8, 68},
	{(800/8)*1, 412, 800/8, 68},
	{(800/8)*2, 412, 800/8, 68},
	{(800/8)*3, 412, 800/8, 68},
	{(800/8)*4, 412, 800/8, 68},
	{(800/8)*5, 412, 800/8, 68},
	{(800/8)*6, 412, 800/8, 68},
	{(800/8)*7, 412, 800/8, 68},
};

uint32_t colorRed[3] = { 0xFF0000, 0xFFFFFF,0xFF0000 };
uint32_t colorGreen[3] = { 0x00FF00, 0xFFFFFF,0x00FF00 };

void cSampleRecorder::initDisplayControls()
{
	// inicjalizacja kontrolek

	strControlProperties prop;

	prop.x = 100;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);

	points.pointsType = 0;
	points.endPoint = MAX_16BIT;
	points.startPoint = 0;
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);

	strControlProperties prop2;


	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder | controlStyleFont1);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 415;
		prop2.h = 26;
		prop2.style |= 	controlStyleManualText;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
	}

	strControlProperties prop3;
	// ramka
	frameData.placesCount = 8;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlacesS1[0][0];
	frameData.places[1] = &framesPlacesS1[1][0];
	frameData.places[2] = &framesPlacesS1[2][0];
	frameData.places[3] = &framesPlacesS1[3][0];
	frameData.places[4] = &framesPlacesS1[4][0];
	frameData.places[5] = &framesPlacesS1[5][0];
	frameData.places[6] = &framesPlacesS1[6][0];
	frameData.places[7] = &framesPlacesS1[7][0];
	prop3.style = 0;
	prop3.value = 0;
	prop3.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop3);

	strControlProperties prop4;
	sourceList.linesCount = 3;
	sourceList.start = recorderConfig.source;
	sourceList.length = 3;
	sourceList.data = sourceNames;
	prop4.x = (800/8)*(0)+5;
	prop4.y = 152;
	prop4.w = 800/8-10;
	prop4.h = 25;
	prop4.data = &sourceList;
	if(sourceListControl == nullptr)  sourceListControl = display.createControl<cList>(&prop4);

	monitorList.linesCount = 2;
	monitorList.start = recorderConfig.monitor;
	monitorList.length = 2;
	monitorList.data = monitorNames;
	prop4.x = (800/8)*(6)+5;
	prop4.y = 165;
	prop4.w = 800/8-10;
	prop4.h = 25;
	prop4.data = &monitorList;
	if(monitorListControl == nullptr)  monitorListControl = display.createControl<cList>(&prop4);


	strControlProperties prop5;
	prop5.x = (800/8)*(4)+5;
	prop5.y = 30;
	prop5.w = 800/8-10;
	prop5.style =  controlStyleShow | controlStyleValue_0_100;
	prop5.h = 380;
	if(levelBarControl == nullptr)  levelBarControl = display.createControl<cBar>(&prop5);

	prop5.x = (800/8)*(5)+5;
	prop5.y = 30;
	prop5.w = 800/8-10;
	prop5.style =  controlStyleShow | controlStyleValue_0_100;
	prop5.h = 380;
	if(gainBarControl == nullptr)  gainBarControl = display.createControl<cBar>(&prop5);

	prop5.x = (800/8)*(1)+5;
	prop5.y = 30;
	prop5.w = 800/8-10;
	prop5.style =  controlStyleShow | controlStyleValue_0_100;
	prop5.h = 380;
	if(radioFreqBarControl == nullptr)  radioFreqBarControl = display.createControl<cBar>(&prop5);

	strControlProperties prop6;
	prop6.x = 80;
	prop6.y = 170;
	prop6.w = 650;
	prop6.h = 210;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop6);

	strControlProperties prop7;
	prop7.text = (char*)"";
	prop7.style = 	(controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop7.x = 398;
	prop7.y = 120;
	prop7.w = 635;
	prop7.h = 30;
	if(editName == nullptr)  editName = display.createControl<cEdit>(&prop7);

	strControlProperties prop8;
	prop8.x = 190;
//	prop.colors = &color[0];
	prop8.y = 170;
	//prop.w = 800/4-10;
	prop8.style = controlStyleValue_0_100;
	prop8.h = 100;
	prop8.w = 420;
//	prop.value = 70;
//	prop.text = "loading...";
	if(saveHorizontalBarControl == nullptr)  saveHorizontalBarControl = display.createControl<cHorizontalBar>(&prop8);

	strControlProperties prop9;

	prop9.x = 400;
	prop9.colors = popUpLabelColors;
//	prop.colors = &color[0];
	prop9.y = 350;
	//prop.w = 800/4-10;
//	prop9.style = controlStyleValue_0_100;
	prop9.h = 100;
	prop9.w = 800-(10);
	prop9.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 | controlStyleRoundedBorder);
	prop9.text = "Changes will be lost. Do you want to continue?";
	if(selectWindowLabel == nullptr)  selectWindowLabel = display.createControl<cLabel>(&prop9);

}


void cSampleRecorder::destroyDisplayControls()
{
	display.destroyControl(spectrumControl);
	spectrumControl = nullptr;
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}

	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(sourceListControl);
	sourceListControl = nullptr;

	display.destroyControl(monitorListControl);
	monitorListControl = nullptr;

	display.destroyControl(levelBarControl);
	levelBarControl = nullptr;

	display.destroyControl(gainBarControl);
	gainBarControl = nullptr;

	display.destroyControl(radioFreqBarControl);
	radioFreqBarControl = nullptr;

	display.destroyControl(pointsControl);
	pointsControl = nullptr;

	display.destroyControl(keyboardControl);
	keyboardControl = nullptr;

	display.destroyControl(editName);
	editName = nullptr;

	display.destroyControl(saveHorizontalBarControl);
	saveHorizontalBarControl = nullptr;

	display.destroyControl(selectWindowLabel);
	selectWindowLabel = nullptr;
}

void cSampleRecorder::showDefaultScreen()
{

	if (currentScreen == screenTypeConfig)
	{
		//spectrum
		display.setControlHide(spectrumControl);
		display.refreshControl(spectrumControl);

		//points

		display.setControlHide(pointsControl);
		display.refreshControl(pointsControl);

		//	listy
		display.setControlShow(sourceListControl);
		display.setControlValue(sourceListControl, recorderConfig.source);
		display.refreshControl(sourceListControl);

		display.setControlShow(monitorListControl);
		display.setControlValue(monitorListControl, recorderConfig.monitor);
		display.refreshControl(monitorListControl);

		//bar
		display.setControlValue(levelBarControl, levelBarVal);
		display.setControlShow(levelBarControl);
		display.refreshControl(levelBarControl);

		display.setControlValue(gainBarControl, gainBarVal);
		display.setControlShow(gainBarControl);
		display.refreshControl(gainBarControl);

		if (recorderConfig.source == sourceTypeRadio)
		{
			display.setControlValue(radioFreqBarControl, radioFreqBarVal);
			display.setControlShow(radioFreqBarControl);
			display.refreshControl(radioFreqBarControl);
			display.setControlText(bottomLabel[1], "Radio Freq");
			display.setControlText(bottomLabel[2], "<<");
			display.setControlText(bottomLabel[3], ">>");

			calcRadioFreqBarVal();
			drawRadioFreqBar();
		}
		else
		{
			display.setControlValue(radioFreqBarControl, radioFreqBarVal);
			display.setControlHide(radioFreqBarControl);
			display.refreshControl(radioFreqBarControl);
			display.setControlText(bottomLabel[1], "");
			display.setControlText(bottomLabel[2], "");
			display.setControlText(bottomLabel[3], "");
		}

		// bottom labels
		display.setControlText(bottomLabel[0], "Source");

		display.setControlText(bottomLabel[4], "Level");
		display.setControlText(bottomLabel[5], "Gain");
		display.setControlText(bottomLabel[6], "Monitor");
		display.setControlText(bottomLabel[7], "Record");

		frameData.placesCount = 8;
		frameData.startPlace = 0;
		frameData.places[0] = &framesPlacesS1[0][0];
		frameData.places[1] = &framesPlacesS1[1][0];
		frameData.places[2] = &framesPlacesS1[2][0];
		frameData.places[3] = &framesPlacesS1[3][0];
		frameData.places[4] = &framesPlacesS1[4][0];
		frameData.places[5] = &framesPlacesS1[5][0];
		frameData.places[6] = &framesPlacesS1[6][0];
		frameData.places[7] = &framesPlacesS1[7][0];

		display.setControlData(frameControl, &frameData);

		calcLevelBarVal();
		drawLevelBar();

		calcGainBarVal();
		drawGainBar();

		display.setControlHide(keyboardControl);
		display.refreshControl(keyboardControl);

		display.setControlHide(editName);
		display.refreshControl(editName);

	}
	else if (currentScreen == screenTypeRecord)
	{
		display.setControlShow(spectrumControl);
		display.refreshControl(spectrumControl);

		if (recordInProgressFlag == 1)
		{
			display.setControlHide(pointsControl);
			display.refreshControl(pointsControl);
		}
		else
		{
			display.setControlShow(pointsControl);
			display.refreshControl(pointsControl);
		}

		//	listy
		display.setControlHide(sourceListControl);
		display.refreshControl(sourceListControl);

		display.setControlHide(monitorListControl);
		display.refreshControl(monitorListControl);

		//bar
		display.setControlHide(levelBarControl);
		display.refreshControl(levelBarControl);

		display.setControlHide(gainBarControl);
		display.refreshControl(gainBarControl);

		display.setControlHide(radioFreqBarControl);
		display.refreshControl(radioFreqBarControl);

		frameData.placesCount = 8;
		frameData.startPlace = 0;
		frameData.places[0] = &framesPlacesS2[0][0];
		frameData.places[1] = &framesPlacesS2[1][0];
		frameData.places[2] = &framesPlacesS2[2][0];
		frameData.places[3] = &framesPlacesS2[3][0];
		frameData.places[4] = &framesPlacesS2[4][0];
		frameData.places[5] = &framesPlacesS2[5][0];
		frameData.places[6] = &framesPlacesS2[6][0];
		frameData.places[7] = &framesPlacesS2[7][0];

		display.setControlData(frameControl, &frameData);

		if (recordInProgressFlag == 1)
		{
			display.setControlText(bottomLabel[0], "");
			display.setControlText(bottomLabel[1], "");
			display.setControlText(bottomLabel[2], "");
			display.setControlText(bottomLabel[3], "");
			display.setControlText(bottomLabel[4], "");
			display.setControlText(bottomLabel[5], "");
			display.setControlText(bottomLabel[6], "");
			display.setControlText(bottomLabel[7], "Stop");
		}
		else
		{
			display.setControlText(bottomLabel[0], "Preview");
			display.setControlText(bottomLabel[1], "Start Point");
			display.setControlText(bottomLabel[2], "End Point");
			display.setControlText(bottomLabel[3], "Zoom");
			display.setControlText(bottomLabel[4], "Crop");
			display.setControlText(bottomLabel[5], "Undo");
			display.setControlText(bottomLabel[6], "Go Back");
			display.setControlText(bottomLabel[7], "Save");
		}

		display.setControlHide(keyboardControl);
		display.refreshControl(keyboardControl);

		display.setControlHide(editName);
		display.refreshControl(editName);
	}
	else if(currentScreen == screenTypeKeyboard)
	{
		//spectrum
		display.setControlHide(spectrumControl);
		display.refreshControl(spectrumControl);

		//points

		display.setControlHide(pointsControl);
		display.refreshControl(pointsControl);

		display.setControlShow(keyboardControl);
		display.refreshControl(keyboardControl);

		showKeyboardEditName();

		display.setControlText(bottomLabel[0], "");
		display.setControlText(bottomLabel[1], "");
		display.setControlText(bottomLabel[2], "");
		display.setControlText(bottomLabel[3], "");
		display.setControlText(bottomLabel[4], "");
		display.setControlText(bottomLabel[5], "");
		display.setControlText(bottomLabel[6], "Go Back");
		display.setControlText(bottomLabel[7], "Save");
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlText(topLabel[i], "");
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	if ((currentScreen == screenTypeRecord) && !recordInProgressFlag)
	{
		showZoomValue();
		showEndPointValue();
		showStartPointValue();
	}

	display.setControlHide(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.synchronizeRefresh();

}

void cSampleRecorder::showRadio()
{
	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlShow(radioFreqBarControl);
	display.setControlText(bottomLabel[1], "Radio Freq");
	display.setControlText(bottomLabel[2], "<<");
	display.setControlText(bottomLabel[3], ">>");

	display.refreshControl(bottomLabel[1]);
	display.refreshControl(bottomLabel[2]);
	display.refreshControl(bottomLabel[3]);
	display.refreshControl(radioFreqBarControl);
	showFreqValue();
}
void cSampleRecorder::hideRadio()
{
	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlHide(radioFreqBarControl);
	display.setControlText(bottomLabel[1], "");
	display.setControlText(bottomLabel[2], "");
	display.setControlText(bottomLabel[3], "");
	display.setControlText(topLabel[1], "");

	display.refreshControl(bottomLabel[1]);
	display.refreshControl(bottomLabel[2]);
	display.refreshControl(bottomLabel[3]);
	display.refreshControl(topLabel[1]);
	display.refreshControl(radioFreqBarControl);
}

void cSampleRecorder::showKeyboard()
{

	if(keyboardShiftFlag) display.setControlValue(keyboardControl, keyboardPosition + 42);
	else display.setControlValue(keyboardControl, keyboardPosition);

	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);
}

void cSampleRecorder::hideKeyboard()
{
	display.setControlHide(keyboardControl);
	display.refreshControl(keyboardControl);
}

void cSampleRecorder::showKeyboardEditName()
{


	display.setControlValue(editName, editPosition);

	display.setControlText(editName, name);
	display.setControlShow(editName);
	display.refreshControl(editName);
}

void cSampleRecorder::hideKeyboardEditName()
{
	display.setControlHide(editName);
	display.refreshControl(editName);
}

//==============================================================================================================
void cSampleRecorder::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================
void cSampleRecorder::showZoomValue()
{
	if(zoomValue >= 10.0)
	{
		zoomTextValue[0] = (uint8_t)zoomValue /10 + 48;
		zoomTextValue[1] = (uint8_t)zoomValue %10 + 48;
		zoomTextValue[2] = '.';
		zoomTextValue[3] = (uint8_t)((zoomValue-(uint16_t)zoomValue)*100) /10 + 48;
		zoomTextValue[4] = (uint8_t)((zoomValue-(uint16_t)zoomValue)*100) %10 + 48;
		zoomTextValue[5] = 0;
	}
	else
	{
		zoomTextValue[0] = (uint8_t)zoomValue + 48;
		zoomTextValue[1] = '.';
		zoomTextValue[2] = (uint8_t)((zoomValue-(uint16_t)zoomValue)*100) /10 + 48;
		zoomTextValue[3] = (uint8_t)((zoomValue-(uint16_t)zoomValue)*100) %10 + 48;
		zoomTextValue[4] = 0;
	}

	display.setControlText(topLabel[3], zoomTextValue);
	display.setControlShow(topLabel[3]);
	display.refreshControl(topLabel[3]);
}

void cSampleRecorder::showRecTimeValue()
{

	recTimeValue = recorder.getLength()/44100.0;

	if(recTimeValue > 100)
	{
		recTimeValueText[0] = (uint8_t)recTimeValue /100 + 48;
		recTimeValueText[1] = ((uint8_t)recTimeValue /10)%10 + 48;
		recTimeValueText[2] = (uint8_t)recTimeValue %10 + 48;
		recTimeValueText[3] = '.';
		recTimeValueText[4] = (uint8_t)(((recTimeValue-(uint16_t)recTimeValue)*1000) /100) + 48;
		recTimeValueText[5] = ((uint8_t)((recTimeValue-(uint16_t)recTimeValue)*1000) /10)%10 + 48;
		recTimeValueText[6] = (uint8_t)((recTimeValue-(uint16_t)recTimeValue)*1000) %10 + 48;
		recTimeValueText[7] = 's';
		recTimeValueText[8] = 0;
	}
	else if(recTimeValue > 10 && recTimeValue < 100)
	{
		recTimeValueText[0] = (uint8_t)recTimeValue /10 + 48;
		recTimeValueText[1] = (uint8_t)recTimeValue %10 + 48;
		recTimeValueText[2] = '.';
		recTimeValueText[3] = (uint8_t)(((recTimeValue-(uint16_t)recTimeValue)*1000) /100) + 48;
		recTimeValueText[4] = ((uint8_t)((recTimeValue-(uint16_t)recTimeValue)*1000) /10)%10 + 48;
		recTimeValueText[5] = (uint8_t)((recTimeValue-(uint16_t)recTimeValue)*1000) %10 + 48;
		recTimeValueText[6] = 's';
		recTimeValueText[7] = 0;
	}
	else if(recTimeValue < 10)
	{
		recTimeValueText[0] = (uint8_t)recTimeValue %10 + 48;
		recTimeValueText[1] = '.';
		recTimeValueText[2] = (uint8_t)(((recTimeValue-(uint16_t)recTimeValue)*1000) /100) + 48;
		recTimeValueText[3] = ((uint8_t)((recTimeValue-(uint16_t)recTimeValue)*1000) /10)%10 + 48;
		recTimeValueText[4] = (uint8_t)((recTimeValue-(uint16_t)recTimeValue)*1000) %10 + 48;
		recTimeValueText[5] = 's';
		recTimeValueText[6] = 0;
	}


	display.setControlText(topLabel[2], recTimeValueText);
	display.setControlShow(topLabel[2]);
	display.refreshControl(topLabel[2]);
}

void cSampleRecorder::showStartPointValue()
{
	recTimeValue = recorder.getLength()/44100.0;
	float localStartPoint = (recTimeValue * startPoint) / MAX_16BIT;

	if(localStartPoint > 100)
	{
		startPointValueText[0] = (uint8_t)localStartPoint /100 + 48;
		startPointValueText[1] = ((uint8_t)localStartPoint /10)%10 + 48;
		startPointValueText[2] = (uint8_t)localStartPoint %10 + 48;
		startPointValueText[3] = '.';
		startPointValueText[4] = (uint8_t)(((localStartPoint-(uint16_t)localStartPoint)*1000) /100) + 48;
		startPointValueText[5] = ((uint8_t)((localStartPoint-(uint16_t)localStartPoint)*1000) /10)%10 + 48;
		startPointValueText[6] = (uint8_t)((localStartPoint-(uint16_t)localStartPoint)*1000) %10 + 48;
		startPointValueText[7] = 's';
		startPointValueText[8] = 0;
	}
	else if(localStartPoint > 10 && localStartPoint < 100)
	{
		startPointValueText[0] = (uint8_t)localStartPoint /10 + 48;
		startPointValueText[1] = (uint8_t)localStartPoint %10 + 48;
		startPointValueText[2] = '.';
		startPointValueText[3] = (uint8_t)(((localStartPoint-(uint16_t)localStartPoint)*1000) /100) + 48;
		startPointValueText[4] = ((uint8_t)((localStartPoint-(uint16_t)localStartPoint)*1000) /10)%10 + 48;
		startPointValueText[5] = (uint8_t)((localStartPoint-(uint16_t)localStartPoint)*1000) %10 + 48;
		startPointValueText[6] = 's';
		startPointValueText[7] = 0;
	}
	else if(localStartPoint < 10)
	{
		startPointValueText[0] = (uint8_t)localStartPoint %10 + 48;
		startPointValueText[1] = '.';
		startPointValueText[2] = (uint8_t)(((localStartPoint-(uint16_t)localStartPoint)*1000) /100) + 48;
		startPointValueText[3] = ((uint8_t)((localStartPoint-(uint16_t)localStartPoint)*1000) /10)%10 + 48;
		startPointValueText[4] = (uint8_t)((localStartPoint-(uint16_t)localStartPoint)*1000) %10 + 48;
		startPointValueText[5] = 's';
		startPointValueText[6] = 0;
	}


	display.setControlText(topLabel[1], startPointValueText);
	display.setControlShow(topLabel[1]);
	display.refreshControl(topLabel[1]);
}

void cSampleRecorder::showEndPointValue()
{

	recTimeValue = recorder.getLength()/44100.0;
	float localEndPoint = (recTimeValue * endPoint) / MAX_16BIT;

	if(localEndPoint > 100)
	{
		endPointValueText[0] = (uint8_t)localEndPoint /100 + 48;
		endPointValueText[1] = ((uint8_t)localEndPoint /10)%10 + 48;
		endPointValueText[2] = (uint8_t)localEndPoint %10 + 48;
		endPointValueText[3] = '.';
		endPointValueText[4] = (uint8_t)(((localEndPoint-(uint16_t)localEndPoint)*1000) /100) + 48;
		endPointValueText[5] = ((uint8_t)((localEndPoint-(uint16_t)localEndPoint)*1000) /10)%10 + 48;
		endPointValueText[6] = (uint8_t)((localEndPoint-(uint16_t)localEndPoint)*1000) %10 + 48;
		endPointValueText[7] = 's';
		endPointValueText[8] = 0;
	}
	else if(localEndPoint > 10 && localEndPoint < 100)
	{
		endPointValueText[0] = (uint8_t)localEndPoint /10 + 48;
		endPointValueText[1] = (uint8_t)localEndPoint %10 + 48;
		endPointValueText[2] = '.';
		endPointValueText[3] = (uint8_t)(((localEndPoint-(uint16_t)localEndPoint)*1000) /100) + 48;
		endPointValueText[4] = ((uint8_t)((localEndPoint-(uint16_t)localEndPoint)*1000) /10)%10 + 48;
		endPointValueText[5] = (uint8_t)((localEndPoint-(uint16_t)localEndPoint)*1000) %10 + 48;
		endPointValueText[6] = 's';
		endPointValueText[7] = 0;
	}
	else if(localEndPoint < 10)
	{
		endPointValueText[0] = (uint8_t)localEndPoint %10 + 48;
		endPointValueText[1] = '.';
		endPointValueText[2] = (uint8_t)(((localEndPoint-(uint16_t)localEndPoint)*1000) /100) + 48;
		endPointValueText[3] = ((uint8_t)((localEndPoint-(uint16_t)localEndPoint)*1000) /10)%10 + 48;
		endPointValueText[4] = (uint8_t)((localEndPoint-(uint16_t)localEndPoint)*1000) %10 + 48;
		endPointValueText[5] = 's';
		endPointValueText[6] = 0;
	}


	display.setControlText(topLabel[2], endPointValueText);
	display.setControlShow(topLabel[2]);
	display.refreshControl(topLabel[2]);
}

void cSampleRecorder::showFreqValue()
{
	snprintf(freqTextValue, 5, "%f", recorderConfig.radioFreq);

	display.setControlText(topLabel[1], freqTextValue);
	display.setControlShow(topLabel[1]);
	display.refreshControl(topLabel[1]);
}

void cSampleRecorder::showSourceList()
{
	sourceList.start = recorderConfig.source;
	sourceList.length = sourceCount;
	sourceList.linesCount = 3;
	sourceList.data = sourceNames;

	display.setControlData(sourceListControl,  &sourceList);
	display.setControlShow(sourceListControl);
	display.refreshControl(sourceListControl);


}

void cSampleRecorder::showMonitorList()
{
	monitorList.start = recorderConfig.monitor;
	monitorList.length = monitorCount;
	monitorList.linesCount = 2;
	monitorList.data = monitorNames;

	display.setControlData(monitorListControl,  &monitorList);
	display.setControlShow(monitorListControl);
	display.refreshControl(monitorListControl);
}

void cSampleRecorder::drawRadioFreqBar()
{
	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlShow(radioFreqBarControl);
	display.refreshControl(radioFreqBarControl);
	showFreqValue();
}
void cSampleRecorder::drawLevelBar()
{
	if(currentScreen != screenTypeConfig ) return ;
	if(levelBarVal < 85)
	{
		if(redColorTim < 350) display.setControlColors(levelBarControl, colorRed);
		else
		{
			if(levelBarVal > 60)
			{
				uint8_t green = map(levelBarVal,60,85,255,0);

				colorGreen[0] = (0xFF << 16)| (green << 8);
			}
			else colorGreen[0] = 0x00FF00;
			display.setControlColors(levelBarControl, colorGreen);
		}

		if(redColorTim > 3000000) redColorTim = 201;
	}
	else
	{
		redColorTim = 0;
		display.setControlColors(levelBarControl, colorRed);

	}
	display.setControlValue(levelBarControl,levelBarVal);
	display.setControlShow(levelBarControl);
	display.refreshControl(levelBarControl);

}
void cSampleRecorder::drawGainBar()
{
	display.setControlValue(gainBarControl,gainBarVal);
	display.setControlShow(gainBarControl);
	display.refreshControl(gainBarControl);
}

void cSampleRecorder::showSaveHorizontalBar()
{
	saveProgress = recorder.getSaveProgress();
	display.setControlValue(saveHorizontalBarControl, saveProgress);
	display.setControlText(saveHorizontalBarControl, "saveing...");
	display.setControlShow(saveHorizontalBarControl);
	display.refreshControl(saveHorizontalBarControl);
}

void cSampleRecorder::hideSaveHorizontalBar()
{
	display.setControlHide(saveHorizontalBarControl);
	display.refreshControl(saveHorizontalBarControl);
}

void cSampleRecorder::showSelectionWindow()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}
	display.setControlText(bottomLabel[3], "Yes");
	display.setControlText(bottomLabel[4], "No");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.synchronizeRefresh();
}


