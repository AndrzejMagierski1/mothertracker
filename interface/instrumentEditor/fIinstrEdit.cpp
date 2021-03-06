

#include "instrumentEditor/instrumentEditor.h"

#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtPadsBacklight.h"
#include "keyScanner.h"


#include "core/interfacePopups.h"

#include "fileManager.h"
//#include "mtFileManager.h"


cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;



static  uint8_t functPlayAction();
static  uint8_t functDelete(uint8_t state);

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

static uint8_t functSwitchToSends(uint8_t state);

static uint8_t functStepNote(uint8_t value);


static uint8_t functSendSelectReverb();
static uint8_t functSendSelectDelay();
static uint8_t functSendBack();


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
void changeParamsDelaySend(int16_t value);
void changeParamsReverbSend(int16_t value);

void changeParamsVelocity(int16_t value);


void cInstrumentEditor::addNode(editFunct_t funct , uint8_t nodeNum, uint8_t reverseInput)
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

	isSendsWindow = false;
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
	FM->setButtonObj(interfaceButtonDelete, functDelete);

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
			if ((!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) && (IE->selectedPlace[IE->mode] > 1)) IE->selectedPlace[IE->mode] = 1;


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
	sequencer.stopManualNotes();
	moduleRefresh = 0;
	mtPadBoard.releaseInstrumentsTakenByPads();
	padsBacklight.clearAllPads(1, 1, 1);
}



void cInstrumentEditor::setDefaultScreenFunct()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	for(uint8_t i = interfaceButton0; i < interfaceButton7; i++)
	{
		FM->setButtonObj(i, functSelectParams);
	}
	if(mode == mtInstEditModeEnv) FM->setButtonObj(interfaceButton7, functSelectParams);
	else FM->setButtonObj(interfaceButton7, functSwitchToSends);


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

void cInstrumentEditor::switchToSendScreen()
{
	isSendsWindow = true;
	setSendScreenFunct();
	showSendScreen();
}
void cInstrumentEditor::setSendScreenFunct()
{
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->setButtonObj(interfaceButton0, buttonPress, functSendSelectReverb);
	FM->setButtonObj(interfaceButton1, buttonPress, functSendSelectDelay);

	FM->setButtonObj(interfaceButton7, buttonPress, functSendBack);
}

//==============================================================================================================

static uint8_t functSelectParams(uint8_t button, uint8_t state)
{
	uint8_t mode_places = button + IE->mode*10;

	if(IE->mode == mtInstEditModeEnv)
	{
		if(state == buttonPress) // zmienianie pozycji listy przyciskiem pod ekranem
		{
			IE->rollListOver = true;

			if(button == 2 && IE->selectedPlace[IE->mode] == 1) changeEnvState(1);
			if(button == 3 && IE->selectedPlace[IE->mode] == 2)
				if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoShape(1);
			if(button == 4 && IE->selectedPlace[IE->mode] == 3)
				if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) changeLfoSpeed(1);

			IE->rollListOver = false;
		}

		if((button > 2) && (!IE->editorInstrument->envelope[IE->selectedEnvelope].enable)) return 1;
	}

	if(IE->mode == mtInstEditModeParams)
	{
		if(state == buttonPress) // zmienianie pozycji listy przyciskiem pod ekranem
		{
			if(button == 4 && IE->selectedPlace[IE->mode] == 4)
			{
				IE->rollListOver = true;
				changeFilterFilterType(1);
				IE->rollListOver = false;
			}
		}

		if( (button == 5) || (button == 6) )
		{
			if((!IE->selectNodes[button].isActive) && (!IE->editorInstrument->filterEnable)) return 1;
		}
	}

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

					if( (!IE->editorInstrument->filterEnable) && ((button == 5) || (button == 6)) && (IE->mode == mtInstEditModeParams))
					{
						IE->selectedPlace[IE->mode] = 4;
					}
				}

			}
		}


	}

	IE->activateLabelsBorder();

	return 1;
}

static uint8_t functSwitchToSends(uint8_t state)
{
	if(state == buttonPress) IE->switchToSendScreen();
	return 1;
}

//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{
	if(IE->isSendsWindow)
	{
		switch(IE->selectedPlaceSends)
		{
			case 0: changeParamsReverbSend(value);		break;
			case 1: changeParamsDelaySend(value);		break;
			default: break;
		}
		return 1;
	}

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

	if(IE->isSendsWindow)
	{
		if(IE->selectedPlaceSends  > 0) IE->selectedPlaceSends--;
		IE->showSendScreenFrame();
		return 1;
	}

	if(IE->selectedPlace[IE->mode] > 0) IE->selectedPlace[IE->mode]--;

	if( IE->mode == mtInstEditModeParams)
	{
		if(IE->selectedPlace[IE->mode] == 6)
		{
			if(!IE->editorInstrument->filterEnable)
			{
				IE->selectedPlace[IE->mode] = 4;
			}
		}
	}

	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(IE->frameData.multiSelActiveNum != 0) return 1;

	if(IE->isSendsWindow)
	{
		if(IE->selectedPlaceSends  < IE->frameData.placesCount - 1) IE->selectedPlaceSends++;
		IE->showSendScreenFrame();
		return 1;
	}

	if(IE->selectedPlace[IE->mode] < IE->frameData.placesCount-1) IE->selectedPlace[IE->mode]++;

	if( IE->mode == mtInstEditModeParams)
	{
		if(IE->selectedPlace[IE->mode] == 5)
		{
			if(!IE->editorInstrument->filterEnable)
			{
				IE->selectedPlace[IE->mode] = 4;
			}
		}
	}

	if(IE->mode == mtInstEditModeEnv)
	{
		if ((!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) && (IE->selectedPlace[IE->mode] > 1)) IE->selectedPlace[IE->mode] = 1;
		else if((IE->selectedPlace[IE->mode] > 4) && (IE->editorInstrument->envelope[IE->selectedEnvelope].loop)) IE->selectedPlace[IE->mode] = 4;
	}

	IE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(IE->isSendsWindow)
	{
		switch(IE->selectedPlaceSends)
		{
			case 0: changeParamsReverbSend(1);		break;
			case 1: changeParamsDelaySend(1);		break;
			default: break;
		}
		return 1;
	}

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
	if(IE->isSendsWindow)
	{
		switch(IE->selectedPlaceSends)
		{
			case 0: changeParamsReverbSend(-1);		break;
			case 1: changeParamsDelaySend(-1);		break;
			default: break;
		}
		return 1;
	}

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
	if (sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
		}
		else
		{
			sequencer.playPattern();
		}
	}
	else
	{
		sequencer.stop();
	}

	return 1;
}

static  uint8_t functDelete(uint8_t state)
{
	if(state == buttonPress)
	{
		if(IE->isSendsWindow)
		{
			switch(IE->selectedPlaceSends)
			{
				case 0: IE->setDefaultReverbSend();		break;
				case 1: IE->setDefaultDelaySend();		break;
			}

			return 1;
		}


		if(IE->frameData.multiSelActiveNum != 0)
		{
			for(uint8_t i = 0 ; i < 8 ; i++)
			{
				if(IE->selectNodes[i].isActive)
				{
					uint8_t mode_places = i + IE->mode*10;
					if(IE->paramsMode == mtInstEditMidi) mode_places = i+20;
					if(IE->mode == mtInstEditModeEnv) mode_places--;
					IE->setDefaultValue(mode_places);
				}
			}
		}
		else
		{
			uint8_t mode_places = IE->selectedPlace[IE->mode] + IE->mode*10;
			if(IE->paramsMode == mtInstEditMidi) mode_places = IE->selectedPlace[IE->mode]+20;
			IE->setDefaultValue(mode_places);
		}

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
	else if(IE->selectedEnvelope + value > 5 ) IE->selectedEnvelope = 5;
	else IE-> selectedEnvelope += value;

	IE->showInstrumentEnv();

	IE->showEnvList();

}

void changeEnvState(int16_t value)
{
	if(value > 0)
	{
		if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable == 0)
		{
			IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 1;
			IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 0;
		}
		else if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop == 0) IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 1;
		else if(IE->rollListOver)
		{
			IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
			IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 0;
		}
	}
	else if(value < 0)
	{
		if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop == 1)
		{
			IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 0;
			IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 1;
		}
		else if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable == 1) IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
//		else if(IE->rollListOver)
//		{
//			IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
//			IE->editorInstrument->envelope[IE->selectedEnvelope].loop = 0;
//		}
	}
//	if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable + value < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 0;
//	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].enable + value > 1 ) IE->editorInstrument->envelope[IE->selectedEnvelope].enable = 1;
//	else IE->editorInstrument->envelope[IE->selectedEnvelope].enable += value;

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showInstrumentEnv();
	IE->showEnvState();
}

void changeEnvAttack(int16_t value)
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	uint16_t localCurrentValue = IE->editorInstrument->envelope[IE->selectedEnvelope].attack > ENVELOPE_MICRO_RANGE ?
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].attack,ENVELOPE_MICRO_RANGE,ATTACK_MAX,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ((ATTACK_MAX - ENVELOPE_MICRO_RANGE)/100)):
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].attack,0,ENVELOPE_MICRO_RANGE,0,ENVELOPE_MICRO_VAL);

	if(localCurrentValue + value > (ENVELOPE_MICRO_VAL + ((ATTACK_MAX - ENVELOPE_MICRO_RANGE )/100))) localCurrentValue = ENVELOPE_MICRO_VAL + ( (ATTACK_MAX - ENVELOPE_MICRO_RANGE)/100);
	else if(localCurrentValue + value < 0) localCurrentValue = 0;
	else localCurrentValue += value;

	IE->editorInstrument->envelope[IE->selectedEnvelope].attack = localCurrentValue > ENVELOPE_MICRO_VAL ?
			map(localCurrentValue,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ( (ATTACK_MAX - ENVELOPE_MICRO_RANGE)/100),ENVELOPE_MICRO_RANGE,ATTACK_MAX):
			map(localCurrentValue,0,ENVELOPE_MICRO_VAL,0,ENVELOPE_MICRO_RANGE);

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showEnvAttack();
}

void changeEnvDecay(int16_t value)
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	uint16_t localCurrentValue = IE->editorInstrument->envelope[IE->selectedEnvelope].decay > ENVELOPE_MICRO_RANGE ?
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].decay,ENVELOPE_MICRO_RANGE,DECAY_MAX,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ((DECAY_MAX - ENVELOPE_MICRO_RANGE)/100)):
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].decay,0,ENVELOPE_MICRO_RANGE,0,ENVELOPE_MICRO_VAL);

	if(localCurrentValue + value > (ENVELOPE_MICRO_VAL + ((DECAY_MAX - ENVELOPE_MICRO_RANGE )/100))) localCurrentValue = ENVELOPE_MICRO_VAL + ( (DECAY_MAX - ENVELOPE_MICRO_RANGE)/100);
	else if(localCurrentValue + value < 0) localCurrentValue = 0;
	else localCurrentValue += value;

	IE->editorInstrument->envelope[IE->selectedEnvelope].decay = localCurrentValue > ENVELOPE_MICRO_VAL ?
			map(localCurrentValue,ENVELOPE_MICRO_VAL ,ENVELOPE_MICRO_VAL + ( (DECAY_MAX - ENVELOPE_MICRO_RANGE)/100),ENVELOPE_MICRO_RANGE,DECAY_MAX):
			map(localCurrentValue,0,ENVELOPE_MICRO_VAL,0,ENVELOPE_MICRO_RANGE);

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showEnvDecay();
}

void changeEnvSustain(int16_t value)
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	float fVal = value * 0.01;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].sustain + fVal < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].sustain + fVal > SUSTAIN_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = SUSTAIN_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].sustain += fVal;

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showEnvSustain();
}

void changeEnvRelease(int16_t value)
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	uint16_t localCurrentValue = IE->editorInstrument->envelope[IE->selectedEnvelope].release > ENVELOPE_MICRO_RANGE ?
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].release,ENVELOPE_MICRO_RANGE,RELEASE_MAX,ENVELOPE_MICRO_VAL,ENVELOPE_MICRO_VAL + ((RELEASE_MAX - ENVELOPE_MICRO_RANGE)/100)):
			map(IE->editorInstrument->envelope[IE->selectedEnvelope].release,0,ENVELOPE_MICRO_RANGE,0,ENVELOPE_MICRO_VAL);

	if(localCurrentValue + value > (ENVELOPE_MICRO_VAL + ((RELEASE_MAX - ENVELOPE_MICRO_RANGE )/100))) localCurrentValue = ENVELOPE_MICRO_VAL + ( (RELEASE_MAX - ENVELOPE_MICRO_RANGE)/100);
	else if(localCurrentValue + value < 0) localCurrentValue = 0;
	else localCurrentValue += value;

	IE->editorInstrument->envelope[IE->selectedEnvelope].release = localCurrentValue > ENVELOPE_MICRO_VAL ?
			map(localCurrentValue,ENVELOPE_MICRO_VAL, ENVELOPE_MICRO_VAL + ( (RELEASE_MAX - ENVELOPE_MICRO_RANGE)/100),ENVELOPE_MICRO_RANGE,RELEASE_MAX):
			map(localCurrentValue,0,ENVELOPE_MICRO_VAL,0,ENVELOPE_MICRO_RANGE);

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showEnvRelease();
}

void changeEnvAmount(int16_t value)
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	float fVal = value * 0.01;

	if(IE->editorInstrument->envelope[IE->selectedEnvelope].amount + fVal < 0) IE->editorInstrument->envelope[IE->selectedEnvelope].amount = 0;
	else if(IE->editorInstrument->envelope[IE->selectedEnvelope].amount + fVal > AMOUNT_MAX ) IE->editorInstrument->envelope[IE->selectedEnvelope].amount = AMOUNT_MAX;
	else IE->editorInstrument->envelope[IE->selectedEnvelope].amount += fVal;

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showEnvAmount();
}

void changeLfoShape(int16_t value)
{
	if(IE->editorInstrument->lfo[IE->selectedEnvelope].shape + value < 0) IE->editorInstrument->lfo[IE->selectedEnvelope].shape = 0;
	else if(IE->editorInstrument->lfo[IE->selectedEnvelope].shape + value > (lfoShapeCount - 1) )
	{
		IE->editorInstrument->lfo[IE->selectedEnvelope].shape = (IE->rollListOver ? 0 : lfoShapeCount - 1);
	}
	else IE->editorInstrument->lfo[IE->selectedEnvelope].shape += value;

	uint32_t statusByte = 0;
	switch(IE->selectedEnvelope)
	{
		case envAmp: 		statusByte = LFO_AMP_SEND_MASK;					break;
		case envCutoff:		statusByte = LFO_FILTER_SEND_MASK;			 	break;
		case envWtPos: 		statusByte = LFO_WT_POS_SEND_MASK;				break;
		case envGranPos:	statusByte = LFO_GRAN_POS_SEND_MASK;			break;
		case envPan:		statusByte = LFO_PANNING_SEND_MASK; 			break;
		case envFinetune:	statusByte = LFO_FINETUNE_SEND_MASK; 			break;
		default: break;
	}
	for (int i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(statusByte);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showLfoShape();
}
void changeLfoSpeed(int16_t value)
{
	uint8_t speedConstrain = IE->selectedEnvelope == envAmp ? CONSTRAIN_SPEED_AMP_ENVELOPE : CONSTRAIN_SPEED_OTHER_ENVELOPE;

	if(IE->editorInstrument->lfo[IE->selectedEnvelope].speed + value < 0) IE->editorInstrument->lfo[IE->selectedEnvelope].speed = 0;
	else if(IE->editorInstrument->lfo[IE->selectedEnvelope].speed + value > speedConstrain )
	{
		IE->editorInstrument->lfo[IE->selectedEnvelope].speed = (IE->rollListOver ? 0 : speedConstrain);
	}
	else IE->editorInstrument->lfo[IE->selectedEnvelope].speed += value;

	uint32_t statusByte = 0;
	switch(IE->selectedEnvelope)
	{
		case envAmp: 		statusByte = LFO_AMP_SEND_MASK;					break;
		case envCutoff:		statusByte = LFO_FILTER_SEND_MASK;			 	break;
		case envWtPos: 		statusByte = LFO_WT_POS_SEND_MASK;				break;
		case envGranPos:	statusByte = LFO_GRAN_POS_SEND_MASK;			break;
		case envPan:		statusByte = LFO_PANNING_SEND_MASK; 			break;
		case envFinetune:	statusByte = LFO_FINETUNE_SEND_MASK; 			break;
		default: break;
	}
	for (int i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(statusByte);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showLfoSpeed();
}
void changeLfoAmount(int16_t value)
{
	float fVal = value * 0.01;

	if(IE->editorInstrument->lfo[IE->selectedEnvelope].amount + fVal < 0) IE->editorInstrument->lfo[IE->selectedEnvelope].amount = 0;
	else if(IE->editorInstrument->lfo[IE->selectedEnvelope].amount + fVal > AMOUNT_MAX ) IE->editorInstrument->lfo[IE->selectedEnvelope].amount = AMOUNT_MAX;
	else IE->editorInstrument->lfo[IE->selectedEnvelope].amount += fVal;

	uint32_t statusByte = 0;
	switch(IE->selectedEnvelope)
	{
		case envAmp: 		statusByte = LFO_AMP_SEND_MASK;					break;
		case envCutoff:		statusByte = LFO_FILTER_SEND_MASK;			 	break;
		case envWtPos: 		statusByte = LFO_WT_POS_SEND_MASK;				break;
		case envGranPos:	statusByte = LFO_GRAN_POS_SEND_MASK;			break;
		case envPan:		statusByte = LFO_PANNING_SEND_MASK; 			break;
		case envFinetune:	statusByte = LFO_FINETUNE_SEND_MASK; 			break;
		default: break;
	}
	for (int i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(statusByte);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

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
	else if(IE->filterModeListPos + value > filterModeCount-1) IE->filterModeListPos = ( IE->rollListOver ? 0 : filterModeCount-1);
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

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showFilterType();
}

void changeFilterCutOff(int16_t value)
{
//	if(bgB + value < 0) bgB = 0;
//	else if(bgB + value > 128 ) bgB = 128;
//	else bgB += value;

	if(!IE->editorInstrument->filterEnable) return;
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

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showFilterCutOff();
}

void changeFilterResonance(int16_t value)
{
	if(!IE->editorInstrument->filterEnable) return;

	float fVal = value * (RESONANCE_MAX-RESONANCE_MIN)/100;

	if(IE->editorInstrument->resonance + fVal < RESONANCE_MIN) IE->editorInstrument->resonance = RESONANCE_MIN;
	else if(IE->editorInstrument->resonance + fVal > RESONANCE_MAX) IE->editorInstrument->resonance = RESONANCE_MAX;
	else IE->editorInstrument->resonance += fVal;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(RESONANCE_MASK);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showFilterResonance();
}

void changeParamsVolume(int16_t value)
{
	if(IE->editorInstrument->volume + value < 0) IE->editorInstrument->volume = 0;
	else if(IE->editorInstrument->volume + value > MAX_INSTRUMENT_VOLUME) IE->editorInstrument->volume = MAX_INSTRUMENT_VOLUME;
	else IE->editorInstrument->volume += value;

	for(uint8_t i = 0; i < 8; i++)
	{
//		if(instrumentPlayer[i].currentInstrument_idx == mtProject.values.lastUsedInstrument)
		   instrumentPlayer[i].setStatusBytes(VOLUME_MASK);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsVolume();
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

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsTune();
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

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsFineTune();
}

void changeParamsGlide(int16_t value)
{
	value = value * 100;

	if(IE->editorInstrument->glide + value < GLIDE_MIN) IE->editorInstrument->glide = GLIDE_MIN;
	else if(IE->editorInstrument->glide + value > GLIDE_MAX ) IE->editorInstrument->glide = GLIDE_MAX;
	else IE->editorInstrument->glide += value;

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsGlide();
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

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsPanning();
}


void changeParamsDelaySend(int16_t value)
{
	if(IE->editorInstrument->delaySend + value < SEND_MIN) IE->editorInstrument->delaySend = SEND_MIN;
	else if(IE->editorInstrument->delaySend + value > SEND_MAX ) IE->editorInstrument->delaySend = SEND_MAX;
	else IE->editorInstrument->delaySend += value;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(DELAY_SEND_MASK);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsDelaySend();
}


void changeParamsReverbSend(int16_t value)
{
	if(IE->editorInstrument->reverbSend + value < SEND_MIN) IE->editorInstrument->reverbSend = SEND_MIN;
	else if(IE->editorInstrument->reverbSend + value > SEND_MAX ) IE->editorInstrument->reverbSend = SEND_MAX;
	else IE->editorInstrument->reverbSend += value;

	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(REVERB_SEND_MASK);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showParamsReverbSend();
}


void changeParamsVelocity(int16_t value)
{
	uint8_t* temp_velocity =  &mtProject.values.midiInstrument[mtProject.values.lastUsedInstrument-INSTRUMENTS_COUNT].velocity;

	if(*temp_velocity + value < 0) *temp_velocity = 0;
	else if(*temp_velocity + value > 127) *temp_velocity = 127;
	else *temp_velocity += value;

	IE->showParamsVelocity();

	newFileManager.setProjectStructChanged();
}


//======================================================================================================================
//==============================================================================================

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
//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		sequencer.stop();
//		for(uint8_t i = 0; i < 8; i++)
//		{
//			instrumentPlayer[i].noteOff(Sequencer::STEP_NOTE_CUT);
//		}
//	}

	if(state == buttonPress)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;

		padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);
		uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
		sequencer.handleNoteOn(
							Sequencer::GRID_OUTSIDE_PATTERN,
							noteFromPad,
							-1,
							pad);
//		mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);

	}
	else if(state == buttonRelease)
	{
		padsBacklight.setFrontLayer(0,0, pad);
//		mtPadBoard.stopInstrument(pad);
		uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
		sequencer.handleNoteOff(Sequencer::GRID_OUTSIDE_PATTERN, noteFromPad, 0, pad);
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

void cInstrumentEditor::setDefaultValue(uint8_t place)
{
	switch(place)
	{
		case 0:  	setDefaultVolume(); 		break;
		case 1:  	setDefaultPanning(); 		break;
		case 2:  	setDefaultTune(); 			break;
		case 3:  	setDefaultFinetune(); 		break;
		case 4:  	setDefaultFilterType(); 	break;
		case 5:  	setDefaultCutoff(); 		break;
		case 6:  	setDefaultResonance(); 		break;
		case 7:  	setDefaultDelaySend(); 			break;
		case 12:
			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) setDefaultLfoShape();
			else setDefaultEnvAttack();
		break;
		case 13:
			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) setDefaultLfoSpeed();
			else setDefaultEnvDecay();
			break;
		case 14:
			if(IE->editorInstrument->envelope[IE->selectedEnvelope].loop) setDefaultLfoAmount();
			else setDefaultEnvSustain();
			break;
		case 15:
			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) setDefaultEnvRelease();
			break;
		case 16:
			if(!IE->editorInstrument->envelope[IE->selectedEnvelope].loop) setDefaultEnvAmount();
			break;
		case 20: 	setDefaultMidiVelocity(); 	break;
		default: break;
	}
}

static uint8_t functSendSelectReverb()
{
	IE->selectedPlaceSends = 0;
	IE->showSendScreenFrame();

	return 1;
}
static uint8_t functSendSelectDelay()
{
	IE->selectedPlaceSends = 1;
	IE->showSendScreenFrame();
	return 1;
}
static uint8_t functSendBack()
{
	IE->isSendsWindow = false;
	IE->setDefaultScreenFunct();
	IE->showInstrumentParams();
	return 1;
}

void cInstrumentEditor::setDefaultVolume()
{
	IE->editorInstrument->volume = defaultInstrumentParams.volume;
	for(uint8_t i = 0; i < 8; i++)
	{
		   instrumentPlayer[i].setStatusBytes(VOLUME_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showParamsVolume();
}
void cInstrumentEditor::setDefaultPanning()
{
	IE->editorInstrument->panning = defaultInstrumentParams.panning;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(PANNING_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showParamsPanning();
}
void cInstrumentEditor::setDefaultTune()
{
	IE->editorInstrument->tune = defaultInstrumentParams.tune;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(TUNE_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showParamsTune();
}
void cInstrumentEditor::setDefaultFinetune()
{
	IE->editorInstrument->fineTune = defaultInstrumentParams.fineTune;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(FINETUNE_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showParamsFineTune();
}
void cInstrumentEditor::setDefaultFilterType()
{
	IE->editorInstrument->filterEnable = defaultInstrumentParams.filterEnable;
	IE->editorInstrument->filterType = defaultInstrumentParams.filterType;

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showFilterType();
}
void cInstrumentEditor::setDefaultCutoff()
{
	if(!IE->editorInstrument->filterEnable) return;
	IE->editorInstrument->cutOff = defaultInstrumentParams.cutOff;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(CUTOFF_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showFilterCutOff();
}
void cInstrumentEditor::setDefaultResonance()
{
	if(!IE->editorInstrument->filterEnable) return;
	IE->editorInstrument->resonance = defaultInstrumentParams.resonance;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(RESONANCE_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showFilterResonance();
}
void cInstrumentEditor::setDefaultDelaySend()
{
	IE->editorInstrument->delaySend = defaultInstrumentParams.delaySend;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(DELAY_SEND_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showParamsDelaySend();
}
void cInstrumentEditor::setDefaultReverbSend()
{
	IE->editorInstrument->reverbSend = defaultInstrumentParams.reverbSend;
	for(uint8_t i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(REVERB_SEND_MASK);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showParamsReverbSend();
}
//env screen
void cInstrumentEditor::setDefaultEnvAttack()
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	IE->editorInstrument->envelope[IE->selectedEnvelope].attack = defaultInstrumentParams.envelope[IE->selectedEnvelope].attack;
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showEnvAttack();
}
void cInstrumentEditor::setDefaultEnvDecay()
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	IE->editorInstrument->envelope[IE->selectedEnvelope].decay = defaultInstrumentParams.envelope[IE->selectedEnvelope].decay;
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showEnvDecay();
}
void cInstrumentEditor::setDefaultEnvSustain()
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	IE->editorInstrument->envelope[IE->selectedEnvelope].sustain = defaultInstrumentParams.envelope[IE->selectedEnvelope].sustain;
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showEnvSustain();
}
void cInstrumentEditor::setDefaultEnvRelease()
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	IE->editorInstrument->envelope[IE->selectedEnvelope].release = defaultInstrumentParams.envelope[IE->selectedEnvelope].release;
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showEnvRelease();
}
void cInstrumentEditor::setDefaultEnvAmount()
{
	if(!IE->editorInstrument->envelope[IE->selectedEnvelope].enable) return;

	IE->editorInstrument->envelope[IE->selectedEnvelope].amount = defaultInstrumentParams.envelope[IE->selectedEnvelope].amount;
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showEnvAmount();
}
//lfo
void cInstrumentEditor::setDefaultLfoShape()
{
	IE->editorInstrument->lfo[IE->selectedEnvelope].shape = defaultInstrumentParams.lfo[IE->selectedEnvelope].shape;
	uint32_t statusByte = 0;
	switch(IE->selectedEnvelope)
	{
		case envAmp: 		statusByte = LFO_AMP_SEND_MASK;					break;
		case envCutoff:		statusByte = LFO_FILTER_SEND_MASK;			 	break;
		case envWtPos: 		statusByte = LFO_WT_POS_SEND_MASK;				break;
		case envGranPos:	statusByte = LFO_GRAN_POS_SEND_MASK;			break;
		case envPan:		statusByte = LFO_PANNING_SEND_MASK; 			break;
		case envFinetune:	statusByte = LFO_FINETUNE_SEND_MASK; 			break;
		default: break;
	}
	for (int i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(statusByte);
	}
	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);
	IE->showLfoShape();
}
void cInstrumentEditor::setDefaultLfoSpeed()
{
	IE->editorInstrument->lfo[IE->selectedEnvelope].speed = defaultInstrumentParams.lfo[IE->selectedEnvelope].speed;

	uint32_t statusByte = 0;
	switch(IE->selectedEnvelope)
	{
		case envAmp: 		statusByte = LFO_AMP_SEND_MASK;					break;
		case envCutoff:		statusByte = LFO_FILTER_SEND_MASK;			 	break;
		case envWtPos: 		statusByte = LFO_WT_POS_SEND_MASK;				break;
		case envGranPos:	statusByte = LFO_GRAN_POS_SEND_MASK;			break;
		case envPan:		statusByte = LFO_PANNING_SEND_MASK; 			break;
		case envFinetune:	statusByte = LFO_FINETUNE_SEND_MASK; 			break;
		default: break;
	}
	for (int i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(statusByte);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showLfoSpeed();
}
void cInstrumentEditor::setDefaultLfoAmount()
{
	IE->editorInstrument->lfo[IE->selectedEnvelope].amount = defaultInstrumentParams.lfo[IE->selectedEnvelope].amount;

	uint32_t statusByte = 0;
	switch(IE->selectedEnvelope)
	{
		case envAmp: 		statusByte = LFO_AMP_SEND_MASK;					break;
		case envCutoff:		statusByte = LFO_FILTER_SEND_MASK;			 	break;
		case envWtPos: 		statusByte = LFO_WT_POS_SEND_MASK;				break;
		case envGranPos:	statusByte = LFO_GRAN_POS_SEND_MASK;			break;
		case envPan:		statusByte = LFO_PANNING_SEND_MASK; 			break;
		case envFinetune:	statusByte = LFO_FINETUNE_SEND_MASK; 			break;
		default: break;
	}
	for (int i = 0; i < 8; i++)
	{
		instrumentPlayer[i].setStatusBytes(statusByte);
	}

	newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

	IE->showLfoAmount();
}
void cInstrumentEditor::setDefaultMidiVelocity()
{
	uint8_t* temp_velocity =  &mtProject.values.midiInstrument[mtProject.values.lastUsedInstrument-INSTRUMENTS_COUNT].velocity;

	*temp_velocity = DEFAULT_MIDI_VELOCITY;

	IE->showParamsVelocity();

	newFileManager.setProjectStructChanged();
}

