#include "mtSleep.h"
#include "mtLED.h"
#include "sdram.h"
#include <Audio.h>
#include "FT812.h"
#include "keyScanner.h"
#include "Si4703.h"

// for displaying end screen
#include "FT812.h"
#include "FT8xx.h"

extern AudioControlSGTL5000 audioShield;
SnoozeDigital digital;
SnoozeBlock config(digital);

uint8_t powerChanged;
uint8_t powerState = powerTypeNormal;

static void countDownDisplay();
static void resetMCU();

void goLowPower()
{
	countDownDisplay();
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

static void countDownDisplay()
{
	uint32_t elapsedTime = 0;
	uint32_t startTime = millis();
	int32_t toGo;

	char text[30];

	while(true)
	{
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();
		API_VERTEX_FORMAT(0);
		API_CLEAR_COLOR(0);
		API_CLEAR(1,1,1);

		elapsedTime = (millis() - startTime)/1000; // calculate and convert to seconds

		toGo = (TURN_OFF_TIME_S - elapsedTime);

		sprintf(text, "Turning off in: %d", (int)toGo);

		API_CMD_TEXT(400, 220 , 30, OPT_CENTERX, text);

		API_DISPLAY();
		API_CMD_SWAP();
		API_LIB_EndCoProList();

		if(toGo <= 0)
		{
			break;
		}
	}
}

