

#include "instrumentEditor.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtPadsBacklight.h"



cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();

static  uint8_t functInstrument(uint8_t state);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functLeftInstr();
static  uint8_t functRightInstr();
static  uint8_t functUpInstr();
static  uint8_t functDownInstr();


static  uint8_t functEncoder(int16_t value);
static  uint8_t functEncoderInstr(int16_t value);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functSwitchMode(uint8_t button);

static  uint8_t functSelectEnv(uint8_t button);
static  uint8_t functSelectParams(uint8_t button);


static uint8_t functShift(uint8_t value);

static uint8_t functStepNote(uint8_t value);





void cInstrumentEditor::update()
{


	moduleRefresh = 0;
}

void cInstrumentEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mode = options;

	selectedInstrument = mtProject.values.lastUsedInstrument;

	mtPadBoard.setPadNotes(mtProject.values.padBoardScale,
			mtProject.values.padBoardNoteOffset,
			mtProject.values.padBoardRootNote);

	mtPadBoard.configureInstrumentPlayer(mtProject.values.padBoardMaxVoices);



	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

	listData();


	FM->setPadsGlobal(functPads);



	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchMode);
	FM->setButtonObj(interfaceButtonEnvelopes, buttonPress, functSwitchMode);


	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	setDefaultScreenFunct();

	switch(mode)
	{
	case mtInstEditModeParams:
	{
		showInstrumentParams();
		setInstrumentParamsFunct();
		break;
	}
	case mtInstEditModeEnv:
	{
		showInstrumentEnv();
		setInstrumentEnvFunct();
		break;
	}
	case mtInstEditModeInstrList:
	{
		showInstrumentList();
		setInstrumentListFunct();
		return;
	}
	}
}

void cInstrumentEditor::stop()
{
	moduleRefresh = 0;
	mtPadBoard.releaseAllInstrument();
	padsBacklight.clearAllPads(1, 1, 1);
}

void cInstrumentEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonShift, functShift);


	FM->setButtonObj(interfaceButtonInstr, functInstrument);



}

void cInstrumentEditor::listData()
{

	for(uint8_t i = 0; i < filterModeCount; i++)
	{
		filterModeNames[i] = (char*)&filterModeFunctLabels[i][0];
	}

	for(uint8_t i = 0; i < 2; i++)
	{
		envelopeNames[i] = (char*)&envelopesLabels[i][0];
	}

	for(uint8_t i = 0; i < 2; i++)
	{
		envStateNames[i] = (char*)&envStateLabels[i][0];
	}

	for(uint8_t i = 0; i < 2; i++)
	{
		envLoopNames[i] = (char*)&envLoopLabels[i][0];
	}

}


//==============================================================================================================
void cInstrumentEditor::setInstrumentEnvFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectEnv);
	}

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	activateLabelsBorder();

	padsBacklight.clearAllPads(0, 1, 1);
}

void cInstrumentEditor::setInstrumentParamsFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectParams);
	}

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);
	FM->setButtonObj(interfaceButtonNote, functStepNote);

	activateLabelsBorder();

	padsBacklight.clearAllPads(0, 1, 1);

	changeFilterFilterType(0);
}


void cInstrumentEditor::setInstrumentListFunct()
{

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setPotObj(interfacePot0, functEncoderInstr, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeftInstr);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRightInstr);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUpInstr);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDownInstr);


	lightUpPadBoard();
}


static  uint8_t functSelectEnv(uint8_t button)
{
	IE->selectedPlace[1] = button;
	IE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectParams(uint8_t button)
{
	IE->selectedPlace[0] = button;
	IE->activateLabelsBorder();

	return 1;
}



//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{
	uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;

	switch(mode_places)
	{
	case 0: IE->changeParamsVolume(value); 		 break;
	case 1:	IE->changeParamsPanning(value); 	 break;
	case 2: IE->changeParamsTune(value); 		 break;
	case 3: IE->changeParamsFineTune(value); 	 break;
	case 4: IE->changeFilterFilterType(value); 	 break;
	case 5: IE->changeFilterCutOff(value); 		 break;
	case 6: IE->changeFilterResonance(value); 	 break;
	case 7: IE->changeParamsReverbSend(value); 	 break;

	case 10: IE->changeEnvList(value); 		break;
	case 11: IE->changeEnvState(value); 	break;
	case 12: IE->changeEnvAttack(value); 	break;
	case 13: IE->changeEnvDecay(value); 	break;
	case 14: IE->changeEnvSustain(value); 	break;
	case 15: IE->changeEnvRelease(value); 	break;
	case 16: IE->changeEnvAmount(value); 	break;
	case 17: IE->changeEnvLoop(value); 		break;

	}

	return 1;
}

static  uint8_t functEncoderInstr(int16_t value)
{
	//if(IE->mode == mtInstEditModeInstrList)
	//{
		IE->changeSelectedInstrument(value);
	//}
	return 1;
}

//=========================================================================================================
static  uint8_t functLeft()
{
	if(IE->selectedPlace[IE->mode] > 0) IE->selectedPlace[IE->mode]--;
	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(IE->selectedPlace[IE->mode] < IE->frameData.placesCount-1) IE->selectedPlace[IE->mode]++;
	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;

	switch(mode_places)
	{
	case 0: IE->changeParamsVolume(1); 		 break;
	case 1:	IE->changeParamsPanning(1); 	 break;
	case 2: IE->changeParamsTune(1); 		 break;
	case 3: IE->changeParamsFineTune(1); 	 break;
	case 4: IE->changeFilterFilterType(-1); 	 break;
	case 5: IE->changeFilterCutOff(1); 		 break;
	case 6: IE->changeFilterResonance(1); 	 break;
	case 7: IE->changeParamsReverbSend(1); 	 break;

	case 10: IE->changeEnvList(-1); 		break;
	case 11: IE->changeEnvState(-1); 	break;
	case 12: IE->changeEnvAttack(1); 	break;
	case 13: IE->changeEnvDecay(1); 	break;
	case 14: IE->changeEnvSustain(1); 	break;
	case 15: IE->changeEnvRelease(1); 	break;
	case 16: IE->changeEnvAmount(1); 	break;
	case 17: IE->changeEnvLoop(-1); 		break;

	}

	return 1;
}


static  uint8_t functDown()
{
	uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;

	switch(mode_places)
	{
	case 0: IE->changeParamsVolume(-1);      break;
	case 1:	IE->changeParamsPanning(-1); 	 break;
	case 2: IE->changeParamsTune(-1); 		 break;
	case 3: IE->changeParamsFineTune(-1); 	 break;
	case 4: IE->changeFilterFilterType(1);  break;
	case 5: IE->changeFilterCutOff(-1); 	 break;
	case 6: IE->changeFilterResonance(-1); 	 break;
	case 7: IE->changeParamsReverbSend(-1);  break;

	case 10: IE->changeEnvList(1); 		break;
	case 11: IE->changeEnvState(1); 	    break;
	case 12: IE->changeEnvAttack(-1); 		break;
	case 13: IE->changeEnvDecay(-1); 		break;
	case 14: IE->changeEnvSustain(-1); 		break;
	case 15: IE->changeEnvRelease(-1); 		break;
	case 16: IE->changeEnvAmount(-1); 		break;
	case 17: IE->changeEnvLoop(1); 		break;

	}


	return 1;
}

//=========================================================================================================
static  uint8_t functLeftInstr()
{
	//if(IE->mode == mtInstEditModeInstrList)
	//{
		if(IE->selectedInstrument >= 12) IE->changeSelectedInstrument(-12,1);
		return 1;
	//}
	return 1;
}

static  uint8_t functRightInstr()
{
	//if(IE->mode == mtInstEditModeInstrList)
	//{
		if(IE->selectedInstrument < 36) IE->changeSelectedInstrument(12,1);
		return 1;
	//}
	return 1;
}

static  uint8_t functUpInstr()
{
	if(IE->selectedInstrument > 0) IE->changeSelectedInstrument(-1);

	return 1;
}

static  uint8_t functDownInstr()
{

	if(IE->selectedInstrument < 47) IE->changeSelectedInstrument(1);

	return 1;
}


//=========================================================================================================
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
	case interfaceButtonParams:
	{
		if(IE->mode != mtInstEditModeParams)
		{
			IE->mode = 0;
			IE->showInstrumentParams();
			IE->setInstrumentParamsFunct();
		}
		break;
	}
	case interfaceButtonEnvelopes:
	{
		if(IE->mode != mtInstEditModeEnv)
		{
			IE->mode = 1;
			IE->showInstrumentEnv();
			IE->setInstrumentEnvFunct();
		}
		break;
	}



	}

	IE->activateLabelsBorder();

	return 1;
}

//======================================================================================================================

void cInstrumentEditor::changeEnvList(int16_t value)
{

	if(selectedEnvelope + value < 0) selectedEnvelope = 0;
	else if(selectedEnvelope + value > 1 ) selectedEnvelope = 1;
	else selectedEnvelope += value;

	showInstrumentEnv();

	showEnvList();

}

void cInstrumentEditor::changeEnvState(int16_t value)
{



	showEnvState();
}

void cInstrumentEditor::changeEnvAttack(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[selectedEnvelope].attack + value < 0) editorInstrument->envelope[selectedEnvelope].attack = 0;
	else if(editorInstrument->envelope[selectedEnvelope].attack + value > ATTACK_MAX ) editorInstrument->envelope[selectedEnvelope].attack = ATTACK_MAX;
	else editorInstrument->envelope[selectedEnvelope].attack += value;

	showEnvAttack();
}

void cInstrumentEditor::changeEnvDecay(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[selectedEnvelope].decay + value < 0) editorInstrument->envelope[selectedEnvelope].decay = 0;
	else if(editorInstrument->envelope[selectedEnvelope].decay + value > DECAY_MAX ) editorInstrument->envelope[selectedEnvelope].decay = DECAY_MAX;
	else editorInstrument->envelope[selectedEnvelope].decay += value;

	showEnvDecay();
}

void cInstrumentEditor::changeEnvSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[selectedEnvelope].sustain + fVal < 0) editorInstrument->envelope[selectedEnvelope].sustain = 0;
	else if(editorInstrument->envelope[selectedEnvelope].sustain + fVal > SUSTAIN_MAX ) editorInstrument->envelope[selectedEnvelope].sustain = SUSTAIN_MAX;
	else editorInstrument->envelope[selectedEnvelope].sustain += fVal;

	showEnvSustain();
}

void cInstrumentEditor::changeEnvRelease(int16_t value)
{
	value = value * 100;

	if(editorInstrument->envelope[selectedEnvelope].release + value < 0) editorInstrument->envelope[selectedEnvelope].release = 0;
	else if(editorInstrument->envelope[selectedEnvelope].release + value > RELEASE_MAX ) editorInstrument->envelope[selectedEnvelope].release = RELEASE_MAX;
	else editorInstrument->envelope[selectedEnvelope].release += value;

	showEnvRelease();
}

void cInstrumentEditor::changeEnvAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(editorInstrument->envelope[selectedEnvelope].amount + fVal < 0) editorInstrument->envelope[selectedEnvelope].amount = 0;
	else if(editorInstrument->envelope[selectedEnvelope].amount + fVal > AMOUNT_MAX ) editorInstrument->envelope[selectedEnvelope].amount = AMOUNT_MAX;
	else editorInstrument->envelope[selectedEnvelope].amount += fVal;

	if(editorInstrument->envelope[selectedEnvelope].amount == 0)
	{
		editorInstrument->envelope[selectedEnvelope].enable = 0;
	}
	else
	{
		editorInstrument->envelope[selectedEnvelope].enable = 1;
	}

	showEnvAmount();
}



void cInstrumentEditor::changeEnvLoop(int16_t value)
{



	showEnvLoop();
}


void cInstrumentEditor::changeSelectedInstrument(int16_t value, uint8_t type)
{
	uint8_t oldList = selectedInstrument/12;

	if(selectedInstrument + value < 0) selectedInstrument = 0;
	else if(selectedInstrument + value > INSTRUMENTS_MAX) selectedInstrument = INSTRUMENTS_MAX;
	else selectedInstrument += value;

	uint8_t newList = selectedInstrument/12;

	mtProject.values.lastUsedInstrument = selectedInstrument;
	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];


	if(oldList != newList)
	{
		if(oldList < newList)
		{
			intrumentsList[newList].start = 0;
			if(type) intrumentsList[newList].start = selectedInstrument%12;
			//intrumentsList[oldList].start = 11;
			intrumentsList[oldList].start = -1;
		}
		else
		{
			//intrumentsList[oldList].start = 0;
			intrumentsList[oldList].start = -1;

			intrumentsList[newList].start = 11;
			if(type) intrumentsList[newList].start = selectedInstrument%12;
		}

		display.setControlData(intrumentsListControl[newList], &intrumentsList[newList]);
		display.setControlData(intrumentsListControl[oldList], &intrumentsList[oldList]);

		showInstrList(oldList);
	}

	showInstrList(newList);

	showActualInstrument();

	lightUpPadBoard();
}


void cInstrumentEditor::changeFilterFilterType(int16_t value)
{

	if(filterModeListPos + value < 0) filterModeListPos = 0;
	else if(filterModeListPos + value > filterModeCount-1) filterModeListPos = filterModeCount-1;
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

	display.setControlText(topLabel[4], filterModeFunctLabels[filterModeListPos]);
	display.refreshControl(topLabel[4]);

	display.setControlValue(filterModeListControl, filterModeListPos);
	display.refreshControl(filterModeListControl);
	//showFilterFilterType();
}

uint8_t bgB = 128;

void cInstrumentEditor::changeFilterCutOff(int16_t value)
{
//	if(bgB + value < 0) bgB = 0;
//	else if(bgB + value > 128 ) bgB = 128;
//	else bgB += value;
	float fVal = value * 0.01;

	if(editorInstrument->cutOff + fVal < MIN_CUTOFF) editorInstrument->cutOff = MIN_CUTOFF;
	else if(editorInstrument->cutOff + fVal > MAX_CUTOFF ) editorInstrument->cutOff = MAX_CUTOFF;
	else editorInstrument->cutOff += fVal;

	display.setBacklightBrightness(map(editorInstrument->cutOff,MIN_CUTOFF,MAX_CUTOFF,0,128));

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


static uint8_t functShift(uint8_t value)
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


//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################


void cInstrumentEditor::lightUpPadBoard()
{
	padsBacklight.clearAllPads(0, 1, 1);


	if(mtProject.values.lastUsedInstrument >= 0 && mtProject.values.lastUsedInstrument <= 48)
	{
		padsBacklight.setBackLayer(1, 20, mtProject.values.lastUsedInstrument);
	}





}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	if(state == 1)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;

		padsBacklight.setFrontLayer(1,20, pad);
		mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);

	}
	else if(state == 0)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);

	}

	return 1;
}

static  uint8_t functInstrument(uint8_t state)
{
	if(state == buttonRelease)
	{
		if(IE->mode == mtInstEditModeInstrList)	IE->eventFunct(eventSwitchToPreviousModule,IE,0,0);
		else
		{
			if(IE->mode == mtInstEditModeParams)
			{
				IE->showInstrumentParams();
				IE->setInstrumentParamsFunct();
			}
			else //mtInstEditModeEnv
			{
				IE->showInstrumentEnv();
				IE->setInstrumentEnvFunct();

			}
		}
	}
	else if(state == buttonPress)
	{
		IE->hideNotePopout();

		if(IE->mode != mtInstEditModeInstrList)
		{
			IE->showInstrumentList();
			IE->setInstrumentListFunct();
		}
	}


	return 1;
}

static uint8_t functStepNote(uint8_t value)
{
	if(IE->mode==0)
	{
		if(value == buttonPress)
		{
			for(uint8_t i = 0; i < 48; i++)
			{
				IE->padNamesPointer[i] = (char*)mtNotes[mtPadBoard.getNoteFromPad(i)];
			}

			IE->showNotePopout();
		}
		else if(value == buttonRelease)
		{
			IE->hideNotePopout();
		}
	}

	return 1;
}



