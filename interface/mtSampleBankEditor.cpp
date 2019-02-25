
#include "mtDisplay.h"
#include "AnalogInputs.h"


#include "mtInterfaceDefs.h"

#include "mtSampleBankEditor.h"


cMtSampleBankEditor mtSampleBankEditor;


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::update()
{
	if(!refreshModule) return;
	refreshModule = 0;


	//-----------------------------------------------------
	if(moduleStart)
	{
		moduleStart = 0;
		labelsChanged = 2;


	}
	//-----------------------------------------------------
	if(labelsChanged)
	{
		if(labelsChanged == 2)
		{
			mtDisplay.setPotsLabels(1);
			mtDisplay.setButtonsLabels(1);
		}

		labelsChanged = 0;

		updateButtonsFunctions();
		updatePotsFunctions();

		processLabels();
	}
	//-----------------------------------------------------


}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::start()
{
	moduleStart = 1;
	refreshModule = 1;



}

void cMtSampleBankEditor::stop()
{



}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtSampleBankEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
	if(type == 1)
	{
		if(n == interfacePadInstrumentEditor)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadProjectEditor)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadConfig)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSettings)
		{
			stop();
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadPlay || n == interfacePadStop)
		{
			eventFunct(mtSampleBankEditorEventPadPress, &n, 0, 0);
		}
	}

	return 0;
}

void cMtSampleBankEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunctions[button])
	{
	case buttonFunctNone				:		break;



	default: break;
	}

	refreshModule = 1;
}


void cMtSampleBankEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunctions[pot])
	{
		case potFunctNone				:		break;



		default: break;
	}

	refreshModule = 1;
}

void cMtSampleBankEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{
/*

	if(type == 1)
	{
		//if(y > 0)
			showStep(x,y);
	}
*/
}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtSampleBankEditor::processLabels()
{

	for(uint8_t i = 0; i < 5; i++)
	{
		switch(buttonFunctions[i])
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

void cMtSampleBankEditor::setButtonLabel(uint8_t function, char* label)
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


void cMtSampleBankEditor::updateButtonsFunctions()
{
	setButtonFunction(0, buttonFunctNone);
	setButtonFunction(1, buttonFunctNone);
	setButtonFunction(2, buttonFunctNone);
	setButtonFunction(3, buttonFunctNone);
	setButtonFunction(4, buttonFunctNone);

//--------------------------------------------------------




//--------------------------------------------------------

	buttonLabels[0] = (char *)&buttonFunctionLabels[buttonFunctions[0]][0];
	buttonLabels[1] = (char *)&buttonFunctionLabels[buttonFunctions[1]][0];
	buttonLabels[2] = (char *)&buttonFunctionLabels[buttonFunctions[2]][0];
	buttonLabels[3] = (char *)&buttonFunctionLabels[buttonFunctions[3]][0];
	buttonLabels[4] = (char *)&buttonFunctionLabels[buttonFunctions[4]][0];

	mtDisplay.changeButtonsLabels(buttonLabels);
}


void cMtSampleBankEditor::setButtonFunction(uint8_t number, uint8_t function)
{
	buttonFunctions[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtSampleBankEditor::setPotsLabel(uint8_t function, char* label)
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



void cMtSampleBankEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);

//--------------------------------------------------------




//--------------------------------------------------------

	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];

	mtDisplay.changePotsLabels(potLabels);
}


void cMtSampleBankEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunctions[number] = function;
	AnalogInputs.setPotResolution(number, potFuncRes[function]);
	AnalogInputs.setPotAcceleration(number, potFuncAcc[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################





