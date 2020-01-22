#ifndef INTERFACE_SAMPLEEDITOR_H_
#define INTERFACE_SAMPLEEDITOR_H_


#include "core/modulesBase.h""
#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"
#include "mtEffector.h"

#define MAX_DATA_BARS	4
#define BAR_MIN_POS		2

typedef enum
{
	noSpectrum,
	fullSpectrum,

}screen_type_t;

typedef enum
{
	eRequireProcessing,
	eProcessed,
	eNotAffecting,
}effect_state_t;

typedef enum
{
	tUNSIGNED8,
	tUNSIGNED16,
	tUNSIGNED32,
	tSIGNED8,
	tSIGNED16,
	tSIGNED32,
	tFLOAT,

}source_datatype_t;

typedef uint8_t (*paramEditFunct_t)(int16_t);

typedef struct
{
	const char* 		name;
	paramEditFunct_t	editFunct;
	void*				dataSource;
	source_datatype_t	dataFormat;
	const char*			dataUnit;
	uint8_t 			effectPercentage;

}effect_bar_t;

typedef struct
{
	uint8_t				paramNum;
	screen_type_t 		screen;// screen type
	uint8_t 			barNum;// number of active data bars
	effect_bar_t		bar[MAX_DATA_BARS];
	uint8_t 			undoActive;
	effect_state_t 		effectStage;

}effect_handle_t;

typedef struct
{
	uint8_t percentages[4];
	uint8_t mask;

}selection_percentages;

typedef struct
{
	uint8_t taskQueue;
	uint8_t tasksInQueue;
	uint8_t currentTask;
	uint8_t taskQueueProgress;
	uint8_t taskQueueActive;
	uint8_t taskQueueNextFlag;

}taskQueue_t;

typedef enum
{
	effectCrop,
	effectReverse,
	effectFlanger,
	effectChorus,
	effectDelay,
	effectCompressor,
	effectBitcrusher,
	effectAmplifier,
	effectLimiter,

}effect_t;

typedef enum
{
	sampleLoadingActive = 0x01U,
	processingActive = 0x02U,
	applyingActive = 0x04U,
	undoActive = 0x08U,
	onExitReloadActive = 0x10U,
	previewRunning = 0x20U,

}flags_t;

typedef enum
{
	tLoadSample = 0x01U,
	tProcessEffect = 0x02U,
	tApply = 0x04U,
	tUndo = 0x08U,

}tasks_t;


const uint8_t effectsCount = 9;
const char effectNamesLabels[effectsCount][15] =
{
		"Crop",
		"Reverse",
		"Flanger",
		"Chorus",
		"Delay",
		"Compressor",
		"Bitcrusher",
		"Amplifier",
		"Limiter",
};


#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	4

constexpr uint32_t PLAY_REFRESH_US = 5000;


class cSampleEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cSampleEditor()
	{
//		inActiveInstrumentsCount = 0;
//		inActiveInstrumentIndex = 0;
		editorInstrument = nullptr;
//		openedInstrFromActive = 0;
//		openedInstrumentIndex = 0;

		spectrumControl = nullptr;
		pointsControl = nullptr;
		label[8] = {nullptr};
	}
	virtual ~cSampleEditor() {}

	void showDefaultScreen();
	void showEffectsList();
	void showTitleBar();

	void setDefaultScreenFunct();
	void cancelPopups();


//	void processSpectrum();
	void processPoints();
	void changeEffectSelection(int16_t value);

	void activateLabelsBorder();

	uint8_t startLoadingSample();

	strFrameData frameData;
	strLabelData labelArrow;

	hControl label[8];
	hControl effectListControl;
	hControl spectrumControl;
	hControl effectSpectrumControl;
	hControl pointsControl;
	hControl frameControl;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl barControl[4];
	hControl processHorizontalBarControl = nullptr;
	hControl progressCursor = nullptr;
	hControl bgLabel;


	uint8_t selectedPlace = 6;


//----------------------------------
// spectrum + punkty
	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;


	strInstrument * editorInstrument;

	strZoomParams zoom;
	strZoomParams effectZoom;

	char dataBarText[4][8];

	strTrackerSpectrum spectrum;
	strTrackerSpectrum effectSpectrum;
	strTrackerPoints points;
	strSpectrumParams spectrumParams;
	strSpectrumParams effectSpectrumParams;

//----------------------------------
// aktualny instrument na belce tytułowej
	void showActualInstrument();
	//char actualInstrName[SAMPLE_NAME_SIZE+4];
//----------------------------------
// lista play mode
	strList effectList;
	void listEffects();
	char *effectNames[effectsCount];

//----------------------------------
// odtwarzanie
	uint8_t isPlayingSample = 0;
	int8_t playNote = 24;
	uint8_t glidePreviewDif = 0;

//----------------------------------

	char instrumentPath[PATCH_SIZE];
	uint8_t localInstrNum;
	effect_handle_t effectControl[effectsCount];
	uint8_t currSelEffect;

	void frameChange(uint8_t control);

	void showEffectScreen(effect_handle_t *screenCfg);
	void showPreviewSpectrum();
	void hidePreviewSpectrum();
	void initEffectsScreenStructs();
	void editParamFunction(uint8_t paramNum, int16_t value);
	void editParamFunctionSelection(int16_t value);

	void showValueLabels(uint8_t whichBar);
	void updateEffectValues(effect_handle_t *effect, uint8_t barNum);
	void printNewValue(const void *data, uint8_t whichBar, const char* unit, source_datatype_t sourceType);
	void refreshBarsValue(uint8_t whichBar, uint8_t newValue);
	void showHorizontalBar(uint8_t progress , const char* text);
	void hideHorizontalBar();
	void updateEffectProcessing();
	void makeEffect();
	void showCurrentSpectrum(uint32_t length, int16_t *source);
	void refreshSampleLoading();
	void refreshSampleApplying();
	void resetSpectrumAndPoints();
	void onExitReload();

	void processOrPreview(uint8_t x);
	void undoDisplayControl(uint8_t onOff);

	void showEffectSpectrum();
	void hideEffectsSpectrum();

	//Crop or Reverse
	uint16_t startPoint;
	uint16_t endPoint = MAX_16BIT;
	float startTime;
	float endTime;

	// Chorus inputs
	uint8_t  mChorusLength;
	uint16_t sChorusLength;
	uint8_t  smChorusStrength;

	// Delay inputs
	float delayFeedback;
	uint16_t delayTime;

	// Flanger inputs
	uint8_t flangerDelay;
	uint8_t flangerDepth;
	uint8_t flangerOffset;

	//Compressor inputs
	uint16_t smCompressorRatio;

	uint16_t mCompressorThrs;
	float	 mCompressorAttack;
	float 	 mCompressorRelease;

	uint16_t sCompressorThrs;
	uint16_t sCompressorAttack;
	float 	 sCompressorRelease;

	//Bitcrusher Inputs
	uint8_t  smBitcrusherBits = BITCRUSHER_BITS_MAX;
	uint8_t	 mBitcrusherRate = 255;
	uint16_t sBitcrusherRate = BITCRUSHER_RATE_MAX;

	//Amplifier inputs
	float amplifierAmp = AMPLIFIER_AMP_DEFAULT;

	//Limiter inputs
	uint8_t	 mLimiterThreshold;
	float 	 mLimiterAttack;
	float	 mLimiterRelease;

	uint16_t sLimiterThreshold;
	uint16_t sLimiterAttack;
	float 	 sLimiterRelease;

	effect_t lastPreviewEffect;

	uint8_t sampleLoadedState;
	uint8_t lastSampleLoadedState;

	uint16_t moduleFlags;

	uint8_t onExitFlag;
	uint8_t exitButton;
	uint8_t effectAppliedFlag;

	uint8_t undoCropFlag;
	uint8_t undoReverseFlag;

	uint8_t sampleIsValid;

	uint8_t previewButtonState = 0;

	uint8_t newSpectrumFlag = 0;

	//----------------------------------
	// multisel
	select_node1_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct1_t funct , uint8_t nodeNum);
	void removeNode(uint8_t nodeNum);
	selection_percentages stepThroughNodes(int16_t value);
	void clearAllNodes();
	void cancelMultiFrame();

	// linia postepu odgrywania
	elapsedMicros playProgresValueTim = 0;
	elapsedMicros refreshPlayProgressValue = 0;
	float playPitch = 1.0 ;
	uint32_t playProgressValue = 0; // 0 - MAX_LEN_RECORD
	uint16_t playProgressInSpectrum = 0; // 0 - 600
	uint8_t refreshSpectrumProgress = 0;

	void refreshPlayingProgress();
	void calcPlayProgressValue();

	void stopPreview(uint8_t pad);
	void playPreview(uint8_t pad);

	void applyEffect();
	void undoEffect();

	void resetAllEffectsState();

	uint8_t applyRequested;
	uint8_t undoRequested;

	//Common progress bar
	taskQueue_t taskQueue;
	uint8_t checkPreConditions(taskQueue_t *queue);
	void cancelTaskQueue(taskQueue_t *queue);
	uint8_t handleTasks(taskQueue_t *queue, uint8_t force);
	void finishTask(taskQueue_t *queue, tasks_t task);
	void handleQueueProgress(taskQueue_t *queue, uint8_t taskProgress, const char *text);
	void showQueueHorizontalBar(uint8_t progress , const char* text);

};


extern cSampleEditor sampleEditor;



#endif /* INTERFACE_SAMPLEEDITOR_H_ */
