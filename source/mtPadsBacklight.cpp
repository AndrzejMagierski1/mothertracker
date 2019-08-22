

#include "mtLED.h"

#include "mtPadsBacklight.h"

cMtPadsBacklight padsBacklight;

void cMtPadsBacklight::init(uint16_t blink)
{
	blinkConstrain = blink;
	blinkTimer=0;
}
void cMtPadsBacklight:: update()
{
	if (blinkCounter)
	{
		if(blinkTimer > blinkConstrain)
		{
			blinkTimer = 0;

			if(toggler == 1) toggler = 0;
			else if(toggler == 0) toggler =1;

			for(uint8_t i=0; i < 48 ; i ++)
			{
				if(blinkLayer[i]) setFrontLayer(toggler, blinkLayer[i], i );
			}
		}
	}

}

void cMtPadsBacklight::setBackLayer(uint8_t state, uint8_t gamma_pwm, uint8_t n)
{
	if(state)
	{
		leds.setLED(n,state,gamma_pwm);
		backLayer[n]=gamma_pwm;
	}
	else
	{
		leds.setLED(n,state,gamma_pwm);
		backLayer[n]=0;
	}
}

void cMtPadsBacklight::setFrontLayer(uint8_t state, uint8_t gamma_pwm, uint8_t n)
{
	if(state)
	{
		leds.setLED(n,state,gamma_pwm);
	}
	else
	{
		if(backLayer[n] != 0) leds.setLED(n,1,backLayer[n]);
		else leds.setLED(n,state,gamma_pwm);
	}
}

void cMtPadsBacklight::startBlink(uint8_t gamma_pwm, uint8_t n)
{
	blinkLayer[n] = gamma_pwm;
	blinkCounter++;
}

void cMtPadsBacklight::stopBlink(uint8_t n)
{
	blinkLayer[n] = 0;
	setFrontLayer(0, blinkLayer[n], n );
	blinkCounter--;
}

void cMtPadsBacklight::clearAllPads(uint8_t front, uint8_t back, uint8_t blink)
{
	for(uint8_t i=0;i<48;i++)
	{
		if(front) setFrontLayer(0,0,i);
		if(back) setBackLayer(0,0,i);
		if(blink) stopBlink(i);
	}
}
