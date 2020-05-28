

#include "core/modulesBase.h"
#include <string.h>

void cFunctionMachine::clearAll()  // stara wersja
{
	if(potsCleared && buttonsCleared && padsCleared) return;

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

	potsCleared = 1;
	buttonsCleared = 1;
	padsCleared = 1;

}

void cFunctionMachine::clearAllButtons()
{
	if(buttonsCleared) return;

	memset((uint8_t *)buttons,0,sizeof(strButtonObject)*buttonsCount);

	buttonsCleared = 1;
}

void cFunctionMachine::clearButtonsRange(uint8_t from, uint8_t to)
{
	if(buttonsCleared) return;

	if(from >= buttonsCount || to >= buttonsCount || from > to) return;

	memset((uint8_t *)(buttons+from),0,(sizeof(strButtonObject)*((to+1)-from)));
}

void cFunctionMachine::clearButton(uint8_t button)
{
	if(buttonsCleared) return;

	if(button >= buttonsCount) return;

	memset( (uint8_t*)(buttons+button),0,sizeof(strButtonObject));
}


void cFunctionMachine::clearAllPots()
{
	if(potsCleared) return;

	memset((uint8_t *)pots,0,sizeof(strPotObject)*potsCount);

	potsCleared = 1;
}

void cFunctionMachine::clearAllPads()
{
	if(padsCleared) return;

	memset((uint8_t *)pads,0,sizeof(strPadObject)*padsCount);

	padsGlobalMode = 0;

	padsCleared = 1;
}

//==================================================================================================================
void cFunctionMachine::blockAllInputs()
{
	memset((uint8_t *)blockButtons,	1,	buttonsCount);
	memset((uint8_t *)blockPots,	1,	potsCount);
	memset((uint8_t *)blockPads,	1,	padsCount);
	blockPowerButton = 1;
}

void cFunctionMachine::blockAllInputsExcept(uint8_t n)
{
	blockAllInputs();
	if(n < buttonsCount) blockButtons[n] = 0;
}

void cFunctionMachine::blockAllInputsExcept(uint8_t n, uint8_t o)
{
	blockAllInputs();
	if(n < buttonsCount) blockButtons[n] = 0;
	if(o < buttonsCount) blockButtons[o] = 0;
}

void cFunctionMachine::unblockAllInputs()
{
	memset((uint8_t *)blockButtons,	0,	buttonsCount);
	memset((uint8_t *)blockPots,	0,	potsCount);
	memset((uint8_t *)blockPads,	0,	padsCount);
	blockPowerButton = 0;
}

void cFunctionMachine::unblockPads()
{
	memset((uint8_t *)blockPads,	0,	padsCount);
}


//==================================================================================================================
void cFunctionMachine::setPotObj(uint8_t objectID, uint8_t(*funct)(int16_t), hControl control)
{
	pots[objectID].funct1 = funct;
	pots[objectID].control = control;

	pots[objectID].mode = 1;

	potsCleared = 0;
}

//==================================================================================================================

// przypisuje funkcje na okreslony stan przycisku bez argumentow
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(void))
{
	buttons[objectID].funct1 = funct;
	buttons[objectID].state = state;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 1;

	buttonsCleared = 0;
}

// przypisuje funkcje na okreslony stan przycisku z argumentem w postaci id przycisku
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(uint8_t))
{
	buttons[objectID].funct2 = funct;
	buttons[objectID].state = state;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 2;

	buttonsCleared = 0;
}

// przypisuje funkcje z argumentem w postaci stanu przycisku
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t(*funct)(uint8_t))
{
	buttons[objectID].funct2 = funct;
	buttons[objectID].state = 255;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 3;

	buttonsCleared = 0;
}

// przypisuje funkcje z argumentami w postaci id i stanu przycisku
void cFunctionMachine::setButtonObj(uint8_t objectID, uint8_t(*funct)(uint8_t,uint8_t))
{
	buttons[objectID].funct3 = funct;
	buttons[objectID].state = 255;
	//buttons[objectID].control = control;

	buttons[objectID].mode = 4;

	buttonsCleared = 0;
}


void cFunctionMachine::setPowerButtonObj(uint8_t(*funct)(uint8_t))
{
	powrButtonFunct = funct;
}

//==================================================================================================================
void cFunctionMachine::setPadObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(), hControl control)
{
	pads[objectID].funct1 = funct;
	pads[objectID].state = state;
	pads[objectID].control = control;

	pads[objectID].mode = 1;

	padsCleared = 0;
}

void cFunctionMachine::setPadObj(uint8_t objectID, uint8_t(*funct)(uint8_t,int16_t), hControl control)
{
	pads[objectID].funct2 = funct;
	pads[objectID].state = 255;
	pads[objectID].control = control;

	pads[objectID].mode = 1;

	padsCleared = 0;
}

void cFunctionMachine::setPadsGlobal(uint8_t(*funct)(uint8_t,uint8_t,int16_t))
{
	padsGlobalMode = 1;
	//padsGlobalState = state;
	padsGlobalFunct = funct;

	padsCleared = 0;
}

//==================================================================================================================


//==================================================================================================================
void cFunctionMachine::processButtonsInput(uint8_t button, uint8_t state)
{
	if(blockButtons[button]) return;

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
	case 4:
		if(buttons[button].funct3 != nullptr) result = buttons[button].funct3(button,state);
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
	if(blockPots[pot]) return;
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



	uint8_t result = 1;

	if(pots[pot].mode == 1 && pots[pot].funct1 != nullptr) result = pots[pot].funct1(value);
	else return;

	display.refreshControl(pots[pot].control);

	if(result == 0)
	{
		pots[pot].mode = 0;
		pots[pot].funct1 = nullptr;
	}
}


void cFunctionMachine::processPadsInput(uint8_t pad, uint8_t state, int16_t velo)
{
	if(blockPads[pad]) return;

	uint8_t result = 0;

	if(padsGlobalMode == 1)
	{
		if(padsGlobalFunct != nullptr) result = padsGlobalFunct(pad,state,velo);
	}

/*
	if(!pads[pad].mode) return;

	uint8_t result = 0;

	if(pads[pad].funct1 != nullptr && state == pads[pad].state) result = pads[pad].funct1();
	else if(pads[pad].funct2 != nullptr) result = pads[pad].funct2(state,velo);

	if(pads[pad].control != nullptr) display.refreshControl(pads[pad].control);
*/
	if(result == 0)
	{
		padsGlobalFunct = nullptr;

		pads[pad].mode = 0;
		pads[pad].funct1 = nullptr;
		pads[pad].funct2 = nullptr;

	}


}


void cFunctionMachine::processPadsInput(uint8_t pad, uint8_t state, int8_t x, int8_t y, int16_t velo)
{
	if(!pads[pad].mode) return;

}



void cFunctionMachine::processPowerButton(uint8_t state)
{
	if(powrButtonFunct != nullptr)
	{
		if(powrButtonFunct(state) == 0) powrButtonFunct = nullptr;
	}
}

