
#ifndef SOURCE_MTAUDIOENGINE_H_
#define SOURCE_MTAUDIOENGINE_H_

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>



extern uint32_t wavLen1;
extern int16_t wavBuf1[500800];
extern AudioPlayMemory playMem1;


class audioEngine
{
public:
	void init();
	void update();

};



class instrumentEngine
{
public:

	void init(AudioPlayMemory * playMem,AudioEffectEnvelope * env, AudioAmplifier * amp, uint8_t panCh);

	uint8_t play(strStep * step, strMtModAudioEngine * mod);
	uint8_t play(strInstrument * instr, strMtModAudioEngine *mod);

	uint8_t change(strStep * step, strMtModAudioEngine * mod);
	uint8_t change(strInstrument * instr,strMtModAudioEngine * mod);

	void update();
	void stop();


private:

	AudioPlayMemory *        	playMemPtr;
	AudioEffectEnvelope *       envelopePtr;
	AudioAmplifier *			ampPtr;
	uint8_t 					numPanChannel;

};

extern instrumentEngine instrumentPlayer[8];
extern strMtModAudioEngine modAudioEngine[8];
extern audioEngine engine;

#endif /* SOURCE_MTAUDIOENGINE_H_ */
