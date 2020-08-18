#include "mtAudioEngine.h"
#include "sampleRecorder/sampleRecorder.h"
#include "mtStructs.h"
#include "metronomeBeep.h"
extern AudioControlSGTL5000 audioShield;
static cSampleRecorder* SR = &sampleRecorder;



AudioInputI2S            i2sIn;
AudioOutputI2S           i2sOut;
AudioRecordQueue         queue;

envelopeGenerator		 envelopeFilter[8];
envelopeGenerator		 envelopeWtPosition[8];
envelopeGenerator		 envelopeGranPosition[8];
envelopeGenerator		 envelopePanning[8];
envelopeGenerator		 envelopeFinetune[8];

AudioPlaySdWav           playSdWav;
AudioPlaySdWavFloat		 playSdWavFloat;
AudioPlaySdWav24bit 	 playSdWav24Bit;

AudioPlayMemory          playMem[8];
AudioAmplifier           amp[8];
AudioFilterStateVariable filter[8];
AudioAnalyzeRMS			 trackRMS[8];
AudioEffectShortDelay	 shortDelay;
AudioAnalyzeRMS			 rmsDelay[2];
AudioEffectLimiter		 limiter[2];
AudioBitDepth			 bitDepthControl[2];
AudioEffectPolyverb		 polyverb;
AudioAnalyzeRMS			 rmsPolyverb[2];

AudioMixer10			 mixerL,mixerR,mixerDelay,mixerReverb;
AudioMixer4              mixerRec;
AudioMixer10             mixerSourceL,mixerSourceR;

AudioAnalyzeRMS			 inputRMS;
AudioRecordQueue		 exportL, exportR;
AudioAnalyzeRMS			 exportRmsL, exportRmsR;
AudioSynthWaveform		 testWaveform;
AudioPlayMemory 		 metronomeTick;

AudioConnection          connectPlayMemToFilter1(&playMem[0], 0, &filter[0], 0);
AudioConnection          connectPlayMemToFilter2(&playMem[1], 0, &filter[1], 0);
AudioConnection          connectPlayMemToFilter3(&playMem[2], 0, &filter[2], 0);
AudioConnection          connectPlayMemToFilter4(&playMem[3], 0, &filter[3], 0);
AudioConnection          connectPlayMemToFilter5(&playMem[4], 0, &filter[4], 0);
AudioConnection          connectPlayMemToFilter6(&playMem[5], 0, &filter[5], 0);
AudioConnection          connectPlayMemToFilter7(&playMem[6], 0, &filter[6], 0);
AudioConnection          connectPlayMemToFilter8(&playMem[7], 0, &filter[7], 0);

AudioConnection          connectFilterToAmp1(&filter[0], 0, &amp[0], 0);
AudioConnection          connectFilterToAmp2(&filter[1], 0, &amp[1], 0);
AudioConnection          connectFilterToAmp3(&filter[2], 0, &amp[2], 0);
AudioConnection          connectFilterToAmp4(&filter[3], 0, &amp[3], 0);
AudioConnection          connectFilterToAmp5(&filter[4], 0, &amp[4], 0);
AudioConnection          connectFilterToAmp6(&filter[5], 0, &amp[5], 0);
AudioConnection          connectFilterToAmp7(&filter[6], 0, &amp[6], 0);
AudioConnection          connectFilterToAmp8(&filter[7], 0, &amp[7], 0);

AudioConnection          connectAmpToMixerL1(&amp[0], 0, &mixerL, 0);
AudioConnection          connectAmpToMixerL2(&amp[1], 0, &mixerL, 1);
AudioConnection          connectAmpToMixerL3(&amp[2], 0, &mixerL, 2);
AudioConnection          connectAmpToMixerL4(&amp[3], 0, &mixerL, 3);
AudioConnection          connectAmpToMixerL5(&amp[4], 0, &mixerL, 4);
AudioConnection          connectAmpToMixerL6(&amp[5], 0, &mixerL, 5);
AudioConnection          connectAmpToMixerL7(&amp[6], 0, &mixerL, 6);
AudioConnection          connectAmpToMixerL8(&amp[7], 0, &mixerL, 7);

AudioConnection          connectAmpToMixerR1(&amp[0], 0, &mixerR, 0);
AudioConnection          connectAmpToMixerR2(&amp[1], 0, &mixerR, 1);
AudioConnection          connectAmpToMixerR3(&amp[2], 0, &mixerR, 2);
AudioConnection          connectAmpToMixerR4(&amp[3], 0, &mixerR, 3);
AudioConnection          connectAmpToMixerR5(&amp[4], 0, &mixerR, 4);
AudioConnection          connectAmpToMixerR6(&amp[5], 0, &mixerR, 5);
AudioConnection          connectAmpToMixerR7(&amp[6], 0, &mixerR, 6);
AudioConnection          connectAmpToMixerR8(&amp[7], 0, &mixerR, 7);


AudioConnection          connectAmpToRMS1(&amp[0], &trackRMS[0]);
AudioConnection          connectAmpToRMS2(&amp[1], &trackRMS[1]);
AudioConnection          connectAmpToRMS3(&amp[2], &trackRMS[2]);
AudioConnection          connectAmpToRMS4(&amp[3], &trackRMS[3]);
AudioConnection          connectAmpToRMS5(&amp[4], &trackRMS[4]);
AudioConnection          connectAmpToRMS6(&amp[5], &trackRMS[5]);
AudioConnection          connectAmpToRMS7(&amp[6], &trackRMS[6]);
AudioConnection          connectAmpToRMS8(&amp[7], &trackRMS[7]);


AudioConnection          connectFilterToMixerDelay1(&filter[0], 0, &mixerDelay, 0);
AudioConnection          connectFilterToMixerDelay2(&filter[1], 0, &mixerDelay, 1);
AudioConnection          connectFilterToMixerDelay3(&filter[2], 0, &mixerDelay, 2);
AudioConnection          connectFilterToMixerDelay4(&filter[3], 0, &mixerDelay, 3);
AudioConnection          connectFilterToMixerDelay5(&filter[4], 0, &mixerDelay, 4);
AudioConnection          connectFilterToMixerDelay6(&filter[5], 0, &mixerDelay, 5);
AudioConnection          connectFilterToMixerDelay7(&filter[6], 0, &mixerDelay, 6);
AudioConnection          connectFilterToMixerDelay8(&filter[7], 0, &mixerDelay, 7);


AudioConnection          connectFilterToMixerReverb1(&filter[0], 0, &mixerReverb, 0);
AudioConnection          connectFilterToMixerReverb2(&filter[1], 0, &mixerReverb, 1);
AudioConnection          connectFilterToMixerReverb3(&filter[2], 0, &mixerReverb, 2);
AudioConnection          connectFilterToMixerReverb4(&filter[3], 0, &mixerReverb, 3);
AudioConnection          connectFilterToMixerReverb5(&filter[4], 0, &mixerReverb, 4);
AudioConnection          connectFilterToMixerReverb6(&filter[5], 0, &mixerReverb, 5);
AudioConnection          connectFilterToMixerReverb7(&filter[6], 0, &mixerReverb, 6);
AudioConnection          connectFilterToMixerReverb8(&filter[7], 0, &mixerReverb, 7);

AudioConnection          connectMixerToDelay(&mixerDelay,&shortDelay);

AudioConnection          connectMixSendReverbL(&mixerReverb,0,&polyverb,0);
AudioConnection          connectMixSendReverbR(&mixerReverb,0,&polyverb,1);

AudioConnection          connectDelayToMixerL(&shortDelay, 0, &mixerL, 8);
AudioConnection          connectDelayToMixerR(&shortDelay, 1, &mixerR, 8);
AudioConnection			 connectDelayToRMS1(&shortDelay, 0, &rmsDelay[0], 0);
AudioConnection			 connectDelayToRMS2(&shortDelay, 1, &rmsDelay[1], 0);

AudioConnection          connectReverbToMixerL(&polyverb,0,&mixerL,9);
AudioConnection          connectReverbToMixerR(&polyverb,1,&mixerR,9);
AudioConnection			 connectReverbToRMS1(&polyverb, 0, &rmsPolyverb[0], 0);
AudioConnection			 connectReverbToRMS2(&polyverb, 1, &rmsPolyverb[1], 0);

AudioConnection          connectMirerLtoBitDepth(&mixerL, &bitDepthControl[0]);
AudioConnection          connectMirerRtoBitDepth(&mixerR, &bitDepthControl[1]);

AudioConnection          connectBitDepthToLimiter1(&bitDepthControl[0], 0, &limiter[0], 0);
AudioConnection          connectBitDepthToLimiter2(&bitDepthControl[1], 0, &limiter[1], 0);

AudioConnection          connectLimiterToMixerSourceL(&limiter[0], 0, &mixerSourceL, 0);
AudioConnection          connectLimiterToMixerSourceR(&limiter[1], 0, &mixerSourceR, 0);

AudioConnection          connectPlaySdWavToMixerSourceL(&playSdWav, 0, &mixerSourceL, 1);
AudioConnection          connectPlaySdWavToMixerSourceR(&playSdWav, 0, &mixerSourceR, 1);

AudioConnection 		 connectPlaySdWavFloatToMixerSourceL(&playSdWavFloat,0,&mixerSourceL,2);
AudioConnection 		 connectPlaySdWavFloatToMixerSourceR(&playSdWavFloat,0,&mixerSourceR,2);

AudioConnection 		 connectPlaySdWav24bitToMixerSourceL(&playSdWav24Bit,0,&mixerSourceL,3);
AudioConnection 		 connectPlaySdWav24bitToMixerSourceR(&playSdWav24Bit,0,&mixerSourceR,3);

AudioConnection          connectTestWaveformToMixerSourceR(&testWaveform, 0, &mixerSourceR, 4);
AudioConnection          connectTestWaveformToMixerSourceL(&testWaveform, 0, &mixerSourceL, 4);

AudioConnection          connectMetronomeTickToMixerSourceR(&metronomeTick, 0, &mixerSourceR, 5);
AudioConnection          connectMetronomeTickToMixerSourceL(&metronomeTick, 0, &mixerSourceL, 5);

AudioConnection          connectMixerSourceRtoI2S(&mixerSourceR, 0, &i2sOut, 0);
AudioConnection          connectMixerSourceLtoI2S(&mixerSourceL, 0, &i2sOut, 1);
//**************** export
AudioConnection          connectMixerSourceLtoExport(&mixerSourceL, &exportL);
AudioConnection          connectMixerSourceRtoExport(&mixerSourceR, &exportR);

AudioConnection          connectMixerSourceLtoRMS(&mixerSourceL, &exportRmsL);
AudioConnection          connectMixerSourceRtoRMS(&mixerSourceR, &exportRmsR);

//**************** recording
AudioConnection          connectI2StoREC1(&i2sIn, 0, &mixerRec, 0);
AudioConnection          connectI2StoREC2(&i2sIn, 1, &mixerRec, 1);


AudioConnection          connectMixerRecToQueue(&mixerRec, &queue);
AudioConnection          connectMixerRecToRMS(&mixerRec, &inputRMS);

IntervalTimer updateTimer;

playerEngine instrumentPlayer[8];
audioEngine engine;

uint8_t isCurrentLoadInstrument[48];



constexpr uint16_t RELEASE_NOTE_ON_VAL = 1;

void updateAudioEngine();

void audioEngine::startTestSignal(float amp, float freq)
{
	testWaveform.amplitude(amp);
	testWaveform.frequency(freq);
}

void audioEngine::stopTestSignal()
{
	testWaveform.amplitude(0.0);
}
// blokowanie odswiezana delaya
void audioEngine::blockDelayRefresh()
{
	shortDelay.blockUpdate();
}
// odblokowanie odswiezana delaya
void audioEngine::unblockDelayRefresh()
{
	shortDelay.unblockUpdate();
}
void audioEngine::setPassEnvelope(uint8_t state)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setPassEnvelope(state);
	}
}

void audioEngine::init()
{
	mixerSourceR.gain(0,1.0);
	mixerSourceL.gain(0,1.0);
	for(uint8_t i = 1; i < 4 ; i ++)
	{
		mixerSourceR.gain(i,ampLogValues[50]);
		mixerSourceL.gain(i,ampLogValues[50]);
	}

//	filterReverbOut.setType(filterType::lowPass);
//	filterReverbOut.setCutoff(1.0);
//	filterReverbOut.connect();
	shortDelay.begin(mtProject.values.delayFeedback, mtProject.values.delayTime,
			mtProject.values.delayParams & 0b10000000,mtProject.values.delayParams & 0b01000000,
			mtProject.values.delayParams & 0b00111111);

	audioShield.volume(mtConfig.audioCodecConfig.volume/100.0);
	audioShield.inputSelect(AUDIO_INPUT_LINEIN);
	mtConfig.audioCodecConfig.inSelect = inputSelectLineIn;

	audioShield.lineInLevel(3);
	limiter[0].begin(mtProject.values.limiterTreshold, mtProject.values.limiterAttack/1000.0, mtProject.values.limiterRelease/1000.0);
	limiter[1].begin(mtProject.values.limiterTreshold, mtProject.values.limiterAttack/1000.0, mtProject.values.limiterRelease/1000.0);
	bitDepthControl[0].setBitDepth(mtProject.values.bitDepth);
	bitDepthControl[1].setBitDepth(mtProject.values.bitDepth);
//	setReverbDamping(mtProject.values.reverbDamping);
//	setReverbRoomsize(mtProject.values.reverbRoomSize);

	testWaveform.begin(0.0,1000,WAVEFORM_SQUARE);

	updateTimer.begin(updateAudioEngine,4500);
	updateTimer.priority(255);


	polyverb.reset();
	polyverb.setTime(mtProject.values.reverb.size);
	polyverb.setDamp(mtProject.values.reverb.damp);
	polyverb.SetPredelayLength(mtProject.values.reverb.predelay);
	polyverb.SetDiffusion(mtProject.values.reverb.diffusion);
	polyverb.unblockUpdate();

//	setPassEnvelope(1);
}

void updateAudioEngine()
{
	engine.update();
}

void audioEngine::update()
{
	if(recorder.update() == 0) SR->fullMemoryDuringRecordFlag = 1; //todo: to interface powinien getowac stan recordera


	if(recorder.mode == recorderModeStop)
	{
		currentTempo = sequencer.getActualTempo();
		if(currentTempo != lastTempo)
		{
			setDelayParams(mtProject.values.delayParams);
		}
		lastTempo = currentTempo;

		for(int i=0; i<8; i++)
		{
			instrumentPlayer[i].update();
		}
	}

}

void audioEngine::endAllFx()
{
	for(uint8_t i = 0 ; i < 8; i++)
	{
		instrumentPlayer[i].endFx(instrumentPlayer[i].lastSeqFx[0],0);
		instrumentPlayer[i].endFx(instrumentPlayer[i].lastSeqFx[1],1);
	}
}

void audioEngine::performanceModeEndAll()
{
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].endCutoffPerformanceMode();
		instrumentPlayer[i].endEndPointPerformanceMode();
		instrumentPlayer[i].endFilterTypePerformanceMode();
		instrumentPlayer[i].endGranularPositionPerformanceMode();
		instrumentPlayer[i].endPanningPerformanceMode();
		instrumentPlayer[i].endDelaySendPerformanceMode();
		instrumentPlayer[i].endReverbSendPerformanceMode();
		instrumentPlayer[i].endSamplePlaybackPerformanceMode();
		instrumentPlayer[i].endStartPointPerformanceMode();
		instrumentPlayer[i].endTunePerformanceMode();
		instrumentPlayer[i].endVolumePerformanceMode();
		instrumentPlayer[i].endWavetableWindowPerformanceMode();
		instrumentPlayer[i].endAmpLfoRatePerformanceMode();
		instrumentPlayer[i].endCutoffLfoRatePerformanceMode();
		instrumentPlayer[i].endPositionLfoRatePerformanceMode();
		instrumentPlayer[i].endPanningLfoRatePerformanceMode();
		instrumentPlayer[i].endFinetuneLfoRatePerformanceMode();
	}
}
void audioEngine::refreshTrackVolume()
{
	for(uint8_t track = 0 ; track < 8; track++)
	{
		instrumentPlayer[track].setStatusBytes(VOLUME_MASK);
	}
}
void audioEngine::refreshReverbVolume()
{
	for(uint8_t track = 0 ; track < 8; track++)
	{
		instrumentPlayer[track].setStatusBytes(REVERB_SEND_MASK);
	}
}
void audioEngine::refreshDelayVolume()
{
	for(uint8_t track = 0 ; track < 8; track++)
	{
		instrumentPlayer[track].setStatusBytes(DELAY_SEND_MASK);
	}
}

void audioEngine::setHeadphonesVolume(uint8_t value)
{
	audioShield.volume(value/100.0);
}

void audioEngine::setInterpolationEnable(bool value)
{
	for(uint8_t i = 0 ; i < 8 ; i++)
	{
		instrumentPlayer[i].setInterpolationEnable(value);
	}
}

void audioEngine::setDelayFeedback(uint8_t value)
{
	shortDelay.setFeedback(value/100.0);
}

void audioEngine::setDelayTime(uint16_t value)
{
	shortDelay.setTime(value);
}

void audioEngine::setDelayParams(uint8_t value)
{
	shortDelay.setPingpongEnable(value & 0b10000000);
	shortDelay.setSyncEnable(value & 0b01000000);
	shortDelay.setRate(value & 0b00111111);
}

void audioEngine::setDelayPanning(int8_t value)
{
	if(value > 0)
	{
		mixerL.gain(8,(100 - value)/100.0);
		mixerR.gain(8,1.0);
	}
	else if(value < 0)
	{
		mixerL.gain(8,1.0);
		mixerR.gain(8, (100 + value)/100.0 );
	}
	else if (value == 0)
	{
		mixerL.gain(8,1.0);
		mixerR.gain(8,1.0);
	}
}

void audioEngine::setReverbSize(float value)
{
	polyverb.setTime(value);
}
void audioEngine::setReverbDamp(float value)
{
	polyverb.setDamp(1.0f - value);
}
void audioEngine::setReverbPredelay(float value)
{
	polyverb.SetPredelayLength(value);
}
void audioEngine::setReverbDiffusion(float value)
{
	polyverb.SetDiffusion(value);
}

void audioEngine::setLimiterAttack(uint16_t attack)
{
	limiter[0].setAttack(attack);
	limiter[1].setAttack(attack);
}
void audioEngine::setLimiterRelease(float release)
{
	limiter[0].setRelease(release);
	limiter[1].setRelease(release);
}
void audioEngine::setLimiterTreshold(uint16_t threshold)
{
	 limiter[0].setThreshold(threshold);
	 limiter[1].setThreshold(threshold);
}

void audioEngine::setBitDepth(uint16_t bitDepth)
{
	bitDepthControl[0].setBitDepth(bitDepth);
	bitDepthControl[1].setBitDepth(bitDepth);
}

void audioEngine::setLastUsedVoice(uint8_t v)
{
	lastUsedVoice = v;
}
uint8_t audioEngine::getLastUsedVoice()
{
	return	lastUsedVoice;
}

void audioEngine::makeMetronomeTick(uint8_t accent)
{
	mixerSourceL.gain(5,ampLogValues[mtConfig.metronome.volume]);
	mixerSourceR.gain(5,ampLogValues[mtConfig.metronome.volume]);

	metronomeTick.envelopeSetPassFlag(1);

	metronomeTick.envelopeDelay(0);
	metronomeTick.envelopeAttack(0);
	metronomeTick.envelopeHold(0);
	metronomeTick.envelopeDecay(0);
	metronomeTick.envelopeSustain(1.0f);
	metronomeTick.envelopeRelease(0);
	metronomeTick.envelopeSetLoop(0);

	metronomeTick.envelopeSetIsRandom(false);
	metronomeTick.envelopeNoteOnForPrev((int16_t *)metronomeBeep, sizeof(metronomeBeep)/sizeof(int16_t), 30, 0);

	if(accent)metronomeTick.setFineTune(100, 30);
	else metronomeTick.setFineTune(45, 30);
	//Pykniecie jest probkowane 8kHz(oszczednosc pamieci), nuta 30 i finetune 45 wynikaja z koniecznosci zamienienia pitcha na odpowiedni dla 44.1 kHz
}

void audioEngine::setCurrentLoadInstrument(int8_t idx)
{
	if((idx < 0) || (idx >= 48)) return;
	isCurrentLoadInstrument[idx] = 1;
}
void audioEngine::clearCurrentLoadInstrument(int8_t idx)
{
	if((idx < 0) || (idx >= 48)) return;
	isCurrentLoadInstrument[idx] = 0;
}

float audioEngine::getDelayRms()
{
	float localVal = - 1.0f;
	if(rmsDelay[0].available() && rmsDelay[1].available())
	{
		localVal = (rmsDelay[0].read() + rmsDelay[1].read())/2.0f;
	}
	return localVal;
}
float audioEngine::getReverbRms()
{
	float localVal = - 1.0f;
	if(rmsPolyverb[0].available() && rmsPolyverb[1].available())
	{
		localVal = (rmsPolyverb[0].read() + rmsPolyverb[1].read())/2.0f;
	}
	return localVal;
}



playerEngine::playerEngine()
{
	// bazuje na zadeklarowanych wyzej obiektach silnika i na tym ze obiekty playerEngine beda umieszczone w tablicy(instrumentPlayer)
	// na tej podstawie jest wyznaczany index(nChannel)
	nChannel = ((uint32_t)this - (uint32_t)instrumentPlayer)/sizeof(playerEngine);
	playMemPtr = &playMem[nChannel];
	filterPtr = &filter[nChannel];
	ampPtr = &amp[nChannel];
	rmsPtr = &trackRMS[nChannel];
	envelopePtr[envCutoff] = &envelopeFilter[nChannel];
	envelopePtr[envWtPos]= &envelopeWtPosition[nChannel];
	envelopePtr[envPan] = &envelopePanning[nChannel];
	envelopePtr[envGranPos] = &envelopeGranPosition[nChannel];
	envelopePtr[envFinetune] = &envelopeFinetune[nChannel];

	playMemPtr->envelopeReleaseNoteOn(RELEASE_NOTE_ON_VAL);
}








void playerEngine :: modGlide(uint16_t value)
{
	playMemPtr->setGlide(value,currentNote,currentInstrument_idx);
}

void playerEngine :: modFineTune(int8_t value)
{
	playMemPtr->setFineTune(value,currentNote);
}

void playerEngine :: modPanning(int16_t value)
{
	float gainL=0,gainR=0;
	if(value < 50)
	{
		gainL=(0 + value)/50.0;
		gainR=1.0;
	}
	else if(value > 50)
	{
		gainL=1.0;
		gainR=(100 - value)/50.0;
	}
	else if(value == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(nChannel,gainL);
	mixerR.gain(nChannel,gainR);
}

void playerEngine::modVolume(float value)
{
	if(mtProject.values.dryMixMute) ampPtr->gain(AMP_MUTED);
	else ampPtr->gain(value * (ampLogValues[mtProject.values.trackVolume[nChannel]]));
}


void playerEngine :: modLP1(uint16_t value)
{
	playMemPtr->setLP1(value);
}
void playerEngine :: modLP2(uint16_t value)
{
	playMemPtr->setLP2(value);
}

void playerEngine :: modCutoff(float value)
{
	filterPtr->setCutoff(value);
}
void playerEngine :: modResonance(float value)
{
	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterPtr->resonance(value + RESONANCE_OFFSET);
	}
}

void playerEngine :: modWavetableWindow(uint16_t value)
{
	playMemPtr->setWavetableWindow(value);
}

void playerEngine :: modGranularPosition(uint16_t value)
{
	playMemPtr->setGranularPosition(value);
}

void playerEngine ::modGranularGrainLength()
{
	playMemPtr->setGranularGrainLength();
}

void playerEngine :: modTune(int8_t value)
{
	playMemPtr->setTune(value,currentNote);
}

void playerEngine :: modDelaySend(uint8_t value)
{
	if(mtProject.values.delayMute) mixerDelay.gain(nChannel,AMP_MUTED);
	else mixerDelay.gain(nChannel,ampLogValues[value] * ampLogValues[mtProject.values.delayVolume] );
}
void playerEngine :: modReverbSend(uint8_t value)
{
	if(mtProject.values.reverbMute) mixerReverb.gain(nChannel, AMP_MUTED);
	else mixerReverb.gain(nChannel,ampLogValues[value] * ampLogValues[mtProject.values.reverbVolume]);
}


void playerEngine::modSeqPoints(uint32_t sp, uint32_t ep, uint8_t fx_n)
{
	if(ep != NOT_MOD_POINTS) currentSeqModValues.endPoint = ep;
	if(sp != NOT_MOD_POINTS) currentSeqModValues.startPoint = sp;

	int32_t startPoint,loopPoint1,loopPoint2,endPoint;

	startPoint = sp != NOT_MOD_POINTS ? currentSeqModValues.startPoint : mtProject.instrument[currentInstrument_idx].startPoint;
	loopPoint1 = mtProject.instrument[currentInstrument_idx].loopPoint1;
	loopPoint2 = mtProject.instrument[currentInstrument_idx].loopPoint2;
	endPoint =  ep != NOT_MOD_POINTS ? currentSeqModValues.endPoint : mtProject.instrument[currentInstrument_idx].endPoint;


//	if(ep != NOT_MOD_POINTS ) trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint] = 1;
//	if(sp != NOT_MOD_POINTS ) trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint] = 1;


	if(mtProject.instrument[currentInstrument_idx].playMode != playModeSingleShot)
	{
		if(startPoint < SAMPLE_POINT_POS_MIN) startPoint = SAMPLE_POINT_POS_MIN;
		else if(startPoint > SAMPLE_POINT_POS_MAX) startPoint = SAMPLE_POINT_POS_MAX;

		if(endPoint < SAMPLE_POINT_POS_MIN) endPoint = SAMPLE_POINT_POS_MIN;
		else if(endPoint > SAMPLE_POINT_POS_MAX) endPoint = SAMPLE_POINT_POS_MAX;

		uint32_t loopSize = loopPoint2 - loopPoint1;

		if(endPoint > loopPoint2)
		{
			if(startPoint > loopPoint1)
			{
				if((int32_t)(startPoint + 1 + loopSize) > (int32_t)endPoint)
				{
					loopPoint2 = endPoint - 1;
					loopPoint1 = loopPoint2 - loopSize;
					startPoint = loopPoint1 > 0 ? loopPoint1 - 1: 0;
				}
				else
				{
					loopPoint1 = startPoint + 1;
					loopPoint2 = loopPoint1 + loopSize;
				}
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint1] = 1;
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint2] = 1;
			}
			else //endPoint > od loopPoint2 , startPoint < od loopPoint1  - nic nie trzeba ruszac
			{
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint1] = 0;
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint2] = 0;
			}
		}
		else
		{
			if(startPoint > loopPoint1)
			{
				startPoint = loopPoint1 > 0 ? loopPoint1 - 1: 0;
				endPoint = loopPoint2 < SAMPLE_POINT_POS_MAX ? loopPoint2 + 1: SAMPLE_POINT_POS_MAX;
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint1] = 0;
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint2] = 0;
			}
			else
			{
				if((int32_t)(endPoint - 1 - loopSize) < (int32_t)startPoint)
				{
					loopPoint1 = startPoint +1;
					loopPoint2 = startPoint + loopSize;
					endPoint = loopPoint2 < SAMPLE_POINT_POS_MAX ? loopPoint2 + 1: SAMPLE_POINT_POS_MAX;
				}
				else
				{
					loopPoint2 = endPoint - 1;
					loopPoint1 = loopPoint2 - loopSize;
				}
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint1] = 1;
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::loopPoint2] = 1;
			}
		}
	}
	else
	{
		if(startPoint < SAMPLE_POINT_POS_MIN) startPoint = SAMPLE_POINT_POS_MIN;
		else if(startPoint > SAMPLE_POINT_POS_MAX) startPoint = SAMPLE_POINT_POS_MAX;

		if(endPoint < SAMPLE_POINT_POS_MIN) endPoint = SAMPLE_POINT_POS_MIN;
		else if(endPoint > SAMPLE_POINT_POS_MAX) endPoint = SAMPLE_POINT_POS_MAX;
	}

	currentSeqModValues.loopPoint1 = loopPoint1;
	currentSeqModValues.loopPoint2 = loopPoint2;

	playMemPtr->setForcedPoints(startPoint,loopPoint1,loopPoint2,endPoint);
	playMemPtr->setPointsForceFlag();
}



void playerEngine :: setStatusBytes(uint32_t value)
{
	statusBytes|=value;
}


void playerEngine :: changeFilterType(uint8_t type)
{
	filterPtr->setType(type);
}

void playerEngine :: filterDisconnect()
{
	filterPtr->disconnect();
}

void playerEngine :: filterConnect()
{
	filterPtr->connect();
}

void playerEngine :: clean(void)
{

	currentInstrument_idx=0;
	currentNote=0;
	statusBytes=0;
	playMemPtr->clean();
}

void playerEngine::setInterpolationEnable(bool value)
{
	playMemPtr->setInterpolationEnable(value);
}

float playerEngine :: fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t playerEngine :: noteOnforPrev (uint8_t instr_idx,int8_t note,int8_t velocity)
{
	AudioNoInterrupts();
	uint8_t status;
//	engine.clearDelay();
	for(uint8_t i = envPan ; i < ACTIVE_ENVELOPES; i++)
	{
		envelopePtr[i]->kill();
	}

	currentInstrument_idx=instr_idx;
	currentNote=note;
	/*================================================ENVELOPE AMP==========================================*/
//	lfoAmpPtr->init(&mtProject.instrument[instr_idx].lfo[lfoA]);

	if(mtProject.instrument[instr_idx].envelope[envAmp].enable)
	{
		if(mtProject.instrument[instr_idx].envelope[envAmp].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[instr_idx].lfo[envAmp],mtProject.instrument[instr_idx].lfo[envAmp].speed, 1);
			initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);
		}
		else
		{
			initEnvelopesParamiters(envAmp, &mtProject.instrument[instr_idx].envelope[envAmp]);
		}

	}
	else
	{
		initEnvelopesParamiters(envAmp, (envelopeGenerator::strEnv*)&passEnvelope);
	}

	/*======================================================================================================*/
	/*================================================ENVELOPE FILTER=======================================*/

	if(mtProject.instrument[instr_idx].filterEnable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[instr_idx].lfo[envCutoff], mtProject.instrument[instr_idx].lfo[envCutoff].speed,0);
				initEnvelopesParamiters(envCutoff, &lfoBasedEnvelope[envCutoff]);
			}
			else
			{
				initEnvelopesParamiters(envCutoff, &mtProject.instrument[instr_idx].envelope[envCutoff]);
			}
		}

//		lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
	/*======================================================================================================*/
	/*================================================FILTER================================================*/
		filterConnect();
		changeFilterType(mtProject.instrument[instr_idx].filterType);
		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);
		if(!isActiveEnvelope(envCutoff)) modCutoff(mtProject.instrument[instr_idx].cutOff);
	}
	else if(!mtProject.instrument[instr_idx].filterEnable) filterDisconnect();

	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		currentEnvelopeModification[envWtPos] = 0;

		if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[instr_idx].lfo[envWtPos],mtProject.instrument[instr_idx].lfo[envWtPos].speed,0);
				initEnvelopesParamiters(envWtPos, &lfoBasedEnvelope[envWtPos]);
			}
			else
			{
				initEnvelopesParamiters(envWtPos, &mtProject.instrument[instr_idx].envelope[envWtPos]);
			}
		}
	}

	if((mtProject.instrument[instr_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[instr_idx].playMode == playModeGranular))
	{
		currentEnvelopeModification[envGranPos] = 0;
		if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[instr_idx].lfo[envGranPos], mtProject.instrument[instr_idx].lfo[envGranPos].speed,0);
				initEnvelopesParamiters(envGranPos, &lfoBasedEnvelope[envGranPos]);
			}
			else
			{
				initEnvelopesParamiters(envGranPos, &mtProject.instrument[instr_idx].envelope[envGranPos]);
			}
		}

	}


	if(mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[instr_idx].lfo[envPan], mtProject.instrument[instr_idx].lfo[envPan].speed,0);
			initEnvelopesParamiters(envPan, &lfoBasedEnvelope[envPan]);
		}
		else
		{
			initEnvelopesParamiters(envPan, &mtProject.instrument[instr_idx].envelope[envPan]);
		}
	}

	if(mtProject.instrument[currentInstrument_idx].envelope[envFinetune].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envFinetune].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[instr_idx].lfo[envFinetune], mtProject.instrument[instr_idx].lfo[envFinetune].speed,0);
			initEnvelopesParamiters(envFinetune, &lfoBasedEnvelope[envFinetune]);
		}
		else
		{
			initEnvelopesParamiters(envFinetune, &mtProject.instrument[instr_idx].envelope[envFinetune]);
		}
	}


	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	float localAmount = mtProject.instrument[instr_idx].envelope[envAmp].loop ? lfoBasedEnvelope[envAmp].amount : mtProject.instrument[instr_idx].envelope[envAmp].amount;

	modVolume(localAmount * ampLogValues[mtProject.instrument[instr_idx].volume]);


	/*======================================================================================================*/
	/*===============================================PANNING================================================*/
	if(!isActiveEnvelope(envPan)) modPanning(mtProject.instrument[instr_idx].panning);

	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/

	modDelaySend(mtProject.instrument[instr_idx].delaySend);


	/*======================================================================================================*/

	for(uint8_t i = envPan ; i < ACTIVE_ENVELOPES; i++)
	{
		if(mtProject.instrument[instr_idx].envelope[i].enable)
		{
			bool isRandom = (mtProject.instrument[instr_idx].envelope[i].loop) && (mtProject.instrument[instr_idx].lfo[i].shape == lfoShapeRandom);
			envelopePtr[i]->setIsRandom(isRandom);

			envelopePtr[i]->start();
		}

	}

	status = 1; //playMemPtr->playForPrev(instr_idx,note);

	playMemPtr->envelopeSetIsRandom(false);
	playMemPtr->envelopeNoteOnForPrev(instr_idx,note);

	return status;
//	__enable_irq();
	AudioInterrupts();


}


uint8_t playerEngine :: noteOnforPrev (int16_t * addr, uint32_t len,uint8_t type)
{
	uint8_t status=0;

	initEnvelopesParamiters(envAmp, (envelopeGenerator::strEnv * )&passEnvelope);


	for(uint8_t i = envPan ; i < ACTIVE_ENVELOPES; i++)
	{
		envelopePtr[i]->kill();
	}

//	engine.clearDelay();

	filterDisconnect();
	modVolume(ampLogValues[50]);

	modPanning(50);
	modDelaySend(AMP_MUTED);
	modReverbSend(AMP_MUTED);
	/*======================================================================================================*/
	limiter[0].setAttack(300);
	limiter[0].setRelease(10);
	limiter[0].setThreshold(32000);
	limiter[1].setAttack(300);
	limiter[1].setRelease(10);
	limiter[1].setThreshold(32000);
	bitDepthControl[0].setBitDepth(16);
	bitDepthControl[1].setBitDepth(16);

	status = 1;// playMemPtr->playForPrev(addr,len,type);

	playMemPtr->envelopeSetIsRandom(false);
	playMemPtr->envelopeNoteOnForPrev(addr,len,type);

	return status;

}

uint8_t playerEngine :: noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note, uint8_t type)
{
	uint8_t status=0;

	initEnvelopesParamiters(envAmp, (envelopeGenerator::strEnv * )&passEnvelope);

	for(uint8_t i = envPan ; i < ACTIVE_ENVELOPES; i++)
	{
		envelopePtr[i]->kill();
	}

	filterDisconnect();
	modVolume(ampLogValues[50]); //wracam z 50 do 100, a teraz ze 100 do 50
//	engine.clearDelay();
	modPanning(50);
	modDelaySend(AMP_MUTED);
	modReverbSend(AMP_MUTED);
	/*======================================================================================================*/
	limiter[0].setAttack(300);
	limiter[0].setRelease(10);
	limiter[0].setThreshold(32000);
	limiter[1].setAttack(300);
	limiter[1].setRelease(10);
	limiter[1].setThreshold(32000);
	bitDepthControl[0].setBitDepth(16);
	bitDepthControl[1].setBitDepth(16);

	status = 1;//playMemPtr->playForPrev(addr,len,note,type);

	playMemPtr->envelopeSetIsRandom(false);
	playMemPtr->envelopeNoteOnForPrev(addr,len,note,type);

	return status;
}
uint8_t playerEngine ::getInterfaceEndReleaseFlag()
{
	return interfaceEndReleaseFlag;
}
void playerEngine ::clearInterfaceEndReleaseFlag()
{
	interfaceEndReleaseFlag = 0;
}

uint8_t playerEngine ::getInterfacePlayingEndFlag()
{
	return interfacePlayingEndFlag;
}
void playerEngine ::clearInterfacePlayingEndFlag()
{
	interfacePlayingEndFlag = 0;
}

void playerEngine::setTrackIsDisplayed()
{
	isTrackDisplayed = true;
}
void playerEngine::clearTrackIsDisplayed()
{
	isTrackDisplayed = false;
}
bool playerEngine::getEndDisplayTrackFlag()
{
	return onEndDisplay;
}
void playerEngine::clearEndDisplayTrackFlag()
{
	onEndDisplay = false;
}




uint16_t playerEngine ::getWavePosition()
{
	return playMemPtr->getPosition();
}
//**************************************************************************************************************************************
//state: 1 - solo ON, 0 - solo OFF
//Na potrzeby eksportu - nie wykorzystywana w mixerze
void audioEngine::soloTrack(uint8_t channel, uint8_t state)
{
	if(channel >= 8 ) return;

	if(state == 1)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			if(i == channel)
			{
				if(!mtProject.values.trackMute[i])
				{
					muteDelaySend(i, 1);
					muteReverbSend(i, 1);
				}
				continue;
			}
			if(!mtProject.values.trackMute[i]) muteTrack(i,1);
		}
	}
	else if(state == 0)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			if(i == channel)
			{
				muteDelaySend(i, 0);
				muteReverbSend(i, 0);
				continue;
			}
			if(!mtProject.values.trackMute[i]) muteTrack(i,0);
		}
	}

}

// selectLR :  0 - L, 1- R, state: 1 - solo ON, 0 - solo OFF
void audioEngine::soloDelaySend(uint8_t state)
{
	if(state == 1)
	{
		forceDelaySend = 1;
		for(uint8_t i = 0; i < 8; i++)
		{
			if(!mtProject.values.trackMute[i]) muteTrack(i,1);
		}
	}
	else if(state == 0)
	{
		forceDelaySend = 0;
		for(uint8_t i = 0; i < 8; i++)
		{
			if(!mtProject.values.trackMute[i]) muteTrack(i,0);
		}
	}

}

void audioEngine::soloReverbSend(uint8_t state)
{
	if(state == 1)
	{
		forceReverbSend = 1;
		for(uint8_t i = 0; i < 8; i++)
		{
			if(!mtProject.values.trackMute[i]) muteTrack(i,1);
		}
	}
	else if(state == 0)
	{
		forceReverbSend = 0;
		for(uint8_t i = 0; i < 8; i++)
		{
			if(!mtProject.values.trackMute[i]) muteTrack(i,0);
		}
	}
}

void audioEngine::clearDelay()
{
//	reverb.clearFilters();
	shortDelay.clear();
	for(uint8_t channel = 0; channel < 8 ; channel++)
	{
		instrumentPlayer[channel].modDelaySend(AMP_MUTED);
	}

}
void audioEngine::clearReverb()
{
	polyverb.reset();
	for(uint8_t channel = 0; channel < 8 ; channel++)
	{
		instrumentPlayer[channel].modReverbSend(AMP_MUTED);
	}

}

void audioEngine::muteDelaySend(uint8_t channel, uint8_t state)
{
	if(channel >= 8) return;
	if(state == 0)
	{
		instrumentPlayer[channel].onlyDelayMuteState = 0;
		instrumentPlayer[channel].setStatusBytes(DELAY_SEND_MASK);
	}
	else
	{
		instrumentPlayer[channel].onlyDelayMuteState = 1;
		if(!forceDelaySend) instrumentPlayer[channel].modDelaySend(0);
	}
}

void audioEngine::muteReverbSend(uint8_t channel, uint8_t state)
{
	if(channel >= 8) return;
	if(state == 0)
	{
		instrumentPlayer[channel].onlyReverbMuteState = 0;
		instrumentPlayer[channel].setStatusBytes(REVERB_SEND_MASK);
	}
	else
	{
		instrumentPlayer[channel].onlyReverbMuteState = 1;
		if(!forceReverbSend) instrumentPlayer[channel].modReverbSend(0);
	}
}

//**************************************************************************************************************************************
//PERFORMANCE MODE

// state = 1 mutuje
void audioEngine::muteTrack(uint8_t channel, uint8_t state)
{
	if(channel >= 8) return;
	if(state == MUTE_DISABLE)
	{
		instrumentPlayer[channel].muteState = 0;
		instrumentPlayer[channel].setStatusBytes(VOLUME_MASK);
		instrumentPlayer[channel].setStatusBytes(DELAY_SEND_MASK);
		instrumentPlayer[channel].setStatusBytes(REVERB_SEND_MASK);
	}
	else
	{
		instrumentPlayer[channel].muteState = 1;
		amp[channel].gain(AMP_MUTED);
		if(!forceDelaySend && mtProject.values.trackMute[channel]) instrumentPlayer[channel].modDelaySend(AMP_MUTED);
		if(!forceReverbSend && mtProject.values.trackMute[channel]) instrumentPlayer[channel].modReverbSend(AMP_MUTED);
	}
}

//************************************************************************************************************
uint32_t playerEngine::getEnvelopeWtPosMod()
{
	if((envelopePtr[envWtPos]->isKeyPressed() == 1) || (envelopePtr[envWtPos]->getPhase() != 0))
	{
		uint32_t localWTPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
		int32_t localWTMod = currentEnvelopeModification[envWtPos] * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;

		if( (int)(localWTPos + localWTMod) >  (int)(mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1))
		{
			localWTPos = ( (int)(mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1) >= 0) ?
								 mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1:
								 0;
		}
		else if( (int)(localWTPos + localWTMod) < 0 )
		{
			localWTPos = 0;
		}
		else
		{
			localWTPos += localWTMod ;
		}

		return localWTPos;
	}
	else
	{
		return mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
	}
}

uint32_t playerEngine::getEnvelopeGranPosMod()
{

	if((envelopePtr[envGranPos]->isKeyPressed() == 1) || (envelopePtr[envGranPos]->getPhase() != 0))
	{
		uint32_t localGranPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
		int32_t localGranMod = currentEnvelopeModification[envGranPos] * MAX_16BIT;

		if((int)(localGranPos + localGranMod) >  MAX_16BIT)
		{
			localGranPos = MAX_16BIT;
		}
		else if((int)(localGranPos + localGranMod) < 0 )
		{
			localGranPos = 0;
		}
		else
		{
			localGranPos += localGranMod ;
		}

		return localGranPos;
	}
	else
	{
		return mtProject.instrument[currentInstrument_idx].granular.currentPosition;
	}
}

void playerEngine::calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo, uint8_t rate, uint8_t isAmp)
{
	env->loop = 1;
	env->enable = 1;
	env->amount = lfo->amount;

	uint8_t envelopeConstrain = isAmp ? CONSTRAIN_SPEED_AMP_ENVELOPE : CONSTRAIN_SPEED_OTHER_ENVELOPE;
	if(rate > envelopeConstrain) rate = envelopeConstrain;

	float lfoFrequency = (sequencer.getActualTempo()/15.0);

	float periodTime = (1000 / lfoFrequency) * (isAmp ? tempoSyncRatesAmp[rate] : tempoSyncRatesOthers[rate]);


	switch(lfo->shape)
	{
	case lfoShapeReverseSaw:

		env->attack = 0;
		env->decay = periodTime;
		env->delay = 0;
		env->sustain = 0.0f;
		env->hold = 0;
		env->release = 0;

		break;

	case lfoShapeSaw:

		env->attack = periodTime;
		env->decay = 0;
		env->delay = 0;
		env->sustain = 0.0f;
		env->hold = 0;
		env->release = 0;

		break;

	case lfoShapeTriangle:

		env->attack = periodTime/2;
		env->decay = periodTime/2;;
		env->delay = 0;
		env->sustain = 0.0f;
		env->hold = 0;
		env->release = 0;

		break;
	case lfoShapeSquare:
		env->attack = 0;
		env->decay = 0 ;
		env->delay = 0;
		env->sustain = 0.0f;
		env->hold = periodTime/2;
		env->release = periodTime/2;
		break;
	case lfoShapeRandom:
		env->attack = 0;
		env->decay = 0 ;
		env->delay = 0;
		env->sustain = 0.0f;
		env->hold = periodTime;
		env->release = 0;
		break;
	}
}

float playerEngine::getRMSValue()
{
	float localVal = - 1.0f;
	if(rmsPtr->available())
	{
		localVal = rmsPtr->read();
	}
	return localVal;
}


void playerEngine::setPassEnvelope(uint8_t state)
{
	playMemPtr->envelopeSetPassFlag(state);
	envelopePassFlag = state;
}



