#include "mtAudioEngine.h"
#include "sampleRecorder/sampleRecorder.h"

extern AudioControlSGTL5000 audioShield;
static cSampleRecorder* SR = &sampleRecorder;




AudioInputI2S            i2sIn;
AudioOutputI2S           i2sOut;
AudioRecordQueue         queue;

envelopeGenerator		 envelopeFilter[8];
envelopeGenerator		 envelopeWtPosition[8];
envelopeGenerator		 envelopeGranPosition[8];
envelopeGenerator		 envelopePanning[8];

AudioPlaySdWav           playSdWav;
AudioPlaySdWavFloat		 playSdWavFloat;
AudioPlaySdWav24bit 	 playSdWav24Bit;

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
AudioAmplifier           amp[8];
AudioFilterStateVariable filter[8];
AudioEffectFreeverb		 reverb;
AudioEffectLimiter		 limiter[2];
AudioBitDepth			 bitDepthControl[2];

AudioMixer9				 mixerL,mixerR,mixerReverb;
AudioMixer4              mixerRec;
AudioMixer9              mixerSourceL,mixerSourceR;

AudioAnalyzeRMS			 rms;
AudioRecordQueue		 exportL, exportR;
AudioAnalyzeRMS			 exportRmsL, exportRmsR;
AudioSynthWaveform		 testWaveform;

AudioConnection          connect1(&playMem[0], 0, &filter[0], 0);
AudioConnection          connect2(&playMem[1], 0, &filter[1], 0);
AudioConnection          connect3(&playMem[2], 0, &filter[2], 0);
AudioConnection          connect4(&playMem[3], 0, &filter[3], 0);
AudioConnection          connect5(&playMem[4], 0, &filter[4], 0);
AudioConnection          connect6(&playMem[5], 0, &filter[5], 0);
AudioConnection          connect7(&playMem[6], 0, &filter[6], 0);
AudioConnection          connect8(&playMem[7], 0, &filter[7], 0);

AudioConnection          connect9(&filter[0], 0, &envelopeAmp[0], 0);
AudioConnection          connect10(&filter[1], 0, &envelopeAmp[1], 0);
AudioConnection          connect11(&filter[2], 0, &envelopeAmp[2], 0);
AudioConnection          connect12(&filter[3], 0, &envelopeAmp[3], 0);
AudioConnection          connect13(&filter[4], 0, &envelopeAmp[4], 0);
AudioConnection          connect14(&filter[5], 0, &envelopeAmp[5], 0);
AudioConnection          connect15(&filter[6], 0, &envelopeAmp[6], 0);
AudioConnection          connect16(&filter[7], 0, &envelopeAmp[7], 0);

AudioConnection          connect17(&envelopeAmp[0], &amp[0]);
AudioConnection          connect18(&envelopeAmp[1], &amp[1]);
AudioConnection          connect19(&envelopeAmp[2], &amp[2]);
AudioConnection          connect20(&envelopeAmp[3], &amp[3]);
AudioConnection          connect21(&envelopeAmp[4], &amp[4]);
AudioConnection          connect22(&envelopeAmp[5], &amp[5]);
AudioConnection          connect23(&envelopeAmp[6], &amp[6]);
AudioConnection          connect24(&envelopeAmp[7], &amp[7]);

AudioConnection          connect25(&amp[0], 0, &mixerL, 0);
AudioConnection          connect26(&amp[1], 0, &mixerL, 1);
AudioConnection          connect27(&amp[2], 0, &mixerL, 2);
AudioConnection          connect28(&amp[3], 0, &mixerL, 3);
AudioConnection          connect29(&amp[4], 0, &mixerL, 4);
AudioConnection          connect30(&amp[5], 0, &mixerL, 5);
AudioConnection          connect31(&amp[6], 0, &mixerL, 6);
AudioConnection          connect32(&amp[7], 0, &mixerL, 7);

AudioConnection          connect33(&amp[0], 0, &mixerR, 0);
AudioConnection          connect34(&amp[1], 0, &mixerR, 1);
AudioConnection          connect35(&amp[2], 0, &mixerR, 2);
AudioConnection          connect36(&amp[3], 0, &mixerR, 3);
AudioConnection          connect37(&amp[4], 0, &mixerR, 4);
AudioConnection          connect38(&amp[5], 0, &mixerR, 5);
AudioConnection          connect39(&amp[6], 0, &mixerR, 6);
AudioConnection          connect40(&amp[7], 0, &mixerR, 7);

AudioConnection          connect41(&envelopeAmp[0], 0, &mixerReverb, 0);
AudioConnection          connect42(&envelopeAmp[1], 0, &mixerReverb, 1);
AudioConnection          connect43(&envelopeAmp[2], 0, &mixerReverb, 2);
AudioConnection          connect44(&envelopeAmp[3], 0, &mixerReverb, 3);
AudioConnection          connect45(&envelopeAmp[4], 0, &mixerReverb, 4);
AudioConnection          connect46(&envelopeAmp[5], 0, &mixerReverb, 5);
AudioConnection          connect47(&envelopeAmp[6], 0, &mixerReverb, 6);
AudioConnection          connect48(&envelopeAmp[7], 0, &mixerReverb, 7);

AudioConnection          connect49(&mixerReverb,&reverb);

AudioConnection          connect50(&reverb, 0, &mixerL, 8);
AudioConnection          connect51(&reverb, 0, &mixerR, 8);

AudioConnection          connect57(&mixerL, &bitDepthControl[0]);
AudioConnection          connect58(&mixerR, &bitDepthControl[1]);

AudioConnection          connect52(&bitDepthControl[0], 0, &limiter[0], 0);
AudioConnection          connect53(&bitDepthControl[1], 0, &limiter[1], 0);

AudioConnection          connect68(&limiter[0], 0, &mixerSourceL, 0);
AudioConnection          connect69(&limiter[1], 0, &mixerSourceR, 0);

AudioConnection          connect61(&playSdWav, 0, &mixerSourceL, 1);
AudioConnection          connect62(&playSdWav, 0, &mixerSourceR, 1);

AudioConnection 		 connect63(&playSdWavFloat,0,&mixerSourceL,2);
AudioConnection 		 connect64(&playSdWavFloat,0,&mixerSourceR,2);

AudioConnection 		 connect65(&playSdWav24Bit,0,&mixerSourceL,3);
AudioConnection 		 connect66(&playSdWav24Bit,0,&mixerSourceR,3);

AudioConnection          connect80(&testWaveform, 0, &mixerSourceR, 4);
AudioConnection          connect81(&testWaveform, 0, &mixerSourceL, 4);

AudioConnection          connect59(&mixerSourceL, 0, &i2sOut, 0);
AudioConnection          connect60(&mixerSourceR, 0, &i2sOut, 1);
//**************** export
AudioConnection          connect70(&mixerSourceL, &exportL);
AudioConnection          connect71(&mixerSourceR, &exportR);

AudioConnection          connect72(&mixerSourceL, &exportRmsL);
AudioConnection          connect73(&mixerSourceR, &exportRmsR);

//**************** recording
AudioConnection          connect54(&i2sIn, 0, &mixerRec, 0);
AudioConnection          connect55(&i2sIn, 1, &mixerRec, 1);


AudioConnection          connect56(&mixerRec, &queue);
AudioConnection          connect67(&mixerRec, &rms);

IntervalTimer updateTimer;

playerEngine instrumentPlayer[8];
audioEngine engine;





constexpr uint16_t RELEASE_NOTE_ON_VAL = 5;

void updateAudioEngine();

void audioEngine::printLog(SdFile * log)
{

	log->println("***********************AudioEngine***************************");
	for(int i=0;i<8; i++)
	{
		instrumentPlayer[i].printLog(log);
	}
}

void audioEngine::startTestSignal(float amp, float freq)
{
	testWaveform.amplitude(amp);
	testWaveform.frequency(freq);
}

void audioEngine::stopTestSignal()
{
	testWaveform.amplitude(0.0);
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
	for(uint8_t i = 0; i < 4 ; i ++)
	{
		mixerSourceR.gain(i,1.0);
		mixerSourceL.gain(i,1.0);
	}

	audioShield.volume(mtProject.values.volume/100.0);
	audioShield.inputSelect(AUDIO_INPUT_LINEIN);
	mtConfig.audioCodecConfig.inSelect = inputSelectLineIn;

	audioShield.lineInLevel(3);
	limiter[0].begin(mtProject.values.limiterTreshold, mtProject.values.limiterAttack/1000.0, mtProject.values.limiterRelease/1000.0);
	limiter[1].begin(mtProject.values.limiterTreshold, mtProject.values.limiterAttack/1000.0, mtProject.values.limiterRelease/1000.0);
	bitDepthControl[0].setBitDepth(mtProject.values.bitDepth);
	bitDepthControl[1].setBitDepth(mtProject.values.bitDepth);
	setReverbDamping(mtProject.values.reverbDamping);
	setReverbRoomsize(mtProject.values.reverbRoomSize);

	testWaveform.begin(0.0,1000,WAVEFORM_SQUARE);

	updateTimer.begin(updateAudioEngine,4500);
	updateTimer.priority(255);
}

void updateAudioEngine()
{
	engine.update();
}

void audioEngine::update()
{
	if(recorder.update() == 0) SR->fullMemoryDuringRecordFlag = 1;


	if(recorder.mode == recorderModeStop)
	{
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
	}
}

void audioEngine::setHeadphonesVolume(uint8_t value)
{
	audioShield.volume(value/100.0);
}

void audioEngine::setReverbRoomsize(uint8_t value)
{
	reverb.roomsize(value/100.0);
}

void audioEngine::setReverbDamping(uint8_t value)
{
	reverb.damping(value/100.0);
}

void audioEngine::setReverbPanning(int8_t value)
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

playerEngine::playerEngine()
{
	// bazuje na zadeklarowanych wyzej obiektach silnika i na tym ze obiekty playerEngine beda umieszczone w tablicy(instrumentPlayer)
	// na tej podstawie jest wyznaczany index(nChannel)
	nChannel = ((uint32_t)this - (uint32_t)instrumentPlayer)/sizeof(playerEngine);
	playMemPtr = &playMem[nChannel];
	envelopeAmpPtr = &envelopeAmp[nChannel];
	filterPtr = &filter[nChannel];
	ampPtr = &amp[nChannel];

	envelopePtr[envFilter] = &envelopeFilter[nChannel];
	envelopePtr[envWtPos]= &envelopeWtPosition[nChannel];
	envelopePtr[envPan] = &envelopePanning[nChannel];
	envelopePtr[envGranPos] = &envelopeGranPosition[nChannel];

	envelopeAmpPtr->releaseNoteOn(RELEASE_NOTE_ON_VAL);
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
		gainR=(0 + value)/50.0;
		gainL=1.0;
	}
	else if(value > 50)
	{
		gainR=1.0;
		gainL=(100 - value)/50.0;
	}
	else if(value == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(nChannel,gainL);
	mixerR.gain(nChannel,gainR);

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
	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterPtr->setCutoff(value);
	}
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

void playerEngine :: modReverbSend(uint8_t value)
{
	mixerReverb.gain(nChannel,value/100.0);
}

void playerEngine::modSeqPoints(uint32_t sp, uint32_t ep)
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


	if(mtProject.instrument[currentInstrument_idx].playMode != singleShot)
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
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] = 1;
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] = 1;
			}
			else //endPoint > od loopPoint2 , startPoint < od loopPoint1  - nic nie trzeba ruszac
			{
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] = 0;
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] = 0;
			}
		}
		else
		{
			if(startPoint > loopPoint1)
			{
				startPoint = loopPoint1 > 0 ? loopPoint1 - 1: 0;
				endPoint = loopPoint2 < SAMPLE_POINT_POS_MAX ? loopPoint2 + 1: SAMPLE_POINT_POS_MAX;
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] = 0;
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] = 0;
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
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] = 1;
				trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] = 1;
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

	playMemPtr->setForcedPoints(startPoint,loopPoint1,loopPoint2,endPoint);
	playMemPtr->setPointsForceFlag();
}

void playerEngine:: update()
{
	currentPlayState = playMemPtr->isPlaying();
	if(currentPlayState == 0 && lastPlayState == 1)
	{
		envelopeAmpPtr->noteOff();
		interfacePlayingEndFlag = 1;
	}
	lastPlayState = currentPlayState;

	if(envelopeAmpPtr->endRelease())
	{
		envelopeAmpPtr->clearEndReleaseFlag();
		interfaceEndReleaseFlag = 1;
		playMemPtr->stop();

		if((mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable && mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
		{
			currentEnvelopeModification[envFilter] = 0;
			envelopePtr[envFilter]->stop();
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
		{
			currentEnvelopeModification[envWtPos] = 0;
			envelopePtr[envWtPos]->stop();
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
		{
			currentEnvelopeModification[envGranPos] = 0;
			envelopePtr[envGranPos]->stop();
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable && mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
		{
			currentEnvelopeModification[envPan] = 0;
			envelopePtr[envPan]->stop();
		}
	}



	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		if((mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
		{
			if((envelopePtr[envFilter]->isKeyPressed() == 1) || (envelopePtr[envFilter]->getPhase() != 0))
			{
				currentEnvelopeModification[envFilter] =  envelopePtr[envFilter]->getOut();
				statusBytes |= CUTOFF_MASK;
			}
		}
	}

	if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
	{
		if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
		{
				if((envelopePtr[envWtPos]->isKeyPressed() == 1) || (envelopePtr[envWtPos]->getPhase() != 0))
				{
					currentEnvelopeModification[envWtPos] = envelopePtr[envWtPos]->getOut();
					statusBytes |= WT_POS_SEND_MASK;
				}
		}
	}

	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
	{
		if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
		{
			if((envelopePtr[envGranPos]->isKeyPressed() == 1) || (envelopePtr[envGranPos]->getPhase() != 0))
			{
				currentEnvelopeModification[envGranPos] = envelopePtr[envGranPos]->getOut();
				statusBytes |= GRANULAR_POS_SEND_MASK;
			}
		}
	}
	if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)||
	  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning]) ||
	  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning]) ||
	  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
	{
		if((envelopePtr[envPan]->isKeyPressed() == 1) || (envelopePtr[envPan]->getPhase() != 0))
		{
			currentEnvelopeModification[envPan] = envelopePtr[envPan]->getOut();
			statusBytes |= PANNING_MASK;
		}
	}



	if(statusBytes)
	{
		if(statusBytes & LP1_MASK)
		{
			statusBytes &= (~LP1_MASK);
			modLP1(mtProject.instrument[currentInstrument_idx].loopPoint1);
		}
		if(statusBytes & LP2_MASK)
		{
			statusBytes &= (~LP2_MASK);
			modLP2(mtProject.instrument[currentInstrument_idx].loopPoint2);
		}
		if(statusBytes & FINETUNE_MASK)
		{
			statusBytes &= (~FINETUNE_MASK);

			int8_t fineTune;
			if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::fineTune] ||
			   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::fineTune] ) fineTune = currentSeqModValues.fineTune;
			else fineTune = mtProject.instrument[currentInstrument_idx].fineTune;


			if(fineTune > MAX_INSTRUMENT_FINETUNE)
			{
				playMemPtr->setForcedFineTune(MAX_INSTRUMENT_FINETUNE);
				modFineTune(MAX_INSTRUMENT_FINETUNE);
			}
			else if(fineTune < MIN_INSTRUMENT_FINETUNE)
			{
				playMemPtr->setForcedFineTune(MIN_INSTRUMENT_FINETUNE);
				modFineTune(MIN_INSTRUMENT_FINETUNE);
			}
			else
			{
				playMemPtr->setForcedFineTune(fineTune);
				modFineTune(fineTune);
			}
		}
		if(statusBytes & TUNE_MASK)
		{
			statusBytes &= (~TUNE_MASK);
			modTune(mtProject.instrument[currentInstrument_idx].tune);
		}
		if(statusBytes & VOLUME_MASK)
		{
			statusBytes &= (~VOLUME_MASK);

			if(muteState == 0)
			{
				uint8_t volume = 0;
				// Nie trzeba tu zwracac uwagi na env i lfo gdyż ogarnia to blok biblioteczny dla kazdej probki wewnetrznie
				if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
				{
					volume = currentPerformanceValues.volume;
				}
				else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
						trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume]	)
				{
					volume = currentSeqModValues.volume;
				}
				else
				{
					volume = mtProject.instrument[currentInstrument_idx].volume;
				}
				float localAmount = 0.0f;

				if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp]  ||
				   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp] ||
				   trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] )
				{
					localAmount = mtProject.instrument[currentInstrument_idx].lfo[envAmp].amount;
				}
				else
				{
					if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
					{
						if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
						{
							localAmount = mtProject.instrument[currentInstrument_idx].lfo[envAmp].amount;
						}
						else
						{
							localAmount = mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount;
						}
					}
					else
					{
						localAmount = 1.0f;
					}

				}

				ampPtr->gain( (volume/100.0) * localAmount );
			}
		}
		if(statusBytes & PANNING_MASK)
		{
			statusBytes &= (~PANNING_MASK);
			if((!mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning])
		    &&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
			{
				currentEnvelopeModification[envPan] = 0;
			}

			int32_t localPanningMod = currentEnvelopeModification[envPan] * PANNING_MAX;

			int16_t localPanning = 0;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
			{
				localPanning = currentPerformanceValues.panning;
			}
			else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
					trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning])
			{
				localPanning = currentSeqModValues.panning;
			}
			else
			{
				localPanning = mtProject.instrument[currentInstrument_idx].panning;
			}

			if((int)(localPanning + localPanningMod) < PANNING_MIN) localPanning = PANNING_MIN;
			else if((int)(localPanning+ localPanningMod) >= PANNING_MAX) localPanning = PANNING_MAX;
			else localPanning += localPanningMod;

			modPanning(localPanning);
		}
		if(statusBytes & CUTOFF_MASK)
		{
			statusBytes &= (~CUTOFF_MASK);
			if((!mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
			{
				currentEnvelopeModification[envFilter] = 0;
			}

			float localCutoff = 0.0f;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				localCutoff = currentPerformanceValues.filterCutoff;
			}
			else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] ||
					trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff])
			{
				localCutoff = currentSeqModValues.filterCutoff;
			}
			else
			{
				localCutoff = mtProject.instrument[currentInstrument_idx].cutOff;
			}

			if(mtProject.instrument[currentInstrument_idx].cutOff + currentEnvelopeModification[envFilter] < 0.0f) localCutoff = 0.0f;
			else if(mtProject.instrument[currentInstrument_idx].cutOff + currentEnvelopeModification[envFilter] > 1.0f) localCutoff = 1.0f;
			else localCutoff += currentEnvelopeModification[envFilter];

			modCutoff(localCutoff);
		}
		if(statusBytes & RESONANCE_MASK)
		{
			statusBytes &= (~RESONANCE_MASK);
			modResonance(mtProject.instrument[currentInstrument_idx].resonance);
		}
		if(statusBytes & REVERB_SEND_MASK)
		{
			statusBytes &= (~REVERB_SEND_MASK);
			if(((muteState == 0) && (onlyReverbMuteState == 0)) || (engine.forceSend == 1))
			{
				modReverbSend(mtProject.instrument[currentInstrument_idx].reverbSend);
			}
		}
		if(statusBytes & WT_POS_SEND_MASK)
		{
			statusBytes &= (~WT_POS_SEND_MASK);
			if((!mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
			{
				currentEnvelopeModification[envWtPos] = 0;
			}

			int32_t localWtPosisionMod = currentEnvelopeModification[envWtPos] * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;
			uint32_t localWtPos = 0;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
			{
				localWtPos = currentPerformanceValues.wavetablePosition;
			}
			else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
					trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::wavetablePosition])
			{
				localWtPos = currentSeqModValues.wavetablePosition;
			}
			else
			{
				localWtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
			}

			if( (int)(localWtPos + localWtPosisionMod) < 0)
			{
				localWtPos = 0;
			}
			else if( (int)(localWtPos+ localWtPosisionMod) > (int)(mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1))
			{
				localWtPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
			}
			else localWtPos += localWtPosisionMod;

			playMemPtr->setWavetableWindow(localWtPos);
		}
		if(statusBytes & GRANULAR_POS_SEND_MASK)
		{
			statusBytes &= (~GRANULAR_POS_SEND_MASK);
			if((!mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition]))
			{
				currentEnvelopeModification[envGranPos] = 0;
			}

			int32_t localGranPosisionMod = currentEnvelopeModification[envGranPos] * MAX_16BIT;

			uint16_t localGranPosition = 0;
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
			{
				localGranPosition = currentPerformanceValues.granularPosition;
			}
			else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
					trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::granularPosition])
			{
				localGranPosition = currentSeqModValues.granularPosition;
			}
			else
			{
				localGranPosition = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
			}

			if((int)(localGranPosition + localGranPosisionMod) < 0) localGranPosition = 0;
			else if((int)(localGranPosition + localGranPosisionMod) >= MAX_16BIT) localGranPosition = MAX_16BIT;
			else localGranPosition += localGranPosisionMod;

			modGranularPosition(localGranPosition);
		}
		if(statusBytes & GRANULAR_LEN_SEND_MASK)
		{
			statusBytes &= (~GRANULAR_LEN_SEND_MASK);
			modGranularGrainLength();
		}
		if(statusBytes & GRANULAR_WAVE_SEND_MASK)
		{
			statusBytes &= (~GRANULAR_WAVE_SEND_MASK);
			playMemPtr->setGranularWave(mtProject.instrument[currentInstrument_idx].granular.shape);
		}
		if(statusBytes & GRANULAR_LOOP_SEND_MASK)
		{
			statusBytes &= (~GRANULAR_LOOP_SEND_MASK);
			playMemPtr->setGranularLoopMode(mtProject.instrument[currentInstrument_idx].granular.type);
		}
		if(statusBytes & LFO_AMP_SEND_MASK)
		{
			statusBytes &= (~LFO_AMP_SEND_MASK);

			if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp]))
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp]);

					envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
					envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
					envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
					envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
					envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
					envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
					envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);
				}
			}

			if(muteState == 0)
			{
				uint8_t currentVolume = 0;

				if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
				{
					currentVolume = currentPerformanceValues.volume;
				}
				else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
						trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume])
				{
					currentVolume = currentSeqModValues.volume;
				}
				else
				{
					currentVolume = mtProject.instrument[currentInstrument_idx].volume;
				}

				ampPtr->gain( (currentVolume/100.0) * lfoBasedEnvelope[envAmp].amount);
			}
		}

		if(statusBytes & LFO_FILTER_SEND_MASK)
		{
			statusBytes &= (~LFO_FILTER_SEND_MASK);

			if(mtProject.instrument[currentInstrument_idx].filterEnable)
			{
				if((mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)
				&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff])
				&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
				&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
				{
					if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)
					{
						calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[currentInstrument_idx].lfo[envFilter]);
						envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);
					}
				}
			}

		}

		if(statusBytes & LFO_WT_POS_SEND_MASK)
		{
			statusBytes &= (~LFO_WT_POS_SEND_MASK);

			if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
			{
				if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
				&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition])
				&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
				&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
				{
					if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
					{
						calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos]);
						envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);
					}
				}
			}
		}

		if(statusBytes & LFO_GRAN_POS_SEND_MASK)
		{
			statusBytes &= (~LFO_GRAN_POS_SEND_MASK);
			if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
			{
				if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
				&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition])
				&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
				&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
				{
					if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
					{
						calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos]);
						envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);
					}
				}

			}

		}

		if(statusBytes & LFO_PANNING_SEND_MASK)
		{
			statusBytes &= (~LFO_PANNING_SEND_MASK);

			if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan]);
					envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);
				}
			}

		}
	}
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

float playerEngine :: fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t playerEngine :: noteOnforPrev (uint8_t instr_idx,int8_t note,int8_t velocity)
{
	__disable_irq();
	uint8_t status;
	float gainL=0,gainR=0;
	engine.clearReverb();
	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX; i++)
	{
		envelopePtr[envelopesWithoutAmpIdx[i]]->kill();
	}

	currentInstrument_idx=instr_idx;
	currentNote=note;
	/*================================================ENVELOPE AMP==========================================*/
//	lfoAmpPtr->init(&mtProject.instrument[instr_idx].lfo[lfoA]);

	if(mtProject.instrument[instr_idx].envelope[envAmp].enable)
	{
		if(mtProject.instrument[instr_idx].envelope[envAmp].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[instr_idx].lfo[envAmp]);

			envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
			envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
			envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
			envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
			envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
			envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
			envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);
		}
		else
		{
			envelopeAmpPtr->delay(mtProject.instrument[instr_idx].envelope[envAmp].delay);
			envelopeAmpPtr->attack(mtProject.instrument[instr_idx].envelope[envAmp].attack);
			envelopeAmpPtr->hold(mtProject.instrument[instr_idx].envelope[envAmp].hold);
			envelopeAmpPtr->decay(mtProject.instrument[instr_idx].envelope[envAmp].decay);
			envelopeAmpPtr->sustain(mtProject.instrument[instr_idx].envelope[envAmp].sustain);
			envelopeAmpPtr->release(mtProject.instrument[instr_idx].envelope[envAmp].release);
			envelopeAmpPtr->setLoop(mtProject.instrument[instr_idx].envelope[envAmp].loop);
		}

	}
	else
	{
		envelopeAmpPtr->delay(0);
		envelopeAmpPtr->attack(0);
		envelopeAmpPtr->hold(0);
		envelopeAmpPtr->decay(0);
		envelopeAmpPtr->sustain(1.0);
		envelopeAmpPtr->release(0.0f);
		envelopeAmpPtr->setLoop(0);
	}

	/*======================================================================================================*/
	/*================================================ENVELOPE FILTER=======================================*/

	if(mtProject.instrument[instr_idx].filterEnable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[instr_idx].lfo[envFilter]);
				envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);
			}
			else
			{
				envelopePtr[envFilter]->init(&mtProject.instrument[instr_idx].envelope[envFilter]);
			}
		}

//		lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
	/*======================================================================================================*/
	/*================================================FILTER================================================*/
		filterConnect();
		changeFilterType(mtProject.instrument[instr_idx].filterType);
		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);
		filterPtr->setCutoff(mtProject.instrument[instr_idx].cutOff);
	}
	else if(!mtProject.instrument[instr_idx].filterEnable) filterDisconnect();

	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		currentEnvelopeModification[envWtPos] = 0;

		if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[instr_idx].lfo[envWtPos]);
				envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);
			}
			else
			{
				envelopePtr[envWtPos]->init(&mtProject.instrument[instr_idx].envelope[envWtPos]);
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
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[instr_idx].lfo[envGranPos]);
				envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);
			}
			else
			{
				envelopePtr[envGranPos]->init(&mtProject.instrument[instr_idx].envelope[envGranPos]);
			}
		}

	}


	if(mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[instr_idx].lfo[envPan]);
			envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);
		}
		else
		{
			envelopePtr[envPan]->init(&mtProject.instrument[instr_idx].envelope[envPan]);
		}
	}



	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	float localAmount = mtProject.instrument[instr_idx].envelope[envAmp].loop ? lfoBasedEnvelope[envAmp].amount : mtProject.instrument[instr_idx].envelope[envAmp].amount;
	if(velocity < 0)
	{
		ampPtr->gain(localAmount * (mtProject.instrument[instr_idx].volume/100.0));
	}
	else
	{
		ampPtr->gain( (velocity/100.0) * localAmount);
	}
	/*======================================================================================================*/
	/*===============================================PANNING================================================*/
	if(mtProject.instrument[instr_idx].panning < 50)
	{
		gainR=(0+mtProject.instrument[instr_idx].panning)/50.0;
		gainL=1.0;
	}
	else if(mtProject.instrument[instr_idx].panning > 50)
	{
		gainR=1.0;
		gainL=(100-mtProject.instrument[instr_idx].panning)/50.0;
	}
	else if(mtProject.instrument[instr_idx].panning == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(nChannel,gainL);
	mixerR.gain(nChannel,gainR);

	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/

	mixerReverb.gain(nChannel,mtProject.instrument[instr_idx].reverbSend/100.0);


	/*======================================================================================================*/

	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX; i++)
	{
		if(mtProject.instrument[instr_idx].envelope[envelopesWithoutAmpIdx[i]].enable) envelopePtr[envelopesWithoutAmpIdx[i]]->start();
	}

	status = playMemPtr->playForPrev(instr_idx,note);
	envelopeAmpPtr->noteOn();

	return status;
	__enable_irq();
}


uint8_t playerEngine :: noteOnforPrev (int16_t * addr, uint32_t len,uint8_t type)
{
	uint8_t status=0;
	envelopeAmpPtr->delay(0);
	envelopeAmpPtr->attack(0);
	envelopeAmpPtr->hold(0);
	envelopeAmpPtr->decay(0);
	envelopeAmpPtr->sustain(1.0);
	envelopeAmpPtr->release(0.0f);
	envelopeAmpPtr->setLoop(0);

	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX; i++)
	{
		envelopePtr[envelopesWithoutAmpIdx[i]]->kill();
	}

	engine.clearReverb();

	filterDisconnect();
	ampPtr->gain(1.0);

	mixerL.gain(nChannel,1.0);
	mixerR.gain(nChannel,1.0);
	mixerReverb.gain(nChannel,0.0);
	/*======================================================================================================*/
	limiter[0].setAttack(300);
	limiter[0].setRelease(10);
	limiter[0].setThreshold(32000);
	limiter[1].setAttack(300);
	limiter[1].setRelease(10);
	limiter[1].setThreshold(32000);
	bitDepthControl[0].setBitDepth(16);
	bitDepthControl[1].setBitDepth(16);

	status = playMemPtr->playForPrev(addr,len,type);
	envelopeAmpPtr->noteOn();

	return status;

}

uint8_t playerEngine :: noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note, uint8_t type)
{
	uint8_t status=0;
	envelopeAmpPtr->delay(0);
	envelopeAmpPtr->attack(0);
	envelopeAmpPtr->hold(0);
	envelopeAmpPtr->decay(0);
	envelopeAmpPtr->sustain(1.0);
	envelopeAmpPtr->release(0.0f);
	envelopeAmpPtr->setLoop(0);

	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX; i++)
	{
		envelopePtr[envelopesWithoutAmpIdx[i]]->kill();
	}

	filterDisconnect();
	ampPtr->gain(1.0);
	engine.clearReverb();

	mixerL.gain(nChannel,1.0);
	mixerR.gain(nChannel,1.0);
	mixerReverb.gain(nChannel,0.0);
	/*======================================================================================================*/
	limiter[0].setAttack(300);
	limiter[0].setRelease(10);
	limiter[0].setThreshold(32000);
	limiter[1].setAttack(300);
	limiter[1].setRelease(10);
	limiter[1].setThreshold(32000);
	bitDepthControl[0].setBitDepth(16);
	bitDepthControl[1].setBitDepth(16);

	status = playMemPtr->playForPrev(addr,len,note,type);
	envelopeAmpPtr->noteOn();

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

uint16_t playerEngine ::getWavePosition()
{
	return playMemPtr->getPosition();
}
//**************************************************************************************************************************************
//state: 1 - solo ON, 0 - solo OFF
void audioEngine::soloTrack(uint8_t channel, uint8_t state)
{
	if(channel >= 8 ) return;

	if(state == 1)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			if(i == channel)
			{
				muteReverbSend(i, 1);
				continue;
			}
			muteTrack(i,1);
		}
	}
	else if(state == 0)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			if(i == channel)
			{
				muteReverbSend(i, 0);
				continue;
			}
			muteTrack(i,0);
		}
	}

}

// selectLR :  0 - L, 1- R, state: 1 - solo ON, 0 - solo OFF
void audioEngine::soloReverbSend(uint8_t state)
{

	if(state == 1)
	{
		forceSend = 1;
		for(uint8_t i = 0; i < 8; i++)
		{
			muteTrack(i,1);
		}
	}
	else if(state == 0)
	{
		forceSend = 0;
		for(uint8_t i = 0; i < 8; i++)
		{
			muteTrack(i,0);
		}
	}

}

void audioEngine::clearReverb()
{
	reverb.clearFilters();
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
		if(!forceSend) instrumentPlayer[channel].modReverbSend(0);
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
		instrumentPlayer[channel].setStatusBytes(REVERB_SEND_MASK);
	}
	else
	{
		instrumentPlayer[channel].muteState = 1;
		amp[channel].gain(AMP_MUTED);
		if(!forceSend) instrumentPlayer[channel].modReverbSend(AMP_MUTED);
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
			localWTPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
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

void playerEngine::calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo, uint8_t rate)
{
	env->loop = 1;
	env->enable = 1;
	env->amount = lfo->amount;

	if(rate > 19) rate = 19;

	float lfoFrequency = sequencer.getActualTempo()/240.0/tempoSyncRates[rate];

	float periodTime = 1000 / lfoFrequency;


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
	}
}

void playerEngine::calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo)
{
	env->loop = 1;
	env->enable = 1;
	env->amount = lfo->amount;

	float lfoFrequency = sequencer.getActualTempo()/240.0/tempoSyncRates[lfo->speed];

	float periodTime = 1000 / lfoFrequency;


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
	}
}



void playerEngine::setPassEnvelope(uint8_t state)
{
	envelopeAmpPtr->setPassFlag(state);
	envelopePassFlag = state;
}

void playerEngine::printLog(SdFile * log)
{
	uint8_t nr_voice = ((uint32_t)this - (uint32_t)instrumentPlayer)/sizeof(playerEngine);
	log->printf("******************voice nr %d******************\n",nr_voice);
	log->println("kluczowe parametry aktualnego instrumentu");
	log->printf("index: %d\n", currentInstrument_idx);
	log->printf("length: %d\n", mtProject.instrument[currentInstrument_idx].sample.length);
	log->printf("addr: %x\n", (uint32_t)mtProject.instrument[currentInstrument_idx].sample.address);
	log->printf("flaga active: %d\n", mtProject.instrument[currentInstrument_idx].isActive);
	log->printf("currentNote %d\n", currentNote);
	log->printf("play mode: %d\n",  mtProject.instrument[currentInstrument_idx].playMode);
	if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		log->printf("gran pos: %d\n",  mtProject.instrument[currentInstrument_idx].granular.currentPosition);
		log->printf("gran len: %d\n",  mtProject.instrument[currentInstrument_idx].granular.grainLength);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable)
	{
		log->printf("wt pos: %d\n",  mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow);
		log->printf("wt window size: %d\n",  mtProject.instrument[currentInstrument_idx].sample.wavetable_window_size);
		log->printf("wt window number: %d\n",  mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeSlice)
	{
		log->printf("slice n: %d\n", mtProject.instrument[currentInstrument_idx].selectedSlice);
		log->printf("slice val: %d\n",  mtProject.instrument[currentInstrument_idx].slices[mtProject.instrument[currentInstrument_idx].selectedSlice]);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == singleShot)
	{
		log->printf("SP: %d\n", mtProject.instrument[currentInstrument_idx].startPoint);
		log->printf("EP: %d\n",  mtProject.instrument[currentInstrument_idx].endPoint);
	}
	else
	{
		log->printf("SP: %d\n", mtProject.instrument[currentInstrument_idx].startPoint);
		log->printf("EP: %d\n",  mtProject.instrument[currentInstrument_idx].endPoint);
		log->printf("LP1: %d\n", mtProject.instrument[currentInstrument_idx].loopPoint1);
		log->printf("LP2: %d\n",  mtProject.instrument[currentInstrument_idx].loopPoint2);
	}

	playMemPtr->printLog(log);

}


