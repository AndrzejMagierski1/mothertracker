

#include "samplePlayback.h"


static uint16_t framesPlaces[7][4] =
{
	{0, 		412, 800/8, 68},
	{(800/8)*1, 412, 800/8, 68},
	{(800/8)*2, 412, 800/8, 68},
	{(800/8)*3, 412, 800/8, 68},
	{(800/8)*4, 412, 800/8, 68},
	{(800/8)*5, 412, 800/8, 68},
	{(800/8)*6, 0, 800/4, 480},
};



void cSamplePlayback::initDisplayControls()
{
	// inicjalizacja kontrolek

	strControlProperties prop;
	strControlProperties prop2;

	for(uint8_t i = 0; i<6; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);

		prop2.x = (800/8)*i+(800/16);
		prop2.y = 415;
		prop2.h = 26;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);
	}

	prop2.x = (800/4)*3+(800/8);
	prop2.y = 450;
	prop2.w = 800/4-10;
	prop2.h = 30;
	if(bottomLabel[6] == nullptr) bottomLabel[6] = display.createControl<cLabel>(&prop2);

	prop2.y = 415;
	prop2.h = 26;
	if(topLabel[6] == nullptr) topLabel[6] = display.createControl<cLabel>(&prop2);


	playModeList.linesCount = 5;
	playModeList.start = editorInstrument->playMode;
	playModeList.length = playModeCount;
	playModeList.data = playModeNames;
	prop.x = (800/8)*6+5;
	prop.y = 140;
	prop.w = 800/4-10;
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
