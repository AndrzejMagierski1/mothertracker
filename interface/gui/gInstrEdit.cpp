



#include <instrumentEditor.h>


static uint16_t framesPlaces[8][4] =
{
	{0, 		30, 800/8, 380},
	{(800/8)*1, 30, 800/8, 380},
	{(800/8)*2, 30, 800/8, 380},
	{(800/8)*3, 30, 800/8, 380},
	{(800/8)*4, 30, 800/8, 380},
	{(800/8)*5, 30, 800/8, 380},
	{(800/8)*6, 30, 800/8, 380},
	{(800/8)*7, 30, 800/8, 380},
};

static uint32_t instrListColors[] =
{
	0xFF0000,	//	 listItemFrame
	0x000000,	//	 listItemFrameBG
	0xFFFFFF,	//	 listScrollBar
	0x554A19,	//	 listBG
	0xFFFFFF,	//	 fontList
};


void cInstrumentEditor::initDisplayControls()
{
	strControlProperties prop2;
	prop2.style = 	( controlStyleShow | controlStyleBackground);
	prop2.x = 0;
	prop2.y = 0;
	prop2.w = 800;
	prop2.h = 25;
	if(titleBar == nullptr) titleBar = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleCenterY);
	prop2.x = 30;
	prop2.y = 12;
	if(titleLabel == nullptr) titleLabel = display.createControl<cLabel>(&prop2);
	prop2.style = 	( controlStyleShow | controlStyleRightX | controlStyleCenterY);
	prop2.x = 769;
	if(instrumentLabel == nullptr) instrumentLabel = display.createControl<cLabel>(&prop2);


	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<8; i++)
	{
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX );
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;
		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);


		prop2.y = 415;
		prop2.h = 34;
		if(topLabel[i] == nullptr) topLabel[i] = display.createControl<cLabel>(&prop2);


		prop2.x = (800/8)*i+5;
		prop2.y = 30;
		prop2.w = 800/8-10;
		prop2.style =  controlStyleValue_0_100;
		prop2.h = 380;
		if(barControl[i] == nullptr)  barControl[i] = display.createControl<cBar>(&prop2);
	}

	filterModeList.linesCount = 5;
	filterModeList.start = editorInstrument->filterEnable ? (editorInstrument->filterType+1) : 0;
	filterModeList.length = filterModeCount;
	filterModeList.data = filterModeNames;
	strControlProperties prop;
	prop.x = (800/8)*(4)+5;
	prop.y = 140;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &filterModeList;
	if(filterModeListControl == nullptr)  filterModeListControl = display.createControl<cList>(&prop);


	envelopesList.linesCount = 5;
	envelopesList.start = selectedEnvelope;
	envelopesList.length = 2;
	envelopesList.data = envelopeNames;
	prop.x = (800/8)*(0)+5;
	prop.y = 140;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &envelopesList;
	if(envelopesListControl == nullptr)  envelopesListControl = display.createControl<cList>(&prop);


	envStateList.linesCount = 5;
	envStateList.start = !editorInstrument->envelope[selectedEnvelope].enable;
	envStateList.length = 2;
	envStateList.data = envStateNames;
	prop.x = (800/8)*(1)+5;
	prop.y = 140;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &envStateList;
	if(envStateListControl == nullptr)  envStateListControl = display.createControl<cList>(&prop);


	envLoopList.linesCount = 5;
	envLoopList.start = editorInstrument->envelope[selectedEnvelope].loop;
	envLoopList.length = 2;
	envLoopList.data = envLoopNames;
	prop.x = (800/8)*(7)+5;
	prop.y = 140;
	prop.w = 800/8-10;
	prop.h = 25;
	prop.data = &envLoopList;
	if(envLoopListControl == nullptr)  envLoopListControl = display.createControl<cList>(&prop);

	// inicjalizacja list instrumentow
	for(uint8_t i = 0; i<4; i++)
	{
		intrumentsList[i].start = 0;
		intrumentsList[i].linesCount = 12;
		intrumentsList[i].length = 12;
		intrumentsList[i].data = &ptrintrumentsNames[i*12];
		prop.style = controlStyleCenterY;
		prop.x = (800/4)*(i)+5;
		prop.y = 240;
		prop.w = 800/4-10;
		prop.h = 25;
		prop.colors = instrListColors;
		prop.data = &intrumentsList[i];
		if(intrumentsListControl[i] == nullptr)  intrumentsListControl[i] = display.createControl<cList>(&prop);
	}

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
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;

		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;

		display.destroyControl(barControl[i]);
		barControl[i] = nullptr;
	}

	display.destroyControl(filterModeListControl);
	filterModeListControl = nullptr;

	display.destroyControl(envelopesListControl);
	envelopesListControl = nullptr;

	display.destroyControl(envStateListControl);
	envStateListControl = nullptr;

	display.destroyControl(envLoopListControl);
	envLoopListControl = nullptr;

	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(intrumentsListControl[i]);
		intrumentsListControl[i] = nullptr;
	}


	display.destroyControl(frameControl);
	frameControl = nullptr;
}
/*
void cInstrumentEditor::showDefaultScreen()
{

	// bottom labels
	display.setControlText(bottomLabel[0], "Start");
	display.setControlText(bottomLabel[1], "Loop Start");
	display.setControlText(bottomLabel[2], "Loop End");
	display.setControlText(bottomLabel[3], "End");
	display.setControlText(bottomLabel[4], "- Zoom");
	display.setControlText(bottomLabel[5], "+ Zoom");
	display.setControlText(bottomLabel[6], "Play Mode");
	//display.setControlText(bottomLabel[7], "End");

	display.refreshControl(titleLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(barControl[i]);
	}

	display.synchronizeRefresh();

}
*/


void cInstrumentEditor::showInstrumentEnv()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Instrument Envelopes");
	display.refreshControl(titleLabel);

	showActualInstrument();

	display.setControlText(bottomLabel[0], "Envelopes");
	display.setControlText(bottomLabel[1], "State");
	display.setControlText(bottomLabel[2], "Attack");
	display.setControlText(bottomLabel[3], "Decay");
	display.setControlText(bottomLabel[4], "Sustain");
	display.setControlText(bottomLabel[5], "Release");
	display.setControlText(bottomLabel[6], "Amount");
	display.setControlText(bottomLabel[7], "Loop");


	display.setControlText(topLabel[0], "");
	display.setControlText(topLabel[1], "");
	display.setControlText(topLabel[2], "");
	display.setControlText(topLabel[3], "");
	display.setControlText(topLabel[4], "");
	display.setControlText(topLabel[5], "");
	display.setControlText(topLabel[6], "");
	display.setControlText(topLabel[7], "");


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

	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlHide(intrumentsListControl[i]);
	}


	//display.setControlStyle(barControl[1], (controlStyleShow | controlStyleValue_0_100));
	display.setControlStyle(barControl[2], (controlStyleShow | controlStyleValue_0_100));
	display.setControlStyle(barControl[3], (controlStyleShow | controlStyleValue_0_100));


//-------------------------------------

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(barControl[i]);
	}


	display.synchronizeRefresh();

}


void cInstrumentEditor::showInstrumentParams()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Instrument Parameters");

	showActualInstrument();

	display.setControlText(bottomLabel[0], "Volume");
	display.setControlText(bottomLabel[1], "Panning");
	display.setControlText(bottomLabel[2], "Tune");
	display.setControlText(bottomLabel[3], "Finetune");
	display.setControlText(bottomLabel[4], "Filter Type");
	display.setControlText(bottomLabel[5], "Cutoff");
	display.setControlText(bottomLabel[6], "Resonance");
	display.setControlText(bottomLabel[7], "Rev. Send");


	showParamsVolume();
	showParamsPanning();
	showParamsTune();
	showParamsFineTune();
	showFilterFilterType();
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


	for(uint8_t i = 0; i<4; i++)
	{
		display.setControlHide(intrumentsListControl[i]);
	}


	display.setControlStyle(barControl[1], (controlStyleShow | controlStyleValueLeftRight_100_100));
	display.setControlStyle(barControl[2], (controlStyleShow | controlStyleValueLeftRight_100_100));
	display.setControlStyle(barControl[3], (controlStyleShow | controlStyleValueLeftRight_100_100));

//-------------------------------------

	display.refreshControl(titleLabel);

	for(uint8_t i = 0; i<8; i++)
	{
		display.setControlShow(topLabel[i]);
		display.setControlShow(bottomLabel[i]);
		display.refreshControl(bottomLabel[i]);
		display.refreshControl(topLabel[i]);
		display.refreshControl(barControl[i]);
	}

	display.synchronizeRefresh();
}



void cInstrumentEditor::showInstrumentList()
{
	display.refreshControl(titleBar);

	display.setControlText(titleLabel, "Instruments");
	display.refreshControl(titleLabel);

	showActualInstrument();
//	display.setControlText(bottomLabel[0], "");
//	display.setControlText(bottomLabel[1], "");
//	display.setControlText(bottomLabel[2], "");
//	display.setControlText(bottomLabel[3], "");
//	display.setControlText(bottomLabel[4], "");
//	display.setControlText(bottomLabel[5], "");
//	display.setControlText(bottomLabel[6], "");
//	display.setControlText(bottomLabel[7], "");


	for(uint8_t i = 0; i<8; i++)
	{
		//display.refreshControl(bottomLabel[i]);
		display.setControlHide(barControl[i]);
		display.setControlHide(bottomLabel[i]);
		display.setControlHide(topLabel[i]);
	}

	display.setControlHide(filterModeListControl);
	display.setControlHide(envelopesListControl);
	display.setControlHide(envStateListControl);
	display.setControlHide(envLoopListControl);
	display.setControlHide(frameControl);



	listInstruments();

	for(uint8_t i = 0; i<4; i++)
	{
		if(selectedInstrument >= i*12 && selectedInstrument < (i+1)*12)
		{
			intrumentsList[i].start = selectedInstrument%12;
		}
		else
		{
			intrumentsList[i].start = -1;
		}

		intrumentsList[i].length = 12;
		intrumentsList[i].linesCount = 12;
		intrumentsList[i].data = &ptrintrumentsNames[i*12];

		display.setControlData(intrumentsListControl[i], &intrumentsList[i]);

		display.setControlShow(intrumentsListControl[i]);

		showInstrList(i);
	}

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
	display.setControlValue(envStateListControl, !editorInstrument->envelope[selectedEnvelope].enable);
	display.refreshControl(envStateListControl);
}

void cInstrumentEditor::showEnvAttack()
{
	display.setControlValue(barControl[2], (editorInstrument->envelope[selectedEnvelope].attack*100)/ATTACK_MAX);
	display.refreshControl(barControl[2]);

}

void cInstrumentEditor::showEnvDecay()
{
	display.setControlValue(barControl[3], (editorInstrument->envelope[selectedEnvelope].decay*100)/DECAY_MAX);
	display.refreshControl(barControl[3]);

}

void cInstrumentEditor::showEnvSustain()
{
	display.setControlValue(barControl[4], (editorInstrument->envelope[selectedEnvelope].sustain*100));
	display.refreshControl(barControl[4]);
}

void cInstrumentEditor::showEnvRelease()
{
	display.setControlValue(barControl[5], (editorInstrument->envelope[selectedEnvelope].release*100)/RELEASE_MAX);
	display.refreshControl(barControl[5]);
}

void cInstrumentEditor::showEnvAmount()
{
	display.setControlValue(barControl[6],(editorInstrument->envelope[selectedEnvelope].amount*100) );
	display.refreshControl(barControl[6]);
}

void cInstrumentEditor::showEnvLoop()
{
	display.setControlValue(envLoopListControl, 0);
	display.refreshControl(envLoopListControl);
}



void cInstrumentEditor::showParamsVolume()
{
	display.setControlValue(barControl[0], editorInstrument->volume);
	display.refreshControl(barControl[0]);
}

void cInstrumentEditor::showParamsPanning()
{
	display.setControlValue(barControl[1], editorInstrument->panning);
	display.refreshControl(barControl[1]);
}

void cInstrumentEditor::showParamsTune()
{
	display.setControlValue(barControl[2], (editorInstrument->tune*100)/24);
	display.refreshControl(barControl[2]);
}

void cInstrumentEditor::showParamsFineTune()
{
	display.setControlValue(barControl[3], editorInstrument->fineTune);
	display.refreshControl(barControl[3]);
}

void cInstrumentEditor::showFilterFilterType()
{

	filterModeList.start = filterModeListPos;
	filterModeList.length = filterModeCount;
	filterModeList.linesCount = 5;
	filterModeList.data = filterModeNames;

	display.setControlData(filterModeListControl,  &filterModeList);
	display.setControlShow(filterModeListControl);
	display.refreshControl(filterModeListControl);
}

void cInstrumentEditor::showFilterCutOff()
{
	display.setControlValue(barControl[5], (editorInstrument->cutOff*100));
	display.refreshControl(barControl[5]);
}

void cInstrumentEditor::showFilterResonance()
{
	display.setControlValue(barControl[6], ((editorInstrument->resonance - RESONANCE_MIN)/(RESONANCE_MAX-RESONANCE_MIN))*100);
	display.refreshControl(barControl[6]);
}

void cInstrumentEditor::showParamsReverbSend()
{
	display.setControlValue(barControl[7], editorInstrument->reverbSend);
	display.refreshControl(barControl[7]);
}



void cInstrumentEditor::showParamsGlide()
{
	display.setControlValue(barControl[3], (editorInstrument->glide*100)/GLIDE_MAX);
	display.refreshControl(barControl[3]);
}




void cInstrumentEditor::showInstrList(uint8_t n)
{
	int8_t position = -1;

	if(selectedInstrument >= n*12 && selectedInstrument < (n+1)*12)
	{
		position = selectedInstrument%12;
	}

	display.setControlValue(intrumentsListControl[n], position);
	display.refreshControl(intrumentsListControl[n]);


}

void cInstrumentEditor::showActualInstrument()
{
	static char actualInstrName[SAMPLE_NAME_SIZE+4];

	uint8_t i = mtProject.values.lastUsedInstrument;

	if(i<9)
	{
		actualInstrName[0] = (i+1)%10 + 48;
		actualInstrName[1] = '.';
		actualInstrName[2] = ' ';
		actualInstrName[3] = 0;
	}
	else
	{
		actualInstrName[0] = ((i+1)/10) + 48;
		actualInstrName[1] = (i+1)%10 + 48;
		actualInstrName[2] = '.';
		actualInstrName[3] = ' ';
		actualInstrName[4] = 0;
	}


	strncat(&actualInstrName[0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);


	display.setControlText(instrumentLabel,  actualInstrName);
	display.refreshControl(instrumentLabel);
}


void cInstrumentEditor::listInstruments()
{
	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		if(i<9)
		{
			intrumentsNames[i][0] = (i+1)%10 + 48;
			intrumentsNames[i][1] = '.';
			intrumentsNames[i][2] = ' ';
			intrumentsNames[i][3] = 0;
		}
		else
		{
			intrumentsNames[i][0] = ((i+1)/10) + 48;
			intrumentsNames[i][1] = (i+1)%10 + 48;
			intrumentsNames[i][2] = '.';
			intrumentsNames[i][3] = ' ';
			intrumentsNames[i][4] = 0;
		}

		if(mtProject.instrument[i].sample.loaded)
		{
			strncat(&intrumentsNames[i][0], mtProject.instrument[i].sample.file_name, SAMPLE_NAME_SIZE);
		}

		ptrintrumentsNames[i] = &intrumentsNames[i][0];
	}
}


