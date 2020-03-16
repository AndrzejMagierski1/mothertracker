

#include "sampleRecorder.h"

#include "core/interfacePopups.h"

#include "Si4703.h"
#include "mtRecorder.h"
#include "mtLED.h"
#include "sdCardDetect.h"

static uint32_t popUpLabelColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	0xFF0000, // ramka
};

static  uint16_t framesPlacesS1[8][4] =
{
		{0+1, 		  29, 800/8-3, 391},
		{(800/8)*1+1, 29, 800/8-3, 391},
		{(800/8)*2+1, 424, 800/8-3, 55},
		{(800/8)*3+1, 424, 800/8-3, 55},
		{(800/8)*4+1, 29, 800/8-3, 391},
		{(800/8)*5+1, 29, 800/8-3, 391},
		{(800/8)*6+1, 29, 800/8-3, 391},
		{(800/8)*7+1, 424, 800/8-3, 55}
};

static  uint16_t framesPlacesS2[8][4] =
{
		{0+1, 		  424, 800/8-3, 55},
		{(800/8)*1+1, 424, 800/8-3, 55},
		{(800/8)*2+1, 424, 800/8-3, 55},
		{(800/8)*3+1, 424, 800/8-3, 55},
		{(800/8)*4+1, 424, 800/8-3, 55},
		{(800/8)*5+1, 424, 800/8-3, 55},
		{(800/8)*6+1, 424, 800/8-3, 55},
		{(800/8)*7+1, 424, 800/8-3, 55},
};




uint32_t colorRed[3] = { one_true_red, 0xFFFFFF,0xFF0000 };
uint32_t colorGreen[3] = { 0x32d642, 0xFFFFFF,0x00FF00 };

uint32_t levelBarColors[] =
{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
};

uint32_t radioLabelColors[3]=
{
		0xFFFFFF, // tekst
		0x0a0a0a, // tło
		0xFF0000, // ramka
};

void cSampleRecorder::initDisplayControls()
{

//	strControlProperties prop9;
//
//	prop9.x = 400;
//	prop9.colors = popUpLabelColors;
////	prop.colors = &color[0];
//	prop9.y = 300;
//	//prop.w = 800/4-10;
////	prop9.style = controlStyleValue_0_100;
//	prop9.h = 100;
//	prop9.w = 800-(10);
//	prop9.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 );
//	prop9.text = (char*)"Changes will be lost. Do you want to continue?";
//	if(popupWindowLabel == nullptr)  popupWindowLabel = display.createControl<cLabel>(&prop9);

	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);

	points.pointsType = 0;
	points.endPoint = MAX_16BIT;
	points.startPoint = 0;
	strControlProperties prop;
	prop.x = 100;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	if(progressCursor == nullptr) progressCursor = display.createControl<cProgressCursor>(&prop);
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);

	doubleArrow[0].bitmaps[0].bitmapIndex = displayDoubleArrowL;
	doubleArrow[0].bitmaps[0].xValue =  (800/8)*2+(800/16);
	doubleArrow[0].bitmaps[0].yValue = 437;
	doubleArrow[1].bitmaps[0].bitmapIndex = displayDoubleArrowR;
	doubleArrow[1].bitmaps[0].xValue =  (800/8)*3+(800/16);
	doubleArrow[1].bitmaps[0].yValue = 437;

	for(uint8_t i = 0; i<8; i++)
	{
		prop2.value = 1;
		prop2.colors = interfaceGlobals.activeLabelsColors;
		prop2.style = 	( controlStyleCenterX | controlStyleFont3 );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.value = 255;
	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 255;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);

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
	prop4.x = 0+1;
	prop4.y = 29;
	prop4.w = 800/8-3;
	prop4.h = 394;
	prop4.data = &sourceList;
	prop4.style = controlStyleBackground;
	if(sourceListControl == nullptr)  sourceListControl = display.createControl<cList>(&prop4);

	monitorList.linesCount = 2;
	monitorList.start = recorderConfig.monitor;
	monitorList.length = 2;
	monitorList.data = monitorNames;
	prop4.x = (800/8)*(6)+1;
	prop4.y = 29;
	prop4.w = 800/8-3;
	prop4.h = 394;
	prop4.data = &monitorList;
	if(monitorListControl == nullptr)  monitorListControl = display.createControl<cList>(&prop4);


	strControlProperties prop5;
	prop5.x = (800/8)*(4)+1;
	prop5.y = 29;
	prop5.w = 800/8-3;
	prop5.style =  controlStyleShow | controlStyleValue_0_100 | controlStyleBackground;
	prop5.h = 394;
	prop5.colors = levelBarColors;
	if(levelBarControl == nullptr)  levelBarControl = display.createControl<cBar>(&prop5);
	prop5.x = (800/8)*(5)+1;
	prop5.colors = nullptr;
	if(gainBarControl == nullptr)  gainBarControl = display.createControl<cBar>(&prop5);
	prop5.x = (800/8)*(1)+1;
	if(radioFreqBarControl == nullptr)  radioFreqBarControl = display.createControl<cBar>(&prop5);

	strControlProperties prop6;
	prop6.x = 13;
	prop6.y = 143;
	prop6.w = 780;
	prop6.h = 260;
	if(keyboardControl == nullptr)  keyboardControl = display.createControl<cKeyboard>(&prop6);

	strControlProperties prop7;
	prop7.text = (char*)"";
	prop7.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleFont2);
	prop7.x = 400;
	prop7.y = 29;
	prop7.w = 795;
	prop7.h = 90;
	if(editName == nullptr)  editName = display.createControl<cEdit>(&prop7);

	strControlProperties prop8;
	prop8.x = 190;
	prop8.y = 170;
	prop8.style = controlStyleValue_0_100;
	prop8.h = 100;
	prop8.w = 420;
	if(saveHorizontalBarControl == nullptr)  saveHorizontalBarControl = display.createControl<cHorizontalBar>(&prop8);



	strControlProperties prop10;
	prop10.text = (char*)"";
	prop10.style = 	(controlStyleCenterX | controlStyleCenterY | controlStyleBackground | controlStyleBottomShadow);
	prop10.x = (800/8)*3-1;
	//prop10.y = 29+394/2;
	prop10.w = 800/4-3;
	prop10.h = 394;
	prop10.y = 29;
	prop10.colors = radioLabelColors;
	if(radioRdsLabel == nullptr) radioRdsLabel = display.createControl<cLabel>(&prop10);

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
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


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

	//display.destroyControl(popupWindowLabel);
	//popupWindowLabel = nullptr;


	display.destroyControl(radioRdsLabel);
	radioRdsLabel = nullptr;


	mtPopups.hideInfoPopup();
}

void cSampleRecorder::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Sample Recorder");
	display.refreshControl(titleLabel);

	for(int i=0;i<8;i++)
	{
		display.setControlText(label[i], "");
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
	}

	//display.setControlShow(bgLabel);
	//display.refreshControl(bgLabel);

	display.setControlData(label[2], &doubleArrow[0]);
	display.setControlData(label[3], &doubleArrow[1]);
	display.setRemoveControlStyle(label[2], controlStyleShowBitmap);
	display.setRemoveControlStyle(label[3], controlStyleShowBitmap);

	// odciemnanie labela wrazie deaktywacji save po wyjeciu karty
	display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);

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

		if (recorderConfig.source == sourceTypeRadio)
		{
			display.setControlValue(radioFreqBarControl, radioFreqBarVal);
			display.setControlShow(radioFreqBarControl);
			display.refreshControl(radioFreqBarControl);
			display.setControlText(label[1], "Radio Freq");
			display.setAddControlStyle(label[2], controlStyleShowBitmap);
			display.setAddControlStyle(label[3], controlStyleShowBitmap);


			display.setControlValue(bgLabel,243);

			calcRadioFreqBarVal();
			drawRadioFreqBar();
		}
		else
		{
			//display.setControlValue(radioFreqBarControl, radioFreqBarVal);
			display.setControlHide(radioFreqBarControl);
			display.refreshControl(radioFreqBarControl);
			display.setControlText(label[1], "");
			display.setControlText(label[2], "");
			display.setControlText(label[3], "");
			display.setControlValue(bgLabel,255);



			if(selectedPlace == 1 || selectedPlace == 2 || selectedPlace == 3)
			{
				selectedPlace = 0;
			}
		}

		// bottom labels
		display.setControlText(label[0], "Source");

		display.setControlText(label[4], "Level");
		display.setControlText(label[5], "Gain");
		display.setControlText(label[6], "Monitor");
		display.setControlText(label[7], "Record");

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
		display.refreshControl(bgLabel);

		calcLevelBarVal();
		drawLevelBar();

		calcGainBarVal();
		drawGainBar();

		keyboardManager.deactivateKeyboard();

		display.setControlHide(editName);
		display.refreshControl(editName);

		clearAllNodes();
		cancelMultiFrame();

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
				display.setControlText(label[i], "");
				display.setControlText2(label[i], "");
			}

			display.setControlValue(bgLabel,140);

			display.setControlText(label[7], "Stop");
		}
		else
		{

			display.setControlText(label[0], "Preview");
			display.setControlText(label[1], "Start Point");
			display.setControlText(label[2], "End Point");
			display.setControlText(label[3], "Zoom");
			display.setControlText(label[4], "Crop");
			display.setControlText(label[5], "Undo");
			display.setControlText(label[6], "Cancel");
			display.setControlText(label[7], "Save");
			display.setControlValue(bgLabel,255);

			if(!sdCardDetector.isCardInserted())
			{
				//dezaktywacja kiedy brak karty sd
				display.setControlColors(label[7], interfaceGlobals.inactiveLabelsColors);
			}
		}

		keyboardManager.deactivateKeyboard();

		display.setControlHide(editName);
		display.refreshControl(editName);
		display.refreshControl(bgLabel);

		if(selectedPlace == 1)
		{
			points.selected = selectStart;
		}
		else if(selectedPlace == 2)
		{
			points.selected = selectEnd;
		}

		clearAllNodes();
		cancelMultiFrame();
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

		keyboardManager.activateKeyboard();


		display.setControlText(label[0], "Enter");
		display.setControlText(label[1], "");
		display.setControlText2(label[1], "");
		display.setControlText(label[2], "");
		display.setControlText2(label[2], "");
		display.setControlText(label[3], "");
		display.setControlText2(label[3], "");
		display.setControlText(label[4], "Auto Name");
		display.setControlText2(label[4], "");
		display.setControlText(label[5], "Cancel");
		display.setControlText(label[6], "Save & Load");
		display.setControlText(label[7], "Save");
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		//display.setControlText(topLabel[i], "");
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
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

	mtPopups.hideInfoPopup();
	//display.setControlHide(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showRadio()
{

	calcRadioFreqBarVal();

	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlShow(radioFreqBarControl);
	display.setControlText(label[1], "Radio Freq");
	display.setAddControlStyle(label[2], controlStyleShowBitmap);
	display.setAddControlStyle(label[3], controlStyleShowBitmap);
	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setControlText(label[2], "");
	display.setControlText(label[3], "");

	display.refreshControl(label[2]);
	display.refreshControl(label[3]);

	display.refreshControl(radioFreqBarControl);

	display.refreshControl(radioRdsLabel);
	display.setControlShow(radioRdsLabel);
	display.setControlText(radioRdsLabel, "");

	display.setControlValue(bgLabel,255);
	display.refreshControl(bgLabel);

	showFreqValue();
}
void cSampleRecorder::hideRadio()
{

	display.setControlValue(radioFreqBarControl,radioFreqBarVal);
	display.setControlHide(radioFreqBarControl);
	display.setControlText(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText(label[3], "");
	display.setControlText2(label[1], "");
	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setRemoveControlStyle(label[2], controlStyleShowBitmap);
	display.setRemoveControlStyle(label[3], controlStyleShowBitmap);

	display.refreshControl(label[1]);
	display.refreshControl(label[2]);
	display.refreshControl(label[3]);

	display.refreshControl(radioFreqBarControl);

	display.refreshControl(radioRdsLabel);
	display.setControlHide(radioRdsLabel);


	display.setControlValue(bgLabel,243);
	display.refreshControl(bgLabel);


}

void cSampleRecorder::hideRDS()
{
	display.setControlHide(radioRdsLabel);
	display.refreshControl(radioRdsLabel);
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

	display.setControlText2(label[3], zoomTextValue);
	display.refreshControl(label[3]);
}

void cSampleRecorder::showRecTimeValue()
{
	recTimeValue = recorder.getLength()/44100.0;

	sprintf(recTimeValueText,"%.3fs",recTimeValue);

	display.setControlText2(label[2], recTimeValueText);
	display.refreshControl(label[2]);
}

void cSampleRecorder::showPreviewValue()
{
	float playTimeValue = ((playProgressValue * ((float) recorder.getLength()/MAX_16BIT) )/44100.0);
	float localEndPoint = (recorder.getLength()  * endPoint) / MAX_16BIT;

	if(playTimeValue >= (localEndPoint - 0.01)) playTimeValue = localEndPoint;

	sprintf(playTimeValueText,"%.3fs",playTimeValue);

	display.setControlText(label[0],"Preview");
	display.setControlText2(label[0], playTimeValueText);
	display.setControlColors(label[0],interfaceGlobals.activeLabelsColors);
	display.setControlShow(label[0]);
	display.refreshControl(label[0]);
}
void cSampleRecorder::hidePreviewValue()
{
	display.setControlText(label[0],"Preview");
	display.setControlText2(label[0],"");
	display.setControlColors(label[0],interfaceGlobals.activeLabelsColors);
	display.refreshControl(label[0]);
}
void cSampleRecorder::showStartPointValue()
{
	recTimeValue = recorder.getLength()/44100.0;
	float localStartPoint = (recTimeValue * startPoint) / MAX_16BIT;

	sprintf(startPointValueText,"%.3fs",localStartPoint);

	display.setControlText2(label[1], startPointValueText);
	display.refreshControl(label[1]);
}

void cSampleRecorder::showEndPointValue()
{
	recTimeValue = recorder.getLength()/44100.0;
	float localEndPoint = (recTimeValue * endPoint) / MAX_16BIT;

	sprintf(endPointValueText,"%.3fs",localEndPoint);

	display.setControlText2(label[2], endPointValueText);
	display.refreshControl(label[2]);
}

void cSampleRecorder::showFreqValue()
{
	snprintf(freqTextValue, 9, "%.1fMHz", recorderConfig.radioFreq);

	display.setControlText2(label[1], freqTextValue);
	display.refreshControl(label[1]);
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
	if(currentScreen != screenTypeConfig ) return;

	if(levelBarVal < 85)
	{
		if(redColorTimer < 350)
		{
			levelBarColors[0] = one_true_red;
		}
		else
		{
			if(levelBarVal > 70)
			{
				uint8_t green = map(levelBarVal,70,85,((one_true_green&0xff00)>>8),0);
				levelBarColors[0] = (one_true_red&0xff0000) | (green << 8);
			}
			else if(levelBarVal > 60)
			{
				//uint8_t green = map(levelBarVal,40,60,((one_true_green&0xff00)>>8),0);
				uint8_t red = map(levelBarVal,60,70,0,((one_true_red&0xff0000)>>16));
				levelBarColors[0] = (red << 16) | (one_true_green&0xff00);
			}
			else
			{
				levelBarColors[0] = one_true_green;
			}
		}

		if(redColorTimer > 3000000) redColorTimer = 201;
	}
	else
	{
		redColorTimer = 0;
		levelBarColors[0] = one_true_red;

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
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");

		display.refreshControl(label[i]);
	}

	display.setControlText(label[7], "Yes");
	display.setControlText(label[6], "Cancel");
	display.setControlColors(label[6], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	mtPopups.showInfoPopup("Changes will be lost. Do you want to continue?", nullptr);
	//display.setControlText(popupWindowLabel,"Changes will be lost. Do you want to continue?");
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionWindowSave()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");

		display.refreshControl(label[i]);
	}

	display.setControlText(label[0], "Yes");
	display.setControlText(label[7], "No");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	mtPopups.showInfoPopup("This name already exists.", "Do you want to overwrite it?");
	//display.setControlText(popupWindowLabel,"This name already exists. Do you want to overwrite it?");
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionWindowFullMemory()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");

		display.refreshControl(label[i]);
	}
	display.setControlText(label[7], "OK");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	mtPopups.showInfoPopup("The memory is full. Recording has been stopped.", nullptr);
	//display.setControlText(popupWindowLabel,"The memory is full. Recording has been stopped.");
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionNotEnoughInstruments()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");

		display.refreshControl(label[i]);
	}
	display.setControlText(label[7], "OK");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	//display.setControlText(popupWindowLabel,"There are not enough instruments");

	mtPopups.showInfoPopup("There are not enough instruments", nullptr);
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionNotEnoughMemory()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");

		display.refreshControl(label[i]);
	}
	display.setControlText(label[7], "OK");

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	mtPopups.showInfoPopup("There is not enough Memory in sample bank.", nullptr);
	//display.setControlText(popupWindowLabel,"There is not enough Memory in sample bank.");
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.synchronizeRefresh();
}

void cSampleRecorder::showSelectionStopPattern()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		display.setControlText(label[i], "");
		display.setControlText2(label[i], "");

		display.refreshControl(label[i]);
		display.setControlShow(label[i]);
	}

	display.setControlText(label[7], "Yes");
	display.setControlText(label[6], "No");
	display.setControlColors(label[6], interfaceGlobals.activeLabelsColors);
	display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);

	display.refreshControl(label[6]);
	display.refreshControl(label[6]);

	display.setControlHide(frameControl);
	display.refreshControl(frameControl);

	mtPopups.showInfoPopup("This action will stop the pattern.", "Do you want to continue?");
	//display.setControlText(popupWindowLabel,"This action will stop the pattern. Do you want to continue?");
	//display.setControlShow(popupWindowLabel);
	//display.refreshControl(popupWindowLabel);

	display.setControlValue(bgLabel,255);
	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);

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


	for(int i=0;i<8;i++)
	{
		display.setControlHide(label[i]);
		display.refreshControl(label[i]);
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


void cSampleRecorder::showSource()
{
	if (currentScreen == screenTypeConfig)
	{
		strcpy(sourceName,sourcesNamesLabels[recorderConfig.source]);
		display.setControlText2(label[0],sourceName);
		display.setControlShow(label[0]);
		display.refreshControl(label[0]);
	}
}

void cSampleRecorder::showMonitor()
{
	if (currentScreen == screenTypeConfig)
	{
	strcpy(monitorVal,monitorNamesLabels[recorderConfig.monitor]);
	display.setControlText2(label[6],monitorVal);
	display.setControlShow(label[6]);
	display.refreshControl(label[6]);
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

	display.setControlText2(label[5],gainVal);
	display.setControlShow(label[5]);
	display.refreshControl(label[5]);
}

