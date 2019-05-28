
#include "modulesBase.h"

#include <string.h>

void cFunctionMachine::clearAll()
{
	for(uint8_t i = 0; i<potsCount; i++)
	{
		pots[i].mode = 0;

	}
	for(uint8_t i = 0; i<buttonsCount; i++)
	{
		buttons[i].mode = 0;

	}
	for(uint8_t i = 0; i<padsCount; i++)
	{
		pads[i].mode = 0;

	}
}

void cFunctionMachine::clearAllButtons()
{
	memset(buttons,0,sizeof(strButtonObject)*buttonsCount);
}

void cFunctionMachine::clearButtonsRange(uint8_t from, uint8_t to)
{
	if(from >= buttonsCount || to >= buttonsCount) return;

	memset(buttons+from,0,sizeof(strButtonObject)*to);
}



void cFunctionMachine::clearAllPots()
{
	delete  (paramChange<uint8_t>*)pots[0].paramStruct;

	memset(pots,0,sizeof(strPotObject)*potsCount);


}

void cFunctionMachine::clearAllPads()
{
	memset(pads,0,sizeof(strPadObject)*padsCount);
}

//==================================================================================================================
void cFunctionMachine::setPotObj(uint8_t objectID, uint8_t(*funct)(int16_t), hControl control)
{
	pots[objectID].funct1 = funct;
	pots[objectID].control = control;

	pots[objectID].mode = 1;
}

//==================================================================================================================
void cFunctionMachine::setPotObj(int8_t objectID, uint8_t* param, uint8_t min, uint8_t max, uint8_t step, hControl control)
{
	delete  (paramChange<uint8_t>*)pots[objectID].paramStruct;

	paramChange<uint8_t>* strparam = new paramChange<uint8_t>;
	strparam->ptr = param;
	strparam->min = min;
	strparam->max = max;
	strparam->step = step;

	pots[objectID].paramStruct = strparam;

	pots[objectID].control = control;

	pots[objectID].mode = 2;
}

void cFunctionMachine::setPotObj(int8_t objectID, uint16_t* param, uint16_t min, uint16_t max, uint16_t step, hControl control)
{
	delete  (paramChange<uint16_t>*)pots[objectID].paramStruct;

	paramChange<uint16_t>* strparam = new paramChange<uint16_t>;
	strparam->ptr = param;
	strparam->min = min;
	strparam->max = max;
	strparam->step = step;

	pots[objectID].paramStruct = strparam;

	pots[objectID].control = control;

	pots[objectID].mode = 3;
}
//==================================================================================================================

// przypisuje funkcje na okreslony stan przycisku bez argumentow
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(void))
{
	buttons[objectID].funct1 = funct;
	buttons[objectID].state = state;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 1;
}

// przypisuje funkcje na okreslony stan przycisku z argumentem w postaci id przycisku
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(uint8_t))
{
	buttons[objectID].funct2 = funct;
	buttons[objectID].state = state;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 2;
}

// przypisuje funkcje z argumentem w postaci stanu przycisku
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t(*funct)(uint8_t))
{
	buttons[objectID].funct2 = funct;
	buttons[objectID].state = 255;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 3;
}

//==================================================================================================================
void cFunctionMachine::setPadObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(), hControl control)
{
	pads[objectID].funct1 = funct;
	pads[objectID].state = state;
	pads[objectID].control = control;

	pads[objectID].mode = 1;
}

void cFunctionMachine::setPadObj(uint8_t objectID, uint8_t(*funct)(uint8_t,int16_t), hControl control)
{
	pads[objectID].funct2 = funct;
	pads[objectID].state = 255;
	pads[objectID].control = control;

	pads[objectID].mode = 1;
}


//==================================================================================================================
void cFunctionMachine::processButtonsInput(uint8_t button, uint8_t state)
{
	uint8_t result = 1;

	switch(buttons[button].mode)
	{
	case 1:
		if(buttons[button].funct1 != nullptr && state == buttons[button].state) result = buttons[button].funct1();
		break;
	case 2:
		if(buttons[button].funct2 != nullptr && state == buttons[button].state) result = buttons[button].funct2(button);
		break;
	case 3:
		if(buttons[button].funct2 != nullptr) result = buttons[button].funct2(state);
		break;
	default: return;
	}

	//if(buttons[button].control != nullptr) display.refreshControl(buttons[button].control);

	if(result == 0)
	{
		buttons[button].mode = 0;
		buttons[button].funct1 = nullptr;
		buttons[button].funct2 = nullptr;
	}

}


void cFunctionMachine::processPotsInput(uint8_t pot, int16_t value)
{
	if(!pots[pot].mode) return;

	if(pots[pot].mode > 1)
	{
		switch(pots[pot].mode)
		{
		case 2:
			changeParam<uint8_t>((paramChange<uint8_t>*) pots[pot].paramStruct, value );

		break;

		case 3:
			changeParam<uint16_t>((paramChange<uint16_t>*) pots[pot].paramStruct, value );

		break;
		}




		display.setControlValue(pots[pot].control, *((paramChange<uint8_t>*) pots[pot].paramStruct)->ptr);
		display.refreshControl(pots[pot].control);



		return;
	}



	uint8_t result = 0;

	if(pots[pot].funct1 != nullptr) result = pots[pot].funct1(value);

	if(pots[pot].control != nullptr) display.refreshControl(pots[pot].control);

	if(result == 0)
	{
		pots[pot].mode = 0;
		pots[pot].funct1 = nullptr;
	}
}


void cFunctionMachine::processPadsInput(uint8_t pad, uint8_t state, int16_t velo)
{
	if(!pads[pad].mode) return;

	uint8_t result = 0;

	if(pads[pad].funct1 != nullptr && state == pads[pad].state) result = pads[pad].funct1();
	else if(pads[pad].funct2 != nullptr) result = pads[pad].funct2(state,velo);

	if(pads[pad].control != nullptr) display.refreshControl(pads[pad].control);

	if(result == 0)
	{
		pads[pad].mode = 0;
		pads[pad].funct1 = nullptr;
		pads[pad].funct2 = nullptr;
	}
}


void cFunctionMachine::processPadsInput(uint8_t pad, uint8_t state, int8_t x, int8_t y, int16_t velo)
{
	if(!pads[pad].mode) return;



}





