#include "mtAudioEngine.h"

extern AudioControlSGTL5000 audioShield;


AudioInputI2S            i2sIn;
AudioRecordQueue         queue;

AudioPlaySdWav           playSdWav;
AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
envelopeGenerator		 envelopeFilter[8];
LFO						 lfoAmp[8];
LFO						 lfoFilter[8];
LFO						 lfoPitch[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer9				 mixerL,mixerR,mixerReverb;
AudioOutputI2S           i2sOut;
AudioMixer4              mixerRec;
AudioEffectFreeverb		 reverb;

int16_t					 mods[MAX_TARGET][MAX_MOD];

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

AudioConnection          connect17(&envelopeAmp[0],&amp[0]);
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

AudioConnection          connect52(&mixerL, 0, &i2sOut, 1);
AudioConnection          connect53(&mixerR, 0, &i2sOut, 0);

AudioConnection          connect54(&i2sIn, 0, &mixerRec, 0);
AudioConnection          connect55(&i2sIn, 1, &mixerRec, 1);

AudioConnection          connect56(&mixerRec, &queue);


playerEngine instrumentPlayer[8];

audioEngine engine;

void audioEngine::init()
{
	pinMode(AUDIO_IN_MUX, OUTPUT);
	pinMode(AUDIO_OUT_MUX, OUTPUT);
	digitalWrite(AUDIO_IN_MUX, LOW);
	digitalWrite(AUDIO_OUT_MUX, LOW);
	i2sConnect[0]= &connect52;
	i2sConnect[1]= &connect53;
	mixerR.gain(8,1.0);
	mixerL.gain(8,1.0);
//	setIn(inputSelectLineIn);
//	audioShield.inputSelect(AUDIO_INPUT_LINEIN);
	setIn(inputSelectMic);
	audioShield.inputSelect(AUDIO_INPUT_MIC);
	audioShield.micGain(25);

	for(int i=0;i<8; i++)
	{
		instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&filter[i],&envelopeAmp[i], &amp[i], i, &lfoAmp[i],&lfoFilter[i],&lfoPitch[i]);
	}

}

void audioEngine::update()
{
	recorder.update();

	if(recorder.mode == recorderModeStop)
	{
		for(int i=0; i<8; i++)
		{
			instrumentPlayer[i].update();
		}

		if(mtConfig.audioCodecConfig.changeFlag)
		{
			mtConfig.audioCodecConfig.changeFlag=0;

			if(mtConfig.audioCodecConfig.outSelect == outputSelectHeadphones)
			{
				if(mtConfig.audioCodecConfig.mutedHeadphone) audioShield.muteHeadphone();
				else
				{
					audioShield.unmuteHeadphone();
					setOut(outputSelectHeadphones);
					audioShield.volume(mtConfig.audioCodecConfig.headphoneVolume);
				}

			}
			else if(mtConfig.audioCodecConfig.outSelect == outputSelectLineOut)
			{
				if(mtConfig.audioCodecConfig.mutedLineOut) audioShield.muteLineout();
				else
				{
					audioShield.unmuteLineout();
					setOut(outputSelectLineOut);
					audioShield.lineOutLevel(mtConfig.audioCodecConfig.lineOutLeft,mtConfig.audioCodecConfig.lineOutRight);
				}
			}

			if(mtConfig.audioCodecConfig.inSelect == inputSelectMic)
			{
				setIn(inputSelectMic);
				audioShield.inputSelect(AUDIO_INPUT_MIC);
				audioShield.micGain(mtConfig.audioCodecConfig.inputGain);
			}
			else if(mtConfig.audioCodecConfig.inSelect == inputSelectLineIn)
			{
				setIn(inputSelectLineIn);
				audioShield.inputSelect(AUDIO_INPUT_LINEIN);
				audioShield.lineInLevel(mtConfig.audioCodecConfig.lineInLeft, mtConfig.audioCodecConfig.lineInRight);
			}
		}
	}

}

void audioEngine::setOut(uint8_t audioOutStatus)
{
	if(audioOutStatus) digitalWrite(AUDIO_OUT_MUX, HIGH);
	else digitalWrite(AUDIO_OUT_MUX, LOW);
}

void audioEngine::setIn(uint8_t audioInStatus)
{
	if(audioInStatus) digitalWrite(AUDIO_IN_MUX, HIGH);
	else digitalWrite(AUDIO_IN_MUX, LOW);
}

void audioEngine::setReverbRoomsize(uint8_t value)
{
	reverb.roomsize(value/100);
}

void audioEngine::setReverbDamping(uint8_t value)
{
	reverb.damping(value/100);
}

void audioEngine::muteTrack(uint8_t channel)
{
	amp[channel].gain(0.0);
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
	switch(panCh)
	{
	case 0:
		conFilterToAmpPtr=&connect9;
		conPlayToFilterPtr=&connect1;
		break;
	case 1:
		conFilterToAmpPtr=&connect10;
		conPlayToFilterPtr=&connect2;
		break;
	case 2:
		conFilterToAmpPtr=&connect11;
		conPlayToFilterPtr=&connect3;
		break;
	case 3:
		conFilterToAmpPtr=&connect12;
		conPlayToFilterPtr=&connect4;
		break;
	case 4:
		conFilterToAmpPtr=&connect13;
		conPlayToFilterPtr=&connect5;
		break;
	case 5:
		conFilterToAmpPtr=&connect14;
		conPlayToFilterPtr=&connect6;
		break;
	case 6:
		conFilterToAmpPtr=&connect15;
		conPlayToFilterPtr=&connect7;
		break;
	case 7:
		conFilterToAmpPtr=&connect16;
		conPlayToFilterPtr=&connect8;
		break;
	default:
		break;
	}

}

uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity)
{
	uint8_t status;
	float gainL=0,gainR=0;


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

	/*======================================================================================================*/
	/*==================================================GAIN================================================*/
	if(!velocity) ampPtr->gain(mtProject.instrument[instr_idx].envelope[envAmp].amount * (mtProject.instrument[instr_idx].volume/100.0));
	else ampPtr->gain( (velocity/100.0) * mtProject.instrument[instr_idx].envelope[envAmp].amount);
	/*======================================================================================================*/
	/*===============================================PANNING================================================*/
	if(mtProject.instrument[instr_idx].panning < 0) gainL=(0-mtProject.instrument[instr_idx].panning)/100.0;
	else if(mtProject.instrument[instr_idx].panning > 0) gainR=(mtProject.instrument[instr_idx].panning)/100.0;
	else if(mtProject.instrument[instr_idx].panning == 0)
	{
		gainL=1.0; gainR=1.0;
	}


	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);
	/*======================================================================================================*/
	/*===============================================REVERB=================================================*/
	mixerReverb.gain(numPanChannel,mtProject.instrument[instr_idx].reverbSend/100);


	/*======================================================================================================*/
	status = playMemPtr->play(instr_idx,note);
	envelopeAmpPtr->noteOn();

	if(mtProject.instrument[instr_idx].lfo[lfoA].enable == lfoOn) lfoAmpPtr->start();
	if(mtProject.instrument[instr_idx].lfo[lfoF].enable == lfoOn) lfoFilterPtr->start();
	if(mtProject.instrument[instr_idx].envelope[envFilter].enable == envelopeOn) envelopeFilterPtr->start();


	return status;
}


void playerEngine :: noteOff()
{
	envelopeAmpPtr->noteOff();
	envelopeFilterPtr->stop();
}


void playerEngine :: modGlide(uint16_t value)
{
	//mods[targetGlide][manualMod]=value;
	playMemPtr->setGlide(value,currentNote,currentInstrument_idx);
}

void playerEngine :: modSlide(uint16_t value,int8_t slideNote)
{
	//mods[targetGlide][manualMod]=value;
	playMemPtr->setSlide(value,currentNote,slideNote,currentInstrument_idx);
}

void playerEngine :: modFineTune(int8_t value)
{
	playMemPtr->setFineTune(value,currentNote);
}

void playerEngine :: modPanning(int16_t value)
{
	//mods[targetPanning][manualMod]=value;
	float gainL=0,gainR=0;
	if(value < 0) gainL=(0-value)/100.0;
	else if(value> 0) gainR=(value)/100.0;
	else if(value == 0)
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
/*	void playerEngine :: modSP(uint16_t value)
{
	//mods[targetSP][manualMod]=value;
}*/
void playerEngine :: modLP1(uint16_t value)
{
	//mods[targetLP1][manualMod]=value;
	playMemPtr->setLP1(value);
}
void playerEngine :: modLP2(uint16_t value)
{
	//mods[targetLP2][manualMod]=value;
	playMemPtr->setLP2(value);
}

void playerEngine :: modCutoff(float value)
{
	if(mtProject.instrument[currentInstrument_idx].filterEnable == filterOn)
	{
		//mods[targetCutoff][manualMod]=value;
		filterPtr->setCutoff(value);
	}

}
void playerEngine :: modResonance(float value)
{
	if(mtProject.instrument[currentInstrument_idx].filterEnable == filterOn)
	{
		//mods[targetResonance][manualMod]=value;
		filterPtr->resonance(value + RESONANCE_OFFSET);
	}
}

void playerEngine :: modWavetableWindow(uint16_t value)
{
	playMemPtr->setWavetableWindow(value);
}

void playerEngine :: modTune(int8_t value)
{
	playMemPtr->setTune(value,currentNote);
}

void playerEngine :: modReverbSend(uint8_t value)
{
	mixerL.gain(numPanChannel,value/100);
}

/*	void playerEngine:: resetMods(void)
{
	for(uint8_t i=0;i<MAX_TARGET;i++)
	{
		mods[i][manualMod]=0;
	}
}*/
void playerEngine:: update()
{
	float filterMod=0;
	float ampMod=0;

	if(envelopeAmpPtr->endRelease())
	{
		playMemPtr->stop();
		lfoAmpPtr->stop();
		lfoFilterPtr->stop();
		lfoPitchPtr->stop();
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
			if(!currentVelocity) ampPtr->gain((mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod) * (mtProject.instrument[currentInstrument_idx].volume/100.0));
			else ampPtr->gain( (currentVelocity/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod));
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
			modReverbSend(mtProject.instrument[currentInstrument_idx].reverbSend);
		}
	}



}

void playerEngine :: setStatusBytes(uint16_t value)
{
	statusBytes|=value;
}


void playerEngine :: changeFilterType(uint8_t type)
{
	conFilterToAmpPtr->disconnect();
	if(type == lowPass) conFilterToAmpPtr->src_index=0;
	else if(type == bandPass) conFilterToAmpPtr->src_index=1;
	else if(type ==  highPass) conFilterToAmpPtr->src_index=2;
	conFilterToAmpPtr->connect();
}

void playerEngine :: filterDisconnect()
{
	//conFilterToAmpPtr->disconnect();
	conPlayToFilterPtr->disconnect();

	conPlayToFilterPtr->src=playMemPtr;
	conPlayToFilterPtr->dst=envelopeAmpPtr;
	conPlayToFilterPtr->src_index=0;
	conPlayToFilterPtr->dest_index=0;

	conPlayToFilterPtr->connect();
}

void playerEngine :: filterConnect()
{
	conFilterToAmpPtr->disconnect();
	conPlayToFilterPtr->disconnect();

	conPlayToFilterPtr->src=playMemPtr;
	conPlayToFilterPtr->dst=filterPtr;
	conPlayToFilterPtr->src_index=0;
	conPlayToFilterPtr->dest_index=0;

	conFilterToAmpPtr->connect();
	conPlayToFilterPtr->connect();

}

void audioEngine:: prevSdConnect()
{
	i2sConnect[0]->disconnect();
	i2sConnect[1]->disconnect();

	i2sConnect[0]->src = &playSdWav;
	i2sConnect[0]->dst = &i2sOut;
	i2sConnect[0]->src_index=0;
	i2sConnect[0]->dest_index=0;

	i2sConnect[1]->src = &playSdWav;
	i2sConnect[1]->dst = &i2sOut;
	i2sConnect[1]->src_index=0;
	i2sConnect[1]->dest_index=1;

	i2sConnect[0]->connect();
	i2sConnect[1]->connect();

}

void audioEngine:: prevSdDisconnect()
{
	i2sConnect[0]->disconnect();
	i2sConnect[1]->disconnect();

	i2sConnect[0]->src = &mixerL;
	i2sConnect[0]->dst = &i2sOut;
	i2sConnect[0]->src_index=0;
	i2sConnect[0]->dest_index=1;

	i2sConnect[1]->src = &mixerR;
	i2sConnect[1]->dst = &i2sOut;
	i2sConnect[1]->src_index=0;
	i2sConnect[1]->dest_index=0;

	i2sConnect[0]->connect();
	i2sConnect[1]->connect();
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

	mixerL.gain(numPanChannel,0.5);
	mixerR.gain(numPanChannel,0.5);
	mixerReverb.gain(numPanChannel,0.0);
	/*======================================================================================================*/

	status = playMemPtr->playForPrev(addr,len);
	envelopeAmpPtr->noteOn();

	return status;

}


