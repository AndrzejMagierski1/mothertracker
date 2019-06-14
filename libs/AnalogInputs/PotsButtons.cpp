
#include "AnalogInputs.h"

#include "Arduino.h"

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::readButtons()
{
	//reading pots & buttons signals
//	uint8_t pot_nr = 0;

//	for(uint8_t i=0;i<ANALOG_MAX_POT_MUXS;i++)
//	{
//		pot_nr = i*16+reading_channel;

		new_button_values[ANALOG_BUTTON1] = digitalRead(BUTTON1) > 0 ? 4095 : 0;
		new_button_values[analog_buttons_mux_channels0[reading_channel]] = digitalRead(analog_pots_muxs_signal[0]) > 0 ? 4095 : 0;



		//new_button_values[analog_buttons_mux_channels0[reading_channel]] = analogRead(analog_pots_muxs_signal[0]);

		//Serial.print(new_button_values[analog_buttons_mux_channels0[reading_channel]]);

		new_button_values[analog_buttons_mux_channels1[reading_channel]] = analogRead(analog_pots_muxs_signal[1]);

		//if(reading_channel == 16)Serial.println();
		//delay(10);
		//	}

}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------



void cAnalogInputs::processButtonData()
{
	for(uint8_t i = 0 ; i < ANALOG_MAX_BUTTONS ; i++)
	{
		if(new_button_values[i] < 2048)
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
		else if(new_button_values[i] >= 2048)
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




