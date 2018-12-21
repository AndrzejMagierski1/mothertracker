#ifndef _ANALOGINPUTS_H_
#define _ANALOGINPUTS_H_

#include <stdint.h>
#include <elapsedMillis.h>

#include "mtHardware.h"

// MODULES OPTIONS

#define ANALOG_POTS_ON 		1
#define ANALOG_PADS_ON 		0
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

#define ANALOG_POT_DEAD_ZONE 10

//buttons settings
#define ANALOG_BUTTON_HOLD_TIME 800
#define ANALOG_BUTTON_DEBOUNCE  10

// counts settings
#define ANALOG_MAX_PAD_BUTTONS 	64
#define ANALOG_MAX_POTS 		5
//TODO:
#define ANALOG_MAX_BUTTONS 		6

// adc pins in - pads
#define ANALOG_ADC_MUX_0 MUX_PAD_OUT_0
#define ANALOG_ADC_MUX_1 MUX_PAD_OUT_1
#define ANALOG_ADC_MUX_2 MUX_PAD_OUT_2
#define ANALOG_ADC_MUX_3 MUX_PAD_OUT_3
#define ANALOG_ADC_MUX_4 MUX_PAD_OUT_4
#define ANALOG_ADC_MUX_5 MUX_PAD_OUT_5
#define ANALOG_ADC_MUX_6 MUX_PAD_OUT_6
#define ANALOG_ADC_MUX_7 MUX_PAD_OUT_7
// adc pins in - buttons/pot
#define ANALOG_ADC_MUX_8 MUX_OUT_8


// pins out - mux control
#define ANALOG_CTRL_XY_Z CTRL_XY_Z
/*
#define ANALOG_MUX_CTRL_A MUX_CTRL_A
#define ANALOG_MUX_CTRL_B MUX_CTRL_B
#define ANALOG_MUX_CTRL_C MUX_CTRL_C
#define ANALOG_MUX_CTRL_D MUX_CTRL_D
*/
// mux settings
#define ANALOG_MAX_PAD_MUXS 8  		//////////////////////////////// 8
#define ANALOG_MAX_MUX_CHANNELS 16  //////////////////////////////// 16
#define ANALOG_MAX_POT_MUXS 1

//#define ANALOG_ALL_MUXS (ANALOG_MAX_PAD_MUXS + ANALOG_MAX_POT_MUXS)
//#define ANALOG_ALL_MUXS_SINGALS (ANALOG_MAX_MUX_CHANNELS*ANALOG_ALL_MUXS)

// pots definitions 0-32 (mux 8-10)
#define ANALOG_POT1_A 		0
#define ANALOG_POT1_B 		1
#define ANALOG_POT2_A 		2
#define ANALOG_POT2_B		3
#define ANALOG_POT3_A 		4
#define ANALOG_POT3_B 		5
#define ANALOG_POT4_A 		6
#define ANALOG_POT4_B 		7
#define ANALOG_POT5_A 		8
#define ANALOG_POT5_B 		9

// buttons definitions 0-32 (mux 8-10)
#define ANALOG_BUTTON1 		10
#define ANALOG_BUTTON2 		11
#define ANALOG_BUTTON3		12
#define ANALOG_BUTTON4 		13
#define ANALOG_BUTTON5 		14

#define ANALOG_BUTTON_TACT 		15

// ----------MUX channels to PADS order------
const uint8_t analog_mux_channels_order[ANALOG_MAX_MUX_CHANNELS] =
{6,7,4,5,2,3,0,1, 15,14,13,12,11,10,9,8};  //medusa/grid

const uint8_t analog_pads_muxs_order[ANALOG_MAX_PAD_MUXS] =
{
	ANALOG_ADC_MUX_0, ANALOG_ADC_MUX_1, ANALOG_ADC_MUX_2, ANALOG_ADC_MUX_3,
	ANALOG_ADC_MUX_4, ANALOG_ADC_MUX_5, ANALOG_ADC_MUX_6, ANALOG_ADC_MUX_7
};

const uint8_t analog_pots_muxs_order[ANALOG_MAX_POT_MUXS] =
{
	ANALOG_ADC_MUX_8
};

const uint8_t analog_pots_buttons_mux_channels[ANALOG_MAX_MUX_CHANNELS] =
{
	ANALOG_BUTTON2,
	ANALOG_BUTTON1,
	ANALOG_BUTTON3,
	ANALOG_BUTTON_TACT,
	ANALOG_BUTTON5,
	ANALOG_BUTTON4,
	ANALOG_POT5_A,
	ANALOG_POT5_B,
	ANALOG_POT4_B,
	ANALOG_POT4_A,
	ANALOG_POT3_B,
	ANALOG_POT3_A,
	ANALOG_POT2_B,
	ANALOG_POT2_A,
	ANALOG_POT1_B,
	ANALOG_POT1_A,
};

const uint8_t analog_buttons_index[ANALOG_MAX_BUTTONS] =
{
	ANALOG_BUTTON1, ANALOG_BUTTON2, ANALOG_BUTTON3,
	ANALOG_BUTTON4, ANALOG_BUTTON5,


	//TODO:
	ANALOG_BUTTON_TACT

};

const uint8_t analog_pots_index_A[ANALOG_MAX_BUTTONS] =
{
	ANALOG_POT1_A, ANALOG_POT2_A,
	ANALOG_POT3_A, ANALOG_POT4_A,
	ANALOG_POT5_A
};

const uint8_t analog_pots_index_B[ANALOG_MAX_BUTTONS] =
{
	ANALOG_POT1_B, ANALOG_POT2_B,
	ANALOG_POT3_B, ANALOG_POT4_B,
	ANALOG_POT5_B
};

class cAnalogInputs
{
public:
	void setPadPressFunc     (void (*func)(uint8_t, int8_t, int8_t, uint8_t));
	void setPadChangeFunc    (void (*func)(uint8_t, int8_t, int8_t, uint8_t));
	void setPadReleaseFunc   (void (*func)(uint8_t));
	void setPotChangeFunc    (void (*func)(uint8_t, int16_t));
	void setButtonChangeFunc (void (*func)(uint8_t, uint8_t));


	void setPotDeathZone(uint16_t value);
	void setPotResolution(uint16_t n, uint16_t value);
	uint8_t howManyPadPressed();
	uint8_t isPadPressed(uint8_t pad);
	uint8_t isButtonPressed(uint8_t button);


	void begin();
	void update();

	void testMode(uint8_t set);
	void setPadxMode(uint8_t mode);
	void setPadyMode(uint8_t mode);


private:


	elapsedMicros last_read_time;
	uint8_t reading_step;
	uint8_t reading_channel;

	uint8_t test_mode = 0;

	uint8_t start_up = 1;

	uint8_t pads_mode_x = 1;
	uint8_t pads_mode_y = 0;

	// 0 = absolturtne
	// 1 = relatywne

	struct strPadButtons
	{
		int8_t pad_buttons_x;
		int8_t pad_buttons_y;
		int8_t pad_buttons_start_x;
		int8_t pad_buttons_start_y;
		uint8_t pad_buttons_dzone_x;
		uint8_t pad_buttons_dzone_y;

		uint8_t pad_buttons_fx;
		uint8_t pad_buttons_fy;
		uint8_t pad_buttons_count_fx;
		uint8_t pad_buttons_count_fy;
		uint8_t start_point_fx;
		uint8_t start_point_fy;

		uint8_t pad_buttons_state_x;
		uint8_t pad_buttons_state_y;
	};

	strPadButtons padButtons[ANALOG_MAX_PAD_BUTTONS];

	uint16_t new_pot_button_values[ANALOG_MAX_MUX_CHANNELS];


	struct strpotentiometers
	{
		int16_t positions[2];
		uint8_t part;
		uint8_t last_part;

		int16_t diffrences[4];

		uint16_t resolution = 1023;
		int16_t  global_diff = 0;

	} potentiometers[ANALOG_MAX_POTS];
	uint16_t pots_death_zone = ANALOG_POT_DEAD_ZONE;



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
	void readPadPressPosition();
	void readPadPressForce();
	void readPotButtons();
	void processPadData();
	void processPotData();
	void processButtonData();
	uint16_t calculatePotPosition(uint16_t A, uint16_t B, uint8_t * part);
	uint16_t calculateForce(uint8_t pad);
	void calculateXPos();
	void calculateYPos();

};

extern cAnalogInputs AnalogInputs;

//=============pad_button_state==========
//
// 0 - pad nie jest wcisniety
// 1 - pad jest wcisniety, brak zmian
// 2 - pad zostal wcisniety *
// 3 - pad zostal puszczony *
// 4 - zmiana wcisniecia pada*
// 5 - zmiana pozycji po stanie 0
// 6 - zmiana pozycji po stanie 1
//
// * - stany wywołujące procedury akcji

const uint16_t force_ratio[375] =
{
0,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
4,
4,
4,
4,
4,
4,
4,
4,
4,
4,
4,
4,
4,
4,
5,
5,
5,
5,
5,
5,
5,
5,
5,
5,
5,
5,
6,
6,
6,
6,
6,
6,
6,
6,
6,
6,
7,
7,
7,
7,
7,
7,
7,
7,
7,
8,
8,
8,
8,
8,
8,
8,
9,
9,
9,
9,
9,
9,
9,
10,
10,
10,
10,
10,
10,
11,
11,
11,
11,
11,
12,
12,
12,
12,
12,
13,
13,
13,
13,
14,
14,
14,
14,
14,
15,
15,
15,
15,
16,
16,
16,
17,
17,
17,
17,
18,
18,
18,
19,
19,
19,
20,
20,
20,
21,
21,
21,
22,
22,
22,
23,
23,
24,
24,
24,
25,
25,
26,
26,
27,
27,
27,
28,
28,
29,
29,
30,
30,
31,
31,
32,
33,
33,
34,
34,
35,
35,
36,
37,
37,
38,
39,
39,
40,
41,
41,
42,
43,
43,
44,
45,
46,
46,
47,
48,
49,
50,
51,
51,
52,
53,
54,
55,
56,
57,
58,
59,
60,
61,
62,
63,
64,
65,
66,
67,
68,
70,
71,
72,
73,
75,
76,
77,
78,
80,
81,
82,
84,
85,
87,
88,
90,
91,
93,
94,
96,
98,
99,
101,
103,
105,
106,
108,
110,
112,
114,
116,
118,
120,
122,
124,
126,
128,
130,
132,
135,
137,
139,
142,
144,
147,
149,
152,
154,
157,
159,
162,
165,
168,
171,
174,
177,
180,
183,
186,
189,
192,
195,
199,
202,
206,
209,
213,
216,
220,
224,
227,
231,
235,
239,
243,
248,
252,
256,
260,
265,
269,
274,
279,
283,
288,
293,
298,
303,
308,
314,
319,
324,
330,
336,
341,
347,
353,
359,
365,
371,
378,
384,
391,
397,
404,
411,
418,
425,
432,
440,
447,
455,
463,
471,
479,
487,
495,
503,
512,
521,
530,
539,
548,
557,
567,
};

#endif
