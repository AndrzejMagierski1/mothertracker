

#include "sampleEditor.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "interfacePopups.h"

#include "graphicProcessing.h"
#include "mtEffector.h"



cSampleEditor sampleEditor;
static cSampleEditor* SE = &sampleEditor;

extern strMtProject mtProject;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();

static  uint8_t functStepNote(uint8_t value);
static  uint8_t functInstrument(uint8_t state);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static uint8_t functSelectPlace(uint8_t button);




static uint8_t functShift(uint8_t value);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);

static uint8_t functPreview(uint8_t state);
static uint8_t functApply();
static uint8_t functUndo();



// ------ EDIT FUNCTIONS PROTOTYPES -----
uint8_t modStartPoint(int16_t value);
uint8_t modEndPoint(int16_t value);
uint8_t changeZoom(int16_t value);

uint8_t editChorusLength(int16_t value);
uint8_t editChorusStrength(int16_t value);

uint8_t editDelayFeedback(int16_t value);
uint8_t editDelayTime(int16_t value);

uint8_t editFlangerOffset(int16_t value);
uint8_t editFlangerDepth(int16_t value);
uint8_t editFlangerDelay(int16_t value);

uint8_t editCompressorThreshold(int16_t value);
uint8_t editCompressorRatio(int16_t value);
uint8_t editCompressorAttack(int16_t value);
uint8_t editCompressorRelease(int16_t value);

uint8_t editBitcrusherBits(int16_t value);
uint8_t editBitcrusherRate(int16_t value);

uint8_t editAmplifierAmp(int16_t value);



void cSampleEditor::update()
{
	if(refreshSpectrum)
	{
		showCurrentSpectrum(effector.getLength()/2, effector.getAddress());

		refreshSpectrum = 0;
	}

	if(refreshPoints)
	{
		processPoints();

		display.refreshControl(pointsControl);

		refreshPoints = 0;
	}

	refreshSampleLoading();
	refreshSampleApplying();

	updateEffectProcessing();

	onExitRaload();
}

void cSampleEditor::onExitRaload()
{
	if(onExitFlag == 1)
	{
		showSampleLoading(fileManager.samplesLoader.getCurrentProgress());

		if(fileManager.samplesLoader.getStateFlag() == loaderStateTypeEnded)
		{
			hideHorizontalBar();
			onExitFlag = 2;
			functSwitchModule(exitButton);
		}
	}
}

void cSampleEditor::refreshSampleApplying()
{
	save_stages_t status;

	status = effector.getSaveStatus();

	if(status == saving)
	{
		showApplying(effector.saveUpdate());
	}
	else if(status == saveDone)
	{
		SE->moduleFlags &= ~applyingActive;
		hideHorizontalBar();
		refreshSpectrum = 1;
		effector.setSaveStatus(waitingForSaveInit);
		SE->isAnyEffectActive = false;
	}
}

void cSampleEditor::refreshSampleLoading()
{
	if(firstSampleLoadFlag == 0)
	{
		sampleLoadedState = fileManager.samplesLoader.waveLoader.getState();

		if(sampleLoadedState == loaderStateTypeInProgress) // refresh update when in progress
		{
			fileManager.samplesLoader.waveLoader.update();
			showSampleLoading(fileManager.samplesLoader.waveLoader.getCurrentWaveProgress());
		}

		if((sampleLoadedState == loaderStateTypeEnded) && (lastSampleLoadedState == loaderStateTypeInProgress)) // do when loaded
		{
			showCurrentSpectrum(effector.getLength()/2, effector.getAddress());
			hideHorizontalBar();
			moduleFlags &= ~sampleLoadingActive;

			firstSampleLoadFlag = 1;
		}

		lastSampleLoadedState = sampleLoadedState;
	}
}

void cSampleEditor::showCurrentSpectrum(uint32_t length, int16_t *source)
{
	spectrumParams.length = length;
	spectrumParams.address = source;
	GP.processSpectrum(&spectrumParams, &zoom, &spectrum);
	display.refreshControl(spectrumControl);
}


void cSampleEditor::start(uint32_t options)
{
	moduleRefresh = 1;
	firstSampleLoadFlag = 0;

	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	//--------------------------------------------------------------------

	//editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

	localInstrNum = mtProject.values.lastUsedInstrument;
	sprintf(instrumentPath, "Workspace/samples/instr%02d.wav", localInstrNum);
	effector.loadSample(instrumentPath);
	moduleFlags |= sampleLoadingActive;

	isAnyEffectActive = 0;

	initEffectsScreenStructs();

	//--------------------------------------------------------------------

	//points.selected = (selectedPlace >= 0 && selectedPlace <= 3) ? selectedPlace+1 : 0;
	startPoint = 0;
	endPoint = MAX_16BIT;
	points.endPoint = endPoint;
	points.startPoint = startPoint;


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

	listEffects();

	showDefaultScreen();

	setDefaultScreenFunct();

	GP.spectrumResetZoom(startPoint, effector.getLength()/2 ,&zoom);
	processPoints();

	activateLabelsBorder();

}


void cSampleEditor::initEffectsScreenStructs()
{
	//!< Crop effect
	//
	effectScreen[effectCrop].screen = fullSpectrum;

	effectScreen[effectCrop].paramNum = 3;
	effectScreen[effectCrop].barNum = 0;

	effectScreen[effectCrop].bar[1].name = "Start";
	effectScreen[effectCrop].bar[1].editFunct = modStartPoint;
	effectScreen[effectCrop].bar[1].dataSource = &startPoint;
	effectScreen[effectCrop].bar[1].dataFormat = tUNSIGNED16,

	effectScreen[effectCrop].bar[2].name = "End";
	effectScreen[effectCrop].bar[2].editFunct = modEndPoint;
	effectScreen[effectCrop].bar[2].dataSource = &endPoint;
	effectScreen[effectCrop].bar[2].dataFormat = tUNSIGNED16,

	effectScreen[effectCrop].bar[3].name = "Zoom";
	effectScreen[effectCrop].bar[3].editFunct = changeZoom;
	effectScreen[effectCrop].bar[3].dataSource = &zoom.zoomValue;
	effectScreen[effectCrop].bar[3].dataFormat = tFLOAT,
	effectScreen[effectCrop].undoActive = 1;

	//!< Reverse effect
	//
	effectScreen[effectReverse].screen = fullSpectrum;

	effectScreen[effectReverse].paramNum = 3;
	effectScreen[effectReverse].barNum = 0;

	effectScreen[effectReverse].bar[1].name = "Start";
	effectScreen[effectReverse].bar[1].editFunct = modStartPoint;
	effectScreen[effectReverse].bar[1].dataSource = &startPoint;
	effectScreen[effectReverse].bar[1].dataFormat = tUNSIGNED16,

	effectScreen[effectReverse].bar[2].name = "End";
	effectScreen[effectReverse].bar[2].editFunct = modEndPoint;
	effectScreen[effectReverse].bar[2].dataSource = &endPoint;
	effectScreen[effectReverse].bar[2].dataFormat = tUNSIGNED16,

	effectScreen[effectReverse].bar[3].name = "Zoom";
	effectScreen[effectReverse].bar[3].editFunct = changeZoom;
	effectScreen[effectReverse].bar[3].dataSource = &zoom.zoomValue;
	effectScreen[effectReverse].bar[3].dataFormat = tFLOAT,
	effectScreen[effectReverse].undoActive = 1;

	//!< Flanger effect
	//
	effectScreen[effectFlanger].screen = noSpectrum;

	effectScreen[effectFlanger].paramNum = 3;
	effectScreen[effectFlanger].barNum = 3;

	effectScreen[effectFlanger].bar[1].name = "Offset";
	effectScreen[effectFlanger].bar[1].editFunct = editFlangerOffset;
	effectScreen[effectFlanger].bar[1].dataSource = &flangerOffset;
	effectScreen[effectFlanger].bar[1].dataFormat = tUNSIGNED8,

	effectScreen[effectFlanger].bar[2].name = "Depth";
	effectScreen[effectFlanger].bar[2].editFunct = editFlangerDepth;
	effectScreen[effectFlanger].bar[2].dataSource = &flangerDepth;
	effectScreen[effectFlanger].bar[2].dataFormat = tUNSIGNED8,

	effectScreen[effectFlanger].bar[3].name = "Delay";
	effectScreen[effectFlanger].bar[3].editFunct = editFlangerDelay;
	effectScreen[effectFlanger].bar[3].dataSource = &flangerDelay;
	effectScreen[effectFlanger].bar[3].dataFormat = tFLOAT,
	effectScreen[effectFlanger].undoActive = 0;

	//!< Chorus effect
	//
	effectScreen[effectChorus].screen =  noSpectrum;

	effectScreen[effectChorus].paramNum = 2;
	effectScreen[effectChorus].barNum = 2;

	effectScreen[effectChorus].bar[2].name = "Length";
	effectScreen[effectChorus].bar[2].editFunct = editChorusLength;
	effectScreen[effectChorus].bar[2].dataSource = &chorusLength;
	effectScreen[effectChorus].bar[2].dataFormat = tUNSIGNED16;

	effectScreen[effectChorus].bar[3].name = "Strength";
	effectScreen[effectChorus].bar[3].editFunct = editChorusStrength;
	effectScreen[effectChorus].bar[3].dataSource = &chorusStrength;
	effectScreen[effectChorus].bar[3].dataFormat = tUNSIGNED8;
	effectScreen[effectChorus].undoActive = 0;

	//!< Delay effect
	//
	effectScreen[effectDelay].screen =  noSpectrum;

	effectScreen[effectDelay].paramNum = 2;
	effectScreen[effectDelay].barNum = 2;

	effectScreen[effectDelay].bar[2].name = "Time";
	effectScreen[effectDelay].bar[2].editFunct = editDelayTime;
	effectScreen[effectDelay].bar[2].dataSource = &delayTime;
	effectScreen[effectDelay].bar[2].dataFormat = tUNSIGNED16;
	effectScreen[effectDelay].bar[2].dataUnit = "ms";

	effectScreen[effectDelay].bar[3].name = "Feedback";
	effectScreen[effectDelay].bar[3].editFunct = editDelayFeedback;
	effectScreen[effectDelay].bar[3].dataSource = &delayFeedback;
	effectScreen[effectDelay].bar[3].dataFormat = tFLOAT;
	effectScreen[effectDelay].undoActive = 0;

	//!< Compressor effect
	//
	effectScreen[effectCompressor].screen =  noSpectrum;

	effectScreen[effectCompressor].paramNum = 4;
	effectScreen[effectCompressor].barNum = 4;

	effectScreen[effectCompressor].bar[0].name = "Threshold";
	effectScreen[effectCompressor].bar[0].editFunct = editCompressorThreshold;
	effectScreen[effectCompressor].bar[0].dataSource = &compressorThrs;
	effectScreen[effectCompressor].bar[0].dataFormat = tUNSIGNED16;

	effectScreen[effectCompressor].bar[1].name = "Ratio";
	effectScreen[effectCompressor].bar[1].editFunct = editCompressorRatio;
	effectScreen[effectCompressor].bar[1].dataSource = &compressorRatio;
	effectScreen[effectCompressor].bar[1].dataFormat = tUNSIGNED16;

	effectScreen[effectCompressor].bar[2].name = "Attack";
	effectScreen[effectCompressor].bar[2].editFunct = editCompressorAttack;
	effectScreen[effectCompressor].bar[2].dataSource = &compressorAttack;
	effectScreen[effectCompressor].bar[2].dataFormat = tUNSIGNED16;

	effectScreen[effectCompressor].bar[3].name = "Release";
	effectScreen[effectCompressor].bar[3].editFunct = editCompressorRelease;;
	effectScreen[effectCompressor].bar[3].dataSource = &compressorRelease;
	effectScreen[effectCompressor].bar[3].dataFormat = tUNSIGNED16;
	effectScreen[effectCompressor].undoActive = 0;

	//!< Bitcrusher effect
	//
	effectScreen[effectBitcrusher].screen =  noSpectrum;

	effectScreen[effectBitcrusher].paramNum = 2;
	effectScreen[effectBitcrusher].barNum = 2;

	effectScreen[effectBitcrusher].bar[2].name = "Bits";
	effectScreen[effectBitcrusher].bar[2].editFunct = editBitcrusherBits;
	effectScreen[effectBitcrusher].bar[2].dataSource = &bitcrusherBits;
	effectScreen[effectBitcrusher].bar[2].dataFormat = tUNSIGNED8;

	effectScreen[effectBitcrusher].bar[3].name = "Rate";
	effectScreen[effectBitcrusher].bar[3].editFunct = editBitcrusherRate;
	effectScreen[effectBitcrusher].bar[3].dataSource = &bitcrusherRate;
	effectScreen[effectBitcrusher].bar[3].dataFormat = tUNSIGNED16;

	effectScreen[effectBitcrusher].undoActive = 0;

	//!< Amplifier effect
	//
	effectScreen[effectAmplifier].screen =  noSpectrum;

	effectScreen[effectAmplifier].paramNum = 1;
	effectScreen[effectAmplifier].barNum = 1;

	effectScreen[effectAmplifier].bar[3].name = "Amp";
	effectScreen[effectAmplifier].bar[3].editFunct = editAmplifierAmp;
	effectScreen[effectAmplifier].bar[3].dataSource = &amplifierAmp;
	effectScreen[effectAmplifier].bar[3].dataFormat = tFLOAT;

	effectScreen[effectAmplifier].undoActive = 0;

}

void cSampleEditor::stop()
{
	moduleRefresh = 0;
}

void cSampleEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	//FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

//	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	//FM->setButtonObj(interfaceButtonShift, functShift);
//	FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);

	FM->setButtonObj(interfaceButtonInstr, functInstrument);

	FM->setButtonObj(interfaceButton0, functPreview);
	FM->setButtonObj(interfaceButton1, buttonPress, functApply);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectPlace);

	FM->setButtonObj(interfaceButton3, buttonPress, functSelectPlace);
	FM->setButtonObj(interfaceButton4, buttonPress, functSelectPlace);
	FM->setButtonObj(interfaceButton5, buttonPress, functSelectPlace);
	FM->setButtonObj(interfaceButton6, buttonPress, functSelectPlace);
	FM->setButtonObj(interfaceButton7, buttonPress, functSelectPlace);

	FM->setButtonObj(interfaceButtonNote, functStepNote);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}



//==============================================================================================================


void cSampleEditor::processPoints()
{
	points.pointsType = 0;

	if(startPoint >= zoom.zoomStart && startPoint <= zoom.zoomEnd)
	{
		points.startPoint = ((startPoint-zoom.zoomStart) * (spectrum.width - 1)) / zoom.zoomWidth;
	}
	else points.startPoint = -1;

	if(endPoint >= zoom.zoomStart && endPoint <= zoom.zoomEnd)
	{
		points.endPoint = ((endPoint-zoom.zoomStart) * (spectrum.width - 1)) / zoom.zoomWidth;
	}
	else points.endPoint = -1;
}

void cSampleEditor::listEffects()
{
	for(uint8_t i = 0; i < effectsCount; i++)
	{
		effectNames[i] = (char*)&effectNamesLabels[i][0];
	}
}

void cSampleEditor::cancelPopups()
{
	if(mtPopups.getStepPopupState() != stepPopupNone)
	{
		mtPopups.hideStepPopups();

		setDefaultScreenFunct();

		showDefaultScreen();
	}
}

//==============================================================================================================
uint8_t cSampleEditor::previewNewEffectRequested()
{
	uint8_t retVal = 0;

	if((SE->lastPreviewEffect != (effect_t)SE->currSelEffect))
	{
		retVal=1;
	}

	return retVal;
}

static uint8_t functPreview(uint8_t state)
{
	if(SE->moduleFlags != 0) return 1;

	if(state == 1)
	{
		SE->makeEffect();

		if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
		{
			effector.play(SE->startPoint, SE->endPoint);
		}
		else
		{
			if(SE->previewReadyFlag)
			{
				effector.playPrev();
			}
		}

		SE->lastPreviewEffect = (effect_t)SE->currSelEffect;
	}
	else if(state == 0)
	{
		effector.stop();
	}

	//SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functApply()
{
	if(SE->moduleFlags != 0) return 1;

	if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
	{
		SE->isAnyEffectActive = true;
	}

	if(SE->moduleFlags == 0 && SE->isAnyEffectActive)
	{
		sprintf(SE->instrumentPath, "Workspace/samples/instr%02d.wav", SE->localInstrNum);

		if(SE->currSelEffect == effectCrop)
		{
			effector.trim(SE->startPoint, SE->endPoint);
			SE->resetSpectrumAndPoints();
			SE->isAnyEffectActive = true;
			SE->undoCropFlag = 1;

		}
		else if(SE->currSelEffect == effectReverse)
		{
			effector.reverse(SE->startPoint, SE->endPoint);
			SE->isAnyEffectActive = true;
			SE->refreshSpectrum = 1;
			SE->undoReverseFlag = 1;
		}
		else
		{
			SE->undoReverseFlag = 0;
			SE->undoCropFlag = 0;
			effector.setEffects();
		}

		effector.save(SE->instrumentPath);

		SE->moduleFlags |= applyingActive;

		//SE->selectedPlace = 1;
		SE->activateLabelsBorder();
	}

	return 1;
}

static uint8_t functUndo()
{
	if(SE->currSelEffect == effectCrop)
	{
		if(SE->undoCropFlag)
		{
			SE->undoCropFlag = 0;

			effector.undoTrim();

			effector.save(SE->instrumentPath);
			SE->moduleFlags |= applyingActive;

			SE->resetSpectrumAndPoints();
		}
	}
	else if(SE->currSelEffect == effectReverse)
	{
		if(SE->undoReverseFlag)
		{
			SE->undoReverseFlag = 0;
			effector.undoReverse();

			effector.save(SE->instrumentPath);
			SE->moduleFlags |= applyingActive;
		}
	}

	//SE->selectedPlace = 2;
	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functSelectPlace(uint8_t button)
{
	if(SE->moduleFlags != 0) return 1;

	uint8_t parameterFlag = 1;

	if(button == interfaceButton2)// Undo
	{
		if(SE->effectScreen[SE->currSelEffect].undoActive)
		{
			parameterFlag = 0;
			functUndo();
		}
	}

	if(SE->selectedPlace == 6) // gora/dol na effekcie
	{
		if(button == interfaceButton7)
		{
			SE->changeEffectSelection(-1);
		}
		else if(button == interfaceButton6)
		{
			SE->changeEffectSelection(1);
		}
	}

	if(parameterFlag)
	{
		if(button < interfaceButton7)
		{
			SE->selectedPlace = button;
		}
		else
		{
			SE->selectedPlace = 6;
		}

		SE->activateLabelsBorder();
	}

	return 1;
}

static  uint8_t functEncoder(int16_t value)
{
	if(SE->moduleFlags != 0) return 1;

	switch(SE->selectedPlace)
	{
	case 0: break;
	case 1: break;
	case 2: SE->editParamFunction(0, value);	break;
	case 3: SE->editParamFunction(1, value);	break;
	case 4: SE->editParamFunction(2, value);    break;
	case 5: SE->editParamFunction(3, value);	break;
	case 6: SE->changeEffectSelection(value);	break;
	}

	return 1;
}

void cSampleEditor::editParamFunction(uint8_t paramNum, int16_t value)
{
	if(effectScreen[currSelEffect].bar[paramNum].editFunct != NULL)
	{
		effectScreen[currSelEffect].bar[paramNum].effectPercentage = effectScreen[currSelEffect].bar[paramNum].editFunct(value);
		updateEffectValues(&effectScreen[currSelEffect], paramNum);
	}
}

static  uint8_t functLeft()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->selectedPlace > 0) SE->selectedPlace--;

	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->selectedPlace < SE->frameData.placesCount-1) SE->selectedPlace++;

	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	if(SE->moduleFlags != 0) return 1;

	switch(SE->selectedPlace)
	{
	case 0: break;
	case 1: break;
	case 2: SE->editParamFunction(0, -1);	break;
	case 3: SE->editParamFunction(1, -1);	break;
	case 4: SE->editParamFunction(2, -1);    break;
	case 5: SE->editParamFunction(3, -1);	break;
	case 6: SE->changeEffectSelection(-1);	break;
	}

	return 1;
}

static  uint8_t functDown()
{
	if(SE->moduleFlags != 0) return 1;

	switch(SE->selectedPlace)
	{
	case 0: break;
	case 1: break;
	case 2: SE->editParamFunction(0, -1);	break;
	case 3: SE->editParamFunction(1, -1);	break;
	case 4: SE->editParamFunction(2, -1);    break;
	case 5: SE->editParamFunction(3, -1);	break;
	case 6: SE->changeEffectSelection(1);	break;
	}

	return 1;
}

static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0)
	{
		sequencer.play();
	}
	else if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	return 1;
}

static  uint8_t functRecAction()
{


	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(SE->moduleFlags != 0) return 1;

	if(SE->onExitFlag == 0)
	{
		SE->exitButton = button;
		SE->moduleFlags |= onExitReloadActive;
		SE->onExitFlag = 1;

		fileManager.samplesLoader.start(0, (char*)"Workspace/samples");
	}

	if(SE->onExitFlag == 2)
	{
		SE->moduleFlags &= ~onExitReloadActive;
		SE->onExitFlag = 0;
		SE->eventFunct(eventSwitchModule,SE,&button,0);
	}

	return 1;
}

//======================================================================================================================
void cSampleEditor::changeEffectSelection(int16_t value)
{
	if(currSelEffect + value < 0) currSelEffect = 0;
	else if(currSelEffect + value > effectsCount - 1) currSelEffect = effectsCount-1;
	else currSelEffect += value;

	refreshPoints = 1;

	display.setControlValue(effectListControl, currSelEffect);
	display.refreshControl(effectListControl);

	showEffectScreen(&effectScreen[currSelEffect]);
}

static uint8_t functShift(uint8_t value)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	if(value == 1)
	{
		//eventFunct(mtInstrumentEditorEventPadPress, &interfacePadStop, 0, 0);
		sequencer.stop();

		SE->isPlayingSample = 1;
		if(SE->editorInstrument->glide > 0)
		{
			switch(	SE->glidePreviewDif)
			{
				case 0: SE->playNote = 24;	break;
				case 1: SE->playNote = (SE->playNote == 24)? 25 : 24; 	break;
				case 2: SE->playNote = (SE->playNote == 24)? 36 : 24; 	break;
				case 3: SE->playNote = (SE->playNote == 24)? 47 : 24; 	break;
			}
		}

		instrumentPlayer[0].noteOn(mtProject.values.lastUsedInstrument, SE->playNote, -1);
	}
	else if(value == 0)
	{
		if(SE->isPlayingSample) instrumentPlayer[0].noteOff();
		SE->isPlayingSample = 0;
	}

	return 1;
}


static  uint8_t functInstrument(uint8_t state)
{
	if(SE->moduleFlags != 0) return 1;

	if(state == buttonRelease)
	{
		SE->cancelPopups();

		if(SE->localInstrNum != mtProject.values.lastUsedInstrument)
		{
			SE->localInstrNum = mtProject.values.lastUsedInstrument;
			SE->start(0);
		}
	}
	else if(state == buttonPress)
	{
		mtPopups.showStepPopup(stepPopupInstr, mtProject.values.lastUsedInstrument);
		//SE->lightUpPadBoard();
	}

	return 1;
}

static uint8_t functStepNote(uint8_t value)
{
	if(SE->moduleFlags != 0) return 1;

	if(value == buttonRelease)
	{
		SE->cancelPopups();

	}
	else if(value == buttonPress)
	{
		mtPopups.showStepPopup(stepPopupNote, -1);

		//SE->lightUpPadBoard();
	}

	return 1;
}

void cSampleEditor::updateEffectValues(effect_screen_t *effect, uint8_t barNum)
{
	printNewValue(effect->bar[barNum].dataSource, barNum, effect->bar[barNum].dataUnit, effect->bar[barNum].dataFormat);
	showValueLabels(barNum);

	refreshBarsValue(barNum,effect->bar[barNum].effectPercentage);
}

void cSampleEditor::printNewValue(const void *data, uint8_t whichBar, const char* unit, source_datatype_t sourceType)
{
	char format[6] ="%d";

	if(data != NULL)
	{
		switch(sourceType)
		{
		case tUNSIGNED8:
			sprintf(&SE->dataBarText[whichBar][0], format, *((uint8_t*)data));
			break;
		case tUNSIGNED16:
			sprintf(&SE->dataBarText[whichBar][0], format, *((uint16_t*)data));
			break;
		case tUNSIGNED32:
			sprintf(&SE->dataBarText[whichBar][0], format, *((uint32_t*)data));
			break;
		case tSIGNED8:
			sprintf(&SE->dataBarText[whichBar][0], format, *((int8_t*)data));
			break;
		case tSIGNED16:
			sprintf(&SE->dataBarText[whichBar][0], format, *((int16_t*)data));
			break;
		case tSIGNED32:
			sprintf(&SE->dataBarText[whichBar][0], format, *((int32_t*)data));
			break;
		case tFLOAT:
			strncpy(format,"%.2f",6);
			sprintf(&SE->dataBarText[whichBar][0], format, *((float_t*)data));
			break;
		}

		if(*unit != '\0')
		{
			strcat(&SE->dataBarText[whichBar][0],unit);
		}
	}
	else
	{
		memset(&SE->dataBarText[whichBar][0],0,6);
	}
}

void cSampleEditor::resetSpectrumAndPoints()
{
	startPoint = 0;
	endPoint = MAX_16BIT;
	points.endPoint = endPoint;
	points.startPoint = startPoint;

	GP.spectrumResetZoom(startPoint, effector.getLength()/2 ,&zoom);

	updateEffectValues(&effectScreen[currSelEffect], 1);
	updateEffectValues(&effectScreen[currSelEffect], 2);
	updateEffectValues(&effectScreen[currSelEffect], 3);

	refreshSpectrum = 1;
	refreshPoints = 1;
}

void cSampleEditor::makeEffect()
{
	if(SE->moduleFlags != 0)
	{
		return;
	}

	uint8_t newPreviewFlag = SE->previewNewEffectRequested();
	uint8_t processingActivated = 0;

	switch((effect_t)SE->currSelEffect)
	{
	case effectCrop:
		break;
	case effectReverse:
		break;
	case effectFlanger:
		processingActivated = effectorFlanger.makeFlanger(FLANGER_LENGTH_MAX, SE->flangerOffset, SE->flangerDepth, SE->flangerDelay, newPreviewFlag);
		break;
	case effectChorus:
		processingActivated = effectorChorus.makeChorus(SE->chorusLength, SE->chorusStrength, newPreviewFlag);
		break;
	case effectDelay:
		processingActivated = effectorDelay.makeDelay(SE->delayFeedback, SE->delayTime, newPreviewFlag);
		break;
	case effectCompressor:
		processingActivated = effectorCompressor.makeCompressor(SE->compressorThrs, SE->compressorRatio, SE->compressorAttack, SE->compressorRelease, newPreviewFlag);
		break;
	case effectBitcrusher:
		processingActivated = effectorBitcrusher.makeBitcrusher(SE->bitcrusherBits, SE->bitcrusherRate, newPreviewFlag);
		break;
	case effectAmplifier:
		effectorAmplifier.makeAmplifier(SE->amplifierAmp, newPreviewFlag);// instant efffect
		previewReadyFlag = 1;
		break;
	default:
		break;
	}

	if(processingActivated)
	{
		SE->isAnyEffectActive = true;
		SE->moduleFlags |= processingActive;
		SE->previewReadyFlag = 0;
		SE->showProcessingBar(0);
	}
}

void cSampleEditor::updateEffectProcessing()
{
	if(previewReadyFlag == 0)
	{
		uint8_t processingStatus = 0;
		uint8_t progress = 0;

		switch(currSelEffect)
		{
		case effectChorus:
			processingStatus = effectorChorus.requireProcess();

			if(processingStatus)
			{
				effectorChorus.process();
				progress = effectorChorus.getProgress();
			}
			break;
		case effectDelay:
			processingStatus = effectorDelay.requireProcess();

			if(processingStatus)
			{
				effectorDelay.process();
				progress = effectorDelay.getProgress();
			}
			break;
		case effectFlanger:
			processingStatus = effectorDelay.requireProcess();

			if(processingStatus)
			{
				effectorFlanger.process();
				progress = effectorFlanger.getProgress();
			}
			break;
		case effectCompressor:
			processingStatus = effectorCompressor.requireProcess();

			if(processingStatus)
			{
				effectorCompressor.process();
				progress = effectorCompressor.getProgress();
			}
			break;
		case effectBitcrusher:
			processingStatus = effectorBitcrusher.requireProcess();

			if(processingStatus)
			{
				effectorBitcrusher.process();
				progress = effectorBitcrusher.getProgress();
			}
			break;
		case effectAmplifier:
			break;
		default:
			break;
		}

		if(processingStatus)
		{
			showProcessingBar(progress);
		}
		else
		{
			SE->moduleFlags &= ~processingActive;
			previewReadyFlag = 1;
			hideHorizontalBar();
		}
	}
}

//------------------- EDIT FUNCTIONS --------------------
//
uint8_t changeZoom(int16_t value)
{
	GP.spectrumChangeZoom(value, effector.getLength()/2, &SE->zoom);

	SE->refreshSpectrum = 1;
	SE->refreshPoints = 1;

	return 0;
}

uint8_t modStartPoint(int16_t value)
{
	// obliczenie kroku przesuniecia w zaleznosci od ilosci widzianych probek na wyswietlaczu
	uint16_t move_step = SE->zoom.zoomWidth / 480;
	value = value * move_step;

	if(SE->startPoint + value < SAMPLE_POINT_POS_MIN) SE->startPoint  = 0;
	else if(SE->startPoint + value > SAMPLE_POINT_POS_MAX ) SE->startPoint  = SAMPLE_POINT_POS_MAX;
	else SE->startPoint += value;

	if(SE->startPoint > SE->endPoint)
	{
		SE->startPoint = SE->endPoint-1;
	}

	// odswiez spektrum tylko jesli: zoom wiekszy niz 1, ostatnio modyfikowany inny punkt, punkt jest poza widocznym obszarem
	if(SE->zoom.zoomValue > 1 && (SE->zoom.lastChangedPoint != 1
			|| (SE->startPoint < SE->zoom.zoomStart || SE->startPoint > SE->zoom.zoomEnd)))
	{
		SE->refreshSpectrum = 1;
	}

	SE->zoom.lastChangedPoint = 1;
	SE->refreshPoints = 1;

	return 0;
}

uint8_t modEndPoint(int16_t value)
{
	uint16_t move_step = SE->zoom.zoomWidth / 480;
	value = value * move_step;

	if(SE->endPoint + value < SAMPLE_POINT_POS_MIN) SE->endPoint  = 0;
	else if(SE->endPoint + value > SAMPLE_POINT_POS_MAX ) SE->endPoint  = SAMPLE_POINT_POS_MAX;
	else SE->endPoint += value;

	if(SE->endPoint < SE->startPoint)
	{
		SE->endPoint = SE->startPoint+1;
	}

	if(SE->zoom.zoomValue > 1 && (SE->zoom.lastChangedPoint != 2
			|| (SE->endPoint < SE-> zoom.zoomStart || SE->endPoint > SE->zoom.zoomEnd)))
	{
		SE->refreshSpectrum = 1;
	}

	SE->zoom.lastChangedPoint = 2;
	SE->refreshPoints = 1;

	return 0;
}

uint8_t editChorusLength(int16_t value)
{
	if(SE->chorusLength + (value * AUDIO_BLOCK_SAMPLES) < 0) SE->chorusLength = 0;
	else if(SE->chorusLength + (value * AUDIO_BLOCK_SAMPLES) > CHORUS_BUF_SIZE) SE->chorusLength = CHORUS_BUF_SIZE;
	else SE->chorusLength += (value * AUDIO_BLOCK_SAMPLES);


	return ((SE->chorusLength * 100)/(CHORUS_BUF_SIZE));
}

uint8_t editChorusStrength(int16_t value)
{
	if(SE->chorusStrength + value < CHORUS_STRENGTH_MIN) SE->chorusStrength = CHORUS_STRENGTH_MIN;
	else if(SE->chorusStrength + value > CHORUS_STRENGTH_MAX) SE->chorusStrength = CHORUS_STRENGTH_MAX;
	else SE->chorusStrength += value;


	return (((SE->chorusStrength - CHORUS_STRENGTH_MIN) * 100)/(CHORUS_STRENGTH_MAX - CHORUS_STRENGTH_MIN));
}

uint8_t editDelayFeedback(int16_t value)
{
	float delayStep = DELAY_FEEDBACK_STEP;

	if(SE->delayFeedback + (value * delayStep) < 0) SE->delayFeedback = 0;
	else if(SE->delayFeedback + (value * delayStep) > DELAY_FEEDBACK_MAX) SE->delayFeedback = DELAY_FEEDBACK_MAX;
	else SE->delayFeedback += (value * delayStep);

	return (SE->delayFeedback * 100)/DELAY_FEEDBACK_MAX;
}

uint8_t editDelayTime(int16_t value)
{
	uint8_t timeStep_ms = DELAY_TIME_STEP;

	if(SE->delayTime + (value * timeStep_ms) < 0) SE->delayTime = 0;
	else if(SE->delayTime + (value * timeStep_ms) > DELAY_TIME_MAX) SE->delayTime = DELAY_TIME_MAX;
	else SE->delayTime += (value * timeStep_ms);

	return (SE->delayTime * 100)/DELAY_TIME_MAX;
}

uint8_t editFlangerOffset(int16_t value)
{
	if(SE->flangerOffset + value < 0) SE->flangerOffset = 0;
	else if(SE->flangerOffset + value  > FLANGER_OFFSET_MAX) SE->flangerOffset = FLANGER_OFFSET_MAX;
	else SE->flangerOffset += value;


	return ((SE->flangerOffset * 100)/(FLANGER_OFFSET_MAX));
}

uint8_t editFlangerDepth(int16_t value)
{
	if(SE->flangerDepth + value < 0) SE->flangerDepth = 0;
	else if(SE->flangerDepth + value > FLANGER_DEPTH_MAX) SE->flangerDepth = FLANGER_DEPTH_MAX;
	else SE->flangerDepth += value;


	return ((SE->flangerDepth * 100)/FLANGER_DEPTH_MAX);
}

uint8_t editFlangerDelay(int16_t value)
{
	if(SE->flangerDelay + value < 0) SE->flangerDelay = 0;
	else if(SE->flangerDelay + value > FLANGER_DELAYRATE_MAX) SE->flangerDelay = FLANGER_DELAYRATE_MAX;
	else SE->flangerDelay += value;


	return ((SE->flangerDelay * 100)/FLANGER_DELAYRATE_MAX);
}

uint8_t editCompressorThreshold(int16_t value)
{
	uint8_t step = 10;

	if(SE->compressorThrs + (value*step) < 0) SE->compressorThrs = 0;
	else if(SE->compressorThrs + (value*step)  > CMPSR_THRESHOLD_MAX) SE->compressorThrs = CMPSR_THRESHOLD_MAX;
	else SE->compressorThrs += (value*step);


	return ((SE->compressorThrs * 100)/(CMPSR_THRESHOLD_MAX));
}

uint8_t editCompressorRatio(int16_t value)
{
	if(SE->compressorRatio + value < 0) SE->compressorRatio = 0;
	else if(SE->compressorRatio + value > CMPSR_RATIO_MAX) SE->compressorRatio = CMPSR_RATIO_MAX;
	else SE->compressorRatio += value;


	return ((SE->compressorRatio * 100)/CMPSR_RATIO_MAX);
}

uint8_t editCompressorAttack(int16_t value)
{
	if(SE->compressorAttack + value < 0) SE->compressorAttack = 0;
	else if(SE->compressorAttack + value > CMPSR_ATTACK_MAX_MS) SE->compressorAttack = CMPSR_ATTACK_MAX_MS;
	else SE->compressorAttack += value;


	return ((SE->compressorAttack * 100)/CMPSR_ATTACK_MAX_MS);
}

uint8_t editCompressorRelease(int16_t value)
{
	if(SE->compressorRelease + value < 0) SE->compressorRelease = 0;
	else if(SE->compressorRelease + value > CMPSR_RELEASE_MAX_MS) SE->compressorRelease = CMPSR_RELEASE_MAX_MS;
	else SE->compressorRelease += value;


	return ((SE->compressorRelease * 100)/CMPSR_RELEASE_MAX_MS);
}

uint8_t editBitcrusherBits(int16_t value)
{
	if(SE->bitcrusherBits + value < 0) SE->bitcrusherBits = 0;
	else if(SE->bitcrusherBits + value > BITCRUSHER_BITS_MAX) SE->bitcrusherBits = BITCRUSHER_BITS_MAX;
	else SE->bitcrusherBits += value;


	return ((SE->bitcrusherBits * 100)/BITCRUSHER_BITS_MAX);
}

uint8_t editBitcrusherRate(int16_t value)
{
	uint8_t step = 10;

	if(SE->bitcrusherRate + (value*step) < 0) SE->bitcrusherRate = 0;
	else if(SE->bitcrusherRate + (value*step) > BITCRUSHER_RATE_MAX) SE->bitcrusherRate = BITCRUSHER_RATE_MAX;
	else SE->bitcrusherRate += (value*step);


	return ((SE->bitcrusherRate * 100)/BITCRUSHER_RATE_MAX);
}

uint8_t editAmplifierAmp(int16_t value)
{
	float step = 0.1f;

	if(SE->amplifierAmp + (value*step) < 0) SE->amplifierAmp = 0;
	else if(SE->amplifierAmp + (value*step) > AMPLIFIER_AMP_MAX) SE->amplifierAmp = AMPLIFIER_AMP_MAX;
	else SE->amplifierAmp += (value * step);


	return ((SE->amplifierAmp * 100)/AMPLIFIER_AMP_MAX);
}



