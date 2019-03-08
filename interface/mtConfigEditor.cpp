
#include "mtDisplay.h"
#include "AnalogInputs.h"
#include "mtAudioEngine.h"
#include "mtConfig.h"
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

	saveConfig(CONFIG_EEPROM_ADDRESS, &mtConfig);

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
		switch(n)
		{
		case interfacePadPlay                 :    sequencer.play();    break;
		case interfacePadStop                 :    sequencer.stop();    break;
		case interfacePadProjectEditor        :
		case interfacePadSampleBank           :
		case interfacePadInstrumentEditor     :
		case interfacePadRecorder             :

			stop();
			eventFunct(mtConfigEditorEventPadPress, &n, 0, 0);

		break;

		case interfacePadConfig       :  switchParametersType(mtConfigEditorStartModeConfig);    break;
		case interfacePadSettings     :  switchParametersType(mtConfigEditorStartModeGlobals);   break;


		default: break;
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
		case potFunctMasterVolume		:	changeMasterVolume(value);		break;
		case potFunctReverbRoomSize		:	changeReverbRoomSize(value);	break;
		case potFunctReverbDamping		:	changeReverbDamping(value);		break;
		case potFunctReverbPanning		:	changeReverbPanning(value);		break;


		case potFunctCodecInput			:	changeInputSignal(value);		break;
		case potFunctCodecOutput		:	changeOutputSignal(value);		break;
		case potFunctCodecMicGain			:	changeMicGain(value);			break;
		case potFunctCodecLineInputLevel	:	changeLineInLevel(value);		break;
		case potFunctCodecLineOutputLevel	:	changeLineOutLevel(value);		break;


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
				values.value1[i] = mtConfig.audioCodecConfig.headphoneVolume*100;
				break;
			}
/*			case valueTempo:
			{
				values.type[i] = valuesTypes[valueTempo];
				values.value1[i] =  (editorInstrument->glide*100)/GLIDE_MAX;
				break;
			}
*/
			case valueReverbRoomSize:
			{
				values.type[i] = valuesTypes[valueReverbRoomSize];
				values.value1[i] = mtProject.values.reverbRoomSize;
				break;
			}
			case valueReverbDamping:
			{
				values.type[i] = valuesTypes[valueReverbDamping];
				values.value1[i] = mtProject.values.reverbDamping;
				break;
			}
			case valueReverbPanning:
			{
				values.type[i] = valuesTypes[valueReverbPanning];
				values.value1[i] = mtProject.values.reverbPanning;
				break;
			}

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
			case valueCodecMicGain:
			{
				values.type[i] = valuesTypes[valueCodecMicGain];
				values.value1[i] =  mtConfig.audioCodecConfig.inputGain;
				break;
			}
			case valueCodecLineInputLevel:
			{
				values.type[i] = valuesTypes[valueCodecLineInputLevel];
				values.value1[i] =  mtConfig.audioCodecConfig.lineInLeft;
				break;
			}
			case valueCodecLineOutputLevel:
			{
				values.type[i] = valuesTypes[valueCodecLineOutputLevel];
				values.value1[i] =  mtConfig.audioCodecConfig.lineOutLeft;
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

		setParameter(1, valueReverbRoomSize);
		setParameter(2, valueReverbDamping);
		setParameter(3, valueReverbPanning);



	}
	else if(parametersType == mtConfigEditorStartModeConfig)
	{
		setParameter(0, valueCodecInput);
		setParameter(1, valueCodecOutput);
		setParameter(2, valueCodecMicGain);
		setParameter(3, valueCodecLineInputLevel);
		setParameter(4, valueCodecLineOutputLevel);


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
		setPotFunction(1, potFunctReverbRoomSize);
		setPotFunction(2, potFunctReverbDamping);
		setPotFunction(3, potFunctReverbPanning);


	}
	else if(parametersType == mtConfigEditorStartModeConfig)
	{
		setPotFunction(0, potFunctCodecInput);
		setPotFunction(1, potFunctCodecOutput);
		setPotFunction(2, potFunctCodecMicGain);
		setPotFunction(3, potFunctCodecLineInputLevel);
		setPotFunction(4, potFunctCodecLineOutputLevel);


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


void cMtConfigEditor::changeMasterVolume(int16_t value)
{
	float fVal = value * 0.01;

	if(mtConfig.audioCodecConfig.headphoneVolume + fVal < MASTER_VOLUME_MIN) mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_MIN;
	else if(mtConfig.audioCodecConfig.headphoneVolume + fVal > MASTER_VOLUME_MAX) mtConfig.audioCodecConfig.headphoneVolume = MASTER_VOLUME_MAX;
	else mtConfig.audioCodecConfig.headphoneVolume += fVal;

	mtConfig.audioCodecConfig.changeFlag = 1;
	parametersChanged = 1;
}

void cMtConfigEditor::changeReverbPanning(int16_t value)
{
	if(mtProject.values.reverbPanning + value < REVERB_PANNING_MIN) mtProject.values.reverbPanning = REVERB_PANNING_MIN;
	else if(mtProject.values.reverbPanning + value > REVERB_PANNING_MAX) mtProject.values.reverbPanning = REVERB_PANNING_MAX;
	else mtProject.values.reverbPanning += value;

	parametersChanged = 1;

	engine.setReverbPanning(mtProject.values.reverbPanning);
}

void cMtConfigEditor::changeReverbRoomSize(int16_t value)
{
	if(mtProject.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.values.reverbRoomSize += value;

	parametersChanged = 1;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
}

void cMtConfigEditor::changeReverbDamping(int16_t value)
{
	if(mtProject.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.values.reverbDamping += value;

	parametersChanged = 1;

	engine.setReverbDamping(mtProject.values.reverbDamping);
}


void cMtConfigEditor::changeInputSignal(int16_t value)
{
	if(mtConfig.audioCodecConfig.inSelect + value >= inputSelectCount)
		mtConfig.audioCodecConfig.inSelect = inputSelectCount-1;
	else if(mtConfig.audioCodecConfig.inSelect + value < 0)
		mtConfig.audioCodecConfig.inSelect = 0;
	else mtConfig.audioCodecConfig.inSelect = mtConfig.audioCodecConfig.inSelect + value;

	mtConfig.audioCodecConfig.changeFlag = 1;

	parametersChanged = 1;
}

void cMtConfigEditor::changeOutputSignal(int16_t value)
{
	if(mtConfig.audioCodecConfig.outSelect + value >= outputSelectCount)
		mtConfig.audioCodecConfig.outSelect = outputSelectCount-1;
	else if(mtConfig.audioCodecConfig.outSelect + value < 0)
		mtConfig.audioCodecConfig.outSelect = 0;
	else mtConfig.audioCodecConfig.outSelect += value;

	mtConfig.audioCodecConfig.changeFlag = 1;

	parametersChanged = 1;
}

void cMtConfigEditor::changeMicGain(int16_t value)
{
	if(mtConfig.audioCodecConfig.inputGain + value > 63)
		mtConfig.audioCodecConfig.inputGain = 63;
	else if(mtConfig.audioCodecConfig.inputGain + value < 0)
		mtConfig.audioCodecConfig.inputGain = 0;
	else mtConfig.audioCodecConfig.inputGain += value;

	mtConfig.audioCodecConfig.changeFlag = 1;

	parametersChanged = 1;
}

void cMtConfigEditor::changeLineInLevel(int16_t value)
{
	if(mtConfig.audioCodecConfig.lineInLeft + value > 15)
		mtConfig.audioCodecConfig.lineInLeft = 15;
	else if(mtConfig.audioCodecConfig.lineInLeft + value < 0)
		mtConfig.audioCodecConfig.lineInLeft = 0;
	else mtConfig.audioCodecConfig.lineInLeft += value;

	mtConfig.audioCodecConfig.lineInRight = mtConfig.audioCodecConfig.lineInLeft;

	mtConfig.audioCodecConfig.changeFlag = 1;

	parametersChanged = 1;
}

void cMtConfigEditor::changeLineOutLevel(int16_t value)
{
	if(mtConfig.audioCodecConfig.lineOutLeft + value > 31)
		mtConfig.audioCodecConfig.lineOutLeft = 31;
	else if(mtConfig.audioCodecConfig.lineOutLeft + value < 13)
		mtConfig.audioCodecConfig.lineOutLeft = 13;
	else mtConfig.audioCodecConfig.lineOutLeft += value;

	mtConfig.audioCodecConfig.lineOutRight = mtConfig.audioCodecConfig.lineOutLeft;

	mtConfig.audioCodecConfig.changeFlag = 1;

	parametersChanged = 1;
}

