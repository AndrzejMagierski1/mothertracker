
#ifndef SOURCE_MTAUDIOENGINE_H_
#define SOURCE_MTAUDIOENGINE_H_

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mtEnvelopeGenerator.h"
#include "mtHardware.h"
#include "mtRecorder.h"
#include "mtExporterWAV.h"


constexpr uint32_t NOT_MOD_POINTS = 1000000;

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
	void soloTrack(uint8_t channel, uint8_t state);
	void muteReverbSend(uint8_t channel, uint8_t state);
	void soloReverbSend(uint8_t state);
	void clearReverb();
	void performanceModeEndAll();
	void printLog(SdFile * log);

	friend class playerEngine;
private:
	AudioConnection* i2sConnect[2];
	uint8_t forceSend = 0;
};


class playerEngine
{
public:

	void init(AudioPlayMemory * playMem,envelopeGenerator* envFilter,AudioFilterStateVariable * filter, AudioEffectEnvelope * envAmp, AudioAmplifier * amp,
			uint8_t panCh, envelopeGenerator* envWtPos, envelopeGenerator * envGranPos, envelopeGenerator * envPanning );


	uint8_t noteOn(uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx_id, uint8_t fx_val,uint8_t,uint8_t );
	void noteOff(int8_t option = -4);
	void clean();

	void seqFx(uint8_t fx_id, uint8_t fx_val, uint8_t fx_n);
	void endFx(uint8_t fx_id, uint8_t fx_n);
	void slide(int8_t note, uint16_t time);
	void modPitch(float value);
	void modGlide(uint16_t value);
	void modFineTune(int8_t value);

	void modPanning(int16_t value);
	void modPlayMode(uint8_t value);
	void modSP(uint16_t value);
	void modLP1(uint16_t value);
	void modLP2(uint16_t value);
	void modCutoff(float value);
	void modResonance(float value);

	void modSeqPoints(uint32_t sp, uint32_t ep);

	void modWavetableWindow(uint16_t value);
	void modTune(int8_t value);
	void modReverbSend(uint8_t value);

	void modGranularPosition(uint16_t value);
	void modGranularGrainLength();

	void setStatusBytes(uint16_t value);
//	void resetMods();
	uint8_t getInterfaceEndReleaseFlag();
	void clearInterfaceEndReleaseFlag();
	uint8_t getInterfacePlayingEndFlag();
	void clearInterfacePlayingEndFlag();
	uint32_t getEnvelopeGranPosMod();

	uint32_t getEnvelopeWtPosMod();
	uint32_t afterEnvelopeWtPos;

	uint16_t getWavePosition();
	void update();
	uint8_t noteOnforPrev (uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t type);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note,uint8_t type);
	AudioEffectEnvelope *       envelopeAmpPtr;

	elapsedMillis ampLfoRefreshTimer;
	elapsedMillis pitchLfoRefreshTimer;
	elapsedMillis envelopesRefreshTimer;

	const uint8_t LFO_REFRESH_TIME = 5;
	const uint8_t ENVELOPES_REFRESH_TIME_MS = 10;

	enum struct controlType
	{
		sequencerMode,
		sequencerMode2,
		performanceMode,

		length
	};

	enum struct parameterList
	{
		startPoint,
		loopPoint1,
		loopPoint2,
		endPoint,
		filterCutoff,
		filterType,
		filterEnable,
		reverbSend,
		panning,
		ampAttack,
		ampRelease,
		filterAttack,
		filterRelease,
		fineTune,
		tune,
		lfoFineTune,
		lfoAmp,
		volume,
		samplePlaybeckDirection,
		sampleSlice,
		glide,
		wavetablePosition,
		granularPosition,
		slice,

		length
	};

	const uint8_t MOST_SIGNIFICANT_FX = 0;
	const uint8_t LEAST_SIGNIFICANT_FX = 1;

	struct strCurrentSeqModValues
	{
	    uint16_t startPoint;
	    uint16_t loopPoint1;
	    uint16_t loopPoint2;
	    uint16_t endPoint;
	    uint8_t sampleSlice; //todo: ustalic co to ma byc
	    uint8_t reversePlayback;
	    float filterCutoff;
		uint8_t filterType;
		uint8_t filterEnable;
		uint8_t reverbSend;
		int16_t panning;

		uint16_t ampAttack;
		uint16_t ampRelease;
		uint16_t filterAttack;
		uint16_t filterRelease;

		int8_t fineTune;
		uint16_t glide;
		uint32_t wavetablePosition;
		uint16_t granularPosition;
		uint8_t volume;
		uint8_t slice;

	} currentSeqModValues ;

	struct strCurrentPerformanceValues
	{
		uint16_t startPoint;
		uint16_t loopPoint1;
		uint16_t loopPoint2;
		uint16_t endPoint;
		uint8_t reversePlayback;
		float filterCutoff;
		uint8_t filterType;
		uint8_t filterEnable;
		uint8_t reverbSend;
		int16_t panning;
		int8_t tune;
		uint8_t volume;
		uint32_t wavetablePosition;
		uint16_t granularPosition;
	} currentPerformanceValues;


	struct strPerformanceMod
	{
		  int8_t 	volume;
		  int8_t 	panning;
		  int8_t 	tune;
		  int8_t 	reverbSend;
		  int8_t 	cutoff;
		  int16_t 	wavetablePosition;
		  int16_t   granularPosition;
		  uint8_t 	filterType;
		  int32_t 	startPoint;
		  int32_t	endPoint;
	} performanceMod;

	struct strInstrumentBasedModValue
	{
		int8_t fineTune;
		uint8_t volume;
		float cutoff;
		uint32_t wtPos;
		uint16_t granPos;
		int8_t panning;
	} instrumentBasedMod;



	uint8_t trackControlParameter[(int)controlType::length][(int)parameterList::length];
//**********************************************************************************************************************************
//PERFORMANCE MODE
//*****************************CHANGE
	void changeVolumePerformanceMode(int8_t value);
	void changePanningPerformanceMode(int8_t value);
	void changeTunePerformanceMode(int8_t value);
	void changeReverbSendPerformanceMode(int8_t value);
	void changeStartPointPerformanceMode(int32_t value);
	void changeEndPointPerformanceMode(int32_t value);
	void changeCutoffPerformanceMode(int8_t value);
	void changeFilterTypePerformanceMode(uint8_t mode);
	void changeSamplePlaybackPerformanceMode(uint8_t value);
	void changeWavetableWindowPerformanceMode(int16_t value);
	void changeGranularPositionPerformanceMode(int16_t value);

//*****************************END


	void endVolumePerformanceMode();
	void endPanningPerformanceMode();
	void endTunePerformanceMode();
	void endReverbSendPerformanceMode();
	void endStartPointPerformanceMode();
	void endCutoffPerformanceMode();
	void endFilterTypePerformanceMode();
	void endSamplePlaybackPerformanceMode();
	void endEndPointPerformanceMode();
	void endWavetableWindowPerformanceMode();
	void endGranularPositionPerformanceMode();

	void printLog(SdFile * log);

private:

	friend 						audioEngine;

	AudioPlayMemory *        	playMemPtr;
	AudioAmplifier *			ampPtr;
	envelopeGenerator* 			envelopeFilterPtr;
	envelopeGenerator* 			envelopeWtPos;
	envelopeGenerator*			envelopeGranPos;
	envelopeGenerator*			envelopePanningPtr;
	AudioFilterStateVariable *	filterPtr;
	AudioConnection*			conFilterToAmpPtr;
	AudioConnection*			conPlayToFilterPtr;
	uint8_t 					numPanChannel;
	uint8_t						lastSeqFx[2];
	uint8_t						lastSeqVal[2];
	uint8_t 					currentInstrument_idx;
	int8_t						currentNote;
	int8_t						currentVelocity;
	uint16_t 					statusBytes; // 8- reverbSend 7-resonance, 6-cutoff, 5-panning ,4-volume,3-tune,2-fineTune, 1-LP1 , 0-LP2
	uint8_t 					interfaceEndReleaseFlag = 0;
	uint8_t 					interfacePlayingEndFlag = 0;
	uint8_t 					currentPlayState = 0;
	uint8_t 					lastPlayState = 0;
	int8_t						filterTypeSequencer = -1;
	int8_t 						filterTypePerformanceMode = -1;

	uint8_t 					muteState = 0;
	uint8_t						onlyReverbMuteState = 0;

	envelopeGenerator::strEnv lfoBasedEnvelope[envelopeTypeMax];

	void changeFilterType(uint8_t type);
	void filterConnect();
	void filterDisconnect();

	void changePointsPerformanceMode(int32_t spValue, int32_t epValue);
	void endPointsPerformanceMode();

	void calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo);
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
extern AudioBitDepth			bitDepthControl[2];

extern AudioInputI2S            i2sIn;
extern AudioRecordQueue         queue;
extern AudioMixer4              mixerRec;
extern AudioAnalyzeRMS			rms;

extern AudioRecordQueue		 	exportL, exportR;
extern AudioAnalyzeRMS			exportRmsL, exportRmsR;

#endif /* SOURCE_MTAUDIOENGINE_H_ */
