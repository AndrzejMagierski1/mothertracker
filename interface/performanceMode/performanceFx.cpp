

#include "mtAudioEngine.h"
#include "mtSequencer.h"

#include "performanceFx.h"

#include "performanceEngine.h"


// INSTRUKCJA DODAWANIA FXa
// 1. Dodaj odpowiednią pozycje enuma w odpowiednim jego miejscu
// 2. Powiel blok tworzący obiekt dowolnego dzialającego juz fxa
// 3. W nowo utworzonym:
//	  - Pozmieniaj nazwy zmiennych/funkcji na odpowiednie
// 	  - Przypisz prawidlowe funkcje i parametry struktury
//    - przypisz wartosc enuma i nazwe do wyswietlania w interfejsie


// enum okreslajacy kolejnosic i ilosc efektow oraz niepowtarzalne id efektu
enum
{
	mtPerfFxNone,
	//------------------------- \/ ---
	mtPerfFxVolume,
	mtPerfPanning,
	mtPerfTune,
	mtPerfLowPass,
	mtPerfHighPass,
	mtPerfBandPass,
	mtPerfDelaySend,
	mtPerfSamplePosition,
	mtPerfSampleEnd,
	mtPerfSamplePlayback,
	mtPerfVolumeLfoSpeed,
	mtPerfPanningLfoSpeed,
	mtPerfFinetuneLfoSpeed,
	mtPerfFilterfoSpeed,
	mtPerfGranWtfoSpeed,
	mtPerfStepStutter,
	mtPerfPatternPlayMode,
	mtPerfPatternLength,


	//--- dodwac efekty powyzej /\ ---
	performanceFxesCount
};

cPerformEngine performance(performanceFxesCount, performanceDataSource, performanceDataSourceCount);

//##################################################################################
//##################################################################################
//########## NONE ########################################################################
void noneSet(uint8_t track, int16_t value) {  }
void noneClear(uint8_t track) {  }
const fxSetup noneSetup = {-100, 100, nullptr};

cPerformFx nonePerfFx(mtPerfFxNone, "", noneSet, noneClear, &noneSetup);

//########## VOLUME ########################################################################
void volumeSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeVolumePerformanceMode(value); }
void volumeClear(uint8_t track) { instrumentPlayer[track].endVolumePerformanceMode(); }
const fxSetup volumeSetup = {-100, 100, nullptr};

cPerformFx volumePerfFx(mtPerfFxVolume, "Volume", volumeSet, volumeClear, &volumeSetup);

//########## PANNING ########################################################################
void panningSet(uint8_t track, int16_t value) { instrumentPlayer[track].changePanningPerformanceMode(value); }
void panningClear(uint8_t track) { instrumentPlayer[track].endPanningPerformanceMode(); }
const fxSetup panningSetup = {-100, 100, nullptr};

cPerformFx panningPerfFx(mtPerfPanning, "Panning", panningSet, panningClear, &panningSetup);

//########## TUNE ########################################################################
void tuneSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeTunePerformanceMode(value); }
void tuneClear(uint8_t track) { instrumentPlayer[track].endTunePerformanceMode(); }
const fxSetup tuneSetup = {-48, 48, nullptr};

cPerformFx tunePerfFx(mtPerfTune, "Tune", tuneSet, tuneClear, &tuneSetup);

//########## FILTER ########################################################################
void lowPassSet(uint8_t track, int16_t value)  { instrumentPlayer[track].changeFilterTypePerformanceMode(1); instrumentPlayer[track].changeCutoffPerformanceMode(value); }
void highPassSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeFilterTypePerformanceMode(2); instrumentPlayer[track].changeCutoffPerformanceMode(value); }
void bandPassSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeFilterTypePerformanceMode(3); instrumentPlayer[track].changeCutoffPerformanceMode(value); }
void filterClear(uint8_t track) { instrumentPlayer[track].endCutoffPerformanceMode(); instrumentPlayer[track].endFilterTypePerformanceMode();}
const fxSetup lowPassSetup =  {-100, 100, nullptr, 0, mtPerfHighPass, mtPerfBandPass};
const fxSetup highPassSetup = {-100, 100, nullptr, 0, mtPerfLowPass,  mtPerfBandPass};
const fxSetup bandPassSetup = {-100, 100, nullptr, 0, mtPerfLowPass,  mtPerfHighPass};

cPerformFx lowPassPerfFx(mtPerfLowPass, "Low-pass cutoff", lowPassSet, filterClear, &lowPassSetup);
cPerformFx highPassPerfFx(mtPerfHighPass, "High-Pass cutoff", highPassSet, filterClear, &highPassSetup);
cPerformFx bandPassPerfFx(mtPerfBandPass, "Band-pass cutoff", bandPassSet, filterClear, &bandPassSetup);

//########## DELAY ########################################################################
void delaySet(uint8_t track, int16_t value) { instrumentPlayer[track].changeReverbSendPerformanceMode(value); }
void delayClear(uint8_t track) { instrumentPlayer[track].endReverbSendPerformanceMode(); }
const fxSetup delaySetup = {-100, 100, nullptr};

cPerformFx delayPerfFx(mtPerfDelaySend, "Delay send", delaySet, delayClear, &delaySetup);

//########## SAMPLE POS ########################################################################
void samplePosSet(uint8_t track, int16_t value) { instrumentPlayer[track].changePositionPerformanceMode(value); }
void samplePosClear(uint8_t track) { instrumentPlayer[track].endStartPointPerformanceMode(); }
const fxSetup samplePosSetup = {-100, 100, nullptr};

cPerformFx samplePosPerfFx(mtPerfSamplePosition, "Sample position", samplePosSet, samplePosClear, &samplePosSetup);

//########## SAMPLE END ########################################################################
void sampleEndSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeEndPointPerformanceMode(value); }
void sampleEndClear(uint8_t track) { instrumentPlayer[track].endEndPointPerformanceMode(); }
const fxSetup sampleEndSetup = {-100, 100, nullptr};

cPerformFx sampleEndPerfFx(mtPerfSampleEnd, "Sample end", panningSet, panningClear, &panningSetup);

//########## SAMPLE PLAYBACK ########################################################################
void samplePlaybackSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeSamplePlaybackPerformanceMode(value); }
void samplePlaybackClear(uint8_t track) { instrumentPlayer[track].endSamplePlaybackPerformanceMode(); }
const char samplePlaybackPerfLabels[2][4] = { ">>>", "<<<" };
const fxSetup samplePlaybackSetup = {0, 1, &samplePlaybackPerfLabels[0][0], 4};

cPerformFx samplePlaybackPerfFx(mtPerfSamplePlayback, "Sample playback", samplePlaybackSet, samplePlaybackClear, &samplePlaybackSetup);

//########## VOLUME LFO SPEED ########################################################################
void volumeLfoSpeedSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeAmpLfoRatePerformanceMode(value); }
void volumeLfoSpeedClear(uint8_t track) { instrumentPlayer[track].endAmpLfoRatePerformanceMode(); }
const fxSetup volumeLfoSpeedSetup = {-20, 20, nullptr};

cPerformFx volumeLfoSpeedPerfFx(mtPerfVolumeLfoSpeed, "Volume LFO Speed", volumeLfoSpeedSet, volumeLfoSpeedClear, &volumeLfoSpeedSetup);

//########## PANNING LFO SPEED ########################################################################
void panningLfoSpeedSet(uint8_t track, int16_t value) { instrumentPlayer[track].changePanningLfoRatePerformanceMode(value); }
void panningLfoSpeedClear(uint8_t track) { instrumentPlayer[track].endPanningLfoRatePerformanceMode(); }
const fxSetup panningLfoSpeedSetup = {-20, 20, nullptr};

cPerformFx panningLfoSpeedPerfFx(mtPerfPanningLfoSpeed, "Panning LFO Speed", panningLfoSpeedSet, panningLfoSpeedClear, &panningLfoSpeedSetup);

//########## PITCH LFO SPEED ########################################################################
void finetuneLfoSpeedSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeFinetuneLfoRatePerformanceMode(value); }
void finetuneLfoSpeedClear(uint8_t track) { instrumentPlayer[track].endFinetuneLfoRatePerformanceMode(); }
const fxSetup finetuneLfoSpeedSetup = {-20, 20, nullptr};

cPerformFx finetuneLfoSpeedPerfFx(mtPerfFinetuneLfoSpeed, "Finetune LFO Speed", finetuneLfoSpeedSet, finetuneLfoSpeedClear, &finetuneLfoSpeedSetup);

//########## FILTER LFO SPEED ########################################################################
void filterLfoSpeedSet(uint8_t track, int16_t value) { instrumentPlayer[track].changeCutoffLfoRatePerformanceMode(value); }
void filterLfoSpeedClear(uint8_t track) { instrumentPlayer[track].endCutoffLfoRatePerformanceMode(); }
const fxSetup filterLfoSpeedSetup = {-20, 20, nullptr};

cPerformFx filterLfoSpeedPerfFx(mtPerfFilterfoSpeed, "Filter LFO Speed", filterLfoSpeedSet, filterLfoSpeedClear, &filterLfoSpeedSetup);

//########## GRAN/WT LFO SPEED ########################################################################
void granWtLfoSpeedSet(uint8_t track, int16_t value) { instrumentPlayer[track].changePositionLfoRatePerformanceMode(value); }
void granWtLfoSpeedClear(uint8_t track) { instrumentPlayer[track].endPositionLfoRatePerformanceMode(); }
const fxSetup granWtLfoSpeedSetup = {-20, 20, nullptr};

cPerformFx granWtLfoSpeedPerfFx(mtPerfGranWtfoSpeed, "Gran/Wt LFO Speed", granWtLfoSpeedSet, granWtLfoSpeedClear, &granWtLfoSpeedSetup);

//########## STEP REPEATER ########################################################################
void stepRepeaterSet(uint8_t track, int16_t value) { sequencer.setPerformanceStutter(track, value); }
void stepRepeaterClear(uint8_t track) { sequencer.setPerformanceStutter(track, 0); }
const char stutterPerfLabels[16][5] = {	"Off",	"16",	"12",	"8",	"6",	"4",	"3",	"2",	"1",	"1/2",	"1/3",	"1/4",	"1/6",	"1/8",	"1/12", "1/16",};
const fxSetup stepRepeaterSetup = {Sequencer::strFxConsts::STUTTER_PERIOD_MIN, Sequencer::strFxConsts::STUTTER_PERIOD_MAX, &stutterPerfLabels[0][0], 5};

cPerformFx stepRepeaterPerfFx(mtPerfStepStutter, "Step repeater", stepRepeaterSet, stepRepeaterClear, &stepRepeaterSetup);

//########## PATTERN PLAY MODE ########################################################################
void patternPlayModeSet(uint8_t track, int16_t value) { sequencer.setPerformancePlayMode(track, value); }
void patternPlayModeClear(uint8_t track) { sequencer.setPerformancePlayMode(track, 0); sequencer.alignToGlobalPos(track); }
const char patternPlayModePerfLabels[3][5] = { "Fwd", "Back", "Rnd", };
const fxSetup patternPlayModeSetup = {0, 2, &patternPlayModePerfLabels[0][0], 5};

cPerformFx patternPlayModePerfFx(mtPerfPatternPlayMode, "Pattern play mode", patternPlayModeSet, patternPlayModeClear, &patternPlayModeSetup);

//########## PATTERN LENGTH ########################################################################
void patternLengthSet(uint8_t track, int16_t value) { sequencer.setPerformancePatternLengthFromFxVal(value); }
void patternLengthClear(uint8_t track) { sequencer.setPerformancePatternLengthFromFxVal(0); }
const char patternLengthPerfLabels[9][4] = {"---", "1", "2", "4", "8", "16", "32", "64", "128" };
const fxSetup patternLengthSetup = {0, 8, &patternLengthPerfLabels[0][0], 4};

cPerformFx patternLengthPerfFx(mtPerfPatternLength, "Pattern length", patternLengthSet, patternLengthClear, &patternLengthSetup);

//##########  ########################################################################

//##########  ########################################################################
