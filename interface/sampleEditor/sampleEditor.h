#ifndef INTERFACE_SAMPLEEDITOR_H_
#define INTERFACE_SAMPLEEDITOR_H_


#include "core/modulesBase.h"
#include "mtStructs.h"
#include "mtSampleEditorEngine.h"



class cSampleEditor: public cModuleBase
{

public:
	enum enScreenType
	{
		mainScreen,
		effectParamsScreen
	};

	enum enMainScreenPlaces
	{
		startPointPlace,
		endPointPlace,
		zoomPlace,
		effectPlaces
	};

	enum enPreviewState
	{
		previewStatePreview, // :)
		previewStatePlay,
		previewStateStop
	} previewState;

//*******************CORE MODULE FUNCTIONS
	virtual void update() override;
	virtual void start(uint32_t options) override;
	virtual void stop();
	void setCommonFunctions();
	void setMainScreenFunctions();
	void setParamsScreenFunctions();
	void switchScreen(enScreenType s);
	void clearAllFunctions();
	void restoreFunctions();
	void setStopSeqFunctions();
	void setSaveChangesFunctions();

//******************* OPERATION FOR GRAPHIC
	void prepareDisplayDataMainScreen();

	void reloadInstrumentName();
	void reloadFrameData(enScreenType s);
	void reloadPointsData();
	void reloadStartPointText();
	void reloadEndPointText();
	void reloadZoomText();
	void reloadPlayheadValue();
	void reloadCurrentEffect();
	void reloadSpectrumData();
//popups
	void reloadApplyingProgress();
	void reloadProcessingProgress();
	void reloadPlayingProgress();
//params screen
	void prepareDisplayDataParamsScreen();
	void reloadParamiterValueText(uint8_t n);
	void reloadParamiterBarValue(uint8_t n);
//*******************
//*******************PREVIEW/PLAY/STOP
	void setPreviewFunction();
	void setPlayFunction();
	void setStopFunction();

	void showPreviewLabel();
	void showPlayLabel();
	void showStopLabel();
//*******************
//*******************CORE GRAPHIC FUNCTIONS
	virtual void initDisplayControls() override;
	virtual void destroyDisplayControls() override;
	void showMainScreen();
	void showEffectParamsScreen();
//*******************
//******************* GRAPHIC OPERATIONS
	void showInstrumentName();
	void showStartPointText();
	void showEndPointText();
	void showZoomText();
	void showSpectrumPoints();
	void hideSpectrumPoints();
	void showFrame();
	void showSpectrum();
	void hideSpectrum();
	void showPlayhead();
	void hidePlayhead();
	void showEffectList();
	void hideEffectList();

	void showParamiterBar(uint8_t n);
	void showParamiterLabel(uint8_t n);
//Popups
//applying
	void showPopupApplying();
	void showProgressApplying();
//processing
	void showPopupProcessing();
	void showProgressProcessing();
//playing
	void showPopupPlaying();
	void showProgressPlaying();

	void hideProgressPopup();

//Stop Seq
	void showPopupStopSequencer();
	void showPopupSeqWindow();
//Save changes
	void showPopupSaveChanges();
	void showPopupSaveChangesWindow();

	void hideStaticPopup();
//*******************
//******************* REFRESH - RELOAD + DISPLAY
	void refreshSpectrumPoints();
	void refreshStartPoint();
	void refreshEndPoint();
	void refreshZoom();
	void refreshEffectList();
	void refreshSpectrum();
	void refreshPlayhead();
	void refreshUndoState();
	void refreshParamiter(uint8_t n);

//popups
	void refreshApplyingProgress();
	void refreshProcessingProgress();
	void refreshPlayingProgress();
//*******************
//******************* PARAMETERS MODIFICATORS
//MainScreen
	void modStartPoint(int16_t val);
	void modEndPoint(int16_t val);
	void modZoom(int16_t val);
	void modSelectedEffect(int16_t val);
//ParamiterScreen
	void modParamiter(int16_t val, uint8_t n);
//*******************
	cSampleEditor()
	{
		label[8] = { nullptr };
		bar[8] = { nullptr };
		effectList = nullptr;
		spectrum = nullptr;
		spectrumPoints = nullptr;
		playhead = nullptr;
		frame = nullptr;
		titleBar = nullptr;
		titleLabel = nullptr;
		instrumentLabel = nullptr;
		popupProgressBar = nullptr;
		popupLabel = nullptr;
		bgLabel = nullptr;
	}
	virtual ~cSampleEditor() {}

//*******************DISPLAY CONTROLS
	hControl label[8];
	hControl bar[8];
	hControl effectList;
//Waveform
	hControl spectrum;
	hControl spectrumPoints;
	hControl playhead;
//frame
	hControl frame;
//title label
	hControl titleBar;
	hControl titleLabel;
	hControl instrumentLabel;
//popups
	hControl popupProgressBar;
	hControl popupLabel;
//background label
	hControl bgLabel;
//*********************
//*******************DISPLAY CONTROLS DATA
	strList effectListData;
	strTrackerSpectrum spectrumData =
	{
			.width = 600,
	};
	strTrackerPoints spectrumPointsData =
	{
			.startPoint = 0,
			.endPoint = 600
	};
	strFrameData frameData;
	strLabelData labelArrow =
	{
		{
		 { (800/8)*6+(800/16), 460, displayArrowU}, // bitmaps[0]
		 { (800/8)*7+(800/16), 460, displayArrowD}  // bitmaps[1]
		}
	};

	uint8_t paramsBarValue[6];
	//applying
	uint8_t applyingProgress;
	uint8_t applyingSteps;
	bool applyingInProgress;
	//processing
	uint8_t processingProgress;
	uint8_t processingSteps;
	bool processingInProgress;
	//playing
	uint8_t playingProgress;
	bool playingInProgress;

	bool isLoadedData;
	bool isProcessedData;

	bool confirmedDataIsChanged;

	uint8_t moduleToChange;

//labelsTxt
	char currentInstrumentName[SAMPLE_NAME_SIZE+6];
	char startPointText[11];
	char endPointText[11];
	char zoomText[7];

//params screen
	char paramiterValueLabel[6][11];

	char * const paramiterValueLabelPtr[6] =
	{
			&paramiterValueLabel[0][0],
			&paramiterValueLabel[1][0],
			&paramiterValueLabel[2][0],
			&paramiterValueLabel[3][0],
			&paramiterValueLabel[4][0],
			&paramiterValueLabel[5][0],
	};
//*********************


//********************SPECTRUM PROCESSING
	strSpectrumParams spectrumParams;
	strZoomParams zoom;

	struct strSelection
	{
		uint16_t startPoint = 0;
		uint16_t endPoint = MAX_16BIT;
	} selection;

	uint8_t needRefreshSpectrum;
	uint8_t needRefreshPlayhead; // globalnie w update
	uint8_t needShowPlayhead; // wewnetrznie w odswiezaniu
	uint16_t playheadValue;
	elapsedMicros refreshPlayheadTimer;

//*********************
//********************MULTI-SCREEN DISPLAY HANDLING
	enScreenType screenType = mainScreen;
	uint8_t selectedEffect = editorEffectDelay;
	uint8_t selectedPlace[2];

	struct strEffectDisplayParams
	{
		int iParameter[6];
		float fParameter[6];

		uint8_t paramsNumber;
		const char * const * labelsText;
		const char * paramsType; //'f' = float 'd' = int

		int * iUpConstrain;
		float * fUpConstrain;

		int * iDownConstrain;
		float * fDownConstrain;

		float * changeStep;
		float * displayMult;
		const char * const * afterValueText;
	} effectDisplayParams[editorEffectMax] =
	{
			{
				{0,50,0,0,0,0},{0.95,0,0,0,0,0}, //start values
				2,delayParams::labelText,delayParams::paramsType,
				delayParams::iUpConstrain,delayParams::fUpConstrain,
				delayParams::iDownConstrain,delayParams::fDownConstrain,
				delayParams::changeStep,delayParams::displayMult,
				delayParams::afterValueText
			},

			{
				{4,44100,0,0,0,0},{0,0,0,0,0,0}, //start values
				2,bitcrusherParams::labelText,bitcrusherParams::paramsType,
				bitcrusherParams::iUpConstrain,bitcrusherParams::fUpConstrain,
				bitcrusherParams::iDownConstrain,bitcrusherParams::fDownConstrain,
				bitcrusherParams::changeStep,bitcrusherParams::displayMult,
				bitcrusherParams::afterValueText
			}
	};

//*********************
//********************CURRENT INSTRUMENT HANDLING
	strInstrument	sampleEditInstrument;
	strInstrument * const editorInstrument = &sampleEditInstrument;
//*********************
//********************EFFECT LIST HANDLING
	uint8_t currentEffectIdx;
	mtEffect * currentEffect = nullptr;
	mtEffect * lastEffect = nullptr;
//*********************
};


extern cSampleEditor sampleEditor;



#endif /* INTERFACE_SAMPLEEDITOR_H_ */
