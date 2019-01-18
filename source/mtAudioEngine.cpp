#include "mtAudioEngine.h"

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
AudioEffectEnvelope      envelopeFilter[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer8				 mixerL,mixerR;
AudioOutputI2S           i2s1;
AudioSynthWaveformDc     dc[0];

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

AudioConnection          connect57(mixerL, 0, i2s1, 1);
AudioConnection          connect59(mixerR, 0, i2s1, 0);



	instrumentEngine instrumentPlayer[8];
	strMtModAudioEngine modAudioEngine[8];
	audioEngine engine;

	void audioEngine::init()
	{
		for(int i=0;i<8; i++)
		{
			instrumentPlayer[i].init(&playMem[i],&envelopeAmp[i], &amp[i], i);
		}

	}
	void audioEngine::update()
	{
		for(int i=0; i<8; i++)
		{
			instrumentPlayer[i].update();
		}
	}

	void instrumentEngine::init(AudioPlayMemory * playMem,AudioEffectEnvelope * env, AudioAmplifier * amp, uint8_t panCh)
	{
		playMemPtr=playMem;
		envelopePtr=env;
		ampPtr=amp;
		numPanChannel=panCh;
	}

	uint8_t instrumentEngine :: play(strStep * step,strMtModAudioEngine * mod)
	{
		uint8_t status;
		float gainL=0,gainR=0;
		envelopePtr->delay(mtProject.instrument[step->instrumentIndex].envelopeAmp.delay);
		envelopePtr->attack(mtProject.instrument[step->instrumentIndex].envelopeAmp.attack);
		envelopePtr->hold(mtProject.instrument[step->instrumentIndex].envelopeAmp.hold);
		envelopePtr->decay(mtProject.instrument[step->instrumentIndex].envelopeAmp.decay);
		envelopePtr->sustain(mtProject.instrument[step->instrumentIndex].envelopeAmp.sustain);
		envelopePtr->release(mtProject.instrument[step->instrumentIndex].envelopeAmp.release);

		ampPtr->gain(step->volume/100.0);

		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);

		status = playMemPtr->play(step,mod);
		envelopePtr->noteOn();

		return status;

	}
	uint8_t instrumentEngine :: play(strInstrument * instr,strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		envelopePtr->delay(instr->envelopeAmp.delay);
		envelopePtr->attack(instr->envelopeAmp.attack);
		envelopePtr->hold(instr->envelopeAmp.hold);
		envelopePtr->decay(instr->envelopeAmp.decay);
		envelopePtr->sustain(instr->envelopeAmp.sustain);
		envelopePtr->release(instr->envelopeAmp.release);

		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);

		status = playMemPtr->play(instr,mod,1.0,24);
		envelopePtr->noteOn();

		return status;

	}


	uint8_t instrumentEngine :: change(strStep * step, strMtModAudioEngine * mod)
	{
		uint8_t status=0;

		status=playMemPtr->setMod(step,mod);

		return status;
	}
	uint8_t instrumentEngine :: change(strInstrument * instr, strMtModAudioEngine * mod)
	{
		uint8_t status=0;

		status=playMemPtr->setMod(instr,mod,24);

		return status;
	}

	void instrumentEngine:: update()
	{
		if(envelopePtr->endRelease()) playMemPtr->stop();
	}

	void instrumentEngine :: stop()
	{
		envelopePtr->noteOff();
	}
