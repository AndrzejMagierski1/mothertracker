

#include "samplePlayback.h"


static uint16_t framesPlaces[7][4] =
{
	{0+1, 		421, 800/8-1, 65},
	{(800/8)*1+1, 421, 800/8-1, 65},
	{(800/8)*2+1, 421, 800/8-1, 65},
	{(800/8)*3+1, 421, 800/8-1, 65},
	{(800/8)*4+1, 421, 800/8-1, 65},
	{(800/8)*5+1, 421, 800/8-1, 65},
	{(800/8)*6+2, 31, 800/4-5, 387},
};



void cSamplePlayback::initDisplayControls()
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
		prop2.style = 	(controlStyleBackground | controlStyleCenterX | controlStyleCenterY);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 465;
		prop2.w = 800/8-6;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 437;
		prop2.h = 28;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
	}


	prop2.x = (800/4)*3+(800/8);
	prop2.w = 800/4-6;
	prop2.y = 452;
	prop2.h = 58;
	if(topLabel[6] == nullptr) topLabel[6] = display.createControl<cLabel>(&prop2);


	playModeList.linesCount = 5;
	playModeList.start = editorInstrument->playMode;
	playModeList.length = playModeCount;
	playModeList.data = playModeNames;
	prop.x = (800/8)*6+8;
	prop.y = 140;
	prop.w = 800/4-16;
	prop.h = 25;
	prop.data = &playModeList;
	if(playModeListControl == nullptr)  playModeListControl = display.createControl<cList>(&prop);


	// spectrum + points
	prop.x = 0;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	if(progressCursor == nullptr) progressCursor = display.createControl<cProgressCursor>(&prop);
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);

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

	display.destroyControl(playModeListControl);
	playModeListControl = nullptr;

	for(uint8_t i = 0; i<7; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		if(i == 6) break;

		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(progressCursor);
	progressCursor = nullptr;

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

	//cursor
	display.setControlShow(progressCursor);
	display.refreshControl(progressCursor);

	//points
	display.setControlShow(pointsControl);
	display.refreshControl(pointsControl);

	//lista
	//display.setControlShow(playModeListControl);
	//display.refreshControl(playModeListControl);

	// bottom labels

	display.setControlText(bottomLabel[0], "Preview");
	display.setControlText(bottomLabel[1], "Start");
	display.setControlText(bottomLabel[2], "Loop Start");
	display.setControlText(bottomLabel[3], "Loop End");
	display.setControlText(bottomLabel[4], "End");
	display.setControlText(bottomLabel[5], "Zoom");
	//display.setControlText(bottomLabel[7], "");


	display.setControlText(topLabel[6], "Play Mode");

	if(editorInstrument->playMode == singleShot) hideLoopPoints();
	else showLoopPoints();

	showStartPointValue();
	showEndPointValue();
	showLoopPoint1Value();
	showLoopPoint2Value();

	for(uint8_t i = 0; i<7; i++)
	{
		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);

		if(i == 6) break;

		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
	}


	display.setControlText(topLabel[0], startPointValueText);

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
void cSamplePlayback::showZoomValue()
{
	sprintf(zoomTextValue, "%.2f", zoom.zoomValue);

	display.setControlText(topLabel[5], zoomTextValue);
	display.setControlShow(topLabel[5]);
	display.refreshControl(topLabel[5]);
}

void cSamplePlayback::showPlayModeList()
{
	playModeList.start = editorInstrument->playMode;
	playModeList.length = playModeCount;
	playModeList.linesCount = 5;
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

	display.setControlText(topLabel[1], startPointValueText);
	display.setControlShow(topLabel[1]);
	display.refreshControl(topLabel[1]);
}

void cSamplePlayback::showEndPointValue()
{

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localEndPoint = (recTimeValue * editorInstrument->endPoint) / MAX_16BIT;

	sprintf(endPointValueText, "%.3fs", localEndPoint);

	display.setControlText(topLabel[4], endPointValueText);
	display.setControlShow(topLabel[4]);
	display.refreshControl(topLabel[4]);
}

void cSamplePlayback::showLoopPoint1Value()
{
	if(editorInstrument->playMode == singleShot) return;

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localLoopPoint1 = (recTimeValue * editorInstrument->loopPoint1) / MAX_16BIT;

	sprintf(loopPoint1ValueText, "%.3fs", localLoopPoint1);

	display.setControlText(topLabel[2], loopPoint1ValueText);
	display.setControlShow(topLabel[2]);
	display.refreshControl(topLabel[2]);
}
void cSamplePlayback::showLoopPoint2Value()
{
	if(editorInstrument->playMode == singleShot) return;

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localLoopPoint2 = (recTimeValue * editorInstrument->loopPoint2) / MAX_16BIT;

	sprintf(loopPoint2ValueText, "%.3fs", localLoopPoint2);

	display.setControlText(topLabel[3], loopPoint2ValueText);
	display.setControlShow(topLabel[3]);
	display.refreshControl(topLabel[3]);

}

void cSamplePlayback::showPreviewValue()
{
	float playTimeValue = ((playProgressValue * ((float)editorInstrument->sample.length /MAX_16BIT) )/44100.0);
	float localEndPoint = (editorInstrument->sample.length * editorInstrument->endPoint) / MAX_16BIT;
	if(playTimeValue >= (localEndPoint - 0.01)) playTimeValue = localEndPoint;

	sprintf(playTimeValueText, "%.3fs", playTimeValue);

	display.setControlText(topLabel[0], playTimeValueText);
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);
}

void cSamplePlayback::hidePreviewValue()
{
	display.setControlText(topLabel[0], startPointValueText);
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);
}
void cSamplePlayback::hideLoopPoints()
{
	display.setControlText(bottomLabel[2], "");
	display.setControlShow(bottomLabel[2]);
	display.refreshControl(bottomLabel[2]);

	display.setControlText(bottomLabel[3], "");
	display.setControlShow(bottomLabel[3]);
	display.refreshControl(bottomLabel[3]);

	display.setControlText(topLabel[2], "");
	display.setControlShow(topLabel[2]);
	display.refreshControl(topLabel[2]);

	display.setControlText(topLabel[3], "");
	display.setControlShow(topLabel[3]);
	display.refreshControl(topLabel[3]);
}

void cSamplePlayback::showLoopPoints()
{
	display.setControlText(bottomLabel[2],"Loop Start");
	display.setControlShow(bottomLabel[2]);
	display.refreshControl(bottomLabel[2]);

	display.setControlText(bottomLabel[3], "Loop End");
	display.setControlShow(bottomLabel[3]);
	display.refreshControl(bottomLabel[3]);

	showLoopPoint1Value();
	showLoopPoint2Value();

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
		i = i-(INSTRUMENTS_COUNT-1);
		sprintf(actualInstrName, "%d. MIDI Channel %d", i, i);
	}

	display.setControlShow(instrumentLabel);
	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}


//==============================================================================================================
