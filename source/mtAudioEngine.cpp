#include "mtAudioEngine.h"

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelope[8];
AudioAmplifier           amp[8];
AudioMixer8				 mixerL,mixerR;
AudioOutputI2S           i2s1;

AudioConnection          patchCord1(playMem[7], envelope[7]);
AudioConnection          patchCord2(playMem[5], envelope[5]);
AudioConnection          patchCord3(playMem[6], envelope[6]);
AudioConnection          patchCord4(playMem[3], envelope[3]);
AudioConnection          patchCord5(playMem[4], envelope[4]);
AudioConnection          patchCord6(playMem[1], envelope[1]);
AudioConnection          patchCord7(playMem[2], envelope[2]);
AudioConnection          patchCord8(playMem[0], envelope[0]);
AudioConnection          patchCord9(envelope[2], amp[2]);
AudioConnection          patchCord10(envelope[3], amp[2]);
AudioConnection          patchCord11(envelope[1], amp[1]);
AudioConnection          patchCord12(envelope[7], amp[7]);
AudioConnection          patchCord13(envelope[4], amp[4]);
AudioConnection          patchCord14(envelope[6], amp[6]);
AudioConnection          patchCord15(envelope[5], amp[5]);
AudioConnection          patchCord16(envelope[0], amp[0]);
AudioConnection          patchCord17(amp[7], 0, mixerL, 7);
AudioConnection          patchCord18(amp[7], 0, mixerR, 7);
AudioConnection          patchCord19(amp[6], 0, mixerL, 6);
AudioConnection          patchCord20(amp[6], 0, mixerR, 6);
AudioConnection          patchCord21(amp[2], 0, mixerL, 2);
AudioConnection          patchCord22(amp[2], 0, mixerR, 2);
AudioConnection          patchCord23(amp[1], 0, mixerL, 1);
AudioConnection          patchCord24(amp[1], 0, mixerR, 1);
AudioConnection          patchCord25(amp[0], 0, mixerL, 0);
AudioConnection          patchCord26(amp[0], 0, mixerR, 0);
AudioConnection          patchCord27(amp[4], 0, mixerL, 4);
AudioConnection          patchCord28(amp[4], 0, mixerR, 4);
AudioConnection          patchCord29(amp[3], 0, mixerL, 3);
AudioConnection          patchCord30(amp[3], 0, mixerR, 3);
AudioConnection          patchCord31(amp[5], 0, mixerL, 5);
AudioConnection          patchCord32(amp[5], 0, mixerR, 5);
AudioConnection          patchCord33(mixerL, 0, i2s1, 1);
AudioConnection          patchCord34(mixerR, 0, i2s1, 0);



	instrumentEngine instrumentPlayer[8];
	strMtModAudioEngine modAudioEngine[8];
	audioEngine engine;

	void audioEngine::init()
	{
		for(int i=0;i<8; i++)
		{
			instrumentPlayer[i].init(&playMem[i],&envelope[i], &amp[i], i);
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
	uint8_t instrumentEngine :: play(strInstrument * instr,strMtModAudioEngine * mod, int8_t note)
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

		ampPtr->gain(instr->volume);

		status = playMemPtr->play(instr,mod,1.0,24);
		envelopePtr->noteOn();

		return status;

	}


	uint8_t instrumentEngine :: change(strStep * step, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		float gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		float gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);

		status=playMemPtr->setMod(step,mod);

		return status;
	}
	uint8_t instrumentEngine :: change(strInstrument * instr, strMtModAudioEngine * mod)
	{
		uint8_t status=0;

		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);

		ampPtr->gain(instr->volume);
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
