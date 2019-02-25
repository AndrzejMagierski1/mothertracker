
#include "mtDisplay.h"
#include "AnalogInputs.h"


#include "mtInterfaceDefs.h"

#include "mtConfigEditor.h"


cMtConfigEditor mtConfigEditor;


strMtConfig mtConfig;

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtConfigEditor::update()
{
	if(!refreshModule) return;
	refreshModule = 0;


	//-----------------------------------------------------
	if(moduleStart)
	{
		moduleStart = 0;
		labelsChanged = 2;
		parametersChanged = 2;

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
	if(parametersChanged)
	{
		if(parametersChanged == 2)
		{
			mtDisplay.setValue(1);

			updateParameters(); // zmiana parametrow
			labelsChanged = 1;
			refreshModule = 1;
		}

		parametersChanged = 0;
		processParameters();   // na zmiane wartosci

		mtDisplay.changeValues(&values);
	}
	//-----------------------------------------------------
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtConfigEditor::start(uint8_t mode)
{
	moduleStart = 1;
	refreshModule = 1;

	parametersType = mode;



}

void cMtConfigEditor::stop()
{
	mtDisplay.setValue(0);


}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
uint8_t cMtConfigEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
	if(type == 1)
	{
		if(n == interfacePadProjectEditor)
		{
			stop();
			eventFunct(mtConfigEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadInstrumentEditor)
		{
			stop();
			eventFunct(mtConfigEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadSampleBank)
		{
			stop();
			eventFunct(mtConfigEditorEventPadPress, &n, 0, 0);
		}
		else if(n == interfacePadPlay || n == interfacePadStop)
		{
			eventFunct(mtConfigEditorEventPadPress, &n, 0, 0);
		}
		//--------------------------------------------------------
		else if(n == interfacePadConfig)
		{
			switchParametersType(mtConfigEditorStartModeConfig);
		}
		else if(n == interfacePadSettings)
		{
			switchParametersType(mtConfigEditorStartModeGlobals);
		}


	}

	return 0;
}

void cMtConfigEditor::buttonChange(uint8_t button, uint8_t value)
{
	switch(buttonFunctions[button])
	{
	case buttonFunctNone				:		break;



	default: break;
	}

	refreshModule = 1;
}


void cMtConfigEditor::potChange(uint8_t pot, int16_t value)
{
	switch(potFunctions[pot])
	{
		case potFunctNone				:		break;



		default: break;
	}

	refreshModule = 1;
}

void cMtConfigEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{




}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtConfigEditor::processLabels()
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
// PARAMETERS
void cMtConfigEditor::processParameters()
{
	// odswieza wartosci tylko te ktore sa aktulanie wyswietlana (5 wartosci w valuesParameters[5])
	for(uint8_t i = 0; i < 5; i++)
	{
		switch(valuesParameters[i])
		{
			case valueNone:
			{
				values.type[i] = valuesTypes[valueNone];
				values.value1[i] = 0;
				break;
			}
			case valueMasterVolume:
			{
				values.type[i] = valuesTypes[valueMasterVolume];
				values.value1[i] = mtConfig.globals.masterVolume;
				break;
			}
/*			case valueTempo:
			{
				values.type[i] = valuesTypes[valueTempo];
				values.value1[i] =  (editorInstrument->glide*100)/GLIDE_MAX;
				break;
			}
*/


			//-------------------------------------------------------------
			case valueCodecInput:
			{
				values.type[i] = valuesTypes[valueCodecInput];
				values.value1[i] =  mtConfig.audioCodecConfig.inSelect;
				break;
			}
			case valueCodecOutput:
			{
				values.type[i] = valuesTypes[valueCodecOutput];
				values.value1[i] =  mtConfig.audioCodecConfig.outSelect;
				break;
			}





			//-------------------------------------------------------------
			default:
			{
				values.type[i] = valuesTypes[valueNone];
				values.value1[i] = 0;
				break;
			}
		}
	}
}

//=====================================================================
void cMtConfigEditor::updateParameters()
{
	setParameter(0, valueNone);
	setParameter(1, valueNone);
	setParameter(2, valueNone);
	setParameter(3, valueNone);
	setParameter(4, valueNone);


	if(parametersType == mtConfigEditorStartModeGlobals)
	{
		setParameter(0, valueMasterVolume);


	}
	else if(parametersType == mtConfigEditorStartModeConfig)
	{
		setParameter(0, valueCodecInput);
		setParameter(1, valueCodecOutput);

	}

}

//=====================================================================
void cMtConfigEditor::setParameter(uint8_t number, uint8_t param)
{
	valuesParameters[number] = param;
}


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// BUTTONS

void cMtConfigEditor::setButtonLabel(uint8_t function, char* label)
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


void cMtConfigEditor::updateButtonsFunctions()
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


void cMtConfigEditor::setButtonFunction(uint8_t number, uint8_t function)
{
	buttonFunctions[number] = function;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
// POTS

void cMtConfigEditor::setPotsLabel(uint8_t function, char* label)
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



void cMtConfigEditor::updatePotsFunctions()
{
	setPotFunction(0, potFunctNone);
	setPotFunction(1, potFunctNone);
	setPotFunction(2, potFunctNone);
	setPotFunction(3, potFunctNone);
	setPotFunction(4, potFunctNone);

//--------------------------------------------------------

	if(parametersType == mtConfigEditorStartModeGlobals)
	{
		setPotFunction(0, potFunctMasterVolume);


	}
	else if(parametersType == mtConfigEditorStartModeConfig)
	{
		setPotFunction(0, potFunctCodecInput);
		setPotFunction(1, potFunctCodecOutput);

	}

//--------------------------------------------------------

	potLabels[0] = (char *)&potFunctionLabels[potFunctions[0]][0];
	potLabels[1] = (char *)&potFunctionLabels[potFunctions[1]][0];
	potLabels[2] = (char *)&potFunctionLabels[potFunctions[2]][0];
	potLabels[3] = (char *)&potFunctionLabels[potFunctions[3]][0];
	potLabels[4] = (char *)&potFunctionLabels[potFunctions[4]][0];

	mtDisplay.changePotsLabels(potLabels);
}


void cMtConfigEditor::setPotFunction(uint8_t number, uint8_t function)
{
	potFunctions[number] = function;
	AnalogInputs.setPotResolution(number, potFuncRes[function]);
	AnalogInputs.setPotAcceleration(number, potFuncAcc[function]);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtConfigEditor::switchParametersType(uint8_t type)
{
	refreshModule = 1;
	labelsChanged = 1;
	parametersType = type;
	parametersChanged = 2;
}



