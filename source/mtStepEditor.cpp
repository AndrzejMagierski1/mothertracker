
#include "mtDisplay.h"
#include "AnalogInputs.h"
#include "sdram.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"

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

		//mtDisplay.setInstrumentEditorPotsLabels();
		//mtDisplay.setInstrumentEditorButtonsLabels();
	}
	//-----------------------------------------------------


}


void cMtStepEditor::showStep(uint8_t track, uint8_t step)
{
	stepEditorModeStart = 1;
	refreshStepEditor = 1;
	actualTrack = track;
	actualStep = step;
}

void cMtStepEditor::stop()
{

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
	}

	return 0;
}

void cMtStepEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunction[button])
	{
	case mtStepEditorButtonFunctionNone:		break;



	default: break;
	}

	refreshStepEditor = 1;
}


void cMtStepEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunction[pot])
	{
		case mtStepEditorPotFunctionNone				: 	break;


		default: break;
	}

	refreshStepEditor = 1;
}

void cMtStepEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{



}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// spectrum processing


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
	setButtonFunction(0, mtStepEditorButtonFunctionNone);
	setButtonFunction(1, mtStepEditorButtonFunctionNone);
	setButtonFunction(2, mtStepEditorButtonFunctionNone);
	setButtonFunction(3, mtStepEditorButtonFunctionNone);
	setButtonFunction(4, mtStepEditorButtonFunctionNone);

/*
	if()
	{
		setButtonFunction(0, mtStepEditorButtonFunctionPlay);
	}
	else
	{

	}
*/


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
	setPotFunction(0, mtStepEditorPotFunctionNone);
	setPotFunction(1, mtStepEditorPotFunctionNone);
	setPotFunction(2, mtStepEditorPotFunctionNone);
	setPotFunction(3, mtStepEditorPotFunctionNone);
	setPotFunction(4, mtStepEditorPotFunctionNone);




	setPotFunction(0, mtStepEditorPotFunctionStepParams);
	setPotFunction(1, mtStepEditorPotFunctionStepFx1);
	setPotFunction(2, mtStepEditorPotFunctionStepFx2);
	setPotFunction(3, mtStepEditorPotFunctionStepFx3);
	setPotFunction(4, mtStepEditorPotFunctionStepFx4);




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
