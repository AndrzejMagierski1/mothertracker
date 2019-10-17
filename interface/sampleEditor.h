#ifndef INTERFACE_SAMPLEEDITOR_H_
#define INTERFACE_SAMPLEEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"

#include "mtStructs.h"

#define MAX_DATA_BARS	3

typedef enum
{
	spectrumWithBars,
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

typedef enum
{
	effectCrop,
	effectReverse,
	effectFlanger,
	effectChorus,
	effectDelay,

}effect_t;

typedef enum
{
	sampleLoadingActive = 0x01U,
	processingActive = 0x02U,
	applyingActive = 0x04U,
	onExitReloadActive = 0x08U,
}flags_t;


const uint8_t effectsCount = 5;
const char effectNamesLabels[effectsCount][15] =
{
		"Crop",
		"Reverse",
		"Flanger",
		"Chorus",
		"Delay",
};





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
	hControl barControl[3];
	hControl processHorizontalBarControl = nullptr;


	uint8_t selectedPlace = 0;


//----------------------------------
// spectrum + punkty
	uint8_t refreshSpectrum = 0;
	uint8_t refreshPoints = 0;


	strInstrument * editorInstrument;

	strZoomParams zoom;

	char dataBarText[3][6];

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

	void showEffectScreen(effect_screen_t *screenCfg);
	void initEffectsScreenStructs();
	void editParamFunction(uint8_t paramNum, int16_t value);

	void showValueLabels(uint8_t whichBar);
	void updateEffectValues(effect_screen_t *effect, uint8_t barNum);
	void printNewValue(const void *data, uint8_t whichBar, source_datatype_t sourceType);
	void refreshBarsValue(uint8_t whichBar, uint8_t newValue);
	void showProcessingBar(uint8_t progress);
	void showSampleLoading(uint8_t progress);
	void showApplying(uint8_t progress);
	void hideHorizontalBar();
	void updateEffectProcessing();
	uint8_t previewNewEffectRequested();
	void makeEffect();
	void showCurrentSpectrum(uint32_t length, int16_t *source);
	void refreshSampleLoading();
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

	effect_t lastPreviewEffect;
	uint8_t previewReadyFlag;

	uint8_t sampleLoadedState;
	uint8_t lastSampleLoadedState;

	uint16_t moduleFlags;

	bool isAnyEffectActive;

	uint8_t onExitFlag;
	uint8_t exitButton;

	uint8_t firstSampleLoadFlag;

	uint8_t undoCropFlag;
	uint8_t undoReverseFlag;

};


extern cSampleEditor sampleEditor;



#endif /* INTERFACE_SAMPLEEDITOR_H_ */
