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


enum mtPerformanceFxes
{
	mtPerfFxVolume,
	mtPerfPanning,
	mtPerfTune,
	mtPerfLowPass,
	mtPerfHighPass,
	mtPerfBandPass,
	mtPerfReverbSend,
	mtPerfSampleStart,
	mtPerfSamplePlayback,
	mtPerfStepStutter,
	mtPerfPatternPlayMode,
	mtPerfFx11,

	performanceFxesCount
};




const uint8_t trackMasterModeCount = 2;

const char trackMasterLabels[trackMasterModeCount][5] =
{
	"On",
	"Off",
};



const char performanceFxesLabels[performanceFxesCount][20] =
{
	"Volume",
	"Panning",
	"Tune",
	"Low-pass cutoff",
	"High-Pass cutoff",
	"Band-pass cutoff",
	"Reverb sent",
	"Sample start",
	"Sample playback",
	"Step stutter",
	"Pattern play mode",
	"mtPerfFx11",
};

const char performanceStutterLabels[13][20] =
{
	"4",
	"3",
	"2",
	"1",
	"Off",
	"1",
	"1/2",
	"1/3",
	"1/4",
	"1/8",
	"1/16",
	"1/32",
	"1/64",
};


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

	void refreshFxNames(uint8_t place);

	void showPerformaceValue(uint8_t fx);
	void refreshTracksState();
	void refreshTracksPatterns();


//----------------------------------


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
	uint8_t fxPlaces[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
	char fxValuesText[12][7] = {0};

	// f
	int8_t tracksPerformanceState[8] = {0};
	int8_t placePerformanceState[12];

	int8_t fxValues[performanceFxesCount] = {0};
	//uint8_t trackPatern[8] =  {1,1,1,1,1,1,1,1};
	char trackPaternText[8][13];

	uint8_t trackPatternChange[8] = {0};
	uint8_t patternChanging = 0;
};

extern cPerformanceMode performanceMode;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
