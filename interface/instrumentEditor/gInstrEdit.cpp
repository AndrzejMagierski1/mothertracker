

#include "instrumentEditor/instrumentEditor.h"


static uint16_t framesPlaces[8][4] =
{
	{0+1, 		  29, 800/8-3, 391},
	{(800/8)*1+1, 29, 800/8-3, 391},
	{(800/8)*2+1, 29, 800/8-3, 391},
	{(800/8)*3+1, 29, 800/8-3, 391},
	{(800/8)*4+1, 29, 800/8-3, 391},
	{(800/8)*5+1, 29, 800/8-3, 391},
	{(800/8)*6+1, 29, 800/8-3, 391},
	{(800/8)*7+1, 29, 800/8-3, 391},
};

static uint16_t framesPlacesAutomation[7][4] =
{
	{0+1, 		  29, 2*(800/8)-3, 391},
	{(800/8)*2+1, 29, 800/8-3, 391},
	{(800/8)*3+1, 29, 800/8-3, 391},
	{(800/8)*4+1, 29, 800/8-3, 391},
	{(800/8)*5+1, 29, 800/8-3, 391},
	{(800/8)*6+1, 29, 800/8-3, 391},
	{(800/8)*7+1, 29, 800/8-3, 391},
};

static uint32_t localActiveBarColors[] =
{
	0xFFFFFF, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
};

static uint32_t localInactiveBarColors[] =
{
	0x222222, // kolor glowny
	0xff0000, // kolor dodatkowy 1 czer
	0x00ff00, // kolor dodatkowy 2 ziel
	0x080808, // kontener
	0x0a0a0a, // tlo
};





void cInstrumentEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleCenterY | controlStyleFont4);
	prop2.x = 9;
	prop2.y = 13;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY | controlStyleFont4);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 2;
	prop2.y = 0;
	prop2.w = 795;
	prop2.h = 26;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);


	strControlProperties prop;
	// ramka
	//strControlProperties prop;
	frameData.placesCount = 8;
	frameData.startPlace = 0;
	frameData.places[0] = &framesPlaces[0][0];
	frameData.places[1] = &framesPlaces[1][0];
	frameData.places[2] = &framesPlaces[2][0];
	frameData.places[3] = &framesPlaces[3][0];
	frameData.places[4] = &framesPlaces[4][0];
	frameData.places[5] = &framesPlaces[5][0];
	frameData.places[6] = &framesPlaces[6][0];
	frameData.places[7] = &framesPlaces[7][0];
	prop.style = 0;
	prop.value = 0;
	prop.colors = nullptr;
	prop.data  = &frameData;
	if(frameControl == nullptr)  frameControl = display.createControl<cFrame>(&prop);

	// inicjalizacja kontrolek

	labelArrow.bitmaps[0].bitmapIndex = displayArrowU;
	labelArrow.bitmaps[0].xValue =  (800/8)*0+(800/16);
	labelArrow.bitmaps[0].yValue = 460;
	labelArrow.bitmaps[1].bitmapIndex = displayArrowD;
	labelArrow.bitmaps[1].xValue =  (800/8)*1+(800/16);
	labelArrow.bitmaps[1].yValue = 460;

	strControlProperties prop3;
	prop3.colors = interfaceGlobals.activeLabelsColors;
	prop3.style = 	( controlStyleCenterX | controlStyleFont3 | controlStyleShowBitmap );
	prop3.x = (800/8)*(0)+(800/16)*2;
	prop3.w = 2 * (800/8) - 6;
	prop3.y = 424;
	prop3.h =  55;
	prop3.value =  1;
	prop3.data = &labelArrow;

	if(doubleLabelAutomation == nullptr) doubleLabelAutomation = display.createControl<cLabel>(&prop3);

	for(uint8_t i = 0; i<8; i++)
	{
		//prop2.data =  &bottomValuesConfig;
		prop2.colors = interfaceGlobals.activeLabelsColors;
		prop2.style = 	( controlStyleCenterX | controlStyleFont3 );
		prop2.x = (800/8)*i+(800/16);
		prop2.w = 800/8-6;
		prop2.y = 424;
		prop2.h =  55;
		prop2.value =  1;

		if(label[i] == nullptr) label[i] = display.createControl<cLabel>(&prop2);

		prop2.colors = localActiveBarColors;
		prop2.x = (800/8)*i+1;
		prop2.y = 29;
		prop2.w = 800/8-3;
		prop2.style =  controlStyleValue_0_100 | controlStyleBackground;
		prop2.h = 394;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);
	}

	prop2.text = nullptr;
	prop2.colors = interfaceGlobals.activeBgLabelsColors;
	prop2.value = 255;
	prop2.style = controlStyleNoTransparency | controlStyleShow;
	prop2.x = 0;
	prop2.w = 800;
	prop2.y = 424;
	prop2.h =  55;
	if(bgLabel == nullptr) bgLabel = display.createControl<cBgLabel>(&prop2);

	filterModeList.linesCount = 5;
	filterModeList.start = 0;
	filterModeList.length = filterModeCount;
	filterModeList.data = (char**)filterModeNames;

	prop.style = controlStyleBackground;
	prop.x = (800/8)*(4)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &filterModeList;
	if(filterModeListControl == nullptr)  filterModeListControl = display.createControl<cList>(&prop);


	envelopesList.linesCount = 5;
	envelopesList.start = 0;
	envelopesList.length = 5;
	envelopesList.data = (char**)envelopeNames;
	prop.x = (800/8)*(0)+1;
	prop.y = 29;
	prop.w = 2 * (800/8)-3;
	prop.h = 394;
	prop.data = &envelopesList;
	if(envelopesListControl == nullptr)  envelopesListControl = display.createControl<cList>(&prop);

	envStateList.linesCount = 3;
	envStateList.start = 0;
	envStateList.length = 3;
	envStateList.data = (char**)envStateNames;
	prop.x = (800/8)*(2)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &envStateList;
	if(envStateListControl == nullptr)  envStateListControl = display.createControl<cList>(&prop);



	lfoShapeList.linesCount = lfoShapeMax;
	lfoShapeList.start = 0;
	lfoShapeList.length = lfoShapeMax;
	lfoShapeList.data = (char**)lfoShapeNames;
	prop.x = (800/8)*(3)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &lfoShapeList;
	if(shapeListControl == nullptr)  shapeListControl = display.createControl<cList>(&prop);


	lfoSpeedList.linesCount = 14;
	lfoSpeedList.start = 0;
	lfoSpeedList.length = 20;
	lfoSpeedList.data = (char**)lfoSpeedNames;
	prop.x = (800/8)*(4)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &lfoSpeedList;
	if(speedListControl == nullptr)  speedListControl = display.createControl<cList>(&prop);

//	envLoopList.linesCount = 5;
//	envLoopList.start = editorInstrument->envelope[selectedEnvelope].loop;
//	envLoopList.length = 2;
//	envLoopList.data = (char**)envLoopNames;
//	prop.x = (800/8)*(7)+1;
//	prop.y = 29;
//	prop.w = 800/8-3;
//	prop.h = 394;
//	prop.data = &envLoopList;
//	if(envLoopListControl == nullptr)  envLoopListControl = display.createControl<cList>(&prop);
}


void cInstrumentEditor::destroyDisplayControls()
{
	display.destroyControl(titleBar);
	titleBar = nullptr;

	display.destroyControl(titleLabel);
	titleLabel = nullptr;

	display.destroyControl(instrumentLabel);
	instrumentLabel = nullptr;

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(label[i]);
		label[i] = nullptr;

		display.destroyControl(barControl[i]);
		barControl[i] = nullptr;
	}

	display.destroyControl(bgLabel);
	bgLabel = nullptr;


	display.destroyControl(filterModeListControl);
	filterModeListControl = nullptr;

	display.destroyControl(envelopesListControl);
	envelopesListControl = nullptr;

	display.destroyControl(envStateListControl);
	envStateListControl = nullptr;

//	display.destroyControl(envLoopListControl);
//	envLoopListControl = nullptr;

	display.destroyControl(speedListControl);
	speedListControl = nullptr;

	display.destroyControl(shapeListControl);
	shapeListControl = nullptr;

	display.destroyControl(doubleLabelAutomation);
	doubleLabelAutomation = nullptr;

	display.destroyControl(frameControl);
	frameControl = nullptr;


}


void cInstrumentEditor::showTitleBar()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	if(mode == mtInstEditModeParams)	display.setControlText(titleLabel, "Instrument Parameters 1/2");
	else display.setControlText(titleLabel, "Instrument Automation 2/2");
	display.refreshControl(titleLabel);

	showActualInstrument();
}


void cInstrumentEditor::showInstrumentEnv()
{
	showTitleBar();
	refreshFramePosition();

	for ( uint8_t i = 0; i < 8; i++ )
	{
		display.setControlColors(label[i],interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[i],localActiveBarColors);
	}

	//**********************************   WSPOLNE DLA LFO I ENV
	display.setControlHide(label[0]);
	display.setControlHide(label[1]);

	display.setControlText(doubleLabelAutomation,"Destination");
	display.setControlShow(doubleLabelAutomation);


	display.setControlText(label[2], "Type");


	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setControlText2(label[4], "");
	display.setControlText2(label[5], "");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");
	display.setControlText2(doubleLabelAutomation,"");


	showEnvList();
	showEnvState();
	display.setControlShow(envelopesListControl);
	display.setControlShow(envStateListControl);

	display.setControlHide(filterModeListControl);

	//**********************************
	if(!editorInstrument->envelope[selectedEnvelope].loop)
	{
		display.setControlText(label[3], "Attack");
		display.setControlText(label[4], "Decay");
		display.setControlText(label[5], "Sustain");
		display.setControlText(label[6], "Release");
		display.setControlText(label[7], "Amount");

		display.setControlValue(bgLabel, 252);
		display.refreshControl(bgLabel);

		display.setControlHide(barControl[0]);
		display.setControlHide(barControl[1]);
		display.setControlHide(barControl[2]);
		display.setControlShow(barControl[3]);
		display.setControlShow(barControl[4]);
		display.setControlShow(barControl[5]);
		display.setControlShow(barControl[6]);
		display.setControlShow(barControl[7]);

		display.setControlStyle(barControl[3], (controlStyleShow | controlStyleValue_0_100 | controlStyleBackground));

		showEnvAttack();
		showEnvDecay();
		showEnvSustain();
		showEnvRelease();
		showEnvAmount();

		display.setControlHide(shapeListControl);
		display.setControlHide(speedListControl);
	}
	else
	{
		display.setControlText(label[3], "Shape");
		display.setControlText(label[4], "Speed");
		display.setControlText(label[5], "Amount");
		display.setControlText(label[6], "");
		display.setControlText(label[7], "");

		display.setControlValue(bgLabel, 124);
		display.refreshControl(bgLabel);

		display.setControlHide(barControl[0]);
		display.setControlHide(barControl[1]);
		display.setControlHide(barControl[2]);
		display.setControlHide(barControl[3]);
		display.setControlHide(barControl[4]);
		display.setControlShow(barControl[5]);
		display.setControlHide(barControl[6]);
		display.setControlHide(barControl[7]);

		showLfoShape();
		showLfoSpeed();
		showLfoAmount();

		display.setControlShow(shapeListControl);
		display.setControlShow(speedListControl);
	}


	//display.setControlStyle(barControl[1], (controlStyleShow | controlStyleValue_0_100));
//	display.setControlStyle(barControl[2], (controlStyleShow | controlStyleValue_0_100 | controlStyleBackground));


//**********************************   WSPOLNE DLA LFO I ENV


	display.setControlStyle2(doubleLabelAutomation, controlStyleCenterX | controlStyleFont2);
	display.setControlShow(doubleLabelAutomation);
	display.refreshControl(doubleLabelAutomation);

	display.refreshControl(shapeListControl);
	display.refreshControl(speedListControl);

	for(uint8_t i = 2; i<8; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
		display.refreshControl(barControl[i]);
	}
	display.refreshControl(label[0]);
	display.refreshControl(label[1]);
	display.refreshControl(barControl[0]);
	display.refreshControl(barControl[1]);


	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);


	frameData.placesCount = 7;

	display.synchronizeRefresh();
//**********************************
}


void cInstrumentEditor::showInstrumentParams()
{
	showTitleBar();

	refreshFramePosition();

	for ( uint8_t i = 0; i < 8; i++ )
	{
		display.setControlColors(label[i],interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[i],localActiveBarColors);
	}

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 1);
	display.setControlValue(label[3], 1);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 1);
	display.setControlValue(label[7], 1);

	display.setControlShow(label[0]);
	display.setControlShow(label[1]);
	display.setControlHide(doubleLabelAutomation);

	display.setControlText(label[0], "Volume");
	display.setControlText(label[1], "Panning");
	display.setControlText(label[2], "Tune");
	display.setControlText(label[3], "Finetune");
	display.setControlText(label[4], "Filter Type");
	display.setControlText(label[5], "Cutoff");
	display.setControlText(label[6], "Resonance");
	display.setControlText(label[7], "Rev. Send");

	display.setControlText2(label[0], "");
	display.setControlText2(label[1], "");
	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setControlText2(label[4], "");
	display.setControlText2(label[5], "");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");


	showParamsVolume();
	showParamsPanning();
	showParamsTune();
	showParamsFineTune();
	showFilterType();
	showFilterCutOff();
	showFilterResonance();
	showParamsReverbSend();

//-------------------------------------

	display.setControlShow(barControl[0]);
	display.setControlShow(barControl[1]);
	display.setControlShow(barControl[2]);
	display.setControlShow(barControl[3]);
	display.setControlHide(barControl[4]);
	display.setControlShow(barControl[5]);
	display.setControlShow(barControl[6]);
	display.setControlShow(barControl[7]);


	display.setControlHide(envelopesListControl);
	display.setControlHide(envStateListControl);
	display.setControlHide(shapeListControl);
	display.setControlHide(speedListControl);
//	display.setControlHide(envLoopListControl);
	display.setControlShow(filterModeListControl);
	display.setControlHide(doubleLabelAutomation);




	display.setControlStyle(barControl[1], (controlStyleShow | controlStyleValueLeftRight_100_100 | controlStyleBackground));
	display.setControlStyle(barControl[2], (controlStyleShow | controlStyleValueLeftRight_100_100 | controlStyleBackground));
	display.setControlStyle(barControl[3], (controlStyleShow | controlStyleValueLeftRight_100_100 | controlStyleBackground));

//-------------------------------------


	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlStyle2(label[i], controlStyleCenterX | controlStyleFont2);
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
		display.refreshControl(barControl[i]);
	}

	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);


	frameData.placesCount = 8;

	display.synchronizeRefresh();
}


void cInstrumentEditor::showInstrumentMidiParams()
{
	showTitleBar();

	display.setControlValue(label[0], 1);
	display.setControlValue(label[1], 1);
	display.setControlValue(label[2], 1);
	display.setControlValue(label[3], 1);
	display.setControlValue(label[4], 1);
	display.setControlValue(label[5], 1);
	display.setControlValue(label[6], 1);
	display.setControlValue(label[7], 1);

	display.setControlText(label[0], "Velocity");
	display.setControlText(label[1], "");
	display.setControlText(label[2], "");
	display.setControlText(label[3], "");
	display.setControlText(label[4], "");
	display.setControlText(label[5], "");
	display.setControlText(label[6], "");
	display.setControlText(label[7], "");

	display.setControlText2(label[0], "");
	display.setControlText2(label[1], "");
	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setControlText2(label[4], "");
	display.setControlText2(label[5], "");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");


	showParamsVelocity();

//-------------------------------------

	display.setControlShow(barControl[0]);
	display.setControlHide(barControl[1]);
	display.setControlHide(barControl[2]);
	display.setControlHide(barControl[3]);
	display.setControlHide(barControl[4]);
	display.setControlHide(barControl[5]);
	display.setControlHide(barControl[6]);
	display.setControlHide(barControl[7]);

	display.setControlHide(envelopesListControl);
	display.setControlHide(envStateListControl);
	display.setControlHide(shapeListControl);
	display.setControlHide(speedListControl);
//	display.setControlHide(envLoopListControl);
	display.setControlHide(filterModeListControl);
	display.setControlHide(doubleLabelAutomation);

//-------------------------------------

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(label[i]);
		display.refreshControl(label[i]);
		display.refreshControl(barControl[i]);
	}

	display.setControlValue(bgLabel,2);
	display.setControlShow(bgLabel);
	display.refreshControl(bgLabel);

	frameData.placesCount = 1;

	display.synchronizeRefresh();
}

//==============================================================================================================
void cInstrumentEditor::activateLabelsBorder()
{
	if(selectedPlace[mode] > frameData.placesCount-1) return;

	display.setControlValue(frameControl, selectedPlace[mode]);
	display.setControlShow(frameControl);
	display.refreshControl(frameControl);
}

//==============================================================================================================


void cInstrumentEditor::showEnvList()
{

	display.setControlValue(envelopesListControl, selectedEnvelope);
	display.refreshControl(envelopesListControl);
}

void cInstrumentEditor::showEnvState()
{
	uint8_t val = editorInstrument->envelope[selectedEnvelope].enable + editorInstrument->envelope[selectedEnvelope].loop;
	display.setControlText2(label[2], envStateLabels[val]);
	display.refreshControl(label[2]);


	display.setControlValue(envStateListControl, val);
	display.refreshControl(envStateListControl);
}

void cInstrumentEditor::showEnvAttack()
{
	sprintf(envAttack,"%.3fs",(float)(editorInstrument->envelope[selectedEnvelope].attack/1000.0f));

	if(editorInstrument->envelope[selectedEnvelope].enable)
	{
		display.setControlColors(label[3], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[3],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[3], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[3], localInactiveBarColors);
	}

	display.setControlText2(label[3], envAttack);
	display.refreshControl(label[3]);

	display.setControlValue(
							barControl[3], (editorInstrument->envelope[selectedEnvelope].attack > ENVELOPE_MICRO_RANGE) ?
							map(editorInstrument->envelope[selectedEnvelope].attack,ENVELOPE_MICRO_RANGE,ATTACK_MAX,50,100):
							map(editorInstrument->envelope[selectedEnvelope].attack,0,ENVELOPE_MICRO_RANGE,0,50)
						   );
	display.refreshControl(barControl[3]);

}

void cInstrumentEditor::showEnvDecay()
{
	sprintf(envDecay,"%.3fs",(float)(editorInstrument->envelope[selectedEnvelope].decay/1000.0f));

	if(editorInstrument->envelope[selectedEnvelope].enable)
	{
		display.setControlColors(label[4], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[4],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[4], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[4],localInactiveBarColors );
	}

	display.setControlText2(label[4], envDecay);
	display.refreshControl(label[4]);

	display.setControlValue(
							barControl[4], (editorInstrument->envelope[selectedEnvelope].decay > ENVELOPE_MICRO_RANGE) ?
							map(editorInstrument->envelope[selectedEnvelope].decay,ENVELOPE_MICRO_RANGE,DECAY_MAX,50,100):
							map(editorInstrument->envelope[selectedEnvelope].decay,0,ENVELOPE_MICRO_RANGE,0,50)
						   );
	display.refreshControl(barControl[4]);
}

void cInstrumentEditor::showEnvSustain()
{
	sprintf(envSustain,"%.0f",(float)(editorInstrument->envelope[selectedEnvelope].sustain*100));

	if(editorInstrument->envelope[selectedEnvelope].enable)
	{
		display.setControlColors(label[5], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[5],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[5], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[5],localInactiveBarColors );
	}

	display.setControlText2(label[5], envSustain);
	display.refreshControl(label[5]);

	display.setControlValue(barControl[5], (editorInstrument->envelope[selectedEnvelope].sustain*100));
	display.refreshControl(barControl[5]);
}

void cInstrumentEditor::showEnvRelease()
{
	sprintf(envRelease,"%.3fs",(float)(editorInstrument->envelope[selectedEnvelope].release/1000.0f));

	if(editorInstrument->envelope[selectedEnvelope].enable)
	{
		display.setControlColors(label[6], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[6],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[6], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[6],localInactiveBarColors );
	}

	display.setControlText2(label[6], envRelease);
	display.refreshControl(label[6]);

	display.setControlValue(
							barControl[6], (editorInstrument->envelope[selectedEnvelope].release > ENVELOPE_MICRO_RANGE) ?
							map(editorInstrument->envelope[selectedEnvelope].release,ENVELOPE_MICRO_RANGE,RELEASE_MAX,50,100):
							map(editorInstrument->envelope[selectedEnvelope].release,0,ENVELOPE_MICRO_RANGE,0,50)
						   );
	display.refreshControl(barControl[6]);
}

void cInstrumentEditor::showEnvAmount()
{
	sprintf(envAmount,"%.0f",(float)(editorInstrument->envelope[selectedEnvelope].amount*100));

	if(editorInstrument->envelope[selectedEnvelope].enable)
	{
		display.setControlColors(label[7], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[7],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[7], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[7],localInactiveBarColors );
	}

	display.setControlText2(label[7], envAmount);
	display.refreshControl(label[7]);

	display.setControlValue(barControl[7],(editorInstrument->envelope[selectedEnvelope].amount*100) );
	display.refreshControl(barControl[7]);
}

//void cInstrumentEditor::showEnvLoop()
//{
//	display.setControlText2(label[7], envLoopLabels[!editorInstrument->envelope[selectedEnvelope].loop]);
//	display.refreshControl(label[7]);
//
//	display.setControlValue(envLoopListControl, !editorInstrument->envelope[selectedEnvelope].loop);
//	display.refreshControl(envLoopListControl);
//}

void cInstrumentEditor::showLfoShape()
{
	display.setControlText2(label[3], lfoShapeLabels[editorInstrument->lfo[selectedEnvelope].shape]);
	display.refreshControl(label[3]);

	display.setControlValue(shapeListControl, editorInstrument->lfo[selectedEnvelope].shape);
	display.refreshControl(shapeListControl);
}
void cInstrumentEditor::showLfoSpeed()
{
	display.setControlText2(label[4], lfoSpeedLabels[editorInstrument->lfo[selectedEnvelope].speed]);
	display.refreshControl(label[4]);

	display.setControlValue(speedListControl, editorInstrument->lfo[selectedEnvelope].speed);
	display.refreshControl(speedListControl);
}
void cInstrumentEditor::showLfoAmount()
{
	sprintf(lfoAmount,"%.0f",(float)(editorInstrument->lfo[selectedEnvelope].amount*100));
	display.setControlText2(label[5], lfoAmount);
	display.refreshControl(label[5]);

	display.setControlValue(barControl[5],(editorInstrument->lfo[selectedEnvelope].amount*100) );
	display.refreshControl(barControl[5]);
}


void cInstrumentEditor::showParamsVelocity()
{
	if(mtProject.values.lastUsedInstrument < INSTRUMENTS_COUNT)
	{
		sprintf(volumeVal,"%d", editorInstrument->volume);

		display.setControlValue(barControl[0], editorInstrument->volume);
	}
	else
	{
		uint8_t temp_velocity =  mtProject.values.midiInstrument[mtProject.values.lastUsedInstrument-INSTRUMENTS_COUNT].velocity;
		sprintf(volumeVal,"%d",temp_velocity);

		display.setControlValue(barControl[0], (temp_velocity*100)/127);
	}

	display.setControlText2(label[0], volumeVal);

	display.refreshControl(label[0]);
	display.refreshControl(barControl[0]);
}

void cInstrumentEditor::showParamsVolume()
{
	sprintf(volumeVal,"%d",editorInstrument->volume);
	display.setControlText2(label[0], volumeVal);
	display.refreshControl(label[0]);

	display.setControlValue(barControl[0], editorInstrument->volume);
	display.refreshControl(barControl[0]);
}

void cInstrumentEditor::showParamsPanning()
{
	sprintf(panningVal,"%d", editorInstrument->panning-50);

	display.setControlText2(label[1], panningVal);
	display.refreshControl(label[1]);

	display.setControlValue(barControl[1], editorInstrument->panning*2-100);
	display.refreshControl(barControl[1]);
}

void cInstrumentEditor::showParamsTune()
{
	sprintf(tuneVal,"%d",editorInstrument->tune);

	display.setControlText2(label[2], tuneVal);
	display.refreshControl(label[2]);

	display.setControlValue(barControl[2], (editorInstrument->tune*100)/24);
	display.refreshControl(barControl[2]);
}

void cInstrumentEditor::showParamsFineTune()
{
	sprintf(fineTuneVal,"%d",editorInstrument->fineTune);
	display.setControlText2(label[3], fineTuneVal);
	display.refreshControl(label[3]);

	display.setControlValue(barControl[3], editorInstrument->fineTune);
	display.refreshControl(barControl[3]);
}

void cInstrumentEditor::showFilterType()
{
	if(editorInstrument->filterEnable)
	{
		if(editorInstrument->filterType == lowPass) filterModeListPos = 1;
		else if(editorInstrument->filterType == highPass) filterModeListPos = 2;
		else filterModeListPos = 3;
	}
	else
	{
		filterModeListPos = 0;
	}

	filterModeList.start = filterModeListPos;
//	filterModeList.length = filterModeCount;
//	filterModeList.linesCount = 5;
//	filterModeList.data = (char**)filterModeNames;

	display.setControlText2(label[4], filterModeFunctLabels[filterModeListPos]);
	display.refreshControl(label[4]);

	display.setControlData(filterModeListControl,  &filterModeList);
	//display.setControlValue(IE->filterModeListControl, IE->filterModeListPos);
	display.setControlShow(filterModeListControl);
	display.refreshControl(filterModeListControl);

	showFilterCutOff();
	showFilterResonance();
}

void cInstrumentEditor::showFilterCutOff()
{
	uint8_t temp_cutoff = (editorInstrument->cutOff*100);

	sprintf(cutoffVal,"%d", temp_cutoff);

	if(editorInstrument->filterEnable)
	{
		display.setControlColors(label[5], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[5],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[5], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[5], localInactiveBarColors);
	}

	display.setControlText2(label[5], cutoffVal);
	display.refreshControl(label[5]);

	display.setControlValue(barControl[5], temp_cutoff);
	display.refreshControl(barControl[5]);
}

void cInstrumentEditor::showFilterResonance()
{
	uint8_t temp_resonance = ((editorInstrument->resonance - RESONANCE_MIN)/(RESONANCE_MAX-RESONANCE_MIN))*100;

	sprintf(resonanceVal,"%d", temp_resonance);

	if(editorInstrument->filterEnable)
	{
		display.setControlColors(label[6], interfaceGlobals.activeLabelsColors);
		display.setControlColors(barControl[6],localActiveBarColors );
	}
	else
	{
		display.setControlColors(label[6], interfaceGlobals.inactiveLabelsColors);
		display.setControlColors(barControl[6], localInactiveBarColors);
	}


	display.setControlText2(label[6], resonanceVal);
	display.refreshControl(label[6]);

	display.setControlValue(barControl[6], temp_resonance);
	display.refreshControl(barControl[6]);
}

void cInstrumentEditor::showParamsReverbSend()
{
	sprintf(revSendVal,"%d",editorInstrument->reverbSend);
	display.setControlText2(label[7], revSendVal);
	display.refreshControl(label[7]);

	display.setControlValue(barControl[7], editorInstrument->reverbSend);
	display.refreshControl(barControl[7]);
}



void cInstrumentEditor::showParamsGlide()
{
	display.setControlValue(barControl[3], (editorInstrument->glide*100)/GLIDE_MAX);
	display.refreshControl(barControl[3]);
}

void cInstrumentEditor::refreshFramePosition()
{
	if(mode == mtInstEditModeParams)
	{
		frameData.placesCount = 8;
		frameData.startPlace = 0;
		frameData.places[0] = &framesPlaces[0][0];
		frameData.places[1] = &framesPlaces[1][0];
		frameData.places[2] = &framesPlaces[2][0];
		frameData.places[3] = &framesPlaces[3][0];
		frameData.places[4] = &framesPlaces[4][0];
		frameData.places[5] = &framesPlaces[5][0];
		frameData.places[6] = &framesPlaces[6][0];
		frameData.places[7] = &framesPlaces[7][0];
	}
	else if( mode == mtInstEditModeEnv)
	{
		frameData.placesCount = 7;
		frameData.startPlace = 0;
		frameData.places[0] = &framesPlacesAutomation[0][0];
		frameData.places[1] = &framesPlacesAutomation[1][0];
		frameData.places[2] = &framesPlacesAutomation[2][0];
		frameData.places[3] = &framesPlacesAutomation[3][0];
		frameData.places[4] = &framesPlacesAutomation[4][0];
		frameData.places[5] = &framesPlacesAutomation[5][0];
		frameData.places[6] = &framesPlacesAutomation[6][0];
	}
}




void cInstrumentEditor::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	if(i < INSTRUMENTS_COUNT)
	{
		sprintf(actualInstrName, "%d. ", i+1);
		strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);
	}
	else
	{
		//i = i-(INSTRUMENTS_COUNT-1);
		sprintf(actualInstrName, "%d. MIDI Channel %d", i+3, i-(INSTRUMENTS_COUNT-1));
	}

	display.setControlShow(instrumentLabel);
	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}





