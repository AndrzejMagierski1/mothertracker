#ifndef INTERFACE_SAMPLEEDITOR_H_
#define INTERFACE_SAMPLEEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"

#define MAX_DATA_BARS	4
#define BAR_MIN_POS		2

typedef enum
{
	noSpectrum,
	fullSpectrum,

}screen_type_t;

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

}effect_screen_t;

typedef struct
{
	uint8_t percentages[4];
	uint8_t mask;

}selection_percentages;

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
	onEntryStillLoading = 0x20U,
}flags_t;


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
		topLabel[8] = {nullptr};
		bottomLabel[8] = {nullptr};
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

	void startLoadingSample();

	strFrameData frameData;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl effectListControl;
	hControl spectrumControl;
	hControl pointsControl;
	hControl frameControl;
	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;
	hControl barControl[4];
	hControl processHorizontalBarControl = nullptr;


	uint8_t selectedPlace = 6;


//----------------------------------
// spectrum + punkty
	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;


	strInstrument * editorInstrument;

	strZoomParams zoom;

	char dataBarText[4][8];

	strTrackerSpectrum spectrum;
	strTrackerPoints points;
	strSpectrumParams spectrumParams;

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
	effect_screen_t effectScreen[effectsCount];
	uint8_t currSelEffect;

	void frameChange(uint8_t control);

	void resizeUndo(uint8_t control);//1 - center 0 - top
	void showEffectScreen(effect_screen_t *screenCfg);
	void initEffectsScreenStructs();
	void editParamFunction(uint8_t paramNum, int16_t value);
	void editParamFunctionSelection(int16_t value);

	void showValueLabels(uint8_t whichBar);
	void updateEffectValues(effect_screen_t *effect, uint8_t barNum);
	void printNewValue(const void *data, uint8_t whichBar, const char* unit, source_datatype_t sourceType);
	void refreshBarsValue(uint8_t whichBar, uint8_t newValue);
	void showHorizontalBar(uint8_t progress , const char* text);
	void hideHorizontalBar();
	void updateEffectProcessing();
	uint8_t previewNewEffectRequested();
	void makeEffect();
	void showCurrentSpectrum(uint32_t length, int16_t *source);
	void refreshSampleLoading();
	void refreshInstrumentLoading();
	void refreshSampleApplying();
	void resetSpectrumAndPoints();
	void onExitRaload();


	void undoDisplayControl(uint8_t onOff);

	uint16_t startPoint;
	uint16_t endPoint = MAX_16BIT;

	// Chorus inputs
	uint16_t chorusLength;
	uint8_t chorusStrength;

	// Delay inputs
	float delayFeedback;
	uint16_t delayTime;

	// Flanger inputs
	float flangerDelay;
	uint8_t flangerDepth;
	uint8_t flangerOffset;

	//Compressor inputs
	uint16_t compressorThrs;
	uint16_t compressorRatio;
	uint16_t compressorAttack;
	uint16_t compressorRelease;

	//Bitcrusher Inputs
	uint8_t bitcrusherBits;
	uint16_t bitcrusherRate;

	//Amplifier inputs
	float amplifierAmp;

	//Limiter inputs
	uint16_t limiterThreshold;
	uint16_t limiterAttack;
	uint16_t limiterRelease;

	effect_t lastPreviewEffect;
	uint8_t previewReadyFlag;

	uint8_t sampleLoadedState;
	uint8_t lastSampleLoadedState;

	uint16_t moduleFlags;

	bool isAnyEffectActive;

	uint8_t onExitFlag;
	uint8_t exitButton;
	uint8_t effectAppliedFlag;

	uint8_t firstSampleLoadFlag;

	uint8_t undoCropFlag;
	uint8_t undoReverseFlag;

	uint8_t sampleIsValid;

	//----------------------------------
	// multisel
	select_node1_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct1_t funct , uint8_t nodeNum);
	void removeNode(uint8_t nodeNum);
	selection_percentages stepThroughNodes(int16_t value);
	void clearAllNodes();
	void cancelMultiFrame();

};


extern cSampleEditor sampleEditor;



#endif /* INTERFACE_SAMPLEEDITOR_H_ */
