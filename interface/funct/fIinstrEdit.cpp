



#include <instrumentEditor.h>


cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functSwitchMode(uint8_t button);

static  uint8_t functSelectVolume(uint8_t button);
static  uint8_t functSelectFilter(uint8_t button);
static  uint8_t functSelectParams(uint8_t button);




void cInstrumentEditor::update()
{


	moduleRefresh = 0;
}

void cInstrumentEditor::start(uint32_t options)
{
	moduleRefresh = 1;


	mode = options;





	// ustawienie funkcji
	FM->setButtonObj(interfaceButton10, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton11, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton12, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton14, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton15, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton16, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButton23, buttonPress, functSwitchMode);
	FM->setButtonObj(interfaceButton24, buttonPress, functSwitchMode);
	FM->setButtonObj(interfaceButton25, buttonPress, functSwitchMode);


	switch(mode)
	{
	case 0:
	{
		showInstrumentVolume();
		setInstrumentVolumeFunct();
		break;
	}
	case 1:
	{
		showInstrumentFilter();
		setInstrumentFilterFunct();
		break;
	}
	case 2:
	{
		showInstrumentParams();
		setInstrumentParamsFunct();
		break;
	}
	}

	activateLabelsBorder();

}

void cInstrumentEditor::stop()
{



}

void cInstrumentEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton8, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButton9, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);



}
//==============================================================================================================
void cInstrumentEditor::setInstrumentVolumeFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectVolume);
	}

}


void cInstrumentEditor::setInstrumentFilterFunct()
{

	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectFilter);
	}

}


void cInstrumentEditor::setInstrumentParamsFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectParams);
	}

}

static  uint8_t functSelectVolume(uint8_t button)
{
	IE->selectedPlace = button;
	IE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectFilter(uint8_t button)
{
	IE->selectedPlace = button;
	IE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectParams(uint8_t button)
{
	IE->selectedPlace = button;
	IE->activateLabelsBorder();

	return 1;
}



//==============================================================================================================


static  uint8_t functEncoder(int16_t value)
{
	uint8_t mode_places = IE->selectedPlace + IE->mode*10;

	switch(mode_places)
	{
	case 0: IE->changeVolumeAttack(value); 		break;
	case 1: IE->changeVolumeDecay(value); 		break;
	case 2: IE->changeVolumeSustain(value); 	break;
	case 3: IE->changeVolumeRelease(value); 	break;
	case 4: IE->changeVolumeAmount(value); 		break;


	case 10: IE->changeFilterAttack(value); 	break;
	case 11: IE->changeFilterDecay(value); 		break;
	case 12: IE->changeFilterSustain(value); 	break;
	case 13: IE->changeFilterRelease(value);	break;
	case 14: IE->changeFilterAmount(value); 	break;
	case 15: IE->changeFilterFilterType(value); break;
	case 16: IE->changeFilterCutOff(value); 	break;
	case 17: IE->changeFilterResonance(value); 	break;

	case 20: IE->changeParamsVolume(value); 	break;
	case 21: IE->changeParamsTune(value); 		break;
	case 22: IE->changeParamsFineTune(value); 	break;
	case 23: IE->changeParamsGlide(value); 		break;
	case 24: IE->changeParamsVibrato(value); 	break;
	case 25: IE->changeParamsTremolo(value); 	break;
	case 26: IE->changeParamsEffect1(value); 	break;
	case 27: IE->changeParamsEffect2(value); 	break;
	}

	return 1;
}


static  uint8_t functLeft()
{

	return 1;
}


static  uint8_t functRight()
{


	return 1;
}


static  uint8_t functUp()
{

	return 1;
}


static  uint8_t functDown()
{


	return 1;
}




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		sequencer.play();
	}
	else if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}



	return 1;
}


static  uint8_t functStopAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}



	return 1;
}

static  uint8_t functRecAction()
{


	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{

	IE->eventFunct(eventSwitchModule,IE,&button,0);

	return 1;
}


static  uint8_t functSwitchMode(uint8_t button)
{
	switch(button)
	{
	case 23:
	{
		if(IE->mode != 0)
		{
			IE->mode = 0;
			IE->showInstrumentVolume();
			IE->setInstrumentVolumeFunct();
		}
		break;
	}
	case 24:
	{
		if(IE->mode != 1)
		{
			IE->mode = 1;
			IE->showInstrumentFilter();
			IE->setInstrumentFilterFunct();
		}
		break;
	}
	case 25:
	{
		if(IE->mode != 2)
		{
			IE->mode = 2;
			IE->showInstrumentParams();
			IE->setInstrumentParamsFunct();
		}
		break;
	}


	}



	return 1;
}

//======================================================================================================================

void cInstrumentEditor::changeVolumeAttack(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeType].attack + value < 0) editorInstrument->envelope[envelopeType].attack = 0;
	else if(editorInstrument->envelope[envelopeType].attack + value > ATTACK_MAX ) editorInstrument->envelope[envelopeType].attack = ATTACK_MAX;
	else editorInstrument->envelope[envelopeType].attack += value;

}

void cInstrumentEditor::changeVolumeDecay(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeType].decay + value < 0) editorInstrument->envelope[envelopeType].decay = 0;
	else if(editorInstrument->envelope[envelopeType].decay + value > DECAY_MAX ) editorInstrument->envelope[envelopeType].decay = DECAY_MAX;
	else editorInstrument->envelope[envelopeType].decay += value;

}

void cInstrumentEditor::changeVolumeSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeType].sustain + fVal < 0) editorInstrument->envelope[envelopeType].sustain = 0;
	else if(editorInstrument->envelope[envelopeType].sustain + fVal > SUSTAIN_MAX ) editorInstrument->envelope[envelopeType].sustain = SUSTAIN_MAX;
	else editorInstrument->envelope[envelopeType].sustain += fVal;

}

void cInstrumentEditor::changeVolumeRelease(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeType].release + value < 0) editorInstrument->envelope[envelopeType].release = 0;
	else if(editorInstrument->envelope[envelopeType].release + value > RELEASE_MAX ) editorInstrument->envelope[envelopeType].release = RELEASE_MAX;
	else editorInstrument->envelope[envelopeType].release += value;

}

void cInstrumentEditor::changeVolumeAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeType].amount + fVal < 0) editorInstrument->envelope[envelopeType].amount = 0;
	else if(editorInstrument->envelope[envelopeType].amount + fVal > AMOUNT_MAX ) editorInstrument->envelope[envelopeType].amount = AMOUNT_MAX;
	else editorInstrument->envelope[envelopeType].amount += fVal;


	editorInstrument->envelope[envelopeTypeAmp].enable = !editorInstrument->envelope[envelopeTypeAmp].enable;

}

void cInstrumentEditor::changeFilterAttack(int16_t value)
{

}

void cInstrumentEditor::changeFilterDecay(int16_t value)
{

}

void cInstrumentEditor::changeFilterSustain(int16_t value)
{

}

void cInstrumentEditor::changeFilterRelease(int16_t value)
{

}

void cInstrumentEditor::changeFilterAmount(int16_t value)
{


	editorInstrument->envelope[envelopeTypeFilter].enable = !editorInstrument->envelope[envelopeTypeFilter].enable;

}

void cInstrumentEditor::changeFilterFilterType(int16_t value)
{
	if(!editorInstrument->filterEnable)
	{
		editorInstrument->filterEnable = 1;
		editorInstrument->filterType = lowPass;
	}
	else if(editorInstrument->filterType == lowPass)
	{
		editorInstrument->filterEnable = 1;
		editorInstrument->filterType = highPass;
	}
	else if(editorInstrument->filterType == highPass)
	{
		editorInstrument->filterEnable = 1;
		editorInstrument->filterType = bandPass;
	}
	else if(editorInstrument->filterType == bandPass)
	{
		editorInstrument->filterEnable = 0;
		editorInstrument->filterType = lowPass;
	}
}

void cInstrumentEditor::changeFilterCutOff(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->cutOff + fVal < MIN_CUTOFF) editorInstrument->cutOff = MIN_CUTOFF;
	else if(editorInstrument->cutOff + fVal > MAX_CUTOFF ) editorInstrument->cutOff = MAX_CUTOFF;
	else editorInstrument->cutOff += fVal;

	instrumentPlayer[0].setStatusBytes(CUTOFF_MASK);

}

void cInstrumentEditor::changeFilterResonance(int16_t value)
{
	float fVal = value * 0.05;

	if(editorInstrument->resonance + fVal < RESONANCE_MIN) editorInstrument->resonance = RESONANCE_MIN;
	else if(editorInstrument->resonance + fVal > RESONANCE_MAX) editorInstrument->resonance = RESONANCE_MAX;
	else editorInstrument->resonance += fVal;

	instrumentPlayer[0].setStatusBytes(RESONANCE_MASK);

}

void cInstrumentEditor::changeParamsVolume(int16_t value)
{
	if(editorInstrument->volume + value < 0) editorInstrument->volume = 0;
	else if(editorInstrument->volume + value > MAX_INSTRUMENT_VOLUME) editorInstrument->volume = MAX_INSTRUMENT_VOLUME;
	else editorInstrument->volume += value;

	instrumentPlayer[0].setStatusBytes(VOLUME_MASK);

}

void cInstrumentEditor::changeParamsTune(int16_t value)
{
	if(editorInstrument->tune + value < MIN_INSTRUMENT_TUNE) editorInstrument->tune = MIN_INSTRUMENT_TUNE;
	else if(editorInstrument->tune + value > MAX_INSTRUMENT_TUNE) editorInstrument->tune = MAX_INSTRUMENT_TUNE;
	else editorInstrument->tune += value;

	instrumentPlayer[0].setStatusBytes(TUNE_MASK);

}

void cInstrumentEditor::changeParamsFineTune(int16_t value)
{
	if(editorInstrument->fineTune + value < MIN_INSTRUMENT_FINETUNE) editorInstrument->fineTune = MIN_INSTRUMENT_FINETUNE;
	else if(editorInstrument->fineTune + value > MAX_INSTRUMENT_FINETUNE) editorInstrument->fineTune = MAX_INSTRUMENT_FINETUNE;
	else editorInstrument->fineTune += value;

	instrumentPlayer[0].setStatusBytes(FINETUNE_MASK);

}

void cInstrumentEditor::changeParamsGlide(int16_t value)
{
	value = value * 100;

	if(editorInstrument->glide + value < GLIDE_MIN) editorInstrument->glide = GLIDE_MIN;
	else if(editorInstrument->glide + value > GLIDE_MAX ) editorInstrument->glide = GLIDE_MAX;
	else editorInstrument->glide += value;

}

void cInstrumentEditor::changeParamsVibrato(int16_t value)
{

}

void cInstrumentEditor::changeParamsTremolo(int16_t value)
{

}

void cInstrumentEditor::changeParamsEffect1(int16_t value)
{

}

void cInstrumentEditor::changeParamsEffect2(int16_t value)
{

}


