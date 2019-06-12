

#include "instrumentEditor.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"




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


static uint8_t play(uint8_t value);




void cInstrumentEditor::update()
{


	moduleRefresh = 0;
}

void cInstrumentEditor::start(uint32_t options)
{
	moduleRefresh = 1;


	mode = options;


	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote);

	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);



	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

	listFilterMode();


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

	setDefaultScreenFunct();

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
	FM->setButtonObj(interfaceButton28, play);


	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);



}

void cInstrumentEditor::listFilterMode()
{

	for(uint8_t i = 0; i < filterModeCount; i++)
	{
		filterModeNames[i] = (char*)&filterModeFunctLabels[i][0];
	}


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
	case 24: IE->changeParamsPanning(value); 	break;
	case 25: IE->changeParamsVibrato(value); 	break;
	case 26: IE->changeParamsTremolo(value); 	break;
	case 27: IE->changeParamsReverbSend(value); 	break;
	}

	return 1;
}


static  uint8_t functLeft()
{
	if(IE->selectedPlace > 0) IE->selectedPlace--;
	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{

	if(IE->selectedPlace < IE->frameData.placesCount-1) IE->selectedPlace++;
	IE->activateLabelsBorder();

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

	if(editorInstrument->envelope[envelopeTypeAmp].attack + value < 0) editorInstrument->envelope[envelopeTypeAmp].attack = 0;
	else if(editorInstrument->envelope[envelopeTypeAmp].attack + value > ATTACK_MAX ) editorInstrument->envelope[envelopeTypeAmp].attack = ATTACK_MAX;
	else editorInstrument->envelope[envelopeTypeAmp].attack += value;

	showVolumeAttack();
}

void cInstrumentEditor::changeVolumeDecay(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeTypeAmp].decay + value < 0) editorInstrument->envelope[envelopeTypeAmp].decay = 0;
	else if(editorInstrument->envelope[envelopeTypeAmp].decay + value > DECAY_MAX ) editorInstrument->envelope[envelopeTypeAmp].decay = DECAY_MAX;
	else editorInstrument->envelope[envelopeTypeAmp].decay += value;

	showVolumeDecay();
}

void cInstrumentEditor::changeVolumeSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeTypeAmp].sustain + fVal < 0) editorInstrument->envelope[envelopeTypeAmp].sustain = 0;
	else if(editorInstrument->envelope[envelopeTypeAmp].sustain + fVal > SUSTAIN_MAX ) editorInstrument->envelope[envelopeTypeAmp].sustain = SUSTAIN_MAX;
	else editorInstrument->envelope[envelopeTypeAmp].sustain += fVal;

	showVolumeSustain();
}

void cInstrumentEditor::changeVolumeRelease(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeTypeAmp].release + value < 0) editorInstrument->envelope[envelopeTypeAmp].release = 0;
	else if(editorInstrument->envelope[envelopeTypeAmp].release + value > RELEASE_MAX ) editorInstrument->envelope[envelopeTypeAmp].release = RELEASE_MAX;
	else editorInstrument->envelope[envelopeTypeAmp].release += value;

	showVolumeRelease();
}

void cInstrumentEditor::changeVolumeAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeTypeAmp].amount + fVal < 0) editorInstrument->envelope[envelopeTypeAmp].amount = 0;
	else if(editorInstrument->envelope[envelopeTypeAmp].amount + fVal > AMOUNT_MAX ) editorInstrument->envelope[envelopeTypeAmp].amount = AMOUNT_MAX;
	else editorInstrument->envelope[envelopeTypeAmp].amount += fVal;

	if(editorInstrument->envelope[envelopeTypeAmp].amount == 0)
	{
		editorInstrument->envelope[envelopeTypeAmp].enable = 0;
	}
	else
	{
		editorInstrument->envelope[envelopeTypeAmp].enable = 1;
	}

	showVolumeAmount();
}

void cInstrumentEditor::changeFilterAttack(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeTypeFilter].attack + value < 0) editorInstrument->envelope[envelopeTypeFilter].attack = 0;
	else if(editorInstrument->envelope[envelopeTypeFilter].attack + value > ATTACK_MAX ) editorInstrument->envelope[envelopeTypeFilter].attack = ATTACK_MAX;
	else editorInstrument->envelope[envelopeTypeFilter].attack += value;

	showFilterAttack();
}

void cInstrumentEditor::changeFilterDecay(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeTypeFilter].decay + value < 0) editorInstrument->envelope[envelopeTypeFilter].decay = 0;
	else if(editorInstrument->envelope[envelopeTypeFilter].decay + value > DECAY_MAX ) editorInstrument->envelope[envelopeTypeFilter].decay = DECAY_MAX;
	else editorInstrument->envelope[envelopeTypeFilter].decay += value;

	showFilterDecay();
}

void cInstrumentEditor::changeFilterSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeTypeFilter].sustain + fVal < 0) editorInstrument->envelope[envelopeTypeFilter].sustain = 0;
	else if(editorInstrument->envelope[envelopeTypeFilter].sustain + fVal > SUSTAIN_MAX ) editorInstrument->envelope[envelopeTypeFilter].sustain = SUSTAIN_MAX;
	else editorInstrument->envelope[envelopeTypeFilter].sustain += fVal;

	showFilterSustain();
}

void cInstrumentEditor::changeFilterRelease(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[envelopeTypeFilter].release + value < 0) editorInstrument->envelope[envelopeTypeFilter].release = 0;
	else if(editorInstrument->envelope[envelopeTypeFilter].release + value > RELEASE_MAX ) editorInstrument->envelope[envelopeTypeFilter].release = RELEASE_MAX;
	else editorInstrument->envelope[envelopeTypeFilter].release += value;

	showFilterRelease();
}

void cInstrumentEditor::changeFilterAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[envelopeTypeFilter].amount + fVal < 0) editorInstrument->envelope[envelopeTypeFilter].amount = 0;
	else if(editorInstrument->envelope[envelopeTypeFilter].amount + fVal > AMOUNT_MAX ) editorInstrument->envelope[envelopeTypeFilter].amount = AMOUNT_MAX;
	else editorInstrument->envelope[envelopeTypeFilter].amount += fVal;

	if(editorInstrument->envelope[envelopeTypeFilter].amount == 0)
	{
		editorInstrument->envelope[envelopeTypeFilter].enable = 0;
	}
	else
	{
		editorInstrument->envelope[envelopeTypeFilter].enable = 1;
	}

	showFilterAmount();
}

void cInstrumentEditor::changeFilterFilterType(int16_t value)
{

	if(filterModeListPos + value < 0) filterModeListPos = 0;
	else if(filterModeListPos + value > filterModeCount-1)filterModeListPos = filterModeCount-1;
	else filterModeListPos += value;

	if(filterModeListPos == 0)
	{
		editorInstrument->filterEnable = 0;
		editorInstrument->filterType = lowPass;
	}
	else if(filterModeListPos == 1)
	{
		editorInstrument->filterEnable = 1;
		editorInstrument->filterType = lowPass;
	}
	else if(filterModeListPos == 2)
	{
		editorInstrument->filterEnable = 1;
		editorInstrument->filterType = highPass;
	}
	else if(filterModeListPos >= filterModeCount-1)
	{
		editorInstrument->filterEnable = 1;
		editorInstrument->filterType = bandPass;
	}


	showFilterFilterType();
}

void cInstrumentEditor::changeFilterCutOff(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->cutOff + fVal < MIN_CUTOFF) editorInstrument->cutOff = MIN_CUTOFF;
	else if(editorInstrument->cutOff + fVal > MAX_CUTOFF ) editorInstrument->cutOff = MAX_CUTOFF;
	else editorInstrument->cutOff += fVal;

	instrumentPlayer[0].setStatusBytes(CUTOFF_MASK);

	showFilterCutOff();
}

void cInstrumentEditor::changeFilterResonance(int16_t value)
{
	float fVal = value * 0.05;

	if(editorInstrument->resonance + fVal < RESONANCE_MIN) editorInstrument->resonance = RESONANCE_MIN;
	else if(editorInstrument->resonance + fVal > RESONANCE_MAX) editorInstrument->resonance = RESONANCE_MAX;
	else editorInstrument->resonance += fVal;

	instrumentPlayer[0].setStatusBytes(RESONANCE_MASK);

	showFilterResonance();
}

void cInstrumentEditor::changeParamsVolume(int16_t value)
{
	if(editorInstrument->volume + value < 0) editorInstrument->volume = 0;
	else if(editorInstrument->volume + value > MAX_INSTRUMENT_VOLUME) editorInstrument->volume = MAX_INSTRUMENT_VOLUME;
	else editorInstrument->volume += value;

	instrumentPlayer[0].setStatusBytes(VOLUME_MASK);

	showParamsVolume();
}

void cInstrumentEditor::changeParamsTune(int16_t value)
{
	if(editorInstrument->tune + value < MIN_INSTRUMENT_TUNE) editorInstrument->tune = MIN_INSTRUMENT_TUNE;
	else if(editorInstrument->tune + value > MAX_INSTRUMENT_TUNE) editorInstrument->tune = MAX_INSTRUMENT_TUNE;
	else editorInstrument->tune += value;

	instrumentPlayer[0].setStatusBytes(TUNE_MASK);

	showParamsTune();
}

void cInstrumentEditor::changeParamsFineTune(int16_t value)
{
	if(editorInstrument->fineTune + value < MIN_INSTRUMENT_FINETUNE) editorInstrument->fineTune = MIN_INSTRUMENT_FINETUNE;
	else if(editorInstrument->fineTune + value > MAX_INSTRUMENT_FINETUNE) editorInstrument->fineTune = MAX_INSTRUMENT_FINETUNE;
	else editorInstrument->fineTune += value;

	instrumentPlayer[0].setStatusBytes(FINETUNE_MASK);

	showParamsFineTune();
}

void cInstrumentEditor::changeParamsGlide(int16_t value)
{
	value = value * 100;

	if(editorInstrument->glide + value < GLIDE_MIN) editorInstrument->glide = GLIDE_MIN;
	else if(editorInstrument->glide + value > GLIDE_MAX ) editorInstrument->glide = GLIDE_MAX;
	else editorInstrument->glide += value;

	showParamsGlide();
}

void cInstrumentEditor::changeParamsPanning(int16_t value)
{
	if(editorInstrument->panning + value < PANNING_MIN) editorInstrument->panning = PANNING_MIN;
	else if(editorInstrument->panning + value > PANNING_MAX ) editorInstrument->panning = PANNING_MAX;
	else editorInstrument->panning += value;

	instrumentPlayer[0].setStatusBytes(PANNING_MASK);

	showParamsPanning();
}

void cInstrumentEditor::changeParamsVibrato(int16_t value)
{


	showParamsVibrato();
}

void cInstrumentEditor::changeParamsTremolo(int16_t value)
{


	showParamsTremolo();
}

void cInstrumentEditor::changeParamsReverbSend(int16_t value)
{
	if(editorInstrument->reverbSend + value < REVERB_SEND_MIN) editorInstrument->reverbSend = REVERB_SEND_MIN;
	else if(editorInstrument->reverbSend + value > REVERB_SEND_MAX ) editorInstrument->reverbSend = REVERB_SEND_MAX;
	else editorInstrument->reverbSend += value;

	instrumentPlayer[0].setStatusBytes(REVERB_SEND_MASK);

	showParamsReverbSend();
}

//======================================================================================================================
//==============================================================================================


static uint8_t play(uint8_t value)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	if(value == 1)
	{
		//eventFunct(mtInstrumentEditorEventPadPress, &interfacePadStop, 0, 0);
		sequencer.stop();

		IE->isPlayingSample = 1;

		instrumentPlayer[0].noteOn(mtProject.values.lastUsedInstrument, 24, -1);
	}
	else if(value == 0)
	{
		if(IE->isPlayingSample) instrumentPlayer[0].noteOff();
		IE->isPlayingSample = 0;
	}

	return 1;
}
