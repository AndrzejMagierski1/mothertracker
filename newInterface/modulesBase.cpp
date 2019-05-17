
#include "modulesBase.h"



void cFunctionMachine::clearAll()
{
	for(uint8_t i = 0; i<potsCount; i++)
	{
		pots[i].isActive = 0;
	}
	for(uint8_t i = 0; i<buttonsCount; i++)
	{
		buttons[i].isActive = 0;
	}
	for(uint8_t i = 0; i<padsCount; i++)
	{
		pads[i].isActive = 0;
	}
}


template <typename T>
void changeParam(T properties)
{

}

//==================================================================================================================
void cFunctionMachine::setPotObj(uint8_t objectID, void(*funct)(int16_t), hControl control)
{
	buttons[objectID].funct1 = funct;
	buttons[objectID].control = control;

	buttons[objectID].isActive = 1;
}

//==================================================================================================================
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t state, void(*funct)(void), hControl control)
{
	buttons[objectID].funct1 = funct;
	buttons[objectID].state = state;
	buttons[objectID].control = control;

	buttons[objectID].isActive = 1;
}

void cFunctionMachine::setButtonObj(uint8_t objectID, void(*funct)(uint8_t), hControl control)
{
	buttons[objectID].funct2 = funct;
	buttons[objectID].state = 255;
	buttons[objectID].control = control;

	buttons[objectID].isActive = 1;
}

//==================================================================================================================
void cFunctionMachine::setPadObj(uint8_t objectID, uint8_t state, void(*funct)(), hControl control)
{
	buttons[objectID].funct1 = funct;
	buttons[objectID].state = state;
	buttons[objectID].control = control;

	buttons[objectID].isActive = 1;
}

void cFunctionMachine::setPadObj(uint8_t objectID, void(*funct)(uint8_t,int16_t), hControl control)
{
	buttons[objectID].funct2 = funct;
	buttons[objectID].state = 255;
	buttons[objectID].control = control;

	buttons[objectID].isActive = 1;
}


//==================================================================================================================
void cFunctionMachine::processButtonsInput(uint8_t button, uint8_t state)
{
	if(!buttons[button].isActive) return;

	if(buttons[button].funct1 != nullptr) buttons[button].funct1();
	else if(buttons[button].funct2 != nullptr) buttons[button].funct2(state);

}


void cFunctionMachine::processPotsInput(uint8_t pot, int16_t value)
{

}


void cFunctionMachine::processPadsInput(uint8_t pad, uint8_t state, int16_t velo)
{

}


void cFunctionMachine::processPadsInput(uint8_t pad, uint8_t state, int8_t x, int8_t y, int16_t velo)
{

}





