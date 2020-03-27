
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
#include "mtSequencer.h"

typedef Sequencer::strFxConsts fx_t;

const float tempoSyncRatesAmp[20] =
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

const float tempoSyncRatesOthers[25] =
{
	32,
	24,
	16,
	12,
	8,
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

constexpr uint8_t AMP_LOG_VALUES = 101;
const float ampLogValues[AMP_LOG_VALUES] =
{
		0,
		0.0043213738,
		0.0086001718,
		0.0128372247,
		0.0170333393,
		0.0211892991,
		0.0253058653,
		0.0293837777,
		0.0334237555,
		0.0374264979,
		0.0413926852,
		0.0453229788,
		0.0492180227,
		0.0530784435,
		0.0569048513,
		0.0606978404,
		0.0644579892,
		0.0681858617,
		0.0718820073,
		0.0755469614,
		0.079181246,
		0.0827853703,
		0.0863598307,
		0.0899051114,
		0.0934216852,
		0.096910013,
		0.1003705451,
		0.103803721,
		0.1072099696,
		0.1105897103,
		0.1139433523,
		0.1172712957,
		0.1205739312,
		0.123851641,
		0.1271047984,
		0.1303337685,
		0.1335389084,
		0.1367205672,
		0.1398790864,
		0.1430148003,
		0.1461280357,
		0.1492191127,
		0.1522883444,
		0.1553360375,
		0.1583624921,
		0.1613680022,
		0.1643528558,
		0.1673173347,
		0.1702617154,
		0.1731862684,
		0.1760912591,
		0.1789769473,
		0.1818435879,
		0.1846914308,
		0.1875207208,
		0.1903316982,
		0.1931245984,
		0.1958996524,
		0.198657087,
		0.2013971243,
		0.2041199827,
		0.206825876,
		0.2095150145,
		0.2121876044,
		0.214843848,
		0.2174839442,
		0.220108088,
		0.2227164711,
		0.2253092817,
		0.2278867046,
		0.2304489214,
		0.2329961104,
		0.2355284469,
		0.2380461031,
		0.2405492483,
		0.2430380487,
		0.2455126678,
		0.2479732664,
		0.2504200023,
		0.252853031,
		0.2552725051,
		0.2925088798,
		0.3297452546,
		0.3669816293,
		0.4042180041,
		0.4414543788,
		0.4786907536,
		0.5159271283,
		0.5531635031,
		0.5903998778,
		0.6276362526,
		0.6648726273,
		0.702109002,
		0.7393453768,
		0.7765817515,
		0.8138181263,
		0.851054501,
		0.8882908758,
		0.9255272505,
		0.9627636253,
		1

};
constexpr uint32_t NOT_MOD_POINTS = 1000000;
constexpr uint8_t MUTE_DISABLE = 0;
constexpr uint8_t MUTE_ENABLE = 1;
constexpr float AMP_MUTED = 0.0f;
constexpr uint8_t MOST_SIGNIFICANT_FX = 0;
constexpr uint8_t LEAST_SIGNIFICANT_FX = 1;
extern IntervalTimer updateTimer;
constexpr uint8_t ACTIVE_ENVELOPES = envMax - 1;

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
	void clearDelay();
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
	void modDelaySend(uint8_t value);

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

	const uint32_t envTargetRefreshMask[envMax] =
	{
			0,
			PANNING_MASK,
			CUTOFF_MASK,
			WT_POS_SEND_MASK,
			GRANULAR_POS_SEND_MASK,
			0
	};
	const uint8_t envelopesControlValue[envMax] =
	{
			(uint8_t)parameterList::lfoAmp,
			(uint8_t)parameterList::lfoPanning,
			(uint8_t)parameterList::lfoCutoff,
			(uint8_t)parameterList::lfoWavetablePosition,
			(uint8_t)parameterList::lfoGranularPosition,
			0
	};
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
		uint8_t reversePlayback;
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


//******testowe
	void printLog(SdFile * log);
	void setPassEnvelope(uint8_t state);
//*****************
	float getRMSValue();
private:

	friend 						audioEngine;

	AudioPlayMemory *        	playMemPtr;
	AudioAmplifier *			ampPtr;
	envelopeGenerator *			envelopePtr[envMax];
	AudioFilterStateVariable *	filterPtr;
	AudioAnalyzeRMS *			rmsPtr;
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
	uint8_t						onlyDelayMuteState = 0;

	uint8_t 					envelopePassFlag = 0;
	float 						currentSeqTempo = 0;
	float						lastSeqTempo = 0;

	envelopeGenerator::strEnv   lfoBasedEnvelope[envMax];
	const envelopeGenerator::strEnv passEnvelope =
	{
			.amount 	= 1.0,
			.delay 		= 0,
			.attack 	= 0,
			.hold		= 0,
			.decay		= 0,
			.sustain 	= 1.0,
			.release 	= 0,
			.loop		= 0,
			.enable		= 1
	};

	uint8_t isActiveFlag = 0;


//**********************FUNKCJE POMOCNICZE
	uint8_t isFxVelocity(uint8_t fx_id);
	float getMostSignificantAmount();
	uint8_t getMostSignificantVolume();
	void initEnvelopesParamiters(uint8_t n, envelopeGenerator::strEnv * env);
	void setSyncParamsLFO(uint8_t type);
	void setSyncParamsAmpLFO();

//**********************NOTE ON/OFF HANDLERS
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
//*****note off
	void noteOffFade();
	void noteOffCut();
	void noteOffOrdinary();
//*************************************************
//**********************UPDATE HANDLERS
	void handleUpdateEndPlayDetect();
	void handleUpdateEnvelope(uint8_t type, bool enableCondition);
	void handleUpdateEndReleaseAction();
	void handleUpdateRefreshLP1();
	void handleUpdateRefreshLP2();
	void handleUpdateRefreshFinetune();
	void handleUpdateRefreshTune();
	void handleUpdateRefreshVolume();
	void handleUpdateRefreshPanning();
	void handleUpdateRefreshCutoff();
	void handleUpdateRefreshResonance();
	void handleUpdateRefreshReverb();
	void handleUpdateRefreshWtPos();
	void handleUpdateRefreshGranPos();
	void handleUpdateRefreshGranLen();
	void handleUpdateRefreshGranWave();
	void handleUpdateRefreshGranLoop();
	void handleUpdateRefreshAmpLFO();
	void handleUpdateRefreshCutoffLFO();
	void handleUpdateRefreshWtPosLFO();
	void handleUpdateRefreshGranPosLFO();
	void handleUpdateRefreshPanningLFO();

//*************************************************
//************************* FX HANDLE
//******* fx start
	void fxFilter(uint8_t fx_val, uint8_t fx_n, uint8_t type);
	void fxGlide(uint8_t fx_val, uint8_t fx_n);
	void fxFinetune(uint8_t fx_val, uint8_t fx_n);
	void fxPanning(uint8_t fx_val, uint8_t fx_n);
	void fxReverbSend(uint8_t fx_val, uint8_t fx_n);
	void fxReversePlayback(uint8_t fx_val, uint8_t fx_n);
//***position
	void fxPosition(uint8_t fx_val, uint8_t fx_n);
	void fxPositionStartPoint(uint8_t fx_val, uint8_t fx_n);
	void fxPositionWavetable(uint8_t fx_val, uint8_t fx_n);
	void fxPositionGranular(uint8_t fx_val, uint8_t fx_n);
//***
	void fxVolume(uint8_t fx_val, uint8_t fx_n);
	void fxRandomVolume(uint8_t fx_val, uint8_t fx_n);
	void fxSampleSlice(uint8_t fx_val, uint8_t fx_n);
	void fxVolumeLFO(uint8_t fx_val, uint8_t fx_n);
	void fxCutoffLFO(uint8_t fx_val, uint8_t fx_n);
//***position lfo
	void fxPositionLFO(uint8_t fx_val, uint8_t fx_n);
	void fxPositionGranularLFO(uint8_t fx_val, uint8_t fx_n);
	void fxPositionWavetableLFO(uint8_t fx_val, uint8_t fx_n);
//***
	void fxPanningLFO(uint8_t fx_val, uint8_t fx_n);
//*************************************************
//************************* FX END HANDLE
	void endFxFilter(uint8_t fx_n);
	void endFxGlide(uint8_t fx_n);
	void endFxFinetune(uint8_t fx_n);
	void endFxPanning(uint8_t fx_n);
	void endFxReverbSend(uint8_t fx_n);
	void endFxReversePlayback(uint8_t fx_n);
//****** position
	void endFxPosition(uint8_t fx_n);
	void endFxPositionStartPoint(uint8_t fx_n);
	void endFxPositionGranular(uint8_t fx_n);
	void endFxPositionWavetable(uint8_t fx_n);
//******
	void endFxRandomVolume(uint8_t fx_n);
	void endFxVolume(uint8_t fx_n);
	void endFxSlice(uint8_t fx_n);
	void endFxVolumeLFO(uint8_t fx_n);
	void endFxCutoffLFO(uint8_t fx_n);
//****** position lfo
	void endFxPositionLFO(uint8_t fx_n);
	void endFxPositionGranularLFO(uint8_t fx_n);
	void endFxPositionWavetableLFO(uint8_t fx_n);
//******
	void endFxPanningLFO(uint8_t fx_n);
//******FUNKCJE NARZUCAJACE WARTOSCI SILNIKOWI
	void setFxVolume();
	void clearFxVolume();

	void setFxPanning();
	void clearFxPanning();

	void setFxFinetune();
	void clearFxFinetune();

	void setFxGlide();
	void clearFxGlide();

	void setFxReverse();
	void clearFxReverse();

	void setFxPositionGranular();
	void clearFxPositionGranular();

	void setFxPositionWavetable();
	void clearFxPositionWavetable();

	void setFxStartPoint();
	void clearFxStartPoint();

	void setFxSlice();
	void clearFxSlice();

	void setFxReverbSend();
	void clearFxReverbSend();

	void setFxCutoff();
	void clearFxCutoff();
	void setFxFilterType();
	void clearFxFilterType();

	void setFxAmpRateLFO();
	void clearFxAmpRateLFO();
	void syncFxAmpLFO();

	void changeFilterType(uint8_t type);
	void filterConnect();
	void filterDisconnect();

	void changePointsPerformanceMode(int32_t spValue, int32_t epValue);
	void endPointsPerformanceMode();

	void calcLfoBasedEnvelope(envelopeGenerator::strEnv * env, strInstrument::strEnvBasedLfo * lfo, uint8_t rate, uint8_t isAmp);


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
extern AudioAnalyzeRMS			trackRMS[8];
extern AudioMixer9				mixerL,mixerR,mixerReverb;
extern AudioOutputI2S           i2s1;
extern AudioBitDepth			bitDepthControl[2];

//extern AudioFilterStateVariable filterReverbOut;

extern AudioInputI2S            i2sIn;
extern AudioRecordQueue         queue;
extern AudioMixer4              mixerRec;
extern AudioAnalyzeRMS			rms;

extern AudioRecordQueue		 	exportL, exportR;
extern AudioAnalyzeRMS			exportRmsL, exportRmsR;
extern AudioSynthWaveform		testWaveform;

#endif /* SOURCE_MTAUDIOENGINE_H_ */
