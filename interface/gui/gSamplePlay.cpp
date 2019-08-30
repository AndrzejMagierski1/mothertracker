

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
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);

	strControlProperties prop;

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
	prop2.y = 465;
	prop2.w = 800/4-6;
	prop2.h = 30;
	if(bottomLabel[6] == nullptr) bottomLabel[6] = display.createControl<cLabel>(&prop2);

	prop2.y = 437;
	prop2.h = 28;
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
	prop.data = &spectrum;
	if(spectrumControl == nullptr)  spectrumControl = display.createControl<cSpectrum>(&prop);
	if(progressCursor == nullptr) progressCursor = display.createControl<cProgressCursor>(&prop);


	prop.x = 0;
	prop.y = 75;
	prop.w = 600;
	prop.h = 300;
	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);


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

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;

	display.destroyControl(progressCursor);
	progressCursor = nullptr;
}

void cSamplePlayback::showDefaultScreen()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Sample Playback");
	display.refreshControl(titleLabel);

	showActualInstrument();


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
	display.setControlText(bottomLabel[0], "Start");
	display.setControlText(bottomLabel[1], "Loop Start");
	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlText(bottomLabel[3], "End");
	//display.setControlText(bottomLabel[4], "Play Mode");
	display.setControlText(bottomLabel[5], "Zoom");
	display.setControlText(bottomLabel[6], " /\\\           \\\/ ");
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
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}


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
	sprintf(zoomTextValue, "%.2f", zoomValue);
/*
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
*/

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


	display.setControlText(topLabel[0], startPointValueText);
	display.setControlShow(topLabel[0]);
	display.refreshControl(topLabel[0]);
}

void cSamplePlayback::showEndPointValue()
{

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localEndPoint = (recTimeValue * editorInstrument->endPoint) / MAX_16BIT;

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


	display.setControlText(topLabel[3], endPointValueText);
	display.setControlShow(topLabel[3]);
	display.refreshControl(topLabel[3]);
}

void cSamplePlayback::showLoopPoint1Value()
{
	if(editorInstrument->playMode == singleShot) return;

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localLoopPoint1 = (recTimeValue * editorInstrument->loopPoint1) / MAX_16BIT;

	if(localLoopPoint1 > 100)
	{
		loopPoint1ValueText[0] = (uint8_t)localLoopPoint1 /100 + 48;
		loopPoint1ValueText[1] = ((uint8_t)localLoopPoint1 /10)%10 + 48;
		loopPoint1ValueText[2] = (uint8_t)localLoopPoint1 %10 + 48;
		loopPoint1ValueText[3] = '.';
		loopPoint1ValueText[4] = (uint8_t)(((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) /100) + 48;
		loopPoint1ValueText[5] = ((uint8_t)((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) /10)%10 + 48;
		loopPoint1ValueText[6] = (uint8_t)((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) %10 + 48;
		loopPoint1ValueText[7] = 's';
		loopPoint1ValueText[8] = 0;
	}
	else if(localLoopPoint1 > 10 && localLoopPoint1 < 100)
	{
		loopPoint1ValueText[0] = (uint8_t)localLoopPoint1 /10 + 48;
		loopPoint1ValueText[1] = (uint8_t)localLoopPoint1 %10 + 48;
		loopPoint1ValueText[2] = '.';
		loopPoint1ValueText[3] = (uint8_t)(((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) /100) + 48;
		loopPoint1ValueText[4] = ((uint8_t)((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) /10)%10 + 48;
		loopPoint1ValueText[5] = (uint8_t)((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) %10 + 48;
		loopPoint1ValueText[6] = 's';
		loopPoint1ValueText[7] = 0;
	}
	else if(localLoopPoint1 < 10)
	{
		loopPoint1ValueText[0] = (uint8_t)localLoopPoint1 %10 + 48;
		loopPoint1ValueText[1] = '.';
		loopPoint1ValueText[2] = (uint8_t)(((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) /100) + 48;
		loopPoint1ValueText[3] = ((uint8_t)((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) /10)%10 + 48;
		loopPoint1ValueText[4] = (uint8_t)((localLoopPoint1-(uint16_t)localLoopPoint1)*1000) %10 + 48;
		loopPoint1ValueText[5] = 's';
		loopPoint1ValueText[6] = 0;
	}


	display.setControlText(topLabel[1], loopPoint1ValueText);
	display.setControlShow(topLabel[1]);
	display.refreshControl(topLabel[1]);
}
void cSamplePlayback::showLoopPoint2Value()
{
	if(editorInstrument->playMode == singleShot) return;

	float recTimeValue = editorInstrument->sample.length/44100.0;
	float localLoopPoint2 = (recTimeValue * editorInstrument->loopPoint2) / MAX_16BIT;

	if(localLoopPoint2 > 100)
	{
		loopPoint2ValueText[0] = (uint8_t)localLoopPoint2 /100 + 48;
		loopPoint2ValueText[1] = ((uint8_t)localLoopPoint2 /10)%10 + 48;
		loopPoint2ValueText[2] = (uint8_t)localLoopPoint2 %10 + 48;
		loopPoint2ValueText[3] = '.';
		loopPoint2ValueText[4] = (uint8_t)(((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) /100) + 48;
		loopPoint2ValueText[5] = ((uint8_t)((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) /10)%10 + 48;
		loopPoint2ValueText[6] = (uint8_t)((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) %10 + 48;
		loopPoint2ValueText[7] = 's';
		loopPoint2ValueText[8] = 0;
	}
	else if(localLoopPoint2 > 10 && localLoopPoint2 < 100)
	{
		loopPoint2ValueText[0] = (uint8_t)localLoopPoint2 /10 + 48;
		loopPoint2ValueText[1] = (uint8_t)localLoopPoint2 %10 + 48;
		loopPoint2ValueText[2] = '.';
		loopPoint2ValueText[3] = (uint8_t)(((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) /100) + 48;
		loopPoint2ValueText[4] = ((uint8_t)((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) /10)%10 + 48;
		loopPoint2ValueText[5] = (uint8_t)((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) %10 + 48;
		loopPoint2ValueText[6] = 's';
		loopPoint2ValueText[7] = 0;
	}
	else if(localLoopPoint2 < 10)
	{
		loopPoint2ValueText[0] = (uint8_t)localLoopPoint2 %10 + 48;
		loopPoint2ValueText[1] = '.';
		loopPoint2ValueText[2] = (uint8_t)(((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) /100) + 48;
		loopPoint2ValueText[3] = ((uint8_t)((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) /10)%10 + 48;
		loopPoint2ValueText[4] = (uint8_t)((localLoopPoint2-(uint16_t)localLoopPoint2)*1000) %10 + 48;
		loopPoint2ValueText[5] = 's';
		loopPoint2ValueText[6] = 0;
	}


	display.setControlText(topLabel[2], loopPoint2ValueText);
	display.setControlShow(topLabel[2]);
	display.refreshControl(topLabel[2]);

}

void cSamplePlayback::showPreviewValue()
{
	float playTimeValue = ((playProgressValue * ((float)editorInstrument->sample.length /MAX_16BIT) )/44100.0);
	float localEndPoint = (editorInstrument->sample.length * editorInstrument->endPoint) / MAX_16BIT;
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

	display.setControlText(topLabel[4], playTimeValueText);
	display.setControlShow(topLabel[4]);
	display.refreshControl(topLabel[4]);
}

void cSamplePlayback::hidePreviewValue()
{
	display.setControlText(topLabel[4], "");
	display.setControlShow(topLabel[4]);
	display.refreshControl(topLabel[4]);
}
void cSamplePlayback::hideLoopPoints()
{
	display.setControlText(bottomLabel[1], "");
	display.setControlShow(bottomLabel[1]);
	display.refreshControl(bottomLabel[1]);

	display.setControlText(bottomLabel[2], "");
	display.setControlShow(bottomLabel[2]);
	display.refreshControl(bottomLabel[2]);

	display.setControlText(topLabel[1], "");
	display.setControlShow(topLabel[1]);
	display.refreshControl(topLabel[1]);

	display.setControlText(topLabel[2], "");
	display.setControlShow(topLabel[2]);
	display.refreshControl(topLabel[2]);
}

void cSamplePlayback::showLoopPoints()
{
	display.setControlText(bottomLabel[1],"Loop Start");
	display.setControlShow(bottomLabel[1]);
	display.refreshControl(bottomLabel[1]);

	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlShow(bottomLabel[2]);
	display.refreshControl(bottomLabel[2]);

	showLoopPoint1Value();
	showLoopPoint2Value();

}


//==============================================================================================================
void cSamplePlayback::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	if(i<9)
	{
		actualInstrName[0] = (i+1)%10 + 48;
		actualInstrName[1] = '.';
		actualInstrName[2] = ' ';
		actualInstrName[3] = 0;
	}
	else
	{
		actualInstrName[0] = ((i+1)/10) + 48;
		actualInstrName[1] = (i+1)%10 + 48;
		actualInstrName[2] = '.';
		actualInstrName[3] = ' ';
		actualInstrName[4] = 0;
	}


	strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);


	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}

//==============================================================================================================
