

#include "samplePlayback.h"
#include "mtAudioEngine.h"

static uint16_t framesPlaces[7][4] =
{
	{0+1, 		  424, 800/8-2, 55},
	{(800/8)*1+1, 424, 800/8-2, 55},
	{(800/8)*2+1, 424, 800/8-2, 55},
	{(800/8)*3+1, 424, 800/8-2, 55},
	{(800/8)*4+1, 424, 800/8-2, 55},
	{(800/8)*5+1, 424, 800/8-2, 55},
	{(800/8)*6+1, 29,  800/4-3, 391},
};



void cSamplePlayback::initDisplayControls()
{
	// inicjalizacja kontrolek
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY | controlStyleFont4);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);

	strControlProperties prop;
	// ramka
	frameData.placesCount = 7;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	prop.style = 0;
	prop.value = 0;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);


	for(uint8_t i = 0; i<6; i++)
	{
		prop2.text = (char*)"";
		//prop2.data =  &bottomValuesConfig;
		prop2.colors = interfaceGlobals.activeLabelsColors;

		prop2.style = 	( controlStyleCenterX | controlStyleCenterY );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 452;
		prop2.h =  59;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 255;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);


	prop2.x = (800/4)*3+(800/8);
	prop2.w = 800/4-6;
	prop2.y = 452;
	prop2.h = 59;
	if(label[6] == nullptr) label[6] = display.createControl<cLabel>(&prop2);


	playModeList.linesCount = 7;
	playModeList.start = editorInstrument->playMode;
	playModeList.length = playModeCount;
	playModeList.data = playModeNames;
	prop.x = (800/8)*6+1;
	prop.y = 29;
	prop.w = 800/4-3;
	prop.h = 394;
	prop.style = controlStyleBackground;
	prop.data = &playModeList;
	if(playModeListControl == nullptr)  playModeListControl = display.createControl<cList>(&prop);


	// spectrum + points
	prop.style = 0;
	prop.x = 0;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	if(progressCursor == nullptr) progressCursor = display.createControl<cProgressCursor>(&prop);
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);
	prop.data = &slicePoints;
	if(slicePointsControl == nullptr)  slicePointsControl = display.createControl<cSlicePoints>(&prop);
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);

	prop.x = 0;
	prop.y = 400;
	prop.w = 600;
	prop.h = 2;
	prop.data = &wtPosition;

	if(wtPositionCursor == nullptr)  wtPositionCursor = display.createControl<cWtProgress>(&prop);
}


void cSamplePlayback::destroyDisplayControls()
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

	display.destroyControl(slicePointsControl);
	slicePointsControl = nullptr;

	display.destroyControl(playModeListControl);
	playModeListControl = nullptr;

	for(uint8_t i = 0; i<7; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;



	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(progressCursor);
	progressCursor = nullptr;


	display.destroyControl(wtPositionCursor);
	wtPositionCursor = nullptr;

}

void cSamplePlayback::showTitleBar()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Sample Playback");
	display.refreshControl(titleLabel);

	showActualInstrument();
}


void cSamplePlayback::showDefaultScreen()
{
	showTitleBar();

	//spectrum
	display.setControlShow(spectrumControl);
	display.refreshControl(spectrumControl);

//	lista
//	display.setControlShow(playModeListControl);
//	display.refreshControl(playModeListControl);

	// bottom labels
	showPlayModeList();

	//display.setControlText(bottomLabel[7], "");



	if(loadedInstrumentType == mtSampleTypeWaveFile)
	{
		display.setControlShow(progressCursor);
		display.refreshControl(progressCursor);


		if(editorInstrument->playMode == playModeSlice)
		{
			display.setControlValue(label[0], 1);
			display.setControlValue(label[1], 1);
			display.setControlValue(label[2], 0);
			display.setControlValue(label[3], 0);
			display.setControlValue(label[4], 0);
			display.setControlValue(label[5], 1);
			display.setControlValue(label[6], 0);

			display.setControlText2(label[0], "Slice");
			display.setControlText2(label[1], "Adjust");
			display.setControlText2(label[5], "Zoom");

			display.setControlText(label[0], "");
			display.setControlText(label[1], "");
			display.setControlText(label[2], "Add");
			display.setControlText(label[3], "Remove");
			display.setControlText(label[4], "Auto Slice");
			display.setControlText(label[5], "");
			display.setControlText(label[6], "Play Mode");

			display.setControlHide(pointsControl);
			display.refreshControl(pointsControl);

			display.setControlShow(slicePointsControl);
			display.refreshControl(slicePointsControl);

			showZoomValue();
			showSlicesAdjustValue();
			showSlicesSelectValue();
			processWavetableCursor(editorInstrument->wavetableCurrentWindow);
			hideWavetablePositionCursor();

		}
		else
		{
			display.setControlHide(slicePointsControl);
			display.refreshControl(slicePointsControl);

			display.setControlValue(label[0], 1);
			display.setControlValue(label[1], 1);
			display.setControlValue(label[2], 1);
			display.setControlValue(label[3], 1);
			display.setControlValue(label[4], 1);
			display.setControlValue(label[5], 1);
			display.setControlValue(label[6], 0);

			display.setControlText2(label[0], "Preview");
			display.setControlText2(label[1], "Start");
			display.setControlText2(label[2], "Loop Start");
			display.setControlText2(label[3], "Loop End");
			display.setControlText2(label[4], "End");
			display.setControlText2(label[5], "Zoom");
			display.setControlText(label[6], "Play Mode");

			display.setControlShow(pointsControl);
			display.refreshControl(pointsControl);

			if((editorInstrument->playMode == singleShot) || (editorInstrument->playMode == playModeWavetable)) hideLoopPoints();
			else showLoopPoints();

			showStartPointValue();
			showEndPointValue();
			showLoopPoint1Value();
			showLoopPoint2Value();
			showZoomValue();
			processWavetableCursor(editorInstrument->wavetableCurrentWindow);
			hideWavetablePositionCursor();

			display.setControlText(label[0], startPointValueText);
		}
	}
	else
	{
		display.setControlValue(label[0], 0);
		display.setControlValue(label[1], 1);
		display.setControlValue(label[2], 1);
		display.setControlValue(label[3], 0);
		display.setControlValue(label[4], 0);
		display.setControlValue(label[5], 0);
		display.setControlValue(label[6], 0);

		display.setControlText(label[0], "Preview");
		display.setControlText2(label[1], "Position");
		display.setControlText2(label[2], "Window");
		display.setControlText(label[3], "");
		display.setControlText(label[4], "");
		display.setControlText(label[5], "");
		display.setControlText(label[6], "Play Mode");

//		display.setControlHide(playModeListControl);
//		display.refreshControl(playModeListControl);

		//cursor
		display.setControlHide(progressCursor);
		display.refreshControl(progressCursor);

		//points
		display.setControlHide(pointsControl);
		display.refreshControl(pointsControl);

		display.setControlHide(slicePointsControl);
		display.refreshControl(slicePointsControl);

		showWavetablePosition();
		showWavetableWindowSize();

		processWavetableCursor(editorInstrument->wavetableCurrentWindow);
		showWavetablePositionCursor();
	}

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);


	display.synchronizeRefresh();

}


//==============================================================================================================
void cSamplePlayback::activateLabelsBorder()
{
	if(selectedPlace > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================
void cSamplePlayback::showWavetablePosition()
{
	if(refreshWavetablePosition) sprintf(wavetablePositionText, "%d",(int) currentEnvelopeWtPos);
	else sprintf(wavetablePositionText, "%d",(int) editorInstrument->wavetableCurrentWindow);

	display.setControlText(label[1], wavetablePositionText);
	display.setControlShow(label[1]);
	display.refreshControl(label[1]);
}

void cSamplePlayback::showWavetableWindowSize()
{
	sprintf(wavetableWindowSizeText, "%d", editorInstrument->sample.wavetable_window_size);

	display.setControlText(label[2], wavetableWindowSizeText);
	display.setControlShow(label[2]);
	display.refreshControl(label[2]);
}

void cSamplePlayback::showZoomValue()
{
	sprintf(zoomTextValue, "%.2f", zoom.zoomValue);

	display.setControlText(label[5], zoomTextValue);
	display.setControlShow(label[5]);
	display.refreshControl(label[5]);
}

void cSamplePlayback::showPlayModeList()
{
	playModeList.start = editorInstrument->playMode;
	playModeList.length = playModeCount;
	playModeList.linesCount = 7;
	playModeList.data = playModeNames;

	display.setControlData(playModeListControl,  &playModeList);
	display.setControlShow(playModeListControl);
	display.refreshControl(playModeListControl);
}

void cSamplePlayback::showStartPointValue()
{
	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localStartPoint = (recTimeValue * editorInstrument->startPoint) / MAX_16BIT;

	sprintf(startPointValueText, "%.3fs", localStartPoint);

	display.setControlText(label[1], startPointValueText);
	display.setControlShow(label[1]);
	display.refreshControl(label[1]);
}

void cSamplePlayback::showEndPointValue()
{

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localEndPoint = (recTimeValue * editorInstrument->endPoint) / MAX_16BIT;

	sprintf(endPointValueText, "%.3fs", localEndPoint);

	display.setControlText(label[4], endPointValueText);
	display.setControlShow(label[4]);
	display.refreshControl(label[4]);
}

void cSamplePlayback::showLoopPoint1Value()
{
	if(editorInstrument->playMode == singleShot) return;

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localLoopPoint1 = (recTimeValue * editorInstrument->loopPoint1) / MAX_16BIT;

	sprintf(loopPoint1ValueText, "%.3fs", localLoopPoint1);

	display.setControlText(label[2], loopPoint1ValueText);
	display.setControlShow(label[2]);
	display.refreshControl(label[2]);
}
void cSamplePlayback::showLoopPoint2Value()
{
	if(editorInstrument->playMode == singleShot) return;

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localLoopPoint2 = (recTimeValue * editorInstrument->loopPoint2) / MAX_16BIT;

	sprintf(loopPoint2ValueText, "%.3fs", localLoopPoint2);

	display.setControlText(label[3], loopPoint2ValueText);
	display.setControlShow(label[3]);
	display.refreshControl(label[3]);

}

void cSamplePlayback::showPreviewValue()
{
	float playTimeValue = ((playProgressValue * ((float)editorInstrument->sample.length /MAX_16BIT) )/44100.0);
	float localEndPoint = (editorInstrument->sample.length * editorInstrument->endPoint) / MAX_16BIT;
	if(playTimeValue >= (localEndPoint - 0.01)) playTimeValue = localEndPoint;

	sprintf(playTimeValueText, "%.3fs", playTimeValue);

	display.setControlText(label[0], playTimeValueText);
	display.setControlShow(label[0]);
	display.refreshControl(label[0]);
}

void cSamplePlayback::hidePreviewValue()
{
	display.setControlText(label[0], startPointValueText);
	display.setControlShow(label[0]);
	display.refreshControl(label[0]);
}
void cSamplePlayback::hideLoopPoints()
{
	display.setControlText(label[2], "");
	display.setControlText2(label[2], "");
	display.setControlShow(label[2]);
	display.refreshControl(label[2]);

	display.setControlText(label[3], "");
	display.setControlText2(label[3], "");
	display.setControlShow(label[3]);
	display.refreshControl(label[3]);
}

void cSamplePlayback::showLoopPoints()
{
	display.setControlText2(label[2],"Loop Start");
	display.setControlShow(label[2]);
	display.refreshControl(label[2]);

	display.setControlText2(label[3], "Loop End");
	display.setControlShow(label[3]);
	display.refreshControl(label[3]);

	showLoopPoint1Value();
	showLoopPoint2Value();

}

void cSamplePlayback::showWavetablePositionCursor()
{
	display.setControlData(wtPositionCursor, &wtPosition);
	display.setControlShow(wtPositionCursor);
	display.refreshControl(wtPositionCursor);
}

void cSamplePlayback::hideWavetablePositionCursor()
{
	display.setControlHide(wtPositionCursor);
	display.refreshControl(wtPositionCursor);
}

void cSamplePlayback::showSlicesSelectValue()
{
	sprintf(sliceSelectTextValue, "%d", editorInstrument->selectedSlice + 1);

	display.setControlText(label[0], sliceSelectTextValue);
	display.setControlShow(label[0]);
	display.refreshControl(label[0]);
}

void cSamplePlayback::showSlicesAdjustValue()
{
	float timeValue = editorInstrument->sample.length/44100.0;
	uint16_t slice =  (editorInstrument->sliceNumber == 0 ) ? 0 : editorInstrument->slices[editorInstrument->selectedSlice];
	float currentSlice = (timeValue * slice) / MAX_16BIT;

	sprintf(sliceAdjustTextValue, "%.3f ms", currentSlice);

	display.setControlText(label[1], sliceAdjustTextValue);
	display.setControlShow(label[1]);
	display.refreshControl(label[1]);
}



//==============================================================================================================
void cSamplePlayback::showActualInstrument()
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
		sprintf(actualInstrName, "%d. MIDI Channel %d",  i+3, i-(INSTRUMENTS_COUNT-1));
	}

	display.setControlShow(instrumentLabel);
	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}

//==============================================================================================================
