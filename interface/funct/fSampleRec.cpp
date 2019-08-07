

#include "sampleRecorder.h"
#include "mtRecorder.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "SI4703.h"




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


static  uint8_t functActionButton0();
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

#ifdef HW_WITH_RADIO
void seek_callback(void);
#endif


void cSampleRecorder::update()
{
	if(refreshSpectrum)
	{
		processSpectrum();

		display.refreshControl(SR->spectrumControl);

		refreshSpectrum = 0;
	}

	if(refreshPoints)
	{
		processPoints();


		display.refreshControl(SR->pointsControl);

		refreshPoints = 0;
	}

	if(previevFlag != lastPrevievFlag)
	{
		if(previevFlag) audioShield.headphoneSourceSelect(1);
		else audioShield.headphoneSourceSelect(0);
	}
	lastPrevievFlag = previevFlag;

	changeLevelBar();

#ifdef HW_WITH_RADIO
	if(radio.update_RDS())
	{
		if(recorderConfig.source==2)
		{
			refreshRDS();
		}
	}
	radio.stateMachineSeek();
#endif
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

#ifdef HW_WITH_RADIO
	radio.setSeekCallback(seek_callback);
#endif

}


void cSampleRecorder::stop()
{

	moduleRefresh = 0;
#ifdef HW_WITH_RADIO
	radio.resetSeekCallback();
#endif

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


	FM->setButtonObj(interfaceButton0, buttonPress, functActionButton0);
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

	points.pointsType = editorInstrument->playMode;

	if(editorInstrument->startPoint >= zoomStart && editorInstrument->startPoint <= zoomEnd)
	{
		points.startPoint = ((editorInstrument->startPoint-zoomStart) * 599) / zoomWidth;
	}
	else points.startPoint = -1;

	if(editorInstrument->endPoint >= zoomStart && editorInstrument->endPoint <= zoomEnd)
	{
		points.endPoint = ((editorInstrument->endPoint-zoomStart) * 599) / zoomWidth;
	}
	else points.endPoint = -1;

	if(editorInstrument->loopPoint1 >= zoomStart && editorInstrument->loopPoint1 <= zoomEnd)
	{
		points.loopPoint1 = ((editorInstrument->loopPoint1-zoomStart) * 599) / zoomWidth;
	}
	else points.loopPoint1 = -1;

	if(editorInstrument->loopPoint2 >= zoomStart && editorInstrument->loopPoint2 <= zoomEnd)
	{
		points.loopPoint2 = ((editorInstrument->loopPoint2-zoomStart) * 599) / zoomWidth;
	}
	else points.loopPoint2 = -1;

}


void cSampleRecorder::processSpectrum()
{

	//refreshSpectrum = 1;


	// uwaga tu wazna kolejnosc + do sprawdzenia
	if(mtProject.values.lastUsedInstrument < 0 || mtProject.instrument[mtProject.values.lastUsedInstrument].isActive == 0 )
	{
		for(uint16_t i = 0; i < 600; i++)
		{
			spectrum.upperData[i] = 0;
			spectrum.lowerData[i] = 0;
		}

		return;
	}

	uint16_t offset_pixel;
	int16_t * sampleData;

	if(editorInstrument->sample.type == mtSampleTypeWavetable)
	{
		zoomWidth = MAX_16BIT;
		zoomStart = 0;
		zoomValue = 1;
		zoomEnd = MAX_16BIT;
		uint16_t windowSize = editorInstrument->sample.wavetable_window_size;

		sampleData = editorInstrument->sample.address
				+ (mtProject.instrument[mtProject.values.lastUsedInstrument].wavetableCurrentWindow * windowSize);

		float resolution = windowSize / 600.0;

		int16_t up = 0;
		int16_t low = 0;
		float step = 0;

		for(uint16_t i = 0; i < 600; i++)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData+(uint32_t)step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum.upperData[i] =  up;
			spectrum.lowerData[i] = low;
		}

		//if(resolution <= 1)
		spectrum.spectrumType = 1;
		//else spectrum.spectrumType = 0;


		return;
	}

	uint32_t resolution;


	switch(lastChangedPoint)
	{
		case 0: zoomPosition = editorInstrument->startPoint; break; //MAX_16BIT/2; break;

		case 1:
			zoomPosition = editorInstrument->startPoint;
		break;
		case 2:
			zoomPosition = editorInstrument->endPoint;
		break;
		case 3:
			zoomPosition = editorInstrument->loopPoint1;
		break;
		case 4:
			zoomPosition = editorInstrument->loopPoint2;
		break;

		default: zoomPosition = editorInstrument->startPoint; break; //MAX_16BIT/2; break;
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


		uint32_t offset = ((float)zoomPosition/MAX_16BIT) * editorInstrument->sample.length;

		sampleData = editorInstrument->sample.address + offset;

		resolution = (((float)zoomWidth/MAX_16BIT) * editorInstrument->sample.length ) / 600;


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
		sampleData = editorInstrument->sample.address;
		resolution = editorInstrument->sample.length / 600;
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
	SR->selectedPlace = 0;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton1()
{
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 1;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton2()
{
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 2;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton3()
{
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 3;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton4()
{
	if(SR->currentScreen == cSampleRecorder::screenTypeConfig) return 1;
	SR->selectedPlace = 4;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton5()
{
	SR->selectedPlace = 5;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton6()
{
	SR->selectedPlace = 6;
	SR->activateLabelsBorder();
	return 1;
}
static  uint8_t functSelectButton7()
{
	SR->selectedPlace = 7;
	SR->activateLabelsBorder();
	return 1;
}


static  uint8_t functActionButton0()
{
	functSelectButton0();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig:		functActionSource(); 			break;
		case cSampleRecorder::screenTypeRecord: 	functActionPreview();			break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
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

	Serial.print(SR->currentScreen);
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
	return 1;
}

static  uint8_t functActionRadioFreq()
{
	return 1;
}

static  uint8_t functActionRadioLeft()
{
#ifdef HW_WITH_RADIO
	SR->displaySeeking();
	radio.clearRDS();
	radio.seekDown();
#endif

	return 1;
}


static  uint8_t functActionRadioRight()
{
#ifdef HW_WITH_RADIO
	SR->displaySeeking();
	radio.clearRDS();
	radio.seekUp();
#endif

	return 1;
}

static  uint8_t functActionPreview()
{
	return 1;
}

static  uint8_t functActionCrop()
{
	return 1;
}

static  uint8_t functActionUndo()
{
	return 1;
}

static  uint8_t functActionGoBack()
{
	return 1;
}

static  uint8_t functActionSave()
{
	return 1;
}

static  uint8_t functActionEndPoint()
{
	return 1;
}

static  uint8_t functActionStartPoint()
{

	return 1;
}

static  uint8_t functActionZoom()
{



	return 1;
}




static  uint8_t functEncoder(int16_t value)
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




	return 1;
}





static  uint8_t functLeft()
{
	if(SR->selectedPlace > 0) SR->selectedPlace--;

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


	return 1;
}


static  uint8_t functRight()
{
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
	radioFreqBarVal = ((recorderConfig.radioFreq - 87.5) * 100)/ (108 - 87);
}
void cSampleRecorder::calcLevelBarVal()
{
	if(rms.available())
	{
		levelBarVal = 100.0 * rms.read();
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
		if(recorderConfig.radioFreq < 108.0f) recorderConfig.radioFreq += 0.05;
	}
	else if(val < 0)
	{
		if(recorderConfig.radioFreq > 87.5f) recorderConfig.radioFreq -= 0.05;
	}

	calcRadioFreqBarVal();
	drawRadioFreqBar();

#ifdef HW_WITH_RADIO
	radio.clearRDS();
	SR->displayEmptyRDS();

	radio.setFrequency(recorderConfig.radioFreq);
#endif
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
		if(recorderConfig.source == 1)
		{
			showRadio();
			digitalWrite(SI4703_KLUCZ,LOW);

		}
		if(recorderConfig.source < 2) recorderConfig.source++;
	}
	else if (value < 0)
	{
		if(recorderConfig.source == 2)
		{
			hideRadio();
			digitalWrite(SI4703_KLUCZ,HIGH);
		}
		if(recorderConfig.source > 0) recorderConfig.source--;
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


	display.setControlValue(monitorListControl, recorderConfig.monitor);
	display.refreshControl(monitorListControl);

}

void cSampleRecorder::modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = zoomWidth / 480;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument->startPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument->startPoint  = 0;
	else if(editorInstrument->startPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument->startPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->startPoint += value;

	if(editorInstrument->startPoint > editorInstrument->endPoint) editorInstrument->startPoint = editorInstrument->endPoint-1;

	if(editorInstrument->playMode != singleShot)
	{
		if(editorInstrument->startPoint > editorInstrument->loopPoint1)
		{
			dif = editorInstrument->loopPoint2 - editorInstrument->loopPoint1;
			editorInstrument->loopPoint1 = editorInstrument->startPoint;

			if(editorInstrument->loopPoint1 + dif > editorInstrument->endPoint)
			{
				editorInstrument->loopPoint2 = editorInstrument->endPoint;
				editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
				editorInstrument->startPoint = editorInstrument->loopPoint1;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
			else
			{
				editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
		}
	}

	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(zoomValue > 1 && lastChangedPoint != 1
		&& (editorInstrument->startPoint < zoomStart || editorInstrument->startPoint > zoomEnd)) refreshSpectrum = 1;

	lastChangedPoint = 1;
	refreshPoints = 1;
}

void cSampleRecorder::modEndPoint(int16_t value)
{
	uint16_t move_step = zoomWidth / 480;
	uint16_t dif;
	value = value * move_step;

	if(editorInstrument->endPoint + value < SAMPLE_POINT_POS_MIN) editorInstrument->endPoint  = 0;
	else if(editorInstrument->endPoint + value > SAMPLE_POINT_POS_MAX ) editorInstrument->endPoint  = SAMPLE_POINT_POS_MAX;
	else editorInstrument->endPoint += value;

	if(editorInstrument->endPoint < editorInstrument->startPoint) editorInstrument->endPoint = editorInstrument->startPoint+1;

	if(editorInstrument->playMode != singleShot)
	{
		if(editorInstrument->endPoint < editorInstrument->loopPoint2)
		{
			dif = editorInstrument->loopPoint2 - editorInstrument->loopPoint1;

			editorInstrument->loopPoint2 = editorInstrument->endPoint;

			if(editorInstrument->loopPoint2 - dif < editorInstrument->startPoint)
			{
				editorInstrument->loopPoint1 = editorInstrument->startPoint;
				editorInstrument->loopPoint2 = editorInstrument->loopPoint1 + dif;
				editorInstrument->endPoint = editorInstrument->loopPoint2;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
				instrumentPlayer[0].setStatusBytes(LP2_MASK);
			}
			else
			{
				editorInstrument->loopPoint1 = editorInstrument->loopPoint2 - dif;
				instrumentPlayer[0].setStatusBytes(LP1_MASK);
			}
		}
	}

	if(zoomValue > 1 && lastChangedPoint != 2
			&& (editorInstrument->endPoint < zoomStart || editorInstrument->endPoint > zoomEnd)) refreshSpectrum = 1;

	lastChangedPoint = 2;
	refreshPoints = 1;
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

#ifdef HW_WITH_RADIO
void seek_callback(void)
{
	SR->recorderConfig.radioFreq = radio.getFrequency();

	SR->calcRadioFreqBarVal();
	SR->drawRadioFreqBar();

	SR->displayEmptyRDS();

}
#endif
