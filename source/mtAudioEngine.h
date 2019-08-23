
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
#include "mtRecorder.h"
#include "mtExporterWAV.h"


class audioEngine
{
public:
	void init();
	void update();
	void prevSdConnect();
	void prevSdDisconnect();
	void setReverbRoomsize(uint8_t value);
	void setReverbDamping(uint8_t value);
	void setReverbPanning(int8_t value);
	void setLimiterAttack(uint16_t attack);
	void setLimiterRelease(uint16_t release);
	void setLimiterTreshold(uint16_t threshold);
	void muteTrack(uint8_t channel);
	void wavExportConnect();
	void wavExportDisconnect();
private:
	AudioConnection* i2sConnect[2];
};



class playerEngine
{
public:

	void init(AudioPlayMemory * playMem,envelopeGenerator* envFilter,AudioFilterStateVariable * filter, AudioEffectEnvelope * envAmp, AudioAmplifier * amp,
			uint8_t panCh, LFO * lfoAmp, LFO * lfoFilter, LFO * lfoPitch );


	uint8_t noteOn(uint8_t instr_idx,int8_t note, int8_t velocity);
	void noteOff();
	void clean();

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
	void modReverbSend(uint8_t value);
	void setStatusBytes(uint16_t value);
//	void resetMods();

	void update();
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note);
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
	uint16_t 					statusBytes; // 8- reverbSend 7-resonance, 6-cutoff, 5-panning ,4-volume,3-tune,2-fineTune, 1-LP1 , 0-LP2
	static uint8_t				onVoices;
	static uint8_t				activeAmpEnvelopes;

	void changeFilterType(uint8_t type);
	void filterConnect();
	void filterDisconnect();

	float fmap(float x, float in_min, float in_max, float out_min, float out_max);

};




extern playerEngine instrumentPlayer[8];
extern audioEngine engine;
extern AudioEffectLimiter		limiter[2];
extern AudioPlaySdWav           playSdWav;
extern AudioPlaySdWavFloat 		playSdWavFloat;
extern AudioPlaySdWav24bit 	 	playSdWav24Bit;
extern AudioPlayMemory          playMem[8];
extern AudioEffectEnvelope      envelopeAmp[8];
extern envelopeGenerator		envelopeFilter[8];
extern AudioFilterStateVariable filter[8];
extern AudioAmplifier           amp[8];
extern AudioMixer9				mixerL,mixerR,mixerReverb;
extern AudioOutputI2S           i2s1;
extern LFO						lfoAmp[8];
extern LFO						lfoFilter[8];
extern LFO						lfoPitch[8];
extern int16_t					mods[MAX_TARGET][MAX_MOD];

extern AudioInputI2S            i2sIn;
extern AudioRecordQueue         queue;
extern AudioMixer4              mixerRec;
extern AudioAnalyzeRMS			rms;

extern AudioRecordQueue		 exportL, exportR;

#endif /* SOURCE_MTAUDIOENGINE_H_ */
