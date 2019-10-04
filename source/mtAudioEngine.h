
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

struct strPerformance
{
//****************************************************************************
// wartosc modyfikujaca
	struct strMod
	{
	  int8_t 	volume;
	  int8_t 	panning;
	  int8_t 	tune;
	  int8_t 	reverbSend;
	  int8_t 	cutoff;
	  int32_t 	startPoint;
	  int32_t 	loopPoint1;
	  int32_t 	loopPoint2;
	  int32_t 	endPoint;
	} mod;
//****************************************************************************
// aktualna aktywna wartosc
	struct strActiveValueSeq
	{
	  uint8_t volume;
	  int16_t panning;
	  int8_t tune;
	  uint8_t reverbSend;
	  float cutoff;
	  uint16_t startPoint;
	  uint16_t loopPoint1;
	  uint16_t loopPoint2;
	  uint16_t endPoint;
	} activeValueSeq;

//****************************************************************************
// flagi wymuszajace użycie performance i wymuszone wartosci
	struct strForcedFlags
	{
	  uint8_t volume;
	  uint8_t panning;
	  uint8_t tune;
	  uint8_t reverbSend;
	  uint8_t cutoff;
	} forcedFlags;

	struct strForcedValues
	{
	  uint8_t filterEnable;
	  int8_t filterType = -1;
	} forcedValues;

} ;

enum fx_ID
{
	fx_ID_cutoff = 1
};

class audioEngine
{
public:
	void init();
	void update();
	void prevSdConnect();
	void prevSdDisconnect();
	void setHeadphonesVolume(uint8_t value);
	void setReverbRoomsize(uint8_t value);
	void setReverbDamping(uint8_t value);
	void setReverbPanning(int8_t value);
	void setLimiterAttack(uint16_t attack);
	void setLimiterRelease(float release);
	void setLimiterTreshold(uint16_t threshold);
	void setBitDepth(uint16_t bitDepth);
	void muteTrack(uint8_t channel, uint8_t state);
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
	uint8_t noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx_id, uint8_t fx_val);
	void noteOff();
	void clean();

	void seqFx(uint8_t fx_id, uint8_t fx_val);
	void endFx(uint8_t fx_id);
	void slide(int8_t note, uint16_t time);
	void modPitch(float value);
	void modGlide(uint16_t value);
	void modSlide(uint16_t value,int8_t slideNote);
	void modFineTune(int8_t value);

	void modPanning(int16_t value);
	void modPlayMode(uint8_t value);
	void modSP(uint16_t value);
	void modLP1(uint16_t value);
	void modLP2(uint16_t value);
	void modCutoff(float value);
	void modResonance(float value);

	void modWavetableWindow(uint16_t value);
	void modTune(int8_t value);
	void modReverbSend(uint8_t value);
	void setStatusBytes(uint16_t value);
//	void resetMods();
	uint8_t getInterfaceEndReleaseFlag();
	void clearInterfaceEndReleaseFlag();
	uint8_t getInterfacePlayingEndFlag();
	void clearInterfacePlayingEndFlag();

	uint16_t getWavePosition();
	void update();
	uint8_t noteOnforPrev (uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note);
	AudioEffectEnvelope *       envelopeAmpPtr;
//**********************************************************************************************************************************
//PERFORMANCE MODE
	void changeVolumePerformanceMode(int8_t value);
	void changePanningPerformanceMode(int8_t value);
	void changeTunePerformanceMode(int8_t value);
	void changeReverbSendPerformanceMode(int8_t value);
	void changeStartPointPerformanceMode(int32_t value);
	void changeCutoffPerformanceMode(int8_t value);
	void changeFilterTypePerformanceMode(uint8_t mode);

private:
	strPerformance 				performance;
	friend 						audioEngine;
	AudioPlayMemory *        	playMemPtr;
	AudioAmplifier *			ampPtr;
	envelopeGenerator* 			envelopeFilterPtr;
	AudioFilterStateVariable *	filterPtr;
	AudioConnection*			conFilterToAmpPtr;
	AudioConnection*			conPlayToFilterPtr;
	LFO *						lfoAmpPtr;
	LFO *						lfoFilterPtr;
	LFO *						lfoPitchPtr;
	uint8_t 					numPanChannel;
	uint8_t						lastSeqFx = 0;
	uint8_t 					currentInstrument_idx;
	int8_t						currentNote;
	int8_t						currentVelocity;
	uint16_t 					statusBytes; // 8- reverbSend 7-resonance, 6-cutoff, 5-panning ,4-volume,3-tune,2-fineTune, 1-LP1 , 0-LP2
	static uint8_t				onVoices;
	static uint8_t				activeAmpEnvelopes;
	uint8_t 					interfaceEndReleaseFlag = 0;
	uint8_t 					interfacePlayingEndFlag = 0;
	uint8_t 					currentPlayState = 0;
	uint8_t 					lastPlayState = 0;
	int8_t						filterTypeSequencer = -1;
	int8_t 						filterTypePerformanceMode = -1;

	//TODO: *ziejas
	uint8_t 					muteState = 0;

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
extern AudioBitDepth			bitDepthControl[2];

extern AudioInputI2S            i2sIn;
extern AudioRecordQueue         queue;
extern AudioMixer4              mixerRec;
extern AudioAnalyzeRMS			rms;

extern AudioRecordQueue		 exportL, exportR;

#endif /* SOURCE_MTAUDIOENGINE_H_ */
