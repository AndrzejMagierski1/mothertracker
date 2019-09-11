

#include "sampleRecorder.h"
#include "Si4703.h"
#include "mtRecorder.h"
#include "mtLED.h"

constexpr uint8_t BACKSPACE_PAD_1 = 10;
constexpr uint8_t BACKSPACE_PAD_2 = 11;

constexpr uint8_t CAPS_LOCK_PAD_1 = 34;
constexpr uint8_t CAPS_LOCK_PAD_2 = 35;

constexpr uint8_t SPACE_PAD_1 = 43;
constexpr uint8_t SPACE_PAD_2 = 44;
constexpr uint8_t SPACE_PAD_3 = 45;
constexpr uint8_t SPACE_PAD_4 = 46;
constexpr uint8_t SPACE_PAD_5 = 47;

constexpr uint8_t F_PAD = 27;

constexpr uint8_t J_PAD = 30;

static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};

static  uint16_t framesPlacesS1[8][4] =
{
	{0+2, 		31, 800/8-5, 387},
	{(800/8)*1+2, 31, 800/8-5, 387},
	{(800/8)*2+1, 421, 800/8-1, 65},
	{(800/8)*3+1, 421, 800/8-1, 65},
	{(800/8)*4+2, 31, 800/8-5, 387},
	{(800/8)*5+2, 31, 800/8-5, 387},
	{(800/8)*6+2, 31, 800/8-5, 387},
	{(800/8)*7+1, 421, 800/8-1, 65}
};

static  uint16_t framesPlacesS2[8][4] =
{
		{0+1, 		421, 800/8-1, 65},
		{(800/8)*1+1, 421, 800/8-1, 65},
		{(800/8)*2+1, 421, 800/8-1, 65},
		{(800/8)*3+1, 421, 800/8-1, 65},
		{(800/8)*4+1, 421, 800/8-1, 65},
		{(800/8)*5+1, 421, 800/8-1, 65},
		{(800/8)*6+2, 31, 800/4-5, 487},
};




uint32_t colorRed[3] = { 0xFF0000, 0xFFFFFF,0xFF0000 };
uint32_t colorGreen[3] = { 0x00FF00, 0xFFFFFF,0x00FF00 };

uint32_t radioLabelColors[3]=
{
		0xFFFFFF, // tekst
		0xFFFFFF, // tło
		0xFF0000, // ramka
};

void cSampleRecorder::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);

	strControlProperties prop;
	prop.x = 100;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);
	if(progressCursor == nullptr) progressCursor = display.createControl<cProgressCursor>(&prop);

	points.pointsType = 0;
	points.endPoint = MAX_16BIT;
	points.startPoint = 0;
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);


	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;
		prop2.w = 800/8-6;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 437;
		prop2.h = 28;


		//prop2.style |= 	controlStyleManualText;
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
	sourceList.linesCount = 4;
	sourceList.start = recorderConfig.source;
	sourceList.length = 4;
	sourceList.data = sourceNames;
	prop4.x = 0+8;
	prop4.y = 152;
	prop4.w = 800/8-16;
	prop4.h = 25;
	prop4.data = &sourceList;
	if(sourceListControl == nullptr)  sourceListControl = display.createControl<cList>(&prop4);

	monitorList.linesCount = 2;
	monitorList.start = recorderConfig.monitor;
	monitorList.length = 2;
	monitorList.data = monitorNames;
	prop4.x = (800/8)*(6)+8;
	prop4.y = 165;
	prop4.w = 800/8-16;
	prop4.h = 25;
	prop4.data = &monitorList;
	if(monitorListControl == nullptr)  monitorListControl = display.createControl<cList>(&prop4);


	strControlProperties prop5;
	prop5.x = (800/8)*(4)+5;
	prop5.y = 30;
	prop5.w = 800/8-10;
	prop5.style =  controlStyleShow | controlStyleValue_0_100;
	prop5.h = 389;
	if(levelBarControl == nullptr)  levelBarControl = display.createControl<cBar>(&prop5);

	prop5.x = (800/8)*(5)+5;
	prop5.y = 30;
	prop5.w = 800/8-10;
	prop5.style =  controlStyleShow | controlStyleValue_0_100;
	prop5.h = 389;
	if(gainBarControl == nullptr)  gainBarControl = display.createControl<cBar>(&prop5);

	prop5.x = (800/8)*(1)+5;
	prop5.y = 30;
	prop5.w = 800/8-10;
	prop5.style =  controlStyleShow | controlStyleValue_0_100;
	prop5.h = 389;
	if(radioFreqBarControl == nullptr)  radioFreqBarControl = display.createControl<cBar>(&prop5);

	strControlProperties prop6;
	prop6.x = 10;
	prop6.y = 120;
	prop6.w = 780;
	prop6.h = 280;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop6);

	strControlProperties prop7;
	prop7.text = (char*)"";
	prop7.style = 	(controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleRoundedBorder);
	prop7.x = 393;
	prop7.y = 60;
	prop7.w = 765;
	prop7.h = 40;
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


	strControlProperties prop10;
	prop10.text = (char*)"";
	prop10.style = 	(controlStyleCenterX);
	prop10.x = (800/8)*3;
	prop10.y = 200;
	prop10.w = 0;
	prop10.h = 0;

	prop10.colors = radioLabelColors;

	if(radioRdsLabel == nullptr) radioRdsLabel = display.createControl<cLabel>(&prop10);

	padNamesStruct.length=5;
	padNamesStruct.name = interfaceGlobals.padNamesPointer;

	strControlProperties prop11;
	prop11.x = 16;
	prop11.y = 130;
	prop11.w = 780;
	prop11.h = 280;
	prop11.value=-1;
	prop11.data=&padNamesStruct;

	if(notePopoutControl== nullptr)  notePopoutControl = display.createControl<cNotePopout>(&prop11);

	display.setControlData(notePopoutControl, &padNamesStruct);


}


void cSampleRecorder::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(progressCursor);
	progressCursor = nullptr;

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

	display.destroyControl(notePopoutControl);
	notePopoutControl = nullptr;
}

void cSampleRecorder::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Sample Recorder");
	display.refreshControl(titleLabel);

	for(int i=0;i<8;i++)
	{
		display.setControlText(topLabel[i], "");
		display.setControlText(bottomLabel[i], "");
	}

	if (currentScreen == screenTypeConfig)
	{
		showGain();
		showSource();
		showMonitor();
		//spectrum
		display.setControlHide(spectrumControl);
		display.refreshControl(spectrumControl);

		//cursor
		display.setControlHide(progressCursor);
		display.refreshControl(progressCursor);

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


		resizer.resizeStyle[0]=makeSmaller;
		resizer.resizeStyle[1]=makeSmaller;
		resizer.resizeStyle[2]=makeBigger;
		resizer.resizeStyle[3]=makeBigger;
		resizer.resizeStyle[4]=makeBigger;
		resizer.resizeStyle[5]=makeSmaller;
		resizer.resizeStyle[6]=makeSmaller;
		resizer.resizeStyle[7]=makeBigger;

		resizeLabel(&resizer);

		if (recorderConfig.source == sourceTypeRadio)
		{
			display.setControlValue(radioFreqBarControl, radioFreqBarVal);
			display.setControlShow(radioFreqBarControl);
			display.refreshControl(radioFreqBarControl);
			display.setControlText(bottomLabel[1], "Radio Freq");
			display.setControlText(topLabel[2], "<<");
			display.setControlText(topLabel[3], ">>");

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


		display.setControlText(topLabel[4], "Level");
		display.setControlText(bottomLabel[5], "Gain");
		display.setControlText(bottomLabel[6], "Monitor");
		display.setControlText(topLabel[7], "Record");

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
		display.setControlShow(frameControl);
		display.refreshControl(frameControl);

		calcLevelBarVal();
		drawLevelBar();

		calcGainBarVal();
		drawGainBar();

		hideKeyboard();

		display.setControlHide(editName);
		display.refreshControl(editName);

	}
	else if (currentScreen == screenTypeRecord)
	{
		display.setControlHide(radioRdsLabel);
		display.refreshControl(radioRdsLabel);

		display.setControlShow(spectrumControl);
		display.refreshControl(spectrumControl);

		display.setControlShow(progressCursor);
		display.refreshControl(progressCursor);

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
		display.setControlShow(frameControl);
		display.refreshControl(frameControl);

		if (recordInProgressFlag == 1)
		{
			for(int i=0;i<7;i++)
			{
				display.setControlText(bottomLabel[i], "");
				display.setControlText(topLabel[i], "");
			}

			resizer.resizeStyle[7]=makeBigger;
			resizer.resizeStyle[2]=makeBigger;
			resizeLabel(&resizer);

			display.setControlText(topLabel[7], "Stop");
		}
		else
		{
			resizer.resizeStyle[0]=makeBigger;
			resizer.resizeStyle[1]=makeSmaller;
			resizer.resizeStyle[2]=makeSmaller;
			resizer.resizeStyle[3]=makeSmaller;
			resizer.resizeStyle[4]=makeBigger;
			resizer.resizeStyle[5]=makeBigger;
			resizer.resizeStyle[6]=makeBigger;

			resizeLabel(&resizer);

			display.setControlText(topLabel[0], "Preview");
			display.setControlText(bottomLabel[1], "Start Point");
			display.setControlText(bottomLabel[2], "End Point");
			display.setControlText(bottomLabel[3], "Zoom");
			display.setControlText(topLabel[4], "Crop");
			display.setControlText(topLabel[5], "Undo");
			display.setControlText(topLabel[6], "Go Back");
			display.setControlText(topLabel[7], "Save");
		}

		hideKeyboard();

		display.setControlHide(editName);
		display.refreshControl(editName);


	}
	else if(currentScreen == screenTypeKeyboard)
	{
		display.setControlHide(radioRdsLabel);
		display.refreshControl(radioRdsLabel);

		//spectrum
		display.setControlHide(spectrumControl);
		display.refreshControl(spectrumControl);

		//cursor
		display.setControlHide(progressCursor);
		display.refreshControl(progressCursor);

		display.setControlHide(frameControl);
		display.refreshControl(frameControl);
		//points

		display.setControlHide(pointsControl);
		display.refreshControl(pointsControl);

		showKeyboard();
		leds.setLED(F_PAD, 1, 10);
		leds.setLED(J_PAD, 1, 10);

		showKeyboardEditName();

		resizer.resizeStyle[2]=makeBigger;
		resizeLabel(&resizer);

		display.setControlText(bottomLabel[0], "");
		display.setControlText(bottomLabel[1], "");
		display.setControlText(topLabel[2], "Confirm");
		display.setControlText(bottomLabel[3], "");
		display.setControlText(bottomLabel[4], "");
		display.setControlText(topLabel[5], "Go Back");
		display.setControlText(topLabel[6], "Save & Load");
		display.setControlText(topLabel[7], "Save");
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		//display.setControlText(topLabel[i], "");
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}

	if ((currentScreen == screenTypeConfig) && recorderConfig.source == sourceTypeRadio)
	{
		showFreqValue();
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
	resizer.resizeStyle[1]=makeSmaller;
	resizer.resizeStyle[2]=makeBigger;
	resizer.resizeStyle[3]=makeBigger;

	resizeLabel(&resizer);

	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlShow(radioFreqBarControl);
	display.setControlText(bottomLabel[1], "Radio Freq");
	display.setControlText(topLabel[2], "<<");
	display.setControlText(topLabel[3], ">>");

	display.refreshControl(bottomLabel[1]);
	display.refreshControl(topLabel[2]);
	display.refreshControl(topLabel[3]);
	display.refreshControl(radioFreqBarControl);

	display.refreshControl(radioRdsLabel);
	display.setControlShow(radioRdsLabel);
	display.setControlText(radioRdsLabel, "");

	showFreqValue();
}
void cSampleRecorder::hideRadio()
{
	resizer.resizeStyle[1]=makeBigger;
	resizer.resizeStyle[2]=makeSmaller;
	resizer.resizeStyle[3]=makeSmaller;

	resizeLabel(&resizer);

	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlHide(radioFreqBarControl);
	display.setControlText(bottomLabel[1], "");
	display.setControlText(topLabel[2], "");
	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[1], "");

	display.refreshControl(bottomLabel[1]);
	display.refreshControl(topLabel[2]);
	display.refreshControl(topLabel[3]);
	display.refreshControl(topLabel[1]);
	display.refreshControl(radioFreqBarControl);

	display.refreshControl(radioRdsLabel);
	display.setControlHide(radioRdsLabel);
}

void cSampleRecorder::hideRDS()
{
	display.setControlHide(radioRdsLabel);
	display.refreshControl(radioRdsLabel);
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
	if(lastPressedPad == BACKSPACE_PAD_1 || lastPressedPad == BACKSPACE_PAD_2)
	{
		leds.setLED(BACKSPACE_PAD_1, 0, 0);
		leds.setLED(BACKSPACE_PAD_2, 0, 0);
	}
	else if(lastPressedPad == CAPS_LOCK_PAD_1 || lastPressedPad == CAPS_LOCK_PAD_2)
	{
		leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
		leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
	}
	else if(lastPressedPad >= SPACE_PAD_1 && lastPressedPad <=SPACE_PAD_5)
	{
		for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
		{
			leds.setLED(i, 0, 0);
		}
	}
	else
	{
		leds.setLED(lastPressedPad,0,0);
	}
	leds.setLED(F_PAD, 0, 0);
	leds.setLED(J_PAD, 0, 0);
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
	sprintf(zoomTextValue, "%.2f", zoom.zoomValue);

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

void cSampleRecorder::showPreviewValue()
{
	float playTimeValue = playProgresValueTim/1000.0;
	float localEndPoint = (recTimeValue * endPoint) / MAX_16BIT;
	if(playTimeValue >= (localEndPoint - 0.01)) playTimeValue = localEndPoint;
	if(playTimeValue > 100)
	{
		playTimeValueText[0] = (uint8_t)playTimeValue /100 + 48;
		playTimeValueText[1] = ((uint8_t)playTimeValue /10)%10 + 48;
		playTimeValueText[2] = (uint8_t)playTimeValue %10 + 48;
		playTimeValueText[3] = '.';
		playTimeValueText[4] = (uint8_t)(((playTimeValue-(uint16_t)playTimeValue)*1000) /100) + 48;
		playTimeValueText[5] = ((uint8_t)((playTimeValue-(uint16_t)playTimeValue)*1000) /10)%10 + 48;
		playTimeValueText[6] = (uint8_t)((playTimeValue-(uint16_t)playTimeValue)*1000) %10 + 48;
		playTimeValueText[7] = 's';
		playTimeValueText[8] = 0;
	}
	else if(playTimeValue > 10 && playTimeValue < 100)
	{
		playTimeValueText[0] = (uint8_t)playTimeValue /10 + 48;
		playTimeValueText[1] = (uint8_t)playTimeValue %10 + 48;
		playTimeValueText[2] = '.';
		playTimeValueText[3] = (uint8_t)(((playTimeValue-(uint16_t)playTimeValue)*1000) /100) + 48;
		playTimeValueText[4] = ((uint8_t)((playTimeValue-(uint16_t)playTimeValue)*1000) /10)%10 + 48;
		playTimeValueText[5] = (uint8_t)((playTimeValue-(uint16_t)playTimeValue)*1000) %10 + 48;
		playTimeValueText[6] = 's';
		playTimeValueText[7] = 0;
	}
	else if(playTimeValue < 10)
	{
		playTimeValueText[0] = (uint8_t)playTimeValue %10 + 48;
		playTimeValueText[1] = '.';
		playTimeValueText[2] = (uint8_t)(((playTimeValue-(uint16_t)playTimeValue)*1000) /100) + 48;
		playTimeValueText[3] = ((uint8_t)((playTimeValue-(uint16_t)playTimeValue)*1000) /10)%10 + 48;
		playTimeValueText[4] = (uint8_t)((playTimeValue-(uint16_t)playTimeValue)*1000) %10 + 48;
		playTimeValueText[5] = 's';
		playTimeValueText[6] = 0;
	}

	display.setControlText(topLabel[0], playTimeValueText);
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);
}
void cSampleRecorder::hidePreviewValue()
{
	display.setControlText(topLabel[0],"Preview");
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);
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
	snprintf(freqTextValue, 7, "%.2f", recorderConfig.radioFreq);

	display.setControlText(topLabel[1], freqTextValue);
	display.setControlShow(topLabel[1]);
	display.refreshControl(topLabel[1]);
}

void cSampleRecorder::showSourceList()
{
	sourceList.start = recorderConfig.source;
	sourceList.length = sourceCount;
	sourceList.linesCount = 4;
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
	display.setControlText(topLabel[0], "Yes");
	display.setControlText(topLabel[7], "No");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(selectWindowLabel,"Changes will be lost. Do you want to continue?");
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionWindowSave()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}
	display.setControlText(topLabel[0], "Yes");
	display.setControlText(topLabel[7], "No");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(selectWindowLabel,"This name already exists. Do you want to overwrite it?");
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionWindowFullMemory()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}
	display.setControlText(topLabel[7], "OK");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(selectWindowLabel,"The memory is full. Recording was stopped.");
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionNotEnoughInstruments()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(bottomLabel[i], "");
		display.setControlText(topLabel[i], "");
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
	}
	display.setControlText(topLabel[7], "OK");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	display.setControlText(selectWindowLabel,"There are not enough instruments");
	display.setControlShow(selectWindowLabel);
	display.refreshControl(selectWindowLabel);

	display.synchronizeRefresh();
}



void cSampleRecorder::refreshRDS()
{
	display.setControlText(radioRdsLabel, radio.rds_data.nazwaStacji);
	display.setControlShow(radioRdsLabel);
	display.refreshControl(radioRdsLabel);
}
void cSampleRecorder::displaySeeking()
{
	display.setControlText(radioRdsLabel, "Seeking...");
	display.setControlShow(radioRdsLabel);
	display.refreshControl(radioRdsLabel);
}
void cSampleRecorder::displayEmptyRDS()
{
	display.setControlText(radioRdsLabel,"");
	display.setControlShow(radioRdsLabel);
	display.refreshControl(radioRdsLabel);
}

void cSampleRecorder::showNotePopout()
{
	display.setControlText(titleLabel, "Notes");
	display.refreshControl(titleLabel);

	display.setControlShow(notePopoutControl);
	display.refreshControl(notePopoutControl);

	for(int i=0;i<8;i++)
	{
		display.setControlHide(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlHide(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	display.setControlHide(sourceListControl);
	display.refreshControl(sourceListControl);

	display.setControlHide(monitorListControl);
	display.refreshControl(monitorListControl);

	display.setControlHide(levelBarControl);
	display.refreshControl(levelBarControl);

	display.setControlHide(gainBarControl);
	display.refreshControl(gainBarControl);

	if(recorderConfig.source == sourceTypeRadio)
	{
		display.setControlHide(radioFreqBarControl);
		display.refreshControl(radioFreqBarControl);
	}

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);
}

void cSampleRecorder::hideNotePopout()
{
	display.setControlText(titleLabel, "Sample Recoder");
	display.refreshControl(titleLabel);

	display.setControlHide(notePopoutControl);
	display.refreshControl(notePopoutControl);

	for(int i=0;i<8;i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}

	display.setControlShow(sourceListControl);
	display.refreshControl(sourceListControl);

	display.setControlShow(monitorListControl);
	display.refreshControl(monitorListControl);

	display.setControlShow(levelBarControl);
	display.refreshControl(levelBarControl);

	display.setControlShow(gainBarControl);
	display.refreshControl(gainBarControl);

	if(recorderConfig.source == sourceTypeRadio)
	{
		display.setControlShow(radioFreqBarControl);
		display.refreshControl(radioFreqBarControl);
	}

	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

void cSampleRecorder::resizeLabel(button_resize_t *handle)
{
	for(int i=0;i<handle->buttonsToResize;i++)
	{
		//display.setControlText(topLabel[i],"");
		//display.setControlText(bottomLabel[i],"");

		if(handle->resizeStyle[i]==makeBigger)
		{
			display.setControlSize(topLabel[i], (800/8)-6, 58);
			display.setControlPosition(topLabel[i], (800/8)*i+(800/16), 452);
			display.refreshControl(topLabel[i]);
		}
		else
		{
			display.setControlSize(topLabel[i], (800/8)-6, 28);
			display.setControlPosition(topLabel[i], (800/8)*i+(800/16), 437);
			display.refreshControl(topLabel[i]);
		}
	}

}

void cSampleRecorder::showSource()
{
	if (currentScreen == screenTypeConfig)
	{
		strcpy(sourceName,sourcesNamesLabels[recorderConfig.source]);
		display.setControlText(topLabel[0],sourceName);
		display.setControlShow(topLabel[0]);
		display.refreshControl(topLabel[0]);
	}
}

void cSampleRecorder::showMonitor()
{
	if (currentScreen == screenTypeConfig)
	{
	strcpy(monitorVal,monitorNamesLabels[recorderConfig.monitor]);
	display.setControlText(topLabel[6],monitorVal);
	display.setControlShow(topLabel[6]);
	display.refreshControl(topLabel[6]);
	}
}

void cSampleRecorder::showGain()
{
	if(recorderConfig.source == sourceTypeLineIn)
	{
		sprintf(gainVal,"%d",recorderConfig.gainLineIn);
	}
	else if(recorderConfig.source == sourceTypeRadio)
	{
		sprintf(gainVal,"%d",recorderConfig.gainRadio);
	}
	else if(recorderConfig.source == sourceTypeMicLG)
	{
		sprintf(gainVal,"%d",recorderConfig.gainMicLow);
	}
	else if(recorderConfig.source == sourceTypeMicHG)
	{
		sprintf(gainVal,"%d",recorderConfig.gainMicHigh);
	}

	display.setControlText(topLabel[5],gainVal);
	display.setControlShow(topLabel[5]);
	display.refreshControl(topLabel[5]);
}

