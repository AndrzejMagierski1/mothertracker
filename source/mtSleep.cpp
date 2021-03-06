
#include "mtLED.h"
#include "sdram.h"
#include <Audio.h>
#include "FT812.h"
#include "keyScanner.h"
#include "Si4703.h"

#include "mtConfig.h"

#include "mtSleep.h"

mtSleep lowPower;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);

extern AudioControlSGTL5000 audioShield;
SnoozeDigital digital;
SnoozeBlock config(digital);

elapsedMillis shutdownTimer;

extern "C" void restartMCU()
{
	lowPower.resetMCU();
}

void mtSleep::startPowerOffSequence()
{
	if(shutdownState == shutdownStateNone)
	{
		shutdownState = shutdownStateStart;
		shutdownTimer = 0;
	}
}

void mtSleep::stopPowerOffSequence()
{
	shutdownState = shutdownStateNone;
}

uint16_t mtSleep::getTimeLeft()
{
	return (shutdownTimer < TURN_OFF_TIME) ? TURN_OFF_TIME-shutdownTimer : 0;
}

uint8_t mtSleep::getShutdownProgress()
{
	return (shutdownTimer < TURN_OFF_TIME) ? ((TURN_OFF_TIME-shutdownTimer)*100)/TURN_OFF_TIME : 100;
}

void mtSleep::wakeUp()
{
	shutdownState = shutdownStateNone;
	Snooze.wakeUp(config); // wake up potrzebne zeby zapis eepromu zadzialal
	resetMCU();
}

void mtSleep::goLowPower(uint8_t isMCUInicialized)
{
	saveStartState(0);
	shutdownState = shutdownStateSleep;
	noInterrupts();

	if(isMCUInicialized) disableAll();

	Snooze.sleep(config);
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
	saveStartState(1);

	__DSB();
	CM4_SCB_AIRCR = (uint32_t)((0x5FAUL << CM4_SCB_AIRCR_VECTKEY_POS) | CM4_SCB_AIRCR_SYSRESETREQ_MASK);
	__DSB();

	for(;;)
	{
		__NOP();
	}

}

