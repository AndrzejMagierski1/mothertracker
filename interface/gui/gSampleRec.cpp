

#include "sampleRecorder.h"


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

static uint32_t colorRed[3] = { 0xFF0000, 0xFF0000,0xFF0000 };
static uint32_t colorGreen[3] = { 0x00FF00, 0x00FF00,0x00FF00 };

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


	prop.data = &points;
	if(pointsControl == nullptr)  pointsControl = display.createControl<cPoints>(&prop);

	strControlProperties prop2;


	for(uint8_t i = 0; i<8; i++)
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


}

void cSampleRecorder::showDefaultScreen()
{

	if(currentScreen == screenTypeConfig)
	{
		//spectrum
		display.setControlHide(spectrumControl);
		display.refreshControl(spectrumControl);

		//points

		display.setControlHide(pointsControl);
		display.refreshControl(pointsControl);

	//	listy
		display.setControlShow(sourceListControl);
		display.setControlValue(sourceListControl,recorderConfig.source);
		display.refreshControl(sourceListControl);

		display.setControlShow(monitorListControl);
		display.setControlValue(monitorListControl,recorderConfig.monitor);
		display.refreshControl(monitorListControl);

	//bar
		display.setControlValue(levelBarControl,levelBarVal);
		display.setControlShow(levelBarControl);
		display.refreshControl(levelBarControl);

		display.setControlValue(gainBarControl,gainBarVal);
		display.setControlShow(gainBarControl);
		display.refreshControl(gainBarControl);

		if(recorderConfig.source == sourceTypeRadio)
		{
			display.setControlValue(radioFreqBarControl,radioFreqBarVal);
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
			display.setControlValue(radioFreqBarControl,radioFreqBarVal);
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



		calcLevelBarVal();
		drawLevelBar();

		calcGainBarVal();
		drawGainBar();
	}
	else if(currentScreen == screenTypeRecord)
	{

	}


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);

		display.setControlShow(topLabel[i]);
		display.refreshControl(topLabel[i]);
	}



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

//	display.setControlText(topLabel[5], zoomTextValue);
//	display.setControlShow(topLabel[5]);
//	display.refreshControl(topLabel[5]);
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
	if(levelBarVal < 75) display.setControlColors(levelBarControl, colorGreen);
	else display.setControlColors(levelBarControl, colorRed);
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
