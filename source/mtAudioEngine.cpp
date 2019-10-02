#include "mtAudioEngine.h"
#include "sampleRecorder.h"
extern AudioControlSGTL5000 audioShield;
static cSampleRecorder* SR = &sampleRecorder;

AudioInputI2S            i2sIn;
AudioRecordQueue         queue;
AudioOutputI2S           i2sOut;

AudioPlaySdWav           playSdWav;
AudioPlaySdWavFloat		 playSdWavFloat;
AudioPlaySdWav24bit 	 playSdWav24Bit;
AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
envelopeGenerator		 envelopeFilter[8];
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

AudioConnection          connect57(&mixerL, &limiter[0]);
AudioConnection          connect58(&mixerR, &limiter[1]);

AudioConnection          connect52(&limiter[0], 0, &bitDepthControl[0], 0);
AudioConnection          connect53(&limiter[1], 0, &bitDepthControl[1], 0);

AudioConnection          connect68(&bitDepthControl[0], 0, &mixerSourceL, 0);
AudioConnection          connect69(&bitDepthControl[1], 0, &mixerSourceR, 0);

AudioConnection          connect61(&playSdWav, 0, &mixerSourceL, 1);
AudioConnection          connect62(&playSdWav, 0, &mixerSourceR, 1);

AudioConnection 		 connect63(&playSdWavFloat,0,&mixerSourceL,2);
AudioConnection 		 connect64(&playSdWavFloat,0,&mixerSourceR,2);

AudioConnection 		 connect65(&playSdWav24Bit,0,&mixerSourceL,3);
AudioConnection 		 connect66(&playSdWav24Bit,0,&mixerSourceR,3);

AudioConnection          connect59(&mixerSourceL, 0, &i2sOut, 0);
AudioConnection          connect60(&mixerSourceR, 0, &i2sOut, 1);

AudioConnection          connect54(&i2sIn, 0, &mixerRec, 0);
AudioConnection          connect55(&i2sIn, 1, &mixerRec, 1);

AudioConnection          connect56(&mixerRec, &queue);
AudioConnection          connect67(&mixerRec, &rms);

playerEngine instrumentPlayer[8];

audioEngine engine;
uint8_t playerEngine::onVoices=0;
uint8_t	playerEngine::activeAmpEnvelopes=0;

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
		instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&filter[i],&envelopeAmp[i], &amp[i], i, &lfoAmp[i],&lfoFilter[i],&lfoPitch[i]);
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
		AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh, LFO * lfoAmp, LFO * lfoFilter, LFO * lfoPitch)
{
	playMemPtr=playMem;
	envelopeAmpPtr=envAmp;
	envelopeFilterPtr=envFilter;
	filterPtr=filter;
	ampPtr=amp;
	numPanChannel=panCh;
	lfoAmpPtr=lfoAmp;
	lfoFilterPtr=lfoFilter;
	lfoPitchPtr=lfoPitch;
	envelopeAmpPtr->releaseNoteOn(releaseNoteOnVal);

}

uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity)
{
	if(mtProject.instrument[instr_idx].isActive != 1) return 0;

	__disable_irq();
	uint8_t status;
	float gainL=0,gainR=0;

	if(envelopeAmpPtr->getState())
	{
		onVoices--;
		activeAmpEnvelopes--;
	}
	onVoices++;
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
		activeAmpEnvelopes++;
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
	}

	/*======================================================================================================*/
	/*================================================ENVELOPE FILTER=======================================*/

	if((mtProject.instrument[instr_idx].filterEnable == filterOn) || (activeValuePerformance.filterForcedEnableFlag))
	{
		envelopeFilterPtr->init(&mtProject.instrument[instr_idx].envelope[envFilter]);
		lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
	/*======================================================================================================*/
	/*================================================FILTER================================================*/
		filterConnect();

		if(activeValuePerformance.filterForcedType == -1 ) changeFilterType(mtProject.instrument[instr_idx].filterType);
		else changeFilterType(activeValuePerformance.filterForcedType);

		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);


		if(!activeValuePerformance.cutOffForceFlag) filterPtr->setCutoff(mtProject.instrument[instr_idx].cutOff);
		activeValuePerformance.cutOff =  mtProject.instrument[instr_idx].cutOff;
	}
	else if(mtProject.instrument[instr_idx].filterEnable == filterOff) filterDisconnect();

	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	if(muteState == 0)
	{
		if(velocity < 0)
		{
			if(!activeValuePerformance.volumeForceFlag)
			{
				ampPtr->gain(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0));
			}
			activeValuePerformance.volume=(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0))*100;
		}
		else
		{
			if(!activeValuePerformance.volumeForceFlag)
			{
				ampPtr->gain( (velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
			}
			activeValuePerformance.volume=((velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount)*100;
		}
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

	activeValuePerformance.panning = mtProject.instrument[instr_idx].panning;
	if(!activeValuePerformance.panningForceFlag)
	{
		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
	}
	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/
	if(muteState == 0)
	{
		if(!activeValuePerformance.reverbSendForceFlag)	mixerReverb.gain(numPanChannel,mtProject.instrument[instr_idx].reverbSend/100.0);
		activeValuePerformance.reverbSend=mtProject.instrument[instr_idx].reverbSend;
	}

	activeValuePerformance.startPoint = mtProject.instrument[instr_idx].startPoint;
	activeValuePerformance.loopPoint1 = mtProject.instrument[instr_idx].loopPoint1;
	activeValuePerformance.loopPoint2 = mtProject.instrument[instr_idx].loopPoint2;
	activeValuePerformance.endPoint = mtProject.instrument[instr_idx].endPoint;

	/*======================================================================================================*/
	activeValuePerformance.tune = mtProject.instrument[instr_idx].tune;
	status = playMemPtr->play(instr_idx,note);
	envelopeAmpPtr->noteOn();

	if(mtProject.instrument[instr_idx].lfo[lfoA].enable == lfoOn) lfoAmpPtr->start();
	if(mtProject.instrument[instr_idx].lfo[lfoF].enable == lfoOn) lfoFilterPtr->start();
	if(mtProject.instrument[instr_idx].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();

	__enable_irq();
	return status;
}

uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx_id, uint8_t fx_val)
{
	__disable_irq();
	uint8_t status;
	float gainL=0,gainR=0;

	if(envelopeAmpPtr->getState())
	{
		onVoices--;
		activeAmpEnvelopes--;
	}
	onVoices++;

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
		activeAmpEnvelopes++;
	}
	else
	{
		envelopeAmpPtr->delay(0);
		envelopeAmpPtr->attack(0);
		envelopeAmpPtr->hold(0);
		envelopeAmpPtr->decay(0);
		envelopeAmpPtr->sustain(1.0);
		envelopeAmpPtr->release(0.0f);
	}

	/*======================================================================================================*/
	/*================================================ENVELOPE FILTER=======================================*/

	if((mtProject.instrument[instr_idx].filterEnable == filterOn) || (activeValuePerformance.filterForcedEnableFlag))
	{
		envelopeFilterPtr->init(&mtProject.instrument[instr_idx].envelope[envFilter]);
		lfoFilterPtr->init(&mtProject.instrument[instr_idx].lfo[lfoF]);
	/*======================================================================================================*/
	/*================================================FILTER================================================*/
		filterConnect();
		if(activeValuePerformance.filterForcedType == -1 ) changeFilterType(mtProject.instrument[instr_idx].filterType);
		else changeFilterType(activeValuePerformance.filterForcedType);
		filterPtr->resonance(mtProject.instrument[instr_idx].resonance + RESONANCE_OFFSET);


		if(!activeValuePerformance.cutOffForceFlag) filterPtr->setCutoff(mtProject.instrument[instr_idx].cutOff);
		activeValuePerformance.cutOff =  mtProject.instrument[instr_idx].cutOff;
	}
	else if(mtProject.instrument[instr_idx].filterEnable == filterOff) filterDisconnect();

	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	if(muteState == 0)
	{
		if(velocity < 0)
		{
			if(!activeValuePerformance.volumeForceFlag)
			{
				ampPtr->gain(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0));
			}
			activeValuePerformance.volume=(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0))*100;
		}
		else
		{
			if(!activeValuePerformance.volumeForceFlag)
			{
				ampPtr->gain( (velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
			}
			activeValuePerformance.volume=((velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount)*100;
		}
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

	activeValuePerformance.panning = mtProject.instrument[instr_idx].panning;
	if(!activeValuePerformance.panningForceFlag)
	{
		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
	}

	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/
	if(muteState == 0)
	{
		if(!activeValuePerformance.reverbSendForceFlag) mixerReverb.gain(numPanChannel,mtProject.instrument[instr_idx].reverbSend/100.0);
		activeValuePerformance.reverbSend=	mtProject.instrument[instr_idx].reverbSend;
	}
	/*======================================================================================================*/

	//*************************************************FX****************************************************

	switch(fx_id)
	{
		case fx_ID_cutoff:

			filterPtr->setCutoff(fx_val/(float)MAX_8BIT);
			activeValuePerformance.cutOff = fx_val/(float)MAX_8BIT;
			break;

	}

	//*******************************************************************************************************
	activeValuePerformance.tune = mtProject.instrument[instr_idx].tune;
	status = playMemPtr->play(instr_idx,note);
	envelopeAmpPtr->noteOn();

	if(mtProject.instrument[instr_idx].lfo[lfoA].enable == lfoOn) lfoAmpPtr->start();
	if(mtProject.instrument[instr_idx].lfo[lfoF].enable == lfoOn) lfoFilterPtr->start();
	if(mtProject.instrument[instr_idx].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();

	__enable_irq();
	return status;
}


void playerEngine :: noteOff()
{
	__disable_irq();
	envelopeAmpPtr->noteOff();
	envelopeFilterPtr->stop();
	if(!mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
	{
		playMemPtr->stop();
		if((getLastExportStep()) && (!onVoices))
		{
			clearLastExportStep();
			finishExport();
		}
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].release == 0.0f)
		{
			playMemPtr->stop();
			if((getLastExportStep()) && (!onVoices) )
			{
				clearLastExportStep();
				finishExport();
			}
		}
	}
	__enable_irq();
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
	activeValuePerformance.panning = value;
	if(activeValuePerformance.panningForceFlag) return;
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
		if(!activeValuePerformance.cutOffForceFlag) filterPtr->setCutoff(value);
		activeValuePerformance.cutOff = value;
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

void playerEngine :: modTune(int8_t value)
{
	activeValuePerformance.tune = value;
	if(activeValuePerformance.tuneForceFlag) return;

	playMemPtr->setTune(value,currentNote);

}

void playerEngine :: modReverbSend(uint8_t value)
{
	activeValuePerformance.reverbSend = value;
	if(activeValuePerformance.reverbSendForceFlag) return;
	mixerReverb.gain(numPanChannel,value/100.0);

}

void playerEngine:: update()
{
	float filterMod=0;
	float ampMod=0;

	currentPlayState = playMemPtr->isPlaying();
	if(currentPlayState == 0 && lastPlayState == 1) interfacePlayingEndFlag = 1;
	lastPlayState = currentPlayState;

	if(envelopeAmpPtr->endRelease())
	{
		envelopeAmpPtr->clearEndReleaseFlag();
		interfaceEndReleaseFlag = 1;

		onVoices--;
		activeAmpEnvelopes--;

		playMemPtr->stop();
		lfoAmpPtr->stop();
		lfoFilterPtr->stop();
		lfoPitchPtr->stop();
		if((getLastExportStep()) && (!onVoices))
		{
			clearLastExportStep();
			finishExport();
		}
	}

	if(mtProject.instrument[currentInstrument_idx].filterEnable == filterOn)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable == envelopeOn)
		{
			filterMod+=envelopeFilterPtr->getOut();
			statusBytes |= CUTOFF_MASK;
		}
		if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable == lfoOn)
		{
			filterMod+=lfoFilterPtr->getOut();
			statusBytes |= CUTOFF_MASK;
		}
	}
	if(mtProject.instrument[currentInstrument_idx].lfo[lfoA].enable == lfoOn )
	{
		ampMod=lfoAmpPtr->getOut();
		statusBytes |= VOLUME_MASK;
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
			modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
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
				if(currentVelocity == -1)
				{
					if(!activeValuePerformance.volumeForceFlag)
					{
						ampPtr->gain((mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod) * (mtProject.instrument[currentInstrument_idx].volume/100.0));
					}
				}
				else
				{
					if(!activeValuePerformance.volumeForceFlag)
					{
						ampPtr->gain( (currentVelocity/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod));
					}
				}
				activeValuePerformance.volume=((currentVelocity/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod))*100;
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
			modCutoff(mtProject.instrument[currentInstrument_idx].cutOff + filterMod);
		}
		if(statusBytes & RESONANCE_MASK)
		{
			statusBytes &= (~RESONANCE_MASK);
			modResonance(mtProject.instrument[currentInstrument_idx].resonance);
		}
		if(statusBytes & REVERB_SEND_MASK)
		{
			statusBytes &= (~REVERB_SEND_MASK);
			if(muteState == 0)
			{
				modReverbSend(mtProject.instrument[currentInstrument_idx].reverbSend);
			}

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

uint8_t playerEngine :: noteOnforPrev (uint8_t instr_idx,int8_t note)
{
	uint8_t status=0;
	envelopeAmpPtr->delay(0);
	envelopeAmpPtr->attack(0);
	envelopeAmpPtr->hold(0);
	envelopeAmpPtr->decay(0);
	envelopeAmpPtr->sustain(1.0);
	envelopeAmpPtr->release(0.0f);


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

	status = playMemPtr->playForPrev(instr_idx,note);
	envelopeAmpPtr->noteOn();

	return status;
}

uint8_t playerEngine :: noteOnforPrev (int16_t * addr, uint32_t len)
{
	uint8_t status=0;
	envelopeAmpPtr->delay(0);
	envelopeAmpPtr->attack(0);
	envelopeAmpPtr->hold(0);
	envelopeAmpPtr->decay(0);
	envelopeAmpPtr->sustain(1.0);
	envelopeAmpPtr->release(0.0f);


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
	status = playMemPtr->playForPrev(addr,len);
	envelopeAmpPtr->noteOn();

	return status;

}

uint8_t playerEngine :: noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note)
{
	uint8_t status=0;
	envelopeAmpPtr->delay(0);
	envelopeAmpPtr->attack(0);
	envelopeAmpPtr->hold(0);
	envelopeAmpPtr->decay(0);
	envelopeAmpPtr->sustain(1.0);
	envelopeAmpPtr->release(0.0f);


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

	status = playMemPtr->playForPrev(addr,len,note);
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
		instrumentPlayer[channel].modReverbSend(0);
	}
}
void playerEngine ::changeVolumePerformanceMode(int8_t value)
{
	if(muteState == 0)
	{
		activeValuePerformance.performanceModVolume = value;
		if(activeValuePerformance.volume + value > MAX_INSTRUMENT_VOLUME) ampPtr->gain(MAX_INSTRUMENT_VOLUME/100.0);
		else if(activeValuePerformance.volume + value < MIN_INSTRUMENT_VOLUME) ampPtr->gain(MIN_INSTRUMENT_VOLUME/100.0);
		else ampPtr->gain((activeValuePerformance.volume + value)/100.0);
		if(value != 0 ) activeValuePerformance.volumeForceFlag = 1;
		else activeValuePerformance.volumeForceFlag = 0;
	}

}
void playerEngine ::changePanningPerformanceMode(int8_t value)
{
	activeValuePerformance.performanceModPanning = value;
	int16_t localPanning;
	if(activeValuePerformance.panning + value > PANNING_MAX) localPanning = PANNING_MAX;
	else if(activeValuePerformance.panning + value < PANNING_MIN) localPanning = PANNING_MIN;
	else localPanning = activeValuePerformance.panning + value;

	float gainL=0,gainR=0;
	if(localPanning < 50)
	{
		gainR=(0 + localPanning)/50.0;
		gainL=1.0;
	}
	else if(localPanning > 50)
	{
		gainR=1.0;
		gainL=(100 - localPanning)/50.0;
	}
	else if(localPanning == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);
	if(value != 0 ) activeValuePerformance.panningForceFlag = 1;
	else activeValuePerformance.panningForceFlag = 0;
}
void playerEngine ::changeTunePerformanceMode(int8_t value)
{
	activeValuePerformance.performanceModTune = value;
	if(activeValuePerformance.tune + value > MAX_INSTRUMENT_TUNE) playMemPtr->setTune(MAX_INSTRUMENT_TUNE,currentNote);
	else if(activeValuePerformance.tune + value < MIN_INSTRUMENT_TUNE) playMemPtr->setTune(MIN_INSTRUMENT_TUNE,currentNote);
	else playMemPtr->setTune(activeValuePerformance.tune + value,currentNote);

	if(value != 0 )
	{
		activeValuePerformance.tuneForceFlag = 1;
		playMemPtr->setTuneForceFlag();
	}
	else
	{
		activeValuePerformance.tuneForceFlag = 0;
		playMemPtr->clearTuneForceFlag();
	}
}
void playerEngine ::changeReverbSendPerformanceMode(int8_t value)
{
	activeValuePerformance.performanceModReverbSend = value;
	if(activeValuePerformance.reverbSend + value > REVERB_SEND_MAX) mixerReverb.gain(numPanChannel,REVERB_SEND_MAX/100.0);
	else if(activeValuePerformance.reverbSend + value < REVERB_SEND_MIN) mixerReverb.gain(numPanChannel,REVERB_SEND_MIN/100.0);
	else mixerReverb.gain(numPanChannel,(activeValuePerformance.reverbSend + value)/100.0);

	if(value != 0 ) activeValuePerformance.reverbSendForceFlag = 1;
	else activeValuePerformance.reverbSendForceFlag = 0;
}
void playerEngine ::changeStartPointPerformanceMode(int32_t value)
{
	if(value == 0)
	{
		playMemPtr->clearPointsForceFlag();
		playMemPtr->setForcedPoints(-1, -1, -1, -1);
		return;
	}
	if(mtProject.instrument[currentInstrument_idx].playMode != singleShot)
	{
		if(activeValuePerformance.startPoint + value >= activeValuePerformance.endPoint )
		{
			activeValuePerformance.performanceModStartPoint = value;
			activeValuePerformance.performanceModLoopPoint1 = activeValuePerformance.endPoint - activeValuePerformance.loopPoint1;
			activeValuePerformance.performanceModLoopPoint2 = activeValuePerformance.endPoint - activeValuePerformance.loopPoint2;
			activeValuePerformance.performanceModEndPoint = 0;
			playMemPtr->setForcedPoints(activeValuePerformance.endPoint, activeValuePerformance.endPoint,
										activeValuePerformance.endPoint, activeValuePerformance.endPoint);
			playMemPtr->setPointsForceFlag();
		}
		else if(activeValuePerformance.startPoint + value <= SAMPLE_POINT_POS_MIN)
		{
			activeValuePerformance.performanceModStartPoint = value;
			activeValuePerformance.performanceModLoopPoint1 = 0;
			activeValuePerformance.performanceModLoopPoint2 = 0;
			activeValuePerformance.performanceModEndPoint = 0;
			playMemPtr->setForcedPoints(SAMPLE_POINT_POS_MIN, activeValuePerformance.loopPoint1,
										activeValuePerformance.loopPoint2, activeValuePerformance.endPoint);
			playMemPtr->setPointsForceFlag();
		}
		else
		{
			if(activeValuePerformance.startPoint + value >= activeValuePerformance.loopPoint1)
			{
				int32_t loopMod = activeValuePerformance.startPoint + value - 1 - activeValuePerformance.loopPoint1;
				if(activeValuePerformance.loopPoint2 + loopMod > activeValuePerformance.endPoint)
				{
					loopMod =  activeValuePerformance.endPoint - activeValuePerformance.loopPoint2 - 1;
					if(loopMod < 0 ) loopMod = 0;
				}
				activeValuePerformance.performanceModStartPoint = value;
				activeValuePerformance.performanceModLoopPoint1 = loopMod;
				activeValuePerformance.performanceModLoopPoint2 = loopMod;
				activeValuePerformance.performanceModEndPoint = 0;
			}
			else
			{
				activeValuePerformance.performanceModStartPoint = value;
			}
			playMemPtr->setForcedPoints(activeValuePerformance.loopPoint1,
										activeValuePerformance.loopPoint1 + activeValuePerformance.performanceModLoopPoint1,
										activeValuePerformance.loopPoint2 + activeValuePerformance.performanceModLoopPoint2,
										-1);

			playMemPtr->setPointsForceFlag();
		}
	}
	else
	{
		activeValuePerformance.performanceModStartPoint = value;
		if(activeValuePerformance.startPoint + value >= activeValuePerformance.endPoint )
		{
			playMemPtr->setForcedPoints(activeValuePerformance.endPoint,-1,-1,-1);
			playMemPtr->setPointsForceFlag();
		}
		else if(activeValuePerformance.startPoint + value <= SAMPLE_POINT_POS_MIN)
		{

			playMemPtr->setForcedPoints(SAMPLE_POINT_POS_MIN,-1,-1,-1);
			playMemPtr->setPointsForceFlag();
		}
		else
		{
			playMemPtr->setForcedPoints(activeValuePerformance.startPoint + activeValuePerformance.performanceModStartPoint,-1,-1,-1);
			playMemPtr->setPointsForceFlag();
		}

	}


}
void playerEngine ::changeCutoffPerformanceMode(int8_t value)
{
	activeValuePerformance.performanceModCutOff = value/100.0;
	if(activeValuePerformance.cutOff + value > 100) filterPtr->setCutoff(1.0);
	else if(activeValuePerformance.cutOff + value < 0) filterPtr->setCutoff(0.0);
	else filterPtr->setCutoff((activeValuePerformance.cutOff + value)/100.0);


	if(value != 0 ) activeValuePerformance.cutOffForceFlag = 1;
	else activeValuePerformance.cutOffForceFlag = 0;
}
void playerEngine ::changeFilterTypePerformanceMode(uint8_t mode)
{
	if(mode != 0 ) activeValuePerformance.filterForcedEnableFlag = 1;
	else
	{
		activeValuePerformance.filterForcedEnableFlag = 0;
		changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
		if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
		activeValuePerformance.filterForcedType = -1;
		return;
	}

	activeValuePerformance.filterForcedType = mode-1;
	changeFilterType(mode-1); //lowpass
}

//************************************************************************************************************
