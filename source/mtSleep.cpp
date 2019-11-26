#include "mtSleep.h"
#include "mtLED.h"
#include "sdram.h"
#include <Audio.h>
#include "FT812.h"
#include "keyScanner.h"
#include "Si4703.h"

mtSleep lowPower;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);

extern AudioControlSGTL5000 audioShield;
SnoozeDigital digital;
SnoozeBlock config(digital);

void mtSleep::goLowPower(uint8_t value)
{
	if(value == 1) // pressed
	{
		if(firstPress == 0)
		{
			firstPressTimestamp = shutdownTimer;
			initDisplayCountDown();
			firstPress = 1;
		}

		int32_t timeToShutdown_ms = 0;

		timeToShutdown_ms = (TURN_OFF_TIME_MS - (shutdownTimer - firstPressTimestamp));

		refreshDisplayCountDown(timeToShutdown_ms);

		if(timeToShutdown_ms <= 0)
		{
			noInterrupts();
			disableAll();
			Snooze.sleep(config);
			powerState=powerTypeLow;
		}
	}
	else
	{
		if(firstPress)
		{
			firstPress = 0;
			deinitDisplayCountDown();
		}
	}

	lastValue = value;
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
		goLowPower(value);
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

void mtSleep::initDisplayCountDown()
{
	interfaceEnvents(eventToggleActiveModule,0,0,0);
	strControlProperties prop;

	prop.x = 190;
	prop.y = 170;
	prop.style = controlStyleValue_0_100;
	prop.h = 100;
	prop.w = 420;

	if(turnOffProgressBar == nullptr)  turnOffProgressBar = display.createControl<cHorizontalBar>(&prop);
}

void mtSleep::refreshDisplayCountDown(uint16_t timeLeft_ms)
{
	uint32_t progress = 0;
	uint8_t timeLeft_s = 0;
	progress = ((timeLeft_ms * 100)/TURN_OFF_TIME_MS);

	timeLeft_s = ceil((timeLeft_ms/1000.0f));

	sprintf(turnOffText, "Shutdown in %ds", timeLeft_s);

	display.setControlValue(turnOffProgressBar, progress);
	display.setControlText(turnOffProgressBar, turnOffText);
	display.setControlShow(turnOffProgressBar);
	display.refreshControl(turnOffProgressBar);
}

void mtSleep::deinitDisplayCountDown()
{
	display.destroyControl(turnOffProgressBar);
	turnOffProgressBar = nullptr;

	interfaceEnvents(eventToggleActiveModule,0,0,0);
}

