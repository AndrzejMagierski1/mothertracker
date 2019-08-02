#ifndef INTERFACE_SAMPLERECORDER_H_
#define INTERFACE_SAMPLERECORDER_H_


#include <modulesBase.h>
#include "SD.h"


#include "mtStructs.h"





const uint8_t sourceCount = 3;
const uint8_t monitorCount = 2;
constexpr uint8_t undoCount = 5;
const char sourcesNamesLabels[sourceCount][10] =
{
		"Line in",
		"Mic",
		"Radio",
};

const char monitorNamesLabels[monitorCount][5] =
{
		"On",
		"Off",
};

struct strRecorderConfig
{
	uint8_t source;
	uint8_t gain;
	uint8_t monitor;
	float radioFreq = 87.0;
};

struct strUndoCrop
{
	int16_t * address;
	uint32_t length;
	uint16_t startPoint;
	uint16_t endPoint;
};

class cSampleRecorder: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSampleRecorder()
	{
//		inActiveInstrumentsCount = 0;
//		inActiveInstrumentIndex = 0;
//		openedInstrFromActive = 0;
//		openedInstrumentIndex = 0;

		 spectrumControl = nullptr;

		 topLabel[8] = {nullptr};
		 bottomLabel[8] = {nullptr};
		 frameControl = nullptr;
		 sourceListControl = nullptr;
		 monitorListControl = nullptr;
		 levelBarControl = nullptr;
		 gainBarControl = nullptr;
		 radioFreqBarControl = nullptr;
		 pointsControl = nullptr;
	}
	virtual ~cSampleRecorder() {}

	void showDefaultScreen();
	void showZoomValue();
	void showSourceList();
	void showMonitorList();

	void showEndPointValue();
	void showStartPointValue();
	void showRecTimeValue();
	void showFreqValue();
	void showRadio();
	void hideRadio();
	void showKeyboard();
	void hideKeyboard();
	void showKeyboardEditName();
	void hideKeyboardEditName();

	void setDefaultScreenFunct();


	void processSpectrum();
	void processPoints();
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
	void changeZoom(int16_t value);
	void changeMonitorSelection(int16_t value);
	void changeSourceSelection(int16_t value);


	void activateLabelsBorder();
	void refreshConfigLists();
	void refreshGain();

	strFrameData frameData;

//*********************************************
	hControl spectrumControl;
	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl frameControl;
	hControl sourceListControl;
	hControl monitorListControl;
	hControl levelBarControl;
	hControl gainBarControl;
	hControl radioFreqBarControl;
	hControl pointsControl;
	hControl keyboardControl;
	hControl editName;
	hControl saveHorizontalBarControl;

//*********************************************


	uint8_t selectedPlace = 0;
	uint8_t currentScreen = 0;
	strRecorderConfig recorderConfig;
	uint32_t spectrumTimerConstrains = 100;
	char freqTextValue[6];
	char recTimeValueText[8];
	char startPointValueText[8];
	char endPointValueText[8];
	float recTimeValue;

	enum sourceType
	{
		sourceTypeLineIn,
		sourceTypeMic,
		sourceTypeRadio
	};

	enum screenType
	{
		screenTypeConfig,
		screenTypeRecord,
		screenTypeKeyboard
	};

//----------------------------------
// spectrum + punkty
	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;



	uint16_t startPoint;
	uint16_t endPoint = MAX_16BIT;

	uint16_t zoomWidth = MAX_16BIT;
	int32_t zoomStart =  0;
	int32_t zoomEnd = MAX_16BIT;
	uint8_t lastChangedPoint = 0;
	float zoomValue = 1;
	char zoomTextValue[6];

	uint16_t zoomPosition = 0;

	strTrackerSpectrum spectrum;
	strTrackerPoints points;

	elapsedMillis refreshSpectrumTimer;

//----------------------------------
// listy
	strList sourceList;
	strList monitorList;

	void listSource();
	void listMonitor();


	char *sourceNames[sourceCount];
	char *monitorNames[monitorCount];


//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;
	int8_t playNote = 24;
	uint8_t glidePreviewDif = 0;

//----------------------------------
//rec

uint8_t recordInProgressFlag = 0;
strUndoCrop undo[undoCount];
uint8_t cropCounter = 0;

//----------------------------------
// bar
	uint8_t radioFreqBarVal;
	uint8_t levelBarVal;
	uint8_t lastLevelBarVal;
	elapsedMicros levelBarTim;
	float measureSum = 0;
	uint8_t levelBarMeasureCounter;
	uint8_t gainBarVal;
	elapsedMillis redColorTim;

	void calcRadioFreqBarVal();
	void calcLevelBarVal();
	void calcGainBarVal();

	void drawRadioFreqBar();
	void drawLevelBar();
	void drawGainBar();

	void changeRadioFreqBar(int16_t val);
	void changeLevelBar();
	void changeGainBar(int16_t val);

//keyboard

	char name[33];
	uint8_t keyboardPosition;
	int8_t editPosition;
	uint8_t keyboardActiveFlag = 0;
	uint8_t keyboardShiftFlag = 0;
//save
	uint8_t saveInProgressFlag = 0;
	uint8_t saveProgress;
	void showSaveHorizontalBar();
	void hideSaveHorizontalBar();

};

extern cSampleRecorder sampleRecorder;



#endif /* INTERFACE_SAMPLERECORDER_H_ */
