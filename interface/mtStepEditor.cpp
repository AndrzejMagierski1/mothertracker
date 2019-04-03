#include "mtStepEditor.h"

#include "mtDisplay.h"
#include "AnalogInputs.h"
#include "sdram.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtSequencer.h"
#include "mtLED.h"
#include "mtPadsBacklight.h"
#include "seqDisplay.h"
#include "mtInterfaceDefs.h"


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
	actualStep = seqDisplay.getStep(track, step);

	showActualParamOnPads();
	seqDisplay.clearAllBlink();
	seqDisplay.setBlink(track,step);
	seqDisplay.setMultiBlink(track, step);

	padsBacklight.setBackLayer(1,10,interfacePadUp);
	padsBacklight.setBackLayer(1,10,interfacePadLeft);
	padsBacklight.setBackLayer(1,10,interfacePadDown);
	padsBacklight.setBackLayer(1,10,interfacePadRight);



	for(uint8_t i = 0; i < 32; i++)
	{
		padsBacklight.setBackLayer(1, 8, 32+i);
	}

	last_selected_instrument = mtProject.values.lastUsedInstrument;

//	mtPrint("track:");
//	mtPrint(actualTrack);
//	mtPrint("  step:");
//	mtPrintln(actualstep);
}

void cMtStepEditor::stop()
{
	mtDisplay.setTrackTable(0);

	clearPads();
}


void cMtStepEditor::showActualParamOnPads()
{
	padsBacklight.setBackLayer( 0, 0, 24);
	padsBacklight.setBackLayer( 0, 0, 25);
	padsBacklight.setBackLayer( 0, 0, 26);
	padsBacklight.setBackLayer( 0, 0, 27);
	padsBacklight.setBackLayer( 0, 0, 28);

	padsBacklight.setBackLayer( 1, 15, 24+actualTrackTableSelection[0]);


}

void cMtStepEditor::clearPads()
{
/*
	padsBacklight.setBackLayer( 0, 0, 24);
	padsBacklight.setBackLayer( 0, 0, 25);
	padsBacklight.setBackLayer( 0, 0, 26);
	padsBacklight.setBackLayer( 0, 0, 27);
	padsBacklight.setBackLayer( 0, 0, 28);

	padsBacklight.setBackLayer( 0, 0, interfacePadUp);(interfacePadUp, 0, 0);
	padsBacklight.setBackLayer( 0, 0, 28);(interfacePadLeft, 0, 0);
	padsBacklight.setBackLayer( 0, 0, 28);(interfacePadDown, 0, 0);
	padsBacklight.setBackLayer( 0, 0, 28);(interfacePadRight, 0, 0);


	for(uint8_t i = 0; i < 32; i++)
	{
		leds.setLEDgrid(32+i, 0, 0);
	}

	*/
	padsBacklight.clearAllPads(0,1,0);

}


void cMtStepEditor::moveActualStep(uint8_t direction)
{

	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	uint8_t track_length = pattern->track[actualTrack].length;

	switch(direction)
	{
	case 0:
	{
		if(actualStep > 0)
		{
			if(seqDisplay.getBlink(actualTrack, actualStep) == 1) seqDisplay.decScroll();
			actualStep--;
			seqDisplay.clearAllBlink();
			seqDisplay.setBlink(actualTrack,seqDisplay.getBlink(actualTrack, actualStep));
		}
		break;
	}
	case 1:
	{
		if(actualTrack < 7)
		{
			actualTrack++;
			seqDisplay.clearAllBlink();
			seqDisplay.setBlink(actualTrack,seqDisplay.getBlink(actualTrack, actualStep));
		}
		break;
	}
	case 2:
	{
		if(actualStep < track_length-1)
		{
			if(seqDisplay.getBlink(actualTrack, actualStep) == 19)seqDisplay.incScroll() ;
			actualStep++;
			seqDisplay.clearAllBlink();
			seqDisplay.setBlink(actualTrack,seqDisplay.getBlink(actualTrack, actualStep));
		}
		break;
	}
	case 3:
	{
		if(actualTrack > 0)
		{
			actualTrack--;
			seqDisplay.clearAllBlink();
			seqDisplay.setBlink(actualTrack,seqDisplay.getBlink(actualTrack, actualStep));
		}
		break;
	}

	default: break;
	}

	stepParametersChanged = 1;
	refreshStepEditor = 1;
}



void cMtStepEditor::setActualEditedStepParam(uint8_t param)
{
	actualTrackTableSelection[0] = param;
	showActualParamOnPads();
	stepParametersChanged = 1;
	refreshStepEditor = 1;
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
		switch(n)
		{
		case interfacePadPlay                 :    sequencer.play();    break;
		case interfacePadStop                 :    sequencer.pause();    break;
		case interfacePadProjectEditor        :	   seqDisplay.incScroll(); break;
		case interfacePadSampleBank           :	   seqDisplay.decScroll();	break;
		case interfacePadInstrumentEditor     :
		case interfacePadConfig               :
		case interfacePadSettings             :
		case interfacePadRecorder             :

			stop();
			eventFunct(mtStepEditorEventPadPress, &n, 0, 0);

		break;

		case interfacePadNotes       :  setActualEditedStepParam(stepParamNote);   break;
		case interfacePadInstr       :  setActualEditedStepParam(stepParamInstr);   break;
		case interfacePadVelocity    :  setActualEditedStepParam(stepParamVolume);   break;
		case interfacePadFx          :  setActualEditedStepParam(stepParamFx);   break;
		case interfacePadFxParams    :  setActualEditedStepParam(stepParamFxParams);   break;
		case interfacePadUp          :  moveActualStep(0);   break;
		case interfacePadLeft        :  moveActualStep(3);   break;
		case interfacePadDown        :  moveActualStep(2);   break;
		case interfacePadRight       :  moveActualStep(1);   break;

		default: break;
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
	case buttonFunctChangeStepParamsSel	:	changeStepParamsSelection(value);	break;
	case buttonFunctShowNextStepFx1		:	changeStepFxSelection(1,value);		break;
	case buttonFunctShowNextStepFx2		:	changeStepFxSelection(2,value);		break;
	case buttonFunctShowNextStepFx3		:	changeStepFxSelection(3,value);		break;
	case buttonFunctShowNextStepFx4		:	changeStepFxSelection(4,value);		break;



	default: break;
	}

	refreshStepEditor = 1;
}


void cMtStepEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunctions[pot])
	{
		case potFunctNone			  	: 		break;
		case potFunctChangeStepNote  	:   	changeActualStepNote(value);		break;
		case potFunctChangeStepInstr    :   	changeActualStepInstrument(value);		break;
		case potFunctChangeStepVolume    :   	changeActualStepVolume(value);		break;
		case potFunctChangeStepFx    	:   	changeActualStepFx1(value);			break;
		case potFunctChangeStepFxParams    :   	changeActualStepFx1Params(value);		break;

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
	else
	{
		seqDisplay.clearLast();
	}

}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtStepEditor::processStepParameters()
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	uint8_t track_length = pattern->track[actualTrack].length;

	for(uint8_t i = 0; i < 5; i++)
	{
		if((i == 0 && actualStep < 2) || (i == 1 && actualStep < 1)
		|| (i == 3 && actualStep > track_length-3) || (i == 4 && actualStep > track_length-2))
		{
			trackTable.state[i] = 0; // linia poza sekwencja
		}
		else
		{
			trackTable.state[i] = 1; // linia w zakresie sekwencji

			if(pattern->track[actualTrack].step[(actualStep-2)+i].isOn) // czy parametry aktywne
			{
				trackTable.params[i].mode = 1; // parametry aktywne
				trackTable.active[0] = actualTrackTableSelection[0];
				trackTable.params[i].iVal1 = pattern->track[actualTrack].step[(actualStep-2)+i].note;
				trackTable.params[i].iVal2 = pattern->track[actualTrack].step[(actualStep-2)+i].instrument+1;
				trackTable.params[i].iVal3 = pattern->track[actualTrack].step[(actualStep-2)+i].length1/48;
				trackTable.params[i].iVal4 = pattern->track[actualTrack].step[(actualStep-2)+i].velocity;
			}
			else
			{
				trackTable.params[i].mode = 0; // parametry nie aktywne
				//trackTable.active[0] = 0;
			}

			if(pattern->track[actualTrack].step[(actualStep-2)+i].fx[0].isOn == 0)
			{
				trackTable.fx1[i].mode = 0;
				getFxNameFromType(0, &fx1ActualNames[i][0]);
				trackTable.fx1[i].name = &fx1ActualNames[i][0];
			}
			else
			{
				trackTable.fx1[i].mode = getFxNameFromType(pattern->track[actualTrack].step[(actualStep-2)+i].fx[0].type, &fx1ActualNames[i][0]);
				trackTable.fx1[i].name = &fx1ActualNames[i][0];
				trackTable.active[1] = actualTrackTableSelection[1];

				switch(trackTable.fx1[i].mode)
				{


				default: break;
				}

				//trackTable.fx1[i].cVal1 =
				//trackTable.fx1[i].cVal2 =
				//trackTable.fx1[i].iVal1 = sequencer.getPatternToUI()->track[actualTrack].step[(actualStep-2)+i].fx[0].val1_i8;
				//trackTable.fx1[i].iVal2 = sequencer.getPatternToUI()->track[actualTrack].step[(actualStep-2)+i].fx[0].val2_i8;

			}
			//-------------------------------------------------------------------------------
			if(pattern->track[actualTrack].step[(actualStep-2)+i].fx[1].isOn == 0)
			{
				trackTable.fx2[i].mode = 0;
				getFxNameFromType(0, &fx2ActualNames[i][0]);
				trackTable.fx2[i].name = &fx2ActualNames[i][0];
			}
			else
			{
				trackTable.fx2[i].mode = getFxNameFromType(pattern->track[actualTrack].step[(actualStep-2)+i].fx[1].type, &fx2ActualNames[i][0]);
				trackTable.fx2[i].name = &fx2ActualNames[i][0];
				trackTable.active[2] = actualTrackTableSelection[2];
			}
			//-------------------------------------------------------------------------------
			if(pattern->track[actualTrack].step[(actualStep-2)+i].fx[2].isOn == 0)
			{
				trackTable.fx3[i].mode = 0;
				getFxNameFromType(0, &fx3ActualNames[i][0]);
				trackTable.fx3[i].name = &fx3ActualNames[i][0];
			}
			else
			{
				trackTable.fx3[i].mode = getFxNameFromType(pattern->track[actualTrack].step[(actualStep-2)+i].fx[2].type, &fx3ActualNames[i][0]);
				trackTable.fx3[i].name = &fx3ActualNames[i][0];
				trackTable.active[3] = actualTrackTableSelection[3];
			}
			//-------------------------------------------------------------------------------
			if(pattern->track[actualTrack].step[(actualStep-2)+i].fx[3].isOn == 0)
			{
				trackTable.fx4[i].mode = 0;
				getFxNameFromType(0, &fx4ActualNames[i][0]);
				trackTable.fx4[i].name = &fx4ActualNames[i][0];
			}
			else
			{
				trackTable.fx4[i].mode = getFxNameFromType(pattern->track[actualTrack].step[(actualStep-2)+i].fx[3].type, &fx4ActualNames[i][0]);
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

/*
	setButtonFunction(0, buttonFunctChangeStepParamsSel);
	setButtonFunction(1, buttonFunctShowNextStepFx1);
	setButtonFunction(2, buttonFunctShowNextStepFx2);
	setButtonFunction(3, buttonFunctShowNextStepFx3);
	setButtonFunction(4, buttonFunctShowNextStepFx4);

*/






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




	setPotFunction(0, potFunctChangeStepNote);
	setPotFunction(1, potFunctChangeStepInstr);
	setPotFunction(2, potFunctChangeStepVolume);
	setPotFunction(3, potFunctChangeStepFx);
	setPotFunction(4, potFunctChangeStepFxParams);




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

		uint8_t fx_type = sequencer.getPatternToUI()->track[actualTrack].step[actualStep].fx[fx-1].type;

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

/*
void cMtStepEditor::changeActualStepParams(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[actualTrack].step[actualStep].isOn)
	{
		switch(actualTrackTableSelection[0])
		{
		case stepParamNote:
		{
			uint8_t step_note = pattern->track[actualTrack].step[actualStep].note;

			if(step_note + value > Sequencer::MAX_NOTE_STEP)
				pattern->track[actualTrack].step[actualStep].note = Sequencer::MAX_NOTE_STEP;
			else if(step_note + value < Sequencer::MIN_NOTE_STEP)
			{
				pattern->track[actualTrack].step[actualStep].note = Sequencer::MIN_NOTE_STEP;
				pattern->track[actualTrack].step[actualStep].isOn = 0;
			}
			else
				pattern->track[actualTrack].step[actualStep].note += value;


			break;
		}
		case stepParamInstr:
		{
			uint8_t step_inst = pattern->track[actualTrack].step[actualStep].instrument;

			if(step_inst + value >= mtProject.instruments_count)
				pattern->track[actualTrack].step[actualStep].instrument = mtProject.instruments_count-1;
			else if(step_inst + value < 0)
				pattern->track[actualTrack].step[actualStep].instrument = 0;
			else
				pattern->track[actualTrack].step[actualStep].instrument += value;

			last_selected_instrument = pattern->track[actualTrack].step[actualStep].instrument;

			break;
		}
		case stepParamLength:
		{
			uint8_t step_length = pattern->track[actualTrack].step[actualStep].length1;

			if(step_length + value*48 > Sequencer::MAX_STEP_LENGTH*48)
				pattern->track[actualTrack].step[actualStep].length1 = Sequencer::MAX_STEP_LENGTH*48;
			else if(step_length + value*48 < 0)
				pattern->track[actualTrack].step[actualStep].length1 = 0;
			else
				pattern->track[actualTrack].step[actualStep].length1 += value*48;

			last_selected_length = pattern->track[actualTrack].step[actualStep].length1;

			break;
		}
		case stepParamVolume:
		{
			int8_t step_volume = pattern->track[actualTrack].step[actualStep].velocity;

			if(step_volume + value > Sequencer::MAX_VELO_STEP)
				pattern->track[actualTrack].step[actualStep].velocity = Sequencer::MAX_VELO_STEP;
			else if(step_volume + value < Sequencer::MIN_VELO_STEP-1)
				pattern->track[actualTrack].step[actualStep].velocity = Sequencer::MIN_VELO_STEP-1;
			else
				pattern->track[actualTrack].step[actualStep].velocity += value;
			break;
		}


		default: break;
		}
	}
	else if(value > 0)
	{
		pattern->track[actualTrack].step[actualStep].isOn = 1;
		actualTrackTableSelection[0] = stepParamNote;

		pattern->track[actualTrack].step[actualStep].note = 24;
		pattern->track[actualTrack].step[actualStep].instrument = last_selected_instrument;
		pattern->track[actualTrack].step[actualStep].velocity = -1;
		pattern->track[actualTrack].step[actualStep].length1 = last_selected_length;
	}


	stepParametersChanged = 1;
	refreshStepEditor = 1;

}

*/



void cMtStepEditor::changeActualStepNote(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[actualTrack].step[actualStep].isOn)
	{
		uint8_t step_note = pattern->track[actualTrack].step[actualStep].note;

		if(step_note + value > Sequencer::MAX_NOTE_STEP)
			pattern->track[actualTrack].step[actualStep].note = Sequencer::MAX_NOTE_STEP;
		else if(step_note + value < Sequencer::MIN_NOTE_STEP)
		{
			pattern->track[actualTrack].step[actualStep].note = Sequencer::MIN_NOTE_STEP;
			pattern->track[actualTrack].step[actualStep].isOn = 0;
		}
		else
			pattern->track[actualTrack].step[actualStep].note += value;
	}
	else if(value > 0)
	{
		pattern->track[actualTrack].step[actualStep].isOn = 1;
		actualTrackTableSelection[0] = stepParamNote;

		pattern->track[actualTrack].step[actualStep].note = 24;
		pattern->track[actualTrack].step[actualStep].instrument = last_selected_instrument;
		pattern->track[actualTrack].step[actualStep].velocity = -1;
		pattern->track[actualTrack].step[actualStep].length1 = last_selected_length;
	}

	setActualEditedStepParam(stepParamNote);
	stepParametersChanged = 1;
	refreshStepEditor = 1;
}

void cMtStepEditor::changeActualStepInstrument(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[actualTrack].step[actualStep].isOn)
	{
		uint8_t step_inst = pattern->track[actualTrack].step[actualStep].instrument;

		if(step_inst + value >= mtProject.instruments_count)
			pattern->track[actualTrack].step[actualStep].instrument = mtProject.instruments_count-1;
		else if(step_inst + value < 0)
			pattern->track[actualTrack].step[actualStep].instrument = 0;
		else
			pattern->track[actualTrack].step[actualStep].instrument += value;

		last_selected_instrument = pattern->track[actualTrack].step[actualStep].instrument;

		mtProject.values.lastUsedInstrument = last_selected_instrument;
	}

	setActualEditedStepParam(stepParamInstr);
	stepParametersChanged = 1;
	refreshStepEditor = 1;
}

void cMtStepEditor::changeActualStepVolume(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[actualTrack].step[actualStep].isOn)
	{
		int8_t step_volume = pattern->track[actualTrack].step[actualStep].velocity;

		if(step_volume + value > Sequencer::MAX_VELO_STEP)
			pattern->track[actualTrack].step[actualStep].velocity = Sequencer::MAX_VELO_STEP;
		else if(step_volume + value < Sequencer::MIN_VELO_STEP-1)
			pattern->track[actualTrack].step[actualStep].velocity = Sequencer::MIN_VELO_STEP-1;
		else
			pattern->track[actualTrack].step[actualStep].velocity += value;
	}

	setActualEditedStepParam(stepParamVolume);
	stepParametersChanged = 1;
	refreshStepEditor = 1;
}

void cMtStepEditor::changeActualStepFx1(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[actualTrack].step[actualStep].fx[0].isOn)
	{

	}
	else if(value > 0)
	{

	}

	setActualEditedStepParam(stepParamFx);
	stepParametersChanged = 1;
	refreshStepEditor = 1;
}

void cMtStepEditor::changeActualStepFx1Params(int16_t value)
{
	Sequencer::strPattern * pattern = sequencer.getPatternToUI();

	if(pattern->track[actualTrack].step[actualStep].fx[0].isOn)
	{

	}



	setActualEditedStepParam(stepParamFxParams);

	stepParametersChanged = 1;
	refreshStepEditor = 1;
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
