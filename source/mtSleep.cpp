#include "mtSleep.h"
#include "mtLED.h"
#include "sdram.h"
#include <Audio.h>
#include "FT812.h"
#include "keyScanner.h"
#include "Si4703.h"
#include "mtConfig.h"


mtSleep lowPower;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);

extern AudioControlSGTL5000 audioShield;
SnoozeDigital digital;
SnoozeBlock config(digital);

void mtSleep::requestShutdown(uint8_t value)
{
	if(value == 1)
	{
		shutdown_requested = 1;
	}
	else
	{
		shutdown_requested = 0;
	}

	lastValue = value;
}

void mtSleep::goLowPower()
{
	saveStartState();

	noInterrupts();
	disableAll();
}

uint8_t mtSleep::getShutdownRequest()
{
	return shutdown_requested;
}

void mtSleep::wakeUp(uint8_t value)
{
	if(value && lastValue == 0)
	{
		resetMCU();
	}

	lastValue = value;
}

void mtSleep::handlePowerState(uint8_t value)
{
	if(powerState == powerTypeLow)
	{
		wakeUp(value);
	}
	else if(powerState == powerTypeNormal)
	{
		requestShutdown(value);
	}
}


uint8_t mtSleep::isLowPower()
{
	return powerState == powerTypeLow;
}

void mtSleep::disableAll()
{
	//wylaczanie usb ze windows nie krzyczal
    USB0_INTEN = 0;
    USB0_CTL = 0;

    digitalWrite(BLINK_LED,HIGH);
	audioShield.write(0x30, 0);
	radio.powerOff();
	Keypad.disableInterrupt();
	tactButtons.disableInterrupt();
	leds.setAllLEDPWM(leds.ledPWM, 0);
	MCU_set_sleepMode();
	digitalWrite(EXTERNAL_RAM_KEY,LOW);//RAM power off
	BOARD_DeinitPins();// RAM pins deinit

	Snooze.sleep(config);
	powerState = powerTypeLow;
}

void mtSleep::resetMCU()
{
	__DSB();
	CM4_SCB_AIRCR = (uint32_t)((0x5FAUL << CM4_SCB_AIRCR_VECTKEY_POS) | CM4_SCB_AIRCR_SYSRESETREQ_MASK);
	__DSB();

	for(;;)
	{
		__NOP();
	}
}
