
#include "i2c_t3.h"
#include "keyScanner.h"



void IO7326_INT_FUNCT_A();
void IO7326_INT_FUNCT_B();
void IO7326_INT_FUNCT_C();


void print_scan_status(uint8_t target)
{
    switch(Wire2.status())
    {
    case I2C_WAITING:
        Serial.printf("Addr: 0x%02X ACK\n", target);
        break;
    case I2C_ADDR_NAK:
    	Serial.printf("Addr: 0x%02X\n", target);
        break;
    default:
        break;
    }
}


keyScanner::keyScanner()
{
	/*
	ucAddr = IO7326_ADDR1;
	IO7326_int_pin = 26;
	IO7326_sda_pin = 18;
	IO7326_scl_pin = 19;
	*/ 
}

keyScanner::keyScanner(uint8_t address, uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin)
{
	ucAddr = address;
	IO7326_int_pin = int_pin;
	IO7326_sda_pin= sda_pin;
	IO7326_scl_pin = scl_pin;
}

uint8_t keyScanner::setButtonPushFunc(void (*func)(uint8_t,uint8_t))
{
	if(func == NULL) return statusError;
	onPush = func;
	return statusSuccess;
}
uint8_t keyScanner::setButtonReleaseFunc(void (*func)(uint8_t,uint8_t))
{
	if(func == NULL) return statusError;
	onRelease = func;
	return statusSuccess;
}
uint8_t keyScanner::setButtonHoldFunc(void (*func)(uint8_t,uint8_t))
{
	if(func == NULL) return statusError;
	onHold = func;
	return statusSuccess;
}
uint8_t keyScanner::setButtonDoubleFunc(void (*func)(uint8_t,uint8_t))
{
	if(func == NULL) return statusError;
	onDouble = func;
	return statusSuccess;
}


void keyScanner::setHoldTime(uint16_t time){button_hold_max = time;}
void keyScanner::setDoubleTime(uint16_t time){button_double_time_max = time;}



void keyScanner::begin(uint8_t address, uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, void (*func)(void))
{

	ucAddr = address;
	IO7326_int_pin = int_pin;
	IO7326_sda_pin= sda_pin;
	IO7326_scl_pin = scl_pin;

	PORTA_PCR4|=PORT_PCR_MUX(1);
	GPIOA_PDDR|= (1<<4);
	GPIOA_PDOR|= (1<<4);



	//Wire.begin(I2C_MASTER, 0x00, IO7326_scl_pin, IO7326_sda_pin); 
	
#ifdef KEYSCANNER_I2C_DMA
	Wire2.begin(I2C_MASTER, 0x00, IO7326_scl_pin, IO7326_sda_pin, I2C_PULLUP_EXT, 400000, I2C_OP_MODE_DMA);
#else
	Wire2.begin(I2C_MASTER, 0x00, IO7326_scl_pin, IO7326_sda_pin, I2C_PULLUP_EXT, 400000);
#endif
	Wire2.setDefaultTimeout(250000); // 250ms default timeout
	/*
		init i2c układów skanujących
	*/
	Wire2.beginTransmission(ucAddr);
	Wire2.write(uint8_t(IS31_CONF_REGISTER));
	Wire2.write(uint8_t((0 << 7) | (IS31_ACIa << 6) | (IS31_ACIb << 5) | (IS31_DE << 4) | (IS31_SD << 3) | (IS31_LE << 2) | (IS31_LTa << 1) | (IS31_LTb << 0)) );
	Wire2.endTransmission();

	/*
		init IO i przerwan od kontrolerów przycisków
	*/
	pinMode(IO7326_int_pin,INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(IO7326_int_pin), func, FALLING);
}


uint8_t keyScanner::update()
{
	uint8_t status = 0;
	if (Wire2.done())
	{
		if (IO7326_int)
		{
			IO7326_int=0;
			read_buttons_IC(0);
			status = 1;
		}

		//handle_howManyPressed();
	}


	if (checkIntPins > checkIntPinsMax)
	{
		checkIntPins = 0;
		if ((digitalReadFast(IO7326_int_pin) == 0) && (IO7326_int == 0))
		{
			IO7326_int = 1;
			if(test_mode) Serial.println("IO7326 int low");
		}
	}


	if (timeoutTrigger >= timeoutTrigger_max)
	{
		timeoutTrigger = 0;
		handle_buttonTimeout();
	}

	return status;
}


void keyScanner::intAction()
{
	if((intProtect >= intProtect_max) && (digitalRead(IO7326_int_pin) == 0))
	{
		intProtect = 0;
		IO7326_int = 1;
	}
	if(test_mode > 1)
	{
		Serial.println("IO7326_int");
	}
}


void keyScanner::read_buttons_IC(uint8_t grid_no) //grid no 0-3
{
	while (!Wire2.done()) {};

	Wire2.beginTransmission(ucAddr);         // slave addr
	Wire2.write(IS31_KEY_STATUS_REGISTER);                       // memory address
	Wire2.endTransmission();       // blocking write (NOSTOP triggers RepSTART on next I2C command)
	print_scan_status(ucAddr);
	//delay(1);

	Wire2.requestFrom((uint8_t)ucAddr, (uint8_t)1); // NON-blocking read (request 256 bytes)

	// Since request is non-blocking, do some other things.
	//Serial.print("...request sent, doing one thing then waiting for Wire.finish()...\n");

	// After doing something, use finish() to wait until I2C done

	//Wire.finish();

	//Serial.printf("I2C READ %d bytes from Slave 0x%0X at MemAddr %d\n", Wire.available(), target, addr);
	//Serial.print("Received: ");             // print received bytes
	uint8_t ob = 0;

	while (Wire2.available())
	{
		ob++;

		uint8_t rcvd = Wire2.read();
		//if(!rcvd) break;

		uint8_t dn = (rcvd & 0b10000000) >> 7;
		uint8_t ks = (rcvd & 0b01000000) >> 6;
		uint16_t key = rcvd & 0b00111111;
		//key += 64 * grid_no;

		if(key >= 64) 
		{
			if (test_mode) {Serial.print("Error: (key number > 63) - "); Serial.println(key);}
			return;
		}

		if (test_mode )
		{
			Serial.print("rcvd\t");
			Serial.println(rcvd, BIN);
			Serial.print("ob:\t");
			Serial.println(ob);
			Serial.print("ks:\t");
			Serial.println(ks);
			Serial.print("dn:\t");
			Serial.println(dn);
		}


		if (dn)
		{
			IO7326_int++;
		}


		if (ks && (button[key] == 0))
		{
			//PRESS
			button[key] = 1;

			buttonsPressed++;

			action_button_press(key);

			button_still_pressing[key] = 10;
		}
		else if (!ks && (button[key] >= 1))
		{
			//RELEASE - release status receive
			if(button[key] == 1)
			{
				button_double_time[key] = doublePushTime - button_double_time[key];

				if(button_double[key] && button_double_time[key] < button_double_time_max)
				{
					button_double[key] = 3;
				}
				else
				{
					if(buttonsPressed == 1) doublePushTime = 0;
					button_double_time[key] = doublePushTime;
					button_double[key] = 1;
				}
			}

			button[key] = 0;
			button_hold[key] = 0;
			button_still_pressing[key] = 0;
			buttonsPressed--;

			action_button_release(key);

			if(button_double[key] == 3) 
			{
				action_button_double(key);
				button_double[key] = 0;
			}

		}
		else if (ks && (button[key] == 1))
		{
			//HOLD
			button_hold[key] = button_hold[key] + 37;
			if (button_hold[key] >= button_hold_max)
			{
				button[key] = 2;

				action_button_hold(key);
			}
		}

		if (ks && (button[key] >= 1))
		{
			button_still_pressing[key] = 20;
		}
	}

}


/*void keyScanner::handle_howManyPressed()
{
	uint8_t tempButtoonsPressed = 0;

	for (uint8_t x = 0; x < BUTTONS_MAX_COUNT; x++)
	{
		// sprawdzam jaki typ przycisku wcisniety
		if (button[x] > 0)
		{
			tempButtoonsPressed++;
		}
	}
	buttonsPressed = tempButtoonsPressed;
}*/


void keyScanner::handle_buttonTimeout()
{
//	uint8_t tempButtonsPressed = 0;

	for (uint16_t x = 0; x < BUTTONS_MAX_COUNT; x++)
	{
/*		// sprawdzam jaki typ przycisku wcisniety
		if (button[x] > 0)
		{
			tempButtonsPressed++;
		}
		buttonsPressed = tempButtonsPressed;
*/

		if (button_still_pressing[x] > 0)
		{
			button_still_pressing[x]--;

			if (button_still_pressing[x] <= 5 )
			{
				//RELEASED - no more press status received
				//---------------DOUBLE------------------
				if(button[x] == 1)
				{
					button_double_time[x] = doublePushTime - button_double_time[x];
	
					if(button_double[x] && button_double_time[x] < button_double_time_max)
					{
						button_double[x] = 3;
					}
					else 
					{
						if(buttonsPressed == 1) doublePushTime = 0;
						button_double_time[x] = doublePushTime;
						button_double[x] = 1;
					}
				}
				//------------DOUBLE_END------------------

				button[x] = 0;
				button_hold[x] = 0;
				button_still_pressing[x] = 0;
				buttonsPressed--;

				action_button_release(x);

				if(button_double[x] == 3) 
				{
					action_button_double(x);
					button_double[x] = 0;
				}
			}
		}
/*		else
		{
			button_timeout[x] = 0;
		}
*/
	}

}


void keyScanner::action_button_press(uint8_t num)
{
	uint8_t x=0,y=0;

	if (test_mode)
	{
		Serial.print("- PushButton ("); Serial.print(num);
		Serial.println(")");
	}

	if(IO7326_int_pin == GRID_A)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 0;

	}

/*	else if(IO7326_int_pin == GRID_B)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 8;

	}
	else if(IO7326_int_pin == GRID_C)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 16;
	}
	//handle_howManyPressed();
*/
	onPush(x,y);
}


void keyScanner::action_button_hold(uint8_t num)
{
	uint8_t x=0,y=0;

	if (test_mode)
	{
		Serial.print("  |    HoldButton ("); Serial.print(num);
		Serial.println(")");
	}

	if(IO7326_int_pin == GRID_A)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 0;

	}

/*	else if(IO7326_int_pin == GRID_B)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 8;

	}
	else if(IO7326_int_pin == GRID_C)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 16;
	}
*/
	onHold(x,y);
}

void keyScanner::action_button_release(uint8_t num)
{
	uint8_t x=0,y=0;

	if (test_mode)
	{
		Serial.print("  |____ReleaseButton ("); Serial.print(num);
		Serial.println(")"); 
	}

	if(IO7326_int_pin == GRID_A)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 0;

	}
/*
	else if(IO7326_int_pin == GRID_B)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 8;

	}
	else if(IO7326_int_pin == GRID_C)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 16;
	}
*/

	onRelease(x,y);
}

void keyScanner::action_button_double(uint8_t num)
{
	uint8_t x=0,y=0;

	if(test_mode)
	{
		Serial.print("++++++ DoubleButton ("); Serial.print(num);
		Serial.println(")");
	}

	if(IO7326_int_pin == GRID_A)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 0;

	}
/*
  else if(IO7326_int_pin == GRID_B)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 8;

	}
	else if(IO7326_int_pin == GRID_C)
	{
		x = (num / 8) + 0;
		y = (num % 8) + 16;
	}
*/
	onDouble(x,y);
}

/*uint8_t keyScanner::getButtonState(uint8_t number)
{
	return button[number];
}

bool keyScanner::isButtonPressed(uint8_t number)
{
	return button[number] > 0;
}

bool keyScanner::isButtonHold(uint8_t number)
{
	return button[number] == 2;
}

uint8_t keyScanner::xy2key(uint8_t x, uint8_t y)
{
	return ((y - 1) * 8) + x - 1;
}

uint8_t keyScanner::buttonToX(uint8_t number)
{
	return (number % 8) + 1;
}

uint8_t keyScanner::buttonToY(uint8_t number)
{
	return (number / 8) + 1;
*/
void keyScanner::testMode(uint8_t set)
{
	test_mode = set;
}
