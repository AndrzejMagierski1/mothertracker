#include "sampleEditor/sampleEditor.h"
#include "core/graphicProcessing.h"
#include "mtSequencer.h"
#include "mtPadBoard.h"
#include "mtPadsBacklight.h"
#include "mtAudioEngine.h"
#include "fileManager.h"
#include "debugLog.h"

cSampleEditor sampleEditor;

static cSampleEditor *SE = &sampleEditor;

static uint16_t mainScreenFramePlaces[4][4] =
{
		{(800/8)*0+1, 424, 800/8-3, 55},
		{(800/8)*1+1, 424, 800/8-3, 55},
		{(800/8)*2+1, 424, 800/8-3, 55},
		{(800/8)*6+1, 31, 800/4-3, 394},
};


static uint16_t paramsScreenFramePlaces[6][4] =
{
		{(800/8)*0+1, 31, 800/8-3, 394},
		{(800/8)*1+1, 31, 800/8-3, 394},
		{(800/8)*2+1, 31, 800/8-3, 394},
		{(800/8)*3+1, 31, 800/8-3, 394},
		{(800/8)*4+1, 31, 800/8-3, 394},
		{(800/8)*5+1, 31, 800/8-3, 394},
};



//************************************************************************** ACTION FUNCTIONS DECLARATIONS
static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

//Main screen functions
static  uint8_t functMainScreenLeft();
static  uint8_t functMainScreenRight();
static  uint8_t functMainScreenUp();
static  uint8_t functMainScreenDown();
static  uint8_t functMainScreenEncoder(int16_t value);

static  uint8_t functSelectStartPoint();
static  uint8_t functSelectEndPoint();
static  uint8_t functSelectZoom();
static  uint8_t functListUp();
static 	uint8_t functListDown();
static  uint8_t functUndo();
static  uint8_t functApply();
// Paramiter screen functions
static  uint8_t functParamsScreenLeft();
static  uint8_t functParamsScreenRight();
static  uint8_t functParamsScreenUp();
static  uint8_t functParamsScreenDown();
static  uint8_t functParamsScreenEncoder(int16_t value);
static  uint8_t functSelectParamiter(uint8_t button);
static  uint8_t functPreview();
static  uint8_t functPlay();
static  uint8_t functStop();
// Stop Pattern Funct
static  uint8_t functStopPatternYes();
static  uint8_t functStopPatternNo();
// Save Sample Funct
static  uint8_t functSaveSampleYes();
static  uint8_t functSaveSampleNo();
//**************************************************************************

void cSampleEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		reloadInstrumentName();
		showPopupSeqWindow();
		setStopSeqFunctions();
		return;
	}

	screenType = mainScreen;
	previewState = previewStatePreview;
	confirmedDataIsChanged = 0;

	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
	{
		editorInstrument->isActive = 0;
		editorInstrument->sample.address = nullptr;
		editorInstrument->sample.length = 0;
	}
	else
	{
		editorInstrument->isActive = mtProject.instrument[mtProject.values.lastUsedInstrument].isActive;
		editorInstrument->sample.address = mtProject.instrument[mtProject.values.lastUsedInstrument].sample.address;
		editorInstrument->sample.length = mtProject.instrument[mtProject.values.lastUsedInstrument].sample.length;

		strcpy((char *)&editorInstrument->sample.file_name, (char *)&mtProject.instrument[mtProject.values.lastUsedInstrument].sample.file_name);
	}



	if(editorInstrument->isActive)
	{
		spectrumParams.address = editorInstrument->sample.address;
		spectrumParams.length = editorInstrument->sample.length;
	}
	else
	{
		spectrumParams.address = nullptr;
		spectrumParams.length = 0;
	}

	setMainScreenFunctions();
	setCommonFunctions();


	selection.startPoint = 0;
	selection.endPoint = MAX_16BIT;

	prepareDisplayDataMainScreen();
	showMainScreen();

	currentEffect->clearIsLoadedData();
	currentEffect->clearIsProcessedData();

	for(uint8_t i = 0; i < effectDisplayParams[currentEffectIdx].paramsNumber; i++)
	{
		if(effectDisplayParams[currentEffectIdx].paramsType[i] == 'f')
		{
			currentEffect->setParamiter(&effectDisplayParams[currentEffectIdx].fParameter[i], i);
		}
		else if(effectDisplayParams[currentEffectIdx].paramsType[i] == 'd')
		{
			currentEffect->setParamiter(&effectDisplayParams[currentEffectIdx].iParameter[i], i);
		}
	}

	currentEffect->changeSelectionRange(selection.startPoint,selection.endPoint);
}

void cSampleEditor::update()
{
	if(needRefreshSpectrum)
	{
		needRefreshSpectrum = 0;
		refreshSpectrum();
	}
	if(needRefreshPlayhead)
	{
		refreshPlayhead();
	}

	if(!currentEffect->getIsLoadedData() && currentEffect->getLoadState())
	{
		currentEffect->update();
		if(!currentEffect->getLoadState())
		{
			reloadSpectrumData();
			isLoadedData = currentEffect->getIsLoadedData();
		}
	}
	else if(!currentEffect->getIsProcessedData() && currentEffect->getProcessSelectionState() )
	{
		currentEffect->update();
		if(!currentEffect->getProcessSelectionState())
		{
			reloadSpectrumData();
			needRefreshSpectrum = 1;

			isProcessedData = currentEffect->getIsProcessedData();

			applyingInProgress = 0;

			if(processingInProgress)
			{
				processingInProgress = false;
				processingProgress = 0;
				showProgressProcessing();
				setPlayFunction();
			}

			if(applyingProgress)
			{
				resetZoom();
				applyingProgress = 0;
			}
			showProgressApplying();

			hideProgressPopup();

			SE->selection.startPoint = SE->currentEffect->getNewStartPoint();
			SE->selection.endPoint = SE->currentEffect->getNewEndPoint();

			refreshStartPoint();
			refreshEndPoint();

			refreshUndoState();

			restoreFunctions();
		}
	}

	if(applyingInProgress)
	{
		refreshApplyingProgress();
	}
	else if(processingInProgress)
	{
		refreshProcessingProgress();
	}

	if(playingInProgress)
	{
		refreshPlayingProgress();
		if(instrumentPlayer[0].getInterfacePlayingEndFlag())
		{
			instrumentPlayer[0].clearInterfacePlayingEndFlag();
			playingInProgress = 0;
			mtPadBoard.clearVoice(0);
			hideProgressPopup();
			restoreFunctions();
			setPlayFunction();
		}
	}
	else if(instrumentPlayer[0].getInterfacePlayingEndFlag())
	{
		instrumentPlayer[0].clearInterfacePlayingEndFlag();
	}





	uint8_t fileManagerStatus = newFileManager.getStatus();

	if(fileManagerStatus == fmImportSampleFromSampleEditorEnd)
	{
		FM->unblockAllInputs();
		newFileManager.clearStatus();
		SE->eventFunct(eventSwitchModule,SE,&SE->moduleToChange,0);
	}
	else if(fileManagerStatus >=  fmError)
	{
		debugLog.addLine("Opretion Error");
		FM->unblockAllInputs();
		newFileManager.clearStatus();
	}

}

void cSampleEditor::stop()
{

}

void cSampleEditor::setCommonFunctions()
{
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

	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX) return;
	if(!editorInstrument->isActive) return;
	FM->setPadsGlobal(functPads);

	//todo: popup instrument
	//todo: popup note
}
void cSampleEditor::setMainScreenFunctions()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton7);
	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);
	FM->clearAllPots();

	if(!editorInstrument->isActive) return;
	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX) return;

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functMainScreenLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functMainScreenRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functMainScreenUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functMainScreenDown);

	FM->setButtonObj(interfaceButton0, buttonPress, functSelectStartPoint);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectEndPoint);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectZoom);
	FM->setButtonObj(interfaceButton3, buttonPress, functUndo);
	FM->setButtonObj(interfaceButton5, buttonPress, functApply);
	FM->setButtonObj(interfaceButton6, buttonPress, functListUp);
	FM->setButtonObj(interfaceButton7, buttonPress, functListDown);

	FM->setPotObj(interfacePot0, functMainScreenEncoder, nullptr);

}
void cSampleEditor::setParamsScreenFunctions()
{
	FM->clearButtonsRange(interfaceButton0, interfaceButton7);
	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);
	FM->clearAllPots();

	if(!editorInstrument->isActive) return;
	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX) return;

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functParamsScreenLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functParamsScreenRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functParamsScreenUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functParamsScreenDown);


	for(uint8_t i = interfaceButton0 ; i <= interfaceButton5 ; i++)
	{
		 if(i < effectDisplayParams[currentEffectIdx].paramsNumber)
		 {
			 FM->setButtonObj(i, buttonPress, functSelectParamiter);
		 }
		 else
		 {
			 break;
		 }
	}
	switch(previewState)
	{
	case previewStatePreview: 	FM->setButtonObj(interfaceButton6, buttonPress, functPreview); 	break;
	case previewStatePlay:		FM->setButtonObj(interfaceButton6, buttonPress, functPlay); 	break;
	case previewStateStop:		FM->setButtonObj(interfaceButton6, buttonPress, functStop); 	break;
	}

	FM->setButtonObj(interfaceButton7, buttonPress, functApply);

	FM->setPotObj(interfacePot0, functParamsScreenEncoder, nullptr);

}
void cSampleEditor::setStopSeqFunctions()
{
	clearAllFunctions();
	FM->setButtonObj(interfaceButton6, buttonPress, functStopPatternNo);
	FM->setButtonObj(interfaceButton7, buttonPress, functStopPatternYes);
}
void cSampleEditor::setSaveChangesFunctions()
{
	clearAllFunctions();
	FM->setButtonObj(interfaceButton6, buttonPress, functSaveSampleNo);
	FM->setButtonObj(interfaceButton7, buttonPress, functSaveSampleYes);
}
void cSampleEditor::switchScreen(enScreenType s)
{
	if(s == mainScreen)
	{
		prepareDisplayDataMainScreen();
		showMainScreen();
		setMainScreenFunctions();
	}
	else if(s == effectParamsScreen)
	{
		prepareDisplayDataParamsScreen();
		showEffectParamsScreen();
		setParamsScreenFunctions();
	}
}


void cSampleEditor::clearAllFunctions()
{
	FM->clearAll();
	FM->setPadsGlobal(nullptr);
	FM->clearAllPads();
}
void cSampleEditor::restoreFunctions()
{
	setCommonFunctions();
	if(screenType == mainScreen) setMainScreenFunctions();
	else if(screenType == effectParamsScreen) setParamsScreenFunctions();
}


void cSampleEditor::prepareDisplayDataMainScreen()
{
	reloadCurrentEffect();
	reloadInstrumentName();
	reloadStartPointText();
	reloadEndPointText();
	reloadZoomText();
	reloadPointsData();
	reloadFrameData(screenType);

	GP.processSpectrum( &spectrumParams, &zoom, &spectrumData);
}
void cSampleEditor::reloadInstrumentName()
{
	if(!editorInstrument->isActive)
	{
		if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX) sprintf(currentInstrumentName,"%d. MIDI Channel %d",mtProject.values.lastUsedInstrument + 3, mtProject.values.lastUsedInstrument % INSTRUMENTS_MAX );
		else sprintf(currentInstrumentName,"%d. Empty slot",mtProject.values.lastUsedInstrument + 1);
	}
	else
	{
		sprintf(currentInstrumentName,"%d. %s",mtProject.values.lastUsedInstrument + 1, editorInstrument->sample.file_name);
	}
}
void cSampleEditor::reloadStartPointText()
{
	float length = editorInstrument->sample.length/44100.0;
	float localStartPoint = (length * selection.startPoint) / MAX_16BIT;

	sprintf(startPointText, "%.3fs", localStartPoint);
}
void cSampleEditor::reloadEndPointText()
{
	float length = editorInstrument->sample.length/44100.0;
	float localEndPoint = (length * selection.endPoint) / MAX_16BIT;

	sprintf(endPointText, "%.3fs", localEndPoint);
}
void cSampleEditor::reloadZoomText()
{
	sprintf(zoomText, "%.2f", zoom.zoomValue);
}
void cSampleEditor::reloadPlayheadValue()
{
	constexpr uint16_t PLAYHEAD_REFRESH_CONSTRAIN_US = 3000;

	if( refreshPlayheadTimer > PLAYHEAD_REFRESH_CONSTRAIN_US)
	{
		refreshPlayheadTimer = 0;

		uint16_t playheadValue_16BIT = (uint16_t)((instrumentPlayer[0].getWavePosition() * ((selection.endPoint - selection.startPoint) / (float)MAX_16BIT)) + selection.startPoint);

		if(zoom.zoomValue == 1.0) playheadValue = (600 *  playheadValue_16BIT)/MAX_16BIT;
		else if(zoom.zoomValue > 1.0)
		{
			if((int32_t)playheadValue_16BIT < zoom.zoomStart || (int32_t)playheadValue_16BIT > zoom.zoomEnd) playheadValue = 0;
			else
			{
				playheadValue = map(playheadValue_16BIT, zoom.zoomStart, zoom.zoomEnd, 0 , 600);
			}
		}
		needShowPlayhead = 1;
	}
}

void cSampleEditor::reloadCurrentEffect()
{
	currentEffect = sampleEditorEffect[currentEffectIdx];

	if((currentEffect != lastEffect) && (lastEffect != nullptr))
	{
		currentEffect->switchEffect(lastEffect);
	}

	lastEffect = currentEffect;

}

void cSampleEditor::reloadSpectrumData()
{
	editorInstrument->sample.address = currentEffect->getAddresToPlay();
	editorInstrument->sample.length = currentEffect->getLengthToPlay();
	spectrumParams.address = editorInstrument->sample.address;
	spectrumParams.length = editorInstrument->sample.length;
}

//popups
void cSampleEditor::reloadApplyingProgress()
{
	if(applyingSteps == 1)
	{
		applyingProgress = currentEffect->getProcessSelectionProgress();
		Serial.printf("ProcessingProgress: %d\n",applyingProgress);
	}
	else if(applyingSteps == 2)
	{
		if(!isLoadedData)
		{
			applyingProgress = currentEffect->getLoadProgress()/2;
			Serial.printf("LoadingProgress: %d\n",applyingProgress);
		}
		else
		{
			applyingProgress = 50 + currentEffect->getProcessSelectionProgress()/2;
			Serial.printf("ProcessingProgress: %d\n",applyingProgress);
		}
	}
}

void cSampleEditor::reloadProcessingProgress()
{
	if(processingSteps == 1)
	{
		processingProgress = currentEffect->getProcessSelectionProgress();
	}
	else if(processingSteps == 2)
	{
		if(!isLoadedData)
		{
			processingProgress = currentEffect->getLoadProgress()/2;
		}
		else
		{
			processingProgress = 50 + currentEffect->getProcessSelectionProgress()/2;
		}
	}
}

void cSampleEditor::reloadPlayingProgress()
{
	playingProgress = (instrumentPlayer[0].getWavePosition() * 100) / MAX_16BIT;
}

void cSampleEditor::reloadFrameData(enScreenType s)
{
	if(s == mainScreen)
	{
		frameData.placesCount = 4;

		for ( uint8_t i = 0; i < frameData.placesCount; i++ )
		{
			frameData.places[i] = &mainScreenFramePlaces[i][0];
		}
	}
	else if(s == effectParamsScreen)
	{
		frameData.placesCount = effectDisplayParams[currentEffectIdx].paramsNumber;

		for ( uint8_t i = 0; i < frameData.placesCount; i++ )
		{
			frameData.places[i] = &paramsScreenFramePlaces[i][0];
		}
	}
}
void cSampleEditor::reloadPointsData()
{
	if(selectedPlace[cSampleEditor::mainScreen] == cSampleEditor::startPointPlace)
	{
		spectrumPointsData.selected = 1;
		zoom.zoomPosition = selection.startPoint;

		bool isZoomActive = (zoom.zoomValue > 1) ;
		bool isPointDisplayed = (selection.startPoint < zoom.zoomStart || selection.startPoint > zoom.zoomEnd);


		if(isZoomActive && isPointDisplayed)
		{
			needRefreshSpectrum = 1;
		}
	}
	else if(selectedPlace[cSampleEditor::mainScreen] == cSampleEditor::endPointPlace)
	{
		spectrumPointsData.selected = 8;
		zoom.zoomPosition = selection.endPoint;


		bool isZoomActive = (zoom.zoomValue > 1) ;
		bool isPointDisplayed = (selection.endPoint < zoom.zoomStart || selection.endPoint > zoom.zoomEnd);
		if(isZoomActive && isPointDisplayed)
		{
			needRefreshSpectrum = 1;
		}
	}
	else
	{
		spectrumPointsData.selected = 0;
	}

	if(needRefreshSpectrum)
	{
		zoom.zoomStart =  zoom.zoomPosition - zoom.zoomWidth/2;
		zoom.zoomEnd = zoom.zoomPosition + zoom.zoomWidth/2;
		if(zoom.zoomStart < 0)
		{
			zoom.zoomEnd = zoom.zoomWidth;
			zoom.zoomStart = 0;
		}
		else if(zoom.zoomEnd > MAX_16BIT)
		{
			zoom.zoomEnd = MAX_16BIT;
			zoom.zoomStart = MAX_16BIT-zoom.zoomWidth;
		}
	}


	if(selection.startPoint >= zoom.zoomStart && selection.startPoint <= zoom.zoomEnd)
	{
		spectrumPointsData.startPoint = ((selection.startPoint-zoom.zoomStart) * (spectrumData.width - 1)) / zoom.zoomWidth;
	}
	else spectrumPointsData.startPoint = -1;

	if(selection.endPoint >= zoom.zoomStart && selection.endPoint <= zoom.zoomEnd)
	{
		spectrumPointsData.endPoint = ((selection.endPoint-zoom.zoomStart) * (spectrumData.width - 1)) / zoom.zoomWidth;
	}
	else spectrumPointsData.endPoint = -1;

}

//params screen

void cSampleEditor::prepareDisplayDataParamsScreen()
{
	for(uint8_t i = 0; i < effectDisplayParams[currentEffectIdx].paramsNumber; i++)
	{
		reloadParamiterValueText(i);
		reloadParamiterBarValue(i);
		reloadFrameData(effectParamsScreen);
	}
}
void cSampleEditor::reloadParamiterValueText(uint8_t n)
{
	if(n >= effectDisplayParams[currentEffectIdx].paramsNumber) return;

	if(effectDisplayParams[currentEffectIdx].paramsType[n] == 'd')
	{
		int displayedValue = effectDisplayParams[currentEffectIdx].iParameter[n] * effectDisplayParams[currentEffectIdx].displayMult[n];
		sprintf(paramiterValueLabelPtr[n], "%d", displayedValue);
		if(effectDisplayParams[currentEffectIdx].afterValueText[n] != nullptr)
		{
			strcat(paramiterValueLabelPtr[n],effectDisplayParams[currentEffectIdx].afterValueText[n]);
		}
	}
	else if(effectDisplayParams[currentEffectIdx].paramsType[n] == 'f')
	{
		float displayedValue = effectDisplayParams[currentEffectIdx].fParameter[n] * effectDisplayParams[currentEffectIdx].displayMult[n];
		sprintf(paramiterValueLabelPtr[n], "%.3f", displayedValue);
		if(effectDisplayParams[currentEffectIdx].afterValueText[n] != nullptr)
		{
			strcat(paramiterValueLabelPtr[n],effectDisplayParams[currentEffectIdx].afterValueText[n]);
		}
	}
}

void cSampleEditor::reloadParamiterBarValue(uint8_t n)
{
	if(n >= effectDisplayParams[currentEffectIdx].paramsNumber) return;

	if(effectDisplayParams[currentEffectIdx].paramsType[n] == 'd')
	{
		paramsBarValue[n] = map(effectDisplayParams[currentEffectIdx].iParameter[n],effectDisplayParams[currentEffectIdx].iDownConstrain[n],effectDisplayParams[currentEffectIdx].iUpConstrain[n], 0, 100);
	}
	else if(effectDisplayParams[currentEffectIdx].paramsType[n] == 'f')
	{
		paramsBarValue[n] = (((effectDisplayParams[currentEffectIdx].fParameter[n] - effectDisplayParams[currentEffectIdx].fDownConstrain[n]) * 100)
		/(effectDisplayParams[currentEffectIdx].fUpConstrain[n] - effectDisplayParams[currentEffectIdx].fDownConstrain[n]) );
	}
}

//******************* REFRESH - RELOAD + DISPLAY
void cSampleEditor::refreshSpectrumPoints()
{
	if(screenType != mainScreen) return;
	reloadPointsData();
	showSpectrumPoints();
}

void cSampleEditor::refreshStartPoint()
{
	if(screenType != mainScreen) return;
	refreshSpectrumPoints();

	reloadStartPointText();
	showStartPointText();
}
void cSampleEditor::refreshEndPoint()
{
	if(screenType != mainScreen) return;
	refreshSpectrumPoints();

	reloadEndPointText();
	showEndPointText();
}
void cSampleEditor::refreshZoom()
{
	reloadZoomText();
	showZoomText();
}
void cSampleEditor::refreshEffectList()
{
	display.setControlValue(effectList, currentEffectIdx);
	showEffectList();
}

void cSampleEditor::refreshSpectrum()
{
	if(screenType != mainScreen) return;

	GP.processSpectrum( &spectrumParams, &zoom, &spectrumData);
	showSpectrum();
}

void cSampleEditor::refreshPlayhead()
{
	if(screenType != mainScreen) return;

	reloadPlayheadValue();
	if(needShowPlayhead)
	{
		needShowPlayhead = 0;
		display.setControlValue(playhead, playheadValue);
		showPlayhead();
	}
}

void cSampleEditor::refreshUndoState()
{
	if((editorInstrument->isActive) && (screenType == mainScreen))
	{
		if(!currentEffect->undo.isEnable) display.setControlColors(label[3],interfaceGlobals.inactiveLabelsColors);
		else display.setControlColors(label[3],interfaceGlobals.activeLabelsColors);

		display.refreshControl(label[3]);
	}
}

void cSampleEditor::refreshParamiter(uint8_t n)
{
	reloadParamiterBarValue(n);
	reloadParamiterValueText(n);

	showParamiterBar(n);
	showParamiterLabel(n);
}

//popups
void cSampleEditor::refreshApplyingProgress()
{
	reloadApplyingProgress();
	showProgressApplying();
}

void cSampleEditor::refreshProcessingProgress()
{
	reloadProcessingProgress();
	showProgressProcessing();
}

void cSampleEditor::refreshPlayingProgress()
{
	reloadPlayingProgress();
	showProgressPlaying();
}
//*******************

//******************* PARAMETERS MODIFICATORS
//MainScreen
void cSampleEditor::modStartPoint(int16_t val)
{
	uint16_t move_step = zoom.zoomWidth / 480;
	val *= move_step;

	if(selection.startPoint + val > MAX_16BIT) selection.startPoint = MAX_16BIT;
	else if(selection.startPoint + val < 0) selection.startPoint = 0;
	else selection.startPoint += val;

	if(selection.startPoint > selection.endPoint)
	{
		selection.startPoint = selection.endPoint - 1;
	}

	bool isZoomActive = (zoom.zoomValue > 1) ;
	bool isNewPointChanged = zoom.lastChangedPoint != 1;
	bool isPointDisplayed = (selection.startPoint < zoom.zoomStart || selection.startPoint > zoom.zoomEnd);

	if(isZoomActive && (isNewPointChanged || isPointDisplayed))
	{
		needRefreshSpectrum = 1;
	}

	zoom.zoomPosition = selection.startPoint;

	currentEffect->changeSelectionRange(selection.startPoint,selection.endPoint);

	refreshStartPoint();
}
void cSampleEditor::modEndPoint(int16_t val)
{
	uint16_t move_step = zoom.zoomWidth / 480;
	val *= move_step;

	if(selection.endPoint + val > MAX_16BIT) selection.endPoint = MAX_16BIT;
	else if(selection.endPoint + val < 0) selection.endPoint = 0;
	else selection.endPoint += val;

	if(selection.startPoint > selection.endPoint)
	{
		selection.endPoint = selection.startPoint + 1;
	}

	bool isZoomActive = (zoom.zoomValue > 1) ;
	bool isNewPointChanged = zoom.lastChangedPoint != 2;
	bool isPointDisplayed = (selection.endPoint < zoom.zoomStart || selection.endPoint > zoom.zoomEnd);

	if(isZoomActive && (isNewPointChanged || isPointDisplayed))
	{
		needRefreshSpectrum = 1;
	}

	zoom.zoomPosition = selection.endPoint;

	currentEffect->changeSelectionRange(selection.startPoint,selection.endPoint);

	refreshEndPoint();
}

void cSampleEditor::modZoom(int16_t val)
{
	GP.spectrumChangeZoom(val, editorInstrument->sample.length, &zoom);

	refreshZoom();
	refreshSpectrumPoints();
	needRefreshSpectrum = 1;
}

void cSampleEditor::resetZoom()
{
	if(zoom.zoomValue == 1.0f) return;

	GP.spectrumResetZoom(zoom.zoomPosition, editorInstrument->sample.length, &zoom);

	refreshZoom();
	refreshSpectrumPoints();
	needRefreshSpectrum = 1;
}

void cSampleEditor::modSelectedEffect(int16_t val)
{
	uint8_t lastEffectIdx = currentEffectIdx;

	if(currentEffectIdx + val > (editorEffectMax - 1 )) currentEffectIdx = editorEffectMax - 1;
	else if(currentEffectIdx + val < 0) currentEffectIdx = 0;
	else currentEffectIdx += val;

	if(currentEffectIdx != lastEffectIdx)
	{
		currentEffect = sampleEditorEffect[currentEffectIdx];

		for(uint8_t i = 0; i < effectDisplayParams[currentEffectIdx].paramsNumber; i++)
		{
			if(effectDisplayParams[currentEffectIdx].paramsType[i] == 'f')
			{
				currentEffect->setParamiter(&effectDisplayParams[currentEffectIdx].fParameter[i], i);
			}
			else if(effectDisplayParams[currentEffectIdx].paramsType[i] == 'd')
			{
				currentEffect->setParamiter(&effectDisplayParams[currentEffectIdx].iParameter[i], i);
			}
		}
		reloadCurrentEffect();
		refreshEffectList();
		setPreviewFunction();
		currentEffect->clearIsProcessedData();
	}
}
//*******************
//ParamiterScreen
void cSampleEditor::modParamiter(int16_t val, uint8_t n)
{
	if(effectDisplayParams[currentEffectIdx].paramsType[n] == 'f')
	{
		float localParam = effectDisplayParams[currentEffectIdx].fParameter[n];
		float localVal = (val * effectDisplayParams[currentEffectIdx].changeStep[n]);

		if(localParam + localVal > effectDisplayParams[currentEffectIdx].fUpConstrain[n] )
		{
			effectDisplayParams[currentEffectIdx].fParameter[n] = effectDisplayParams[currentEffectIdx].fUpConstrain[n];
		}
		else if(localParam + localVal < effectDisplayParams[currentEffectIdx].fDownConstrain[n])
		{
			effectDisplayParams[currentEffectIdx].fParameter[n] = effectDisplayParams[currentEffectIdx].fDownConstrain[n];
		}
		else
		{
			effectDisplayParams[currentEffectIdx].fParameter[n] += localVal;
		}
		currentEffect->setParamiter(&effectDisplayParams[currentEffectIdx].fParameter[n], n);
	}
	else if(effectDisplayParams[currentEffectIdx].paramsType[n] == 'd')
	{
		int localParam = effectDisplayParams[currentEffectIdx].iParameter[n];
		int localVal = (int)(val * effectDisplayParams[currentEffectIdx].changeStep[n]);

		if(localParam + localVal > effectDisplayParams[currentEffectIdx].iUpConstrain[n] )
		{
			effectDisplayParams[currentEffectIdx].iParameter[n] = effectDisplayParams[currentEffectIdx].iUpConstrain[n];
		}
		else if(localParam + localVal < effectDisplayParams[currentEffectIdx].iDownConstrain[n])
		{
			effectDisplayParams[currentEffectIdx].iParameter[n] = effectDisplayParams[currentEffectIdx].iDownConstrain[n];
		}
		else
		{
			effectDisplayParams[currentEffectIdx].iParameter[n] += localVal;
		}
		currentEffect->setParamiter(&effectDisplayParams[currentEffectIdx].iParameter[n], n);
	}

	setPreviewFunction();
	currentEffect->clearIsProcessedData();
	refreshParamiter(n);
}


void cSampleEditor::setPreviewFunction()
{
	if(previewState == previewStatePreview) return;

	FM->clearButton(interfaceButton6);
	FM->setButtonObj(interfaceButton6, buttonPress, functPreview);

	previewState = previewStatePreview;

	showPreviewLabel();
}
void cSampleEditor::setPlayFunction()
{
	if(previewState == previewStatePlay) return;

	FM->clearButton(interfaceButton6);
	FM->setButtonObj(interfaceButton6, buttonPress, functPlay);

	previewState = previewStatePlay;

	showPlayLabel();
}
void cSampleEditor::setStopFunction()
{
	if(previewState == previewStateStop) return;

	FM->clearButton(interfaceButton6);
	FM->setButtonObj(interfaceButton6, buttonPress, functStop);

	previewState = previewStateStop;

	showStopLabel();
}

//************************************************************************** ACTION FUNCTIONS
static uint8_t functSwitchModule(uint8_t button)
{

	if(button == interfaceButtonSampleEdit)
	{
		if(SE->currentEffectIdx < 2) return 1;

		if(SE->screenType == cSampleEditor::mainScreen) SE->screenType = cSampleEditor::effectParamsScreen;
		else if(SE->screenType == cSampleEditor::effectParamsScreen) SE->screenType = cSampleEditor::mainScreen;

		SE->switchScreen(SE->screenType);
	}
	else
	{
		if(SE->confirmedDataIsChanged)
		{
			SE->reloadInstrumentName();
			SE->showPopupSaveChangesWindow();
			SE->setSaveChangesFunctions();
			SE->moduleToChange = button;
		}
		else SE->eventFunct(eventSwitchModule,SE,&button,0);
	}
	return 1;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == 1)
	{
		if(mtPadBoard.getEmptyVoice() == 0)
		{
			SE->needRefreshPlayhead = 1;
			SE->playheadValue = 0;
		}

		uint32_t length;
		uint32_t addressShift;
		length =(uint32_t)((uint32_t)SE->selection.endPoint * (float)(SE->editorInstrument->sample.length)/MAX_16BIT);

		addressShift = (uint32_t)( (uint32_t)SE->selection.startPoint * (float)(SE->editorInstrument->sample.length)/MAX_16BIT);

		padsBacklight.setFrontLayer(1,20, pad);
		mtPadBoard.startInstrument(pad,SE->editorInstrument->sample.address + addressShift,length - addressShift);
	}
	else if(state == 0)
	{
		if(mtPadBoard.getVoiceTakenByPad(pad) == 0)
		{
			SE->needRefreshPlayhead = 0;
			SE->playheadValue = 0;
			SE->hidePlayhead();
		}

		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);
	}
	return 1;
}

static  uint8_t functMainScreenLeft()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] > 0) SE->selectedPlace[cSampleEditor::mainScreen]--;

	SE->refreshSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functMainScreenRight()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] < (SE->frameData.placesCount - 1 )) SE->selectedPlace[cSampleEditor::mainScreen]++;

	SE->refreshSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functMainScreenUp()
{
	switch(SE->selectedPlace[cSampleEditor::mainScreen])
	{
		case cSampleEditor::startPointPlace : 	SE->modStartPoint(1);			break;
		case cSampleEditor::endPointPlace : 	SE->modEndPoint(1);				break;
		case cSampleEditor::zoomPlace : 		SE->modZoom(1);					break;
		case cSampleEditor::effectPlaces :		SE->modSelectedEffect(-1);   	break;
		default: break;
	}
	return 1;
}
static  uint8_t functMainScreenDown()
{
	switch(SE->selectedPlace[cSampleEditor::mainScreen])
	{
		case cSampleEditor::startPointPlace : 	SE->modStartPoint(-1);			break;
		case cSampleEditor::endPointPlace : 	SE->modEndPoint(-1);			break;
		case cSampleEditor::zoomPlace : 		SE->modZoom(-1);				break;
		case cSampleEditor::effectPlaces :		SE->modSelectedEffect(1); 		break;
		default: break;
	}
	return 1;
}
static  uint8_t functMainScreenEncoder(int16_t value)
{
	switch(SE->selectedPlace[cSampleEditor::mainScreen])
	{
		case cSampleEditor::startPointPlace : 	SE->modStartPoint(value);		break;
		case cSampleEditor::endPointPlace : 	SE->modEndPoint(value);			break;
		case cSampleEditor::zoomPlace : 		SE->modZoom(value);				break;
		case cSampleEditor::effectPlaces :		SE->modSelectedEffect(value);   break;
		default: break;
	}

	return 1;
}

static  uint8_t functSelectStartPoint()
{
	SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::startPointPlace;

	SE->refreshSpectrumPoints();

	SE->showFrame();

	return 1;
}
static  uint8_t functSelectEndPoint()
{
	SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::endPointPlace;

	SE->refreshSpectrumPoints();

	SE->showSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functSelectZoom()
{
	SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::zoomPlace;

	SE->refreshSpectrumPoints();

	SE->showFrame();
	return 1;
}
static  uint8_t functListUp()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] != cSampleEditor::effectPlaces)
	{
		SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::effectPlaces;

		SE->refreshSpectrumPoints();

		SE->showFrame();
	}
	else
	{
		SE->modSelectedEffect(-1);
	}

	return 1;
}
static 	uint8_t functListDown()
{
	if(SE->selectedPlace[cSampleEditor::mainScreen] != cSampleEditor::effectPlaces)
	{
		SE->selectedPlace[cSampleEditor::mainScreen] = cSampleEditor::effectPlaces;

		SE->refreshSpectrumPoints();

		SE->showFrame();
	}
	else
	{
		SE->modSelectedEffect(1);
	}

	return 1;
}
static  uint8_t functUndo()
{
	SE->currentEffect->startUndo();
	SE->refreshUndoState();

	SE->reloadSpectrumData();

	SE->selection.startPoint = SE->currentEffect->getNewStartPoint();
	SE->selection.endPoint = SE->currentEffect->getNewEndPoint();

	SE->refreshStartPoint();
	SE->refreshEndPoint();
	SE->needRefreshSpectrum = 1;
	return 1;
}
static  uint8_t functApply()
{

	SE->applyingSteps = 0;
	SE->confirmedDataIsChanged = 1;

	SE->isLoadedData = SE->currentEffect->getIsLoadedData();
	SE->isProcessedData = SE->currentEffect->getIsProcessedData();

	if(!SE->isLoadedData) SE->applyingSteps++;
	if(!SE->isProcessedData) SE->applyingSteps++;

	if(SE->applyingSteps)
	{
		SE->showPopupApplying();
		SE->applyingInProgress = 1;
		SE->applyingProgress = 0;
		SE->showProgressApplying();
		SE->clearAllFunctions();
	}

	SE->applyingProgress = 0;
	SE->currentEffect->startApply();

	if(!SE->applyingSteps)
	{
		SE->selection.startPoint = SE->currentEffect->getNewStartPoint();
		SE->selection.endPoint = SE->currentEffect->getNewEndPoint();
		SE->reloadSpectrumData();
		SE->refreshStartPoint();
		SE->refreshEndPoint();
		SE->needRefreshSpectrum = 1;

		SE->resetZoom();
	}

	SE->setPreviewFunction();
	SE->currentEffect->clearIsProcessedData();
	SE->refreshUndoState();
	return 1;
}
// Paramiter screen functions
static  uint8_t functParamsScreenLeft()
{
	if(SE->selectedPlace[cSampleEditor::effectParamsScreen] > 0) SE->selectedPlace[cSampleEditor::effectParamsScreen]--;
	SE->showFrame();
	return 1;
}
static  uint8_t functParamsScreenRight()
{
	if(SE->selectedPlace[cSampleEditor::effectParamsScreen] < (SE->frameData.placesCount - 1 )) SE->selectedPlace[cSampleEditor::effectParamsScreen]++;
	SE->showFrame();
	return 1;
}
static  uint8_t functParamsScreenUp()
{
	SE->modParamiter(1, SE->selectedPlace[cSampleEditor::effectParamsScreen]);
	return 1;
}
static  uint8_t functParamsScreenDown()
{
	SE->modParamiter(-1, SE->selectedPlace[cSampleEditor::effectParamsScreen]);
	return 1;
}
static  uint8_t functParamsScreenEncoder(int16_t value)
{
	SE->modParamiter(value, SE->selectedPlace[cSampleEditor::effectParamsScreen]);
	return 1;
}
static  uint8_t functSelectParamiter(uint8_t button)
{
	SE->selectedPlace[cSampleEditor::effectParamsScreen] = button;
	SE->showFrame();
	return 1;
}
static  uint8_t functPreview()
{
	SE->processingSteps = 1;

	SE->isLoadedData = SE->currentEffect->getIsLoadedData();
	SE->isProcessedData = SE->currentEffect->getIsProcessedData();

	if(!SE->isLoadedData) SE->processingSteps++;

	if(SE->processingSteps)
	{
		SE->showPopupProcessing();
		SE->processingInProgress = 1;
		SE->clearAllFunctions();
	}

	SE->processingProgress = 0;
	SE->showProgressProcessing();
	SE->currentEffect->startProcessingSelection();

	return 1;
}

static  uint8_t functPlay()
{
	mtPadBoard.cutAllInstrument();
	mtPadBoard.startInstrument(12, SE->currentEffect->getAddresToPreview(), SE->currentEffect->getLengthToPreview());
	SE->playingInProgress = 1;
	SE->playingProgress = 0;
	SE->showPopupPlaying();

	SE->clearAllFunctions();
	SE->setStopFunction();
	return 1;
}
static  uint8_t functStop()
{
	mtPadBoard.stopInstrument(12);
	SE->playingInProgress = 0;
	SE->hideProgressPopup();

	SE->restoreFunctions();
	SE->setPlayFunction();
	return 1;
}
// Stop Pattern Funct
static  uint8_t functStopPatternYes()
{
	sequencer.stop();
	SE->start(0);
	return 1;
}
static  uint8_t functStopPatternNo()
{
	SE->eventFunct(eventSwitchToPreviousModule,SE,0,0);
	return 1;
}
// Save Sample Funct
static  uint8_t functSaveSampleYes()
{
	if(newFileManager.importSampleFromSampleEditor(SE->currentEffect->getAddresToPlay(),
											  SE->currentEffect->getLengthToPlay(),
											  mtProject.values.lastUsedInstrument))
	{
		SE->FM->blockAllInputs();
	}
	return 1;
}
static  uint8_t functSaveSampleNo()
{
	SE->eventFunct(eventSwitchModule,SE,&SE->moduleToChange,0);
	return 1;
}
