
#ifndef _BLINK_LED_H
#define _BLINK_LED_H

#include <elapsedMillis.h>

#define BLINK_ONCE_TIME 500



class cBlinkLed
{
public:
	void begin(uint8_t pin);
	void update();
	void blinkOnce();
	void blinkXTimes(uint8_t x);

private:
	elapsedMillis timer;
	uint8_t _pin;
	uint8_t pinState;
	uint16_t setTime;
	uint8_t setCount;
};


extern cBlinkLed BlinkLed;

#endif
