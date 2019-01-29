
#ifndef SOURCE_MTAUDIOENGINE_H_
#define SOURCE_MTAUDIOENGINE_H_

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "mtEnvelopeGenerator.h"



class audioEngine
{
public:
	void init();
	void update();

};



class instrumentEngine
{
public:

	void init(AudioPlayMemory * playMem,envelopeGenerator* envFilter,AudioFilterStateVariable * filter, AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh);

	uint8_t play(strStep * step, strMtModAudioEngine * mod);
	uint8_t play(strInstrument * instr, strMtModAudioEngine *mod);

	uint8_t change(strStep * step,strMtModAudioEngine * mod);
	uint8_t change(strInstrument * instr,strMtModAudioEngine * mod);

	void changeFilterType(uint8_t type);
	void filterConnect();
	void filterDisconnect();

	void update();


	void stop();


private:

	AudioPlayMemory *        	playMemPtr;
	AudioEffectEnvelope *       envelopeAmpPtr;
	AudioAmplifier *			ampPtr;
	envelopeGenerator* 			envelopeFilterPtr;
	AudioFilterStateVariable *	filterPtr;
	AudioConnection*			conFilterToAmpPtr;
	AudioConnection*			conPlayToFilterPtr;
	uint8_t 					numPanChannel;
	strStep *					actualStepPtr=NULL;
	strInstrument *				actualInstrPtr=NULL;

	float fmap(float x, float in_min, float in_max, float out_min, float out_max);

};


extern AudioPlayMemory          playMem[8];
extern AudioEffectEnvelope      envelopeAmp[8];
extern envelopeGenerator		envelopeFilter[8];
extern AudioFilterStateVariable filter[8];
extern AudioAmplifier           amp[8];
extern AudioMixer8				mixerL,mixerR;
extern AudioOutputI2S           i2s1;


#endif /* SOURCE_MTAUDIOENGINE_H_ */
