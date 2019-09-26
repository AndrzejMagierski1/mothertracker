#ifndef INTERFACE_PERFORMANCEMODE_H_
#define INTERFACE_PERFORMANCEMODE_H_


#include <modulesBase.h>

#include "mtStructs.h"



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


	//----------------------------------
	void showDefaultScreen();

	void showPerformanceMaster();
	void showPerformanceFxes();


	void showPerformaceValue(uint8_t fx);
	void refreshTracksState();

//----------------------------------


	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;


	hControl topLabel[8] = {nullptr};
	hControl bottomLabel[8] = {nullptr};

	hControl textLabel[12] = {nullptr};
	hControl value1Label[12] = {nullptr};




	// typ trybu/ekranu
	uint8_t mode = mtPerformanceFxes;
	uint8_t refreshMaster = 1;



	//--------performance values
	// g
	int8_t fxValues[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	char fxValuesText[12][7] = {0};

	// f
	int8_t tracksPerformanceState[8] = {0};
	int8_t fxPerformanceState[12] = {};



};

extern cPerformanceMode performanceMode;



#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
