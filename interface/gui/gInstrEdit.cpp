

#include <instrumentEditor.h>


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
	filterModeList.start = editorInstrument->filterEnable ? (editorInstrument->filterType+1) : 0;
	filterModeList.length = filterModeCount;
	filterModeList.data = (char**)filterModeNames;

	prop.style = controlStyleBackground;
	prop.x = (800/8)*(4)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &filterModeList;
	if(filterModeListControl == nullptr)  filterModeListControl = display.createControl<cList>(&prop);


	envelopesList.linesCount = 6;
	envelopesList.start = selectedEnvelope;
	envelopesList.length = 3;
	envelopesList.data = (char**)envelopeNames;
	prop.x = (800/8)*(0)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &envelopesList;
	if(envelopesListControl == nullptr)  envelopesListControl = display.createControl<cList>(&prop);


	envStateList.linesCount = 5;
	envStateList.start = !editorInstrument->envelope[selectedEnvelope].enable;
	envStateList.length = 2;
	envStateList.data = (char**)envStateNames;
	prop.x = (800/8)*(1)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &envStateList;
	if(envStateListControl == nullptr)  envStateListControl = display.createControl<cList>(&prop);


	envLoopList.linesCount = 5;
	envLoopList.start = editorInstrument->envelope[selectedEnvelope].loop;
	envLoopList.length = 2;
	envLoopList.data = (char**)envLoopNames;
	prop.x = (800/8)*(7)+1;
	prop.y = 29;
	prop.w = 800/8-3;
	prop.h = 394;
	prop.data = &envLoopList;
	if(envLoopListControl == nullptr)  envLoopListControl = display.createControl<cList>(&prop);
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

	display.destroyControl(envLoopListControl);
	envLoopListControl = nullptr;


	display.destroyControl(frameControl);
	frameControl = nullptr;


}


void cInstrumentEditor::showTitleBar()
{
	display.setControlShow(titleBar);
	display.refreshControl(titleBar);

	display.setControlShow(titleLabel);
	if(mode == mtInstEditModeParams)	display.setControlText(titleLabel, "Instrument Parameters 1/2");
	else display.setControlText(titleLabel, "Instrument Envelopes 2/2");
	display.refreshControl(titleLabel);

	showActualInstrument();
}


void cInstrumentEditor::showInstrumentEnv()
{
	showTitleBar();


	display.setControlText(label[0], "Envelopes");
	display.setControlText(label[1], "State");
	display.setControlText(label[2], "Attack");
	display.setControlText(label[3], "Decay");
	display.setControlText(label[4], "Sustain");
	display.setControlText(label[5], "Release");
	display.setControlText(label[6], "Amount");
	display.setControlText(label[7], "Loop");

	display.setControlText2(label[0], "");
	display.setControlText2(label[1], "");
	display.setControlText2(label[2], "");
	display.setControlText2(label[3], "");
	display.setControlText2(label[4], "");
	display.setControlText2(label[5], "");
	display.setControlText2(label[6], "");
	display.setControlText2(label[7], "");


	showEnvList();
	showEnvState();
	showEnvAttack();
	showEnvDecay();
	showEnvSustain();
	showEnvRelease();
	showEnvAmount();
	showEnvLoop();

//-------------------------------------

	display.setControlHide(barControl[0]);
	display.setControlHide(barControl[1]);
	display.setControlShow(barControl[2]);
	display.setControlShow(barControl[3]);
	display.setControlShow(barControl[4]);
	display.setControlShow(barControl[5]);
	display.setControlShow(barControl[6]);
	display.setControlHide(barControl[7]);


	display.setControlHide(filterModeListControl);
	display.setControlShow(envelopesListControl);
	display.setControlShow(envStateListControl);
	display.setControlShow(envLoopListControl);


	//display.setControlStyle(barControl[1], (controlStyleShow | controlStyleValue_0_100));
	display.setControlStyle(barControl[2], (controlStyleShow | controlStyleValue_0_100 | controlStyleBackground));
	display.setControlStyle(barControl[3], (controlStyleShow | controlStyleValue_0_100 | controlStyleBackground));


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


void cInstrumentEditor::showInstrumentParams()
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
	display.setControlHide(envLoopListControl);
	display.setControlShow(filterModeListControl);




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
	display.setControlHide(envLoopListControl);
	display.setControlHide(filterModeListControl);

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
	display.setControlText2(label[0], envelopesLabels[selectedEnvelope]);
	display.refreshControl(label[0]);

	display.setControlValue(envelopesListControl, selectedEnvelope);
	display.refreshControl(envelopesListControl);
}

void cInstrumentEditor::showEnvState()
{
	uint8_t val;
	display.setControlText2(label[1], envStateLabels[!editorInstrument->envelope[selectedEnvelope].enable]);
	display.refreshControl(label[1]);

	if(editorInstrument->envelope[selectedEnvelope].enable)
	{
		val = 0;
	}
	else
	{
		val = 1;
	}

	display.setControlValue(envStateListControl, val);
	display.refreshControl(envStateListControl);
}

void cInstrumentEditor::showEnvAttack()
{
	uint8_t length;

	sprintf(envAttack,"%.3f",(float)(editorInstrument->envelope[selectedEnvelope].attack/1000.0f));
	length=strlen(envAttack);
	envAttack[length]='s';
	envAttack[length+1]=0;

	display.setControlText2(label[2], envAttack);
	display.refreshControl(label[2]);

	display.setControlValue(
							barControl[2], (editorInstrument->envelope[selectedEnvelope].attack > ENVELOPE_MICRO_RANGE) ?
							map(editorInstrument->envelope[selectedEnvelope].attack,ENVELOPE_MICRO_RANGE,ATTACK_MAX,50,100):
							map(editorInstrument->envelope[selectedEnvelope].attack,0,ENVELOPE_MICRO_RANGE,0,50)
						   );
	display.refreshControl(barControl[2]);

}

void cInstrumentEditor::showEnvDecay()
{
	uint8_t length;

	sprintf(envDecay,"%.3f",(float)(editorInstrument->envelope[selectedEnvelope].decay/1000.0f));
	length=strlen(envDecay);
	envDecay[length]='s';
	envDecay[length+1]=0;

	display.setControlText2(label[3], envDecay);
	display.refreshControl(label[3]);

	display.setControlValue(
							barControl[3], (editorInstrument->envelope[selectedEnvelope].decay > ENVELOPE_MICRO_RANGE) ?
							map(editorInstrument->envelope[selectedEnvelope].decay,ENVELOPE_MICRO_RANGE,DECAY_MAX,50,100):
							map(editorInstrument->envelope[selectedEnvelope].decay,0,ENVELOPE_MICRO_RANGE,0,50)
						   );
	display.refreshControl(barControl[3]);
}

void cInstrumentEditor::showEnvSustain()
{
	sprintf(envSustain,"%.0f",(float)(editorInstrument->envelope[selectedEnvelope].sustain*100));
	display.setControlText2(label[4], envSustain);
	display.refreshControl(label[4]);

	display.setControlValue(barControl[4], (editorInstrument->envelope[selectedEnvelope].sustain*100));
	display.refreshControl(barControl[4]);
}

void cInstrumentEditor::showEnvRelease()
{
	uint8_t length;

	sprintf(envRelease,"%.3f",(float)(editorInstrument->envelope[selectedEnvelope].release/1000.0f));
	length=strlen(envRelease);
	envRelease[length]='s';
	envRelease[length+1]=0;

	display.setControlText2(label[5], envRelease);
	display.refreshControl(label[5]);

	display.setControlValue(
							barControl[5], (editorInstrument->envelope[selectedEnvelope].release > ENVELOPE_MICRO_RANGE) ?
							map(editorInstrument->envelope[selectedEnvelope].release,ENVELOPE_MICRO_RANGE,RELEASE_MAX,50,100):
							map(editorInstrument->envelope[selectedEnvelope].release,0,ENVELOPE_MICRO_RANGE,0,50)
						   );
	display.refreshControl(barControl[5]);
}

void cInstrumentEditor::showEnvAmount()
{
	sprintf(envAmount,"%.0f",(float)(editorInstrument->envelope[selectedEnvelope].amount*100));
	display.setControlText2(label[6], envAmount);
	display.refreshControl(label[6]);

	display.setControlValue(barControl[6],(editorInstrument->envelope[selectedEnvelope].amount*100) );
	display.refreshControl(barControl[6]);
}

void cInstrumentEditor::showEnvLoop()
{
	display.setControlText2(label[7], envLoopLabels[!editorInstrument->envelope[selectedEnvelope].loop]);
	display.refreshControl(label[7]);

	display.setControlValue(envLoopListControl, !editorInstrument->envelope[selectedEnvelope].loop);
	display.refreshControl(envLoopListControl);
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
}

void cInstrumentEditor::showFilterCutOff()
{
	uint8_t temp_cutoff = (editorInstrument->cutOff*100);

	sprintf(cutoffVal,"%d", temp_cutoff);
	display.setControlText2(label[5], cutoffVal);
	display.refreshControl(label[5]);

	display.setControlValue(barControl[5], temp_cutoff);
	display.refreshControl(barControl[5]);
}

void cInstrumentEditor::showFilterResonance()
{
	uint8_t temp_resonance = ((editorInstrument->resonance - RESONANCE_MIN)/(RESONANCE_MAX-RESONANCE_MIN))*100;

	sprintf(resonanceVal,"%d", temp_resonance);
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





