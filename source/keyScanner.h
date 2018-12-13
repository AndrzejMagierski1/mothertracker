

#ifndef _KEYSCANNER_H_
#define _KEYSCANNER_H_


#include <stdint.h>
#include <elapsedMillis.h>

// BUTTNOS DEFINITIONS

#define BUTTON_GRID			49	//SW15 (LED)
#define BUTTON_VOICE		57	//SW16 (LED)
#define BUTTON_RANDOM		20	//SW35
#define BUTTON_BACK			28  //SW36
#define BUTTON_LOAD 		43	//SW30
#define BUTTON_SAVE 		35	//SW29
#define BUTTON_CLEAR 		27	//SW28
#define BUTTON_PLAY 		42	//SW22 (LED)
#define BUTTON_REC 			34	//SW21 (LED)
#define BUTTON_HOLD 		26	//SW20 (LED)

#define BUTTON_WAVE_1 		59	//SW32
#define BUTTON_OCTAVE_1 	3	//SW25
#define BUTTON_WAVE_2 		11	//SW26
#define BUTTON_OCTAVE_2 	19	//SW27
#define BUTTON_WAVE_3 		51	//SW31
#define BUTTON_OCTAVE_3  	4	//SW33
#define BUTTON_DIGITAL		5	//SW37 (LED)
#define BUTTON_OSC 			18	//SW19 (LED)
#define BUTTON_OSC_FILTER	50	//SW23 (LED)
#define BUTTON_SYNC2 		58	//SW24 (LED)
#define BUTTON_SYNC3		2	//SW17 (LED)
#define BUTTON_FINETUNE		10	//SW18 (LED)

#define BUTTON_PLAY_MODE 	13	//SW38

#define BUTTON_TYPE			12	//SW34

#define BUTTON_ENV1			41	//SW14 (LED)
#define BUTTON_ENV2			9	//SW10 (LED)
#define BUTTON_ENV3			1	//SW9 (LED)
#define BUTTON_ENV4			40	//SW6 (LED)
#define BUTTON_ENV5			8	//SW2 (LED)
#define BUTTON_ENV6			0	//SW1 (LED)

#define BUTTON_LFO1			25	//SW12 (LED)
#define BUTTON_LFO2			17	//SW11 (LED)
#define BUTTON_LFO3			33	//SW13 (LED)
#define BUTTON_LFO4			24	//SW4 (LED)
#define BUTTON_LFO5			16	//SW3 (LED)
#define BUTTON_LFO6			32	//SW5 (LED)

#define IO7326_ADDR1 0b1011000
#define IO7326_ADDR2 0b1011001
#define IO7326_ADDR3 0b1011010
#define IO7326_ADDR4 0b1011011

//CONFIGURATION REGISTER SETTINGS
#define IS31_ACIa   0   // auto clear int
#define IS31_ACIb   0
#define IS31_DE   1  	// input port filter enable
#define IS31_SD   0 	// key scan debounce time
#define IS31_LE   1   	// long press key detect enable 
#define IS31_LTa  0
#define IS31_LTb  0

#define IS31_CONF_REGISTER 0x08
#define IS31_KEY_STATUS_REGISTER 0x10

#define BUTTONS_MAX_COUNT 64


///
//#define KEYSCANNER_I2C_DMA
///

//------------------------------------------------------
class keyScanner
{
public:
	keyScanner();
	keyScanner(uint8_t address, uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin);

	void begin(uint8_t address, uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, void (*func)(void));
	//void begin(void (*func)(void));
	uint8_t update();
	void intAction();

	void setButtonPushFunc(void (*func)(uint8_t));
	void setButtonReleaseFunc(void (*func)(uint8_t));
	void setButtonHoldFunc(void (*func)(uint8_t));
	void setButtonDoubleFunc(void (*func)(uint8_t));
	void setHoldTime(uint16_t time);
	void setDoubleTime(uint16_t time);
	void testMode(uint8_t set);

	uint8_t buttonToX(uint8_t number);
	uint8_t buttonToY(uint8_t number);
	bool isButtonPressed(uint8_t number);
	bool isButtonHold(uint8_t number);
	uint8_t getButtonState(uint8_t number);
	uint8_t xy2key(uint8_t x, uint8_t y);

private:
	void (*onPush)(uint8_t);
	void (*onRelease)(uint8_t);
	void (*onHold)(uint8_t);
	void (*onDouble)(uint8_t);

	void read_buttons_IC(uint8_t grid_no);
	void handle_howManyPressed();
	void handle_buttonTimeout();
	void action_button_press(uint8_t num);
	void action_button_hold(uint8_t num);
	void action_button_release(uint8_t num);
	void action_button_double(uint8_t num);


	uint8_t test_mode = 0;
	uint8_t IO7326_int_pin;
	uint8_t IO7326_sda_pin;
	uint8_t IO7326_scl_pin;
	uint8_t ucAddr;

	uint8_t button[BUTTONS_MAX_COUNT];
	uint8_t button_double[BUTTONS_MAX_COUNT];
	uint16_t button_hold[BUTTONS_MAX_COUNT];
	uint8_t button_still_pressing[BUTTONS_MAX_COUNT];
	uint16_t button_double_time[BUTTONS_MAX_COUNT];

	uint16_t button_hold_max = 200;
//	const uint16_t button_timeout_max = 1000;

	//pomiar czasu pomiedzy wcisnieciami
	elapsedMillis doublePushTime = 0; 
	uint16_t button_double_time_max = 100;

	elapsedMillis intProtect;
	const uint32_t intProtect_max = 6;

	// struct
	uint8_t IO7326_int = 0;

	elapsedMillis timeoutTrigger = 0; // trigger do odmierzania timeoutu wcisnietego przycisku
	const uint32_t timeoutTrigger_max = 10;

	int8_t buttonsPressed = 0;

	elapsedMillis checkIntPins = 0;
	uint32_t checkIntPinsMax = 500;
	// struct end
};

#endif