#include "mtSleep.h"
#include "mtLED.h"
#include "sdram.h"
#include <Audio.h>
#include "FT812.h"
#include "keyScanner.h"
#include "Si4703.h"

extern AudioControlSGTL5000 audioShield;
SnoozeDigital digital;
SnoozeBlock config(digital);

uint8_t powerChanged;
uint8_t powerState = powerTypeNormal;

static void resetMCU();

void goLowPower()
{
	disableAll();
	Snooze.sleep(config);
	powerState=powerTypeLow;
	powerChanged=1;
	noInterrupts();
}

void wakeUp()
{
	interrupts();
	Snooze.wakeUp(config);
	powerState=powerTypeNormal;
	powerChanged=1;
}

void changePowerState()
{
	if(powerState == powerTypeLow) 					wakeUp();
	else if (powerState == powerTypeNormal)			goLowPower();
}


uint8_t isLowPower()
{
	uint8_t status=0;

	if(powerState ==  powerTypeLow)
	{
		status = 1;
	}

	return status;
}

void disableAll()
{
	//wylaczanie usb ze windows nie krzyczal
    USB0_INTEN = 0;
    USB0_CTL = 0;

	audioShield.write(0x30, 0);
	radio.powerOff();
	Keypad.disableInterrupt();
	tactButtons.disableInterrupt();
	leds.setAllLEDPWM(leds.ledPWM, 0);
	MCU_set_sleepMode();
	digitalWrite(EXTERNAL_RAM_KEY,LOW);//RAM power off
	BOARD_DeinitPins();// RAM pins deinit
}

void powerModeUpdate()
{
	if(powerChanged)
	{
		powerChanged=0;
		if(powerState == powerTypeLow) 				disableAll();
		else if(powerState == powerTypeNormal)
		{
			resetMCU();
		}
	}
}

static void resetMCU()
{
	__DSB();
	CM4_SCB_AIRCR = (uint32_t)((0x5FAUL << CM4_SCB_AIRCR_VECTKEY_POS) | CM4_SCB_AIRCR_SYSRESETREQ_MASK);
	__DSB();

	for(;;)
	{
		__NOP();
	}
}

