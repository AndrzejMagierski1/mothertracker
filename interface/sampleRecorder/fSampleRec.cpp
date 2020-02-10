

#include "sampleRecorder/sampleRecorder.h"
#include "mtRecorder.h"
#include "Si4703.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtLED.h"
#include "mtFileManager.h"
#include "mtPadsBacklight.h"
#include "core/graphicProcessing.h"
#include "sdCardDetect.h"
#include "mtRandomNameGenerator.h"



constexpr uint16_t POP_TIME = 200; // czas po jakim nie ma pykniecia przy zmianie z lineIn na mic

constexpr uint32_t PLAY_REFRESH_US = 5000;

extern AudioControlSGTL5000 audioShield;

cSampleRecorder sampleRecorder;
static cSampleRecorder* SR = &sampleRecorder;

extern strMtProject mtProject;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

//static uint8_t functEnter();
static uint8_t functDeleteBackspace(uint8_t state);

static  uint8_t functSelectButton0();
static  uint8_t functSelectButton1(uint8_t state);
static  uint8_t functSelectButton2(uint8_t state);
static  uint8_t functSelectButton3();
static  uint8_t functSelectButton4();
static  uint8_t functSelectButton5();
static  uint8_t functSelectButton6();
static  uint8_t functSelectButton7();


static  uint8_t functActionButton0(uint8_t s);
static  uint8_t functActionButton1(uint8_t state);
static  uint8_t functActionButton2(uint8_t state);
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
static  uint8_t functActionStopRec();
static  uint8_t functActionSave();
static  uint8_t functActionAutoName();
static  uint8_t functActionConfirmSaveLoad();
static  uint8_t functActionConfirmSave();
static  uint8_t functActionEndPoint();
static  uint8_t functActionStartPoint();
static  uint8_t functActionZoom();
static uint8_t functConfirmKey();
static uint8_t functInsert();

static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);

static uint8_t functStepNote(uint8_t value);

static void modStartPoint(int16_t value);
static void modEndPoint(int16_t value);

static uint8_t functSdCard(uint8_t state);

void seek_callback(void);




void cSampleRecorder::update()
{
	if(fullMemoryDuringRecordFlag)
	{
		fullMemoryDuringRecordFlag = 0;
		functActionStopRec();

		fullMemoryWindowFlag = 1;
		showSelectionWindowFullMemory();
	}

	if(refreshSpectrum)
	{
		//processSpectrum();
		params.address = recorder.getStartAddress();
		params.length = recorder.getLength();
		params.recordInProgressFlag = recordInProgressFlag;
		GP.processSpectrum(&params, &zoom, &spectrum);


		display.refreshControl(spectrumControl);
		if(recordInProgressFlag) spectrumTimerConstrains+=50;
		refreshSpectrum = 0;
	}
	if(refreshSpectrumValue)
	{
		display.setControlValue(progressCursor,playProgressInSpectrum);
		display.refreshControl(progressCursor);
		refreshSpectrumValue = 0;
	}

	if(refreshPoints)
	{
		processPoints();
//
		display.refreshControl(SR->pointsControl);

		refreshPoints = 0;
	}

	if(recordInProgressFlag)
	{
		if(firstPeakFlag)
		{
			if(levelBarVal > 20)
			{
				firstPeakFlag = 0;
				firstPeakPlace = recorder.getLength();
			}
		}
		showRecTimeValue();
		if(refreshSpectrumTimer > spectrumTimerConstrains)
		{
			refreshSpectrumTimer = 0;
			refreshSpectrum = 1;
		}
	}


	if(radio.update_RDS())
	{
		if(recorderConfig.source==sourceTypeRadio && currentScreen == screenTypeConfig)
		{
			refreshRDS();
		}
	}
	radio.stateMachineSeek();

	if(saveInProgressFlag)
	{
//		recorder.updateSave();
		showSaveHorizontalBar();
		if(recorder.getSaveState() == 0)
		{
			saveInProgressFlag = 0;

			hideSaveHorizontalBar();
			currentScreen = screenTypeConfig;
			showDefaultScreen();
			if(SR->recorderConfig.monitor) audioShield.headphoneSourceSelect(0);
			else audioShield.headphoneSourceSelect(1);
		}
	}
	if(notEnoughInstrumentsFlag)
	{
		notEnoughInstrumentsFlag = 0;
		showSelectionNotEnoughInstruments();
	}
	if(notEnoughMemoryFlag)
	{
		notEnoughMemoryFlag = 0;
		showSelectionNotEnoughMemory();
	}



	if(changeSourceToMicFlag == 1)
	{
		if(changeSourceToMicTimer > POP_TIME)
		{
			changeSourceToMicFlag = 0;
			audioShield.unmuteHeadphone();
		}
	}

	if(playInProgressFlag)
	{
		calcPlayProgressValue();
		if(playInProgressFlag)	showPreviewValue(); // w calcPlayProgress jest mozliwosc wyzerowania tej flagi wtedy nie chcemy wyswietlac wartosci;
		else
		{
			hidePreviewValue();
			mtPadBoard.clearVoice(0);
		}
	}
	if(!notePopoutFlag) changeLevelBar();


}

void cSampleRecorder::start(uint32_t options)
{
	moduleRefresh = 1;
	dontTurnOffRadio = 0;
//--------------------------------------------------------------------
	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
	   sequencer.stop();
	}

	FM->setPadsGlobal(functPads);

	keyboardManager.init(keyboardControl, editName);

	params.length = recorder.getLength();
	params.address = recorder.getStartAddress();
	params.recordInProgressFlag = recordInProgressFlag;
	GP.processSpectrum(&params, &zoom, &spectrum);

	processPoints();
	listMonitor();
	listSource();

	showMonitorList();
	showSourceList();
	refreshConfigLists();
	refreshGain();
	//showZoomValue();

	points.selected = 0;
	clearAllNodes();
	cancelMultiFrame();

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
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

	radio.setSeekCallback(seek_callback);

	if(radio.hasRegionChanged())
	{
		recorderConfig.radioFreq = radio.getRadioBottomBase();
		radioFreqBarVal = 0;
	}
	radio.setFrequency(recorderConfig.radioFreq);

	if((recorderConfig.source == sourceTypeRadio) && (currentScreen == screenTypeConfig))
	{
		showRadio();
	}
	if(currentScreen == screenType::screenTypeConfig)
	{
		if(recorderConfig.monitor) audioShield.headphoneSourceSelect(0);
		else audioShield.headphoneSourceSelect(1);
	}
	else audioShield.headphoneSourceSelect(0);

	engine.setHeadphonesVolume(mtProject.values.volume * 0.85);

	mtPadBoard.configureInstrumentPlayer(8);
}


void cSampleRecorder::stop()
{
	if(!dontTurnOffRadio)
	{
		audioShield.headphoneSourceSelect(0);
		radio.clearRDS();
		radio.resetSeekCallback();
		engine.setHeadphonesVolume(mtProject.values.volume);

	}

	moduleRefresh = 0;
	keyboardManager.deinit();
	//hideRDS();

}


void cSampleRecorder::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();



	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

//	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
//	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);


	FM->setButtonObj(interfaceButton0, functActionButton0);
	FM->setButtonObj(interfaceButton1, functActionButton1);
	FM->setButtonObj(interfaceButton2, functActionButton2);
	FM->setButtonObj(interfaceButton3, buttonPress, functActionButton3);
	FM->setButtonObj(interfaceButton4, buttonPress, functActionButton4);
	FM->setButtonObj(interfaceButton5, buttonPress, functActionButton5);
	FM->setButtonObj(interfaceButton6, buttonPress, functActionButton6);
	FM->setButtonObj(interfaceButton7, buttonPress, functActionButton7);
	FM->setButtonObj(interfaceButtonInsert, buttonPress, functInsert);

	FM->setButtonObj(interfaceButtonNote, functStepNote);

	FM->setButtonObj(interfaceButtonDelete, functDeleteBackspace);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setSdDetection(functSdCard);

}

//==============================================================================================================
void cSampleRecorder::refreshConfigLists()
{
	if(currentScreen != screenTypeConfig) return;
	if(recorderConfig.monitor) audioShield.headphoneSourceSelect(0);
	else audioShield.headphoneSourceSelect(1);

	if(recorderConfig.source == sourceTypeLineIn )
	{
		audioShield.inputSelect(0);
		digitalWrite(SI4703_KLUCZ,HIGH);
	}
	else if((recorderConfig.source == sourceTypeMicLG) || (recorderConfig.source == sourceTypeMicHG))
	{
		audioShield.muteHeadphone();
		audioShield.inputSelect(1);
		changeSourceToMicTimer = 0;
		changeSourceToMicFlag = 1;

	}
	else if(recorderConfig.source == sourceTypeRadio)
	{
		audioShield.inputSelect(0);
		digitalWrite(SI4703_KLUCZ,LOW);
	}
}

void cSampleRecorder::refreshGain()
{
	if((recorderConfig.source == sourceTypeLineIn) || (recorderConfig.source == sourceTypeRadio))
	{
		uint8_t localMap = 0;
		if(recorderConfig.source == sourceTypeLineIn) localMap = recorderConfig.gainLineIn *15/100;
		else if(recorderConfig.source == sourceTypeRadio) localMap = recorderConfig.gainRadio *15/100;
		audioShield.lineInLevel(localMap);
	}
	else if((recorderConfig.source == sourceTypeMicLG) || (recorderConfig.source == sourceTypeMicHG))
	{
		float localMap = 0.0f;
		if((recorderConfig.source == sourceTypeMicLG))  localMap= recorderConfig.gainMicLow* 25.0 / 100.0;
		else if(recorderConfig.source == sourceTypeMicHG) localMap= recorderConfig.gainMicHigh* 44.0 / 100.0;
		audioShield.micGain(localMap);
	}
}

void cSampleRecorder::processPoints()
{

	points.pointsType = 0;

	if(startPoint >= zoom.zoomStart && startPoint <= zoom.zoomEnd)
	{
		points.startPoint = ((startPoint-zoom.zoomStart) * 599) / zoom.zoomWidth;
	}
	else points.startPoint = -1;

	if(endPoint >= zoom.zoomStart && endPoint <= zoom.zoomEnd)
	{
		points.endPoint = ((endPoint-zoom.zoomStart) * 599) / zoom.zoomWidth;
	}
	else points.endPoint = -1;

}

/*
void cSampleRecorder::processSpectrum1()
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
			low = up = 0;

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
			low = up = 0;

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
		low = up = 0; // *(sampleData+step);

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

}*/

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
	if(SR->selectionWindowFlag == 1) return 1;
	SR->selectedPlace = 0;
	SR->activateLabelsBorder();
	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}

static  uint8_t functSelectButton1(uint8_t state)
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
			(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		if(state == buttonPress || state == UINT8_MAX)
		{
			if(SR->zoom.zoomValue > 1.0 && SR->zoom.lastChangedPoint != 1)
			{
				SR->refreshSpectrum = 1;
				SR->zoom.lastChangedPoint = 1;
				SR->zoom.zoomPosition = SR->startPoint;
			}

			if(state == buttonPress)
			{
				SR->frameData.multisel[1].frameNum = 1;
				SR->frameData.multisel[1].isActive = 1;
				SR->frameData.multiSelActiveNum  += 1;
				SR->addNode(modStartPoint, 0);
			}

			if(SR->frameData.multiSelActiveNum < 2)
			{
				SR->points.selected = 0;
			}

			SR->points.selected |= selectStart;

			SR->selectedPlace = 1;
		}
		else if(state == buttonRelease)
		{
			if(SR->frameData.multiSelActiveNum)
			{
				SR->points.selected &= ~selectStart;

				if(SR->frameData.multisel[1].isActive)
				{
					SR->removeNode(0);
					SR->frameData.multiSelActiveNum  -= 1;

					SR->frameData.multisel[1].isActive = 0;

					if(SR->frameData.multiSelActiveNum == 0)
					{
						SR->selectedPlace = 1;
						SR->points.selected |= selectStart;
					}
				}
			}
		}

		SR->refreshPoints = 1;
	}
	else
	{
		if(state == buttonPress)
		{
			SR->selectedPlace = 1;
		}
	}

	SR->activateLabelsBorder();

	return 1;
}
static  uint8_t functSelectButton2(uint8_t state)
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		if(state == buttonPress || state == UINT8_MAX)
		{
			if(SR->zoom.zoomValue > 1.0 && SR->zoom.lastChangedPoint != 2 )
			{
				SR->refreshSpectrum = 1;
				SR->zoom.lastChangedPoint = 2;
				SR->zoom.zoomPosition = SR->endPoint;
			}

			if(state == buttonPress)
			{
				SR->frameData.multisel[2].frameNum = 2;
				SR->frameData.multisel[2].isActive = 1;
				SR->frameData.multiSelActiveNum  += 1;
				SR->addNode(modEndPoint, 1);
			}

			if(SR->frameData.multiSelActiveNum < 2)
			{
				SR->points.selected = 0;
			}

			SR->points.selected |= selectEnd;

			SR->selectedPlace = 2;
		}
		else if(state == buttonRelease)
		{
			if(SR->frameData.multiSelActiveNum)
			{
				SR->points.selected &= ~selectEnd;

				if(SR->frameData.multisel[2].isActive)
				{
					SR->removeNode(1);
					SR->frameData.multiSelActiveNum  -= 1;

					SR->frameData.multisel[2].isActive = 0;

					if(SR->frameData.multiSelActiveNum == 0)
					{
						SR->selectedPlace = 2;
						SR->points.selected |= selectEnd;
					}
				}
			}
		}

		SR->refreshPoints = 1;
	}
	else
	{
		if(state == buttonPress)
		{
			SR->selectedPlace = 2;
		}
	}

	SR->activateLabelsBorder();

	return 1;
}
static  uint8_t functSelectButton3()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if ((SR->currentScreen == cSampleRecorder::screenTypeConfig) &&
	(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio)) return 1;
	SR->selectedPlace = 3;
	SR->activateLabelsBorder();

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->clearAllNodes();
		SR->cancelMultiFrame();
		SR->points.selected = 0;
		SR->refreshPoints = 1;
	}
	return 1;
}
static  uint8_t functSelectButton4()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
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
	if(SR->selectionWindowFlag == 1) return 1;
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
	if(SR->selectionWindowFlag == 1) return 1;
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
	if(SR->selectionWindowFlag == 1) return 1;
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
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowFlag) return 1;

	if(SR->currentScreen == cSampleRecorder::screenTypeKeyboard)
	{
		functConfirmKey();
		return 1;
	}

	if(SR->selectionWindowSaveFlag == 1)
	{
		SR->selectionWindowSaveFlag = 0;
		if(SR->saveOrSaveloadFlag == cSampleRecorder::saveTypeNormal)
		{
			recorder.startSave(SR->keyboardManager.getName(),1);
		}
		else if(SR->saveOrSaveloadFlag == cSampleRecorder::saveTypeLoad)
		{

			if((mtProject.used_memory + recorder.getLength() * 2) > mtProject.max_memory)
			{
				SR->notEnoughMemoryFlag = 1;
				return 0;
			}
			uint8_t firstFree = -1;
			for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++ )
			{
				if(!mtProject.instrument[i].isActive)
				{
					firstFree = i;
					break;
				}
			}
			if(firstFree == -1)
			{
				SR->notEnoughInstrumentsFlag = 1;
				return 0;
			}

			recorder.startSaveLoad(SR->keyboardManager.getName(), firstFree, 1 );
		}

		SR->saveInProgressFlag = 1;
		SR->keyboardManager.deactivateKeyboard();
		return 1;
	}

	if(s == 1)
	{
		if(SR->currentScreen == cSampleRecorder::screenTypeConfig) functSelectButton0();
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
//==============================================================================================================


static  uint8_t functActionButton1(uint8_t state)
{
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->selectionWindowSaveFlag == 1) return 1;


	if(state == buttonPress || state == buttonRelease)
	{
		if(SR->currentScreen != cSampleRecorder::screenTypeKeyboard) functSelectButton1(state);
	}

	if(state == buttonPress)
	{
		switch(SR->currentScreen)
		{
		case cSampleRecorder::screenTypeConfig: 	functActionRadioFreq();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionStartPoint();		break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
		}
	}
	return 1;
}

static  uint8_t functActionButton2(uint8_t state)
{
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->selectionWindowSaveFlag == 1) return 1;

	if(state == buttonPress || state == buttonRelease)
	{
		if(SR->currentScreen != cSampleRecorder::screenTypeKeyboard) functSelectButton2(state);
	}

	if(state == buttonPress)
	{
		switch(SR->currentScreen)
		{
		case cSampleRecorder::screenTypeConfig:		functActionRadioLeft();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionEndPoint();			break;
		case cSampleRecorder::screenTypeKeyboard: break;
		default: break;
		}
	}
	return 1;
}

static  uint8_t functActionButton3()
{
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->selectionWindowSaveFlag == 1) return 1;

	if(SR->currentScreen != cSampleRecorder::screenTypeKeyboard) functSelectButton3();
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
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->selectionWindowSaveFlag == 1) return 1;

	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: break;
		case cSampleRecorder::screenTypeRecord: 	functActionCrop();				break;
		case cSampleRecorder::screenTypeKeyboard: 	functActionAutoName();			break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton5()
{
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowSaveFlag == 1) return 1;
	if((SR->currentScreen == cSampleRecorder::screenTypeConfig) && (SR->currentScreen != cSampleRecorder::screenTypeKeyboard)) functSelectButton5();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionGain();				break;
		case cSampleRecorder::screenTypeRecord:		functActionUndo();				break;
		case cSampleRecorder::screenTypeKeyboard: 	functActionGoBack();			break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton6()
{
	if(SR->fullMemoryWindowFlag) return 1;
	if(SR->selectionWindowSaveFlag == 1) return 1;

	if(SR->selectionWindowFlag == 1)
	{
		SR->selectionWindowFlag = 0;
		//SR->selectedPlace = 6;

		SR->showDefaultScreen();
		SR->activateLabelsBorder();
		return 1;
	}

	if((SR->currentScreen != cSampleRecorder::screenTypeRecord) && (SR->currentScreen != cSampleRecorder::screenTypeKeyboard))	functSelectButton6();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionMonitor();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionGoBack();			break;
		case cSampleRecorder::screenTypeKeyboard: 	functActionConfirmSaveLoad();	break;
		default: break;
	}
	return 1;
}

static  uint8_t functActionButton7()
{
	if(SR->selectedPlace == 1)
	{
		SR->points.selected = selectStart;
	}
	else if(SR->selectedPlace == 2)
	{
		SR->points.selected = selectEnd;
	}

	if(SR->selectionWindowFlag == 1)
	{
		SR->clearAllNodes();
		SR->cancelMultiFrame();

		SR->selectionWindowFlag = 0;

		SR->currentScreen = cSampleRecorder::screenTypeConfig;
		SR->selectedPlace = 0;
		if(!SR->recorderConfig.monitor) audioShield.headphoneSourceSelect(1);

		GP.spectrumResetZoom(0, recorder.getLength(), &SR->zoom);

		SR->showDefaultScreen();
		SR->activateLabelsBorder();

		return 1;
	}

	if(SR->selectionWindowSaveFlag == 1)
	{
		char localPatch[70];
		uint16_t cnt=1;
		char localName[32];
		strcpy(localName,SR->keyboardManager.getName());
		do
		{
		   char keyboardName[MAX_NAME_LENGTH + 4];
		   sprintf(keyboardName,"%s%d",localName,cnt);

		   SR->keyboardManager.fillName(keyboardName);

		   sprintf(localPatch, "Recorded/%s.wav",SR->keyboardManager.getName());

		   cnt++;
		   if(cnt > 9999)
		   {
			   memset(keyboardName,0,33);
			   SR->keyboardManager.fillName(keyboardName);
			   break;
		   }
		} while(SD.exists(localPatch));

		SR->keyboardManager.activateKeyboard();

		SR->selectionWindowSaveFlag = 0;

		SR->showDefaultScreen();
		return 1;
	}

	if(SR->fullMemoryWindowFlag)
	{
		SR->fullMemoryWindowFlag = 0;
		SR->selectedPlace = 7;
		SR->showDefaultScreen();
		SR->activateLabelsBorder();
		return 1;
	}
	if(SR->notEnoughInstrumentsFlag)
	{
		SR->selectedPlace = 7;
		SR->showDefaultScreen();
		return 1;
	}
	if(SR->notEnoughMemoryFlag)
	{
		SR->selectedPlace = 7;
		SR->showDefaultScreen();
		return 1;
	}
	if(SR->currentScreen != cSampleRecorder::screenTypeKeyboard) functSelectButton7();
	switch(SR->currentScreen)
	{
		case cSampleRecorder::screenTypeConfig: 	functActionRecord();			break;
		case cSampleRecorder::screenTypeRecord: 	functActionStopRec();			break;
		case cSampleRecorder::screenTypeKeyboard: 	functActionConfirmSave();		break;
		default: break;
	}
	return 1;
}

//==============================================================================================================
static uint8_t functDeleteBackspace(uint8_t state)
{
	if((state == buttonPress) || (state == buttonHold))
	{
		SR->keyboardManager.makeBackspace();
	}
	return 1;
}

//==============================================================================================================
static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(SR->recordInProgressFlag == 0)
	{
		if((state == 1) || (state == 2))
		{
			SR->keyboardManager.onPadChange(pad, state);
		}

		if(SR->currentScreen == SR->screenTypeRecord)
		{
			if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
			{
				sequencer.stop();
				for(uint8_t i = 0; i < 8; i++)
				{
					instrumentPlayer[i].noteOff(Sequencer::STEP_NOTE_CUT);
				}
			}

			if(state == 1)
			{
				if(mtPadBoard.getEmptyVoice() < 0) return 1;

				if(mtPadBoard.getEmptyVoice() == 0)
				{
					SR->playPitch = (float)notes[mtPadBoard.convertPadToNote(pad)];
					SR->playProgresValueTim = (((((recorder.getLength()/44100.0 ) * SR->startPoint) / MAX_16BIT) * 1000) / SR->playPitch);
					SR->refreshPlayProgressValue = 0;
					SR->loopDirection = 0;
					SR->playInProgressFlag = 1;
				}

				padsBacklight.setFrontLayer(1,20, pad);

				uint32_t length;
				uint32_t addressShift;
				length =(uint32_t)((uint32_t)SR->endPoint * (float)(recorder.getLength())/MAX_16BIT);

				addressShift = (uint32_t)( (uint32_t)SR->startPoint * (float)(recorder.getLength())/MAX_16BIT);

				mtPadBoard.startInstrument(pad,recorder.getStartAddress()+ addressShift,length - addressShift );
			}
			else if(state == 0)
			{
				padsBacklight.setFrontLayer(0,0, pad);

				if(mtPadBoard.getVoiceTakenByPad(pad) == 0)
				{
					SR->playProgressValue=0;
					SR->playProgressInSpectrum = 0;
					SR->playInProgressFlag = 0;
					SR->refreshSpectrumProgress = 1;
					SR->refreshSpectrumValue = 1;
					SR->hidePreviewValue();
				}

				mtPadBoard.stopInstrument(pad);
			}
		}
	}

	return 1;
}


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
	SR->firstPeakFlag = 1;
	SR->currentScreen = SR->screenTypeRecord;
	SR->cropCounter = 0;
	SR->showDefaultScreen();
	recorder.startRecording(sdram_effectsBank);
	GP.spectrumResetZoom(0, 0, &SR->zoom);
	SR->spectrumTimerConstrains = 100;
	SR->params.address = sdram_effectsBank;
	return 1;
}

static  uint8_t functActionRadioFreq()
{
	return 1;
}

static  uint8_t functActionRadioLeft()
{
	if(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio) return 1;

	if(radio.getInitializationStatus())
	{
		SR->displaySeeking();
		radio.clearRDS();
		radio.seekDown();
	}

	return 1;
}

static  uint8_t functActionRadioRight()
{
	if(SR->recorderConfig.source != cSampleRecorder::sourceTypeRadio) return 1;

	if(radio.getInitializationStatus())
	{
		SR->displaySeeking();
		radio.clearRDS();
		radio.seekUp();
	}

	return 1;

}

static  uint8_t functActionPreview()
{
	if(SR->recordInProgressFlag == 1) return 1;

	if(mtPadBoard.getEmptyVoice() < 0) return 1;

	if(mtPadBoard.getEmptyVoice() == 0)
	{
		SR->playPitch=1.0;
		SR->playProgresValueTim = ((((recorder.getLength()/44100.0 ) * SR->startPoint) / MAX_16BIT) * 1000) / SR->playPitch;
		SR->refreshPlayProgressValue = 0;
	}

	uint32_t length;
	uint32_t addressShift;

	length =(uint32_t)((uint32_t)SR->endPoint * (float)(recorder.getLength())/MAX_16BIT);
	addressShift = (uint32_t)( (uint32_t)SR->startPoint * (float)(recorder.getLength())/MAX_16BIT);

	mtPadBoard.startInstrument(12,recorder.getStartAddress()+ addressShift,length - addressShift );


	SR->playInProgressFlag = 1;
	return 1;
}

static uint8_t functActionStopPreview()
{
	if(mtPadBoard.getVoiceTakenByPad(12) == 0)
	{
		SR->playProgressValue=0;
		SR->playProgressInSpectrum = 0;
		SR->playInProgressFlag = 0;
		SR->refreshSpectrumProgress = 1;
		SR->refreshSpectrumValue = 1;
		SR->hidePreviewValue();
	}

	mtPadBoard.stopInstrument(12);

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
	SR->undo[0].startPoint = SR->startPoint;
	SR->undo[0].endPoint = SR->endPoint;

	if(SR->cropCounter < undoCount) SR->cropCounter++;

	recorder.trim(SR->startPoint,SR->endPoint);
	SR->startPoint = 0;
	SR->endPoint = MAX_16BIT;

	GP.spectrumResetZoom(SR->startPoint, recorder.getLength(), &SR->zoom);

	SR->showZoomValue();
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
	SR->startPoint = SR->undo[0].startPoint;
	SR->endPoint = SR->undo[0].endPoint;
	for(uint8_t i = 0 ; i< (undoCount-1); i++)
	{
		SR->undo[i] = SR->undo[i+1];

	}


	SR->showEndPointValue();
	SR->showStartPointValue();
	SR->refreshSpectrum = 1;
	SR->refreshPoints = 1;

	return 1;
}

static  uint8_t functActionGoBack()
{
	if(SR->recordInProgressFlag == 1) return 1;
	if(SR->selectionWindowFlag == 1) return 1;

	if(SR->currentScreen == cSampleRecorder::screenTypeRecord)
	{
		SR->selectionWindowFlag = 1;
		SR->showSelectionWindow();
	}
	else if(SR->currentScreen == cSampleRecorder::screenTypeKeyboard)
	{
		SR->currentScreen = cSampleRecorder::screenTypeRecord;
		SR->keyboardManager.deactivateKeyboard();
		SR->selectedPlace = 7;
		SR->showDefaultScreen();
	}

	return 1;
}

static  uint8_t functActionSave()
{
	SR->currentScreen = cSampleRecorder::screenTypeKeyboard;

	char localPatch[70];
	uint16_t cnt=1;

	do
	{
	   char keyboardName[MAX_NAME_LENGTH];
	   sprintf(keyboardName, "recording%d",cnt);
	   SR->keyboardManager.fillName(keyboardName);
	   sprintf(localPatch,"Recorded/%s.wav",keyboardName);

	   cnt++;
	   if(cnt > 9999)
	   {
		   memset(keyboardName,0,MAX_NAME_LENGTH);
		   SR->keyboardManager.fillName(keyboardName);
		   break;
	   }
	} while(SD.exists(localPatch));

	SR->keyboardManager.activateKeyboard();

	SR->showDefaultScreen();
	return 1;
}

static  uint8_t functActionConfirmSave()
{
	SR->saveOrSaveloadFlag = cSampleRecorder::saveTypeNormal;
	if(recorder.startSave(SR->keyboardManager.getName()) == 0)
	{
		 SR->selectionWindowSaveFlag = 1;
		 SR->keyboardManager.deactivateKeyboard();
		 SR->showSelectionWindowSave();
	}
	else
	{
		 SR->saveInProgressFlag = 1;
		 SR->keyboardManager.deactivateKeyboard();
	}

	return 1;
}

static  uint8_t functActionAutoName()
{
	SR->keyboardManager.setRandomName();

	return 1;
}


static  uint8_t functActionConfirmSaveLoad()
{
	SR->saveOrSaveloadFlag = cSampleRecorder::saveTypeLoad;

	if((mtProject.used_memory + recorder.getLength() * 2) > mtProject.max_memory)
	{
		SR->notEnoughMemoryFlag = 1;
		return 0;
	}
	int8_t firstFree = -1;
	char localName[37];

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++ )
	{
		if(!mtProject.instrument[i].isActive)
		{
			firstFree = i;
			break;
		}
	}
	if(firstFree == -1)
	{
		SR->notEnoughInstrumentsFlag = 1;
		return 0;
	}
	else
	{
		 sprintf(localName,"%s.wav",SR->keyboardManager.getName());

		 if(recorder.startSaveLoad(SR->keyboardManager.getName(), firstFree) == 0)
		 {
			 SR->selectionWindowSaveFlag = 1;
			 SR->keyboardManager.deactivateKeyboard();
			 SR->showSelectionWindowSave();
			 return 0 ;
		 }
		 else
		 {
			 SR->keyboardManager.deactivateKeyboard();
			 SR->saveInProgressFlag = 1;
		 }


		SR->forceSwitchModule = 1;

		firstFree++;
		uint8_t button = interfaceButtonSampleLoad;
		SR->eventFunct(eventSwitchModule,SR,&button,&firstFree);
//		functSwitchModule(interfaceButtonSampleLoad);
	}

	return 0;
}

static  uint8_t functActionStopRec()
{

	if(SR->recordInProgressFlag == 1)
	{
		SR->recordInProgressFlag = 0;

		SR->endPoint = MAX_16BIT;
		SR->showDefaultScreen();
		recorder.stopRecording();
		if(SR->firstPeakFlag)
		{
			SR->firstPeakFlag = 0;
			SR->startPoint = 0;
		}
		else SR->startPoint = (uint16_t)(((float)(SR->firstPeakPlace) * MAX_16BIT)/recorder.getLength());

		SR->showStartPointValue();
		audioShield.headphoneSourceSelect(0);
		SR->refreshSpectrum = 1;


		return 1;
	}

	if(!sdCardDetector.isCardInserted())
	{
		return 1;
	}

	functActionSave();
	return 1;
}

static  uint8_t functActionEndPoint()
{
	if(SR->recordInProgressFlag == 1) return 1;
	//SR->points.selected = 2;
	if(SR->zoom.zoomValue > 1.0 )
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
	if(SR->selectionWindowFlag == 1) return 1;

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
		case 1:
			if(SR->frameData.multiSelActiveNum)
			{
				SR->stepThroughNodes(value);
			}
			else
			{
				modStartPoint(value);
			}
			break;
		case 2:
			if(SR->frameData.multiSelActiveNum)
			{
				SR->stepThroughNodes(value);
			}
			else
			{
				modEndPoint(value);
			}
			break;
		case 3: 	SR->changeZoom(value);						break;
		case 5: 	break;
		case 6: 	break;
		}
	}




	return 1;
}





static  uint8_t functLeft()
{
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->frameData.multiSelActiveNum != 0) return 1;

	SR->keyboardManager.makeMove('a');
	if(SR->keyboardManager.getState()) return 1;

	if(SR->recordInProgressFlag) return 1;



	if(SR->currentScreen == cSampleRecorder::screenTypeConfig)
	{
		if(SR->selectedPlace > 0) SR->selectedPlace--;

		switch(SR->selectedPlace)
		{
			case 0:
				functSelectButton0();
				break;
			case 1:
				functSelectButton1(UINT8_MAX);
				break;
			case 2:
				functSelectButton2(UINT8_MAX);
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
		if(SR->selectedPlace > 1) SR->selectedPlace--;

		switch(SR->selectedPlace)
		{
			case 0:
				functSelectButton0();
				break;
			case 1:
				functSelectButton1(UINT8_MAX);
				break;
			case 2:
				functSelectButton2(UINT8_MAX);
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
	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->frameData.multiSelActiveNum != 0) return 1;

	SR->keyboardManager.makeMove('d');
	if(SR->keyboardManager.getState()) return 1;

	if(SR->recordInProgressFlag) return 1;


	if(SR->currentScreen == cSampleRecorder::screenTypeConfig )
	{
		if(SR->selectedPlace < SR->frameData.placesCount-1) SR->selectedPlace++;

		switch(SR->selectedPlace)
		{
			case 1:
				if(SR->recorderConfig.source == cSampleRecorder::sourceTypeRadio)
				{
					functSelectButton1(UINT8_MAX);
				}
				else
				{
					SR->selectedPlace = 5;
					functSelectButton5();
				}
				break;
			case 2:
				functSelectButton2(UINT8_MAX);
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
		if(SR->selectedPlace < 3) SR->selectedPlace++;

		switch(SR->selectedPlace)
		{
			case 1:
				functSelectButton1(UINT8_MAX);
				break;
			case 2:
				functSelectButton2(UINT8_MAX);
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
	if(SR->selectionWindowFlag == 1) return 1;

	SR->keyboardManager.makeMove('w');
	if(SR->keyboardManager.getState()) return 1;
	if(	SR->currentScreen != SR->screenTypeConfig) return 1;

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
	if(SR->selectionWindowFlag == 1) return 1;

	SR->keyboardManager.makeMove('s');
	if(SR->keyboardManager.getState()) return 1;
	if(	SR->currentScreen != SR->screenTypeConfig) return 1;

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


static uint8_t functSwitchModule(uint8_t button)
{

	if(SR->selectionWindowFlag == 1) return 1;
	if(SR->recordInProgressFlag) return 1;
	if(SR->saveInProgressFlag) return 1;
	if(SR->notEnoughInstrumentsFlag) return 1;
	if(SR->notEnoughMemoryFlag) return 1;
	if(SR->currentScreen == cSampleRecorder::screenTypeKeyboard) return 1;



	if(button == interfaceButtonMaster) SR->dontTurnOffRadio = 1;



    SR->eventFunct(eventSwitchModule,SR,&button,0);

	return 1;
}


//======================================================================================================================
void cSampleRecorder::calcRadioFreqBarVal()
{
	float radioBottomFreq = radio.getRadioBottomBase();
	radioFreqBarVal = ((recorderConfig.radioFreq - radioBottomFreq) * 100) / (MAX_RADIO_FREQ_MHZ - radioBottomFreq);
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
	if(recorderConfig.source == sourceTypeLineIn) gainBarVal = recorderConfig.gainLineIn;
	else if(recorderConfig.source == sourceTypeRadio) gainBarVal = recorderConfig.gainRadio;
	else if(recorderConfig.source == sourceTypeMicLG) gainBarVal = recorderConfig.gainMicLow;
	else if(recorderConfig.source == sourceTypeMicHG) gainBarVal = recorderConfig.gainMicHigh;

}


void cSampleRecorder::changeRadioFreqBar(int16_t val)
{
	if(radio.getInitializationStatus())
	{
		float radioBottomFreq = radio.getRadioBottomBase();
		float radioSpacing = radio.getRadioSpacing();
		float previousFreq = recorderConfig.radioFreq;

		if((recorderConfig.radioFreq + (val*radioSpacing)) > MAX_RADIO_FREQ_MHZ) recorderConfig.radioFreq = MAX_RADIO_FREQ_MHZ;
		else if((recorderConfig.radioFreq + (val*radioSpacing)) < radioBottomFreq) recorderConfig.radioFreq = radioBottomFreq;
		else
		{
			recorderConfig.radioFreq+=(val*radioSpacing);
		}

		if(previousFreq != recorderConfig.radioFreq)
		{
			calcRadioFreqBarVal();
			drawRadioFreqBar();

			radio.clearRDS();
			SR->displayEmptyRDS();

			radio.setFrequency(recorderConfig.radioFreq);
			mtProject.values.radioFreq = sampleRecorder.recorderConfig.radioFreq;
			fileManager.configIsChangedFlag = 1;
			mtProject.values.projectNotSavedFlag = 1;
		}
	}
}
void cSampleRecorder::changeLevelBar()
{
	calcLevelBarVal();
	if(lastLevelBarVal != levelBarVal) drawLevelBar();

	lastLevelBarVal = levelBarVal;
}
void cSampleRecorder::changeGainBar(int16_t val)
{
	if(recorderConfig.source == sourceTypeLineIn)
	{
		if(recorderConfig.gainLineIn + val > 100) recorderConfig.gainLineIn = 100;
		else if ((int16_t) (recorderConfig.gainLineIn + val) < 0) recorderConfig.gainLineIn = 0;
		else
		{
			recorderConfig.gainLineIn += val;
		}
		refreshGain();
		mtProject.values.gainLineIn = sampleRecorder.recorderConfig.gainLineIn;
	}
	else if(recorderConfig.source == sourceTypeRadio)
	{
		if(recorderConfig.gainRadio + val > 100) recorderConfig.gainRadio = 100;
		else if ((int16_t) (recorderConfig.gainRadio + val) < 0) recorderConfig.gainRadio = 0;
		else
		{
			recorderConfig.gainRadio += val;
		}
		refreshGain();

		mtProject.values.gainRadio = sampleRecorder.recorderConfig.gainRadio;
	}
	else if(recorderConfig.source == sourceTypeMicLG)
	{
		if(recorderConfig.gainMicLow + val > 100) recorderConfig.gainMicLow = 100;
		else if ((int16_t) (recorderConfig.gainMicLow + val) < 0) recorderConfig.gainMicLow = 0;
		else
		{
			recorderConfig.gainMicLow += val;
		}
		refreshGain();

		mtProject.values.gainMicLow = sampleRecorder.recorderConfig.gainMicLow;
	}
	else if(recorderConfig.source == sourceTypeMicHG)
	{
		if(recorderConfig.gainMicHigh + val > 100) recorderConfig.gainMicHigh = 100;
		else if ((int16_t) (recorderConfig.gainMicHigh + val) < 0) recorderConfig.gainMicHigh = 0;
		else
		{
			recorderConfig.gainMicHigh += val;
		}
		refreshGain();
		mtProject.values.gainMicHigh = sampleRecorder.recorderConfig.gainMicHigh;
	}

	calcGainBarVal();
	drawGainBar();

	showGain();
	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void cSampleRecorder::changeZoom(int16_t value)
{

	GP.spectrumChangeZoom(value, recorder.getLength(), &zoom);

	refreshSpectrum = 1;
	refreshPoints = 1;

	showZoomValue();
}

void cSampleRecorder::changeSourceSelection(int16_t value)
{
    if(value > 0)
    {
        if(recorderConfig.source == 0)
        {
        	digitalWrite(SI4703_KLUCZ,LOW);
        	showRadio();
        }
        else if(recorderConfig.source == 1)
        {
        	digitalWrite(SI4703_KLUCZ,HIGH);
        	hideRadio();

        	audioShield.muteHeadphone();

        	audioShield.inputSelect(AUDIO_INPUT_MIC);
    		changeSourceToMicTimer = 0;
    		changeSourceToMicFlag = 1;

        }
        if(recorderConfig.source < 3) recorderConfig.source++;
    }
    else if (value < 0)
    {
        if(recorderConfig.source == 1)
        {
        	digitalWrite(SI4703_KLUCZ,HIGH);
        	hideRadio();
        }
        else if(recorderConfig.source == 2)
        {
        	digitalWrite(SI4703_KLUCZ,LOW);
        	showRadio();

        	audioShield.inputSelect(AUDIO_INPUT_LINEIN);
        }
        if(recorderConfig.source > 0) recorderConfig.source--;
    }
    if((recorderConfig.source == sourceTypeLineIn) || (recorderConfig.source == sourceTypeRadio))
    {
        mtConfig.audioCodecConfig.inSelect = inputSelectLineIn;
        if(recorderConfig.source == sourceTypeLineIn)  audioShield.lineInLevel(map(recorderConfig.gainLineIn,0,100,0,15));
        else if(recorderConfig.source == sourceTypeRadio) audioShield.lineInLevel(map(recorderConfig.gainRadio,0,100,0,15));
    }
    else if(recorderConfig.source == sourceTypeMicLG)
    {
        mtConfig.audioCodecConfig.inSelect = inputSelectMic;
        audioShield.micGain(recorderConfig.gainMicLow*25.0/100);
    }
    else if(recorderConfig.source == sourceTypeMicHG)
    {
        mtConfig.audioCodecConfig.inSelect = inputSelectMic;
        audioShield.micGain(recorderConfig.gainMicHigh*44.0/100);
    }
    display.setControlValue(sourceListControl, recorderConfig.source);
    display.refreshControl(sourceListControl);
    calcGainBarVal();
    drawGainBar();

    showSource();
    showGain();

    display.synchronizeRefresh();
    mtProject.values.source =  sampleRecorder.recorderConfig.source;
	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;

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

	mtProject.values.monitor = sampleRecorder.recorderConfig.monitor;
	showMonitor();

	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;
}

static void modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = SR->zoom.zoomWidth / 480;
	value = value * move_step;

	if(SR->startPoint + value < SAMPLE_POINT_POS_MIN) SR->startPoint  = 0;
	else if(SR->startPoint + value > SAMPLE_POINT_POS_MAX ) SR->startPoint  = SAMPLE_POINT_POS_MAX;
	else SR->startPoint += value;

	if(SR->startPoint >= SR->endPoint)
	{
		SR->startPoint = SR->endPoint - 2;
	}



	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(SR->zoom.zoomValue > 1 && (SR->zoom.lastChangedPoint != 1 || (SR->startPoint < SR->zoom.zoomStart || SR->startPoint > SR->zoom.zoomEnd)))SR-> refreshSpectrum = 1;

	SR->zoom.zoomPosition = SR->startPoint;
	SR->zoom.lastChangedPoint = 1;
	SR->refreshPoints = 1;

	SR->showStartPointValue();
}

static void modEndPoint(int16_t value)
{
	uint16_t move_step = SR->zoom.zoomWidth / 480;
	value = value * move_step;

	if(SR->endPoint + value < SAMPLE_POINT_POS_MIN) SR->endPoint  = 0;
	else if(SR->endPoint + value > SAMPLE_POINT_POS_MAX ) SR->endPoint  = SAMPLE_POINT_POS_MAX;
	else SR->endPoint += value;

	if(SR->endPoint <= SR->startPoint)
	{
		SR->endPoint = SR->startPoint + 2;
	}


	if(SR->zoom.zoomValue > 1 && (SR->zoom.lastChangedPoint != 2 || (SR->endPoint < SR->zoom.zoomStart ||SR-> endPoint > SR->zoom.zoomEnd))) SR->refreshSpectrum = 1;

	SR->zoom.zoomPosition = SR->endPoint;
	SR->zoom.lastChangedPoint = 2;
	SR->refreshPoints = 1;

	SR->showEndPointValue();
}

void cSampleRecorder::calcPlayProgressValue()
{
	if( refreshPlayProgressValue > PLAY_REFRESH_US)
	{
		refreshPlayProgressValue = 0;

		playProgressValue = (uint16_t)((instrumentPlayer[0].getWavePosition() * ((endPoint - startPoint) / (float)MAX_16BIT)) + startPoint);


		if(zoom.zoomValue == 1.0) playProgressInSpectrum = (600 *  playProgressValue)/MAX_16BIT;
		else if(zoom.zoomValue > 1.0)
		{
			if((int32_t)playProgressValue < zoom.zoomStart || (int32_t)playProgressValue > zoom.zoomEnd) playProgressInSpectrum = 0;
			else
			{
				playProgressInSpectrum = map(playProgressValue, zoom.zoomStart, zoom.zoomEnd, 0 , 600);
			}
		}

		refreshSpectrumValue = 1;
	}
}



/*
static uint8_t stopPlaying(uint8_t value)
{
	if(SR->isPlayingSample) instrumentPlayer[0].noteOff();

	SR->isPlayingSample = 0;

	return 1;
}
*/

static uint8_t functInsert()
{
	if(SR->selectionWindowFlag) return 1;
	if(SR->currentScreen == cSampleRecorder::screenTypeKeyboard) functConfirmKey();
	return 1;
}

static uint8_t functConfirmKey()
{
	SR->keyboardManager.confirmKey();
	return 1;
}


void seek_callback(void)
{
	SR->recorderConfig.radioFreq = radio.getFrequency();

	mtProject.values.radioFreq = sampleRecorder.recorderConfig.radioFreq;
	SR->calcRadioFreqBarVal();
	SR->drawRadioFreqBar();

	SR->displayEmptyRDS();

	fileManager.configIsChangedFlag = 1;
	mtProject.values.projectNotSavedFlag = 1;
}


static uint8_t functStepNote(uint8_t value)
{
	if(value == buttonRelease)
	{
		if(SR->currentScreen==0)
		{
			SR->setDefaultScreenFunct();
			SR->notePopoutFlag = 0;
			//SR->hideNotePopout();

			if((SR->recorderConfig.source == SR->sourceTypeRadio) && (SR->currentScreen == SR->screenTypeConfig))
			{
				SR->showRadio();
			}
		}
	}
	else if(value == buttonHold)
	{
		if(SR->currentScreen <1)
		{
			for(uint8_t i = 0; i < 48; i++)
			{
				interfaceGlobals.padNamesPointer[i] = (char*)mtNotes[mtPadBoard.getNoteFromPad(i)];
			}

			SR->FM->clearButtonsRange(interfaceButton0, interfaceButton7);
			SR->FM->clearButtonsRange(interfaceButtonUp, interfaceButtonRight);
			SR->FM->clearAllPots();
			SR->notePopoutFlag = 1;
			//SR->showNotePopout();
			SR->hideRDS();
		}
	}
	return 1;
}

/*/////////// MultiSelect Functions ////////////////*/
void cSampleRecorder::addNode(editFunct_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cSampleRecorder::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cSampleRecorder::stepThroughNodes(int16_t value)
{
	//kolejnosc wykonywania funkcji w loop pointach ma znaczenie
	//
	if(value < 0)
	{
		for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
		{
			if(selectNodes[node].isActive)
			{
				if(selectNodes[node].editFunct != NULL)
				{
					selectNodes[node].editFunct(value);
				}
			}
		}
	}
	else if(value > 0)
	{
		for(uint8_t node = MAX_SELECT_NODES; node > 0; node--)
		{
			if(selectNodes[node-1].isActive)
			{
				if(selectNodes[node-1].editFunct != NULL)
				{
					selectNodes[node-1].editFunct(value);
				}
			}
		}
	}
}

void cSampleRecorder::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}


void cSampleRecorder::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		SR->frameData.multisel[i].isActive = 0;
	}

	SR->frameData.multiSelActiveNum = 0;
}
///////////////////////////////////////////////////////////////////////////

static uint8_t functSdCard(uint8_t state)
{
	SR->showDefaultScreen();


	return 1;
}
