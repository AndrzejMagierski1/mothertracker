#include "mtAudioEngine.h"
#include "sampleRecorder.h"
#include "mtSequencer.h"
extern AudioControlSGTL5000 audioShield;
static cSampleRecorder* SR = &sampleRecorder;

typedef Sequencer::strFxConsts fx_t;


AudioInputI2S            i2sIn;
AudioRecordQueue         queue;
AudioOutputI2S           i2sOut;

AudioPlaySdWav           playSdWav;
AudioPlaySdWavFloat		 playSdWavFloat;
AudioPlaySdWav24bit 	 playSdWav24Bit;
AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
envelopeGenerator		 envelopeFilter[8];
envelopeGenerator		 envelopeWtPosition[8];
envelopeGenerator		 envelopeGranPosition[8];
LFO						 lfoAmp[8];
LFO						 lfoFilter[8];
LFO						 lfoPitch[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer9				 mixerL,mixerR,mixerReverb;

AudioMixer4              mixerRec;
AudioMixer4              mixerSourceL,mixerSourceR;
AudioEffectFreeverb		 reverb;
AudioEffectLimiter		 limiter[2];
AudioBitDepth			 bitDepthControl[2];
AudioAnalyzeRMS			 rms;
AudioRecordQueue		 exportL, exportR;
AudioAnalyzeRMS			 exportRmsL, exportRmsR;


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



playerEngine instrumentPlayer[8];

audioEngine engine;


constexpr uint16_t releaseNoteOnVal = 5;

void audioEngine::init()
{

	i2sConnect[0]= &connect59;
	i2sConnect[1]= &connect60;


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
	for(int i=0;i<8; i++)
	{
		instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&filter[i],&envelopeAmp[i], &amp[i], i, &lfoAmp[i],&lfoFilter[i],&lfoPitch[i],&envelopeWtPosition[i],&envelopeGranPosition[i]);
	}

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
		instrumentPlayer[i].endSamplePlaybackPerformanceMode()();
		instrumentPlayer[i].endStartPointPerformanceMode()();
		instrumentPlayer[i].endTunePerformanceMode()();
		instrumentPlayer[i].endVolumePerformanceMode()();
		instrumentPlayer[i].endWavetableWindowPerformanceMode();
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

void playerEngine::init(AudioPlayMemory * playMem,envelopeGenerator* envFilter,AudioFilterStateVariable * filter,
		AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh, LFO * lfoAmp, LFO * lfoFilter, LFO * lfoPitch,envelopeGenerator* envWtPos, envelopeGenerator * envGranPos)
{
	playMemPtr=playMem;
	envelopeAmpPtr=envAmp;
	envelopeFilterPtr=envFilter;
	envelopeWtPos=envWtPos;
	filterPtr=filter;
	ampPtr=amp;
	numPanChannel=panCh;
	lfoAmpPtr=lfoAmp;
	lfoFilterPtr=lfoFilter;
	lfoPitchPtr=lfoPitch;
	envelopeGranPos = envGranPos;
	envelopeAmpPtr->releaseNoteOn(releaseNoteOnVal);

}

uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity)
{
	if(mtProject.instrument[instr_idx].isActive != 1) return 0;

	endFx(lastSeqFx[0],0);
	endFx(lastSeqFx[1],1);
	__disable_irq();
	AudioNoInterrupts();
	uint8_t status;
	float gainL=0,gainR=0;

	envelopeFilterPtr->kill();
	envelopeWtPos->kill();
	envelopeGranPos->kill();

	currentInstrument_idx=instr_idx;
	currentNote=note;
	currentVelocity=velocity;
	/*================================================ENVELOPE AMP==========================================*/
	lfoAmpPtr->init(&mtProject.instrument[instr_idx].lfo[lfoA]);

	if(mtProject.instrument[instr_idx].envelope[envAmp].enable)
	{
		envelopeAmpPtr->delay(mtProject.instrument[instr_idx].envelope[envAmp].delay);
		envelopeAmpPtr->attack(mtProject.instrument[instr_idx].envelope[envAmp].attack);
		envelopeAmpPtr->hold(mtProject.instrument[instr_idx].envelope[envAmp].hold);
		envelopeAmpPtr->decay(mtProject.instrument[instr_idx].envelope[envAmp].decay);
		envelopeAmpPtr->sustain(mtProject.instrument[instr_idx].envelope[envAmp].sustain);
		envelopeAmpPtr->release(mtProject.instrument[instr_idx].envelope[envAmp].release);
		envelopeAmpPtr->setLoop(mtProject.instrument[instr_idx].envelope[envAmp].loop);
		//Serial.println(activeAmpEnvelopes);
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
	if(mtProject.instrument[instr_idx].filterEnable == filterOn)
	{
		instrumentBasedMod.cutoff = mtProject.instrument[instr_idx].cutOff;
		if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)
		{
			envelopeFilterPtr->init(&mtProject.instrument[instr_idx].envelope[envFilter]);
		}
		if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable)
		{
			lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
		}
		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);
	}

	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		instrumentBasedMod.wtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
		if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
		{
			envelopeWtPos->init(&mtProject.instrument[instr_idx].envelope[envWtPos]);
		}
	}

	if((mtProject.instrument[instr_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[instr_idx].playMode == playModeGranular))
	{
		instrumentBasedMod.granPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
		if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
		{
			envelopeGranPos->init(&mtProject.instrument[instr_idx].envelope[envGranPos]);
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff); // włączenie filtra jest w środku
	}
	else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] ||
			trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff])
	{
		filterConnect();
		filterPtr->setCutoff(currentSeqModValues.filterCutoff);
	}
	else
	{
		if(mtProject.instrument[instr_idx].filterEnable == filterOn)
		{
			filterConnect();
			filterPtr->setCutoff(mtProject.instrument[instr_idx].cutOff);
		}
		else if(mtProject.instrument[instr_idx].filterEnable == filterOff) filterDisconnect();

	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
	{
		changeFilterTypePerformanceMode(performanceMod.filterType);
	}
	else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterType] ||
			trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterType])
	{
		changeFilterType(mtProject.instrument[instr_idx].filterType);
	}
	else
	{
		if(mtProject.instrument[instr_idx].filterEnable == filterOn)
		{
			changeFilterType(mtProject.instrument[instr_idx].filterType);
		}
	}



	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	if(velocity < 0)
	{
		instrumentBasedMod.volume=(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0))*100;
	}
	else
	{
		instrumentBasedMod.volume=((velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount)*100;
	}
	if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable)
	{
		instrumentBasedMod.volume += lfoAmpPtr->getOut();
	}

	if(muteState == 0)
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
		{
			changeVolumePerformanceMode(performanceMod.volume);
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume])
		{
			ampPtr->gain( (currentSeqModValues.volume/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
		}
		else
		{
			if(velocity < 0)
			{
				ampPtr->gain(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0));
			}
			else
			{
				ampPtr->gain( (velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
			}
		}
	}
	/*======================================================================================================*/
	/*===============================================PANNING================================================*/


	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning])
	{
		if(currentSeqModValues.panning < 50)
		{
			gainR=(0+currentSeqModValues.panning)/50.0;
			gainL=1.0;
		}
		else if(currentSeqModValues.panning > 50)
		{
			gainR=1.0;
			gainL=(100-currentSeqModValues.panning)/50.0;
		}
		else if(currentSeqModValues.panning == 50)
		{
			gainL=1.0; gainR=1.0;
		}

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
	}
	else
	{
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

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
	}
	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/
	if(((muteState == 0) && (onlyReverbMuteState == 0)) || (engine.forceSend == 1))
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
		{
			changeReverbSendPerformanceMode(performanceMod.reverbSend);
		}
		else if (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend])
		{
			mixerReverb.gain(numPanChannel,currentSeqModValues.reverbSend/100.0);
		}
		else
		{
			mixerReverb.gain(numPanChannel,mtProject.instrument[instr_idx].reverbSend/100.0);
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
	{
		changeEndPointPerformanceMode(performanceMod.endPoint);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
	}


	/*======================================================================================================*/
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune])
	{
		changeTunePerformanceMode(performanceMod.tune);
	}
	status = playMemPtr->play(instr_idx,note);
	envelopeAmpPtr->noteOn();

	if(mtProject.instrument[instr_idx].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();
	if(mtProject.instrument[instr_idx].envelope[envWtPos].enable == envelopeOn) envelopeWtPos->start();
	if(mtProject.instrument[instr_idx].envelope[envGranPos].enable == envelopeOn) envelopeGranPos->start();

	__enable_irq();
	AudioInterrupts();
	return status;
}

uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx1_id, uint8_t fx1_val,
								uint8_t fx2_id, uint8_t fx2_val)
{
	if(mtProject.instrument[instr_idx].isActive != 1) return 0;
	__disable_irq();
	AudioNoInterrupts();
	uint8_t status;
	float gainL=0,gainR=0;

	currentInstrument_idx=instr_idx;
	currentNote=note;
	currentVelocity=velocity;

	envelopeFilterPtr->kill();
	envelopeWtPos->kill();
	envelopeGranPos->kill();
	/*================================================ENVELOPE AMP==========================================*/
	lfoAmpPtr->init(&mtProject.instrument[instr_idx].lfo[lfoA]);

	if(mtProject.instrument[instr_idx].envelope[envAmp].enable)
	{
		envelopeAmpPtr->delay(mtProject.instrument[instr_idx].envelope[envAmp].delay);
		envelopeAmpPtr->attack(mtProject.instrument[instr_idx].envelope[envAmp].attack);
		envelopeAmpPtr->hold(mtProject.instrument[instr_idx].envelope[envAmp].hold);
		envelopeAmpPtr->decay(mtProject.instrument[instr_idx].envelope[envAmp].decay);
		envelopeAmpPtr->sustain(mtProject.instrument[instr_idx].envelope[envAmp].sustain);
		envelopeAmpPtr->release(mtProject.instrument[instr_idx].envelope[envAmp].release);
		envelopeAmpPtr->setLoop(mtProject.instrument[instr_idx].envelope[envAmp].loop);
		//Serial.println(activeAmpEnvelopes);
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
	if(mtProject.instrument[instr_idx].filterEnable == filterOn)
	{
		instrumentBasedMod.cutoff = mtProject.instrument[instr_idx].cutOff;
		if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)
		{
			envelopeFilterPtr->init(&mtProject.instrument[instr_idx].envelope[envFilter]);
		}
		if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable)
		{
			lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
		}
		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);
	}

	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		instrumentBasedMod.wtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
		if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
		{
			envelopeWtPos->init(&mtProject.instrument[instr_idx].envelope[envWtPos]);
		}
	}

	if((mtProject.instrument[instr_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[instr_idx].playMode == playModeGranular))
	{
		instrumentBasedMod.granPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
		if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
		{
			envelopeGranPos->init(&mtProject.instrument[instr_idx].envelope[envGranPos]);
		}
	}


	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff); // włączenie filtra jest w środku
	}
	else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff])
	{
		filterConnect();
		filterPtr->setCutoff(currentSeqModValues.filterCutoff);
	}
	else
	{
		if(mtProject.instrument[instr_idx].filterEnable == filterOn)
		{
			filterConnect();
			filterPtr->setCutoff(mtProject.instrument[instr_idx].cutOff);
		}
		else if(mtProject.instrument[instr_idx].filterEnable == filterOff) filterDisconnect();

	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
	{
		changeFilterTypePerformanceMode(performanceMod.filterType);
	}
	else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterType])
	{
		changeFilterType(mtProject.instrument[instr_idx].filterType);
	}
	else
	{
		if(mtProject.instrument[instr_idx].filterEnable == filterOn)
		{
			changeFilterType(mtProject.instrument[instr_idx].filterType);
		}
	}



	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	if(velocity < 0)
	{
		instrumentBasedMod.volume=(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0))*100;
	}
	else
	{
		instrumentBasedMod.volume=((velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount)*100;
	}
	if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable)
	{
		instrumentBasedMod.volume += lfoAmpPtr->getOut();
	}

	if(muteState == 0)
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
		{
			changeVolumePerformanceMode(performanceMod.volume);
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume])
		{
			ampPtr->gain( (currentSeqModValues.volume/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
		}
		else
		{
			if(velocity < 0)
			{
				ampPtr->gain(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0));
			}
			else
			{
				ampPtr->gain( (velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
			}
		}
	}
	/*======================================================================================================*/
	/*===============================================PANNING================================================*/


	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning])
	{
		if(currentSeqModValues.panning < 50)
		{
			gainR=(0+currentSeqModValues.panning)/50.0;
			gainL=1.0;
		}
		else if(currentSeqModValues.panning > 50)
		{
			gainR=1.0;
			gainL=(100-currentSeqModValues.panning)/50.0;
		}
		else if(currentSeqModValues.panning == 50)
		{
			gainL=1.0; gainR=1.0;
		}

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
	}
	else
	{
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

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
	}
	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/
	if(((muteState == 0) && (onlyReverbMuteState == 0)) || (engine.forceSend == 1))
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
		{
			changeReverbSendPerformanceMode(performanceMod.reverbSend);
		}
		else if (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend])
		{
			mixerReverb.gain(numPanChannel,currentSeqModValues.reverbSend/100.0);
		}
		else
		{
			mixerReverb.gain(numPanChannel,mtProject.instrument[instr_idx].reverbSend/100.0);
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
	{
		changeEndPointPerformanceMode(performanceMod.endPoint);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
	}


	/*======================================================================================================*/
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune])
	{
		changeTunePerformanceMode(performanceMod.tune);
	}
	//*************************************************FX****************************************************
	seqFx(fx1_id,fx1_val,0);
	seqFx(fx2_id,fx2_val,1);
	//*******************************************************************************************************
	status = playMemPtr->play(instr_idx,note);
	envelopeAmpPtr->noteOn();

	if(mtProject.instrument[instr_idx].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();
	if(mtProject.instrument[instr_idx].envelope[envWtPos].enable == envelopeOn) envelopeWtPos->start();
	if(mtProject.instrument[instr_idx].envelope[envGranPos].enable == envelopeOn) envelopeGranPos->start();

	__enable_irq();
	AudioInterrupts();
	return status;
}


void playerEngine::noteOff(int8_t option)
{
	switch (option)
	{
	case Sequencer::STEP_NOTE_FADE:
		__disable_irq();
		AudioNoInterrupts();
		envelopeAmpPtr->release(300);
		envelopeAmpPtr->noteOff();
		envelopeFilterPtr->stop();
		envelopeWtPos->stop();
		envelopeGranPos->stop();
		AudioInterrupts();
		__enable_irq();
		break;
	case Sequencer::STEP_NOTE_CUT:
		__disable_irq();
		AudioNoInterrupts();
		envelopeAmpPtr->noteOff();
		envelopeAmpPtr->setIdle();
		envelopeFilterPtr->stop();
		envelopeFilterPtr->kill();
		envelopeWtPos->stop();
		envelopeWtPos->kill();
		envelopeGranPos->stop();
		envelopeGranPos->kill();
		playMemPtr->stop();

		AudioInterrupts();
		__enable_irq();
		break;
	default:
		__disable_irq();
		AudioNoInterrupts();
		envelopeAmpPtr->noteOff();
		envelopeFilterPtr->stop();
		envelopeWtPos->stop();
		envelopeGranPos->stop();
		if(!mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
		{
			playMemPtr->stop();
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].release == 0.0f)
			{
				playMemPtr->stop();
			}
		}
		AudioInterrupts();
		__enable_irq();
		break;
	}
}

void playerEngine::seqFx(uint8_t fx_id, uint8_t fx_val, uint8_t fx_n)
{
	AudioNoInterrupts();
	__disable_irq();
	endFx(lastSeqFx[fx_n],fx_n);

	uint8_t otherFx_n = !fx_n;

	switch(fx_id)
	{
		case 0:
			// na 0 mial wywolywac endFx ale wywoluje go zawsze i tak
		break;
		case fx_t::FX_TYPE_FILTER_BANDPASS :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 1;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 1;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 1;
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.filterCutoff = fx_val/(float)127;
				currentSeqModValues.filterType = bandPass;
				currentSeqModValues.filterEnable = 1;
			}
			else if( fx_n == LEAST_SIGNIFICANT_FX)
			{
				if( !trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
					!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					currentSeqModValues.filterCutoff = fx_val/(float)127;
					currentSeqModValues.filterType = bandPass;
					currentSeqModValues.filterEnable = 1;
				}
			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				if(currentPerformanceValues.filterType == bandPass)
				{
					changeCutoffPerformanceMode(performanceMod.cutoff);
				}
			}
			else
			{
				filterPtr->setCutoff(currentSeqModValues.filterCutoff);
				changeFilterType(currentSeqModValues.filterType);
				filterConnect();
			}
		break;
		case fx_t::FX_TYPE_FILTER_HIGHPASS :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 1;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 1;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 1;

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.filterCutoff = fx_val/(float)127;
				currentSeqModValues.filterType = highPass;
				currentSeqModValues.filterEnable = 1;
			}
			else if( fx_n == LEAST_SIGNIFICANT_FX)
			{
				if( !trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
					!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					currentSeqModValues.filterCutoff = fx_val/(float)127;
					currentSeqModValues.filterType = highPass;
					currentSeqModValues.filterEnable = 1;
				}
			}


			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				if(currentPerformanceValues.filterType == highPass)
				{
					changeCutoffPerformanceMode(performanceMod.cutoff);
				}
			}
			else
			{
				filterPtr->setCutoff(currentSeqModValues.filterCutoff);
				changeFilterType(currentSeqModValues.filterType);
				filterConnect();
			}
		break;
		case fx_t::FX_TYPE_FILTER_LOWPASS :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 1;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 1;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 1;

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.filterCutoff = fx_val/(float)127;
				currentSeqModValues.filterType = lowPass;
				currentSeqModValues.filterEnable = 1;
			}
			else if( fx_n == LEAST_SIGNIFICANT_FX)
			{
				if( !trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
					!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					currentSeqModValues.filterCutoff = fx_val/(float)127;
					currentSeqModValues.filterType = lowPass;
					currentSeqModValues.filterEnable = 1;
				}
			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				if(currentPerformanceValues.filterType == lowPass)
				{
					changeCutoffPerformanceMode(performanceMod.cutoff);
				}
			}
			else
			{
				filterPtr->setCutoff(currentSeqModValues.filterCutoff);
				changeFilterType(currentSeqModValues.filterType);
				filterConnect();
			}
		break;
		case fx_t::FX_TYPE_GLIDE :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.glide = (fx_val > 100) ? map(fx_val,100,255,1000,GLIDE_MAX) : map(fx_val,0,100,GLIDE_MIN,1000);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
				{
					currentSeqModValues.glide = (fx_val > 100) ? map(fx_val,100,255,1000,GLIDE_MAX) : map(fx_val,0,100,GLIDE_MIN,1000);
				}
			}
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::glide] = 1;

			playMemPtr->setGlideForceFlag();
			playMemPtr->setForcedGlide(currentSeqModValues.glide);
		break;
		case fx_t::FX_TYPE_MICROTUNING :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.fineTune = map(fx_val,0,127,MIN_INSTRUMENT_FINETUNE,MAX_INSTRUMENT_FINETUNE);
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
				{
					int8_t lfoOut = 100*lfoPitchPtr->getOut();
					if(currentSeqModValues.fineTune + lfoOut >= MAX_INSTRUMENT_FINETUNE) currentSeqModValues.fineTune = MAX_INSTRUMENT_FINETUNE;
					else if(currentSeqModValues.fineTune + lfoOut <= MIN_INSTRUMENT_FINETUNE) currentSeqModValues.fineTune = MIN_INSTRUMENT_FINETUNE;
					else currentSeqModValues.fineTune += lfoOut;
				}

			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune])
				{
					if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
					{
						int8_t lfoOut = 100*lfoPitchPtr->getOut();
						if(currentSeqModValues.fineTune + lfoOut >= MAX_INSTRUMENT_FINETUNE) currentSeqModValues.fineTune = MAX_INSTRUMENT_FINETUNE;
						else if(currentSeqModValues.fineTune + lfoOut <= MIN_INSTRUMENT_FINETUNE) currentSeqModValues.fineTune = MIN_INSTRUMENT_FINETUNE;
						else currentSeqModValues.fineTune += lfoOut;
					}
					currentSeqModValues.fineTune = map(fx_val,0,127,MIN_INSTRUMENT_FINETUNE,MAX_INSTRUMENT_FINETUNE);
				}
			}
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::fineTune] = 1;

			playMemPtr->setFineTuneForceFlag();
			playMemPtr->setForcedFineTune(currentSeqModValues.fineTune);
		break;
		case fx_t::FX_TYPE_PANNING :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.panning = map(fx_val,0,100,PANNING_MIN,PANNING_MAX);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
				{
					currentSeqModValues.panning = map(fx_val,0,127,PANNING_MIN,PANNING_MAX);
				}
			}
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::panning] = 1;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
			{
				changePanningPerformanceMode(performanceMod.panning);
			}
			else
			{
				modPanning(currentSeqModValues.panning);
			}

		break;
		case fx_t::FX_TYPE_REVERB_SEND :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.reverbSend = map(fx_val,0,127,REVERB_SEND_MIN,REVERB_SEND_MAX);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
				{
					currentSeqModValues.reverbSend = map(fx_val,0,127,REVERB_SEND_MIN,REVERB_SEND_MAX);
				}
			}
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::reverbSend] = 1;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
			{
				changeReverbSendPerformanceMode(performanceMod.reverbSend);
			}
			else
			{
				modReverbSend(currentSeqModValues.reverbSend);
			}

		break;
		case fx_t::FX_TYPE_REVERSE_PLAYBACK :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				playMemPtr->setReverse();
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection])
				{
					playMemPtr->setReverse();
				}
			}
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::samplePlaybeckDirection] = 1;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection]) playMemPtr->clearReverse();
		break;
		case fx_t::FX_TYPE_SAMPLE_START :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::startPoint] = 1;
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.startPoint = map(fx_val,0,127,0,MAX_16BIT);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
				{
					currentSeqModValues.startPoint = map(fx_val,0,127,0,MAX_16BIT);
				}
			}



			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint]) modSeqPoints(currentSeqModValues.startPoint, currentSeqModValues.endPoint);
			else modSeqPoints(currentSeqModValues.startPoint , NOT_MOD_POINTS);

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
			{
				changeStartPointPerformanceMode(performanceMod.startPoint);
			}

		break;
		case fx_t::FX_TYPE_TREMOLO_FAST :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 1;
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.tremolo.amount = map(fx_val,0,127,0,4095);
				currentSeqModValues.tremolo.enable = 1;
				currentSeqModValues.tremolo.rate = 2048;
				lfoAmpPtr->init(&currentSeqModValues.tremolo);
				lfoAmpPtr->start();
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
				{
					currentSeqModValues.tremolo.amount = map(fx_val,0,127,0,4095);
					currentSeqModValues.tremolo.enable = 1;
					currentSeqModValues.tremolo.rate = 2048;
					lfoAmpPtr->init(&currentSeqModValues.tremolo);
					lfoAmpPtr->start();
				}
			}

		break;
		case fx_t::FX_TYPE_TREMOLO_SLOW :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 1;

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.tremolo.amount = map(fx_val,0,127,0,4095);
				currentSeqModValues.tremolo.enable = 1;
				currentSeqModValues.tremolo.rate = 256;
				lfoAmpPtr->init(&currentSeqModValues.tremolo);
				lfoAmpPtr->start();
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
				{
					currentSeqModValues.tremolo.amount = map(fx_val,0,127,0,4095);
					currentSeqModValues.tremolo.enable = 1;
					currentSeqModValues.tremolo.rate = 256;
					lfoAmpPtr->init(&currentSeqModValues.tremolo);
					lfoAmpPtr->start();
				}
			}


		break;
		case fx_t::FX_TYPE_VIBRATO_FAST :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFineTune] = 1;

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.vibrato.amount = map(fx_val,0,127,0,4095);
				currentSeqModValues.vibrato.enable = 1;
				currentSeqModValues.vibrato.rate = 2048;
				playMemPtr->setFineTuneForceFlag();
				playMemPtr->setForcedFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
				lfoPitchPtr->init(&currentSeqModValues.vibrato);
				lfoPitchPtr->start();
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
				{
					currentSeqModValues.vibrato.amount = map(fx_val,0,127,0,4095);
					currentSeqModValues.vibrato.enable = 1;
					currentSeqModValues.vibrato.rate = 2048;
					playMemPtr->setFineTuneForceFlag();
					playMemPtr->setForcedFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
					lfoPitchPtr->init(&currentSeqModValues.vibrato);
					lfoPitchPtr->start();
				}
			}
		break;
		case fx_t::FX_TYPE_VIBRATO_SLOW :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFineTune] = 1;
				currentSeqModValues.vibrato.amount = map(fx_val,0,127,0,4095);
				currentSeqModValues.vibrato.enable = 1;
				currentSeqModValues.vibrato.rate = 256;
				playMemPtr->setFineTuneForceFlag();
				playMemPtr->setForcedFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
				lfoPitchPtr->init(&currentSeqModValues.vibrato);
				lfoPitchPtr->start();
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
				{
					trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFineTune] = 1;
					currentSeqModValues.vibrato.amount = map(fx_val,0,127,0,4095);
					currentSeqModValues.vibrato.enable = 1;
					currentSeqModValues.vibrato.rate = 256;
					playMemPtr->setFineTuneForceFlag();
					playMemPtr->setForcedFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
					lfoPitchPtr->init(&currentSeqModValues.vibrato);
					lfoPitchPtr->start();
				}
			}
		break;
		case fx_t::FX_TYPE_SAMPLE_END :

			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.endPoint = map(fx_val,0,127,0,MAX_16BIT);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint])
				{
					currentSeqModValues.endPoint = map(fx_val,0,127,0,MAX_16BIT);
				}
			}
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::endPoint] = 1;

			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint]) modSeqPoints(currentSeqModValues.startPoint, currentSeqModValues.endPoint);
			else modSeqPoints(NOT_MOD_POINTS,currentSeqModValues.endPoint);

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
			{
				changeEndPointPerformanceMode(performanceMod.endPoint);
			}
		break;
		case fx_t::FX_TYPE_WT_POSITION :
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.wavetablePosition = map(fx_val,0,127,0,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::wavetablePosition])
				{
					currentSeqModValues.wavetablePosition = map(fx_val,0,127,0,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
				}
			}

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::wavetablePosition] = 1;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
			{
				changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
			}
			else
			{
				playMemPtr->setWavetableWindowFlag();
				playMemPtr->setForcedWavetableWindow(currentSeqModValues.wavetablePosition);
				playMemPtr->setWavetableWindow(currentSeqModValues.wavetablePosition);
			}
		break;
		case fx_t::FX_TYPE_GRANULAR_POSITION :
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.granularPosition = map(fx_val,0,127,0,MAX_16BIT);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::granularPosition])
				{
					currentSeqModValues.granularPosition = map(fx_val,0,127,0,MAX_16BIT);
				}
			}

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::granularPosition] = 1;

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
			{
				changeGranularPositionPerformanceMode(performanceMod.granularPosition);
			}
			else
			{
				playMemPtr->setGranularPosForceFlag();
				playMemPtr->setForcedGranularPos(currentSeqModValues.granularPosition);
				playMemPtr->setGranularPosition(currentSeqModValues.granularPosition);
			}
		break;
		case fx_t::FX_TYPE_VELOCITY:
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::volume] = 1;
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.volume = map(fx_val,0,127,0,MAX_INSTRUMENT_VOLUME);
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
				{
					currentSeqModValues.volume = map(fx_val,0,127,0,MAX_INSTRUMENT_VOLUME);
				}
			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
			{

				changeVolumePerformanceMode(performanceMod.volume);
			}
			else
			{
				ampPtr->gain( (currentSeqModValues.volume/100.0) * mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount);
			}
		break;
		case fx_t::FX_TYPE_SAMPLE_SLICE:
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.slice = fx_val > mtProject.instrument[currentInstrument_idx].sliceNumber - 1 ?
				mtProject.instrument[currentInstrument_idx].sliceNumber - 1: fx_val;
			}
			else if(fx_n == LEAST_SIGNIFICANT_FX)
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::slice])
				{
					currentSeqModValues.slice = fx_val > mtProject.instrument[currentInstrument_idx].sliceNumber - 1 ?
					mtProject.instrument[currentInstrument_idx].sliceNumber - 1: fx_val;
				}
			}

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::slice] = 1;

			playMemPtr->setSliceForcedFlag();
			playMemPtr->setForcedSlice(currentSeqModValues.slice);
		break;
		default:
		break;
	}
	lastSeqFx[fx_n] = fx_id;
	lastSeqVal[fx_n] = fx_val;
	AudioInterrupts();
	__enable_irq();
}
//*** Jeżeli kończy się mniej znaczący efekt mimo, że aktywny jest bardziej znaczący efekt to nie wykonujemy żadnej akcji poza wyzerowaniem flag ***/
//*** Jeżeli kończy się bardziej znaczący efekt i aktywny jest mniej znaczący efekt to mniej znaczący efekt przejmuje kontrolę lub performanceMode wykonuje
// swoje operacje na podstawie wartości mniej znaczącego efektu ***/
void playerEngine::endFx(uint8_t fx_id, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;
	switch(fx_id)
	{
		case 0:

			break;
		case fx_t::FX_TYPE_FILTER_BANDPASS :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 0;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 0;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 0;

			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					currentSeqModValues.filterCutoff = lastSeqVal[otherFx_n]/(float)127;
					currentSeqModValues.filterType = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_BANDPASS ?  bandPass
							: lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_HIGHPASS ? highPass : lowPass;
					currentSeqModValues.filterEnable = 1;
				}

			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				changeCutoffPerformanceMode(performanceMod.cutoff);
			}
			else
			{
				if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						filterPtr->setCutoff(currentSeqModValues.filterCutoff);
					}
				}
				else
				{
					filterPtr->setCutoff(instrumentBasedMod.cutoff);
					if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
					else filterConnect();
				}
			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
			{
				changeFilterTypePerformanceMode(performanceMod.filterType);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						changeFilterType(currentSeqModValues.filterType);
					}
				}
				else changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
			}

		break;
		case fx_t::FX_TYPE_FILTER_HIGHPASS :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 0;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 0;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 0;

			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					currentSeqModValues.filterCutoff = lastSeqVal[otherFx_n]/(float)127;
					currentSeqModValues.filterType = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_BANDPASS ?  bandPass
							: lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_HIGHPASS ? highPass : lowPass;
					currentSeqModValues.filterEnable = 1;
				}

			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				changeCutoffPerformanceMode(performanceMod.cutoff);
			}
			else
			{
				if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						filterPtr->setCutoff(currentSeqModValues.filterCutoff);
					}
				}
				else
				{
					filterPtr->setCutoff(mtProject.instrument[currentInstrument_idx].cutOff);
					if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
					else filterConnect();
				}
			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
			{
				changeFilterTypePerformanceMode(performanceMod.filterType);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						changeFilterType(currentSeqModValues.filterType);
					}
				}
				else changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
			}
		break;
		case fx_t::FX_TYPE_FILTER_LOWPASS :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 0;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 0;
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 0;

			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					currentSeqModValues.filterCutoff = lastSeqVal[otherFx_n]/(float)127;
					currentSeqModValues.filterType = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_BANDPASS ?  bandPass
							: lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_HIGHPASS ? highPass : lowPass;
					currentSeqModValues.filterEnable = 1;
				}

			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
			{
				changeCutoffPerformanceMode(performanceMod.cutoff);
			}
			else
			{
				if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
					trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						filterPtr->setCutoff(currentSeqModValues.filterCutoff);
					}
				}
				else
				{
					filterPtr->setCutoff(mtProject.instrument[currentInstrument_idx].cutOff);
					if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
					else filterConnect();
				}
			}

			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
			{
				changeFilterTypePerformanceMode(performanceMod.filterType);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						changeFilterType(currentSeqModValues.filterType);
					}
				}
				else changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);

			}
		break;
		case fx_t::FX_TYPE_GLIDE:
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::glide] = 0;

			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
				{
					currentSeqModValues.glide = (lastSeqVal[otherFx_n] > 100) ? map(lastSeqVal[otherFx_n],100,255,1000,GLIDE_MAX) : map(lastSeqVal[otherFx_n],0,100,GLIDE_MIN,1000);
					playMemPtr->setForcedGlide(currentSeqModValues.glide);
				}
			}
			else
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
				{
					playMemPtr->clearGlideForceFlag();
					playMemPtr->setForcedGlide(0);
				}


			}
		break;
		case fx_t::FX_TYPE_MICROTUNING :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::fineTune] = 0;

			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune])
				{
					currentSeqModValues.fineTune = map(lastSeqVal[otherFx_n],0,127,MIN_INSTRUMENT_FINETUNE,MAX_INSTRUMENT_FINETUNE);
					playMemPtr->setFineTuneForceFlag();
					playMemPtr->setForcedFineTune(currentSeqModValues.fineTune);
				}
			}
			else
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune] && !trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune] )
				{
					playMemPtr->clearFineTuneForceFlag();
					modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
				}
			}

			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
			{
				playMemPtr->setFineTuneForceFlag();
				playMemPtr->setForcedFineTune(lfoPitch->getOut() * 100);
			}


		break;
		case fx_t::FX_TYPE_PANNING :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::panning] = 0;
			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.panning = map(lastSeqVal[otherFx_n],0,127,PANNING_MIN,PANNING_MAX);
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
			{
				changePanningPerformanceMode(performanceMod.panning);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						modPanning(currentSeqModValues.panning);
					}
				}
				else
				{
					modPanning(mtProject.instrument[currentInstrument_idx].panning);
				}
			}
		break;
		case fx_t::FX_TYPE_REVERB_SEND :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::reverbSend] = 0;
			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.reverbSend = map(lastSeqVal[otherFx_n],0,127,REVERB_SEND_MIN,REVERB_SEND_MAX);
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
			{
				changeReverbSendPerformanceMode(performanceMod.reverbSend);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						modReverbSend(currentSeqModValues.reverbSend);
					}
				}
				else
				{
					modReverbSend(mtProject.instrument[currentInstrument_idx].reverbSend);
				}
			}

		break;
		case fx_t::FX_TYPE_REVERSE_PLAYBACK :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::samplePlaybeckDirection] = 0;


			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection])
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection]) playMemPtr->setReverse();
				else playMemPtr->clearReverse();
			}
			else
			{
				if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection]) playMemPtr->clearReverse();
				else playMemPtr->setReverse();
			}
		break;
		case fx_t::FX_TYPE_SAMPLE_START :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::startPoint] = 0;


			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
				{
					currentSeqModValues.startPoint = map(lastSeqVal[otherFx_n],0,127,0,MAX_16BIT);
				}
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
			{
				changeStartPointPerformanceMode(performanceMod.startPoint);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint])
				{
					modSeqPoints(NOT_MOD_POINTS,currentSeqModValues.endPoint);
				}
				else if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						modSeqPoints(currentSeqModValues.startPoint, NOT_MOD_POINTS);
					}
				}
				else
				{
					playMemPtr->clearPointsForceFlag();
					playMemPtr->setForcedPoints(-1, -1, -1, -1);
				}
			}

		break;
		case fx_t::FX_TYPE_TREMOLO_FAST :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 0;
			lfoAmpPtr->stop();
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
			{
				if(fx_id == MOST_SIGNIFICANT_FX)
				{
					lfoAmpPtr->stop();
					currentSeqModValues.tremolo.amount = map(lastSeqVal[otherFx_n],0,127,0,4095);
					currentSeqModValues.tremolo.enable = 1;
					currentSeqModValues.tremolo.rate = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_TREMOLO_FAST ? 2048: 256 ;
					lfoAmpPtr->init(&currentSeqModValues.tremolo);
					lfoAmpPtr->start();
				}
			}
			else lfoAmpPtr->stop();

		break;
		case fx_t::FX_TYPE_TREMOLO_SLOW :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 0;

			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
			{
				if(fx_id == MOST_SIGNIFICANT_FX)
				{
					lfoAmpPtr->stop();
					currentSeqModValues.tremolo.amount = map(lastSeqVal[otherFx_n],0,127,0,4095);
					currentSeqModValues.tremolo.enable = 1;
					currentSeqModValues.tremolo.rate = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_TREMOLO_FAST ? 2048: 256 ;
					lfoAmpPtr->init(&currentSeqModValues.tremolo);
					lfoAmpPtr->start();
				}
			}
			else lfoAmpPtr->stop();
		break;
		case fx_t::FX_TYPE_VIBRATO_FAST :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFineTune] = 0;

			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
			{
				if(fx_id == MOST_SIGNIFICANT_FX)
				{
					lfoPitchPtr->stop();
					currentSeqModValues.vibrato.amount = map(lastSeqVal[otherFx_n],0,127,0,4095);
					currentSeqModValues.vibrato.enable = 1;
					currentSeqModValues.vibrato.rate = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_VIBRATO_FAST ? 2048: 256 ;
					lfoPitchPtr->init(&currentSeqModValues.vibrato);
					lfoPitchPtr->start();
				}
			}
			else if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
			{
				lfoPitchPtr->stop();
			}
			else
			{
				playMemPtr->clearFineTuneForceFlag();
				modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
				lfoPitchPtr->stop();
			}
		break;
		case fx_t::FX_TYPE_VIBRATO_SLOW :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFineTune] = 0;
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
			{
				if(fx_id == MOST_SIGNIFICANT_FX)
				{
					lfoPitchPtr->stop();
					currentSeqModValues.vibrato.amount = map(lastSeqVal[otherFx_n],0,127,0,4095);
					currentSeqModValues.vibrato.enable = 1;
					currentSeqModValues.vibrato.rate = lastSeqFx[otherFx_n] == fx_t::FX_TYPE_VIBRATO_FAST ? 2048: 256 ;
					lfoPitchPtr->init(&currentSeqModValues.vibrato);
					lfoPitchPtr->start();
				}
			}
			else if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFineTune])
			{
				lfoPitchPtr->stop();
			}
			else
			{
				playMemPtr->clearFineTuneForceFlag();
				modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
				lfoPitchPtr->stop();
			}
		break;
		case fx_t::FX_TYPE_SAMPLE_END :

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::endPoint] = 0;


			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint])
				{
					currentSeqModValues.endPoint = map(lastSeqVal[otherFx_n],0,127,0,MAX_16BIT);
				}
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
			{
				changeStartPointPerformanceMode(performanceMod.endPoint);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
				{
					modSeqPoints(currentSeqModValues.startPoint, NOT_MOD_POINTS);
				}
				else if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						modSeqPoints(NOT_MOD_POINTS, currentSeqModValues.endPoint);
					}
				}
				else
				{
					playMemPtr->clearPointsForceFlag();
					playMemPtr->setForcedPoints(-1, -1, -1, -1);
				}
			}
		break;
		case fx_t::FX_TYPE_WT_POSITION :
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::wavetablePosition] = 0;
			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.wavetablePosition = map(lastSeqVal[otherFx_n],0,127,0,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
			{
				changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::wavetablePosition])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						playMemPtr->setWavetableWindowFlag();
						playMemPtr->setForcedWavetableWindow(currentSeqModValues.wavetablePosition);
					}
				}
				else
				{
					playMemPtr->clearWavetableWindowFlag();
					playMemPtr->setWavetableWindow(instrumentBasedMod.wtPos);
				}
			}
		break;
		case fx_t::FX_TYPE_GRANULAR_POSITION:

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::granularPosition] = 0;
			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.granularPosition = map(lastSeqVal[otherFx_n],0,127,0,MAX_16BIT);
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
			{
				changeWavetableWindowPerformanceMode(performanceMod.granularPosition);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::granularPosition])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						playMemPtr->setGranularPosForceFlag();
						playMemPtr->setForcedGranularPos(currentSeqModValues.granularPosition);
					}
				}
				else
				{
					playMemPtr->clearGranularPosForceFlag();
					playMemPtr->setGranularPosition(instrumentBasedMod.granPos);
				}
			}
		break;

		case fx_t::FX_TYPE_VELOCITY:

			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::volume] = 0;
			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.volume = map(lastSeqVal[otherFx_n],0,127,0,MAX_INSTRUMENT_VOLUME);
			}
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
			{
				changeVolumePerformanceMode(performanceMod.volume);
			}
			else
			{
				if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
				{
					if(fx_id == MOST_SIGNIFICANT_FX)
					{
						ampPtr->gain( (currentSeqModValues.volume/100.0) * mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount);
					}
				}
				else
				{
					ampPtr->gain( (instrumentBasedMod.volume/100.0) * mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount);
				}
			}
		break;
		case fx_t::FX_TYPE_SAMPLE_SLICE:
			trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::slice] = 0;

			if(fx_id == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.slice = lastSeqVal[otherFx_n] > mtProject.instrument[currentInstrument_idx].sliceNumber - 1 ?
				mtProject.instrument[currentInstrument_idx].sliceNumber - 1:
				lastSeqVal[otherFx_n];
			}

			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::slice])
			{
				if(fx_id == MOST_SIGNIFICANT_FX)
				{
					playMemPtr->setSliceForcedFlag();
					playMemPtr->setForcedSlice(currentSeqModValues.slice);
				}
			}
			else
			{
				playMemPtr->clearSliceForcedFlag();
			}
		break;
		default:
		break;
	}
}

void playerEngine :: modGlide(uint16_t value)
{
	playMemPtr->setGlide(value,currentNote,currentInstrument_idx);
}

void playerEngine :: modSlide(uint16_t value,int8_t slideNote)
{
	playMemPtr->setSlide(value,currentNote,slideNote,currentInstrument_idx);
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

	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);

}

void playerEngine :: modPlayMode(uint8_t value)
{
	playMemPtr->setPlayMode(value);

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
	if(mtProject.instrument[currentInstrument_idx].filterEnable == filterOn)
	{
		filterPtr->setCutoff(value);
	}
}
void playerEngine :: modResonance(float value)
{
	if(mtProject.instrument[currentInstrument_idx].filterEnable == filterOn)
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
	mixerReverb.gain(numPanChannel,value/100.0);
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
	float filterMod=0;
	float ampMod=0;
	float fineTuneMod = 0;
	float wtPositionMod = 0;
	float granPositionMod = 0;

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
//		lfoAmpPtr->stop();
//		lfoFilterPtr->stop();
//		lfoPitchPtr->stop();
	}

	if(mtProject.instrument[currentInstrument_idx].filterEnable == filterOn)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable == envelopeOn)
		{
			if((envelopeFilterPtr->isKeyPressed() == 1) || (envelopeFilterPtr->getPhase() != 0))
			{
				filterMod=envelopeFilterPtr->getOut();
				statusBytes |= CUTOFF_MASK;

				if(mtProject.instrument[currentInstrument_idx].cutOff + filterMod < 0.0f) instrumentBasedMod.cutoff = 0.0f;
				else if(mtProject.instrument[currentInstrument_idx].cutOff + filterMod > 1.0f) instrumentBasedMod.cutoff = 1.0f;
				else instrumentBasedMod.cutoff = mtProject.instrument[currentInstrument_idx].cutOff + filterMod;
			}
		}
//		if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable == lfoOn)
//		{
//			filterMod+=lfoFilterPtr->getOut();
//			statusBytes |= CUTOFF_MASK;
//		}

	}

	if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable == envelopeOn)
		{
				if((envelopeWtPos->isKeyPressed() == 1) || (envelopeWtPos->getPhase() != 0))
				{
					wtPositionMod=envelopeWtPos->getOut();
					statusBytes |= WT_POS_SEND_MASK;

					int32_t iwtPosisionMod = wtPositionMod * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;
					if(mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow + iwtPosisionMod < 0) instrumentBasedMod.wtPos = 0;
					else if(mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow + iwtPosisionMod >= mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber)
						instrumentBasedMod.wtPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
					else instrumentBasedMod.wtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow + iwtPosisionMod;
				}
		}
	}

	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable == envelopeOn)
		{
			if((envelopeGranPos->isKeyPressed() == 1) || (envelopeGranPos->getPhase() != 0))
			{
				granPositionMod=envelopeGranPos->getOut();
				statusBytes |= GRANULAR_POS_SEND_MASK;

				int32_t iGranPosisionMod = granPositionMod * MAX_16BIT;
				if(mtProject.instrument[currentInstrument_idx].granular.currentPosition + iGranPosisionMod < 0) instrumentBasedMod.granPos = 0;
				else if(mtProject.instrument[currentInstrument_idx].granular.currentPosition + iGranPosisionMod >= MAX_16BIT) instrumentBasedMod.granPos = MAX_16BIT;
				else instrumentBasedMod.granPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition + iGranPosisionMod;
			}
		}
	}
//	if(mtProject.instrument[currentInstrument_idx].lfo[lfoA].enable == lfoOn )
//	{
//		ampMod=lfoAmpPtr->getOut();
//		statusBytes |= VOLUME_MASK;
//	}
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp]  )
	{
		if(ampLfoRefreshTimer > LFO_REFRESH_TIME)
		{
			ampLfoRefreshTimer = 0;
			ampMod=lfoAmpPtr->getOut();
			statusBytes |= VOLUME_MASK;
		}

	}

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoFineTune] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoFineTune])
	{
		if(pitchLfoRefreshTimer > LFO_REFRESH_TIME)
		{
			pitchLfoRefreshTimer = 0;
			fineTuneMod = lfoPitchPtr->getOut();
			statusBytes |= FINETUNE_MASK;
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

			if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoFineTune] ||
			   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoFineTune] )
			{
				int8_t fineTune;
				if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::fineTune] ||
				   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::fineTune] ) fineTune = currentSeqModValues.fineTune;
				else fineTune = mtProject.instrument[currentInstrument_idx].fineTune;


				if(fineTune + fineTuneMod*100 > MAX_INSTRUMENT_FINETUNE)
				{
					playMemPtr->setForcedFineTune(MAX_INSTRUMENT_FINETUNE);
					modFineTune(MAX_INSTRUMENT_FINETUNE);
				}
				else if(fineTune + fineTuneMod*100 < MIN_INSTRUMENT_FINETUNE)
				{
					playMemPtr->setForcedFineTune(MIN_INSTRUMENT_FINETUNE);
					modFineTune(MIN_INSTRUMENT_FINETUNE);
				}
				else
				{
					playMemPtr->setForcedFineTune(fineTune + fineTuneMod*100);
					modFineTune(fineTune+ fineTuneMod*100);
				}
			}
			else modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);


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
				if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp] ||
				   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp] )
				{
					uint8_t volume;

					if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
					   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume]	) volume = currentSeqModValues.volume;
					else if (currentVelocity == -1 ) volume = mtProject.instrument[currentInstrument_idx].volume;
					else volume = currentVelocity;

					if( volume + ampMod*100 > MAX_INSTRUMENT_VOLUME) ampPtr->gain( (MAX_INSTRUMENT_VOLUME/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount));
					else if( volume + ampMod*100 < MIN_INSTRUMENT_VOLUME)  ampPtr->gain( (MIN_INSTRUMENT_VOLUME/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount));
					else ampPtr->gain( ( (volume/100.0) + ampMod) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount));
				}
				else
				{
					uint8_t volume = (currentVelocity == -1) ?  mtProject.instrument[currentInstrument_idx].volume : currentVelocity;
					ampPtr->gain( (volume/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount));
				}
			}
		}
		if(statusBytes & PANNING_MASK)
		{
			statusBytes &= (~PANNING_MASK);
			modPanning(mtProject.instrument[currentInstrument_idx].panning);
		}
		if(statusBytes & CUTOFF_MASK)
		{
			statusBytes &= (~CUTOFF_MASK);
			modCutoff(instrumentBasedMod.cutoff);
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
			playMemPtr->setWavetableWindow(instrumentBasedMod.wtPos);
		}
		if(statusBytes & GRANULAR_POS_SEND_MASK)
		{
			statusBytes &= (~GRANULAR_POS_SEND_MASK);
			modGranularPosition(instrumentBasedMod.granPos);
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


	}




}

void playerEngine :: setStatusBytes(uint16_t value)
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
	currentVelocity=0;
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

	envelopeFilterPtr->kill();
	envelopeWtPos->kill();
	envelopeGranPos->kill();

	currentInstrument_idx=instr_idx;
	currentNote=note;
	currentVelocity=velocity;
	/*================================================ENVELOPE AMP==========================================*/
	lfoAmpPtr->init(&mtProject.instrument[instr_idx].lfo[lfoA]);

	if(mtProject.instrument[instr_idx].envelope[envAmp].enable)
	{
		envelopeAmpPtr->delay(mtProject.instrument[instr_idx].envelope[envAmp].delay);
		envelopeAmpPtr->attack(mtProject.instrument[instr_idx].envelope[envAmp].attack);
		envelopeAmpPtr->hold(mtProject.instrument[instr_idx].envelope[envAmp].hold);
		envelopeAmpPtr->decay(mtProject.instrument[instr_idx].envelope[envAmp].decay);
		envelopeAmpPtr->sustain(mtProject.instrument[instr_idx].envelope[envAmp].sustain);
		envelopeAmpPtr->release(mtProject.instrument[instr_idx].envelope[envAmp].release);
		envelopeAmpPtr->setLoop(mtProject.instrument[instr_idx].envelope[envAmp].loop);
		//Serial.println(activeAmpEnvelopes);
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

	if(mtProject.instrument[instr_idx].filterEnable == filterOn)
	{
		envelopeFilterPtr->init(&mtProject.instrument[instr_idx].envelope[envFilter]);
		lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
	/*======================================================================================================*/
	/*================================================FILTER================================================*/
		filterConnect();
		changeFilterType(mtProject.instrument[instr_idx].filterType);
		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);
		filterPtr->setCutoff(mtProject.instrument[instr_idx].cutOff);
	}
	else if(mtProject.instrument[instr_idx].filterEnable == filterOff) filterDisconnect();

	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
		{
			envelopeWtPos->init(&mtProject.instrument[instr_idx].envelope[envWtPos]);
		}
	}

	if((mtProject.instrument[instr_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[instr_idx].playMode == playModeGranular))
	{
//		instrumentBasedMod.granPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
		if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
		{
			envelopeGranPos->init(&mtProject.instrument[instr_idx].envelope[envGranPos]);
		}
	}

	/*======================================================================================================*/
	/*==================================================GAIN================================================*/

	if(velocity < 0)
	{
		ampPtr->gain(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0));
	}
	else
	{
		ampPtr->gain( (velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
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

	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);

	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/

	mixerReverb.gain(numPanChannel,mtProject.instrument[instr_idx].reverbSend/100.0);


	/*======================================================================================================*/

	if(mtProject.instrument[instr_idx].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();
	if(mtProject.instrument[instr_idx].envelope[envWtPos].enable == envelopeOn) envelopeWtPos->start();
	if(mtProject.instrument[instr_idx].envelope[envGranPos].enable == envelopeOn) envelopeGranPos->start();

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

	envelopeFilterPtr->kill();
	envelopeWtPos->kill();
	envelopeGranPos->kill();

	filterDisconnect();
	ampPtr->gain(1.0);

	mixerL.gain(numPanChannel,1.0);
	mixerR.gain(numPanChannel,1.0);
	mixerReverb.gain(numPanChannel,0.0);
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

	envelopeFilterPtr->kill();
	envelopeWtPos->kill();
	envelopeGranPos->kill();

	filterDisconnect();
	ampPtr->gain(1.0);

	mixerL.gain(numPanChannel,1.0);
	mixerR.gain(numPanChannel,1.0);
	mixerReverb.gain(numPanChannel,0.0);
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
	if(state == 0)
	{
		instrumentPlayer[channel].muteState = 0;
		instrumentPlayer[channel].setStatusBytes(VOLUME_MASK);
		instrumentPlayer[channel].setStatusBytes(REVERB_SEND_MASK);
	}
	else
	{
		instrumentPlayer[channel].muteState = 1;
		amp[channel].gain(0.0);
		if(!forceSend) instrumentPlayer[channel].modReverbSend(0);
	}
}
//*******************************************change
void playerEngine ::changeVolumePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] != 1) && (value == 0)) return;

	uint8_t volume;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume]) volume = currentSeqModValues.volume;
	else volume = instrumentBasedMod.volume;

	performanceMod.volume = value;
	if(volume + value > MAX_INSTRUMENT_VOLUME) currentPerformanceValues.volume = MAX_INSTRUMENT_VOLUME;
	else if(volume + value < MIN_INSTRUMENT_VOLUME) currentPerformanceValues.volume = MIN_INSTRUMENT_VOLUME;
	else currentPerformanceValues.volume = volume + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] = 1;

	if(muteState == 0)
	{
		ampPtr->gain(currentPerformanceValues.volume/100.0);

	}

}
void playerEngine ::changePanningPerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] != 1) && (value == 0)) return;
	performanceMod.panning = value;

	int16_t panning;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning]	) panning = currentSeqModValues.panning;
	else panning = mtProject.instrument[currentInstrument_idx].panning;


	if(panning + value > PANNING_MAX) currentPerformanceValues.panning = PANNING_MAX;
	else if(panning + value < PANNING_MIN) currentPerformanceValues.panning = PANNING_MIN;
	else currentPerformanceValues.panning = panning + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] = 1;

	float gainL=0,gainR=0;
	if(currentPerformanceValues.panning < 50)
	{
		gainR=(0 + currentPerformanceValues.panning)/50.0;
		gainL=1.0;
	}
	else if(currentPerformanceValues.panning > 50)
	{
		gainR=1.0;
		gainL=(100 - currentPerformanceValues.panning)/50.0;
	}
	else if(currentPerformanceValues.panning == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);
}
void playerEngine ::changeTunePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] != 1) && (value == 0)) return;
	performanceMod.tune = value;
	if(mtProject.instrument[currentInstrument_idx].tune + value > MAX_INSTRUMENT_TUNE) currentPerformanceValues.tune = MAX_INSTRUMENT_TUNE;
	else if(mtProject.instrument[currentInstrument_idx].tune + value < MIN_INSTRUMENT_TUNE) currentPerformanceValues.tune = MIN_INSTRUMENT_TUNE;
	else currentPerformanceValues.tune = mtProject.instrument[currentInstrument_idx].tune + value;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] = 1;
	playMemPtr->setTuneForceFlag();
	playMemPtr->setForcedTune(currentPerformanceValues.tune);


	playMemPtr->setTune(currentPerformanceValues.tune,currentNote);
}
void playerEngine ::changeReverbSendPerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] != 1) && (value == 0)) return;
	performanceMod.reverbSend = value;

	uint8_t reverbSend;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]	) reverbSend = currentSeqModValues.reverbSend;
	else reverbSend = mtProject.instrument[currentInstrument_idx].reverbSend;

	if(reverbSend + value > REVERB_SEND_MAX) currentPerformanceValues.reverbSend = REVERB_SEND_MAX;
	else if(reverbSend + value < REVERB_SEND_MIN) currentPerformanceValues.reverbSend = REVERB_SEND_MIN;
	else currentPerformanceValues.reverbSend = reverbSend + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] = 1;


	mixerReverb.gain(numPanChannel,currentPerformanceValues.reverbSend/100.0);

}

void playerEngine::changeStartPointPerformanceMode(int32_t value)
{
	changePointsPerformanceMode(value, trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] ? performanceMod.endPoint : 0);
}
void playerEngine::changeEndPointPerformanceMode(int32_t value)
{
	changePointsPerformanceMode(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] ? performanceMod.startPoint : 0, value );
}

void playerEngine::changePointsPerformanceMode(int32_t spValue, int32_t epValue)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] != 1) && (epValue == 0) &&
	  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] != 1) && (spValue == 0)) return;

	performanceMod.endPoint = epValue;
	performanceMod.startPoint = spValue;

	int32_t startPoint,loopPoint1,loopPoint2,endPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint] ||
	 trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::startPoint]) startPoint = currentSeqModValues.startPoint;
	else startPoint = mtProject.instrument[currentInstrument_idx].startPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint1])	loopPoint1 = currentSeqModValues.loopPoint1;
	else loopPoint1 = mtProject.instrument[currentInstrument_idx].loopPoint1;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]) loopPoint2 = currentSeqModValues.loopPoint2;
	else loopPoint2 = mtProject.instrument[currentInstrument_idx].loopPoint2;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint]) endPoint = currentSeqModValues.endPoint;
	else endPoint = mtProject.instrument[currentInstrument_idx].endPoint;

	if(epValue != 0 ) trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] = 1;
	if(spValue != 0 ) 	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] = 1;


	if(mtProject.instrument[currentInstrument_idx].playMode != singleShot)
	{
		startPoint+=spValue;
		if(startPoint < SAMPLE_POINT_POS_MIN) startPoint = SAMPLE_POINT_POS_MIN;
		else if(startPoint > SAMPLE_POINT_POS_MAX) startPoint = SAMPLE_POINT_POS_MAX;

		endPoint+=epValue;
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

			}
			// else endPoint > od loopPoint2 , startPoint < od loopPoint1  - nic nie trzeba ruszac
		}
		else
		{
			if(startPoint > loopPoint1)
			{
				startPoint = loopPoint1 > 0 ? loopPoint1 - 1: 0;
				endPoint = loopPoint2 < SAMPLE_POINT_POS_MAX ? loopPoint2 + 1: SAMPLE_POINT_POS_MAX;
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
			}
		}
	}
	else
	{
		startPoint+=spValue;
		if(startPoint < SAMPLE_POINT_POS_MIN) startPoint = SAMPLE_POINT_POS_MIN;
		else if(startPoint > SAMPLE_POINT_POS_MAX) startPoint = SAMPLE_POINT_POS_MAX;

		endPoint+=epValue;
		if(endPoint < SAMPLE_POINT_POS_MIN) endPoint = SAMPLE_POINT_POS_MIN;
		else if(endPoint > SAMPLE_POINT_POS_MAX) endPoint = SAMPLE_POINT_POS_MAX;
	}

	playMemPtr->setForcedPoints(startPoint,loopPoint1,loopPoint2,endPoint);
	playMemPtr->setPointsForceFlag();
}


void playerEngine ::changeCutoffPerformanceMode(int8_t value) // przed ta funkcja musi byc wowołana funkcja changeFilterTypePerformanceMode
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff] != 1) && (value == 0)) return;
	performanceMod.cutoff = value;

	float cutoff;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff] )
	{
		if(currentPerformanceValues.filterType == currentSeqModValues.filterType) cutoff = currentSeqModValues.filterCutoff;
		else cutoff = 0;
	}
	else
	{
		if(currentPerformanceValues.filterType == mtProject.instrument[currentInstrument_idx].filterType) cutoff = instrumentBasedMod.cutoff;
		else cutoff = 0;
	}

	if(cutoff*100 + value > 100) currentPerformanceValues.filterCutoff = 1.0;
	else if(cutoff*100 + value < 0) currentPerformanceValues.filterCutoff = 0.0;
	else currentPerformanceValues.filterCutoff = cutoff + (value/100.0);


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff] = 1;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterEnable] = 1;
	currentPerformanceValues.filterEnable = 1;
	filterConnect();

	filterPtr->setCutoff(currentPerformanceValues.filterCutoff);

}
void playerEngine ::changeFilterTypePerformanceMode(uint8_t mode)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType] != 1) && (mode == 0)) return;
	if((mode < 1) || (mode > 3)) return;

	performanceMod.filterType = mode;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType] = 1;
	currentPerformanceValues.filterType = mode -1;
	changeFilterType(currentPerformanceValues.filterType);
}

void playerEngine ::changeSamplePlaybackPerformanceMode(uint8_t value)
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection] = 1;
	if(value)
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::samplePlaybeckDirection]) playMemPtr->clearReverse();
		else playMemPtr->setReverse();
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection] ||
		   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection]) playMemPtr->setReverse();
		else playMemPtr->clearReverse();
	}
}

void playerEngine::changeWavetableWindowPerformanceMode(int16_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition] != 1) && (value == 0)) return;
	performanceMod.wavetablePosition = value;

	uint32_t wavetableWindow;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::wavetablePosition]) wavetableWindow = currentSeqModValues.wavetablePosition;
	else wavetableWindow = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;

	int32_t valueMap = map(value,-255,255,- mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber) ;

	if(wavetableWindow + valueMap >= mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber) currentPerformanceValues.wavetablePosition = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
	else if(wavetableWindow + valueMap < 0) currentPerformanceValues.wavetablePosition = 0;
	else currentPerformanceValues.wavetablePosition = wavetableWindow + valueMap;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition] = 1;

	playMemPtr->setWavetableWindowFlag();
	playMemPtr->setForcedWavetableWindow(currentPerformanceValues.wavetablePosition);



	playMemPtr->setWavetableWindow(currentPerformanceValues.wavetablePosition);
}

void playerEngine::changeGranularPositionPerformanceMode(int16_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition] != 1) && (value == 0)) return;
	performanceMod.granularPosition = value;

	uint32_t granularPosition;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::granularPosition]) granularPosition = currentSeqModValues.granularPosition;
	else granularPosition = mtProject.instrument[currentInstrument_idx].granular.currentPosition;

	int32_t valueMap = map(value,-255,255,- MAX_16BIT,MAX_16BIT) ;

	if(granularPosition + valueMap >= MAX_16BIT) currentPerformanceValues.granularPosition = MAX_16BIT;
	else if(granularPosition + valueMap < 0) currentPerformanceValues.granularPosition = 0;
	else currentPerformanceValues.granularPosition = granularPosition + valueMap;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition] = 1;

	playMemPtr->setGranularPosForceFlag();
	playMemPtr->setForcedGranularPos(currentPerformanceValues.granularPosition);

	playMemPtr->setGranularPosition(currentPerformanceValues.granularPosition);
}
//*******************************************end
void playerEngine::endVolumePerformanceMode()
{

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] = 0;

	if(muteState == 0)
	{
		ampPtr->gain(instrumentBasedMod.volume/100.0);
	}
}
void playerEngine::endPanningPerformanceMode()
{
	int16_t panning;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning]	) panning = currentSeqModValues.panning;
	else panning = mtProject.instrument[currentInstrument_idx].panning;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] = 0;


	float gainL=0,gainR=0;
	if(panning < 50)
	{
		gainR=(0 + panning)/50.0;
		gainL=1.0;
	}
	else if(panning> 50)
	{
		gainR=1.0;
		gainL=(100 - panning)/50.0;
	}
	else if(panning == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);
}
void playerEngine::endTunePerformanceMode()
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] = 0;
	playMemPtr->clearTuneForceFlag(); //blokuje zmiane tuna w playMemory


	playMemPtr->setTune(mtProject.instrument[currentInstrument_idx].tune,currentNote);
}
void playerEngine::endReverbSendPerformanceMode()
{
	uint8_t reverbSend;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]) reverbSend = currentSeqModValues.reverbSend;
	else reverbSend = mtProject.instrument[currentInstrument_idx].reverbSend;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] = 0;

	mixerReverb.gain(numPanChannel,reverbSend/100.0);
}

void playerEngine::endPointsPerformanceMode()
{
	uint16_t startPoint,loopPoint1,loopPoint2,endPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::startPoint]	) startPoint = currentSeqModValues.startPoint;
	else startPoint = mtProject.instrument[currentInstrument_idx].startPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint1]	) loopPoint1 = currentSeqModValues.loopPoint1;
	else loopPoint1 = mtProject.instrument[currentInstrument_idx].loopPoint1;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]	) loopPoint2 = currentSeqModValues.loopPoint2;
	else loopPoint2 = mtProject.instrument[currentInstrument_idx].loopPoint2;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]	) endPoint = currentSeqModValues.endPoint;
	else endPoint = mtProject.instrument[currentInstrument_idx].endPoint;


	if((!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint]) 		&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::startPoint]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint1]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::endPoint]))
	{
		playMemPtr->clearPointsForceFlag();
	}
	else
	{
		playMemPtr->setForcedPoints(startPoint, loopPoint1, loopPoint2, endPoint);
		playMemPtr->setPointsForceFlag();
	}


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::loopPoint1] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::loopPoint2] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] = 0;
}
void playerEngine::endStartPointPerformanceMode()
{

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
	{
		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] = 0;
		changePointsPerformanceMode(0 , performanceMod.endPoint);
		return;
	}

	endPointsPerformanceMode();
}


void playerEngine::endCutoffPerformanceMode()
{
	if(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] &&
	   !trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff])
	{
		if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
		filterPtr->setCutoff(instrumentBasedMod.cutoff);
	}
	else
	{
		filterPtr->setCutoff(currentSeqModValues.filterCutoff);
	}

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterEnable] = 0;
	currentPerformanceValues.filterEnable = 0;

}
void playerEngine::endFilterTypePerformanceMode()
{

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType] = 0;
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterType] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterType]	) changeFilterType(currentSeqModValues.filterType);
	else changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);

}
void playerEngine ::endSamplePlaybackPerformanceMode()
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection] = 0;
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::samplePlaybeckDirection] ) playMemPtr->setReverse();
	else playMemPtr->clearReverse();
}
void playerEngine ::endEndPointPerformanceMode()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] = 0;
		changePointsPerformanceMode(performanceMod.startPoint, 0);
		return;
	}

	endPointsPerformanceMode();
}
void playerEngine::endWavetableWindowPerformanceMode()
{
	uint32_t wavetableWindow;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition])
	{
		wavetableWindow = currentSeqModValues.wavetablePosition;
		playMemPtr->setForcedWavetableWindow(wavetableWindow);
	}
	else
	{
		wavetableWindow = instrumentBasedMod.wtPos;
		playMemPtr->clearWavetableWindowFlag();

	}
	playMemPtr->setWavetableWindow(wavetableWindow);
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition] = 0;
}

void playerEngine::endGranularPositionPerformanceMode()
{
	uint32_t granularPos;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition])
	{
		granularPos = currentSeqModValues.granularPosition;
		playMemPtr->setForcedGranularPos(granularPos);
	}
	else
	{
		granularPos = instrumentBasedMod.granPos;
		playMemPtr->clearGranularPosForceFlag();

	}
	playMemPtr->setGranularPosition(granularPos);
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition] = 0;
}
//************************************************************************************************************
uint32_t playerEngine::getEnvelopeWtPosMod()
{

	return ((envelopeWtPos->isKeyPressed() == 1) || (envelopeWtPos->getPhase() != 0)) ? instrumentBasedMod.wtPos : mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
}

uint32_t playerEngine::getEnvelopeGranPosMod()
{

	return ((envelopeGranPos->isKeyPressed() == 1) || (envelopeGranPos->getPhase() != 0)) ? instrumentBasedMod.granPos : mtProject.instrument[currentInstrument_idx].granular.currentPosition;
}

