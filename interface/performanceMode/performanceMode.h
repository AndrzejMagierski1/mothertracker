#ifndef INTERFACE_PERFORMANCEMODE_H_
#define INTERFACE_PERFORMANCEMODE_H_


#include "core/modulesBase.h"

#include "mtStructs.h"

#include "mtAudioEngine.h"

enum mtPerformanceMode
{
	mtPerformanceMaster,
	mtPerformanceFxes,

};


//------------------------------------------------------------------------------

const uint8_t trackMasterModeCount = 2;


const char trackMasterLabels[trackMasterModeCount][5] =
{
	"On",
	"Off",
};




//const char * const performanceLfoSpeedLabels[20] =
//{
//		"6",
//		"4",
//		"3",
//		"2",
//		"3/2",
//		"1",
//		"3/4",
//		"1/2",
//		"3/8",
//		"1/3",
//		"1/4",
//		"316",
//		"1/6",
//		"1/8",
//		"/12",
//		"/16",
//		"/24",
//		"/32",
//		"/48",
//		"/64"
//};

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
	void refreshPerformanceValuesForTrack(uint8_t track, uint8_t place);
	void clearOtherFilterTypes(uint8_t  track, uint8_t choosenType);

	uint8_t wasPatternOntrackChenged(uint8_t track);

	void toggleEditState();
	void setProjectSaveFlags();
	//----------------------------------
	void showDefaultScreen();

	void showPerformanceMaster();
	void showPerformanceFxes();

	void showEditFrame(uint8_t place);
	void hideEditFrame();

	void showPerformaceValue(uint8_t place);
	void showTracksState();
	void showTracksPatterns();
	void showFxNames(uint8_t place);
	//----------------------------------

	void setPlaceNewFx(uint8_t place, uint8_t newFx);
	uint8_t fxAlredyTaken(uint8_t fx);

//----------------------------------

	strMultiLabelData multiLabelData[12];
	strFrameData frameData;

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl bgLabel = nullptr;
	hControl label[8] = {nullptr};
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

	uint8_t dontClearPerformanceMode = 0;


	//--------performance values
	// gui
	char fxValuesText[12][4][7] = {0};
	char trackPaternText[8][12]; // do labeli przyciskow pod ekranem

	// funct
	uint8_t trackPatternChange[8] = {0,0,0,0,0,0,0,0}; // zarzazdanie pattern-trackami

	// blinkowanie na zmienianycnm tracku
	void blinkTrackUntilSwitch();
	void colorTracksLabel(uint8_t track, uint8_t state);

	uint8_t refreshBlinkingTrack = 0;
	uint8_t blinkInterval = 0;
	uint8_t blinkState = 0;



};

extern cPerformanceMode performanceMode;


//#define FX_VALUE(x) PM->fxValues[x][PM->activeFxValues[x]]
//#define FX_VALUE(x) PM->placesTempValues[x]


#endif /* INTERFACE_INSTRUMENTEDITOR_H_ */
