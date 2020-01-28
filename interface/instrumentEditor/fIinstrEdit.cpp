

#include "instrumentEditor/instrumentEditor.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtPadsBacklight.h"


#include "core/interfacePopups.h"

#include "mtFileManager.h"


cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;



static  uint8_t functPlayAction();


static  uint8_t functInstrument(uint8_t state);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();



static  uint8_t functEncoder(int16_t value);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchMode(uint8_t button);

static  uint8_t functSelectParams(uint8_t button, uint8_t state);



static uint8_t functStepNote(uint8_t value);


void changeEnvList(int16_t value);
void changeEnvState(int16_t value);
void changeEnvAttack(int16_t value);
void changeEnvDecay(int16_t value);
void changeEnvSustain(int16_t value);
void changeEnvRelease(int16_t value);
void changeEnvAmount(int16_t value);
//void changeEnvLoop(int16_t value);

void changeLfoShape(int16_t value);
void changeLfoSpeed(int16_t value);
void changeLfoAmount(int16_t value);


void changeParamsVolume(int16_t value);
void changeParamsPanning(int16_t value);
void changeParamsTune(int16_t value);
void changeParamsFineTune(int16_t value);
void changeFilterFilterType(int16_t value);
void changeFilterCutOff(int16_t value);
void changeFilterResonance(int16_t value);
void changeParamsReverbSend(int16_t value);

void changeParamsVelocity(int16_t value);


void cInstrumentEditor::addNode(editFunct_t funct , uint8_t nodeNum, uint8_t reverseInput = 0)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].reverseInput = reverseInput;
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cInstrumentEditor::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cInstrumentEditor::stepThroughNodes(int16_t value, uint8_t source) /*source: 1 - przycisk, 0 - enkoder*/
{
	int16_t nodeVal;
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		nodeVal = value;
		if(selectNodes[node].isActive)
		{
			if(selectNodes[node].editFunct != NULL)
			{
				/*odwracanie wartosci dla list przy wprowadzaniu z przycisku*/
				if(source == 1)
				{
					if(selectNodes[node].reverseInput == 1)
					{
						nodeVal = (-nodeVal);
					}
				}

				selectNodes[node].editFunct(nodeVal);
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


	//mtProject.values.lastUsedInstrument = constrain(mtProject.values.lastUsedInstrument, 0, INSTRUMENTS_MAX);

	if(mtProject.values.lastUsedInstrument < INSTRUMENTS_COUNT)
	{
		editorInstrument = &mtProject.instrument[mtProject.values.lastUsedInstrument];
	}


	clearAllNodes();
	cancelMultiFrame();

	//listData();



	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchMode);  //<<<<<<<<< MODE

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

	FM->setButtonObj(interfaceButtonInstr, functInstrument);


	if(mtProject.values.lastUsedInstrument >= INSTRUMENTS_COUNT)
	{
		paramsMode = mtInstEditMidi;
	}
	else
	{
		paramsMode = mtInstEditNormal;
	}

	switch(mode)
	{
		case mtInstEditModeParams:
		{
			if(paramsMode == mtInstEditMidi)
			{
				showInstrumentMidiParams();
				setDefaultScreenFunct();
				return;
			}
			showInstrumentParams();
			setDefaultScreenFunct();
			return;
		}
		case mtInstEditModeEnv:
		{
			if(paramsMode == mtInstEditMidi)
			{
				display.hideAllControls();
				showTitleBar();
				display.synchronizeRefresh();
				clearDefaultScreenFunct();
				return;
			}
			showInstrumentEnv();
			setDefaultScreenFunct();
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
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

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


	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	//FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);
	//FM->setButtonObj(interfaceButtonShift, functShift);
	FM->setButtonObj(interfaceButtonInstr, functInstrument);
	FM->setPadsGlobal(functPads);

}


void cInstrumentEditor::clearDefaultScreenFunct()
{
	FM->clearAllPots();
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->clearButton(interfaceButtonLeft);
	FM->clearButton(interfaceButtonRight);
	FM->clearButton(interfaceButtonUp);
	FM->clearButton(interfaceButtonDown);

}

//==============================================================================================================

static uint8_t functSelectParams(uint8_t button, uint8_t state)
{
	uint8_t mode_places = button + IE->mode*10;

	if(IE->paramsMode == mtInstEditMidi) mode_places = button+20;

	uint8_t node = button;

	if(state == buttonPress)
	{
		uint8_t lastSelectedPlace = IE->selectedPlace[IE->mode];

		IE->selectedPlace[IE->mode] = (button > 0) && (IE->mode == mtInstEditModeEnv)  ? button - 1 : button;

		if( (IE->mode == mtInstEditModeEnv) && (IE->selectedPlace[IE->mode] > 4) && (IE->editorInstrument->envelope[IE->selectedEnvelope].loop))
		{
			IE->selectedPlace[IE->mode] = 4;
		}

		switch(mode_places)
		{
		case 0: IE->addNode(changeParamsVolume, node); 	    	break;
		case 1:	IE->addNode(changeParamsPanning, node); 	    break;
		case 2: IE->addNode(changeParamsTune, node); 			break;
		case 3: IE->addNode(changeParamsFineTune, node); 		break;
		case 4: IE->addNode(changeFilterFilterType, node, 1);   break;
		case 5: IE->addNode(changeFilterCutOff, node); 	    	break;
		case 6: IE->addNode(changeFilterResonance, node);    	break;
		case 7: IE->addNode(changeParamsReverbSend, node);   	break;

		case 10:

			if (lastSelectedPlace == 0 )	changeEnvList(-1);

			break;
		case 11:

			if (lastSelectedPlace == 0 ) 	changeEnvList(1);

		break;
		case 12: break;
		case 13:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) IE->addNode(changeLfoShape, node , 1);
			else IE->addNode(changeEnvAttack, node);

			break;
		case 14:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) IE->addNode(changeLfoSpeed, node , 1);
			else IE->addNode(changeEnvDecay, node);

			break;
		case 15:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) IE->addNode(changeLfoAmount, node);
			else IE->addNode(changeEnvSustain, node);

			break;
		case 16:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) IE->addNode(changeEnvRelease, node);

			break;
		case 17: ;

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) IE->addNode(changeEnvAmount, node);

			break;

		// midi params
		case 20: IE->addNode(changeParamsVelocity, node); 		 break;

		}

		if((node > 2) && (IE->mode == mtInstEditModeEnv))
		{
			if( ((node <= 5 ) && (IE->editorInstrument->envelope[IE->selectedEnvelope].loop)) || (!IE->editorInstrument->envelope[IE->selectedEnvelope].loop ))
			{
				IE->frameData.multisel[button].frameNum = node - 1;
				IE->frameData.multisel[button].isActive = 1;
				IE->frameData.multiSelActiveNum  += 1;
			}
		}
		else if(IE->mode == mtInstEditModeParams)
		{
			IE->frameData.multisel[button].frameNum = node;
			IE->frameData.multisel[button].isActive = 1;
			IE->frameData.multiSelActiveNum  += 1;
		}
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
					IE->selectedPlace[IE->mode] = (button > 0) && (IE->mode == mtInstEditModeEnv) ? button - 1 : button;
					if( (IE->mode == mtInstEditModeEnv) && (IE->selectedPlace[IE->mode] > 4) && (IE->editorInstrument->envelope[IE->selectedEnvelope].loop)) IE->selectedPlace[IE->mode] = 4;
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
		IE->stepThroughNodes(value, 0);
	}
	else
	{
		uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;
		if(IE->paramsMode == mtInstEditMidi) mode_places = IE->selectedPlace[IE->mode]+20;

		switch(mode_places)
		{
		case 0: changeParamsVolume(value); 		 break;
		case 1:	changeParamsPanning(value); 	 break;
		case 2: changeParamsTune(value); 		 break;
		case 3: changeParamsFineTune(value); 	 break;
		case 4: changeFilterFilterType(value);	 break;
		case 5: changeFilterCutOff(value); 		 break;
		case 6: changeFilterResonance(value); 	 break;
		case 7: changeParamsReverbSend(value); 	 break;

		case 10: changeEnvList(value); 		break;
		case 11: changeEnvState(value); 	break;
		case 12:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoShape(value);
			else changeEnvAttack(value);

			break;
		case 13:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoSpeed(value);
			else changeEnvDecay(value);

			break;
		case 14:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoAmount(value);
			else changeEnvSustain(value);

			break;
		case 15:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeEnvRelease(value);

			break;
		case 16:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeEnvAmount(value);

			break;
//		case 17: changeEnvLoop(value); 		break;

		case 20: changeParamsVelocity(value); 	break;
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
	if((IE->selectedPlace[IE->mode] > 4) && ( IE->mode == mtInstEditModeEnv) && (IE->editorInstrument->envelope[IE->selectedEnvelope].loop)) IE->selectedPlace[IE->mode] = 4;
	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(IE->frameData.multiSelActiveNum != 0)
	{
		IE->stepThroughNodes(1, 1);
	}
	else
	{
		uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;
		if(IE->paramsMode == mtInstEditMidi) mode_places = IE->selectedPlace[IE->mode]+20;

		switch(mode_places)
		{
		case 0: changeParamsVolume(1); 		 break;
		case 1:	changeParamsPanning(1); 	 break;
		case 2: changeParamsTune(1); 		 break;
		case 3: changeParamsFineTune(1); 	 break;
		case 4: changeFilterFilterType(-1);  break;
		case 5: changeFilterCutOff(1); 		 break;
		case 6: changeFilterResonance(1); 	 break;
		case 7: changeParamsReverbSend(1); 	 break;

		case 10: changeEnvList(-1); 		break;
		case 11: changeEnvState(-1); 	break;
		case 12:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoShape(-1);
			else changeEnvAttack(1);

			break;
		case 13:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoSpeed(-1);
			else changeEnvDecay(1);

			break;
		case 14:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoAmount(1);
			else changeEnvSustain(1);

			break;
		case 15:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeEnvRelease(1);

			break;
		case 16:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeEnvAmount(1);

			break;
//		case 17: changeEnvLoop(-1); 		break;

		case 20: changeParamsVelocity(1); 	break;
		}
	}

	return 1;
}


static  uint8_t functDown()
{
	if(IE->frameData.multiSelActiveNum != 0)
	{
		IE->stepThroughNodes(-1, 1);
	}
	else
	{
		uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;
		if(IE->paramsMode == mtInstEditMidi) mode_places = IE->selectedPlace[IE->mode]+20;

		switch(mode_places)
		{
		case 0: changeParamsVolume(-1); 		 break;
		case 1:	changeParamsPanning(-1); 		 break;
		case 2: changeParamsTune(-1); 		 	 break;
		case 3: changeParamsFineTune(-1); 	 	 break;
		case 4: changeFilterFilterType(1); 		 break;
		case 5: changeFilterCutOff(-1); 		 break;
		case 6: changeFilterResonance(-1); 	 	 break;
		case 7: changeParamsReverbSend(-1); 	 break;

		case 10: changeEnvList(1); 				break;
		case 11: changeEnvState(1); 			break;
		case 12:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoShape(1);
			else changeEnvAttack(-1);

			break;
		case 13:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoSpeed(1);
			else changeEnvDecay(-1);

			break;
		case 14:

			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoAmount(-1);
			else changeEnvSustain(-1);

			break;
		case 15:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeEnvRelease(-1);

			break;
		case 16:

			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeEnvAmount(-1);

			break;
//		case 17: changeEnvLoop(1); 			break;

		case 20: changeParamsVelocity(-1); 	break;
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
	else
	{
		sequencer.stop();
	}

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
		//IE->showInstrumentEnv();
		//IE->setInstrumentEnvFunct();
	}
	else if(IE->mode == mtInstEditModeEnv)
	{
		IE->clearAllNodes();
		IE->cancelMultiFrame();

		IE->mode = 0;
		//IE->showInstrumentParams();
		//IE->setInstrumentParamsFunct();
	}

	//IE->activateLabelsBorder();

	IE->start(0);


	return 1;
}

//======================================================================================================================

void changeEnvList(int16_t value)
{

	if(IE->selectedEnvelope + value < 0) IE->selectedEnvelope = 0;
	else if(IE->selectedEnvelope + value > 4 ) IE->selectedEnvelope = 4;
	else IE-> selectedEnvelope += value;

	IE->showInstrumentEnv();

	IE->showEnvList();

}

void changeEnvState(int16_t value)
{
	if(value > 0)
	{
		if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable == 0) IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 1;
		else IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 1;
	}
	else if(value < 0)
	{
		if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop == 1) IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 0;
		else IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
	}
//	if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable + value < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
//	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable + value > 1 ) IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 1;
//	else IE->editorInstrument->envelope[IE->selectedEnvelope].enable += value;

	IE->showInstrumentEnv();

	IE->showEnvState();
}

void changeEnvAttack(int16_t value)
{
	uint16_t localCurrentValue = IE->editorInstrument->envelope[IE->selectedEnvelope].attack > ENVELOPE_MICRO_RANGE ?
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].attack,ENVELOPE_MICRO_RANGE,ATTACK_MAX,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ((ATTACK_MAX - ENVELOPE_MICRO_RANGE)/100)):
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].attack,0,ENVELOPE_MICRO_RANGE,0,ENVELOPE_MICRO_VAL);

	if(localCurrentValue + value > (ENVELOPE_MICRO_VAL + ((ATTACK_MAX - ENVELOPE_MICRO_RANGE )/100))) localCurrentValue = ENVELOPE_MICRO_VAL + ( (ATTACK_MAX - ENVELOPE_MICRO_RANGE)/100);
	else if(localCurrentValue + value < 0) localCurrentValue = 0;
	else localCurrentValue += value;

	IE->editorInstrument->envelope[IE->selectedEnvelope].attack = localCurrentValue > ENVELOPE_MICRO_VAL ?
			map(localCurrentValue,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ( (ATTACK_MAX - ENVELOPE_MICRO_RANGE)/100),ENVELOPE_MICRO_RANGE,ATTACK_MAX):
			map(localCurrentValue,0,ENVELOPE_MICRO_VAL,0,ENVELOPE_MICRO_RANGE);

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);

	IE->showEnvAttack();
}

void changeEnvDecay(int16_t value)
{
	uint16_t localCurrentValue = IE->editorInstrument->envelope[IE->selectedEnvelope].decay > ENVELOPE_MICRO_RANGE ?
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].decay,ENVELOPE_MICRO_RANGE,DECAY_MAX,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ((DECAY_MAX - ENVELOPE_MICRO_RANGE)/100)):
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].decay,0,ENVELOPE_MICRO_RANGE,0,ENVELOPE_MICRO_VAL);

	if(localCurrentValue + value > (ENVELOPE_MICRO_VAL + ((DECAY_MAX - ENVELOPE_MICRO_RANGE )/100))) localCurrentValue = ENVELOPE_MICRO_VAL + ( (DECAY_MAX - ENVELOPE_MICRO_RANGE)/100);
	else if(localCurrentValue + value < 0) localCurrentValue = 0;
	else localCurrentValue += value;

	IE->editorInstrument->envelope[IE->selectedEnvelope].decay = localCurrentValue > ENVELOPE_MICRO_VAL ?
			map(localCurrentValue,ENVELOPE_MICRO_VAL ,ENVELOPE_MICRO_VAL + ( (DECAY_MAX - ENVELOPE_MICRO_RANGE)/100),ENVELOPE_MICRO_RANGE,DECAY_MAX):
			map(localCurrentValue,0,ENVELOPE_MICRO_VAL,0,ENVELOPE_MICRO_RANGE);

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);

	IE->showEnvDecay();
}

void changeEnvSustain(int16_t value)
{
	float fVal = value * 0.01;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].sustain + fVal < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].sustain + fVal > SUSTAIN_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = SUSTAIN_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].sustain += fVal;

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);

	IE->showEnvSustain();
}

void changeEnvRelease(int16_t value)
{
	uint16_t localCurrentValue = IE->editorInstrument->envelope[IE->selectedEnvelope].release > ENVELOPE_MICRO_RANGE ?
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].release,ENVELOPE_MICRO_RANGE,RELEASE_MAX,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ((RELEASE_MAX - ENVELOPE_MICRO_RANGE)/100)):
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].release,0,ENVELOPE_MICRO_RANGE,0,ENVELOPE_MICRO_VAL);

	if(localCurrentValue + value > (ENVELOPE_MICRO_VAL + ((RELEASE_MAX - ENVELOPE_MICRO_RANGE )/100))) localCurrentValue = ENVELOPE_MICRO_VAL + ( (RELEASE_MAX - ENVELOPE_MICRO_RANGE)/100);
	else if(localCurrentValue + value < 0) localCurrentValue = 0;
	else localCurrentValue += value;

	IE->editorInstrument->envelope[IE->selectedEnvelope].release = localCurrentValue > ENVELOPE_MICRO_VAL ?
			map(localCurrentValue,ENVELOPE_MICRO_VAL, ENVELOPE_MICRO_VAL + ( (RELEASE_MAX - ENVELOPE_MICRO_RANGE)/100),ENVELOPE_MICRO_RANGE,RELEASE_MAX):
			map(localCurrentValue,0,ENVELOPE_MICRO_VAL,0,ENVELOPE_MICRO_RANGE);

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);

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

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);

	IE->showEnvAmount();
}

void changeLfoShape(int16_t value)
{
	if(IE->editorInstrument->lfo[IE->selectedEnvelope].shape + value < 0) IE->editorInstrument->lfo[IE->selectedEnvelope].shape = 0;
	else if(IE->editorInstrument->lfo[IE->selectedEnvelope].shape + value > 2 ) IE->editorInstrument->lfo[IE->selectedEnvelope].shape = 2;
	else IE->editorInstrument->lfo[IE->selectedEnvelope].shape += value;

	IE->showLfoShape();
}
void changeLfoSpeed(int16_t value)
{
	if(IE->editorInstrument->lfo[IE->selectedEnvelope].speed + value < 0) IE->editorInstrument->lfo[IE->selectedEnvelope].speed = 0;
	else if(IE->editorInstrument->lfo[IE->selectedEnvelope].speed + value > 19 ) IE->editorInstrument->lfo[IE->selectedEnvelope].speed = 19;
	else IE->editorInstrument->lfo[IE->selectedEnvelope].speed += value;

	IE->showLfoSpeed();
}
void changeLfoAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(IE->editorInstrument->lfo[IE->selectedEnvelope].amount + fVal < 0) IE->editorInstrument->lfo[IE->selectedEnvelope].amount = 0;
	else if(IE->editorInstrument->lfo[IE->selectedEnvelope].amount + fVal > AMOUNT_MAX ) IE->editorInstrument->lfo[IE->selectedEnvelope].amount = AMOUNT_MAX;
	else IE->editorInstrument->lfo[IE->selectedEnvelope].amount += fVal;

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);

	IE->showLfoAmount();
}

//void changeEnvLoop(int16_t value)
//{
//
//	if((!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) && (value < 0)) IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 1;
//	else if ((IE->editorInstrument->envelope[IE->selectedEnvelope].loop) && (value > 0)) IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 0;
//
//	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
////	IE->showEnvLoop();
//}



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

	IE->showFilterType();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

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
//	instrumentPlayer[0].instrumentBasedMod.cutoff = IE->editorInstrument->cutOff;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(CUTOFF_MASK);
	}

	IE->showFilterCutOff();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

void changeFilterResonance(int16_t value)
{
	float fVal = value * (RESONANCE_MAX-RESONANCE_MIN)/100;

	if(IE->editorInstrument->resonance + fVal < RESONANCE_MIN) IE->editorInstrument->resonance = RESONANCE_MIN;
	else if(IE->editorInstrument->resonance + fVal > RESONANCE_MAX) IE->editorInstrument->resonance = RESONANCE_MAX;
	else IE->editorInstrument->resonance += fVal;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(RESONANCE_MASK);
	}

	IE->showFilterResonance();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

void changeParamsVolume(int16_t value)
{
	if(IE->editorInstrument->volume + value < 0) IE->editorInstrument->volume = 0;
	else if(IE->editorInstrument->volume + value > MAX_INSTRUMENT_VOLUME) IE->editorInstrument->volume = MAX_INSTRUMENT_VOLUME;
	else IE->editorInstrument->volume += value;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(VOLUME_MASK);
	}

	IE->showParamsVolume();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

void changeParamsTune(int16_t value)
{
	if(IE->editorInstrument->tune + value < MIN_INSTRUMENT_TUNE) IE->editorInstrument->tune = MIN_INSTRUMENT_TUNE;
	else if(IE->editorInstrument->tune + value > MAX_INSTRUMENT_TUNE) IE->editorInstrument->tune = MAX_INSTRUMENT_TUNE;
	else IE->editorInstrument->tune += value;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(TUNE_MASK);
	}

	IE->showParamsTune();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

void changeParamsFineTune(int16_t value)
{
	if(IE->editorInstrument->fineTune + value < MIN_INSTRUMENT_FINETUNE) IE->editorInstrument->fineTune = MIN_INSTRUMENT_FINETUNE;
	else if(IE->editorInstrument->fineTune + value > MAX_INSTRUMENT_FINETUNE) IE->editorInstrument->fineTune = MAX_INSTRUMENT_FINETUNE;
	else IE->editorInstrument->fineTune += value;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(FINETUNE_MASK);
	}

	IE->showParamsFineTune();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

void changeParamsGlide(int16_t value)
{
	value = value * 100;

	if(IE->editorInstrument->glide + value < GLIDE_MIN) IE->editorInstrument->glide = GLIDE_MIN;
	else if(IE->editorInstrument->glide + value > GLIDE_MAX ) IE->editorInstrument->glide = GLIDE_MAX;
	else IE->editorInstrument->glide += value;

	IE->showParamsGlide();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}

void changeParamsPanning(int16_t value)
{
	if(IE->editorInstrument->panning + value < PANNING_MIN) IE->editorInstrument->panning = PANNING_MIN;
	else if(IE->editorInstrument->panning + value > PANNING_MAX ) IE->editorInstrument->panning = PANNING_MAX;
	else IE->editorInstrument->panning += value;


	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(PANNING_MASK);
	}

	IE->showParamsPanning();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}


void changeParamsReverbSend(int16_t value)
{
	if(IE->editorInstrument->reverbSend + value < REVERB_SEND_MIN) IE->editorInstrument->reverbSend = REVERB_SEND_MIN;
	else if(IE->editorInstrument->reverbSend + value > REVERB_SEND_MAX ) IE->editorInstrument->reverbSend = REVERB_SEND_MAX;
	else IE->editorInstrument->reverbSend += value;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(REVERB_SEND_MASK);
	}

	IE->showParamsReverbSend();

	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
}



void changeParamsVelocity(int16_t value)
{
	uint8_t* temp_velocity =  &mtProject.values.midiInstrument[mtProject.values.lastUsedInstrument-INSTRUMENTS_COUNT].velocity;

	if(*temp_velocity + value < 0) *temp_velocity = 0;
	else if(*temp_velocity + value > 127) *temp_velocity = 127;
	else *temp_velocity += value;

	IE->showParamsVelocity();

	IE->setProjectSaveFlags();
}


//======================================================================================================================
//==============================================================================================

void cInstrumentEditor::setProjectSaveFlags()
{
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
}

void cInstrumentEditor::cancelPopups()
{
	if(mtPopups.getStepPopupState() != stepPopupNone)
	{
		mtPopups.hideStepPopups();

		start(0);
	}
}

//======================================================================================================================
//==============================================================================================


//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################


void cInstrumentEditor::lightUpPadBoard()
{
	padsBacklight.clearAllPads(0, 1, 1);


	if(mtProject.values.lastUsedInstrument >= 0 && mtProject.values.lastUsedInstrument < INSTRUMENTS_COUNT)
	{
		padsBacklight.setBackLayer(1, 20, mtProject.values.lastUsedInstrument);
	}

}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
		for(uint8_t i = 0; i < 8; i++)
		{
			instrumentPlayer[i].noteOff(Sequencer::STEP_NOTE_CUT);
		}
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
		//IE->lightUpPadBoard(); // obsluga podswwietleniem przeniesiona do popupow
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
	}

	return 1;
}
