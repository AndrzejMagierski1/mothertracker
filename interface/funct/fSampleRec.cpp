

#include "sampleRecorder.h"
#include "mtRecorder.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"




extern AudioControlSGTL5000 audioShield;

cSampleRecorder sampleRecorder;
static cSampleRecorder* SR = &sampleRecorder;

extern strMtProject mtProject;


static  uint8_t functPlayAction(uint8_t state);
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functSelectButton0();
static  uint8_t functSelectButton1();
static  uint8_t functSelectButton2();
static  uint8_t functSelectButton3();
static  uint8_t functSelectButton4();
static  uint8_t functSelectButton5();
static  uint8_t functSelectButton6();
static  uint8_t functSelectButton7();


static  uint8_t functActionButton0(uint8_t s);
static  uint8_t functActionButton1();
static  uint8_t functActionButton2();
static  uint8_t functActionButton3();
static  uint8_t functActionButton4();
static  uint8_t functActionButton5();
static  uint8_t functActionButton6();
static  uint8_t functActionButton7();

static uint8_t functActionSource();
static uint8_t functActionGain();
static  uint8_t functActionMonitor();
static  uint8_t functActionRecord();
static  uint8_t functActionRadioFreq();
static  uint8_t functActionRadioLeft();
static  uint8_t functActionRadioRight();
static  uint8_t functActionPreview();
static 	uint8_t functActionStopPreview();
static  uint8_t functActionCrop();
static  uint8_t functActionUndo();
static  uint8_t functActionGoBack();
static  uint8_t functActionSave();
static  uint8_t functActionEndPoint();
static  uint8_t functActionStartPoint();
static  uint8_t functActionZoom();





static uint8_t functShift(uint8_t value);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);






void cSampleRecorder::update()
{
	if(refreshSpectrum)
	{
		processSpectrum();

		display.refreshControl(SR->spectrumControl);
		if(recordInProgressFlag) spectrumTimerConstrains+=50;
		refreshSpectrum = 0;
	}

	if(refreshPoints)
	{
		processPoints();
//
		display.refreshControl(SR->pointsControl);

		refreshPoints = 0;
	}

	if(previevFlag != lastPrevievFlag)
	{
		if(previevFlag) audioShield.headphoneSourceSelect(1);
		else audioShield.headphoneSourceSelect(0);
	}
	lastPrevievFlag = previevFlag;

	if(recordInProgressFlag)
	{
		showRecTimeValue();
		if(refreshSpectrumTimer > spectrumTimerConstrains)
		{
			refreshSpectrumTimer = 0;
			refreshSpectrum = 1;
		}
	}


	changeLevelBar();

}

void cSampleRecorder::start(uint32_t options)
{
	moduleRefresh = 1;

//--------------------------------------------------------------------

	processSpectrum();
	processPoints();
	listMonitor();
	listSource();

	showMonitorList();
	showSourceList();
	showZoomValue();

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	activateLabelsBorder();

	setPrevievFlag(1);

}


void cSampleRecorder::stop()
{

	moduleRefresh = 0;
}


void cSampleRecorder::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay,functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

//	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
//	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);


	FM->setButtonObj(interfaceButton0, functActionButton0);
	FM->setButtonObj(interfaceButton1, buttonPress, functActionButton1);
	FM->setButtonObj(interfaceButton2, buttonPress, functActionButton2);
	FM->setButtonObj(interfaceButton3, buttonPress, functActionButton3);
	FM->setButtonObj(interfaceButton4, buttonPress, functActionButton4);
	FM->setButtonObj(interfaceButton5, buttonPress, functActionButton5);
	FM->setButtonObj(interfaceButton6, buttonPress, functActionButton6);
	FM->setButtonObj(interfaceButton7, buttonPress, functActionButton7);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

//==============================================================================================================


void cSampleRecorder::processPoints()
{

	points.pointsType = 0;

	if(startPoint >= zoomStart && startPoint <= zoomEnd)
	{
		points.startPoint = ((startPoint-zoomStart) * 599) / zoomWidth;
	}
	else points.startPoint = -1;

	if(endPoint >= zoomStart && endPoint <= zoomEnd)
	{
		points.endPoint = ((endPoint-zoomStart) * 599) / zoomWidth;
	}
	else points.endPoint = -1;

}


void cSampleRecorder::processSpectrum()
{

	if(recorder.getLength() == 0)
	{
		for(uint16_t i = 0; i < 600; i++)
		{
			spectrum.upperData[i] = 0;
			spectrum.lowerData[i] = 0;
		}

		return;
	}
	if(recordInProgressFlag)
	{
		int16_t * sampleData = recorder.getStartAddress();
		uint32_t resolution = recorder.getLength() / 600;

		if(resolution < 1) resolution = 1;

		uint16_t offset_pixel = 0;

		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomEnd = MAX_16BIT;

		int16_t up = 0;
		int16_t low = 0;
		uint32_t step = 0;

		for(uint16_t i = offset_pixel; i < 600; i++)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData+step+j);


				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum.upperData[i] =  up;
			spectrum.lowerData[i] = low;

		}

		if(resolution <= 1) spectrum.spectrumType = 1;
		else spectrum.spectrumType = 0;
		return;
	}


	// uwaga tu wazna kolejnosc + do sprawdzenia
//	if()
//	{
//		for(uint16_t i = 0; i < 600; i++)
//		{
//			spectrum.upperData[i] = 0;
//			spectrum.lowerData[i] = 0;
//		}
//
//		return;
//	}

	uint16_t offset_pixel;
	int16_t * sampleData;



	uint32_t resolution;


	switch(lastChangedPoint)
	{
		case 0: zoomPosition = startPoint; break; //MAX_16BIT/2; break;

		case 1:
			zoomPosition = startPoint;
		break;
		case 2:
			zoomPosition = endPoint;
		break;

		default: zoomPosition = startPoint; break; //MAX_16BIT/2; break;
	}







	if(zoomValue > 1.0)
	{
		zoomWidth = (MAX_16BIT/zoomValue);
		zoomStart =  zoomPosition - zoomWidth/2;
		zoomEnd = zoomPosition + zoomWidth/2;

		if(zoomStart < 0)
		{
			zoomEnd = zoomWidth;
			zoomStart = 0;
			offset_pixel = ((zoomPosition-zoomStart) * 599) / zoomWidth;
		}
		else if(zoomEnd > MAX_16BIT)
		{
			zoomEnd = MAX_16BIT;
			zoomStart = MAX_16BIT-zoomWidth;
			offset_pixel = ((zoomPosition-zoomStart) * 599) / zoomWidth;
		}
		else
		{
			offset_pixel = ((zoomPosition-zoomStart) * 599) / zoomWidth;
		}


		uint32_t offset = ((float)zoomPosition/MAX_16BIT) * recorder.getLength();

		sampleData = recorder.getStartAddress() + offset;

		resolution = (((float)zoomWidth/MAX_16BIT) * recorder.getLength() ) / 600;


//		Serial.print(zoomValue);
//		Serial.print("   ");
//		Serial.print(zoomStart);
//		Serial.print("   ");
//		Serial.print(zoomEnd);
//		Serial.print("   ");
//
//		Serial.println();

	}
	else
	{

		offset_pixel = 0;
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomEnd = MAX_16BIT;
		sampleData = recorder.getStartAddress();;
		resolution = recorder.getLength() / 600;
	}


	if(resolution < 1) resolution = 1;


	int16_t up = 0;
	int16_t low = 0;

	uint32_t step = 0;



	if(offset_pixel > 0)
	{
		for(int16_t i = offset_pixel-1; i >= 0; i--)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData-step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum.upperData[i] =  up;
			spectrum.lowerData[i] = low;
		}
	}

	up = 0;
	low = 0;
	step = 0;


	for(uint16_t i = offset_pixel; i < 600; i++)
	{
		low = up = 0; //*(sampleData+step);

		for(uint16_t j = 0; j < resolution; j++)
		{
			int16_t sample = *(sampleData+step+j);


			if(sample > up)  up = sample;
			else if(sample < low) low = sample;

		}
		step+= resolution;

		up = up/300;
		low = low/300;

		spectrum.upperData[i] =  up;
		spectrum.lowerData[i] = low;
	}

	if(resolution <= 1) spectrum.spectrumType = 1;
	else spectrum.spectrumType = 0;

}

void cSampleRecorder::listSource()
{

	for(uint8_t i = 0; i < sourceCount; i++)
	{
		sourceNames[i] = (char*)&sourcesNamesLabels[i][0];
	}


}

void cSampleRecorder::listMonitor()
{

	for(uint8_t i = 0; i < monitorCount; i++)
	{
		monitorNames[i] = (char*)&monitorNamesLabels[i][0];
	}


}

//==============================================================================================================
static  uint8_t functSelectButton0()
{
	if(SR->recordInProgressFlag == 1) return 1;
	SR->selectedPlace = 0;
	SR->activateLabelsBorder();
	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}

	return 1;
}
static  uint8_t functSelectButton1()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 1;
	SR->activateLabelsBorder();

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 1;
		if(SR->zoomValue > 1.0)
		{
			SR->refreshSpectrum = 1;
		}
		SR->refreshPoints = 1;
	}

	return 1;
}
static  uint8_t functSelectButton2()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 2;
	SR->activateLabelsBorder();

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 2;
		if(SR->zoomValue > 1.0)
		{
			SR->refreshSpectrum = 1;
		}
		SR->refreshPoints = 1;
	}
	return 1;
}
static  uint8_t functSelectButton3()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 3;
	SR->activateLabelsBorder();

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}
static  uint8_t functSelectButton4()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(SR->currentScreen == cSampleRecorder::screenTypeConfig) return 1;
	SR->selectedPlace = 4;
	SR->activateLabelsBorder();

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}
static  uint8_t functSelectButton5()
{
	if(SR->recordInProgressFlag == 1) return 1;
	SR->selectedPlace = 5;
	SR->activateLabelsBorder();

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}
static  uint8_t functSelectButton6()
{
	if(SR->recordInProgressFlag == 1) return 1;
	SR->selectedPlace = 6;
	SR->activateLabelsBorder();
	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}
static  uint8_t functSelectButton7()
{
	SR->selectedPlace = 7;
	SR->activateLabelsBorder();
	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}


static  uint8_t functActionButton0(uint8_t s)
{
	if(s == 1)
	{
		functSelectButton0();
		switch(SR->currentScreen)
		{
			case cSampleRecorder::screenTypeConfig:		functActionSource(); 			break;
			case cSampleRecorder::screenTypeRecord: 	functActionPreview();			break;
			case cSampleRecorder::screenTypeKeyboard: break;
			default: break;
		}
	}
	else if ( s == 0)
	{
		switch(SR->currentScreen)
		{
			case cSampleRecorder::screenTypeConfig:	break;
			case cSampleRecorder::screenTypeRecord:  functActionStopPreview(); break;
			case cSampleRecorder::screenTypeKeyboard: break;
			default: break;
		}
	}

	return 1;
}

static  uint8_t functActionButton1()
{
	functSelectButton1();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionRadioFreq();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionStartPoint();		break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton2()
{
	functSelectButton2();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig:		functActionRadioLeft();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionEndPoint();			break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton3()
{
	functSelectButton3();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig:		functActionRadioRight(); 		break;
		case cSampleRecorder::screenTypeRecord: 	functActionZoom();				break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton4()
{
	functSelectButton4();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: break;
		case cSampleRecorder::screenTypeRecord: 	functActionCrop();				break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton5()
{
	functSelectButton5();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionGain();				break;
		case cSampleRecorder::screenTypeRecord:		functActionUndo();				break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton6()
{
	functSelectButton6();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionMonitor();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionGoBack();			break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton7()
{
	functSelectButton7();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionRecord();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionSave();				break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
	}
	return 1;
}
//==============================================================================================================
static uint8_t functActionSource()
{
	return 1;
}

static uint8_t functActionGain()
{
	return 1;
}

static  uint8_t functActionMonitor()
{
	return 1;
}

static  uint8_t functActionRecord()
{
	SR->recordInProgressFlag = 1;
	SR->currentScreen = SR->screenTypeRecord;
	SR->showDefaultScreen();
	recorder.startRecording(sdram_effectsBank);
	SR->spectrumTimerConstrains = 100;
	return 1;
}

static  uint8_t functActionRadioFreq()
{
	return 1;
}

static  uint8_t functActionRadioLeft()
{
	return 1;
}

static  uint8_t functActionRadioRight()
{
	return 1;
}

static  uint8_t functActionPreview()
{
	if(SR->recordInProgressFlag == 1) return 1;
	recorder.play(SR->startPoint,SR->endPoint);
	return 1;
}

static uint8_t functActionStopPreview()
{
	recorder.stop();
	return 1;
}

static  uint8_t functActionCrop()
{
	if(SR->recordInProgressFlag == 1) return 1;
	for(int8_t i = (undoCount-2); i>=0 ; i--)
	{
		SR->undo[i+1] = SR->undo[i];
	}


	SR->undo[0].address = recorder.getStartAddress();
	SR->undo[0].length = recorder.getLength();


	if(SR->cropCounter < undoCount) SR->cropCounter++;

	recorder.trim(SR->startPoint,SR->endPoint);
	SR->startPoint = 0;
	SR->endPoint = MAX_16BIT;

	SR->showEndPointValue();
	SR->showStartPointValue();
	SR->refreshSpectrum = 1;
	SR->refreshPoints = 1;
	return 1;
}

static  uint8_t functActionUndo()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(!SR->cropCounter) return 1;
	SR->cropCounter--;
	recorder.undo(SR->undo[0].address,SR->undo[0].length);

	for(uint8_t i = 0 ; i< (undoCount-1); i++)
	{
		SR->undo[i] = SR->undo[i+1];

	}
	SR->startPoint = 0;
	SR->endPoint = MAX_16BIT;

	SR->showEndPointValue();
	SR->showStartPointValue();
	SR->refreshSpectrum = 1;
	SR->refreshPoints = 1;

	return 1;
}

static  uint8_t functActionGoBack()
{
	if(SR->recordInProgressFlag == 1) return 1;
	//todo: okienko pytajace czy zapisac o ile nie zapisano nic wczesniej
	SR->currentScreen = cSampleRecorder::screenTypeConfig;
	if(!SR->recorderConfig.monitor) audioShield.headphoneSourceSelect(1);
	SR->showDefaultScreen();

	return 1;
}

static  uint8_t functActionSave()
{

	if(SR->recordInProgressFlag == 1)
	{
		SR->recordInProgressFlag = 0;
		SR->startPoint = 0;
		SR->endPoint = MAX_16BIT;
		SR->showDefaultScreen();
		recorder.stopRecording();
		audioShield.headphoneSourceSelect(0);


		return 1;
	}

	return 1;
}

static  uint8_t functActionEndPoint()
{
	if(SR->recordInProgressFlag == 1) return 1;
	SR->points.selected = 2;
	if(SR->zoomValue > 1.0)
	{
		SR->refreshSpectrum = 1;
	}
	SR->refreshPoints = 1;
	return 1;
}

static  uint8_t functActionStartPoint()
{
	if(SR->recordInProgressFlag == 1) return 1;

	return 1;
}

static  uint8_t functActionZoom()
{
	if(SR->recordInProgressFlag == 1) return 1;


	return 1;
}




static  uint8_t functEncoder(int16_t value)
{

	if(SR->currentScreen == cSampleRecorder::screenTypeConfig)
	{
		switch(SR->selectedPlace)
		{
		case 0: 	SR->changeSourceSelection(value);			break;
		case 1: 	SR->changeRadioFreqBar(value);				break;
		case 2: 	 											break;
		case 3: 												break;
		case 5: 	SR->changeGainBar(value);					break;
		case 6: 	SR->changeMonitorSelection(value);			break;
		}
	}
	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		switch(SR->selectedPlace)
		{
		case 0: 	break;
		case 1: 	SR->modStartPoint(value);					break;
		case 2: 	SR->modEndPoint(value); 					break;
		case 3: 	SR->changeZoom(value);						break;
		case 5: 	break;
		case 6: 	break;
		}
	}



	return 1;
}





static  uint8_t functLeft()
{
	if(SR->recordInProgressFlag) return 1;

	if(SR->selectedPlace > 0) SR->selectedPlace--;

	if(SR->currentScreen == cSampleRecorder::screenTypeConfig)
	{
		switch(SR->selectedPlace)
		{
			case 0:
				functSelectButton0();
				break;
			case 1:
				functSelectButton1();
				break;
			case 2:
				functSelectButton2();
				break;
			case 3:
				functSelectButton3();
				break;
			case 4:
				if(SR->recorderConfig.source == cSampleRecorder::sourceTypeRadio)
				{
					SR->selectedPlace = 3;
					functSelectButton3();
				}
				else
				{
					SR->selectedPlace = 0;
					functSelectButton0();
				}
				break;
			case 5:
				functSelectButton5();
				break;
			case 6:
				functSelectButton6();
				break;
		}
	}
	else if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		switch(SR->selectedPlace)
		{
			case 0:
				functSelectButton0();
				break;
			case 1:
				functSelectButton1();
				break;
			case 2:
				functSelectButton2();
				break;
			case 3:
				functSelectButton3();
				break;
			case 4:
				functSelectButton4();
				break;
			case 5:
				functSelectButton5();
				break;
			case 6:
				functSelectButton6();
				break;
		}
	}



	return 1;
}


static  uint8_t functRight()
{
	if(SR->recordInProgressFlag) return 1;

	if(SR->selectedPlace < SR->frameData.placesCount-1) SR->selectedPlace++;

	if(SR->currentScreen == cSampleRecorder::screenTypeConfig )
	{
		switch(SR->selectedPlace)
		{
			case 1:
				if(SR->recorderConfig.source == cSampleRecorder::sourceTypeRadio)
				{
					functSelectButton1();
				}
				else
				{
					SR->selectedPlace = 5;
					functSelectButton5();
				}
				break;
			case 2:
				functSelectButton2();
				break;
			case 3:
				functSelectButton3();
				break;
			case 4:
				SR->selectedPlace = 5;
				functSelectButton5();

				break;
			case 5:
				functSelectButton5();
				break;
			case 6:
				functSelectButton6();
				break;
			case 7:
				functSelectButton7();
				break;

		}
	}
	else if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{

		switch(SR->selectedPlace)
		{
			case 1:
				functSelectButton1();
				break;
			case 2:
				functSelectButton2();
				break;
			case 3:
				functSelectButton3();
				break;
			case 4:
				functSelectButton4();
				break;
			case 5:
				functSelectButton5();
				break;
			case 6:
				functSelectButton6();
				break;
			case 7:
				functSelectButton7();
				break;
		}
	}




	return 1;
}


static  uint8_t functUp()
{
	switch(SR->selectedPlace)
	{
	case 0:  		SR->changeSourceSelection(-1);			break;
	case 1:			SR->changeRadioFreqBar(1);				break;
	case 2: 		break;
	case 3:			break;
	case 5: 		SR->changeGainBar(1);					break;
	case 6:			SR->changeMonitorSelection(-1);			break;
	}

	return 1;
}


static  uint8_t functDown()
{
	switch(SR->selectedPlace)
	{
	case 0: 		SR->changeSourceSelection(1);			break;
	case 1: 		SR->changeRadioFreqBar(-1);				break;
	case 2: 			break;
	case 3: 			break;
	case 5: 		SR->changeGainBar(-1);					break;
	case 6:			SR->changeMonitorSelection(1);			break;
	}

	return 1;
}




static  uint8_t functPlayAction(uint8_t state)
{
	if(state == 1)
	{
		if(sequencer.getSeqState() == 0)
		{
			sequencer.play();
		}
		else if(sequencer.getSeqState() == 1)
		{
			sequencer.stop();
		}
		SR->setPrevievFlag(0);
		recorder.play(0, MAX_16BIT);
	}
	else if(state == 0)
	{
		recorder.stop();
		SR->setPrevievFlag(1);
	}
	return 1;
}

void cSampleRecorder::setPrevievFlag(uint8_t s)
{
	previevFlag = s;
}

static  uint8_t functRecAction()
{
	recorder.startRecording(sdram_effectsBank);
	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{
	SR->eventFunct(eventSwitchModule,SR,&button,0);
	SR->setPrevievFlag(0);
	return 1;
}


//======================================================================================================================
void cSampleRecorder::calcRadioFreqBarVal()
{
	radioFreqBarVal = ((recorderConfig.radioFreq - 87.0) * 100)/ (108 - 87);
}
void cSampleRecorder::calcLevelBarVal()
{
	if(rms.available())
	{
		measureSum += rms.read();
		levelBarMeasureCounter++;
	}
	if(levelBarMeasureCounter == 10)
	{
		levelBarMeasureCounter = 0;

		float localMeasureSum = (measureSum/10)/0.85;
		if(localMeasureSum < 0.001f) localMeasureSum = 0.001f;
		uint8_t localLevelToConvert = (((log10(localMeasureSum) + 3.0) * 100.0)/ 3.0);
		uint8_t localLevel = map(localLevelToConvert,0,117,0,100);

		if(((levelBarTim > 500 )) && (levelBarVal != 0 ))
		{
			levelBarTim = 0;
			levelBarVal--;
		}
		if(localLevel > levelBarVal) levelBarVal = localLevel;

		measureSum = 0;
	}


}
void cSampleRecorder::calcGainBarVal()
{
	gainBarVal = recorderConfig.gain;
}


void cSampleRecorder::changeRadioFreqBar(int16_t val)
{
	if(val > 0)
	{
		if(recorderConfig.radioFreq < 108.0f) recorderConfig.radioFreq += 0.1;
	}
	else if(val < 0)
	{
		if(recorderConfig.radioFreq > 87.0f) recorderConfig.radioFreq -= 0.1;
	}

	calcRadioFreqBarVal();
	drawRadioFreqBar();
	//todo: tu fizyczna zmiana czestotliwosci
}
void cSampleRecorder::changeLevelBar()
{
	calcLevelBarVal();
	if(lastLevelBarVal != levelBarVal) 	drawLevelBar();
	lastLevelBarVal = levelBarVal;
}
void cSampleRecorder::changeGainBar(int16_t val)
{
	if(val > 0)
	{
		if(recorderConfig.gain < 100)
		{
			recorderConfig.gain++;
			if(recorderConfig.source == sourceTypeLineIn)
			{
				uint8_t localMap = recorderConfig.gain *15/100;
				audioShield.lineInLevel(localMap);
			}
			else if((recorderConfig.source == sourceTypeMic) || (recorderConfig.source == sourceTypeRadio))
			{
				float localMap = recorderConfig.gain* 63.0 / 100.0;
				audioShield.micGain(localMap);
			}
		}

	}
	else if(val < 0)
	{
		if(recorderConfig.gain > 0)
		{
			recorderConfig.gain--;
			if(recorderConfig.source == sourceTypeLineIn)
			{
				uint8_t localMap = recorderConfig.gain *15/100;
				audioShield.lineInLevel(localMap);
			}
			else if((recorderConfig.source == sourceTypeMic) || (recorderConfig.source == sourceTypeRadio))
			{
				float localMap = recorderConfig.gain* 63.0 / 100.0;
				audioShield.micGain(localMap);
			}
		}
	}
	calcGainBarVal();
	drawGainBar();
	//tu fizyczna zmiana wzmocnienia
}

void cSampleRecorder::changeZoom(int16_t value)
{

	float fVal = value * ZOOM_FACTOR;

	if(zoomValue + fVal < ZOOM_MIN) zoomValue  = ZOOM_MIN;
	else if(zoomValue + fVal > ZOOM_MAX ) zoomValue  = ZOOM_MAX;
	else zoomValue += fVal;

	refreshSpectrum = 1;
	refreshPoints = 1;

	showZoomValue();

}

void cSampleRecorder::changeSourceSelection(int16_t value)
{
	if(value > 0)
	{
		if(recorderConfig.source == 1) showRadio();
		if(recorderConfig.source < 2) recorderConfig.source++;
	}
	else if (value < 0)
	{
		if(recorderConfig.source == 2) hideRadio();
		if(recorderConfig.source > 0) recorderConfig.source--;
	}

	if((recorderConfig.source == sourceTypeLineIn) || (recorderConfig.source == sourceTypeRadio))
	{
		audioShield.inputSelect(AUDIO_INPUT_LINEIN);
		mtConfig.audioCodecConfig.inSelect = inputSelectLineIn;
		audioShield.lineInLevel(map(recorderConfig.gain,0,100,0,15));
	}
	else if(recorderConfig.source == sourceTypeMic)
	{
		audioShield.inputSelect(AUDIO_INPUT_MIC);
		mtConfig.audioCodecConfig.inSelect = inputSelectMic;
		audioShield.micGain(recorderConfig.gain*63.0/100);
	}
	display.setControlValue(sourceListControl, recorderConfig.source);
	display.refreshControl(sourceListControl);

}

void cSampleRecorder::changeMonitorSelection(int16_t value)
{
	if(value > 0)
	{
		if(recorderConfig.monitor < 1) recorderConfig.monitor++;
	}
	else if (value < 0)
	{
		if(recorderConfig.monitor > 0) recorderConfig.monitor--;
	}

	if(recorderConfig.monitor) audioShield.headphoneSourceSelect(0);
	else audioShield.headphoneSourceSelect(1);

	display.setControlValue(monitorListControl, recorderConfig.monitor);
	display.refreshControl(monitorListControl);

}

void cSampleRecorder::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = zoomWidth / 480;
	value = value * move_step;

	if(startPoint + value < SAMPLE_POINT_POS_MIN) startPoint  = 0;
	else if(startPoint + value > SAMPLE_POINT_POS_MAX ) startPoint  = SAMPLE_POINT_POS_MAX;
	else startPoint += value;

	if(startPoint > endPoint) startPoint = endPoint-1;



	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(zoomValue > 1 && lastChangedPoint != 1 && (startPoint < zoomStart || startPoint > zoomEnd)) refreshSpectrum = 1;

	lastChangedPoint = 1;
	refreshPoints = 1;

	showStartPointValue();
}

void cSampleRecorder::modEndPoint(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	value = value * move_step;

	if(endPoint + value < SAMPLE_POINT_POS_MIN) endPoint  = 0;
	else if(endPoint + value > SAMPLE_POINT_POS_MAX ) endPoint  = SAMPLE_POINT_POS_MAX;
	else endPoint += value;

	if(endPoint < startPoint) endPoint = startPoint+1;


	if(zoomValue > 1 && lastChangedPoint != 2 && (endPoint < zoomStart || endPoint > zoomEnd)) refreshSpectrum = 1;

	lastChangedPoint = 2;
	refreshPoints = 1;

	showEndPointValue();
}




static uint8_t functShift(uint8_t value)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	recorder.stopRecording();

	return 1;
}
/*
static uint8_t stopPlaying(uint8_t value)
{
	if(SR->isPlayingSample) instrumentPlayer[0].noteOff();

	SR->isPlayingSample = 0;

	return 1;
}
*/
