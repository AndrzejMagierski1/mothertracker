
#include "AnalogInputs.h"

uint8_t mux2pad[64] =
{
		56,48,40,32,24,16,8,0,
		57,49,41,33,25,17,9,1,
		58,50,42,34,26,18,10,2,
		59,51,43,35,27,19,11,3,
		60,52,44,36,28,20,12,4,
		61,53,45,37,29,21,13,5,
		62,54,46,38,30,22,14,6,
		63,55,47,39,31,23,15,7
};

uint8_t pad2mux[64] =
{
		7,15,23,31,39,47,55,63,
		6,14,22,30,38,46,54,62,
		5,13,21,29,37,45,53,61,
		4,12,20,28,36,44,52,60,
		3,11,19,27,35,43,51,59,
		2,10,18,26,34,42,50,58,
		1,9,17,25,33,41,49,57,
		0,8,16,24,32,40,48,56
};
//-----------------------------------------------------------------------------------------------
void cAnalogInputs::readPadPressPosition()
{
	//reading pads signals (x/y) and comparing with previouse ones
	int16_t new_value = 0;
	uint8_t button_nr = 0;
	//uint8_t y_dead_zone = ANALOG_BUTTON_POSITION_DEAD_ZONE;

	for(uint8_t i=0;i<ANALOG_MAX_PAD_MUXS;i++)
	{
		button_nr = i*8+reading_channel/2;
		new_value = (2047 - analogRead(analog_pads_muxs_order[i]))/16;    ////// 127-255 = -128

		if(reading_channel%2 == 0) // X
		{
			// jesli sila zminiala sie
			if(padButtons[button_nr].count_fx > 0)
			{
				// mniej niz 5 odczytow sily w osi x
				if(padButtons[button_nr].count_fx < 5)
				{
					if(!pads_mode_x) // 0 - absolutie
					{
						padButtons[button_nr].dzone_x = ANALOG_BUTTON_POSITION_DEAD_ZONE_NORMAL;
					}
					else // 1 - relatywnie
					{
						padButtons[button_nr].start_x = new_value;
						padButtons[button_nr].dzone_x = ANALOG_BUTTON_POSITION_DEAD_ZONE_START;
						padButtons[button_nr].x = 0;
					}
				}
				// odejmuje od odczytu wartosc poczatkowa  // 1 - relatywnie
				if(pads_mode_x) new_value = new_value-padButtons[button_nr].start_x;

				if(new_value > 127) new_value = 127;
				else if(new_value < -127) new_value = -127;

				// jesli przekoroczona delete = zmiana polozenia w osi x
				if(new_value > padButtons[button_nr].x+padButtons[button_nr].dzone_x
				|| new_value < padButtons[button_nr].x-padButtons[button_nr].dzone_x)
				{
					// zmiana stanow osi x
					if(padButtons[button_nr].state_x == 0)
					{
						padButtons[button_nr].state_x = 5;
					}
					else if(padButtons[button_nr].state_x == 1)
					{
						padButtons[button_nr].dzone_x = ANALOG_BUTTON_POSITION_DEAD_ZONE_NORMAL;
						padButtons[button_nr].state_x = 6;
					}

					padButtons[button_nr].x = new_value;
				}
			}
			// sila na osi x rowna zero
			else
			{
				padButtons[button_nr].x = 0;
			}

		}
		else // Y
		{
			if(padButtons[button_nr].count_fy > 0)
			{
				if(padButtons[button_nr].count_fy < 5)
				{
					if(!pads_mode_y) //
					{
						padButtons[button_nr].dzone_y = ANALOG_BUTTON_POSITION_DEAD_ZONE_NORMAL;
					}
					else
					{
						padButtons[button_nr].start_y = new_value;
						padButtons[button_nr].dzone_y = ANALOG_BUTTON_POSITION_DEAD_ZONE_START;
						padButtons[button_nr].y = 0;
					}
				}
				if(pads_mode_y) new_value = new_value-padButtons[button_nr].start_y;

				if(new_value > 127) new_value = 127;
				else if(new_value < -127) new_value = -127;

				if(new_value > padButtons[button_nr].y+padButtons[button_nr].dzone_y
				|| new_value < padButtons[button_nr].y-padButtons[button_nr].dzone_y)
				{
					if(padButtons[button_nr].state_y == 0)
					{
						padButtons[button_nr].state_y = 5;
					}
					else if(padButtons[button_nr].state_y == 1)
					{
						padButtons[button_nr].dzone_y = ANALOG_BUTTON_POSITION_DEAD_ZONE_NORMAL;
						padButtons[button_nr].state_y = 6;
					}

					padButtons[button_nr].y = new_value*(-1); /// odwrocenie osi y
				}
			}
			else
			{
				padButtons[button_nr].y = 0;
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::readPadPressForce()
{
	//reading pads signals (force)
	int16_t new_value = 0;
	uint8_t button_nr = 0;

	for(uint8_t i=0;i<ANALOG_MAX_PAD_MUXS;i++)
	{
		button_nr = i*8+reading_channel/2;
		new_value = ((255-ANALOG_BUTTON_FORCE_OFFSET) - (analogRead(analog_pads_muxs_order[i])/16));


		if(reading_channel%2 == 0) // nacisk na pad w osi X
		{
			if(new_value < ANALOG_BUTTON_FORCE_START_DEAD_ZONE+padButtons[button_nr].start_point_fx) // wartosc mniejsza niz graniczna
			{
				if(padButtons[button_nr].state_x == 0 || padButtons[button_nr].state_x == 5)
				{
					padButtons[button_nr].state_x = 0;
				}
				else if(padButtons[button_nr].state_x == 1 || padButtons[button_nr].state_x == 6)
				{
					padButtons[button_nr].state_x = 3;
					//padButtons[button_nr].fx = new_value;
					padButtons[button_nr].count_fx = 0;
				}
				else if(padButtons[button_nr].state_x == 4)
				{
					padButtons[button_nr].state_x = 0;
				}

				padButtons[button_nr].start_point_fx = new_value;
			}
			else //wartosc wieksza niz graniczna
			{
				if(padButtons[button_nr].count_fx < 20)
					padButtons[button_nr].count_fx++;

				// czy zaszla zmiana sily nacisku
				if(	padButtons[button_nr].count_fx < 3)
				{
					if(padButtons[button_nr].state_x == 0 || padButtons[button_nr].state_x == 5)
					{
						if(padButtons[button_nr].count_fx == 1)
						{
							padButtons[button_nr].velo_timer_x = 0;
							padButtons[button_nr].state_x = 0;
							padButtons[button_nr].fx = new_value;
						}
						else if(padButtons[button_nr].count_fx > 1)
						{
							padButtons[button_nr].state_x = 2;
							padButtons[button_nr].start_point_fx = 0;
							padButtons[button_nr].fx2 = new_value;
							padButtons[button_nr].time_x = padButtons[button_nr].velo_timer_x;
						}


					}

				}
				else if(new_value > (padButtons[button_nr].fx+ANALOG_BUTTON_FORCE_DEAD_ZONE)
						|| new_value < (padButtons[button_nr].fx-ANALOG_BUTTON_FORCE_DEAD_ZONE))
				{
					//if(padButtons[button_nr].state_x == 0 || padButtons[button_nr].state_x == 5)
					//{
					//	padButtons[button_nr].state_x = 4;

					//	padButtons[button_nr].fx = new_value;
					//}
					//else if(padButtons[button_nr].state_x == 1 || padButtons[button_nr].state_x == 6)
					//{
						/*if(padButtons[button_nr].state_x == 6)*/
						padButtons[button_nr].state_x = 4;

						padButtons[button_nr].fx = new_value;
					//}
				}
				else
				{
					if(padButtons[button_nr].state_x == 0)
					{
						padButtons[button_nr].state_x = 0;
					}
					else if(padButtons[button_nr].state_x == 1)
					{
						padButtons[button_nr].state_x = 1;
					}
					else if(padButtons[button_nr].state_x == 5)
					{
						padButtons[button_nr].state_x = 0;
					}
					else if(padButtons[button_nr].state_x == 6)
					{
						padButtons[button_nr].state_x = 4;
					}
				}
			}
		}
		else  // nacisk na pad w osi Y
		{

			if(new_value < ANALOG_BUTTON_FORCE_START_DEAD_ZONE+padButtons[button_nr].start_point_fy) // wartosc mniejsza niz graniczna
			{
				if(padButtons[button_nr].state_y == 0 || padButtons[button_nr].state_y == 5)
				{
					padButtons[button_nr].state_y = 0;
				}
				else if(padButtons[button_nr].state_y == 1 || padButtons[button_nr].state_y == 6)
				{
					padButtons[button_nr].state_y = 3;
					//padButtons[button_nr].fx = new_value;
					padButtons[button_nr].count_fy = 0;
				}
				else if(padButtons[button_nr].state_y == 4)
				{
					padButtons[button_nr].state_y = 0;
				}

				padButtons[button_nr].start_point_fy = new_value;
			}
			else //wartosc wieksza niz graniczna
			{
				if(padButtons[button_nr].count_fy < 20)
					padButtons[button_nr].count_fy++;


				if(padButtons[button_nr].count_fy < 3)
				{
					if(padButtons[button_nr].state_y == 0 || padButtons[button_nr].state_y == 5)
					{
						if(padButtons[button_nr].count_fy == 1)
						{
							padButtons[button_nr].velo_timer_y = 0;
							padButtons[button_nr].state_y = 0;
							padButtons[button_nr].fy = new_value;
						}
						else if(padButtons[button_nr].count_fy > 1)
						{
							padButtons[button_nr].state_y = 2;
							padButtons[button_nr].start_point_fy = 0;
							padButtons[button_nr].fy2 = new_value;
							padButtons[button_nr].time_y = padButtons[button_nr].velo_timer_y;
						}



					}
				}
				else if(new_value > (padButtons[button_nr].fy+ANALOG_BUTTON_FORCE_DEAD_ZONE)
						|| new_value < (padButtons[button_nr].fy-ANALOG_BUTTON_FORCE_DEAD_ZONE))
				///*padButtons[button_nr].state_y == 1 || */padButtons[button_nr].state_y == 6)
				{
					/*if(padButtons[button_nr].state_y == 6)*/
					padButtons[button_nr].state_y = 4;
					padButtons[button_nr].fy = new_value;
				}
				else
				{
					if(padButtons[button_nr].state_y == 0)
					{
						padButtons[button_nr].state_y = 0;
					}
					else if(padButtons[button_nr].state_y == 1)
					{
						padButtons[button_nr].state_y = 1;
					}
					else if(padButtons[button_nr].state_y == 5)
					{
						padButtons[button_nr].state_y = 0;
					}
					else if(padButtons[button_nr].state_y == 6)
					{
						padButtons[button_nr].state_y = 4;
					}
				}
			}
		}

	}

}

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::processPadData()
{
	if(start_up)
	{
		for(uint8_t i= 0;i<64;i++)
		{
			padButtons[i].state_x = 0;
			padButtons[i].state_y = 0;
		}
		#if ANALOG_POTS_ON == 0
		start_up++;
		if(start_up > 10)start_up = 0;
		#endif
		return;
	}

	for(uint8_t i= 0;i<64;i++)
	{
		if(padButtons[i].state_x > 0 || padButtons[i].state_y > 0) // ( x&&y  < 5)
		{
			// 2 & 2 ; 0 & 2 ; 2 & 0
			if((padButtons[i].state_x == 2 && padButtons[i].state_y == 2)
				||(padButtons[i].state_x == 0 && padButtons[i].state_y == 2)
				||(padButtons[i].state_x == 2 && padButtons[i].state_y == 0))
			{

				if(padButtons[i].state_x != 0) padButtons[i].state_x = 1;
				if(padButtons[i].state_y != 0) padButtons[i].state_y = 1;
				PadPressFunc(mux2pad[i],padButtons[i].x,padButtons[i].y,calculateVelocity(i));
			}
			// 3 & 3 ; 0 & 3 ; 3 & 0
			else if((padButtons[i].state_x == 3 && padButtons[i].state_y == 3)
				||(padButtons[i].state_x == 0 && padButtons[i].state_y == 3)
				||(padButtons[i].state_x == 3 && padButtons[i].state_y == 0))
			{
				padButtons[i].start_x = 0;
				padButtons[i].start_y = 0;
				padButtons[i].fx = 0;
				padButtons[i].fy = 0;

				PadReleaseFunc(mux2pad[i]);
				if(padButtons[i].state_x == 3) padButtons[i].state_x = 0;
				if(padButtons[i].state_y == 3) padButtons[i].state_y = 0;
			}
			// 4 || 4 => x & 4 / 4 & x
			else if(padButtons[i].state_x == 4 || padButtons[i].state_y == 4)
			{
				PadChangeFunc(mux2pad[i],padButtons[i].x,padButtons[i].y,calculateForce(i));
				if(padButtons[i].state_x == 0 || padButtons[i].state_x == 3) padButtons[i].state_x = 0;
				else padButtons[i].state_x = 1;
				if(padButtons[i].state_y == 0 || padButtons[i].state_y == 3) padButtons[i].state_y = 0;
				else padButtons[i].state_y = 1;
			}
			// 1 & 3 ; 3 & 1
			// 1 & 2 ; 2 & 1
			else if((padButtons[i].state_x == 1 && padButtons[i].state_y == 3)
				||(padButtons[i].state_x == 3 && padButtons[i].state_y == 1)
				||(padButtons[i].state_x == 2 && padButtons[i].state_y == 1)
				||(padButtons[i].state_x == 1 && padButtons[i].state_y == 2))
			{
				PadChangeFunc(mux2pad[i],padButtons[i].x,padButtons[i].y,calculateForce(i));
				if(padButtons[i].state_x == 2 || padButtons[i].state_x == 1) padButtons[i].state_x = 1;
				else padButtons[i].state_x = 0;
				if(padButtons[i].state_y == 2 || padButtons[i].state_y == 1) padButtons[i].state_y = 1;
				else padButtons[i].state_y = 0;
			}
			// 2 & 3 ; 3 & 2
			else if((padButtons[i].state_x == 2 && padButtons[i].state_y == 3)
				||(padButtons[i].state_x == 3 && padButtons[i].state_y == 2))
			{
				PadChangeFunc(mux2pad[i],padButtons[i].x,padButtons[i].y,calculateForce(i));
				if(padButtons[i].state_x == 2) padButtons[i].state_x = 1;
				else padButtons[i].state_x = 0;
				if(padButtons[i].state_y == 2) padButtons[i].state_y = 1;
				else padButtons[i].state_y = 0;
			}
			// x && x (x != 0)
			else if(padButtons[i].state_x != 0 && padButtons[i].state_y != 0)
			{
				if(padButtons[i].state_x != 1 && padButtons[i].state_y != 1)
				{
					//Serial.print(padButtons[i].state_x);Serial.print(padButtons[i].state_y);Serial.println("");
				}
			}
		}
		else
		{
			padButtons[i].state_x = 0;
			padButtons[i].state_y = 0;
		}

	}

}

uint16_t cAnalogInputs::calculateForce(uint8_t pad)
{
	uint16_t force = force_ratio[padButtons[pad].fx+padButtons[pad].fy]/2;
	if(force > 127) force = 127;
	return force;
}

uint16_t cAnalogInputs::calculateVelocity(uint8_t pad)
{
	//	int16_t diff =	 force_ratio[padButtons[pad].pad_buttons_fx2+padButtons[pad].pad_buttons_fy2]/2
	//				   - force_ratio[padButtons[pad].pad_buttons_fx+padButtons[pad].pad_buttons_fy]/2;
	//	if(diff <= 0) 		diff = 100;
	//	else if(diff > 127) diff = 127;

		uint16_t force = force_ratio[padButtons[pad].fx2+padButtons[pad].fy2]/2;

	//	uint16_t velocity = 127-diff;
	/*
		Serial.print("diff: ");
		Serial.print(diff);
		Serial.print(" czas: ");
		Serial.print(padButtons[pad].time_x);
		Serial.print(" velo: ");
		Serial.print(velocity);
		Serial.print(" force: ");
		Serial.println(force);
	*/
		if(force > 127) force = 127;
		return force;
}

uint8_t cAnalogInputs::howManyPadPressed()
{
	uint8_t counter = 0;
	for(uint8_t i = 0;i < 64; i++)
	{
		if(	padButtons[i].state_x == 1 || padButtons[i].state_x == 2  || padButtons[i].state_x == 4  || padButtons[i].state_x == 6 ||
			padButtons[i].state_y == 1 || padButtons[i].state_y == 2  || padButtons[i].state_y == 4  || padButtons[i].state_y == 6
			) counter++;
	}
	//Serial.print("pads press count: ");
	//Serial.println(counter);
	return counter;
}

uint8_t cAnalogInputs::isPadPressed(uint8_t pad)
{
	pad = pad2mux[pad];
	if(	padButtons[pad].state_x == 1 || padButtons[pad].state_x == 2  || padButtons[pad].state_x == 4  || padButtons[pad].state_x == 6 ||
		padButtons[pad].state_y == 1 || padButtons[pad].state_y == 2  || padButtons[pad].state_y == 4  || padButtons[pad].state_y == 6
		) return 1;
	return 0;
}

void cAnalogInputs::setPadxMode(uint8_t mode)
{
	pads_mode_x = mode;
}

void cAnalogInputs::setPadyMode(uint8_t mode)
{
	pads_mode_y = mode;
}
