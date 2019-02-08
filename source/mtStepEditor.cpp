
#include "mtDisplay.h"
#include "AnalogInputs.h"
#include "sdram.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"
#include "mtSequencer.h"

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
		if(n == 3)
		{
			stop();
			eventFunct(mtStepEditorEventPadPress, &n, 0, 0);
		}
		if(n == 0 || n == 2)
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
	case mtStepEditButtonFunctNone				:		break;
	case mtStepEditButtonFunctPlay            	:		break;
	case mtStepEditButtonFunctStop            	:		break;
	case mtStepEditButtonFunctChangeStepParamsSel:	changeStepParamsSelection(value);	break;
	case mtStepEditButtonFunctShowNextStepParams:	showNextStepParams(value);	break;



	default: break;
	}

	refreshStepEditor = 1;
}


void cMtStepEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunction[pot])
	{
		case mtStepEditPotFunctNone				: 	break;


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
			trackTable.params[i].iVal3 = sequencer.pattern->track[actualTrack].step[(actualStep-2)+i].length1;
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
	mtStepEditorButtonsLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		mtStepEditorButtonsLabels[function][i] = label[i];
		i++;
	}
	mtStepEditorButtonsLabels[function][i] = 0;

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtStepEditor::updateButtonsFunctions()
{
	setButtonFunction(0, mtStepEditButtonFunctNone);
	setButtonFunction(1, mtStepEditButtonFunctNone);
	setButtonFunction(2, mtStepEditButtonFunctNone);
	setButtonFunction(3, mtStepEditButtonFunctNone);
	setButtonFunction(4, mtStepEditButtonFunctNone);


	setButtonFunction(0, mtStepEditButtonFunctChangeStepParamsSel);
/*	setButtonFunction(1, mtStepEditButtonFunctChangeStepFx1);
	setButtonFunction(2, mtStepEditButtonFunctChangeStepFx2);
	setButtonFunction(3, mtStepEditButtonFunctChangeStepFx3);
*/
	setButtonFunction(4, mtStepEditButtonFunctShowNextStepParams);







	buttonLabels[0] = (char *)&mtStepEditorButtonsLabels[buttonFunction[0]][0];
	buttonLabels[1] = (char *)&mtStepEditorButtonsLabels[buttonFunction[1]][0];
	buttonLabels[2] = (char *)&mtStepEditorButtonsLabels[buttonFunction[2]][0];
	buttonLabels[3] = (char *)&mtStepEditorButtonsLabels[buttonFunction[3]][0];
	buttonLabels[4] = (char *)&mtStepEditorButtonsLabels[buttonFunction[4]][0];

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
	mtStepEditorPotsLabels[function][i] = 0;
	while(label[i] != 0 && i < 19)
	{
		mtStepEditorPotsLabels[function][i] = label[i];
		i++;
	}
	mtStepEditorPotsLabels[function][i] = 0;

	mtDisplay.changePotsLabels(potsLabels);
}



void cMtStepEditor::updatePotsFunctions()
{
	setPotFunction(0, mtStepEditPotFunctNone);
	setPotFunction(1, mtStepEditPotFunctNone);
	setPotFunction(2, mtStepEditPotFunctNone);
	setPotFunction(3, mtStepEditPotFunctNone);
	setPotFunction(4, mtStepEditPotFunctNone);




	setPotFunction(0, mtStepEditPotFunctChangeStepInstrument);
	setPotFunction(1, mtStepEditPotFunctChangeStepNote);
	setPotFunction(2, mtStepEditPotFunctChangeStepLength);
	setPotFunction(3, mtStepEditPotFunctChangeStepVolume);
	setPotFunction(4, mtStepEditPotFunctChangeSeqPosition);




	potsLabels[0] = (char *)&mtStepEditorPotsLabels[potFunction[0]][0];
	potsLabels[1] = (char *)&mtStepEditorPotsLabels[potFunction[1]][0];
	potsLabels[2] = (char *)&mtStepEditorPotsLabels[potFunction[2]][0];
	potsLabels[3] = (char *)&mtStepEditorPotsLabels[potFunction[3]][0];
	potsLabels[4] = (char *)&mtStepEditorPotsLabels[potFunction[4]][0];

	mtDisplay.changePotsLabels(potsLabels);
}


void cMtStepEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunction[number] = function;
	AnalogInputs.setPotResolution(number, stepEditorPotsFuncRes[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// instrument modification

void cMtStepEditor::changeStepParamsSelection(uint8_t value)
{
	if(value == 1)
	{
		actualTrackTableSelection[0]++;
		if(actualTrackTableSelection[0] > 3) actualTrackTableSelection[0] = 0;

		stepParametersChanged = 1;
		refreshStepEditor = 1;
	}
}

void cMtStepEditor::showNextStepParams(uint8_t value)
{

}


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
