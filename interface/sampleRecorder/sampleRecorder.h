#ifndef INTERFACE_SAMPLERECORDER_H_
#define INTERFACE_SAMPLERECORDER_H_


#include "core/modulesBase.h"
#include "SD.h"
#include "mtStructs.h"
#include "mtKeyboardManager.h"

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

const strRecorderConfig defaultRecorderConfig =
{
	.source = 0,
	.gainLineIn = 0,
	.gainMicLow = 0,
	.gainMicHigh = 0,
	.gainRadio = 0,
	.radioFreq = 87.5
};

struct strUndoCrop
{
	int16_t * address;
	uint32_t length;
	uint16_t startPoint;
	uint16_t endPoint;
};

#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	2

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
		 label[8] = {nullptr};
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
		 //popupWindowLabel = nullptr;

	}
	virtual ~cSampleRecorder() {}

	void showDefaultScreen();
	void deactivateGui();
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

	void setDefaultScreenFunct();


	void processSpectrum1();
	void processPoints();
	void changeZoom(int16_t value);
	void changeMonitorSelection(int16_t value, bool rollListOver = false);
	void changeSourceSelection(int16_t value, bool rollListOver = false);


	void activateLabelsBorder();
	void refreshConfigLists();
	void refreshGain();

	void setPrevievFlag(uint8_t s);

	void refreshRDS();
	void displaySeeking();
	void displayEmptyRDS();
	void hideRDS();

	void noteOnHandle(uint8_t channel, uint8_t note, uint8_t velocity, int16_t source);
	void noteOffHandle(uint8_t channel, uint8_t note, uint8_t velocity, int16_t source);

	strFrameData frameData;
	strSelectWindowData selectWindowData;

//*********************************************
	hControl spectrumControl;
	hControl label[8];
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
	//hControl popupWindowLabel;
	hControl progressCursor;
	hControl radioRdsLabel;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	//hControl notePopoutControl = nullptr;
	hControl bgLabel;

//*********************************************
	uint8_t dontTurnOffRadio;
	uint8_t wasSampleMemoryOverwritten;

	uint8_t selectedPlace = 0;
	uint8_t currentScreen = 0;
	strRecorderConfig recorderConfig;
	uint32_t spectrumTimerConstrains = 100;
	char freqTextValue[10];
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

	strLabelData doubleArrow[2];

	elapsedMillis refreshSpectrumTimer;


	strInstrument * editorInstrument;
	uint8_t refreshSpectrumProgress = 0;
	uint8_t loopDirection = 0;
	float playPitch = 1.0 ;
	elapsedMicros playProgresValueBackwardTim = 0;

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
	elapsedMillis redColorTimer;

	void calcRadioFreqBarVal();
	void calcLevelBarVal();
	void calcGainBarVal();

	void drawRadioFreqBar();
	void drawLevelBar();
	void drawGainBar();

	void changeRadioFreqBar(int16_t val);
	void changeLevelBar();
	void changeGainBar(int16_t val);

	void setDefaultSource();
	void setDefaultGain();
	void setDefaultMonitorEnable();
	void setDefaultRadioFreq();

	void setDefaultStartPoint();
	void setDefaultEndPoint();
	void setDefaultZoom();


	mtKeyboardManager keyboardManager;
//save
	enum saveType
	{
		saveTypeNormal = 1,
		saveTypeLoad

	};

	int8_t firstFreeInstrumentSlotFound;
	uint8_t saveProgress;
	uint8_t saveOrSaveloadFlag = 0;
	uint8_t forceSwitchModule = 0;
	void showSaveHorizontalBar();
	void hideSaveHorizontalBar();
// selection window
	void showSelectionWindow();
	void showSelectionNotEnoughInstruments();
	void showSelectionNotEnoughMemory();
	void showSelectionStopPattern();
	uint8_t notEnoughInstrumentsFlag = 0;
	uint8_t notEnoughMemoryFlag = 0;
	uint8_t patternIsPlayingFlag = 0;

	enum struct enChangesLostSource
	{
		cancelFunct,
		switchModuleFunct
	} changesLostSource;
	uint8_t moduleToSwitch;
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


	char sourceName[8];
	char monitorVal[4];
	char gainVal[4];

	void showSource();
	void showMonitor();
	void showGain();

	uint8_t notePopoutFlag = 0;

	select_node_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct_t funct , uint8_t nodeNum);
	void removeNode(uint8_t nodeNum);
	void stepThroughNodes(int16_t value);
	void clearAllNodes();
	void cancelMultiFrame();
};

extern cSampleRecorder sampleRecorder;



#endif /* INTERFACE_SAMPLERECORDER_H_ */
