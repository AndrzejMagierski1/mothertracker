
#include "core_pins.h"


#include "AnalogInputs.h"


cAnalogInputs AnalogInputs;


//-----------------------------------------------------------------------------------------------
void cAnalogInputs::begin(uint16_t pot_res)
{
	last_read_time = 0;
	reading_step = 0;
	reading_channel = 0;

/*
	setPotResolution(0, pot_res);
	setPotResolution(1, pot_res);
	setPotResolution(2, pot_res);
	setPotResolution(3, pot_res);
	setPotResolution(4, pot_res);
*/

	//pinMode(ANALOG_CTRL_XY_Z, OUTPUT);

	MUX_CTRL_A_PCR = PORT_PCR_MUX(1);
	MUX_CTRL_B_PCR = PORT_PCR_MUX(1);
	MUX_CTRL_C_PCR = PORT_PCR_MUX(1);
	MUX_CTRL_D_PCR = PORT_PCR_MUX(1);

	MUX_CTRL_A_GPIO_DDR |= (1 << MUX_CTRL_A);
	MUX_CTRL_B_GPIO_DDR |= (1 << MUX_CTRL_B);
	MUX_CTRL_C_GPIO_DDR |= (1 << MUX_CTRL_C);
	MUX_CTRL_D_GPIO_DDR |= (1 << MUX_CTRL_D);


	analogReadRes(ANALOG_ADC_RESOLUTION);
}

//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
void cAnalogInputs::setButtonChangeFunc(void (*func)(uint8_t n, uint8_t value))
{
	ButtonChangeFunc = func;
}

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::update()
{

	//--------------------------------------------------------------------------------------
	//początek cyklu; zerowanie zmiennych; ustawienie wartosci poczatkowych
	if(last_read_time > ANALOG_READING_INTERVAL && reading_step == 0)
	{  //setting up
		setMux(analog_mux_channels_order[0]);
		reading_channel = 0;
		reading_step = 1;
		last_read_time = 0;
	}
	//--------------------------------------------------------------------------------------
	// odczyt sily nacisku i pozycji potencjometrow
	else if(last_read_time > ANALOG_STAB_DELAY && reading_step == 1)
	{
		while(reading_step == 1)
		{
			delayMicroseconds(5);

			readButtons();

			reading_channel++;
			if(reading_channel < ANALOG_MAX_MUX_CHANNELS)
			{
				setMux(analog_mux_channels_order[reading_channel]);
				reading_step = 1;
				last_read_time = 0;
			}
			else
			{
				setMux(analog_mux_channels_order[0]);
				reading_channel = 0;
				reading_step = 2;
				last_read_time = 0;
			}
		}

	}
	//--------------------------------------------------------------------------------------
	//przetworzenie danych porownanie odczytow z poprzednimi i wykonanie akcji
	else if(reading_step == 2)
	{

		#if ANALOG_BUTTONS_ON
		processButtonData();
		#endif

		reading_step = 0;
		last_read_time = 0;
	}
}

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::testMode(uint8_t set)
{
	if(set == 1) test_mode = 1;
	else test_mode = 0;
}

//-----------------------------------------------------------------------------------------------
void cAnalogInputs::setMux(uint8_t channel)
{
	if(channel & 1)	MUX_CTRL_A_GPIO_SET = (1 << MUX_CTRL_A);
	else			MUX_CTRL_A_GPIO_CLR = (1 << MUX_CTRL_A);

	if(channel & 2)	MUX_CTRL_B_GPIO_SET = (1 << MUX_CTRL_B);
	else			MUX_CTRL_B_GPIO_CLR = (1 << MUX_CTRL_B);

	if(channel & 4)	MUX_CTRL_C_GPIO_SET = (1 << MUX_CTRL_C);
	else			MUX_CTRL_C_GPIO_CLR = (1 << MUX_CTRL_C);

	if(channel & 8)	MUX_CTRL_D_GPIO_SET = (1 << MUX_CTRL_D);
	else			MUX_CTRL_D_GPIO_CLR = (1 << MUX_CTRL_D);
}
