#include "mtAudioEngine.h"






/*
AudioConnection,	dcToEnvFilter[8] =
{
		AudioConnection(dc[0],envelopeFilter[0]),
		AudioConnection(dc[1],envelopeFilter[1]),
		AudioConnection(dc[2],envelopeFilter[2]),
		AudioConnection(dc[3],envelopeFilter[3]),
		AudioConnection(dc[4],envelopeFilter[4]),
		AudioConnection(dc[5],envelopeFilter[5]),
		AudioConnection(dc[6],envelopeFilter[6]),
		AudioConnection(dc[7],envelopeFilter[7])
};

AudioConnection			playMemToFilter[8] =
{
		AudioConnection(playMem[0],0, filter[0],0),
		AudioConnection(playMem[1],0, filter[1],0),
		AudioConnection(playMem[2],0, filter[2],0),
		AudioConnection(playMem[3],0, filter[3],0),
		AudioConnection(playMem[4],0, filter[4],0),
		AudioConnection(playMem[5],0, filter[5],0),
		AudioConnection(playMem[6],0, filter[6],0),
		AudioConnection(playMem[7],0, filter[7],0)
};

AudioConnection			envToFilter[8] =
{
		AudioConnection(envelopeFilter[0], 0, filter[0], 1),
		AudioConnection(envelopeFilter[1], 0, filter[1], 1),
		AudioConnection(envelopeFilter[2], 0, filter[2], 1),
		AudioConnection(envelopeFilter[3], 0, filter[3], 1),
		AudioConnection(envelopeFilter[4], 0, filter[4], 1),
		AudioConnection(envelopeFilter[5], 0, filter[5], 1),
		AudioConnection(envelopeFilter[6], 0, filter[6], 1),
		AudioConnection(envelopeFilter[7], 0, filter[7], 1)
};

AudioConnection			filterToEnvAmp[8] =
{
		AudioConnection(filter[0], 0, envelopeAmp[0], 0),
		AudioConnection(filter[1], 0, envelopeAmp[1], 0),
		AudioConnection(filter[2], 0, envelopeAmp[2], 0),
		AudioConnection(filter[3], 0, envelopeAmp[3], 0),
		AudioConnection(filter[4], 0, envelopeAmp[4], 0),
		AudioConnection(filter[5], 0, envelopeAmp[5], 0),
		AudioConnection(filter[6], 0, envelopeAmp[6], 0),
		AudioConnection(filter[7], 0, envelopeAmp[7], 0)
};

AudioConnection			envToAmp[8] =
{
		AudioConnection(envelopeAmp[0], amp[0]),
		AudioConnection(envelopeAmp[1], amp[1]),
		AudioConnection(envelopeAmp[2], amp[2]),
		AudioConnection(envelopeAmp[3], amp[3]),
		AudioConnection(envelopeAmp[4], amp[4]),
		AudioConnection(envelopeAmp[5], amp[5]),
		AudioConnection(envelopeAmp[6], amp[6]),
		AudioConnection(envelopeAmp[7], amp[7])
};

AudioConnection			ampToMixerL[8] =
{
		AudioConnection(amp[0], 0, mixerL, 0),
		AudioConnection(amp[1], 0, mixerL, 1),
		AudioConnection(amp[2], 0, mixerL, 2),
		AudioConnection(amp[3], 0, mixerL, 3),
		AudioConnection(amp[4], 0, mixerL, 4),
		AudioConnection(amp[5], 0, mixerL, 5),
		AudioConnection(amp[6], 0, mixerL, 6),
		AudioConnection(amp[7], 0, mixerL, 7)
};

AudioConnection			ampToMixerR[8] =
{
		AudioConnection(amp[0], 0, mixerR, 0),
		AudioConnection(amp[1], 0, mixerR, 1),
		AudioConnection(amp[2], 0, mixerR, 2),
		AudioConnection(amp[3], 0, mixerR, 3),
		AudioConnection(amp[4], 0, mixerR, 4),
		AudioConnection(amp[5], 0, mixerR, 5),
		AudioConnection(amp[6], 0, mixerR, 6),
		AudioConnection(amp[7], 0, mixerR, 7)
};
*/

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
AudioEffectEnvelope      envelopeFilter[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer8				 mixerL,mixerR;
AudioOutputI2S           i2s1;
AudioSynthWaveformDc     dc[8];

AudioConnection          connect1(dc[0], envelopeFilter[0]);
AudioConnection          connect2(dc[1], envelopeFilter[1]);
AudioConnection          connect3(dc[2], envelopeFilter[2]);
AudioConnection          connect4(dc[3], envelopeFilter[3]);
AudioConnection          connect5(dc[4], envelopeFilter[4]);
AudioConnection          connect6(dc[5], envelopeFilter[5]);
AudioConnection          connect7(dc[6], envelopeFilter[6]);
AudioConnection          connect8(dc[7], envelopeFilter[7]);

AudioConnection          connect9(playMem[0], 0, filter[0], 0);
AudioConnection          connect10(playMem[1], 0, filter[1], 0);
AudioConnection          connect11(playMem[2], 0, filter[2], 0);
AudioConnection          connect12(playMem[3], 0, filter[3], 0);
AudioConnection          connect13(playMem[4], 0, filter[4], 0);
AudioConnection          connect14(playMem[5], 0, filter[5], 0);
AudioConnection          connect15(playMem[6], 0, filter[6], 0);
AudioConnection          connect16(playMem[7], 0, filter[7], 0);

AudioConnection          connect17(envelopeFilter[0], 0, filter[0], 1);
AudioConnection          connect18(envelopeFilter[1], 0, filter[1], 1);
AudioConnection          connect19(envelopeFilter[2], 0, filter[2], 1);
AudioConnection          connect20(envelopeFilter[3], 0, filter[3], 1);
AudioConnection          connect21(envelopeFilter[4], 0, filter[4], 1);
AudioConnection          connect22(envelopeFilter[5], 0, filter[5], 1);
AudioConnection          connect23(envelopeFilter[6], 0, filter[6], 1);
AudioConnection          connect24(envelopeFilter[7], 0, filter[7], 1);

AudioConnection          connect25(filter[0], 0, envelopeAmp[0], 0);
AudioConnection          connect26(filter[1], 0, envelopeAmp[1], 0);
AudioConnection          connect27(filter[2], 0, envelopeAmp[2], 0);
AudioConnection          connect28(filter[3], 0, envelopeAmp[3], 0);
AudioConnection          connect29(filter[4], 0, envelopeAmp[4], 0);
AudioConnection          connect30(filter[5], 0, envelopeAmp[5], 0);
AudioConnection          connect31(filter[6], 0, envelopeAmp[6], 0);
AudioConnection          connect32(filter[7], 0, envelopeAmp[7], 0);

AudioConnection          connect33(envelopeAmp[0], amp[0]);
AudioConnection          connect34(envelopeAmp[1], amp[1]);
AudioConnection          connect35(envelopeAmp[2], amp[2]);
AudioConnection          connect36(envelopeAmp[3], amp[3]);
AudioConnection          connect37(envelopeAmp[4], amp[4]);
AudioConnection          connect38(envelopeAmp[5], amp[5]);
AudioConnection          connect39(envelopeAmp[6], amp[6]);
AudioConnection          connect40(envelopeAmp[7], amp[7]);

AudioConnection          connect41(amp[0], 0, mixerL, 0);
AudioConnection          connect42(amp[1], 0, mixerL, 1);
AudioConnection          connect43(amp[2], 0, mixerL, 2);
AudioConnection          connect44(amp[3], 0, mixerL, 3);
AudioConnection          connect45(amp[4], 0, mixerL, 4);
AudioConnection          connect46(amp[5], 0, mixerL, 5);
AudioConnection          connect47(amp[6], 0, mixerL, 6);
AudioConnection          connect48(amp[7], 0, mixerL, 7);


AudioConnection          connect49(amp[0], 0, mixerR, 0);
AudioConnection          connect50(amp[1], 0, mixerR, 1);
AudioConnection          connect51(amp[2], 0, mixerR, 2);
AudioConnection          connect52(amp[3], 0, mixerR, 3);
AudioConnection          connect53(amp[4], 0, mixerR, 4);
AudioConnection          connect54(amp[5], 0, mixerR, 5);
AudioConnection          connect55(amp[6], 0, mixerR, 6);
AudioConnection          connect56(amp[7], 0, mixerR, 7);

AudioConnection         connect57(mixerL, 0, i2s1, 1);
AudioConnection         connect58(mixerR, 0, i2s1, 0);



	instrumentEngine instrumentPlayer[8];
	strMtModAudioEngine modAudioEngine[8];
	audioEngine engine;

	void audioEngine::init()
	{
		for(int i=0;i<8; i++)
		{
			instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&dc[i],&filter[i],&envelopeAmp[i], &amp[i], i);
		}

	}
	void audioEngine::update()
	{
		for(int i=0; i<8; i++)
		{
			instrumentPlayer[i].update();
		}
	}

	void instrumentEngine::init(AudioPlayMemory * playMem,AudioEffectEnvelope * envFilter,AudioSynthWaveformDc * dcFilter,AudioFilterStateVariable * filter,
			AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh)
	{
		playMemPtr=playMem;
		envelopeAmpPtr=envAmp;
		envelopeFilterPtr=envFilter;
		dcFilterPtr=dcFilter;
		filterPtr=filter;
		ampPtr=amp;
		numPanChannel=panCh;
		switch(panCh)
		{
		case 0:
			conFilterToAmpPtr=&connect25;
			break;
		case 1:
			conFilterToAmpPtr=&connect26;
			break;
		case 2:
			conFilterToAmpPtr=&connect27;
			break;
		case 3:
			conFilterToAmpPtr=&connect28;
			break;
		case 4:
			conFilterToAmpPtr=&connect29;
			break;
		case 5:
			conFilterToAmpPtr=&connect30;
			break;
		case 6:
			conFilterToAmpPtr=&connect31;
			break;
		case 7:
			conFilterToAmpPtr=&connect32;
			break;
		default:
			break;
		}

	}

	uint8_t instrumentEngine :: play(strStep * step,strMtModAudioEngine * mod)
	{
		uint8_t status;
		float gainL=0,gainR=0;

		/*================================================ENVELOPE AMP==========================================*/
		envelopeAmpPtr->delay(mtProject.instrument[step->instrumentIndex].envelope[envAmp].delay);
		envelopeAmpPtr->attack(mtProject.instrument[step->instrumentIndex].envelope[envAmp].attack);
		envelopeAmpPtr->hold(mtProject.instrument[step->instrumentIndex].envelope[envAmp].hold);
		envelopeAmpPtr->decay(mtProject.instrument[step->instrumentIndex].envelope[envAmp].decay);
		envelopeAmpPtr->sustain(mtProject.instrument[step->instrumentIndex].envelope[envAmp].sustain);
		envelopeAmpPtr->release(mtProject.instrument[step->instrumentIndex].envelope[envAmp].release);
		/*======================================================================================================*/
		/*================================================ENVELOPE FILTER=======================================*/
		envelopeFilterPtr->delay(mtProject.instrument[step->instrumentIndex].envelope[envFilter].delay);
		envelopeFilterPtr->attack(mtProject.instrument[step->instrumentIndex].envelope[envFilter].attack);
		envelopeFilterPtr->hold(mtProject.instrument[step->instrumentIndex].envelope[envFilter].hold);
		envelopeFilterPtr->decay(mtProject.instrument[step->instrumentIndex].envelope[envFilter].decay);
		envelopeFilterPtr->sustain(mtProject.instrument[step->instrumentIndex].envelope[envFilter].sustain);
		envelopeFilterPtr->release(mtProject.instrument[step->instrumentIndex].envelope[envFilter].release);

		/*======================================================================================================*/
		/*================================================FILTER================================================*/
		changeFilterType(mtProject.instrument[step->instrumentIndex].filterType);
		filterPtr->resonance(mtProject.instrument[step->instrumentIndex].resonance);

		if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOff)
		{
			filterPtr->octaveControl(0.0,manualControl);
			dcFilterPtr->amplitude(0.0);

			if(mtProject.instrument[step->instrumentIndex].filterType == highPass) filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);
			else if(mtProject.instrument[step->instrumentIndex].filterType == lowPass) filterPtr->setCutoff(0-mtProject.instrument[step->instrumentIndex].cutOff);
		}
		else if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOn)
		{
			filterPtr->setEnvAmount(mtProject.instrument[step->instrumentIndex].envelope[envFilter].amount);
			if(mtProject.instrument[step->instrumentIndex].filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);

				filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL, envelopeControlHighPass);
				filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);
			}
			else if(mtProject.instrument[step->instrumentIndex].filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);

				filterPtr->frequency(LOW_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL,envelopeControlLowPass);
				filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);
			}
		}
		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain( (step->volume/100.0) * mtProject.instrument[step->instrumentIndex].envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/

		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
		/*======================================================================================================*/

		status = playMemPtr->play(step,mod);
		envelopeAmpPtr->noteOn();
		if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOn) envelopeFilterPtr->noteOn();

		return status;

	}
	uint8_t instrumentEngine :: play(strInstrument * instr,strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		/*================================================ENVELOPE AMP==========================================*/
		envelopeAmpPtr->delay(instr->envelope[envAmp].delay);
		envelopeAmpPtr->attack(instr->envelope[envAmp].attack);
		envelopeAmpPtr->hold(instr->envelope[envAmp].hold);
		envelopeAmpPtr->decay(instr->envelope[envAmp].decay);
		envelopeAmpPtr->sustain(instr->envelope[envAmp].sustain);
		envelopeAmpPtr->release(instr->envelope[envAmp].release);
		/*======================================================================================================*/
		/*================================================ENVELOPE FILTER=======================================*/
		envelopeFilterPtr->delay(instr->envelope[envFilter].delay);
		envelopeFilterPtr->attack(instr->envelope[envFilter].attack);
		envelopeFilterPtr->hold(instr->envelope[envFilter].hold);
		envelopeFilterPtr->decay(instr->envelope[envFilter].decay);
		envelopeFilterPtr->sustain(instr->envelope[envFilter].sustain);
		envelopeFilterPtr->release(instr->envelope[envFilter].release);
		/*======================================================================================================*/
		/*================================================FILTER================================================*/
		filterPtr->resonance(instr->resonance);

		if(instr->filterEnvelope == envelopeOff)
		{
			filterPtr->octaveControl(0.0,manualControl);
			dcFilterPtr->amplitude(0.0);

			if(instr->filterType == highPass) filterPtr->setCutoff(instr->cutOff);
			else if(instr->filterType == lowPass) filterPtr->setCutoff(0-instr->cutOff);
		}
		else if(instr->filterEnvelope == envelopeOn)
		{
			filterPtr->setEnvAmount(instr->envelope[envFilter].amount);
			if(instr->filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);

				filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL, envelopeControlHighPass);
				filterPtr->setCutoff(instr->cutOff);
			}
			else if(instr->filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);

				filterPtr->frequency(LOW_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL,envelopeControlLowPass);
				filterPtr->setCutoff(instr->cutOff);
			}
		}
		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain(instr->envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);
		/*======================================================================================================*/
		status = playMemPtr->play(instr,mod,1.0,24);
		envelopeAmpPtr->noteOn();

		return status;

	}


	uint8_t instrumentEngine :: change(strStep * step, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		float gainL=0,gainR=0;

		envelopeFilterPtr->sustain(mtProject.instrument[step->instrumentIndex].envelope[envFilter].sustain);
		/*================================================FILTER================================================*/
		filterPtr->resonance(mtProject.instrument[step->instrumentIndex].resonance);

		if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOff)
		{
			filterPtr->octaveControl(0.0,manualControl);
			dcFilterPtr->amplitude(0.0);

			if(mtProject.instrument[step->instrumentIndex].filterType == highPass) filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);
			else if(mtProject.instrument[step->instrumentIndex].filterType == lowPass) filterPtr->setCutoff(0-mtProject.instrument[step->instrumentIndex].cutOff);
		}
		else if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOn)
		{
			filterPtr->setEnvAmount(mtProject.instrument[step->instrumentIndex].envelope[envFilter].amount);
			if(mtProject.instrument[step->instrumentIndex].filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);

				filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL, envelopeControlHighPass);
				filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);
			}
			else if(mtProject.instrument[step->instrumentIndex].filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);

				filterPtr->frequency(LOW_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL,envelopeControlLowPass);
				filterPtr->setCutoff(mtProject.instrument[step->instrumentIndex].cutOff);
			}
		}
		/*======================================================================================================*/

		/*==================================================GAIN================================================*/
		ampPtr->gain( (step->volume/100.0) * mtProject.instrument[step->instrumentIndex].envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
		/*======================================================================================================*/

		status=playMemPtr->setMod(step,mod);


		return status;
	}
	uint8_t instrumentEngine :: change(strInstrument * instr, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		envelopeAmpPtr->sustain(instr->envelope[envAmp].sustain);
		/*================================================FILTER================================================*/
		if(instr->filterEnvelope == envelopeOff)
		{
			filterPtr->octaveControl(0.0,manualControl);
			dcFilterPtr->amplitude(0.0);

			if(instr->filterType == highPass) filterPtr->setCutoff(instr->cutOff);
			else if(instr->filterType == lowPass) filterPtr->setCutoff(0-instr->cutOff);
		}
		else if(instr->filterEnvelope == envelopeOn)
		{
			filterPtr->setEnvAmount(instr->envelope[envFilter].amount);
			if(instr->filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);

				filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL, envelopeControlHighPass);
				filterPtr->setCutoff(instr->cutOff);
			}
			else if(instr->filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);

				filterPtr->frequency(LOW_PASS_FILTER_FREQ);
				filterPtr->octaveControl(MAX_OCTAVE_CONTROL,envelopeControlLowPass);
				filterPtr->setCutoff(instr->cutOff);
			}
		}

		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain(instr->envelope[envAmp].amount);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);
		/*======================================================================================================*/

		status=playMemPtr->setMod(instr,mod,24);

		return status;
	}

	void instrumentEngine:: update()
	{
		if(envelopeAmpPtr->endRelease()) playMemPtr->stop();
	}

	void instrumentEngine :: stop()
	{
		envelopeAmpPtr->noteOff();
		envelopeFilterPtr->noteOff();
	}

	void instrumentEngine :: changeFilterType(uint8_t type)
	{
		conFilterToAmpPtr->disconnect();
		if(type == lowPass) conFilterToAmpPtr->src_index=0;
		else if(type == bandPass) conFilterToAmpPtr->src_index=1;
		else if(type ==  highPass) conFilterToAmpPtr->src_index=2;
		conFilterToAmpPtr->connect();
	}

