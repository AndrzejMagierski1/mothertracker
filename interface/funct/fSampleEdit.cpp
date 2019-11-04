

#include "sampleEditor.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "interfacePopups.h"

#include "graphicProcessing.h"
#include "mtEffector.h"

#include "mtPadsBacklight.h"



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

static uint8_t functSelectPlace(uint8_t button, uint8_t state);


static uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static uint8_t functShift(uint8_t value);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);

static uint8_t functPreview(uint8_t state);
static uint8_t functApply();
static uint8_t functUndo();

static uint8_t changeEffect(uint8_t button);



// ------ EDIT FUNCTIONS PROTOTYPES -----
static uint8_t modStartPoint(int16_t value);
static uint8_t modEndPoint(int16_t value);
static uint8_t changeZoom(int16_t value);

static uint8_t editChorusLength(int16_t value);
static uint8_t editChorusStrength(int16_t value);

static uint8_t editDelayFeedback(int16_t value);
static uint8_t editDelayTime(int16_t value);

static uint8_t editFlangerOffset(int16_t value);
static uint8_t editFlangerDepth(int16_t value);
static uint8_t editFlangerDelay(int16_t value);

static uint8_t editCompressorThreshold(int16_t value);
static uint8_t editCompressorRatio(int16_t value);
static uint8_t editCompressorAttack(int16_t value);
static uint8_t editCompressorRelease(int16_t value);

static uint8_t editBitcrusherBits(int16_t value);
static uint8_t editBitcrusherRate(int16_t value);

static uint8_t editAmplifierAmp(int16_t value);

static uint8_t editLimiterThreshold(int16_t value);
static uint8_t editLimiterAttack(int16_t value);
static uint8_t editLimiterRelease(int16_t value);



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

	refreshPlayingProgress();
	refreshSampleLoading();
	refreshSampleApplying();
	refreshInstrumentLoading();

	updateEffectProcessing();

	onExitRaload();
}

void cSampleEditor::onExitRaload()
{
	if(onExitFlag == 1)
	{
		uint8_t progress = fileManager.samplesLoader.getCurrentProgress();
		showHorizontalBar(progress, "Reloading Instruments");

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
		uint8_t progress = effector.saveUpdate();

		if((moduleFlags & applyingActive))
		{
			showHorizontalBar(progress,"Applying effect");
		}
		else if((moduleFlags & undoActive))
		{
			showHorizontalBar(progress,"Undoing");
		}
	}
	else if(status == saveDone)
	{
		SE->moduleFlags &= ~(applyingActive | undoActive);
		hideHorizontalBar();

		if(currSelEffect == effectCrop)
		{
			SE->resetSpectrumAndPoints();
		}

		refreshSpectrum = 1;
		effector.setSaveStatus(waitingForSaveInit);
		SE->isAnyEffectActive = false;
	}
}

void cSampleEditor::refreshInstrumentLoading()
{
	if(moduleFlags == onEntryStillLoading)
	{
		uint8_t state = fileManager.samplesLoader.getStateFlag();

		if(state == loaderStateTypeInProgress)
		{
			uint8_t progress = fileManager.samplesLoader.getCurrentProgress();
			showHorizontalBar(progress,"Loading Instruments");
		}
		else
		{
			hideHorizontalBar();
			moduleFlags &= ~onEntryStillLoading;
			//startLoadingSample();
		}
	}
}

void cSampleEditor::refreshPlayingProgress()
{
	if(refreshSpectrumProgress)
	{
		display.setControlValue(progressCursor,playProgressInSpectrum);
		display.refreshControl(progressCursor);
		refreshSpectrumProgress = 0;
	}

	if(isPlayingSample)
	{
		calcPlayProgressValue();
		if(isPlayingSample)
		{
			//showPreviewValue(); // w calcPlayProgress jest mozliwosc wyzerowania tej flagi wtedy nie chcemy wyswietlac wartosci;
		}
		else
		{
			//hidePreviewValue();
			display.setControlHide(progressCursor);
			display.refreshControl(progressCursor);
		}

		if(instrumentPlayer[0].getInterfacePlayingEndFlag())
		{
			instrumentPlayer[0].clearInterfacePlayingEndFlag();

			playProgressValue=0;
			playProgressInSpectrum = 0;
			isPlayingSample = 0;
			refreshSpectrumProgress = 1;
			//hidePreviewValue();
			mtPadBoard.clearVoice(0);
		}
	}
}

void cSampleEditor::refreshSampleLoading()
{
	if((firstSampleLoadFlag == 0) && (moduleFlags & sampleLoadingActive))
	{
		sampleLoadedState = fileManager.samplesLoader.waveLoader.getState();

		if(sampleLoadedState == loaderStateTypeInProgress) // refresh update when in progress
		{
			fileManager.samplesLoader.waveLoader.update();

			uint8_t progress = fileManager.samplesLoader.waveLoader.getCurrentWaveProgress();
			showHorizontalBar(progress,"Loading sample");
		}

		if((sampleLoadedState == loaderStateTypeEnded) && (lastSampleLoadedState == loaderStateTypeInProgress)) // do when loaded
		{
			showCurrentSpectrum(effector.getLength()/2, effector.getAddress());
			hideHorizontalBar();
			moduleFlags &= ~sampleLoadingActive;

			firstSampleLoadFlag = 1;
			sampleIsValid = 1;

			functPreview(1);
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

void cSampleEditor::startLoadingSample()
{
	if((mtProject.instrument[localInstrNum].isActive) && (mtProject.instrument[localInstrNum].sample.type != mtSampleTypeWavetable))
	{
		if(!(moduleFlags & onEntryStillLoading))
		{
			sprintf(instrumentPath, "Workspace/samples/instr%02d.wav", localInstrNum);
			effector.loadSample(instrumentPath);
			moduleFlags |= sampleLoadingActive;
		}
	}
	else
	{
		sampleIsValid = 0;
	}
}

void cSampleEditor::start(uint32_t options)
{
	moduleRefresh = 1;
	firstSampleLoadFlag = 0;
	sampleIsValid = 0;
	lastPreviewEffect = (effect_t)UINT32_MAX;// this will force new calculation on effect on each entry
	previewReadyFlag = 0;

	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
	}

	//--------------------------------------------------------------------
	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
	{
		mtProject.values.lastUsedInstrument = 0;
	}

	if(fileManager.samplesLoader.getStateFlag() == loaderStateTypeInProgress)
	{
		moduleFlags |= onEntryStillLoading;
	}

	if(mtProject.values.lastUsedInstrument < INSTRUMENTS_COUNT)
	{
		editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];
		GP.processSpectrum(editorInstrument, &zoom, &spectrum);
	}

	localInstrNum = mtProject.values.lastUsedInstrument;

	isAnyEffectActive = 0;
	effectAppliedFlag = 0;

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

	//!< Limiter effect
	//
	effectScreen[effectLimiter].screen = noSpectrum;

	effectScreen[effectLimiter].paramNum = 3;
	effectScreen[effectLimiter].barNum = 3;

	effectScreen[effectLimiter].bar[1].name = "Threshold";
	effectScreen[effectLimiter].bar[1].editFunct = editLimiterThreshold;
	effectScreen[effectLimiter].bar[1].dataSource = &limiterThreshold;
	effectScreen[effectLimiter].bar[1].dataFormat = tUNSIGNED16,

	effectScreen[effectLimiter].bar[2].name = "Attack";
	effectScreen[effectLimiter].bar[2].editFunct = editLimiterAttack;
	effectScreen[effectLimiter].bar[2].dataSource = &limiterAttack;
	effectScreen[effectLimiter].bar[2].dataFormat = tUNSIGNED16,

	effectScreen[effectLimiter].bar[3].name = "Release";
	effectScreen[effectLimiter].bar[3].editFunct = editLimiterRelease;
	effectScreen[effectLimiter].bar[3].dataSource = &limiterRelease;
	effectScreen[effectLimiter].bar[3].dataFormat = tUNSIGNED16,
	effectScreen[effectLimiter].undoActive = 0;

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
	FM->setButtonObj(interfaceButton2, functSelectPlace);

	FM->setButtonObj(interfaceButton3, functSelectPlace);
	FM->setButtonObj(interfaceButton4, functSelectPlace);
	FM->setButtonObj(interfaceButton5, functSelectPlace);
	FM->setButtonObj(interfaceButton6, buttonPress, changeEffect);
	FM->setButtonObj(interfaceButton7, buttonPress, changeEffect);

	FM->setButtonObj(interfaceButtonNote, functStepNote);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setPadsGlobal(functPads);

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
	if(state > buttonPress) return 1;

	if((SE->sampleIsValid != 1) && (state == 1))
	{
		if((SE->firstSampleLoadFlag == 0))
		{
			SE->startLoadingSample();
		}

		return 1;
	}

	if((state == 1) && (SE->moduleFlags == 0))
	{
		SE->makeEffect();

		if(SE->previewReadyFlag)
		{
			if(mtPadBoard.getEmptyVoice() < 0) return 1;

			if(SE->effectScreen[SE->currSelEffect].screen == fullSpectrum)
			{
				if(mtPadBoard.getEmptyVoice() == 0)
				{
					SE->playPitch = (float)notes[mtPadBoard.convertPadToNote(12)];
					SE->playProgresValueTim = ((((effector.getLength()/44100.0 ) * SE->startPoint) / MAX_16BIT) * 1000000) / SE->playPitch;
					SE->refreshPlayProgressValue = 0;
					SE->loopDirection = 0;
					SE->isPlayingSample = 1;

					instrumentPlayer[0].clearInterfacePlayingEndFlag();
				}
			}

			if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
			{
				effector.play(SE->startPoint, SE->endPoint , 12);

			}
			else
			{
				effector.playPrev(12);
			}
		}

		SE->lastPreviewEffect = (effect_t)SE->currSelEffect;

		//mtPadBoard.startInstrument(pad, SE->localInstrNum,-1);
	}
	else if(state == 0)
	{
		effector.stop(12);

		if((SE->effectScreen[SE->currSelEffect].screen == fullSpectrum))
		{
			if(mtPadBoard.getVoiceTakenByPad(12) == 0)
			{
				SE->playProgressValue=0;
				SE->playProgressInSpectrum = 0;
				SE->isPlayingSample = 0;
				SE->refreshSpectrumProgress = 1;
			}
		}
	}

	//SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functApply()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->sampleIsValid != 1) return 1;

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
		SE->effectAppliedFlag = 1;

		//SE->selectedPlace = 1;
		//SE->activateLabelsBorder();
	}

	return 1;
}

static uint8_t functUndo()
{
	if(SE->sampleIsValid != 1) return 1;

	if(SE->currSelEffect == effectCrop)
	{
		if(SE->undoCropFlag)
		{
			SE->undoCropFlag = 0;

			effector.undoTrim();

			effector.save(SE->instrumentPath);
			SE->moduleFlags |= undoActive;
		}
	}
	else if(SE->currSelEffect == effectReverse)
	{
		if(SE->undoReverseFlag)
		{
			SE->undoReverseFlag = 0;

			effector.undoReverse();

			effector.save(SE->instrumentPath);
			SE->moduleFlags |= undoActive;
		}
	}

	return 1;
}

static uint8_t changeEffect(uint8_t button)
{
	if(SE->moduleFlags != 0) return 1;

	SE->clearAllNodes();
	SE->cancelMultiFrame();

	if(button == interfaceButton7)
	{
		SE->changeEffectSelection(-1);
	}
	else if(button == interfaceButton6)
	{
		SE->changeEffectSelection(1);
	}

	SE->points.selected = 0;
	SE->refreshPoints = 1;

	SE->selectedPlace = 6;
	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functSelectPlace(uint8_t button , uint8_t state)
{
	if(SE->moduleFlags != 0) return 1;
	if(state > buttonPress) return 1;

	uint8_t parameterFlag = 1;

	if(button == interfaceButton2 && state == buttonPress)// Undo
	{
		if(SE->effectScreen[SE->currSelEffect].undoActive)
		{
			parameterFlag = 0;
			functUndo();
		}
	}

	if(parameterFlag)
	{
		uint8_t minSelectedPlace = 0;
		if(SE->effectScreen[SE->currSelEffect].paramNum == 4)
		{
			minSelectedPlace = 2;
		}
		else if(SE->effectScreen[SE->currSelEffect].paramNum == 3)
		{
			minSelectedPlace = 3;
		}
		else if(SE->effectScreen[SE->currSelEffect].paramNum == 2)
		{
			minSelectedPlace = 4;
		}
		else if(SE->effectScreen[SE->currSelEffect].paramNum == 1)
		{
			minSelectedPlace = 5;
		}

		if(state == buttonPress)
		{
			SE->selectedPlace = button;

			if(button >= minSelectedPlace)
			{
				SE->addNode(SE->effectScreen[SE->currSelEffect].bar[button - 2].editFunct, button - 2);

				SE->frameData.multisel[button].frameNum = button;
				SE->frameData.multisel[button].isActive = 1;
				SE->frameData.multiSelActiveNum  += 1;
			}

			if(SE->currSelEffect == effectCrop || SE->currSelEffect == effectReverse)
			{
				if(SE->frameData.multiSelActiveNum == 1)
				{
					SE->points.selected = 0;
				}

				if(button == interfaceButton3)
				{
					SE->points.selected |= selectStart;
				}
				else if(button == interfaceButton4)
				{
					SE->points.selected |= selectEnd;
				}
				else if(button == interfaceButton5)
				{
					SE->points.selected = 0;
					SE->clearAllNodes();
					SE->cancelMultiFrame();
				}
			}
		}
		else if(state == buttonRelease)
		{
			if(button >= minSelectedPlace)
			{
				if(SE->frameData.multiSelActiveNum)
				{
					if(SE->frameData.multisel[button].isActive)
					{
						SE->removeNode(button - 2);
						SE->frameData.multiSelActiveNum  -= 1;

						SE->frameData.multisel[button].isActive = 0;

						if(SE->currSelEffect == effectCrop || SE->currSelEffect == effectReverse)
						{
							uint8_t sel = 0;

							if(button == interfaceButton3)
							{
								sel = selectStart;
							}
							else if(button == interfaceButton4)
							{
								sel = selectEnd;
							}

							SE->points.selected &= ~sel;

							if(SE->frameData.multiSelActiveNum == 0)
							{
								SE->points.selected = sel;
							}
						}

						if(SE->frameData.multiSelActiveNum == 0)
						{
							SE->selectedPlace = button;
						}
					}
				}
			}
		}

		if(SE->currSelEffect == effectCrop || SE->currSelEffect == effectReverse)
		{
			SE->refreshPoints = 1;
		}

		SE->activateLabelsBorder();
	}

	return 1;
}

static  uint8_t functEncoder(int16_t value)
{
	if(SE->moduleFlags != 0) return 1;

	if(SE->frameData.multiSelActiveNum != 0)
	{
		SE->editParamFunctionSelection(value);
	}
	else
	{
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

void cSampleEditor::editParamFunctionSelection(int16_t value)
{
	selection_percentages temp;

	temp = SE->stepThroughNodes(value);

	for(uint8_t i = 0; i < MAX_DATA_BARS; i++)
	{
		if(temp.mask & (1 << i))
		{
			effectScreen[currSelEffect].bar[i].effectPercentage = temp.percentages[i];
			updateEffectValues(&effectScreen[currSelEffect], i);
		}
	}
}

static  uint8_t functLeft()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->frameData.multiSelActiveNum != 0) return 1;

	if(SE->selectedPlace > ((SE->frameData.placesCount - 1) - SE->effectScreen[SE->currSelEffect].paramNum))
	{
		SE->selectedPlace--;
	}

	if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
	{
		if(SE->selectedPlace == 3)
		{
			SE->points.selected = selectStart;
		}
		else if(SE->selectedPlace == 4)
		{
			SE->points.selected = selectEnd;
		}
		else
		{
			SE->points.selected = 0;
		}

		SE->refreshPoints = 1;
	}

	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->frameData.multiSelActiveNum != 0) return 1;

	if(SE->selectedPlace < (SE->frameData.placesCount-1)) SE->selectedPlace++;

	if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
	{
		if(SE->selectedPlace == 3)
		{
			SE->points.selected = selectStart;
		}
		else if(SE->selectedPlace == 4)
		{
			SE->points.selected = selectEnd;
		}
		else
		{
			SE->points.selected = 0;
		}

		SE->refreshPoints = 1;
	}

	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	if(SE->moduleFlags != 0) return 1;

	if(SE->frameData.multiSelActiveNum != 0)
	{
		SE->editParamFunctionSelection(1);
	}
	else
	{
		switch(SE->selectedPlace)
		{
		case 0: break;
		case 1: break;
		case 2: SE->editParamFunction(0, 1);	break;
		case 3: SE->editParamFunction(1, 1);	break;
		case 4: SE->editParamFunction(2, 1);    break;
		case 5: SE->editParamFunction(3, 1);	break;
		case 6: SE->changeEffectSelection(-1);	break;
		}
	}

	return 1;
}

static  uint8_t functDown()
{
	if(SE->moduleFlags != 0) return 1;

	if(SE->frameData.multiSelActiveNum != 0)
	{
		SE->editParamFunctionSelection(-1);
	}
	else
	{
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
	}

	return 1;
}

static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		sequencer.play();
	}
	else
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
	if(SE->onExitFlag == 1) return 1;

	if(!(SE->moduleFlags & onExitReloadActive))
	{
		if(SE->onExitFlag == 0)
		{
			if(SE->effectAppliedFlag)
			{
				if(fileManager.samplesLoader.getStateFlag() == loaderStateTypeEnded)
				{
					SE->exitButton = button;
					SE->moduleFlags |= onExitReloadActive;
					SE->onExitFlag = 1;

					fileManager.samplesLoader.start(0, (char*)"Workspace/samples");
				}
			}
			else
			{
				SE->onExitFlag = 2; // allows to exit immediately
			}
		}
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
	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
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

static uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(SE->moduleFlags != 0) return 1;
	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
	}

	if(state == 1)
	{
		if(SE->previewReadyFlag)
		{
			if(mtPadBoard.getEmptyVoice() < 0) return 1;

			if(SE->effectScreen[SE->currSelEffect].screen == fullSpectrum)
			{
				if(mtPadBoard.getEmptyVoice() == 0)
				{
					SE->playPitch = (float)notes[mtPadBoard.convertPadToNote(pad)];
					SE->playProgresValueTim = ((((effector.getLength()/44100.0 ) * SE->startPoint) / MAX_16BIT) * 1000000) / SE->playPitch;
					SE->refreshPlayProgressValue = 0;
					SE->loopDirection = 0;
					SE->isPlayingSample = 1;

					instrumentPlayer[0].clearInterfacePlayingEndFlag();
				}
			}

			padsBacklight.setFrontLayer(1,20, pad);

			if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
			{
				effector.play(SE->startPoint, SE->endPoint , pad);
			}
			else
			{
				effector.playPrev(pad);
			}
		}
	}
	else if(state == 0)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		effector.stop(pad);

		if((SE->isPlayingSample) && (SE->effectScreen[SE->currSelEffect].screen == fullSpectrum))
		{
			if(mtPadBoard.getVoiceTakenByPad(pad) == 0)
			{
				SE->playProgressValue=0;
				SE->playProgressInSpectrum = 0;
				SE->isPlayingSample = 0;
				SE->refreshSpectrumProgress = 1;
				//SE->hidePreviewValue();
			}
		}
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
		previewReadyFlag = 1;
		break;
	case effectReverse:
		previewReadyFlag = 1;
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
	case effectLimiter:
		processingActivated = effectorLimiter.makeLimiter(SE->limiterThreshold, SE->limiterAttack, SE->limiterRelease, newPreviewFlag);
		break;
	default:
		break;
	}

	if(processingActivated)
	{
		SE->isAnyEffectActive = true;
		SE->moduleFlags |= processingActive;
		SE->previewReadyFlag = 0;
		//SE->showProcessingBar(0);
	}
}

void cSampleEditor::updateEffectProcessing()
{
	if(SE->moduleFlags & processingActive)
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
				processingStatus = effectorFlanger.requireProcess();

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
			case effectLimiter:
				processingStatus = effectorLimiter.requireProcess();

				if(processingStatus)
				{
					effectorLimiter.process();
					progress = effectorLimiter.getProgress();
				}
				break;
			default:
				break;
			}

			if(processingStatus)
			{
				showHorizontalBar(progress,"Processing effect");
			}
			else
			{
				SE->moduleFlags &= ~processingActive;
				previewReadyFlag = 1;
				hideHorizontalBar();
			}
		}
	}
}

//------------------- EDIT FUNCTIONS --------------------
//
static uint8_t changeZoom(int16_t value)
{
	GP.spectrumChangeZoom(value, effector.getLength()/2, &SE->zoom);

	SE->refreshSpectrum = 1;
	SE->refreshPoints = 1;

	return 0;
}

static uint8_t modStartPoint(int16_t value)
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

static uint8_t modEndPoint(int16_t value)
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

static uint8_t editChorusLength(int16_t value)
{
	if(SE->chorusLength + (value * AUDIO_BLOCK_SAMPLES) < 0) SE->chorusLength = 0;
	else if(SE->chorusLength + (value * AUDIO_BLOCK_SAMPLES) > CHORUS_BUF_SIZE) SE->chorusLength = CHORUS_BUF_SIZE;
	else SE->chorusLength += (value * AUDIO_BLOCK_SAMPLES);


	return ((SE->chorusLength * 100)/(CHORUS_BUF_SIZE));
}

static uint8_t editChorusStrength(int16_t value)
{
	if(SE->chorusStrength + value < CHORUS_STRENGTH_MIN) SE->chorusStrength = CHORUS_STRENGTH_MIN;
	else if(SE->chorusStrength + value > CHORUS_STRENGTH_MAX) SE->chorusStrength = CHORUS_STRENGTH_MAX;
	else SE->chorusStrength += value;


	return (((SE->chorusStrength - CHORUS_STRENGTH_MIN) * 100)/(CHORUS_STRENGTH_MAX - CHORUS_STRENGTH_MIN));
}

static uint8_t editDelayFeedback(int16_t value)
{
	float delayStep = DELAY_FEEDBACK_STEP;

	if(SE->delayFeedback + (value * delayStep) < 0) SE->delayFeedback = 0;
	else if(SE->delayFeedback + (value * delayStep) > DELAY_FEEDBACK_MAX) SE->delayFeedback = DELAY_FEEDBACK_MAX;
	else SE->delayFeedback += (value * delayStep);

	return (SE->delayFeedback * 100)/DELAY_FEEDBACK_MAX;
}

static uint8_t editDelayTime(int16_t value)
{
	uint8_t timeStep_ms = DELAY_TIME_STEP;

	if(SE->delayTime + (value * timeStep_ms) < 0) SE->delayTime = 0;
	else if(SE->delayTime + (value * timeStep_ms) > DELAY_TIME_MAX) SE->delayTime = DELAY_TIME_MAX;
	else SE->delayTime += (value * timeStep_ms);

	return (SE->delayTime * 100)/DELAY_TIME_MAX;
}

static uint8_t editFlangerOffset(int16_t value)
{
	if(SE->flangerOffset + value < 0) SE->flangerOffset = 0;
	else if(SE->flangerOffset + value  > FLANGER_OFFSET_MAX) SE->flangerOffset = FLANGER_OFFSET_MAX;
	else SE->flangerOffset += value;


	return ((SE->flangerOffset * 100)/(FLANGER_OFFSET_MAX));
}

static uint8_t editFlangerDepth(int16_t value)
{
	if(SE->flangerDepth + value < 0) SE->flangerDepth = 0;
	else if(SE->flangerDepth + value > FLANGER_DEPTH_MAX) SE->flangerDepth = FLANGER_DEPTH_MAX;
	else SE->flangerDepth += value;


	return ((SE->flangerDepth * 100)/FLANGER_DEPTH_MAX);
}

static uint8_t editFlangerDelay(int16_t value)
{
	if(SE->flangerDelay + value < 0) SE->flangerDelay = 0;
	else if(SE->flangerDelay + value > FLANGER_DELAYRATE_MAX) SE->flangerDelay = FLANGER_DELAYRATE_MAX;
	else SE->flangerDelay += value;


	return ((SE->flangerDelay * 100)/FLANGER_DELAYRATE_MAX);
}

static uint8_t editCompressorThreshold(int16_t value)
{
	uint8_t step = 10;

	if(SE->compressorThrs + (value*step) < 0) SE->compressorThrs = 0;
	else if(SE->compressorThrs + (value*step)  > CMPSR_THRESHOLD_MAX) SE->compressorThrs = CMPSR_THRESHOLD_MAX;
	else SE->compressorThrs += (value*step);


	return ((SE->compressorThrs * 100)/(CMPSR_THRESHOLD_MAX));
}

static uint8_t editCompressorRatio(int16_t value)
{
	if(SE->compressorRatio + value < 0) SE->compressorRatio = 0;
	else if(SE->compressorRatio + value > CMPSR_RATIO_MAX) SE->compressorRatio = CMPSR_RATIO_MAX;
	else SE->compressorRatio += value;


	return ((SE->compressorRatio * 100)/CMPSR_RATIO_MAX);
}

static uint8_t editCompressorAttack(int16_t value)
{
	if(SE->compressorAttack + value < 0) SE->compressorAttack = 0;
	else if(SE->compressorAttack + value > CMPSR_ATTACK_MAX_MS) SE->compressorAttack = CMPSR_ATTACK_MAX_MS;
	else SE->compressorAttack += value;


	return ((SE->compressorAttack * 100)/CMPSR_ATTACK_MAX_MS);
}

static uint8_t editCompressorRelease(int16_t value)
{
	if(SE->compressorRelease + value < 0) SE->compressorRelease = 0;
	else if(SE->compressorRelease + value > CMPSR_RELEASE_MAX_MS) SE->compressorRelease = CMPSR_RELEASE_MAX_MS;
	else SE->compressorRelease += value;


	return ((SE->compressorRelease * 100)/CMPSR_RELEASE_MAX_MS);
}

static uint8_t editBitcrusherBits(int16_t value)
{
	if(SE->bitcrusherBits + value < 0) SE->bitcrusherBits = 0;
	else if(SE->bitcrusherBits + value > BITCRUSHER_BITS_MAX) SE->bitcrusherBits = BITCRUSHER_BITS_MAX;
	else SE->bitcrusherBits += value;


	return ((SE->bitcrusherBits * 100)/BITCRUSHER_BITS_MAX);
}

static uint8_t editBitcrusherRate(int16_t value)
{
	uint8_t step = 10;

	if(SE->bitcrusherRate + (value*step) < 0) SE->bitcrusherRate = 0;
	else if(SE->bitcrusherRate + (value*step) > BITCRUSHER_RATE_MAX) SE->bitcrusherRate = BITCRUSHER_RATE_MAX;
	else SE->bitcrusherRate += (value*step);


	return ((SE->bitcrusherRate * 100)/BITCRUSHER_RATE_MAX);
}

static uint8_t editAmplifierAmp(int16_t value)
{
	float step = 0.1f;

	if(SE->amplifierAmp + (value*step) < 0) SE->amplifierAmp = 0;
	else if(SE->amplifierAmp + (value*step) > AMPLIFIER_AMP_MAX) SE->amplifierAmp = AMPLIFIER_AMP_MAX;
	else SE->amplifierAmp += (value * step);


	return ((SE->amplifierAmp * 100)/AMPLIFIER_AMP_MAX);
}

static uint8_t editLimiterThreshold(int16_t value)
{
	uint8_t step = 10;

	if(SE->limiterThreshold + (value*step) < 0) SE->limiterThreshold = 0;
	else if(SE->limiterThreshold + (value*step) > LIMITER_THRESHOLD_MAX) SE->limiterThreshold = LIMITER_THRESHOLD_MAX;
	else SE->limiterThreshold += (value*step);


	return ((SE->limiterThreshold * 100)/LIMITER_THRESHOLD_MAX);
}

static uint8_t editLimiterAttack(int16_t value)
{
	uint8_t step = 10;

	if(SE->limiterAttack + (value*step) < 0) SE->limiterAttack = 0;
	else if(SE->limiterAttack + (value*step) > LIMITER_ATTACK_MAX) SE->limiterAttack = LIMITER_ATTACK_MAX;
	else SE->limiterAttack += (value*step);


	return ((SE->limiterAttack * 100)/LIMITER_ATTACK_MAX);
}

static uint8_t editLimiterRelease(int16_t value)
{
	uint8_t  step = 10;

	if(SE->limiterRelease + (value*step) < 0) SE->limiterRelease = 0;
	else if(SE->limiterRelease + (value*step) > LIMITER_RELEASE_MAX) SE->limiterRelease = LIMITER_RELEASE_MAX;
	else SE->limiterRelease += (value * step);


	return ((SE->limiterRelease * 100)/LIMITER_RELEASE_MAX);
}

/*/////////// MultiSelect Functions ////////////////*/
void cSampleEditor::addNode(editFunct1_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cSampleEditor::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

selection_percentages cSampleEditor::stepThroughNodes(int16_t value)
{
	selection_percentages temp;

	memset(&temp,0,sizeof(temp));

	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		if(selectNodes[node].isActive)
		{
			if(selectNodes[node].editFunct != NULL)
			{
				temp.mask |= (1 << node);
				temp.percentages[node] = selectNodes[node].editFunct(value);
			}
		}
	}

	return temp;
}

void cSampleEditor::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}

void cSampleEditor::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		frameData.multisel[i].isActive = 0;
	}

	frameData.multiSelActiveNum = 0;
}
///////////////////////////////////////////////////////////////////////////

void cSampleEditor::calcPlayProgressValue()
{
	if(refreshPlayProgressValue > PLAY_REFRESH_US)
	{
		refreshPlayProgressValue = 0;

		playProgressValue = (uint16_t)((instrumentPlayer[0].getWavePosition() * ((endPoint - startPoint) / (float)MAX_16BIT)) + startPoint);

		if(zoom.zoomValue == 1.0) playProgressInSpectrum = (600 *  playProgressValue)/MAX_16BIT;
		else if(zoom.zoomValue > 1.0)
		{
			if(playProgressValue < zoom.zoomStart || playProgressValue > zoom.zoomEnd) playProgressInSpectrum = 0;
			else
			{
				playProgressInSpectrum = map(playProgressValue, zoom.zoomStart, zoom.zoomEnd, 0 , 600);
			}
		}

		refreshSpectrumProgress = 1;
	}
}



