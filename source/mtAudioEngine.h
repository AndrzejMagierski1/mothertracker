
#ifndef SOURCE_MTAUDIOENGINE_H_
#define SOURCE_MTAUDIOENGINE_H_

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "mtEnvelopeGenerator.h"
#include "mtLFO.h"
#include "mtHardware.h"



extern uint32_t wavLen1;
extern int16_t wavBuf1[500800];
extern AudioPlayMemory playMem1;


class audioEngine
{
public:
	void init();
	void update();
	void setOut(uint8_t audioOutStatus);
	void setIn(uint8_t audioInStatus);
};



class playerEngine
{
public:

	void init(AudioPlayMemory * playMem,envelopeGenerator* envFilter,AudioFilterStateVariable * filter, AudioEffectEnvelope * envAmp, AudioAmplifier * amp,
			uint8_t panCh, LFO * lfoAmp, LFO * lfoFilter, LFO * lfoPitch );


	uint8_t noteOn(uint8_t instr_idx,int8_t note, int8_t velocity);
	void noteOff();


	void slide(int8_t note, uint16_t time);
	void modPitch(float value);
	void modGlide(uint16_t value);
	void modSlide(uint16_t value,int8_t slideNote);
	void modFineTune(int8_t value);

	void modPanning(int16_t value);
	void modPlayMode(uint8_t value);
//	void modSP(uint16_t value);
	void modLP1(uint16_t value);
	void modLP2(uint16_t value);
	void modCutoff(float value);
	void modResonance(float value);

	void modWavetableWindow(uint16_t value);
	void modTune(int8_t value);
	void setStatusByte(uint8_t value);
//	void resetMods();

	void update();
private:

	AudioPlayMemory *        	playMemPtr;
	AudioEffectEnvelope *       envelopeAmpPtr;
	AudioAmplifier *			ampPtr;
	envelopeGenerator* 			envelopeFilterPtr;
	AudioFilterStateVariable *	filterPtr;
	AudioConnection*			conFilterToAmpPtr;
	AudioConnection*			conPlayToFilterPtr;
	LFO *						lfoAmpPtr;
	LFO *						lfoFilterPtr;
	LFO *						lfoPitchPtr;
	uint8_t 					numPanChannel;

	uint8_t 					currentInstrument_idx;
	int8_t						currentNote;
	int8_t						currentVelocity;
	uint8_t 					statusByte; // 7-resonance, 6-cutoff, 5-panning ,4-volume,3-tune,2-fineTune, 1-LP1 , 0-LP2

	void changeFilterType(uint8_t type);
	void filterConnect();
	void filterDisconnect();
	float fmap(float x, float in_min, float in_max, float out_min, float out_max);

};


extern playerEngine instrumentPlayer[8];
extern audioEngine engine;


extern AudioPlayMemory          playMem[8];
extern AudioEffectEnvelope      envelopeAmp[8];
extern envelopeGenerator		envelopeFilter[8];
extern AudioFilterStateVariable filter[8];
extern AudioAmplifier           amp[8];
extern AudioMixer8				mixerL,mixerR;
extern AudioOutputI2S           i2s1;
extern LFO						lfoAmp[8];
extern LFO						lfoFilter[8];
extern LFO						lfoPitch[8];
extern int16_t					mods[MAX_TARGET][MAX_MOD];



#endif /* SOURCE_MTAUDIOENGINE_H_ */
