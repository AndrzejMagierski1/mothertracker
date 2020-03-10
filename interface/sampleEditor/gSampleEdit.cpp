#include "sampleEditor.h"
#include "mtSampleEditorEngine.h"

char * const effectNamesLabels[editorEffectMax] =
{
		(char*)"Delay",
		(char*)"Bitcrusher"
};

static uint32_t popupLabelColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	0xFF0000, // ramka
};

void cSampleEditor::initDisplayControls()
{

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
//frame
	strControlProperties frameProperties;

	frameProperties.style = 0;
	frameProperties.value = 0;
	frameProperties.data= &frameData;

	if(frame == nullptr)  frame = display.createControl<cFrame>(&frameProperties);
// label
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
// bar
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
//list
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

void cSampleEditor::showMainScreen()
{
//Bars
	for (uint8_t i = 0; i < 8; i++ )
	{
		display.setControlHide(bar[i]);
		display.refreshControl(bar[i]);
	}
//Labels
	display.setControlText(label[0], "Start");
	display.setControlText(label[1], "End");
	display.setControlText(label[2], "Zoom");
	display.setControlText(label[3], "Undo");
	display.setControlText(label[4], "");
	display.setControlText(label[5], "Apply");

	showStartPointText();
	showEndPointText();
	showZoomText();

	for(uint8_t i = 0; i < 7; i++)
	{
		if(editorInstrument->isActive) display.setControlColors(label[i],interfaceGlobals.activeLabelsColors);
		else display.setControlColors(label[i],interfaceGlobals.inactiveLabelsColors);
	}

	if(editorInstrument->isActive)
	{
		if(!currentEffect->undo.isEnable) display.setControlColors(label[3],interfaceGlobals.inactiveLabelsColors);
	}



	for (uint8_t i = 0; i < 6; i++ )
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlPosition(label[i], (800/8)*i+(800/16), 424);
		display.setControlSize(label[i], 800/8-6, 55);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

	display.setControlText(label[6], "Effect");
	display.setControlData(label[6], &labelArrow);
	display.setAddControlStyle(label[6], controlStyleShowBitmap);
	display.setControlPosition(label[6], (800/4)*3+(800/8), 424);
	display.setControlSize(label[6], 800/4-6, 55);
	display.setControlShow(label[6]);
	display.refreshControl(label[6]);

	display.setControlHide(label[7]);
	display.refreshControl(label[7]);
//List
	refreshEffectList();
//Waveform
	showSpectrum();

	showSpectrumPoints();

	hidePlayhead();
//frame
	if(editorInstrument->isActive) 	showFrame();
	else
	{
		display.setControlHide(frame);
		display.refreshControl(frame);
	}
//title label
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Sample Editor 1/2");
	display.refreshControl(titleLabel);

	showInstrumentName();
//popups
	display.setControlHide(popupLabel);
	display.refreshControl(popupLabel);
	display.setControlHide(popupProgressBar);
	display.refreshControl(popupLabel);
//background label

	display.setControlValue(bgLabel,0b01111111);
	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);

	display.synchronizeRefresh();

}
void cSampleEditor::showEffectParamsScreen()
{
//Bars
	for (uint8_t i = 0; i < 8; i++ )
	{
		if( i < effectDisplayParams[currentEffectIdx].paramsNumber)
		{
			display.setControlValue(bar[i],paramsBarValue[i]);
			display.setControlShow(bar[i]);
		}
		else
		{
			display.setControlHide(bar[i]);
		}
		display.refreshControl(bar[i]);
	}
//Labels

	for ( uint8_t i = 0; i < 6; i++ )
	{
		if( i < effectDisplayParams[currentEffectIdx].paramsNumber)
		{
			display.setControlText(label[i],  effectDisplayParams[currentEffectIdx].labelsText[i]);
			display.setControlText2(label[i], paramiterValueLabelPtr[i]);
		}
		else
		{
			display.setControlText(label[i], "");
			display.setControlText2(label[i],"");
		}
	}
	switch(previewState)
	{
	case previewStatePreview: 		display.setControlText(label[6], "Preview"); 	break;
	case previewStatePlay:			display.setControlText(label[6], "Play");	break;
	case previewStateStop:			display.setControlText(label[6], "Stop");	break;
	}


	display.setControlText(label[7], "Apply");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");



	for(uint8_t i = 0; i < 8; i++)
	{
		if(editorInstrument->isActive) display.setControlColors(label[i],interfaceGlobals.activeLabelsColors);
		else display.setControlColors(label[i],interfaceGlobals.inactiveLabelsColors);
	}

	for (uint8_t i = 0; i < 8; i++ )
	{
		display.setControlStyle(label[i],  controlStyleCenterX | controlStyleFont3);
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlPosition(label[i], (800/8)*i+(800/16), 424);
		display.setControlSize(label[i], 800/8-6, 55);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
	}

//List
	hideEffectList();
//Waveform
	hideSpectrum();

	hideSpectrumPoints();

	hidePlayhead();
//frame
	if(editorInstrument->isActive) 	showFrame();
	else
	{
		display.setControlHide(frame);
		display.refreshControl(frame);
	}
//title label
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	display.setControlText(titleLabel, "Sample Editor 2/2");
	display.refreshControl(titleLabel);

	showInstrumentName();
//popups
	display.setControlHide(popupLabel);
	display.refreshControl(popupLabel);
	display.setControlHide(popupProgressBar);
	display.refreshControl(popupLabel);
//background label

	display.setControlValue(bgLabel,0b11111111);
	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);

	display.synchronizeRefresh();
}

void cSampleEditor::showInstrumentName()
{
	display.setControlShow(instrumentLabel);
	display.setControlText(instrumentLabel,  currentInstrumentName);
	display.refreshControl(instrumentLabel);
}
void cSampleEditor::showStartPointText()
{
	display.setControlText2(label[0],startPointText);
	display.refreshControl(label[0]);
}
void cSampleEditor::showEndPointText()
{
	display.setControlText2(label[1],endPointText);
	display.refreshControl(label[1]);
}
void cSampleEditor::showZoomText()
{
	display.setControlText2(label[2],zoomText);
	display.refreshControl(label[2]);
}
void cSampleEditor::showSpectrumPoints()
{
	display.setControlShow(spectrumPoints);
	display.refreshControl(spectrumPoints);
}

void cSampleEditor::hideSpectrumPoints()
{
	display.setControlHide(spectrumPoints);
	display.refreshControl(spectrumPoints);
}

void cSampleEditor::showFrame()
{
	display.setControlValue(frame, selectedPlace[screenType]);
	display.setControlShow(frame);
	display.refreshControl(frame);
}
void cSampleEditor::showSpectrum()
{
	display.setControlShow(spectrum);
	display.refreshControl(spectrum);
}

void cSampleEditor::hideSpectrum()
{
	display.setControlHide(spectrum);
	display.refreshControl(spectrum);
}
void cSampleEditor::showPlayhead()
{
	display.setControlShow(playhead);
	display.refreshControl(playhead);
}
void cSampleEditor::hidePlayhead()
{
	display.setControlHide(playhead);
	display.refreshControl(playhead);
}
void cSampleEditor::showEffectList()
{
	display.setControlShow(effectList);
	display.refreshControl(effectList);
}
void cSampleEditor::hideEffectList()
{
	display.setControlHide(effectList);
	display.refreshControl(effectList);
}

void cSampleEditor::showParamiterBar(uint8_t n)
{
	display.setControlValue(bar[n], paramsBarValue[n]);
	display.setControlShow(bar[n]);
	display.refreshControl(bar[n]);
}
void cSampleEditor::showParamiterLabel(uint8_t n)
{
	display.setControlText2(label[n], paramiterValueLabelPtr[n]);
	display.setControlShow(label[n]);
	display.refreshControl(label[n]);
}

//Popups

//Applying
void  cSampleEditor::showPopupApplying()
{
	display.setControlText(popupProgressBar, "Applying...");
	display.setControlValue(popupProgressBar,applyingProgress);
	display.setControlShow(popupProgressBar);
	display.refreshControl(popupProgressBar);
}
void  cSampleEditor::showProgressApplying()
{
	display.setControlValue(popupProgressBar,applyingProgress);
	display.refreshControl(popupProgressBar);
}
//processing
void cSampleEditor::showPopupProcessing()
{
	display.setControlText(popupProgressBar, "Processing...");
	display.setControlValue(popupProgressBar, processingProgress);
	display.setControlShow(popupProgressBar);
	display.refreshControl(popupProgressBar);
}
void cSampleEditor::showProgressProcessing()
{
	display.setControlValue(popupProgressBar,processingProgress);
	display.refreshControl(popupProgressBar);
}
//playing
void cSampleEditor::showPopupPlaying()
{
	display.setControlText(popupProgressBar, "Playing...");
	display.setControlValue(popupProgressBar,playingProgress);
	display.setControlShow(popupProgressBar);
	display.refreshControl(popupProgressBar);
}
void cSampleEditor::showProgressPlaying()
{
	display.setControlValue(popupProgressBar,playingProgress);
	display.refreshControl(popupProgressBar);
}
void  cSampleEditor::hidePopup()
{
	display.setControlHide(popupProgressBar);
	display.refreshControl(popupProgressBar);
}

//************* play/stop/preview
void cSampleEditor::showPreviewLabel()
{
	display.setControlText(label[6], "Preview");
	display.refreshControl(label[6]);
}
void cSampleEditor::showPlayLabel()
{
	display.setControlText(label[6], "Play");
	display.refreshControl(label[6]);
}
void cSampleEditor::showStopLabel()
{
	display.setControlText(label[6], "Stop");
	display.refreshControl(label[6]);
}
