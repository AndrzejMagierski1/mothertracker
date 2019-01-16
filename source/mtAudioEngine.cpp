#include "mtAudioEngine.h"

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelope[8];
AudioMixer4              mixer[3];
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(playMem[7], envelope[7]);
AudioConnection          patchCord2(playMem[5], envelope[5]);
AudioConnection          patchCord3(playMem[6], envelope[6]);
AudioConnection          patchCord4(playMem[3], envelope[3]);
AudioConnection          patchCord5(playMem[4], envelope[4]);
AudioConnection          patchCord6(playMem[1], envelope[1]);
AudioConnection          patchCord7(playMem[2], envelope[2]);
AudioConnection          patchCord8(playMem[0], envelope[0]);
AudioConnection          patchCord9(envelope[2], 0, mixer[0], 2);
AudioConnection          patchCord10(envelope[3], 0, mixer[0], 3);
AudioConnection          patchCord11(envelope[1], 0, mixer[0], 1);
AudioConnection          patchCord12(envelope[7], 0, mixer[1], 3);
AudioConnection          patchCord13(envelope[4], 0, mixer[1], 0);
AudioConnection          patchCord14(envelope[6], 0, mixer[1], 2);
AudioConnection          patchCord15(envelope[5], 0, mixer[1], 1);
AudioConnection          patchCord16(envelope[0], 0, mixer[0], 0);
AudioConnection          patchCord17(mixer[1], 0, mixer[2], 1);
AudioConnection          patchCord18(mixer[0], 0, mixer[2], 0);
AudioConnection          patchCord19(mixer[2], 0, i2s1, 0);
AudioConnection          patchCord20(mixer[2], 0, i2s1, 1);

	instrumentEngine instrumentPlayer[8];
	strMtModAudioEngine modAudioEngine[8];
	audioEngine engine;

	void audioEngine::init()
	{
		for(int i=0;i<8; i++)
		{
			instrumentPlayer[i].init(&playMem[i],&envelope[i]);
		}
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				mixer[j].gain(i,1.0);
			}
		}
		modAudioEngine[0].startPointMod=relativeMod;
		modAudioEngine[0].endPointMod=relativeMod;
		modAudioEngine[0].loopPoint1Mod=relativeMod;
		modAudioEngine[0].loopPoint2Mod=relativeMod;
		modAudioEngine[0].pitchCtrlMod=relativeMod;
		modAudioEngine[0].startPoint=0;
		modAudioEngine[0].endPoint=0;
		modAudioEngine[0].loopPoint1=0;
		modAudioEngine[0].loopPoint2=0;
		modAudioEngine[0].pitchCtrl=0;


	}
	void audioEngine::update()
	{
		instrumentPlayer[0].update();
	}

	void instrumentEngine::init(AudioPlayMemory * playMem,AudioEffectEnvelope * env)
	{
		playMemPtr=playMem;
		envelopePtr=env;
	}

	uint8_t instrumentEngine :: play(strStep * step,strMtModAudioEngine * mod)
	{
		uint8_t status;
		envelopePtr->delay(mtProject.instrument[step->instrumentIndex].ampDelay);
		envelopePtr->attack(mtProject.instrument[step->instrumentIndex].ampAttack);
		envelopePtr->hold(mtProject.instrument[step->instrumentIndex].ampHold);
		envelopePtr->decay(mtProject.instrument[step->instrumentIndex].ampDecay);
		envelopePtr->sustain(mtProject.instrument[step->instrumentIndex].ampSustain);
		envelopePtr->release(mtProject.instrument[step->instrumentIndex].ampRelease);



		status = playMemPtr->play(step,mod);
		envelopePtr->noteOn();

		return status;

	}
	uint8_t instrumentEngine :: play(strInstrument * instr,strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		envelopePtr->delay(instr->ampDelay);
		envelopePtr->attack(instr->ampAttack);
		envelopePtr->hold(instr->ampHold);
		envelopePtr->decay(instr->ampDecay);
		envelopePtr->sustain(instr->ampSustain);
		envelopePtr->release(instr->ampRelease);

		status = playMemPtr->play(instr,mod,1.0);
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

		status=playMemPtr->setMod(instr,mod);

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
