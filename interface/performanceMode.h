#ifndef INTERFACE_PERFORMANCEMODE_H_
#define INTERFACE_PERFORMANCEMODE_H_


#include <modulesBase.h>

#include "mtStructs.h"

#include "mtAudioEngine.h"


enum mtPerformanceMode
{
	mtPerformanceMaster,
	mtPerformanceFxes,

};

//------------------------------------------------------------------------------

enum mtPerformanceFxes
{
	mtPerfFxNone,
	mtPerfFxVolume,
	mtPerfPanning,
	mtPerfTune,
	mtPerfLowPass,
	mtPerfHighPass,
	mtPerfBandPass,
	mtPerfReverbSend,
	mtPerfSampleStart,
	mtPerfSampleEnd,
	mtPerfSamplePlayback,
	mtPerfWavetablePos,
	mtPerfStepStutter,
	mtPerfPatternPlayMode,
	mtPerfPatternLength,

	performanceFxesCount
};


const char performanceFxesLabels[performanceFxesCount][20] =
{
	"",
	"Volume",
	"Panning",
	"Tune",
	"Low-pass cutoff",
	"High-Pass cutoff",
	"Band-pass cutoff",
	"Reverb sent",
	"Sample start",
	"Sample end",
	"Sample playback",
	"Wavetable pos",
	"Step stutter",
	"Pattern play mode",
	"Pattern length",
};

//------------------------------------------------------------------------------

const uint8_t trackMasterModeCount = 2;
const char trackMasterLabels[trackMasterModeCount][5] =
{
	"On",
	"Off",
};

const char performanceStutterLabels[12][20] =
{
	"Off",
	"4",
	"3",
	"2",
	"1",
	"1/2",
	"1/3",
	"1/4",
	"1/6",
	"1/8",
	"1/12",
	"1/16",
};

const uint8_t performancePatternLengthValues[] =
{ 1, 2, 4, 8, 16, 32, 64, 128 };


class cPerformanceMode: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cPerformanceMode()
	{

	}
	~cPerformanceMode() {}


	//----------------------------------
	void setDefaultScreenFunct();

	void setPerformanceMaster();
	void setPerformanceFxes();
	void lightUpPadBoard();

	void toggleTrackPerformanceState(uint8_t track);
	void clearPerformanceValues(uint8_t track, uint8_t fx);
	void refreshPerformanceValuesForTrack(uint8_t track);

	uint8_t wasPatternOntrackChenged(uint8_t track);

	void toggleEditState();

	//----------------------------------
	void showDefaultScreen();

	void showPerformanceMaster();
	void showPerformanceFxes();

	void showEditFrame(uint8_t place);
	void hideEditFrame();

	//----------------------------------
	void refreshFxNames(uint8_t place);

	//
	//void changePerformanceData(int16_t value);


	uint8_t getFxFromPlace(uint8_t place);

	void refreshActiveValueForFx(uint8_t fx);
	void showPerformaceValue(uint8_t fx);

	void refreshTracksState();
	void refreshTracksPatterns();


	void savePlaceFxValue(uint8_t place, uint8_t slot, int16_t value);
	void clearHidedFx(uint8_t fx);
	uint8_t fxAlredyTaken(uint8_t fx);
//----------------------------------


	void showArrow(uint8_t place, uint8_t type);
	void hideArrow(uint8_t place);

	//strLabelData textLabelData[12];
	strMultiLabelData multiLabelData[12];

	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;


	hControl topLabel[8] = {nullptr};
	hControl bottomLabel[8] = {nullptr};

	hControl textLabel[12] = {nullptr};
	hControl value1Label[12] = {nullptr};

	hControl frameControl = nullptr;


	// typ trybu/ekranu
	uint8_t mode = mtPerformanceFxes;
	uint8_t refreshMaster = 1;
	uint8_t refreshTrackState = 1;
	uint8_t refreshTrackPattern = 1;

	uint8_t performanceEditState = 0;
	uint8_t performanceEditPlace = 0;

	//--------performance values
	// g
	//uint8_t fxPlaces[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
	char fxValuesText[12][4][7] = {0};

	// f
	int8_t tracksPerformanceState[8] = {0};
	int8_t placePerformanceState[12];

	int16_t fxValues[performanceFxesCount][4] = {0};
	uint8_t activeFxValues[performanceFxesCount] = {0};
	int16_t fxTempValues[performanceFxesCount] = {0};

	//uint8_t trackPatern[8] =  {1,1,1,1,1,1,1,1};
	char trackPaternText[8][13];

	uint8_t trackPatternChange[8] = {0};
};

extern cPerformanceMode performanceMode;


//#define FX_VALUE(x) PM->fxValues[x][PM->activeFxValues[x]]
#define FX_VALUE(x) PM->fxTempValues[x]


#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
