#include "mtSleep.h"
#include "mtDisplay.h"
#include "mtLED.h"
#include "sdram.h"
#include <Audio.h>
extern AudioControlSGTL5000 audioShield;
SnoozeDigital digital;
SnoozeBlock config(digital);

uint8_t powerChanged;
uint8_t powerState = powerTypeNormal;

void goLowPower()
{
	disableAll();
	Snooze.sleep(config);
	powerState=powerTypeLow;
	powerChanged=1;
}

void wakeUp()
{
	Snooze.wakeUp(config);
	powerState=powerTypeNormal;
	powerChanged=1;
}

void changePowerState()
{
	if(powerState == powerTypeLow) 					wakeUp();
	else if (powerState == powerTypeNormal)			goLowPower();
}

void disableAll()
{
	audioShield.write(0x30, 0);
	leds.setSleep();
	mtDisplay.setSleepMode();
	SDRAM_setSleepMode();
//todo: sdram...

}

void powerModeUpdate()
{
	if(powerChanged)
	{
		powerChanged=0;
		if(powerState == powerTypeLow) 				disableAll();
		else if(powerState == powerTypeNormal) 		setup(); //todo: lepszy bedzie software'owy reset
	}
}

