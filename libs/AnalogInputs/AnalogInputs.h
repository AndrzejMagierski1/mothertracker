#ifndef _ANALOGINPUTS_H_
#define _ANALOGINPUTS_H_

#include <stdint.h>
#include <elapsedMillis.h>

#include "mtHardware.h"

// MODULES OPTIONS

#define ANALOG_POTS_ON 		1
#define ANALOG_PADS_ON 		1
#define ANALOG_BUTTONS_ON 	1



//performance settings (time in us)
#define ANALOG_READING_INTERVAL 1000
#define ANALOG_STAB_DELAY 50
#define ANALOG_ADC_RESOLUTION 12

#define ANALOG_BUTTON_FORCE_DEAD_ZONE 2
#define ANALOG_BUTTON_FORCE_START_DEAD_ZONE 11
#define ANALOG_BUTTON_FORCE_OFFSET 66

#define ANALOG_BUTTON_POSITION_DEAD_ZONE_START 22
#define ANALOG_BUTTON_POSITION_DEAD_ZONE_NORMAL 2

#define ANALOG_POT_DEAD_ZONE 5
#define ANALOG_POT_REFRESH_TIME 20

//buttons settings
#define ANALOG_BUTTON_HOLD_TIME 800
#define ANALOG_BUTTON_DEBOUNCE  10

// counts settings
#define ANALOG_MAX_PAD_BUTTONS 	64
#define ANALOG_MAX_POTS 		5
//TODO:
#define ANALOG_MAX_BUTTONS 		33



// adc pins in - buttons/pot
#define ANALOG_ADC_MUX_0 MUX_OUT_0
#define ANALOG_ADC_MUX_1 MUX_OUT_1


/*
#define ANALOG_MUX_CTRL_A MUX_CTRL_A
#define ANALOG_MUX_CTRL_B MUX_CTRL_B
#define ANALOG_MUX_CTRL_C MUX_CTRL_C
#define ANALOG_MUX_CTRL_D MUX_CTRL_D
*/

// mux settings
#define ANALOG_MAX_PAD_MUXS 8  		//////////////////////////////// 8
#define ANALOG_MAX_MUX_CHANNELS 16  //////////////////////////////// 16
#define ANALOG_MAX_BUTT_MUXS 2

//#define ANALOG_ALL_MUXS (ANALOG_MAX_PAD_MUXS + ANALOG_MAX_POT_MUXS)
//#define ANALOG_ALL_MUXS_SINGALS (ANALOG_MAX_MUX_CHANNELS*ANALOG_ALL_MUXS)


// buttons definitions 0-32 (mux 8-10)
#define ANALOG_BUTTON1 		0

#define ANALOG_BUTTON2 		1
#define ANALOG_BUTTON3		2
#define ANALOG_BUTTON4 		3
#define ANALOG_BUTTON5 		4
#define ANALOG_BUTTON6 		5
#define ANALOG_BUTTON7 		6
#define ANALOG_BUTTON8		7

#define ANALOG_BUTTON9 		8//0 mux
#define ANALOG_BUTTON10 	9//0


#define ANALOG_BUTTON11 	10  //0
#define ANALOG_BUTTON12 	11  //0
#define ANALOG_BUTTON13		12  //0
#define ANALOG_BUTTON14 	13 //0
#define ANALOG_BUTTON15 	14 //0
#define ANALOG_BUTTON16 	15  //0
#define ANALOG_BUTTON17 	16  //0
#define ANALOG_BUTTON18		17  //0
#define ANALOG_BUTTON19 	18  //0
#define ANALOG_BUTTON20 	19  //0


#define ANALOG_BUTTON21 	20//0
#define ANALOG_BUTTON22 	21//0
#define ANALOG_BUTTON23		22//0
#define ANALOG_BUTTON24 	23
#define ANALOG_BUTTON25 	24
#define ANALOG_BUTTON26 	25
#define ANALOG_BUTTON27 	26
#define ANALOG_BUTTON28		27//0
#define ANALOG_BUTTON29 	28
#define ANALOG_BUTTON30 	29


#define ANALOG_BUTTON31 	30
#define ANALOG_BUTTON32 	31
#define ANALOG_BUTTON33		32




#define ANALOG_BUTTON_TACT 		15

// ----------MUX channels to PADS order------
const uint8_t analog_mux_channels_order[ANALOG_MAX_MUX_CHANNELS] =
{6,7,4,5,2,3,0,1,15,14,13,12,11,10,9,8};  //medusa/grid



const uint8_t analog_pots_muxs_signal[ANALOG_MAX_BUTT_MUXS] =
{
	ANALOG_ADC_MUX_0,
	ANALOG_ADC_MUX_1
};

const uint8_t analog_buttons_mux_channels0[ANALOG_MAX_MUX_CHANNELS] =
{ // zgodnie z analog_mux_channels_order[]
	ANALOG_BUTTON10,      //6
	ANALOG_BUTTON9,      //7
	ANALOG_BUTTON12,      //4
	ANALOG_BUTTON11, 	 //5
	ANALOG_BUTTON18,      //2
	ANALOG_BUTTON13,      //3
	ANALOG_BUTTON16,       //0
	ANALOG_BUTTON17,       //1
	ANALOG_BUTTON28,       //15
	ANALOG_BUTTON15,       //14
	ANALOG_BUTTON14,       //13
	ANALOG_BUTTON23,       //12
	ANALOG_BUTTON22,       //11
	ANALOG_BUTTON21,       //10
	ANALOG_BUTTON20,       //9
	ANALOG_BUTTON19,       //8
};

const uint8_t analog_buttons_mux_channels1[ANALOG_MAX_MUX_CHANNELS] =
{ // zgodnie z analog_mux_channels_order[]
	ANALOG_BUTTON32,      //6
	ANALOG_BUTTON33,      //7
	ANALOG_BUTTON30,      //4
	ANALOG_BUTTON31,  	  //5
	ANALOG_BUTTON26,      //2
	ANALOG_BUTTON27,      //3
	ANALOG_BUTTON24,       //0
	ANALOG_BUTTON25,       //1
	ANALOG_BUTTON29,       //15
	ANALOG_BUTTON8,       //14
	ANALOG_BUTTON7,       //13
	ANALOG_BUTTON6,       //12
	ANALOG_BUTTON2,       //11
	ANALOG_BUTTON3,       //10
	ANALOG_BUTTON4,       //9
	ANALOG_BUTTON5,       //8
};



class cAnalogInputs
{
public:
	void setButtonChangeFunc (void (*func)(uint8_t, uint8_t));

	uint8_t isButtonPressed(uint8_t button);


	void begin(uint16_t pot_res);
	void update();

	void testMode(uint8_t set);


private:


	elapsedMicros last_read_time;
	uint8_t reading_step;
	uint8_t reading_channel;

	uint8_t test_mode = 0;

	uint8_t start_up = 1;




	uint16_t new_button_values[ANALOG_MAX_BUTTONS];


	struct strButtons
	{
		 uint8_t state;
		 int8_t debounce;
		 elapsedMillis hold_time;
		 elapsedMillis double_time;

	} buttons[ANALOG_MAX_BUTTONS];


	void (*PadPressFunc)(uint8_t, int8_t, int8_t, uint8_t);
	void (*PadChangeFunc)(uint8_t, int8_t, int8_t, uint8_t);
	void (*PadReleaseFunc)(uint8_t);
	void (*PotChangeFunc)(uint8_t, int16_t);
	void (*ButtonChangeFunc)(uint8_t, uint8_t);

	void setMux(uint8_t channel);

	void readButtons();

	void processButtonData();



};

extern cAnalogInputs AnalogInputs;




#endif
