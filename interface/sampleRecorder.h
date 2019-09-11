#ifndef INTERFACE_SAMPLERECORDER_H_
#define INTERFACE_SAMPLERECORDER_H_


#include <modulesBase.h>
#include "SD.h"


#include "mtStructs.h"

typedef enum
{
	makeBigger,
	makeSmaller,

}resize_style_t;

typedef struct
{
	resize_style_t resizeStyle[8];
	uint8_t buttonsToResize;

}button_resize_t;



const uint8_t sourceCount = 4;
const uint8_t monitorCount = 2;
constexpr uint8_t undoCount = 5;
const char sourcesNamesLabels[sourceCount][10] =
{
		"Line in",
		"Radio",
		"Mic LG",
		"Mic HG"
};

const char monitorNamesLabels[monitorCount][5] =
{
		"On",
		"Off",
};

struct strRecorderConfig
{
	uint8_t source;
	uint8_t gainLineIn;
	uint8_t gainMicLow;
	uint8_t gainMicHigh;
	uint8_t gainRadio;
	uint8_t monitor;
	float radioFreq = 87.5;
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

		 radioRdsLabel = nullptr;
		 spectrumControl = nullptr;
		 progressCursor = nullptr;
		 topLabel[8] = {nullptr};
		 bottomLabel[8] = {nullptr};
		 frameControl = nullptr;
		 sourceListControl = nullptr;
		 monitorListControl = nullptr;
		 levelBarControl = nullptr;
		 gainBarControl = nullptr;
		 radioFreqBarControl = nullptr;
		 pointsControl = nullptr;
		 keyboardControl = nullptr;
		 editName = nullptr;
		 saveHorizontalBarControl = nullptr;
		 selectWindowLabel = nullptr;

	}
	virtual ~cSampleRecorder() {}

	void showDefaultScreen();
	void showZoomValue();
	void showSourceList();
	void showMonitorList();

	void showEndPointValue();
	void showStartPointValue();
	void showRecTimeValue();
	void showPreviewValue();
	void hidePreviewValue();
	void showFreqValue();
	void showRadio();
	void hideRadio();
	void showKeyboard();
	void hideKeyboard();
	void showKeyboardEditName();
	void hideKeyboardEditName();

	void setDefaultScreenFunct();


	void processSpectrum1();
	void processPoints();
	void modStartPoint(int16_t value);
	void modEndPoint(int16_t value);
	void changeZoom(int16_t value);
	void changeMonitorSelection(int16_t value);
	void changeSourceSelection(int16_t value);


	void activateLabelsBorder();
	void refreshConfigLists();
	void refreshGain();

	void setPrevievFlag(uint8_t s);

	void refreshRDS();
	void displaySeeking();
	void displayEmptyRDS();
	void hideRDS();

	strFrameData frameData;
	strSelectWindowData selectWindowData;

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
	hControl selectWindowLabel;
	hControl progressCursor;
	hControl radioRdsLabel;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl notePopoutControl = nullptr;

//*********************************************


	uint8_t selectedPlace = 0;
	uint8_t currentScreen = 0;
	strRecorderConfig recorderConfig;
	uint32_t spectrumTimerConstrains = 100;
	char freqTextValue[6];
	char recTimeValueText[8];
	char playTimeValueText[8];
	char startPointValueText[8];
	char endPointValueText[8];
	float recTimeValue;

	enum sourceType
	{
		sourceTypeLineIn,
		sourceTypeRadio,
		sourceTypeMicLG,
		sourceTypeMicHG,

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
	uint8_t refreshSpectrumValue = 0;
	uint8_t refreshPoints = 0;



	uint16_t startPoint;
	uint16_t endPoint = MAX_16BIT;

	uint32_t playProgressValue = 0; // 0 - MAX_LEN_RECORD
	uint16_t playProgressInSpectrum = 0; // 0 - 600
	elapsedMillis playProgresValueTim;
	elapsedMicros refreshPlayProgressValue;

	uint8_t playInProgressFlag = 0;

	void calcPlayProgressValue();

	strZoomParams zoom;
	strSpectrumParams params;

	char zoomTextValue[6];


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

	elapsedMillis changeSourceToMicTimer = 0;
	uint8_t changeSourceToMicFlag = 0;

//----------------------------------
//rec

uint8_t recordInProgressFlag = 0;
uint8_t firstPeakFlag = 0;
uint32_t firstPeakPlace;
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
	uint8_t lastPressedPad;


//save
	uint8_t saveInProgressFlag = 0;
	uint8_t saveLoadFlag = 0;
	uint8_t saveProgress;
	uint8_t forceSwitchModule = 0;
	void showSaveHorizontalBar();
	void hideSaveHorizontalBar();
// selection window
	void showSelectionWindow();
	void showSelectionNotEnoughInstruments();
	uint8_t notEnoughInstrumentsFlag = 0;


	uint8_t selectionWindowFlag = 0;
// selection window  - zapis
	uint8_t selectionWindowSaveFlag = 0;
	void showSelectionWindowSave();

	uint8_t fullMemoryDuringRecordFlag = 0;
	uint8_t fullMemoryWindowFlag = 0;
	void showSelectionWindowFullMemory();

	strPadNames padNamesStruct;

	//char *padNamesPointer[48];


	void showNotePopout();
	void hideNotePopout();

	void resizeLabel(button_resize_t *handle);
	button_resize_t resizer;

	char sourceName[8];
	char monitorVal[4];
	char gainVal[4];

	void showSource();
	void showMonitor();
	void showGain();
};

extern cSampleRecorder sampleRecorder;



#endif /* INTERFACE_SAMPLERECORDER_H_ */
