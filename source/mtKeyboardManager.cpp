#include "mtKeyboardManager.h"
#include "mtLED.h"
#include "display.h"

void mtKeyboardManager::init(hControl keyboardC, hControl nameC)
{
	keyboardControl = keyboardC;
	editName = nameC;
}

void mtKeyboardManager::deinit()
{
	keyboardControl = nullptr;
	editName = nullptr;
}

void mtKeyboardManager::activateKeyboard()
{
	editPosition = strlen(name);
	keyboardPosition = BACKSPACE_PAD_1;
	lastPressedPad = BACKSPACE_PAD_1;
	leds.setLED(BACKSPACE_PAD_1, 1, mtConfig.values.padsLightFront);
	leds.setLED(BACKSPACE_PAD_2, 1, mtConfig.values.padsLightFront);

	keyboardActiveFlag = 1;

	showKeyboard();
	showKeyboardEditName();
}

void mtKeyboardManager::deactivateKeyboard()
{
	keyboardActiveFlag = 0;
	hideKeyboard();
	hideEditName();
}

void mtKeyboardManager::setRandomName()
{
	strcpy(name,randomNameGenerator.getRandomName());
	editPosition = strlen(name);
	showKeyboardEditName();
}
char * mtKeyboardManager::getName()
{
	return name;
}

void mtKeyboardManager::fillName(char * txt)
{
	strncpy(name,txt,MAX_NAME_LENGTH - 1);
}

void mtKeyboardManager::onPadChange(uint8_t pad, uint8_t state)
{
	if((state == 1) || (state == 2))
	{
		if(keyboardActiveFlag)
		{
			if( (lastPressedPad == BACKSPACE_PAD_1) || (lastPressedPad == BACKSPACE_PAD_2) ) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 0, 0);
				leds.setLED(BACKSPACE_PAD_2, 0, 0);
			}
			else if( (lastPressedPad == CAPS_LOCK_PAD_1) || (lastPressedPad == CAPS_LOCK_PAD_2) ) //capslock
			{
				if(keyboardShiftFlag)
				{
					leds.setLED(CAPS_LOCK_PAD_1, 1, mtConfig.values.padsLightBack);
					leds.setLED(CAPS_LOCK_PAD_2, 1, mtConfig.values.padsLightBack);
				}
				else
				{
					leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
					leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
				}

			}
			else if( (lastPressedPad >= SPACE_PAD_1) && (lastPressedPad <= SPACE_PAD_5) ) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 0, 0);
				}
			}
			else
			{
				if( (lastPressedPad != F_PAD) && (lastPressedPad != J_PAD) ) leds.setLED(lastPressedPad,0,0);
				else leds.setLED(lastPressedPad,1,mtConfig.values.padsLightBack);
			}


			lastPressedPad = pad;

			if(pad == BACKSPACE_PAD_1 || pad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 1, mtConfig.values.padsLightFront);
				leds.setLED(BACKSPACE_PAD_2, 1, mtConfig.values.padsLightFront);
			}
			else if(pad == CAPS_LOCK_PAD_1 || pad == CAPS_LOCK_PAD_2) //capslock
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, mtConfig.values.padsLightFront);
				leds.setLED(CAPS_LOCK_PAD_2, 1, mtConfig.values.padsLightFront);
			}
			else if(pad >= SPACE_PAD_1 && pad <=SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 1, mtConfig.values.padsLightFront);
				}
			}
			else
			{
				leds.setLED(pad,1,mtConfig.values.padsLightFront);
			}

			keyboardPosition = valueMapPads[pad];


			if(editPosition > 31) return;
			if(smallKeyboard[keyboardPosition] > 1)
			{
				if(editPosition == 31) return;
				uint8_t localNameLen = strlen(name);
				if(editPosition < localNameLen)
				{
					for(uint8_t i = localNameLen; i >= editPosition ; i-- )
					{
						name[i+1] = name[i];
					}
				}

				name[editPosition] = keyboardShiftFlag ? bigKeyboard[keyboardPosition] : smallKeyboard[keyboardPosition];
				name[editPosition + 1] = 0;
				editPosition++;
			}
			else if(smallKeyboard[keyboardPosition] == 0)
			{
				if(editPosition == 0 ) return;

				name[editPosition-1] = 0;
				editPosition--;


			}
			else if(smallKeyboard[keyboardPosition] == 1)
			{
				keyboardShiftFlag = ! keyboardShiftFlag;
			}
			showKeyboard();
			showKeyboardEditName();
			return;
		}

		return;
	}
}

void mtKeyboardManager::confirmKey()
{
	if(keyboardActiveFlag)
	{
		if(editPosition > 31) return;

		//****************************************************ledy
		if(lastPressedPad == BACKSPACE_PAD_1 || lastPressedPad == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 0, 0);
			leds.setLED(BACKSPACE_PAD_2, 0, 0);
		}
		else if(lastPressedPad == CAPS_LOCK_PAD_1 || lastPressedPad == CAPS_LOCK_PAD_2) //capslock
		{
			if(keyboardShiftFlag)
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, mtConfig.values.padsLightBack);
				leds.setLED(CAPS_LOCK_PAD_2, 1, mtConfig.values.padsLightBack);
			}
			else
			{
				leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
				leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
			}

		}
		else if(lastPressedPad >= SPACE_PAD_1 && lastPressedPad <= SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 0, 0);
			}
		}
		else
		{
			if(lastPressedPad != F_PAD && lastPressedPad != J_PAD) leds.setLED(lastPressedPad,0,0);
			else leds.setLED(lastPressedPad,1,mtConfig.values.padsLightBack);
		}


		lastPressedPad = keyPositionToPads[keyboardPosition];

		if(keyPositionToPads[keyboardPosition] == BACKSPACE_PAD_1 || keyPositionToPads[keyboardPosition] == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 1, mtConfig.values.padsLightFront);
			leds.setLED(BACKSPACE_PAD_2, 1, mtConfig.values.padsLightFront);
		}
		else if(keyPositionToPads[keyboardPosition] == CAPS_LOCK_PAD_1 || keyPositionToPads[keyboardPosition] == CAPS_LOCK_PAD_2) //capslock
		{
			leds.setLED(CAPS_LOCK_PAD_1, 1, mtConfig.values.padsLightFront);
			leds.setLED(CAPS_LOCK_PAD_2, 1, mtConfig.values.padsLightFront);
		}
		else if(keyPositionToPads[keyboardPosition] >= SPACE_PAD_1 && keyPositionToPads[keyboardPosition] <=SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 1, mtConfig.values.padsLightFront);
			}
		}
		else
		{
			leds.setLED(keyPositionToPads[keyboardPosition],1,mtConfig.values.padsLightFront);
		}
		//////////////////////////////////////
		if(smallKeyboard[keyboardPosition] > 1)
		{
			if(editPosition == 31) return;
			uint8_t localNameLen = strlen(name);
			if(editPosition < localNameLen)
			{
				for(uint8_t i = localNameLen; i >= editPosition ; i-- )
				{
					name[i+1] = name[i];
				}
			}

			name[editPosition] = keyboardShiftFlag ? bigKeyboard[keyboardPosition] : smallKeyboard[keyboardPosition];
			name[editPosition + 1] = 0;

			editPosition++;
		}
		else if(smallKeyboard[keyboardPosition] == 0)
		{
			if(editPosition == 0 ) return;

			name[editPosition-1] = 0;
			editPosition--;
		}
		else if(smallKeyboard[keyboardPosition] == 1)
		{
			keyboardShiftFlag = ! keyboardShiftFlag;
			showKeyboard();
		}

		showKeyboardEditName();
	}
}


void mtKeyboardManager::makeBackspace()
{
	if(keyboardActiveFlag)
	{
		if(editPosition == 0 ) return;

		name[editPosition-1] = 0;
		editPosition--;
		showKeyboardEditName();
	}
}

void mtKeyboardManager::makeMove(char c)
{
	if(keyboardActiveFlag)
	{
		switch(c)
		{
			case 'w': 	keyboardPosition = valueMap[valueMapDirectionUp][keyboardPosition];		break;
			case 's':	keyboardPosition = valueMap[valueMapDirectionDown][keyboardPosition];	break;
			case 'a':	keyboardPosition = valueMap[valueMapDirectionLeft][keyboardPosition];	break;
			case 'd':	keyboardPosition = valueMap[valueMapDirectionRight][keyboardPosition];	break;
			default: break;
		}
		showKeyboard();
	}

}

uint8_t mtKeyboardManager::getState()
{
	return keyboardActiveFlag;
}

void mtKeyboardManager::showKeyboard()
{
	if(keyboardControl == nullptr) return;

	leds.setLED(F_PAD, 1, mtConfig.values.padsLightBack);
	leds.setLED(J_PAD, 1, mtConfig.values.padsLightBack);

	if(keyboardShiftFlag) display.setControlValue(keyboardControl, keyboardPosition + 42);
	else display.setControlValue(keyboardControl, keyboardPosition);

	display.setControlShow(keyboardControl);
	display.refreshControl(keyboardControl);
}

void mtKeyboardManager:: hideKeyboard()
{
	if(lastPressedPad == BACKSPACE_PAD_1 || lastPressedPad == BACKSPACE_PAD_2)
	{
		leds.setLED(BACKSPACE_PAD_1, 0, 0);
		leds.setLED(BACKSPACE_PAD_2, 0, 0);
	}
	else if(lastPressedPad == CAPS_LOCK_PAD_1 || lastPressedPad == CAPS_LOCK_PAD_2)
	{
		leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
		leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
	}
	else if(lastPressedPad >= SPACE_PAD_1 && lastPressedPad <=SPACE_PAD_5)
	{
		for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
		{
			leds.setLED(i, 0, 0);
		}
	}
	else
	{
		leds.setLED(lastPressedPad,0,0);
	}
	leds.setLED(F_PAD, 0, 0);
	leds.setLED(J_PAD, 0, 0);

	display.setControlHide(keyboardControl);
	display.refreshControl(keyboardControl);
}
void mtKeyboardManager::showKeyboardEditName()
{
	if(editName == nullptr) return;

	display.setControlValue(editName, editPosition);

	display.setControlText(editName, name);
	display.setControlShow(editName);
	display.refreshControl(editName);
}

void mtKeyboardManager:: hideEditName()
{
	display.setControlHide(editName);
	display.refreshControl(editName);
}
