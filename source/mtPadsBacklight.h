#ifndef _MT_PADS_BACKLIGHT_H
#define _MT_PADS_BACKLIGHT_H
#include "mtLED.h"
#include "Arduino.h"







class cMtPadsBacklight
{
public:
	void init(uint16_t blink);
	void update();
	void startBlink(uint8_t gamma_pwm, uint8_t n);
	void stopBlink(uint8_t gamma_pwm, uint8_t n);
	void setFrontLayer(uint8_t state, uint8_t gamma_pwm, uint8_t n );
	void setBackLayer(uint8_t state, uint8_t gamma_pwm, uint8_t n );


private:
	void clearAllPads();
	elapsedMillis blinkTimer;
	uint8_t backLayer[64];
	uint8_t blinkLayer[64];
	uint16_t blinkConstrain;
	uint8_t blinkCounter;
	uint8_t toggler;
};











extern cMtPadsBacklight padsBacklight;


#endif
