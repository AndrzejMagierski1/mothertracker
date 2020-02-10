
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


const float tempoSyncRates[20] =
{
	6,
	4,
	3,
	2,
	1.5,
	1,
	0.75,
	0.5,
	0.375,
	0.333333,
	0.25,
	0.1875,
	0.166667,
	0.125,
	0.083333,
	0.0625,
	0.041667,
	0.03125,
	0.020833,
	0.015625
};

constexpr uint32_t NOT_MOD_POINTS = 1000000;
constexpr uint8_t ENVELOPES_WITHOUT_AMP_MAX = 4;
constexpr uint8_t MUTE_DISABLE = 0;
constexpr uint8_t MUTE_ENABLE = 1;
constexpr float AMP_MUTED = 0.0f;

extern IntervalTimer updateTimer;

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
	void startTestSignal(float amp, float freq);
	void stopTestSignal();
	void endAllFx();
	void setPassEnvelope(uint8_t state);

	friend class playerEngine;
private:
	uint8_t forceSend = 0;
};


class playerEngine
{
public:

	playerEngine();

	uint8_t noteOn(uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx1_id, uint8_t fx1_val, uint8_t fx2_id, uint8_t fx2_val); // tylko ten note on do nut w sequencerze
	void noteOff(int8_t option = -4);
	void clean();

	void seqFx(uint8_t fx_id, uint8_t fx_val, uint8_t fx_n);
	void endFx(uint8_t fx_id, uint8_t fx_n);
	void slide(int8_t note, uint16_t time);
	void modPitch(float value);
	void modGlide(uint16_t value);
	void modFineTune(int8_t value);

	void modPanning(int16_t value);
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

	void setStatusBytes(uint32_t value);
//	void resetMods();
	uint8_t getInterfaceEndReleaseFlag();
	void clearInterfaceEndReleaseFlag();
	uint8_t getInterfacePlayingEndFlag();
	void clearInterfacePlayingEndFlag();
	uint32_t getEnvelopeGranPosMod();

	uint32_t getEnvelopeWtPosMod();

	uint16_t getWavePosition();
	void update();
	uint8_t noteOnforPrev (uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t type);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note,uint8_t type);
	AudioEffectEnvelope * envelopeAmpPtr;

	uint8_t currentInstrument_idx;

//********************************************* ujednolicenie obslugi envelope
	enum struct envWithoutAmp
	{
		panning,
		filter,
		wtPos,
		granPos
	};

	const uint8_t envelopesWithoutAmpIdx[ENVELOPES_WITHOUT_AMP_MAX] =
	{
			envPan,
			envFilter,
			envWtPos,
			envGranPos
	}; // na potrzeby wykonania czegos w petli - przefiltrowanie enumów z env do aktywnych bez ampa - musi korespondować z envelopesWithoutAmpControlValue

	const uint8_t envelopesWithoutAmpControlValue[ENVELOPES_WITHOUT_AMP_MAX] =
	{
			(uint8_t)parameterList::lfoPanning,
			(uint8_t)parameterList::lfoCutoff,
			(uint8_t)parameterList::lfoWavetablePosition,
			(uint8_t)parameterList::lfoGranularPosition
	}; // na potrzeby wykonania czegos w petli - przefiltrowanie enumów parametrów dla lfo bez ampa - musi korespondować z envelopesWithoutAmpIdx
//********************************************
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
		lfoCutoff,
		lfoWavetablePosition,
		lfoGranularPosition,
		lfoPanning,
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
	    uint8_t sampleSlice;
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

		uint8_t lfoAmpEnable;
		uint8_t lfoAmpRate;

		uint8_t lfoCutoffEnable;
		uint8_t lfoCutoffRate;

		uint8_t lfoWavetablePositionEnable;
		uint8_t lfoWavetablePositionRate;

		uint8_t lfoGranularPositionEnable;
		uint8_t lfoGranularPositionRate;

		uint8_t lfoPanningEnable;
		uint8_t lfoPanningRate;

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

		uint8_t lfoAmpEnable;
		uint8_t lfoAmpRate;

		uint8_t lfoCutoffEnable;
		uint8_t lfoCutoffRate;

		uint8_t lfoWavetablePositionEnable;
		uint8_t lfoWavetablePositionRate;

		uint8_t lfoGranularPositionEnable;
		uint8_t lfoGranularPositionRate;

		uint8_t lfoPanningEnable;
		uint8_t lfoPanningRate;

	} currentPerformanceValues;


	struct strPerformanceMod
	{
		int8_t 	volume;
		int8_t 	panning;
		int8_t 	tune;
		int8_t 	reverbSend;
		int8_t 	cutoff;
		int16_t wavetablePosition;
		int16_t granularPosition;
		uint8_t filterType;
		int32_t startPoint;
		int32_t	endPoint;
		int8_t  lfoAmpRate;
		int8_t  lfoCutoffRate;
		int8_t  lfoPositionRate;
		int8_t  lfoPanningRate;
	} performanceMod;

	float currentEnvelopeModification[envMax];

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

	void changePositionPerformanceMode(int16_t value);

	void changeAmpLfoRatePerformanceMode(int8_t value);
	void changeCutoffLfoRatePerformanceMode(int8_t value);
	void changePositionLfoRatePerformanceMode(int8_t value);
	void changePanningLfoRatePerformanceMode(int8_t value);


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

	void endPositionPerformanceMode();

	void endAmpLfoRatePerformanceMode();
	void endCutoffLfoRatePerformanceMode();
	void endPositionLfoRatePerformanceMode();
	void endPanningLfoRatePerformanceMode();


	void printLog(SdFile * log);
	void setPassEnvelope(uint8_t state);
private:

	friend 						audioEngine;

	AudioPlayMemory *        	playMemPtr;
	AudioAmplifier *			ampPtr;
	envelopeGenerator *			envelopePtr[envMax];
	AudioFilterStateVariable *	filterPtr;
	uint8_t 					nChannel;
	uint8_t						lastSeqFx[2];
	uint8_t						lastSeqVal[2];
	int8_t						currentNote;
	uint32_t 					statusBytes;
	uint8_t 					interfaceEndReleaseFlag = 0;
	uint8_t 					interfacePlayingEndFlag = 0;
	uint8_t 					currentPlayState = 0;
	uint8_t 					lastPlayState = 0;

	uint8_t 					muteState = 0;
	uint8_t						onlyReverbMuteState = 0;

	uint8_t 					envelopePassFlag = 0;

	envelopeGenerator::strEnv   lfoBasedEnvelope[envMax];

	uint8_t isActiveFlag = 0;

//**********************NOTE ON HANDLERS
//*****note on fx
	void handleInitFxNoteOnEnvelope(uint8_t n);
	void handleInitFxNoteOnAmpEnvelope();
	void handleInitFxNoteOnAllEnvelopes(); // wywolywany w note on fx
	void handleInitFxNoteOnFilter();
	void handleFxNoteOnFilter();
	void handleFxNoteOnGain();
	void handleFxNoteOnPanning();
	void handleFxNoteOnReverbSend();
//*****note on
	void handleInitNoteOnEnvelope(uint8_t n);
	void handleInitNoteOnAmpEnvelope();
	void handleInitNoteOnAllEnvelopes(); // wywolywany w note on
	void handleNoteOnFilter();
	void handleNoteOnGain();
	void handleNoteOnPanning();
	void handleNoteOnReverbSend();
//*************************************************
	void changeFilterType(uint8_t type);
	void filterConnect();
	void filterDisconnect();

	void changePointsPerformanceMode(int32_t spValue, int32_t epValue);
	void endPointsPerformanceMode();

	void calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo);
	void calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo, uint8_t rate);

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
extern AudioSynthWaveform		testWaveform;

#endif /* SOURCE_MTAUDIOENGINE_H_ */
