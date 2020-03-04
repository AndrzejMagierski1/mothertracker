#include "sampleEditor.h"
#include "mtSampleEditorEngine.h"

char * const effectNamesLabels[editorEffectMax] =
{
		(char*)"Delay"
};

static uint32_t popupLabelColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	0xFF0000, // ramka
};

void cSampleEditor::initDisplayControls()
{
	strControlProperties labelProperties;

	for(uint8_t i = 0; i < 8; i++)
	{
		labelProperties.value = 1;
		labelProperties.colors = interfaceGlobals.activeLabelsColors;
		labelProperties.style = ( controlStyleCenterX | controlStyleFont3 );
		labelProperties.x = (800/8)*i+(800/16);
		labelProperties.w = 800/8-6;
		labelProperties.y = 424;
		labelProperties.h = 55;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&labelProperties);
	}

	strControlProperties barProperties;

	for(uint8_t i = 0; i < 8; i++)
	{
		barProperties.x = (800/8)*i+1;
		barProperties.y = 29;
		barProperties.w = 800/8-4;
		barProperties.style =  controlStyleValue_0_100 | controlStyleBackground;
		barProperties.h = 394;

		if(bar[i] == nullptr) bar[i] = display.createControl<cBar>(&barProperties);
	}

	strControlProperties effectListProperties;

	effectListData.linesCount = editorEffectMax;
	effectListData.start = 0;
	effectListData.length = editorEffectMax;
	effectListData.data = (char**)effectNamesLabels;

	effectListProperties.x = (800/8)*6+1;
	effectListProperties.y = 29;
	effectListProperties.w = 800/4-3;
	effectListProperties.h = 394;
	effectListProperties.style = controlStyleBackground;
	effectListProperties.data = &effectListData;

	if(effectList == nullptr) effectList = display.createControl<cList>(&effectListProperties);
//Waveform
	strControlProperties spectrumProperties;

	spectrumProperties.x = 0;
	spectrumProperties.y = 75;
	spectrumProperties.w = 600;
	spectrumProperties.h = 300;
	if(playhead == nullptr) playhead = display.createControl<cProgressCursor>(&spectrumProperties);

	spectrumProperties.data = &spectrumData;
	if(spectrum == nullptr)  spectrum = display.createControl<cSpectrum>(&spectrumProperties);

	spectrumProperties.data = &spectrumPointsData;
	if(spectrumPoints == nullptr)  spectrumPoints = display.createControl<cPoints>(&spectrumProperties);
//frame
	strControlProperties frameProperties;

	frameProperties.style = 0;
	frameProperties.value = 0;

	if(frame == nullptr)  frame = display.createControl<cFrame>(&frameProperties);
//title label
	strControlProperties titleLabelProperties;

	titleLabelProperties.style = (controlStyleShow | controlStyleCenterY | controlStyleFont4);
	titleLabelProperties.x = 9;
	titleLabelProperties.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&titleLabelProperties);

	titleLabelProperties.style = ( controlStyleShow | controlStyleRightX | controlStyleCenterY | controlStyleFont4);
	titleLabelProperties.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&titleLabelProperties);

	titleLabelProperties.style = 	( controlStyleShow | controlStyleBackground);
	titleLabelProperties.x = 2;
	titleLabelProperties.y = 0;
	titleLabelProperties.w = 795;
	titleLabelProperties.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&titleLabelProperties);
//popups
	strControlProperties popupProgressProperties;

	popupProgressProperties.x = 190;
	popupProgressProperties.y = 170;
	popupProgressProperties.style = controlStyleValue_0_100;
	popupProgressProperties.h = 100;
	popupProgressProperties.w = 420;
	if(popupProgressBar == nullptr)  popupProgressBar = display.createControl<cHorizontalBar>(&popupProgressProperties);

	strControlProperties popupLabelProperties;

	popupLabelProperties.x = 400;
	popupLabelProperties.colors = popupLabelColors;
	popupLabelProperties.y = 300;
	popupLabelProperties.h = 100;
	popupLabelProperties.w = 800-(10);
	popupLabelProperties.style = ( controlStyleBackground | controlStyleCenterX | controlStyleCenterY | controlStyleFont2 );
	popupLabelProperties.text = nullptr;

	if(popupLabel == nullptr)  popupLabel = display.createControl<cLabel>(&popupLabelProperties);
//background label

	strControlProperties backgroundLabelProperties;

	backgroundLabelProperties.text = nullptr;
	backgroundLabelProperties.colors = interfaceGlobals.activeBgLabelsColors;
	backgroundLabelProperties.value = 127;
	backgroundLabelProperties.style = controlStyleNoTransparency | controlStyleShow;
	backgroundLabelProperties.x = 0;
	backgroundLabelProperties.w = 800;
	backgroundLabelProperties.y = 424;
	backgroundLabelProperties.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&backgroundLabelProperties);

//*********************
}
void cSampleEditor::destroyDisplayControls()
{
	for(uint8_t i = 0; i < 8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;

		display.destroyControl(bar[i]);
		bar[i] = nullptr;
	}

	display.destroyControl(effectList);
	effectList = nullptr;

//Waveform
	display.destroyControl(playhead);
	playhead = nullptr;

	display.destroyControl(spectrum);
	spectrum = nullptr;

	display.destroyControl(spectrumPoints);
	spectrumPoints = nullptr;
//frame
	display.destroyControl(frame);
	frame = nullptr;
//title label
	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;

	display.destroyControl(titleBar);
	titleBar = nullptr;
//popups
	display.destroyControl(popupProgressBar);
	popupProgressBar = nullptr;

	display.destroyControl(popupLabel);
	popupLabel = nullptr;
//background label
	display.destroyControl(bgLabel);
	bgLabel = nullptr;

//*********************


}
