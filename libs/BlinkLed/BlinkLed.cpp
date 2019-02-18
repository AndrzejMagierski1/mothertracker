
#include "core_pins.h"

#include "BlinkLed.h"


cBlinkLed BlinkLed;



void cBlinkLed::begin(uint8_t pin)
{
	_pin = pin;
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, HIGH);
	pinState = 0;
}

void cBlinkLed::update()
{
	if(setCount > 0 && timer > setTime)
	{
		if(pinState)
		{
			digitalWrite(_pin, HIGH);
			setCount--;
			pinState = 0;
		}
		else
		{
			digitalWrite(_pin, LOW);
			pinState = 1;
		}

		timer = 0;
	}
}


void cBlinkLed::blinkOnce()
{
	digitalWrite(_pin, LOW);
	setTime = BLINK_ONCE_TIME;
	setCount = 1;
	timer = 0;
}

void cBlinkLed::blinkXTimes(uint8_t x)
{
	if(x == 0) return;

	digitalWrite(_pin, LOW);
	setTime = BLINK_ONCE_TIME;
	setCount = x;
	timer = 0;
}
