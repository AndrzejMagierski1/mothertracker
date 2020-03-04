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


	virtual void update() override;
	virtual void start(uint32_t options) override;
	virtual void stop();

	virtual void initDisplayControls() override;
	virtual void destroyDisplayControls() override;

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
	strTrackerSpectrum spectrumData;
	strTrackerPoints spectrumPointsData;
//*********************


//********************SPECTRUM PROCESSING
	strSpectrumParams spectrumParams;
	strZoomParams zoom;
//*********************
//********************MULTI-SCREEN DISPLAY HANDLING
	enScreenType screenType = mainScreen;
	uint8_t selectedEffect = editorEffectDelay;
//*********************
//********************CURRENT INSTRUMENT HANDLING
	strInstrument * editorInstrument = nullptr;
//*********************
};


extern cSampleEditor sampleEditor;



#endif /* INTERFACE_SAMPLEEDITOR_H_ */
