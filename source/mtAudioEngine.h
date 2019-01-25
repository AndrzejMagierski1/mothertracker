
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

	void init(AudioPlayMemory * playMem,AudioEffectEnvelope * envFilter,AudioSynthWaveformDc * dcFilter,AudioFilterStateVariable * filter,
			AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh);

	uint8_t play(strStep * step, strMtModAudioEngine * mod);
	uint8_t play(strInstrument * instr, strMtModAudioEngine *mod, int8_t note);

	uint8_t change(strStep * step, strMtModAudioEngine * mod);
	uint8_t change(strInstrument * instr,strMtModAudioEngine * mod);

	void changeFilterType(uint8_t type);


	void update();
	void stop();


private:

	AudioPlayMemory *        	playMemPtr;
	AudioEffectEnvelope *       envelopeAmpPtr;
	AudioEffectEnvelope *       envelopeFilterPtr;
	AudioAmplifier *			ampPtr;
	AudioSynthWaveformDc * 		dcFilterPtr;
	AudioFilterStateVariable *	filterPtr;
	AudioConnection*			conFilterToAmpPtr;
	uint8_t 					numPanChannel;

};


extern instrumentEngine instrumentPlayer[8];
extern strMtModAudioEngine modAudioEngine[8];
extern audioEngine engine;


extern AudioPlayMemory          playMem[8];
extern AudioEffectEnvelope      envelopeAmp[8];
extern AudioEffectEnvelope      envelopeFilter[8];
extern AudioFilterStateVariable filter[8];
extern AudioAmplifier           amp[8];
extern AudioMixer8				mixerL,mixerR;
extern AudioOutputI2S           i2s1;
extern AudioSynthWaveformDc     dc[8];



#endif /* SOURCE_MTAUDIOENGINE_H_ */
