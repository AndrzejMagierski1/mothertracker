#ifndef INTERFACE_SAMPLEPLAYBACK_H_
#define INTERFACE_SAMPLEPLAYBACK_H_


#include "core/modulesBase.h"
#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"



#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	4

const char playModeFunctLabels[playModeCount][15]=
{
		"1-Shot",
		"Forward Loop",
		"Backward Loop",
		"Pingpong Loop",
		"Slice",
		"Wavetable",
		"Granular"
};

constexpr uint8_t MAX_WAVETABLE_WINDOWS_COUNTER = 11;
constexpr uint8_t MIN_WAVETABLE_WINDOWS_COUNTER = 5;


class cSamplePlayback: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSamplePlayback()
	{
		playModeListControl = nullptr;
		frameControl = nullptr;
		titleBar = nullptr ;
		titleLabel = nullptr;
		instrumentLabel = nullptr;
		wtPositionCursor = nullptr;
		bgLabel = nullptr;

		editorInstrument = nullptr;

		progressCursor = nullptr;
		granularCursor = nullptr;
		spectrumControl = nullptr;
		pointsControl = nullptr;
		slicePointsControl = nullptr;
		label[7] = {nullptr};
		popupLabel = nullptr;
		lastSampleLength = 0;
	}
	virtual ~cSamplePlayback() {}

	void showDefaultScreen();
	void showZoomValue();
	void showPlayModeList();
	void showTitleBar();
	void hideUselessControls();

	void setDefaultScreenFunct();
	void cancelPopups();

	void processPoints();
	void processSlicePoints();
	void showPreviewValue();
	void hidePreviewValue();

	char startPointValueText[8];
	char endPointValueText[8];
	char loopPoint1ValueText[8];
	char loopPoint2ValueText[8];
	char playTimeValueText[8];
	void showStartPointValue();
	void showEndPointValue();
	void showLoopPoint1Value();
	void showLoopPoint2Value();
	void showSlicesSelectValue();
	void showSlicesAdjustValue();


	void hideLoopPoints();
	void showLoopPoints();

	void showWavetablePositionCursor();
	void hideWavetablePositionCursor();

	void selectCorrectPoints();

	void calcPlayProgressValue();
	void activateLabelsBorder();


	void showAutoSlicePopup();
	void hideAutoSlicePopup();
	strFrameData frameData;

	strLabelData labelArrow;

	hControl label[7];
	hControl playModeListControl;
	hControl spectrumControl;
	hControl pointsControl;
	hControl slicePointsControl;
	hControl frameControl;
	hControl titleBar;
	hControl titleLabel;
	hControl instrumentLabel;
	hControl progressCursor;
	hControl granularCursor;
	hControl wtPositionCursor;
	hControl bgLabel;
	hControl popupLabel;




	uint8_t selectedPlace = 6;


//----------------------------------
// spectrum + punkty

	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;
	uint8_t refreshSlicePoints = 0;
	uint8_t currentAutoSlice = 0;
	uint8_t lastAutoSlice = 0;
	uint8_t autoSlicePopupVisible = 0;
	uint8_t refreshSpectrumProgress = 0;
	uint32_t playProgressValue = 0; // 0 - MAX_LEN_RECORD
	uint16_t playProgressInSpectrum = 0; // 0 - 600
	uint8_t loopDirection = 0;
	elapsedMicros playProgresValueBackwardTim = 0;
	elapsedMicros playProgresValueTim = 0;
	elapsedMicros refreshPlayProgressValue = 0;
	float playPitch = 1.0 ;


	strInstrument * editorInstrument;
	uint32_t lastSampleLength = 0xFFFFFFFF;

	strZoomParams zoom;

	char zoomTextValue[7];

	char sliceSelectTextValue[5];
	char sliceAdjustTextValue[9];

	strTrackerSpectrum spectrum;
	strTrackerPoints points;
	strSlicePoints slicePoints;

	wt_position_data_t wtPosition;


//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
	//char actualInstrName[SAMPLE_NAME_SIZE+4];

//----------------------------------
// lista play mode
	strList playModeList;

	void listPlayMode();

	char *playModeNames[playModeCount];


//----------------------------------
// odtwarzanie
	uint8_t currentSeqState = 0;
	uint8_t lastSeqState = 0;
	uint8_t isPlayingSample = 0;
	uint8_t isPlayingWavetable = 0;
	uint32_t currentEnvelopeWtPos;
	uint32_t lastEnvelopeWtPos;
	uint32_t currentEnvelopeGranPos;
	uint32_t lastEnvelopeGranPos;
	int8_t playNote = 24;
	uint8_t glidePreviewDif = 0;
	uint8_t seqReleaseFlag = 0;
	uint8_t startNoteStoppedSeq = 0;

//----------------------------------
	// multisel
	select_node_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct_t funct , uint8_t nodeNum);
	void removeNode(uint8_t nodeNum);
	void stepThroughNodes(int16_t value);
	void clearAllNodes();
	void cancelMultiFrame();
//--------------------------------------
	uint8_t loadedInstrumentType = 0;
	char wavetablePositionText[7];
	char wavetableWindowSizeText[7];
	void showWavetablePosition();
	void showWavetableWindowSize();
	uint16_t convertWavetableWindowsCounterToSize(uint8_t cnt);
	uint8_t convertWavetableWindowsSizeToCounter(uint16_t size);
	uint8_t wavetableWindowsCounter = 11;
	uint8_t refreshWavetablePosition;

	void processWavetableCursor(uint32_t position);

	char granularPositionTextValue[15];
	char grainLengthTextValue[15];
	char shapeText[15];
	char loopTypeText[15];

	uint16_t granularPositionInSpectrum;
	float grainLengthMs;

	void showGranularPositionValue();
	void showGrainLengthValue();
	void showShapeText();
	void showLoopTypeText();
};

extern cSamplePlayback samplePlayback;



#endif /* INTERFACE_SAMPLEPLAYBACK_H_ */
