

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


//static  uint8_t functPlayAction();
static  uint8_t functRecAction();

static  uint8_t functStepNote(uint8_t value);
static  uint8_t functInstrument(uint8_t state);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static uint8_t functSelectPlace(uint8_t button, uint8_t state);


static uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

//static uint8_t functShift(uint8_t value);



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
		refreshSpectrum = 0;

		if(sampleIsValid)
		{
			showCurrentSpectrum(effector.getLength()/2, effector.getAddress());
		}
		else
		{
			GP.processSpectrum(editorInstrument, &zoom, &spectrum);
		}

		display.refreshControl(spectrumControl);
	}

	if(refreshPoints)
	{
		refreshPoints = 0;

		processPoints();
		display.refreshControl(pointsControl);
	}

	handleTasks(&taskQueue, 0);

	refreshPlayingProgress();
	refreshSampleLoading();
	refreshSampleApplying();
	refreshInstrumentLoading();

	updateEffectProcessing();

	onExitReload();
}

void cSampleEditor::onExitReload()
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

void cSampleEditor::resetAllEffectsState()
{
	for(uint8_t i = 0; i < effectsCount; i++)
	{
		if(effectControl[i].effectStage != eNotAffecting)
		{
			effectControl[i].effectStage = eRequireProcessing;
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
			handleQueueProgress(&taskQueue, progress, "Applying effect");
		}
		else if((moduleFlags & undoActive))
		{
			handleQueueProgress(&taskQueue, progress, "Undoing");
		}
	}
	else if(status == saveDone)
	{
		resetAllEffectsState();

		if(moduleFlags & applyingActive)
		{
			finishTask(&taskQueue, tApply);
		}
		else if(moduleFlags & undoActive)
		{
			finishTask(&taskQueue, tUndo);
		}

		moduleFlags &= ~(applyingActive | undoActive);

		if(currSelEffect == effectCrop)
		{
			resetSpectrumAndPoints();
		}

		refreshSpectrum = 1;
		effector.setSaveStatus(waitingForSaveInit);

		fileManager.setInstrumentChangeFlag(localInstrNum);
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

		if(instrumentPlayer[0].getInterfacePlayingEndFlag())
		{
			instrumentPlayer[0].clearInterfacePlayingEndFlag();

			playProgressValue=0;
			playProgressInSpectrum = 0;
			isPlayingSample = 0;
			refreshSpectrumProgress = 1;
			mtPadBoard.clearVoice(0);

			if((currSelEffect != effectCrop) && (currSelEffect != effectReverse))
			{
				hideEffectsSpectrum();
			}

			moduleFlags &= ~previewRunning;
		}
	}
}

void cSampleEditor::refreshSampleLoading()
{
	if(moduleFlags & sampleLoadingActive)
	{
		sampleLoadedState = fileManager.samplesLoader.waveLoader.getState();

		if(sampleLoadedState == loaderStateTypeInProgress) // refresh update when in progress
		{
			fileManager.samplesLoader.waveLoader.update();

			uint8_t progress = fileManager.samplesLoader.waveLoader.getCurrentWaveProgress();
			handleQueueProgress(&taskQueue, progress, "Loading sample");
		}

		if((sampleLoadedState == loaderStateTypeEnded) && (lastSampleLoadedState == loaderStateTypeInProgress)) // do when loaded
		{
			// after first load change crop and reverse stage
			effectControl[effectCrop].effectStage = eNotAffecting;
			effectControl[effectReverse].effectStage = eNotAffecting;
			processOrPreview(effectControl[effectReverse].effectStage);

			moduleFlags &= ~sampleLoadingActive;
			sampleIsValid = 1;

			finishTask(&taskQueue, tLoadSample);
		}

		lastSampleLoadedState = sampleLoadedState;
	}
}

void cSampleEditor::showCurrentSpectrum(uint32_t length, int16_t *source)
{
	spectrumParams.length = length;
	spectrumParams.address = source;
	GP.processSpectrum(&spectrumParams, &zoom, &spectrum);
}

uint8_t cSampleEditor::startLoadingSample()
{
	uint8_t status = 0;

	if((mtProject.instrument[localInstrNum].isActive) && (mtProject.instrument[localInstrNum].sample.type != mtSampleTypeWavetable))
	{
		if(!(moduleFlags & onEntryStillLoading))
		{
			sprintf(instrumentPath, "Workspace/samples/instr%02d.wav", localInstrNum);
			effector.loadSample(instrumentPath);
			moduleFlags |= sampleLoadingActive;
			status = 1;
		}
	}

	return status;
}

void cSampleEditor::start(uint32_t options)
{
	moduleRefresh = 1;
	sampleIsValid = 0;

	undoCropFlag = 0;
	undoReverseFlag = 0;

	undoRequested = 0;
	applyRequested = 0;

	moduleFlags = 0;
	cancelTaskQueue(&taskQueue);

	lastPreviewEffect = (effect_t)UINT32_MAX;// this will force new calculation on effect on each entry

	zoom.zoomResolution = INT32_MAX;

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

	if((mtProject.values.lastUsedInstrument < INSTRUMENTS_COUNT) && (mtProject.instrument[localInstrNum].sample.type != mtSampleTypeWavetable))
	{
		editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

		GP.processSpectrum(editorInstrument, &zoom, &spectrum);
	}

	localInstrNum = mtProject.values.lastUsedInstrument;

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

	processPoints();

	activateLabelsBorder();

}


void cSampleEditor::initEffectsScreenStructs()
{

	//!< Crop effect
	//
	effectControl[effectCrop].screen = fullSpectrum;

	effectControl[effectCrop].paramNum = 3;
	effectControl[effectCrop].barNum = 0;

	effectControl[effectCrop].bar[1].name = "Start";
	effectControl[effectCrop].bar[1].editFunct = modStartPoint;
	effectControl[effectCrop].bar[1].dataSource = &startPoint;
	effectControl[effectCrop].bar[1].dataFormat = tUNSIGNED16,

	effectControl[effectCrop].bar[2].name = "End";
	effectControl[effectCrop].bar[2].editFunct = modEndPoint;
	effectControl[effectCrop].bar[2].dataSource = &endPoint;
	effectControl[effectCrop].bar[2].dataFormat = tUNSIGNED16,

	effectControl[effectCrop].bar[3].name = "Zoom";
	effectControl[effectCrop].bar[3].editFunct = changeZoom;
	effectControl[effectCrop].bar[3].dataSource = &zoom.zoomValue;
	effectControl[effectCrop].bar[3].dataFormat = tFLOAT,
	effectControl[effectCrop].undoActive = 1;
	effectControl[effectCrop].effectStage = eRequireProcessing;

	//!< Reverse effect
	//
	effectControl[effectReverse].screen = fullSpectrum;

	effectControl[effectReverse].paramNum = 3;
	effectControl[effectReverse].barNum = 0;

	effectControl[effectReverse].bar[1].name = "Start";
	effectControl[effectReverse].bar[1].editFunct = modStartPoint;
	effectControl[effectReverse].bar[1].dataSource = &startPoint;
	effectControl[effectReverse].bar[1].dataFormat = tUNSIGNED16,

	effectControl[effectReverse].bar[2].name = "End";
	effectControl[effectReverse].bar[2].editFunct = modEndPoint;
	effectControl[effectReverse].bar[2].dataSource = &endPoint;
	effectControl[effectReverse].bar[2].dataFormat = tUNSIGNED16,

	effectControl[effectReverse].bar[3].name = "Zoom";
	effectControl[effectReverse].bar[3].editFunct = changeZoom;
	effectControl[effectReverse].bar[3].dataSource = &zoom.zoomValue;
	effectControl[effectReverse].bar[3].dataFormat = tFLOAT,
	effectControl[effectReverse].undoActive = 1;
	effectControl[effectReverse].effectStage = eRequireProcessing;

	//!< Flanger effect
	//
	effectControl[effectFlanger].screen = noSpectrum;

	effectControl[effectFlanger].paramNum = 3;
	effectControl[effectFlanger].barNum = 3;

	effectControl[effectFlanger].bar[1].name = "Offset";
	effectControl[effectFlanger].bar[1].editFunct = editFlangerOffset;
	effectControl[effectFlanger].bar[1].dataSource = &flangerOffset;
	effectControl[effectFlanger].bar[1].dataFormat = tUNSIGNED8,

	effectControl[effectFlanger].bar[2].name = "Depth";
	effectControl[effectFlanger].bar[2].editFunct = editFlangerDepth;
	effectControl[effectFlanger].bar[2].dataSource = &flangerDepth;
	effectControl[effectFlanger].bar[2].dataFormat = tUNSIGNED8,

	effectControl[effectFlanger].bar[3].name = "Delay";
	effectControl[effectFlanger].bar[3].editFunct = editFlangerDelay;
	effectControl[effectFlanger].bar[3].dataSource = &flangerDelay;
	effectControl[effectFlanger].bar[3].dataFormat = tFLOAT,
	effectControl[effectFlanger].undoActive = 0;
	effectControl[effectFlanger].effectStage = eRequireProcessing;

	//!< Chorus effect
	//
	effectControl[effectChorus].screen =  noSpectrum;

	effectControl[effectChorus].paramNum = 2;
	effectControl[effectChorus].barNum = 2;

	effectControl[effectChorus].bar[2].name = "Length";
	effectControl[effectChorus].bar[2].editFunct = editChorusLength;
	effectControl[effectChorus].bar[2].dataSource = &chorusLength;
	effectControl[effectChorus].bar[2].dataFormat = tUNSIGNED16;

	effectControl[effectChorus].bar[3].name = "Strength";
	effectControl[effectChorus].bar[3].editFunct = editChorusStrength;
	effectControl[effectChorus].bar[3].dataSource = &chorusStrength;
	effectControl[effectChorus].bar[3].dataFormat = tUNSIGNED8;
	effectControl[effectChorus].undoActive = 0;
	effectControl[effectChorus].effectStage = eRequireProcessing;


	//!< Delay effect
	//
	effectControl[effectDelay].screen =  noSpectrum;

	effectControl[effectDelay].paramNum = 2;
	effectControl[effectDelay].barNum = 2;

	effectControl[effectDelay].bar[2].name = "Time";
	effectControl[effectDelay].bar[2].editFunct = editDelayTime;
	effectControl[effectDelay].bar[2].dataSource = &delayTime;
	effectControl[effectDelay].bar[2].dataFormat = tUNSIGNED16;
	effectControl[effectDelay].bar[2].dataUnit = "ms";

	effectControl[effectDelay].bar[3].name = "Feedback";
	effectControl[effectDelay].bar[3].editFunct = editDelayFeedback;
	effectControl[effectDelay].bar[3].dataSource = &delayFeedback;
	effectControl[effectDelay].bar[3].dataFormat = tFLOAT;
	effectControl[effectDelay].undoActive = 0;
	effectControl[effectDelay].effectStage = eRequireProcessing;

	//!< Compressor effect
	//
	effectControl[effectCompressor].screen =  noSpectrum;

	effectControl[effectCompressor].paramNum = 4;
	effectControl[effectCompressor].barNum = 4;

	effectControl[effectCompressor].bar[0].name = "Threshold";
	effectControl[effectCompressor].bar[0].editFunct = editCompressorThreshold;
	effectControl[effectCompressor].bar[0].dataSource = &compressorThrs;
	effectControl[effectCompressor].bar[0].dataFormat = tUNSIGNED16;

	effectControl[effectCompressor].bar[1].name = "Ratio";
	effectControl[effectCompressor].bar[1].editFunct = editCompressorRatio;
	effectControl[effectCompressor].bar[1].dataSource = &compressorRatio;
	effectControl[effectCompressor].bar[1].dataFormat = tUNSIGNED16;

	effectControl[effectCompressor].bar[2].name = "Attack";
	effectControl[effectCompressor].bar[2].editFunct = editCompressorAttack;
	effectControl[effectCompressor].bar[2].dataSource = &compressorAttack;
	effectControl[effectCompressor].bar[2].dataFormat = tUNSIGNED16;

	effectControl[effectCompressor].bar[3].name = "Release";
	effectControl[effectCompressor].bar[3].editFunct = editCompressorRelease;;
	effectControl[effectCompressor].bar[3].dataSource = &compressorRelease;
	effectControl[effectCompressor].bar[3].dataFormat = tUNSIGNED16;
	effectControl[effectCompressor].undoActive = 0;
	effectControl[effectCompressor].effectStage = eRequireProcessing;

	//!< Bitcrusher effect
	//
	effectControl[effectBitcrusher].screen =  noSpectrum;

	effectControl[effectBitcrusher].paramNum = 2;
	effectControl[effectBitcrusher].barNum = 2;

	effectControl[effectBitcrusher].bar[2].name = "Bits";
	effectControl[effectBitcrusher].bar[2].editFunct = editBitcrusherBits;
	effectControl[effectBitcrusher].bar[2].dataSource = &bitcrusherBits;
	effectControl[effectBitcrusher].bar[2].dataFormat = tUNSIGNED8;

	effectControl[effectBitcrusher].bar[3].name = "Rate";
	effectControl[effectBitcrusher].bar[3].editFunct = editBitcrusherRate;
	effectControl[effectBitcrusher].bar[3].dataSource = &bitcrusherRate;
	effectControl[effectBitcrusher].bar[3].dataFormat = tUNSIGNED16;
	effectControl[effectBitcrusher].undoActive = 0;
	effectControl[effectBitcrusher].effectStage = eRequireProcessing;

	//!< Amplifier effect
	//
	effectControl[effectAmplifier].screen =  noSpectrum;

	effectControl[effectAmplifier].paramNum = 1;
	effectControl[effectAmplifier].barNum = 1;

	effectControl[effectAmplifier].bar[3].name = "Amp";
	effectControl[effectAmplifier].bar[3].editFunct = editAmplifierAmp;
	effectControl[effectAmplifier].bar[3].dataSource = &amplifierAmp;
	effectControl[effectAmplifier].bar[3].dataFormat = tFLOAT;

	effectControl[effectAmplifier].undoActive = 0;
	effectControl[effectAmplifier].effectStage = eRequireProcessing;

	//!< Limiter effect
	//
	effectControl[effectLimiter].screen = noSpectrum;

	effectControl[effectLimiter].paramNum = 3;
	effectControl[effectLimiter].barNum = 3;

	effectControl[effectLimiter].bar[1].name = "Threshold";
	effectControl[effectLimiter].bar[1].editFunct = editLimiterThreshold;
	effectControl[effectLimiter].bar[1].dataSource = &limiterThreshold;
	effectControl[effectLimiter].bar[1].dataFormat = tUNSIGNED16,

	effectControl[effectLimiter].bar[2].name = "Attack";
	effectControl[effectLimiter].bar[2].editFunct = editLimiterAttack;
	effectControl[effectLimiter].bar[2].dataSource = &limiterAttack;
	effectControl[effectLimiter].bar[2].dataFormat = tUNSIGNED16,

	effectControl[effectLimiter].bar[3].name = "Release";
	effectControl[effectLimiter].bar[3].editFunct = editLimiterRelease;
	effectControl[effectLimiter].bar[3].dataSource = &limiterRelease;
	effectControl[effectLimiter].bar[3].dataFormat = tUNSIGNED16,
	effectControl[effectLimiter].undoActive = 0;
	effectControl[effectLimiter].effectStage = eRequireProcessing;

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

void cSampleEditor::showEffectSpectrum()
{
	if(newSpectrumFlag== 0)
	{
		newSpectrumFlag = 1;
		effectSpectrumParams.length = effector.getEffectLength();
		effectSpectrumParams.address = effector.getEffectAddress();
		GP.processSpectrum(&effectSpectrumParams, &effectZoom, &effectSpectrum);
	}

	showPreviewSpectrum();
}

void cSampleEditor::hideEffectsSpectrum()
{
	hidePreviewSpectrum();
}

void cSampleEditor::playPreview(uint8_t pad)
{
	if(sampleIsValid)
	{
		if(mtPadBoard.getEmptyVoice() < 0) return;

		if(mtPadBoard.getEmptyVoice() == 0)
		{
			if((currSelEffect != effectCrop) && (currSelEffect != effectReverse))
			{
				showEffectSpectrum();
			}

			moduleFlags |= previewRunning;

			playPitch = notes[mtPadBoard.convertPadToNote(pad)];

			if((currSelEffect == effectCrop) || (currSelEffect == effectReverse))
			{
				playProgresValueTim = ((((effector.getLength()/44100.0 ) * startPoint) / MAX_16BIT) * 1000000) / playPitch;
			}
			else
			{
				playProgresValueTim = 0;
			}

			refreshPlayProgressValue = 0;
			isPlayingSample = 1;

			instrumentPlayer[0].clearInterfacePlayingEndFlag();
		}

		if((currSelEffect == effectCrop) || (currSelEffect == effectReverse))
		{
			effector.play(startPoint, endPoint , pad);
		}
		else
		{
			effector.playPrev(pad);
		}


		if(effectControl[currSelEffect].effectStage != eNotAffecting)
		{
			lastPreviewEffect = (effect_t)currSelEffect;
		}
	}
}

void cSampleEditor::stopPreview(uint8_t pad)
{
	if(isPlayingSample)
	{
		if(mtPadBoard.getVoiceTakenByPad(pad) == 0)
		{
			if((currSelEffect != effectCrop) && (currSelEffect != effectReverse))
			{
				hideEffectsSpectrum();
			}

			moduleFlags &= ~previewRunning;

			playProgressValue=0;
			playProgressInSpectrum = 0;
			isPlayingSample = 0;
			refreshSpectrumProgress = 1;
		}
	}

	effector.stop(pad);
}

static uint8_t functPreview(uint8_t state)
{
	if(state > buttonPress) return 1;

	if((state == 1) && (SE->taskQueue.taskQueueActive == 0))
	{
		SE->previewButtonState = state;

		if(!SE->handleTasks(&SE->taskQueue, 1))
		{
			if(SE->effectControl[SE->currSelEffect].effectStage >= eProcessed)
			{
				SE->playPreview(12);
			}
		}
	}
	else if(state == 0)
	{
		SE->previewButtonState = state;
		SE->stopPreview(12);
	}

	return 1;
}

void cSampleEditor::applyEffect()
{
	applyRequested =0;

	sprintf(instrumentPath, "Workspace/samples/instr%02d.wav", localInstrNum);

	if(currSelEffect == effectCrop)
	{
		effector.trim(startPoint, endPoint);
		undoCropFlag = 1;

	}
	else if(currSelEffect == effectReverse)
	{
		effector.reverse(startPoint, endPoint);
		refreshSpectrum = 1;
		undoReverseFlag = 1;
	}
	else
	{
		undoReverseFlag = 0;
		undoCropFlag = 0;
		effector.setEffects();
	}

	effector.save(instrumentPath);

	if(effectControl[currSelEffect].effectStage != eNotAffecting)
	{
		effectControl[currSelEffect].effectStage = eRequireProcessing;
		processOrPreview(effectControl[currSelEffect].effectStage);
	}


	moduleFlags |= applyingActive;
	effectAppliedFlag = 1;
}

static uint8_t functApply()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->taskQueue.taskQueueActive) return 1;

	SE->applyRequested = 1;
	SE->handleTasks(&SE->taskQueue, 1);

	return 1;
}

void cSampleEditor::undoEffect()
{
	undoRequested = 0;
	if(currSelEffect == effectCrop)
	{
		effector.undoTrim();
	}
	else if(currSelEffect == effectReverse)
	{
		effector.undoReverse();
	}

	effector.save(SE->instrumentPath);

	moduleFlags |= undoActive;
}

static uint8_t functUndo()
{
	if(SE->undoCropFlag || SE->undoReverseFlag)
	{
		SE->undoCropFlag = 0;
		SE->undoReverseFlag = 0;

		SE->undoRequested = 1;

		SE->handleTasks(&SE->taskQueue, 1);
	}

	return 1;
}

static uint8_t changeEffect(uint8_t button)
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->taskQueue.taskQueueActive) return 1;

	SE->clearAllNodes();
	SE->cancelMultiFrame();

	if(SE->selectedPlace == 6)
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

	SE->points.selected = 0;
	SE->refreshPoints = 1;

	SE->selectedPlace = 6;
	SE->activateLabelsBorder();

	return 1;
}

static uint8_t functSelectPlace(uint8_t button , uint8_t state)
{
	if(state > buttonPress) return 1;
	if(SE->moduleFlags != 0) return 1;
	if(SE->taskQueue.taskQueueActive) return 1;

	uint8_t parameterFlag = 1;

	if(button == interfaceButton2 && state == buttonPress)// Undo
	{
		if(SE->effectControl[SE->currSelEffect].undoActive)
		{
			parameterFlag = 0;
			functUndo();
		}
	}

	if(parameterFlag)
	{
		uint8_t minSelectedPlace = 0;
		if(SE->effectControl[SE->currSelEffect].paramNum == 4)
		{
			minSelectedPlace = 2;
		}
		else if(SE->effectControl[SE->currSelEffect].paramNum == 3)
		{
			minSelectedPlace = 3;
		}
		else if(SE->effectControl[SE->currSelEffect].paramNum == 2)
		{
			minSelectedPlace = 4;
		}
		else if(SE->effectControl[SE->currSelEffect].paramNum == 1)
		{
			minSelectedPlace = 5;
		}

		if(state == buttonPress)
		{
			if(button >= minSelectedPlace)
			{
				SE->selectedPlace = button;

				SE->addNode(SE->effectControl[SE->currSelEffect].bar[button - 2].editFunct, button - 2);

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
	if(SE->taskQueue.taskQueueActive) return 1;

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
	if(effectControl[currSelEffect].bar[paramNum].editFunct != NULL)
	{
		effectControl[currSelEffect].bar[paramNum].effectPercentage = effectControl[currSelEffect].bar[paramNum].editFunct(value);

		if(effectControl[currSelEffect].effectStage != eNotAffecting)
		{
			effectControl[currSelEffect].effectStage = eRequireProcessing;
		}

		updateEffectValues(&effectControl[currSelEffect], paramNum);
		processOrPreview(effectControl[currSelEffect].effectStage);
	}
}

void cSampleEditor::editParamFunctionSelection(int16_t value)
{
	selection_percentages temp;

	temp = stepThroughNodes(value);

	for(uint8_t i = 0; i < MAX_DATA_BARS; i++)
	{
		if(temp.mask & (1 << i))
		{
			effectControl[currSelEffect].bar[i].effectPercentage = temp.percentages[i];

			if(effectControl[currSelEffect].effectStage != eNotAffecting)
			{
				effectControl[currSelEffect].effectStage = eRequireProcessing;
			}

			updateEffectValues(&effectControl[currSelEffect], i);
			processOrPreview(effectControl[currSelEffect].effectStage);
		}
	}
}

static  uint8_t functLeft()
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->taskQueue.taskQueueActive) return 1;
	if(SE->frameData.multiSelActiveNum != 0) return 1;

	if(SE->selectedPlace > ((SE->frameData.placesCount - 1) - SE->effectControl[SE->currSelEffect].paramNum))
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
	if(SE->taskQueue.taskQueueActive) return 1;
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
	if(SE->taskQueue.taskQueueActive) return 1;

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
	if(SE->taskQueue.taskQueueActive) return 1;

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

/*static  uint8_t functPlayAction()
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
}*/

static  uint8_t functRecAction()
{


	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(SE->onExitFlag == 1) return 1;

	if(!(SE->moduleFlags & onExitReloadActive) && !(SE->moduleFlags & applyingActive) && !(SE->moduleFlags & undoActive))
	{
		SE->cancelTaskQueue(&SE->taskQueue);
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
		SE->effectAppliedFlag = 0;
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

	showEffectScreen(&effectControl[currSelEffect]);
}

/*static uint8_t functShift(uint8_t value)
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
}*/


static  uint8_t functInstrument(uint8_t state)
{
	if(SE->moduleFlags != 0) return 1;
	if(SE->taskQueue.taskQueueActive) return 1;

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
	if(!(SE->taskQueue.taskQueueActive)) return 1;

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
	if(state > 1) return 1;
	if(SE->taskQueue.taskQueueActive) return 1;

	if(state == 1)
	{
		if(SE->effectControl[SE->currSelEffect].effectStage >= eProcessed)
		{
			padsBacklight.setFrontLayer(1,20, pad);
			SE->playPreview(pad);
		}
	}
	else if(state == 0)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		SE->stopPreview(pad);
	}

	return 1;
}

void cSampleEditor::updateEffectValues(effect_handle_t *effect, uint8_t barNum)
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
			sprintf(&dataBarText[whichBar][0], format, *((uint8_t*)data));
			break;
		case tUNSIGNED16:
			sprintf(&dataBarText[whichBar][0], format, *((uint16_t*)data));
			break;
		case tUNSIGNED32:
			sprintf(&dataBarText[whichBar][0], format, *((uint32_t*)data));
			break;
		case tSIGNED8:
			sprintf(&dataBarText[whichBar][0], format, *((int8_t*)data));
			break;
		case tSIGNED16:
			sprintf(&dataBarText[whichBar][0], format, *((int16_t*)data));
			break;
		case tSIGNED32:
			sprintf(&dataBarText[whichBar][0], format, *((int32_t*)data));
			break;
		case tFLOAT:
			strncpy(format,"%.2f",6);
			sprintf(&dataBarText[whichBar][0], format, *((float_t*)data));
			break;
		}

		if(*unit != '\0')
		{
			strcat(&dataBarText[whichBar][0],unit);
		}
	}
	else
	{
		memset(&dataBarText[whichBar][0],0,6);
	}
}

void cSampleEditor::resetSpectrumAndPoints()
{
	startPoint = 0;
	endPoint = MAX_16BIT;
	points.endPoint = endPoint;
	points.startPoint = startPoint;

	GP.spectrumResetZoom(startPoint, effector.getLength()/2 ,&zoom);

	updateEffectValues(&effectControl[currSelEffect], 1);
	updateEffectValues(&effectControl[currSelEffect], 2);
	updateEffectValues(&effectControl[currSelEffect], 3);

	refreshSpectrum = 1;
	refreshPoints = 1;
}

void cSampleEditor::makeEffect()
{
	if(moduleFlags != 0) return;
	if(effectControl[currSelEffect].effectStage == eProcessed) return;


	if(effectControl[currSelEffect].effectStage != eNotAffecting)// if last used effect is not affecting buffer we dont change it's state
	{
		if(lastPreviewEffect < effectsCount)
		{
			effectControl[lastPreviewEffect].effectStage = eRequireProcessing; // last effect is no longer valid
		}
	}

	uint8_t processingActivated = 0;

	if(effectControl[currSelEffect].effectStage == eRequireProcessing)
	{
		switch((effect_t)currSelEffect)
		{
		case effectCrop:
			break;
		case effectReverse:
			break;
		case effectFlanger:
			processingActivated = effectorFlanger.makeFlanger(FLANGER_LENGTH_MAX, flangerOffset, flangerDepth, flangerDelay);
			break;
		case effectChorus:
			processingActivated = effectorChorus.makeChorus(chorusLength, chorusStrength);
			break;
		case effectDelay:
			processingActivated = effectorDelay.makeDelay(delayFeedback, delayTime);
			break;
		case effectCompressor:
			processingActivated = effectorCompressor.makeCompressor(compressorThrs, compressorRatio, compressorAttack, compressorRelease);
			break;
		case effectBitcrusher:
			processingActivated = effectorBitcrusher.makeBitcrusher(bitcrusherBits, bitcrusherRate);
			break;
		case effectAmplifier:
			effectorAmplifier.makeAmplifier(amplifierAmp);// instant efffect, no background calculations
			effectControl[currSelEffect].effectStage = eProcessed;
			break;
		case effectLimiter:
			processingActivated = effectorLimiter.makeLimiter(limiterThreshold, limiterAttack, limiterRelease);
			break;
		default:
			break;
		}
	}

	if(processingActivated)
	{
		newSpectrumFlag = 0;
		moduleFlags |= processingActive;
		//SE->showProcessingBar(0);
	}
	else
	{
		newSpectrumFlag = 0;
		processOrPreview(effectControl[currSelEffect].effectStage);
		handleQueueProgress(&taskQueue, 100, "Processing");
		finishTask(&taskQueue, tProcessEffect);
	}
}

void cSampleEditor::updateEffectProcessing()
{
	if(moduleFlags & processingActive)
	{
		if(effectControl[currSelEffect].effectStage == eRequireProcessing)
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
				handleQueueProgress(&taskQueue, progress,"Processing effect");
			}
			else
			{
				moduleFlags &= ~processingActive;
				effectControl[currSelEffect].effectStage = eProcessed;

				processOrPreview(effectControl[currSelEffect].effectStage);

				finishTask(&taskQueue, tProcessEffect);
			}
		}
	}
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
					temp.mask |= (1 << node);
					temp.percentages[node] = selectNodes[node].editFunct(value);
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
					temp.mask |= (1 << (node-1));
					temp.percentages[node-1] = selectNodes[node-1].editFunct(value);
				}
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

		strZoomParams localZoom;

		uint16_t tempEndPoint;
		uint16_t tempStartPoint;

		if((SE->currSelEffect == effectCrop) || (SE->currSelEffect == effectReverse))
		{
			localZoom = zoom;
			tempEndPoint = endPoint;
			tempStartPoint = startPoint;
		}
		else
		{
			localZoom = effectZoom;
			tempEndPoint = MAX_16BIT;
			tempStartPoint = 0;
		}

		playProgressValue = (uint16_t)((instrumentPlayer[0].getWavePosition() * ((tempEndPoint - tempStartPoint) / (float)MAX_16BIT)) + tempStartPoint);

		if(localZoom.zoomValue == 1.0) playProgressInSpectrum = (600 *  playProgressValue)/MAX_16BIT;
		else if(localZoom.zoomValue > 1.0)
		{
			if(playProgressValue < localZoom.zoomStart || playProgressValue > localZoom.zoomEnd) playProgressInSpectrum = 0;
			else
			{
				playProgressInSpectrum = map(playProgressValue, localZoom.zoomStart, localZoom.zoomEnd, 0 , 600);
			}
		}

		refreshSpectrumProgress = 1;
	}
}

uint8_t cSampleEditor::checkPreConditions(taskQueue_t *queue)
{
	queue->taskQueue = 0;
	queue->tasksInQueue = 0;

	if(!sampleIsValid)
	{
		queue->taskQueue |= tLoadSample;
		queue->tasksInQueue++;
	}

	if(effectControl[currSelEffect].effectStage == eRequireProcessing)
	{
		if(currSelEffect > 1)// wszystkie oprocz crop i reverse kolejkuja processing
		{
			queue->taskQueue |= tProcessEffect;
			queue->tasksInQueue++;
		}
	}

	if(applyRequested)
	{
		queue->taskQueue |= tApply;
		queue->tasksInQueue++;
	}

	if(undoRequested)
	{
		queue->taskQueue |= tUndo;
		queue->tasksInQueue++;
	}

	return queue->tasksInQueue;
}

void cSampleEditor::cancelTaskQueue(taskQueue_t *queue)
{
	memset(queue, 0, sizeof(taskQueue_t));
}

uint8_t cSampleEditor::handleTasks(taskQueue_t *queue, uint8_t force)
{
	if((queue->taskQueueActive == 0) && force)
	{
		cancelTaskQueue(queue);
		checkPreConditions(queue);

		if(queue->tasksInQueue)
		{
			queue->taskQueueActive = 1;
		}

		queue->taskQueueNextFlag = 1;
	}

	if((queue->taskQueueActive == 1) && (queue->taskQueueNextFlag == 1))
	{
		queue->taskQueueNextFlag = 0;
		queue->currentTask++;

		if(queue->taskQueue & tLoadSample)
		{
			if(!startLoadingSample())
			{
				cancelTaskQueue(queue);
			}
		}
		else if(queue->taskQueue & tProcessEffect)
		{
			makeEffect();
		}
		else if(queue->taskQueue & tApply)
		{
			applyEffect();
		}
		else if(queue->taskQueue & tUndo)
		{
			undoEffect();
		}
	}

	return queue->taskQueueActive;
}

void cSampleEditor::finishTask(taskQueue_t *queue, tasks_t task)
{
	queue->taskQueue &= ~task;

	if(queue->taskQueue)
	{
		queue->taskQueueNextFlag = 1;
	}
	else
	{
		hideHorizontalBar();
		cancelTaskQueue(queue);

		if(previewButtonState)
		{
			playPreview(12);
		}
	}
}

void cSampleEditor::handleQueueProgress(taskQueue_t *queue, uint8_t taskProgress, const char *text)
{
	uint8_t progressBase = (queue->currentTask - 1) * ( 100 / queue->tasksInQueue);
	uint8_t tempProgress = progressBase + (taskProgress / queue->tasksInQueue);

	queue->taskQueueProgress += (tempProgress - queue->taskQueueProgress);

	showQueueHorizontalBar(queue->taskQueueProgress, text);
}

//------------------- EDIT FUNCTIONS --------------------
//
//
static uint8_t changeZoom(int16_t value)
{
	if(SE->sampleIsValid)
	{
		GP.spectrumChangeZoom(value, effector.getLength()/2, &SE->zoom);
	}
	else
	{
		GP.spectrumChangeZoom(value, SE->editorInstrument->sample.length, &SE->zoom);
	}

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

	if(SE->startPoint >= SE->endPoint)
	{
		SE->startPoint = SE->endPoint - 2;
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

	if(SE->endPoint <= SE->startPoint)
	{
		SE->endPoint = SE->startPoint + 2;
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



