

#include "mtAudioEngine.h"

extern AudioControlSGTL5000 audioShield;

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
envelopeGenerator		 envelopeFilter[8];
LFO						 lfoAmp[8];
LFO						 lfoFilter[8];
LFO						 lfoPitch[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer8				 mixerL,mixerR;
AudioOutputI2S           i2s1;

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

AudioConnection         connect41(&mixerL, 0, &i2s1, 1);
AudioConnection         connect42(&mixerR, 0, &i2s1, 0);

playerEngine instrumentPlayer[8];


audioEngine engine;

void audioEngine::init()
{
	pinMode(AUDIO_IN_MUX, OUTPUT);
	pinMode(AUDIO_OUT_MUX, OUTPUT);
	digitalWrite(AUDIO_IN_MUX, LOW);
	digitalWrite(AUDIO_OUT_MUX, LOW);

	for(int i=0;i<8; i++)
	{
		instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&filter[i],&envelopeAmp[i], &amp[i], i, &lfoAmp[i],&lfoFilter[i],&lfoPitch[i]);
	}

}

void audioEngine::update()
{
	for(int i=0; i<8; i++)
	{
		instrumentPlayer[i].update();
	}

	if(mtProject.audioCodacConfig.changeFlag)
	{
		mtProject.audioCodacConfig.changeFlag=0;

		if(mtProject.audioCodacConfig.outSelect == outputSelectHeadphones)
		{
			if(mtProject.audioCodacConfig.mutedHeadphone) audioShield.muteHeadphone();
			else
			{
				audioShield.unmuteHeadphone();
				setOut(outputSelectHeadphones);
				audioShield.volume(mtProject.audioCodacConfig.headphoneVolume);
			}

		}
		else if(mtProject.audioCodacConfig.outSelect == outputSelectLineOut)
		{
			if(mtProject.audioCodacConfig.mutedLineOut) audioShield.muteLineout();
			else
			{
				audioShield.unmuteLineout();
				setOut(outputSelectLineOut);
				audioShield.lineOutLevel(mtProject.audioCodacConfig.lineOutLeft,mtProject.audioCodacConfig.lineOutRight);
			}
		}

		if(mtProject.audioCodacConfig.inSelect == inputSelectMic)
		{
			setIn(inputSelectMic);
			audioShield.inputSelect(AUDIO_INPUT_MIC);
			audioShield.micGain(mtProject.audioCodacConfig.inputGain);
		}
		else if(mtProject.audioCodacConfig.inSelect == inputSelectLineIn)
		{
			setIn(inputSelectLineIn);
			audioShield.inputSelect(AUDIO_INPUT_LINEIN);
			audioShield.lineInLevel(mtProject.audioCodacConfig.lineInLeft, mtProject.audioCodacConfig.lineInRight);
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

	gainL=mtProject.instrument[instr_idx].panning/100.0;
	gainR=(100-mtProject.instrument[instr_idx].panning)/100.0;

	mixerL.gain(numPanChannel,gainL);
	mixerR.gain(numPanChannel,gainR);
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
	gainL=value/100.0;
	gainR=(100-value)/100.0;

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
			statusByte |= CUTOFF_MASK;
		}
		if(mtProject.instrument[currentInstrument_idx].lfo[lfoF].enable == lfoOn)
		{
			filterMod+=lfoFilterPtr->getOut();
			statusByte |= CUTOFF_MASK;
		}
	}
	if(mtProject.instrument[currentInstrument_idx].lfo[lfoA].enable == lfoOn )
	{
		ampMod=lfoAmpPtr->getOut();
		statusByte |= VOLUME_MASK;
	}


	if(statusByte)
	{
		if(statusByte & LP1_MASK)
		{
			statusByte &= (~LP1_MASK);
			modLP1(mtProject.instrument[currentInstrument_idx].loopPoint1);
		}
		if(statusByte & LP2_MASK)
		{
			statusByte &= (~LP2_MASK);
			modLP2(mtProject.instrument[currentInstrument_idx].loopPoint2);
		}
		if(statusByte & FINETUNE_MASK)
		{
			statusByte &= (~FINETUNE_MASK);
			modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
		}
		if(statusByte & TUNE_MASK)
		{
			statusByte &= (~TUNE_MASK);
			modTune(mtProject.instrument[currentInstrument_idx].tune);
		}
		if(statusByte & VOLUME_MASK)
		{
			statusByte &= (~VOLUME_MASK);
			if(!currentVelocity) ampPtr->gain((mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod) * (mtProject.instrument[currentInstrument_idx].volume/100.0));
			else ampPtr->gain( (currentVelocity/100.0) * (mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount + ampMod));
		}
		if(statusByte & PANNING_MASK)
		{
			statusByte &= (~PANNING_MASK);
			modPanning(mtProject.instrument[currentInstrument_idx].panning);
		}
		if(statusByte & CUTOFF_MASK)
		{
			statusByte &= (~CUTOFF_MASK);
			modCutoff(mtProject.instrument[currentInstrument_idx].cutOff + filterMod);
		}
		if(statusByte & RESONANCE_MASK)
		{
			statusByte &= (~RESONANCE_MASK);
			modResonance(mtProject.instrument[currentInstrument_idx].resonance);
		}
	}



}

void playerEngine :: setStatusByte(uint8_t value)
{
	statusByte|=value;
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

float playerEngine :: fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

