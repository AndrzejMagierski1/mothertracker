
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

const float tempoSyncRatesAmp[ENVELOPE_SPEED_AMP_COUNT] =
{
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

const float tempoSyncRatesOthers[ENVELOPE_SPEED_OTHER_COUNT] =
{
	128,
	96,
	64,
	48,
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
		0.01047128548,
		0.01096478196,
		0.01148153621,
		0.01202264435,
		0.01258925412,
		0.01318256739,
		0.01380384265,
		0.01445439771,
		0.01513561248,
		0.01584893192,
		0.01659586907,
		0.01737800829,
		0.01819700859,
		0.01905460718,
		0.01995262315,
		0.02089296131,
		0.02187761624,
		0.02290867653,
		0.02398832919,
		0.02511886432,
		0.02630267992,
		0.02754228703,
		0.02884031503,
		0.0301995172,
		0.0316227766,
		0.03311311215,
		0.03467368505,
		0.03630780548,
		0.03801893963,
		0.03981071706,
		0.04168693835,
		0.04365158322,
		0.04570881896,
		0.04786300923,
		0.05011872336,
		0.05248074602,
		0.05495408739,
		0.05754399373,
		0.06025595861,
		0.06309573445,
		0.0660693448,
		0.06918309709,
		0.07244359601,
		0.0758577575,
		0.07943282347,
		0.08317637711,
		0.087096359,
		0.09120108394,
		0.0954992586,
		0.1,
		0.1047128548,
		0.1096478196,
		0.1148153621,
		0.1202264435,
		0.1258925412,
		0.1318256739,
		0.1380384265,
		0.1445439771,
		0.1513561248,
		0.1584893192,
		0.1659586907,
		0.1737800829,
		0.1819700859,
		0.1905460718,
		0.1995262315,
		0.2089296131,
		0.2187761624,
		0.2290867653,
		0.2398832919,
		0.2511886432,
		0.2630267992,
		0.2754228703,
		0.2884031503,
		0.301995172,
		0.316227766,
		0.3311311215,
		0.3467368505,
		0.3630780548,
		0.3801893963,
		0.3981071706,
		0.4168693835,
		0.4365158322,
		0.4570881896,
		0.4786300923,
		0.5011872336,
		0.5248074602,
		0.5495408739,
		0.5754399373,
		0.6025595861,
		0.6309573445,
		0.660693448,
		0.6918309709,
		0.7244359601,
		0.758577575,
		0.7943282347,
		0.8317637711,
		0.87096359,
		0.9120108394,
		0.954992586,
		1
};
constexpr uint32_t NOT_MOD_POINTS = 1000000;
constexpr uint8_t MUTE_DISABLE = 0;
constexpr uint8_t MUTE_ENABLE = 1;
constexpr float AMP_MUTED = 0.0f;
constexpr uint8_t MOST_SIGNIFICANT_FX = 0;
constexpr uint8_t LEAST_SIGNIFICANT_FX = 1;
extern IntervalTimer updateTimer;
constexpr uint8_t ACTIVE_ENVELOPES = envMax;

class audioEngine
{
public:
	void init();
	void update();
	void prevSdConnect();
	void prevSdDisconnect();
	void refreshTrackVolume();
	void refreshReverbVolume();
	void refreshDelayVolume();
	void setHeadphonesVolume(uint8_t value);
	void setDelayFeedback(uint8_t value);
	void setDelayTime(uint16_t value);
	void setDelayPanning(int8_t value);
	void setReverbSize(float value);
	void setReverbDamp(float value);
	void setReverbPredelay(float value);
	void setReverbDiffusion(float value);
	void setDelayParams(uint8_t value);
	void setLimiterAttack(uint16_t attack);
	void setLimiterRelease(float release);
	void setLimiterTreshold(uint16_t threshold);
	void setBitDepth(uint16_t bitDepth);
	void muteTrack(uint8_t channel, uint8_t state);
	void soloTrack(uint8_t channel, uint8_t state);
	void muteDelaySend(uint8_t channel, uint8_t state);
	void muteReverbSend(uint8_t channel, uint8_t state);
	void soloDelaySend(uint8_t state);
	void soloReverbSend(uint8_t state);
	void clearDelay();
	void clearReverb();
	void performanceModeEndAll();
	void startTestSignal(float amp, float freq);
	void stopTestSignal();
	void endAllFx();
	void setPassEnvelope(uint8_t state);
	void blockDelayRefresh();
	void unblockDelayRefresh();
	void setLastUsedVoice(uint8_t v);
	uint8_t getLastUsedVoice();
	void makeMetronomeTick(uint8_t);
	void setCurrentLoadInstrument(int8_t idx);
	void clearCurrentLoadInstrument(int8_t idx);
	void setInterpolationEnable(bool value);
	float getDelayAverageRMS();
	float getReverbAverageRMS();
	float getDryMixAverageRMS();
	float getDelayRMS(uint8_t channel);
	float getReverbRMS(uint8_t channel);
	float getDryMixRMS(uint8_t channel);


	friend class playerEngine;
private:
	uint8_t forceDelaySend;
	uint8_t forceReverbSend;
	uint16_t currentTempo;
	uint16_t lastTempo;
	uint8_t lastUsedVoice;
};


class playerEngine
{
public:

	playerEngine();

	uint8_t noteOn(uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx1_id, uint8_t fx1_val, uint8_t fx2_id, uint8_t fx2_val); // tylko ten note on do nut w sequencerze
	void noteOff(int8_t option = -4);
	void clean();

	void setInterpolationEnable(bool value);

	void seqFx(uint8_t fx_id, uint8_t fx_val, uint8_t fx_n);
	void endFx(uint8_t fx_id, uint8_t fx_n);
	void slide(int8_t note, uint16_t time);
	void modPitch(float value);
	void modGlide(uint16_t value);
	void modFineTune(int8_t value);

	void modPanning(int16_t value);
	void modVolume(float value);
	void modSP(uint16_t value);
	void modLP1(uint16_t value);
	void modLP2(uint16_t value);
	void modCutoff(float value);
	void modResonance(float value);

	void modSeqPoints(uint32_t sp, uint32_t ep, uint8_t fx_n);

	void modWavetableWindow(uint16_t value);
	void modTune(int8_t value);
	void modDelaySend(uint8_t value);
	void modReverbSend(uint8_t value);

	void modGranularPosition(uint16_t value);
	void modGranularGrainLength();

	void setStatusBytes(uint32_t value);
//	void resetMods();
	uint8_t getInterfaceEndReleaseFlag();
	void clearInterfaceEndReleaseFlag();
	uint8_t getInterfacePlayingEndFlag();
	void clearInterfacePlayingEndFlag();

	void setTrackIsDisplayed();
	void clearTrackIsDisplayed();
	bool getEndDisplayTrackFlag();
	void clearEndDisplayTrackFlag();


	uint32_t getEnvelopeGranPosMod();

	uint32_t getEnvelopeWtPosMod();

	uint16_t getWavePosition();
	void update();
	uint8_t noteOnforPrev (uint8_t instr_idx,int8_t note, int8_t velocity);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t type);
	uint8_t noteOnforPrev (int16_t * addr, uint32_t len, uint8_t note,uint8_t type);

	uint8_t currentInstrument_idx;

	const uint32_t envTargetRefreshMask[envMax] =
	{
			0,
			PANNING_MASK,
			CUTOFF_MASK,
			WT_POS_SEND_MASK,
			GRANULAR_POS_SEND_MASK,
			FINETUNE_MASK
	};
	const uint8_t envelopesControlValue[envMax] =
	{
			(uint8_t)parameterList::lfoAmp,
			(uint8_t)parameterList::lfoPanning,
			(uint8_t)parameterList::lfoCutoff,
			(uint8_t)parameterList::lfoWavetablePosition,
			(uint8_t)parameterList::lfoGranularPosition,
			(uint8_t)parameterList::lfoFinetune,
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
		delaySend,
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
		lfoFinetune,
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
		uint8_t delaySend;
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

		uint8_t lfoFinetuneEnable;
		uint8_t lfoFinetuneRate;

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
		uint8_t delaySend;
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

		uint8_t lfoFinetuneEnable;
		uint8_t lfoFinetuneRate;

	} currentPerformanceValues;


	struct strPerformanceMod
	{
		int8_t 	volume;
		int8_t 	panning;
		int8_t 	tune;
		int8_t 	delaySend;
		int8_t  reverbSend;
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
		int8_t  lfoFinetuneRate;
	} performanceMod;

	float currentEnvelopeModification[envMax];

	uint8_t trackControlParameter[(int)controlType::length][(int)parameterList::length];
//**********************************************************************************************************************************
//PERFORMANCE MODE
//*****************************CHANGE
	void changeVolumePerformanceMode(int8_t value);
	void changePanningPerformanceMode(int8_t value);
	void changeTunePerformanceMode(int8_t value);
	void changeDelaySendPerformanceMode(int8_t value);
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
	void changeFinetuneLfoRatePerformanceMode(int8_t value);

//*****************************END
	void endVolumePerformanceMode();
	void endPanningPerformanceMode();
	void endTunePerformanceMode();
	void endDelaySendPerformanceMode();
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
	void endFinetuneLfoRatePerformanceMode();


	void setPassEnvelope(uint8_t state);
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
	uint8_t						onlyReverbMuteState = 0;

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
			.enable		= 0
	};

	uint8_t isActiveFlag = 0;

	bool isTrackDisplayed;
	bool onEndDisplay;


//**********************FUNKCJE POMOCNICZE
	uint8_t isFxVelocity(uint8_t fx_id);
	float getMostSignificantAmount();
	uint8_t getMostSignificantVolume();
	void initEnvelopesParamiters(uint8_t n, envelopeGenerator::strEnv * env);
	void setSyncParamsLFO(uint8_t type);
	void setSyncParamsAmpLFO();
	bool isActiveEnvelope(uint8_t type);

//**********************NOTE ON/OFF HANDLERS
//*****note on fx
	void handleInitFxNoteOnEnvelope(uint8_t n);
	void handleInitFxNoteOnAmpEnvelope();
	void handleInitFxNoteOnAllEnvelopes(); // wywolywany w note on fx
	void handleInitFxNoteOnFilter();
	void handleFxNoteOnFilter();
	void handleFxNoteOnGain();
	void handleFxNoteOnPanning();
	void handleFxNoteOnDelaySend();
	void handleFxNoteOnReverbSend();
//*****note on
	void handleInitNoteOnEnvelope(uint8_t n);
	void handleInitNoteOnAmpEnvelope();
	void handleInitNoteOnAllEnvelopes(); // wywolywany w note on
	void handleNoteOnFilter();
	void handleNoteOnGain();
	void handleNoteOnPanning();
	void handleNoteOnDelaySend();
	void handleNoteOnReverbSend();
//*****note off
	void noteOffFade();
	void noteOffCut();
	void noteOffOrdinary();
//*************************************************
//**********************UPDATE HANDLERS
	inline void handleUpdateEndPlayDetect();
	inline void handleUpdateEnvelope(uint8_t type, bool enableCondition);
	inline void handleUpdateEndReleaseAction();
	inline void handleUpdateRefreshLP1();
	inline void handleUpdateRefreshLP2();
	inline void handleUpdateRefreshFinetune();
	inline void handleUpdateRefreshTune();
	inline void handleUpdateRefreshVolume();
	inline void handleUpdateRefreshPanning();
	inline void handleUpdateRefreshCutoff();
	inline void handleUpdateRefreshResonance();
	inline void handleUpdateRefreshDelay();
	inline void handleUpdateRefreshReverb();
	inline void handleUpdateRefreshWtPos();
	inline void handleUpdateRefreshGranPos();
	inline void handleUpdateRefreshGranLen();
	inline void handleUpdateRefreshGranWave();
	inline void handleUpdateRefreshGranLoop();
	inline void handleUpdateRefreshAmpLFO();
	inline void handleUpdateRefreshCutoffLFO();
	inline void handleUpdateRefreshWtPosLFO();
	inline void handleUpdateRefreshGranPosLFO();
	inline void handleUpdateRefreshPanningLFO();
	inline void handleUpdateRefreshFinetuneLFO();

//*************************************************
//************************* FX HANDLE
//******* fx start
	void fxFilter(uint8_t fx_val, uint8_t fx_n, uint8_t type);
	void fxGlide(uint8_t fx_val, uint8_t fx_n);
	void fxFinetune(uint8_t fx_val, uint8_t fx_n);
	void fxPanning(uint8_t fx_val, uint8_t fx_n);
	void fxDelaySend(uint8_t fx_val, uint8_t fx_n);
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
//***
	void fxFinetuneLFO(uint8_t fx_val, uint8_t fx_n);
//*************************************************
//************************* FX END HANDLE
	void endFxFilter(uint8_t fx_n);
	void endFxGlide(uint8_t fx_n);
	void endFxFinetune(uint8_t fx_n);
	void endFxPanning(uint8_t fx_n);
	void endFxDelaySend(uint8_t fx_n);
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
//******
	void endFxFinetuneLFO(uint8_t fx_n);
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

	void setFxStartPoint(uint8_t fx_n);
	void clearFxStartPoint();

	void setFxSlice();
	void clearFxSlice();

	void setFxDelaySend();
	void clearFxDelaySend();

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
extern envelopeGenerator		envelopeFilter[8];
extern AudioFilterStateVariable filter[8];
extern AudioAmplifier           amp[8];
extern AudioAnalyzeRMS			trackRMS[8];
extern AudioMixer10				mixerL,mixerR,mixerDelay;
extern AudioOutputI2S           i2s1;
extern AudioBitDepth			bitDepthControl[2];

//extern AudioFilterStateVariable filterReverbOut;

extern AudioInputI2S            i2sIn;
extern AudioRecordQueue         queue;
extern AudioMixer4              mixerRec;
extern AudioAnalyzeRMS			inputRMS;

extern AudioRecordQueue		 	exportL, exportR;
extern AudioAnalyzeRMS			exportRmsL, exportRmsR;
extern AudioSynthWaveform		testWaveform;
extern AudioEffectPolyverb		polyverb;
extern AudioEffectShortDelay	shortDelay;

extern uint8_t isCurrentLoadInstrument[48];

#endif /* SOURCE_MTAUDIOENGINE_H_ */
