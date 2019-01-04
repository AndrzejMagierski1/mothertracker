
#include "AnalogInputs.h"



//-----------------------------------------------------------------------------------------------
void cAnalogInputs::readPotButtons()
{
	//reading pots & buttons signals
//	uint8_t pot_nr = 0;

//	for(uint8_t i=0;i<ANALOG_MAX_POT_MUXS;i++)
//	{
//		pot_nr = i*16+reading_channel;
		new_pot_button_values[analog_pots_buttons_mux_channels[reading_channel]] = analogRead(analog_pots_muxs_order[0]);
//	}

}

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::processPotData()
{
	uint16_t A, B;

	for(uint8_t i=0;i<ANALOG_MAX_POTS;i++)
	{
		A = new_pot_button_values[analog_pots_index_A[i]];
		B = new_pot_button_values[analog_pots_index_B[i]];
		potentiometers[i].positions[0] = calculatePotPosition(A ,B, &(potentiometers[i].part));

		if(start_up) potentiometers[i].positions[1] = potentiometers[i].positions[0];
	}

	if(start_up) start_up = 0;
	int16_t diffrence, is_moving_diff, resolution_step;
	uint8_t in_death_zone, direction;

	for(uint8_t i=0;i<ANALOG_MAX_POTS;i++)
	{
		// roznica pozycji pomiedzy aktualna a ostanio zapisana
		diffrence = potentiometers[i].positions[0] - potentiometers[i].positions[1];

		if(diffrence > 0)
		{
			if(potentiometers[i].last_part < 2 && potentiometers[i].part > 5)
			{
				diffrence = 1023 - diffrence;
				diffrence = diffrence * (-1);
			}
			else if(potentiometers[i].last_part < 2 && potentiometers[i].part > 5)
			{
				diffrence = 0;
			}
		}
		else if(diffrence < 0)
		{
			if(potentiometers[i].last_part > 5 && potentiometers[i].part < 2)
			{
				diffrence = 1023 + diffrence;
			}
			else if(potentiometers[i].last_part > 5 && potentiometers[i].part < 2)
			{
				diffrence = 0;
			}
		}

		potentiometers[i].last_part = potentiometers[i].part;

		potentiometers[i].diffrences[2] = potentiometers[i].diffrences[1];
		potentiometers[i].diffrences[1] = potentiometers[i].diffrences[0];
		potentiometers[i].diffrences[0] = diffrence;

		is_moving_diff =  potentiometers[i].diffrences[2]
						+ potentiometers[i].diffrences[1] + potentiometers[i].diffrences[0];

		in_death_zone = (is_moving_diff > (pots_death_zone * (-1)) &&  is_moving_diff < pots_death_zone);

		if(in_death_zone) continue;

		// zapisywanie poprzedniej pozycji dopiero po wykryciu zmiany
		potentiometers[i].positions[1] = potentiometers[i].positions[0];

//		mtPrint(potentiometers[i].diffrences[2]); mtPrint(" ");
//		mtPrint(potentiometers[i].diffrences[1]); mtPrint(" ");
//		mtPrint(potentiometers[i].diffrences[0]); mtPrint(" ");
//		mtPrintln();
//		Serial.print(" ");

		// zerowanie global_diff przy zmianie kierunkudirection = 1;
		direction = 0;
		if(diffrence > 0) 		direction = 1;
		else if(diffrence < 0) 	direction = 2;
		if(direction != potentiometers[i].last_direction) potentiometers[i].global_diff = 0;
		potentiometers[i].last_direction = direction;

		resolution_step = 1023.0 / potentiometers[i].resolution;

		potentiometers[i].global_diff = potentiometers[i].global_diff + diffrence;

		if(potentiometers[i].global_diff >= resolution_step || potentiometers[i].global_diff <= resolution_step*(-1))
		{
			/// AKCJA
			PotChangeFunc(i, potentiometers[i].global_diff / resolution_step);

			potentiometers[i].global_diff = potentiometers[i].global_diff % resolution_step;
		}

	}

}


uint16_t cAnalogInputs::calculatePotPosition(uint16_t A, uint16_t B, uint8_t * part)
{
	int16_t position = 0;
	*part = 0;

	if(B > (A+2000) ) // 1
	{
		if(A < 400)
		{
			position=map(B,2001,4095,0,243);   //map(B,2048,4095,0,243);
		}
		else
		{
			position=map(A,0,4095,20,499);
		}
		*part = 1;
	}

	else if( (B >= A ) && (A > 2000) && (B > 2047) ) // 2
	{
		position=map(A,0,4095,12,499);
		*part = 2;
	}

	else if( (A >= B) && (A > 2047) && (B > 2020) ) //3
	{
		position=map(B,4095,0,268,755);
		*part = 3;
	}

	else if( A > (B+2000)) //4
	{
		if(A>2900)
		{
			position=map(B,4095,0,268,755);
		}
		else
		{
			position=map(A,4095,0,530,1008);
		}
		*part = 4;
	}

	else if( (A >= B)  && (A < 2150) && (B < 2047)) //5
	{
		position=map(A,4095,0,524,1011);
		*part = 5;
	}

	else if( (B >= A) && (A < 2047) && (B < 2070)) //6
	{
		position=map(B,0,2047,780,1023);
		*part = 6;
	}
	else
	{
		if(A < 400)
		{
			position=map(B,2048,4095,0,243);
		}
		else
		{
			position=map(A,0,4095,20,499);
		}
		*part = 7;
	}


	if(position < 0) 			position = 0;
	else if(position > 1023) 	position = 1023;
	return position;
}


//-----------------------------------------------------------------------------------------------
// n = 0-5
// value 0-1023
void cAnalogInputs::setPotResolution(uint16_t n, uint16_t value)
{
	potentiometers[n].resolution = value;
	potentiometers[n].global_diff = 0;
}

//-----------------------------------------------------------------------------------------------
// value = 0-?
void cAnalogInputs::setPotDeathZone(uint16_t value)
{
	pots_death_zone = value;
}




void cAnalogInputs::processButtonData()
{
	for(uint8_t i = 0 ; i < ANALOG_MAX_BUTTONS ; i++)
	{
		if(new_pot_button_values[analog_buttons_index[i]] < 2048)
		{
			if(buttons[i].debounce <= 0 && buttons[i].debounce > (0-ANALOG_BUTTON_DEBOUNCE) )
			{
				buttons[i].debounce--;
			}
			else if(buttons[i].debounce <= 0 && buttons[i].state != 1)
			{
				buttons[i].debounce = 1;
				buttons[i].state = 1;
				buttons[i].hold_time = 0;
				// press
				ButtonChangeFunc(i,1);
			}
			else if(buttons[i].hold_time > ANALOG_BUTTON_HOLD_TIME &&  buttons[i].state != 2)
			{
				 buttons[i].state = 2;
				// hold
				ButtonChangeFunc(i,2);
			}
		}
		else if(new_pot_button_values[analog_buttons_index[i]] >= 2048)
		{
			if(buttons[i].state > 0 && buttons[i].debounce < ANALOG_BUTTON_DEBOUNCE)
			{
				buttons[i].debounce++;
			}
			else if(buttons[i].debounce > 0 && buttons[i].state != 0)
			{
				buttons[i].debounce = 0;
				buttons[i].state = 0;
				//buttons[i].hold_time = 0;
				// release
				ButtonChangeFunc(i,0);
			}


		}
	}





}

uint8_t cAnalogInputs::isButtonPressed(uint8_t button)
{
	return buttons[button].state != 0;
}




