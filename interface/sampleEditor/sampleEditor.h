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

//*******************CORE MODULE FUNCTIONS
	virtual void update() override;
	virtual void start(uint32_t options) override;
	virtual void stop();
	void setCommonFunctions();
	void setMainScreenFunctions();
	void setParamsScreenFunctions();

//******************* OPERATION FOR GRAPHIC
	void reloadInstrumentName();
	void reloadFrameData(enScreenType s);
	void reloadPointsData();
	void reloadStartPointText();
	void reloadEndPointText();
	void reloadZoomText();

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
	void showFrame();
	void showSpectrum();
//*******************
//******************* REFRESH - RELOAD + DISPLAY
	void refreshSpectrumPoints();
	void refreshStartPoint();
	void refreshEndPoint();
	void refreshZoom();
	void refreshEffectList();
	void refreshSpectrum();
//*******************
//******************* PARAMETERS MODIFICATORS
//MainScreen
	void modStartPoint(int16_t val);
	void modEndPoint(int16_t val);
	void modZoom(int16_t val);
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
//labelsTxt
	char currentInstrumentName[SAMPLE_NAME_SIZE+6];
	char startPointText[11];
	char endPointText[11];
	char zoomText[7];

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

//*********************
//********************MULTI-SCREEN DISPLAY HANDLING
	enScreenType screenType = mainScreen;
	uint8_t selectedEffect = editorEffectDelay;
	uint8_t selectedPlace[2];
//*********************
//********************CURRENT INSTRUMENT HANDLING
	strInstrument * editorInstrument = nullptr;
//*********************
};


extern cSampleEditor sampleEditor;



#endif /* INTERFACE_SAMPLEEDITOR_H_ */
