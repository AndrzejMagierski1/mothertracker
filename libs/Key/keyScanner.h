

#ifndef _KEYSCANNER_H_
#define _KEYSCANNER_H_


#include <stdint.h>
#include <elapsedMillis.h>
#include "mtHardware.h"
#include "mtKeyMapping.h"

// BUTTNOS DEFINITIONS

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

	void begin(uint8_t address, uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin,const uint8_t *keyMapping, void (*func)(void));

	//void begin(void (*func)(void));
	uint8_t update();
	void intAction();

	uint8_t setButtonPushFunc(void (*func)(uint8_t,uint8_t));
	uint8_t setButtonReleaseFunc(void (*func)(uint8_t,uint8_t));
	uint8_t setButtonHoldFunc(void (*func)(uint8_t,uint8_t));
	uint8_t setButtonDoubleFunc(void (*func)(uint8_t,uint8_t));
	void setHoldTime(uint16_t time);
	void setDoubleTime(uint16_t time);
	void testMode(uint8_t set);

/*	uint8_t buttonToX(uint8_t number);
	uint8_t buttonToY(uint8_t number);
	bool (uint8_t number);
	bool isButtonHold(uint8_t number);
	uint8_t getButtonState(uint8_t number);
	uint8_t xy2key(uint8_t x, uint8_t y);*/

private:
	void (*onPush)(uint8_t,uint8_t);
	void (*onRelease)(uint8_t,uint8_t);
	void (*onHold)(uint8_t,uint8_t);
	void (*onDouble)(uint8_t,uint8_t);

	void read_buttons_IC(uint8_t grid_no);
//	void handle_howManyPressed();
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

	uint8_t *keyMap;

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

	elapsedMillis holdTimer;
	uint16_t speedFactors[5] ={300,250,200,150,100};
	uint8_t holdMultiplier;
	uint32_t holdTimestamp[BUTTONS_MAX_COUNT];

	// struct end

	enum status
	{
		statusError=0,
		statusSuccess
	};
};



extern keyScanner Buttons;


#endif
