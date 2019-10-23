

#include "instrumentEditor.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtPadsBacklight.h"


#include "interfacePopups.h"

#include "mtFileManager.h"


cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();

static  uint8_t functInstrument(uint8_t state);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();



static  uint8_t functCopy();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functSwitchMode(uint8_t button);

static  uint8_t functSelectEnv(uint8_t button);
static  uint8_t functSelectParams(uint8_t button, uint8_t state);


static uint8_t functShift(uint8_t value);

static uint8_t functStepNote(uint8_t value);


void changeEnvList(int16_t value);
void changeEnvState(int16_t value);
void changeEnvAttack(int16_t value);
void changeEnvDecay(int16_t value);
void changeEnvSustain(int16_t value);
void changeEnvRelease(int16_t value);
void changeEnvAmount(int16_t value);
void changeEnvLoop(int16_t value);


void changeParamsVolume(int16_t value);
void changeParamsPanning(int16_t value);
void changeParamsTune(int16_t value);
void changeParamsFineTune(int16_t value);
void changeFilterFilterType(int16_t value);
void changeFilterCutOff(int16_t value);
void changeFilterResonance(int16_t value);
void changeParamsReverbSend(int16_t value);


void cInstrumentEditor::addNode(editFunct_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cInstrumentEditor::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cInstrumentEditor::stepThroughNodes(int16_t value)
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		if(selectNodes[node].isActive)
		{
			if(selectNodes[node].editFunct != NULL)
			{
				selectNodes[node].editFunct(value);
			}
		}
	}
}

void cInstrumentEditor::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}

void cInstrumentEditor::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		IE->frameData.multisel[i].isActive = 0;
	}

	IE->frameData.multiSelActiveNum = 0;
}


void cInstrumentEditor::update()
{


	moduleRefresh = 0;
}

void cInstrumentEditor::start(uint32_t options)
{
	moduleRefresh = 1;


	mtProject.values.lastUsedInstrument = constrain(mtProject.values.lastUsedInstrument, 0, INSTRUMENTS_MAX);
	editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

	listData();

	clearAllNodes();
	cancelMultiFrame();


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchMode);


	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
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

	FM->setPadsGlobal(functPads);

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
		FM->setButtonObj(i, functSelectParams);
	}

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);
	FM->setButtonObj(interfaceButtonNote, functStepNote);

	activateLabelsBorder();

	padsBacklight.clearAllPads(0, 1, 1);
}

void cInstrumentEditor::setInstrumentParamsFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, functSelectParams);
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




static  uint8_t functSelectEnv(uint8_t button)
{
	IE->selectedPlace[1] = button;
	IE->activateLabelsBorder();

	return 1;
}

static uint8_t functSelectParams(uint8_t button, uint8_t state)
{
	uint8_t mode_places = button + IE->mode*10;

	uint8_t node = button;

	if(state == buttonPress)
	{
		switch(mode_places)
		{
		case 0: IE->addNode(changeParamsVolume, node); 	    	break;
		case 1:	IE->addNode(changeParamsPanning, node); 	    break;
		case 2: IE->addNode(changeParamsTune, node); 			break;
		case 3: IE->addNode(changeParamsFineTune, node); 		break;
		case 4: IE->addNode(changeFilterFilterType, node);   	break;
		case 5: IE->addNode(changeFilterCutOff, node); 	    	break;
		case 6: IE->addNode(changeFilterResonance, node);    	break;
		case 7: IE->addNode(changeParamsReverbSend, node);   	break;

		case 10: IE->addNode(changeEnvList, node); 		 		break;
		case 11: IE->addNode(changeEnvState, node); 		 	break;
		case 12: IE->addNode(changeEnvAttack, node); 	 		break;
		case 13: IE->addNode(changeEnvDecay, node); 		 	break;
		case 14: IE->addNode(changeEnvSustain, node); 	 		break;
		case 15: IE->addNode(changeEnvRelease, node); 	 		break;
		case 16: IE->addNode(changeEnvAmount, node); 	 		break;
		case 17: IE->addNode(changeEnvLoop, node); 		 		break;
		}

		IE->frameData.multisel[button].frameNum = node;
		IE->frameData.multisel[button].isActive = 1;
		IE->frameData.multiSelActiveNum  += 1;
	}
	else if(state == buttonRelease)
	{
		if(IE->frameData.multiSelActiveNum)
		{
			if(IE->frameData.multisel[button].isActive)
			{
				IE->removeNode(node);
				IE->frameData.multiSelActiveNum  -= 1;

				IE->frameData.multisel[button].isActive = 0;

				if(IE->frameData.multiSelActiveNum == 0)
				{
					IE->selectedPlace[IE->mode] = button;
				}
			}
		}
	}

	IE->activateLabelsBorder();

	return 1;
}



//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{
	if(IE->frameData.multiSelActiveNum != 0)
	{
		IE->stepThroughNodes(value);
	}
	else
	{
		uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;

		switch(mode_places)
		{
		case 0: changeParamsVolume(value); 		 break;
		case 1:	changeParamsPanning(value); 	 break;
		case 2: changeParamsTune(value); 		 break;
		case 3: changeParamsFineTune(value); 	 break;
		case 4: changeFilterFilterType(value); 	 break;
		case 5: changeFilterCutOff(value); 		 break;
		case 6: changeFilterResonance(value); 	 break;
		case 7: changeParamsReverbSend(value); 	 break;

		case 10: changeEnvList(value); 		break;
		case 11: changeEnvState(value); 	break;
		case 12: changeEnvAttack(value); 	break;
		case 13: changeEnvDecay(value); 	break;
		case 14: changeEnvSustain(value); 	break;
		case 15: changeEnvRelease(value); 	break;
		case 16: changeEnvAmount(value); 	break;
		case 17: changeEnvLoop(value); 		break;

		}
	}

	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	if(IE->frameData.multiSelActiveNum != 0) return 1;

	if(IE->selectedPlace[IE->mode] > 0) IE->selectedPlace[IE->mode]--;
	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(IE->frameData.multiSelActiveNum != 0) return 1;

	if(IE->selectedPlace[IE->mode] < IE->frameData.placesCount-1) IE->selectedPlace[IE->mode]++;
	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(IE->frameData.multiSelActiveNum != 0)
	{
		IE->stepThroughNodes(1);
	}
	else
	{
		uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;

		switch(mode_places)
		{
		case 0: changeParamsVolume(1); 		 break;
		case 1:	changeParamsPanning(1); 	 break;
		case 2: changeParamsTune(1); 		 break;
		case 3: changeParamsFineTune(1); 	 break;
		case 4: changeFilterFilterType(1); 	 break;
		case 5: changeFilterCutOff(1); 		 break;
		case 6: changeFilterResonance(1); 	 break;
		case 7: changeParamsReverbSend(1); 	 break;

		case 10: changeEnvList(1); 		break;
		case 11: changeEnvState(1); 	break;
		case 12: changeEnvAttack(1); 	break;
		case 13: changeEnvDecay(1); 	break;
		case 14: changeEnvSustain(1); 	break;
		case 15: changeEnvRelease(1); 	break;
		case 16: changeEnvAmount(1); 	break;
		case 17: changeEnvLoop(1); 		break;

		}
	}

	return 1;
}


static  uint8_t functDown()
{
	if(IE->frameData.multiSelActiveNum != 0)
	{
		IE->stepThroughNodes(-1);
	}
	else
	{
		uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;

		switch(mode_places)
		{
		case 0: changeParamsVolume(-1); 		 break;
		case 1:	changeParamsPanning(-1); 		 break;
		case 2: changeParamsTune(-1); 		 	 break;
		case 3: changeParamsFineTune(-1); 	 	 break;
		case 4: changeFilterFilterType(-1); 	 break;
		case 5: changeFilterCutOff(-1); 		 break;
		case 6: changeFilterResonance(-1); 	 	 break;
		case 7: changeParamsReverbSend(-1); 	 break;

		case 10: changeEnvList(-1); 			break;
		case 11: changeEnvState(-1); 			break;
		case 12: changeEnvAttack(-1); 			break;
		case 13: changeEnvDecay(-1); 			break;
		case 14: changeEnvSustain(-1); 			break;
		case 15: changeEnvRelease(-1); 			break;
		case 16: changeEnvAmount(-1); 			break;
		case 17: changeEnvLoop(-1); 			break;

		}
	}

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
/*	switch(button)
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
*/

	if(IE->mode == mtInstEditModeParams)
	{
		IE->clearAllNodes();
		IE->cancelMultiFrame();

		IE->mode = 1;
		IE->showInstrumentEnv();
		IE->setInstrumentEnvFunct();
	}
	else if(IE->mode == mtInstEditModeEnv)
	{
		IE->clearAllNodes();
		IE->cancelMultiFrame();

		IE->mode = 0;
		IE->showInstrumentParams();
		IE->setInstrumentParamsFunct();
	}


	IE->activateLabelsBorder();

	return 1;
}

//======================================================================================================================

void changeEnvList(int16_t value)
{

	if(IE->selectedEnvelope + value < 0) IE->selectedEnvelope = 0;
	else if(IE->selectedEnvelope + value > 1 ) IE->selectedEnvelope = 1;
	else IE-> selectedEnvelope += value;

	IE->showInstrumentEnv();

	IE->showEnvList();

}

void changeEnvState(int16_t value)
{



	IE->showEnvState();
}

void changeEnvAttack(int16_t value)
{
	value = value * 100;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].attack + value < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].attack = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].attack + value > ATTACK_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].attack = ATTACK_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].attack += value;
	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
	IE->showEnvAttack();
}

void changeEnvDecay(int16_t value)
{
	value = value * 100;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].decay + value < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].decay = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].decay + value > DECAY_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].decay = DECAY_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].decay += value;
	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
	IE->showEnvDecay();
}

void changeEnvSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].sustain + fVal < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].sustain + fVal > SUSTAIN_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = SUSTAIN_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].sustain += fVal;
	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
	IE->showEnvSustain();
}

void changeEnvRelease(int16_t value)
{
	value = value * 100;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].release + value < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].release = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].release + value > RELEASE_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].release = RELEASE_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].release += value;
	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
	IE->showEnvRelease();
}

void changeEnvAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].amount + fVal < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].amount = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].amount + fVal > AMOUNT_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].amount = AMOUNT_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].amount += fVal;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].amount == 0)
	{
		IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
	}
	else
	{
		IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 1;
	}
	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
	IE->showEnvAmount();
}



void changeEnvLoop(int16_t value)
{


	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
	IE->showEnvLoop();
}



void changeFilterFilterType(int16_t value)
{

	if(IE->filterModeListPos + value < 0) IE->filterModeListPos = 0;
	else if(IE->filterModeListPos + value > filterModeCount-1) IE->filterModeListPos = filterModeCount-1;
	else IE->filterModeListPos += value;

	if(IE->filterModeListPos == 0)
	{
		IE->editorInstrument->filterEnable = 0;
		IE->editorInstrument->filterType = lowPass;
	}
	else if(IE->filterModeListPos == 1)
	{
		IE->editorInstrument->filterEnable = 1;
		IE->editorInstrument->filterType = lowPass;
	}
	else if(IE->filterModeListPos == 2)
	{
		IE->editorInstrument->filterEnable = 1;
		IE->editorInstrument->filterType = highPass;
	}
	else if(IE->filterModeListPos >= filterModeCount-1)
	{
		IE->editorInstrument->filterEnable = 1;
		IE->editorInstrument->filterType = bandPass;
	}

	display.setControlText(IE->topLabel[4], filterModeFunctLabels[IE->filterModeListPos]);
	display.refreshControl(IE->topLabel[4]);

	display.setControlValue(IE->filterModeListControl, IE->filterModeListPos);
	display.refreshControl(IE->filterModeListControl);
	//showFilterFilterType();
	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

uint8_t bgB = 128;

void changeFilterCutOff(int16_t value)
{
//	if(bgB + value < 0) bgB = 0;
//	else if(bgB + value > 128 ) bgB = 128;
//	else bgB += value;
	float fVal = value * 0.01;

	if(IE->editorInstrument->cutOff + fVal < MIN_CUTOFF) IE->editorInstrument->cutOff = MIN_CUTOFF;
	else if(IE->editorInstrument->cutOff + fVal > MAX_CUTOFF ) IE->editorInstrument->cutOff = MAX_CUTOFF;
	else IE->editorInstrument->cutOff += fVal;

//	display.setBacklightBrightness(map(editorInstrument->cutOff,MIN_CUTOFF,MAX_CUTOFF,0,128));

	instrumentPlayer[0].setStatusBytes(CUTOFF_MASK);

	IE->showFilterCutOff();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void changeFilterResonance(int16_t value)
{
	float fVal = value * 0.05;

	if(IE->editorInstrument->resonance + fVal < RESONANCE_MIN) IE->editorInstrument->resonance = RESONANCE_MIN;
	else if(IE->editorInstrument->resonance + fVal > RESONANCE_MAX) IE->editorInstrument->resonance = RESONANCE_MAX;
	else IE->editorInstrument->resonance += fVal;

	instrumentPlayer[0].setStatusBytes(RESONANCE_MASK);

	IE->showFilterResonance();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void changeParamsVolume(int16_t value)
{
	if(IE->editorInstrument->volume + value < 0) IE->editorInstrument->volume = 0;
	else if(IE->editorInstrument->volume + value > MAX_INSTRUMENT_VOLUME) IE->editorInstrument->volume = MAX_INSTRUMENT_VOLUME;
	else IE->editorInstrument->volume += value;

	instrumentPlayer[0].setStatusBytes(VOLUME_MASK);

	IE->showParamsVolume();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void changeParamsTune(int16_t value)
{
	if(IE->editorInstrument->tune + value < MIN_INSTRUMENT_TUNE) IE->editorInstrument->tune = MIN_INSTRUMENT_TUNE;
	else if(IE->editorInstrument->tune + value > MAX_INSTRUMENT_TUNE) IE->editorInstrument->tune = MAX_INSTRUMENT_TUNE;
	else IE->editorInstrument->tune += value;

	instrumentPlayer[0].setStatusBytes(TUNE_MASK);

	IE->showParamsTune();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void changeParamsFineTune(int16_t value)
{
	if(IE->editorInstrument->fineTune + value < MIN_INSTRUMENT_FINETUNE) IE->editorInstrument->fineTune = MIN_INSTRUMENT_FINETUNE;
	else if(IE->editorInstrument->fineTune + value > MAX_INSTRUMENT_FINETUNE) IE->editorInstrument->fineTune = MAX_INSTRUMENT_FINETUNE;
	else IE->editorInstrument->fineTune += value;

	instrumentPlayer[0].setStatusBytes(FINETUNE_MASK);

	IE->showParamsFineTune();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void changeParamsGlide(int16_t value)
{
	value = value * 100;

	if(IE->editorInstrument->glide + value < GLIDE_MIN) IE->editorInstrument->glide = GLIDE_MIN;
	else if(IE->editorInstrument->glide + value > GLIDE_MAX ) IE->editorInstrument->glide = GLIDE_MAX;
	else IE->editorInstrument->glide += value;

	IE->showParamsGlide();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

void changeParamsPanning(int16_t value)
{
	if(IE->editorInstrument->panning + value < PANNING_MIN) IE->editorInstrument->panning = PANNING_MIN;
	else if(IE->editorInstrument->panning + value > PANNING_MAX ) IE->editorInstrument->panning = PANNING_MAX;
	else IE->editorInstrument->panning += value;

	instrumentPlayer[0].setStatusBytes(PANNING_MASK);

	IE->showParamsPanning();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}


void changeParamsReverbSend(int16_t value)
{
	if(IE->editorInstrument->reverbSend + value < REVERB_SEND_MIN) IE->editorInstrument->reverbSend = REVERB_SEND_MIN;
	else if(IE->editorInstrument->reverbSend + value > REVERB_SEND_MAX ) IE->editorInstrument->reverbSend = REVERB_SEND_MAX;
	else IE->editorInstrument->reverbSend += value;

	instrumentPlayer[0].setStatusBytes(REVERB_SEND_MASK);

	IE->showParamsReverbSend();

	fileManager.instrumentIsChangedFlag[mtProject.values.lastUsedInstrument]= 1;
	mtProject.values.projectNotSavedFlag = 1;
}

//======================================================================================================================
//==============================================================================================

void cInstrumentEditor::cancelPopups()
{
	if(mtPopups.getStepPopupState() != stepPopupNone)
	{
		mtPopups.hideStepPopups();

		setDefaultScreenFunct();

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

		IE->editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];

		IE->cancelPopups();


	}
	else if(state == buttonPress)
	{
		mtPopups.showStepPopup(stepPopupInstr, mtProject.values.lastUsedInstrument);
		IE->lightUpPadBoard();
	}

	return 1;
}

static uint8_t functStepNote(uint8_t value)
{
	if(value == buttonRelease)
	{
		IE->cancelPopups();

	}
	else if(value == buttonPress)
	{
		mtPopups.showStepPopup(stepPopupNote, -1);
		IE->lightUpPadBoard();
	}

	return 1;
}



