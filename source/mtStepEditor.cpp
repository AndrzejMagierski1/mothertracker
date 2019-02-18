
#include "mtDisplay.h"
#include "AnalogInputs.h"
#include "sdram.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtSequencer.h"


#include "mtInterfaceDefs.h"
#include "mtStepEditor.h"


cMtStepEditor mtStepEditor;


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtStepEditor::update()
{
	if(!refreshStepEditor) return;
	refreshStepEditor = 0;


	//-----------------------------------------------------
	if(stepEditorModeStart)
	{
		stepEditorModeStart = 0;
		labelsChanged = 2;
		stepParametersChanged = 2;

		updateButtonsFunctions();
		updatePotsFunctions();
	}
	//-----------------------------------------------------
	if(labelsChanged)
	{
		if(labelsChanged == 2)
		{
			updateButtonsFunctions();
			updatePotsFunctions();
			mtDisplay.setPotsLabels(1);
			mtDisplay.setButtonsLabels(1);
		}

		labelsChanged = 0;
		processLabels();
	}
	if(stepParametersChanged)
	{
		if(stepParametersChanged == 2)
		{
			updateButtonsFunctions();
			updatePotsFunctions();
			mtDisplay.setTrackTable(1);
		}

		stepParametersChanged = 0;
		processStepParameters();

		mtDisplay.changeTrackTable(&trackTable);
	}

	//-----------------------------------------------------


}


void cMtStepEditor::showStep(uint8_t track, uint8_t step)
{
	stepEditorModeStart = 1;
	refreshStepEditor = 1;
	actualTrack = track;
	actualStep = step;

//	mtPrint("track:");
//	mtPrint(actualTrack);
//	mtPrint("  step:");
//	mtPrintln(actualstep);
}

void cMtStepEditor::stop()
{
	mtDisplay.setTrackTable(0);


}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtStepEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
	if(type == 1)
	{
		if(n == interfacePadInstrumentEditor)
		{
			stop();
			eventFunct(mtStepEditorEventPadPress, &n, 0, 0);
		}
		if(n == interfacePadPlay || n == interfacePadStop)
		{
			eventFunct(mtStepEditorEventPadPress, &n, 0, 0);
		}
	}

	return 0;
}

void cMtStepEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunction[button])
	{
	case buttonFunctNone				:		break;
	case buttonFunctPlay            	:		break;
	case buttonFunctStop            	:		break;
	case buttonFunctChangeStepParamsSel:	changeStepParamsSelection(value);	break;
	case buttonFunctShowNextStepFx1	:	changeStepFxSelection(1,value);		break;
	case buttonFunctShowNextStepFx2	:	changeStepFxSelection(2,value);		break;
	case buttonFunctShowNextStepFx3	:	changeStepFxSelection(3,value);		break;
	case buttonFunctShowNextStepFx4	:	changeStepFxSelection(4,value);		break;



	default: break;
	}

	refreshStepEditor = 1;
}


void cMtStepEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunctions[pot])
	{
		case potFunctNone			  : 	break;
		case potFunctChangeStepParam  :   changeActualStepParams(value);	break;
		case potFunctChangeStepFx1    :   changeActualStepFx1(value);		break;
		case potFunctChangeStepFx2    :   changeActualStepFx2(value);		break;
		case potFunctChangeStepFx3    :   changeActualStepFx3(value);		break;
		case potFunctChangeStepFx4    :   changeActualStepFx4(value);		break;

		default: break;
	}

	refreshStepEditor = 1;
}

void cMtStepEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{
	if(type == 1)
	{
		//if(y > 0)
			showStep(x,y);
	}

}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtStepEditor::processStepParameters()
{
	uint8_t track_length = sequencer.pattern->track[actualTrack].length;

	for(uint8_t i = 0; i < 5; i++)
	{
		if((i == 0 && actualStep < 2) || (i == 1 && actualStep < 1)
		|| (i == 3 && actualStep > track_length-3) || (i == 4 && actualStep > track_length-2))
		{
			trackTable.state[i] = 0;
		}
		else
		{
			trackTable.state[i] = 1;
			trackTable.active[0] = actualTrackTableSelection[0];

			trackTable.params[i].iVal1 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].note;
			trackTable.params[i].iVal2 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].instrument;
			trackTable.params[i].iVal3 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].length1/48;
			trackTable.params[i].iVal4 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].velocity;

			if(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[0].isOn == 0)
			{
				trackTable.fx1[i].mode = 0;
				getFxNameFromType(0, &fx1ActualNames[i][0]);
				trackTable.fx1[i].name = &fx1ActualNames[i][0];
			}
			else
			{
				trackTable.fx1[i].mode = getFxNameFromType(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[0].type, &fx1ActualNames[i][0]);
				trackTable.fx1[i].name = &fx1ActualNames[i][0];
				trackTable.active[1] = actualTrackTableSelection[1];

				switch(trackTable.fx1[i].mode)
				{


				default: break;
				}

				//trackTable.fx1[i].cVal1 =
				//trackTable.fx1[i].cVal2 =
				//trackTable.fx1[i].iVal1 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[0].val1_i8;
				//trackTable.fx1[i].iVal2 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[0].val2_i8;

			}
			//-------------------------------------------------------------------------------
			if(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[1].isOn == 0)
			{
				trackTable.fx2[i].mode = 0;
				getFxNameFromType(0, &fx2ActualNames[i][0]);
				trackTable.fx2[i].name = &fx2ActualNames[i][0];
			}
			else
			{
				trackTable.fx2[i].mode = getFxNameFromType(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[1].type, &fx2ActualNames[i][0]);
				trackTable.fx2[i].name = &fx2ActualNames[i][0];
				trackTable.active[2] = actualTrackTableSelection[2];
			}
			//-------------------------------------------------------------------------------
			if(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[2].isOn == 0)
			{
				trackTable.fx3[i].mode = 0;
				getFxNameFromType(0, &fx3ActualNames[i][0]);
				trackTable.fx3[i].name = &fx3ActualNames[i][0];
			}
			else
			{
				trackTable.fx3[i].mode = getFxNameFromType(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[2].type, &fx3ActualNames[i][0]);
				trackTable.fx3[i].name = &fx3ActualNames[i][0];
				trackTable.active[3] = actualTrackTableSelection[3];
			}
			//-------------------------------------------------------------------------------
			if(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[3].isOn == 0)
			{
				trackTable.fx4[i].mode = 0;
				getFxNameFromType(0, &fx4ActualNames[i][0]);
				trackTable.fx4[i].name = &fx4ActualNames[i][0];
			}
			else
			{
				trackTable.fx4[i].mode = getFxNameFromType(sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].fx[3].type, &fx4ActualNames[i][0]);
				trackTable.fx4[i].name = &fx4ActualNames[i][0];
				trackTable.active[4] = actualTrackTableSelection[4];
			}




		}
	}

}

uint8_t cMtStepEditor::getFxNameFromType(uint8_t fxType, char* ptrName)
{

	uint8_t i = 0;
	ptrName[0] = 0;
	while(stepEditStepFxNames[fxType][i] != 0 && i < 19)
	{
		ptrName[i] = stepEditStepFxNames[fxType][i];
		i++;
	}
	ptrName[i] = 0;


	return sequencer.get_fxValType(fxType);
}


void cMtStepEditor::processLabels()
{

	for(uint8_t i = 0; i < 5; i++)
	{
		switch(buttonFunction[i])
		{


		default: break;
		}
	}

}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################



//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// BUTTONS

void cMtStepEditor::setButtonLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	buttonFunctionLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		buttonFunctionLabels[function][i] = label[i];
		i++;
	}
	buttonFunctionLabels[function][i] = 0;

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtStepEditor::updateButtonsFunctions()
{
	setButtonFunction(0, buttonFunctNone);
	setButtonFunction(1, buttonFunctNone);
	setButtonFunction(2, buttonFunctNone);
	setButtonFunction(3, buttonFunctNone);
	setButtonFunction(4, buttonFunctNone);


	setButtonFunction(0, buttonFunctChangeStepParamsSel);
	setButtonFunction(1, buttonFunctShowNextStepFx1);
	setButtonFunction(2, buttonFunctShowNextStepFx2);
	setButtonFunction(3, buttonFunctShowNextStepFx3);
	setButtonFunction(4, buttonFunctShowNextStepFx4);








	buttonLabels[0] = (char *)&buttonFunctionLabels[buttonFunction[0]][0];
	buttonLabels[1] = (char *)&buttonFunctionLabels[buttonFunction[1]][0];
	buttonLabels[2] = (char *)&buttonFunctionLabels[buttonFunction[2]][0];
	buttonLabels[3] = (char *)&buttonFunctionLabels[buttonFunction[3]][0];
	buttonLabels[4] = (char *)&buttonFunctionLabels[buttonFunction[4]][0];

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtStepEditor::setButtonFunction(uint8_t number, uint8_t function)
{
	buttonFunction[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtStepEditor::setPotsLabel(uint8_t function, char* label)
{
	uint8_t i = 0;
	potFunctionLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		potFunctionLabels[function][i] = label[i];
		i++;
	}
	potFunctionLabels[function][i] = 0;

	mtDisplay.changePotsLabels(potLabels);
}



void cMtStepEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);




	setPotFunction(0, potFunctChangeStepParam);
	setPotFunction(1, potFunctChangeStepFx1);
	setPotFunction(2, potFunctChangeStepFx2);
	setPotFunction(3, potFunctChangeStepFx3);
	setPotFunction(4, potFunctChangeStepFx4);




	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];

	mtDisplay.changePotsLabels(potLabels);
}


void cMtStepEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunctions[number] = function;
	AnalogInputs.setPotResolution(number, potFuncRes[function]);
	AnalogInputs.setPotAcceleration(number, potFuncAcc[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// steps modification

void cMtStepEditor::changeStepParamsSelection(uint8_t value)
{
	if(value == 1)
	{
		actualTrackTableSelection[0]++;
		if(actualTrackTableSelection[0] >= stepParamsCount) actualTrackTableSelection[0] = 0;

		stepParametersChanged = 1;
		refreshStepEditor = 1;
	}
}

void cMtStepEditor::changeStepFxSelection(uint8_t fx, uint8_t value)
{
	if(value == 1)
	{
		uint8_t fx_type = sequencer.pattern->track[actualTrack].step[actualStep].fx[fx-1].type;

		if(fx_type > Sequencer::strFxConsts::FX_TYPE_NONE)
		{
			uint8_t fx_val_count = 1;
			if(sequencer.get_fxValType(fx_type) > Sequencer::strFxConsts::FX_VAL_TYPE_I16) fx_val_count = 2;

			actualTrackTableSelection[fx]++;
			if(actualTrackTableSelection[fx] > fx_val_count) actualTrackTableSelection[fx] = 0;

			stepParametersChanged = 1;
			refreshStepEditor = 1;
		}
	}
}


//-----------------------------------------------------------------------------------
void cMtStepEditor::changeActualStepParams(int16_t value)
{
	switch(actualTrackTableSelection[0])
	{
	case stepParamNote:
	{
		uint8_t step_note = sequencer.pattern->track[actualTrack].step[actualStep].note;

		if(step_note + value > Sequencer::MAX_NOTE_STEP)
			sequencer.seq[0].track[actualTrack].step[actualStep].note = Sequencer::MAX_NOTE_STEP;
		else if(step_note + value < Sequencer::MIN_NOTE_STEP)
			sequencer.seq[0].track[actualTrack].step[actualStep].note = Sequencer::MIN_NOTE_STEP;
		else
			sequencer.seq[0].track[actualTrack].step[actualStep].note += value;
		break;
	}
	case stepParamInstr:
	{
		uint8_t step_inst = sequencer.pattern->track[actualTrack].step[actualStep].instrument;

		if(step_inst + value >= mtProject.instruments_count)
			sequencer.seq[0].track[actualTrack].step[actualStep].instrument = mtProject.instruments_count-1;
		else if(step_inst + value < 0)
			sequencer.seq[0].track[actualTrack].step[actualStep].instrument = 0;
		else
			sequencer.seq[0].track[actualTrack].step[actualStep].instrument += value;
		break;
	}
	case stepParamLength:
	{
		uint8_t step_length = sequencer.pattern->track[actualTrack].step[actualStep].length1;

		if(step_length + value*48 > Sequencer::MAX_STEP_LENGTH*48)
			sequencer.seq[0].track[actualTrack].step[actualStep].length1 = Sequencer::MAX_STEP_LENGTH*48;
		else if(step_length + value*48 < 0)
			sequencer.seq[0].track[actualTrack].step[actualStep].length1 = 0;
		else
			sequencer.seq[0].track[actualTrack].step[actualStep].length1 += value*48;
		break;
	}
	case stepParamVolume:
	{
		uint8_t step_volume = sequencer.pattern->track[actualTrack].step[actualStep].velocity;

		if(step_volume + value > Sequencer::MAX_VELO_STEP)
			sequencer.seq[0].track[actualTrack].step[actualStep].velocity = Sequencer::MAX_VELO_STEP;
		else if(step_volume + value < Sequencer::MIN_VELO_STEP)
			sequencer.seq[0].track[actualTrack].step[actualStep].velocity = Sequencer::MIN_VELO_STEP;
		else
			sequencer.seq[0].track[actualTrack].step[actualStep].velocity += value;
		break;
	}


	default: break;
	}



	stepParametersChanged = 1;
	refreshStepEditor = 1;

}

void cMtStepEditor::changeActualStepFx1(int16_t value)
{

}

void cMtStepEditor::changeActualStepFx2(int16_t value)
{

}

void cMtStepEditor::changeActualStepFx3(int16_t value)
{

}

void cMtStepEditor::changeActualStepFx4(int16_t value)
{

}

//-----------------------------------------------------------------------------------

void cMtStepEditor::play(uint8_t value)
{
	if(value == 1)
	{

		instrumentPlayer[0].noteOn(0, 24, 100);
	}
	else if(value == 0)
	{
		instrumentPlayer[0].noteOff();
	}
}

void cMtStepEditor::stop(uint8_t value)
{

	instrumentPlayer[0].noteOff();


}
