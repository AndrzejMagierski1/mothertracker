
#include "mtHardware.h"
#include "AnalogInputs.h"

#include "keyScanner.h"
#include "mtLED.h"
#include "BlinkLed.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Encoder.h"

#include "display.h"
#include "SD.h"

#include "sdram.h"
#include "hidConnection.h"
#include "sdCardDetect.h"

#include "mtMidi.h"





uint8_t hardwareTest;

//-------- TACT POWER SWITCH -------------
elapsedMillis powerSwitchHoldTimer;
uint8_t powerSwitchLastState = 0;
int8_t powerSwitchDebounce;

void TactSwitchRead();
void updateEncoder();

void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo);
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f);
void onPadRelease(uint8_t n);
void onPotChange(uint8_t n, int16_t value);
void onButtonChange(uint8_t n, uint8_t value);
void onPowerButtonChange(uint8_t value);

//keyScanner
void onButtonPush			(uint8_t x,uint8_t state);
void onButtonRelease		(uint8_t x,uint8_t state);
void onButtonHold			(uint8_t x,uint8_t state);
void onButtonDouble			(uint8_t x,uint8_t state);

keyScanner seqButtonsA;
keyScanner tactButtons;

AudioControlSGTL5000 audioShield;

void IO7326_INT_FUNCT_A() { seqButtonsA.intAction(); }
void IO7326_TACT_INT_FUNCT() {tactButtons.intAction();}
//void IO7326_INT_FUNCT_B() { seqButtonsB.intAction(); }
//void IO7326_INT_FUNCT_C() { seqButtonsC.intAction(); }


void ENC_SW_INT_FUNCT() { }

// hid connection
hidConnection hid(0);
void hidSendButtonState(uint16_t button, uint16_t state);



void initHardware()
{


	hardwareTest=0;


	BlinkLed.begin(BLINK_LED);
	BlinkLed.blinkOnceWithDelay();



	Serial.begin(9600);

	//....................................................
	//CODEC AUDIO

	RAM_CTRL_PCR = PORT_PCR_MUX(1);
	RAM_CTRL_GPIO_DDR |= (1 << RAM_CTRL);
	RAM_CTRL_GPIO_SET = (1 << RAM_CTRL);



	audioShield.enable();
	AudioMemory(200);

	//engine.setOut(1);

	// LCD
	display.begin();

	//SD CARD
	//....................................................
	if (!SD.begin(SdioConfig(DMA_SDIO)))	//FIFO_SDIO
	{
		if(hardwareTest)
		{
		 Serial.println("SD card init error");
		 //mtPrint("SD card init error");
		}
	}
	else
	{
		if(hardwareTest)
		{
		 Serial.println("SD card init succesfull");
		 //mtPrint("SD card init succesfull");
		}
	}



	//....................................................
	//SDRAM
	Extern_SDRAM_Init();

	//....................................................
	// Pady, Poty, Buttony
	//AnalogInputs.setButtonChangeFunc(onButtonChange);

	//AnalogInputs.testMode(hardwareTest); // (1 = on; 0 = off) test mode
	//pinMode(MUX_OUT_0, INPUT);
/*
	AnalogInputs.setPotResolution(0, 100);
	AnalogInputs.setPotResolution(1, 100);
	AnalogInputs.setPotResolution(2, 100);
	AnalogInputs.setPotResolution(3, 100);
	AnalogInputs.setPotResolution(4, 100);
*/

//	AnalogInputs.setPotAcceleration(0, 3);
//	AnalogInputs.setPotAcceleration(1, 3);
//	AnalogInputs.setPotAcceleration(2, 3);
//	AnalogInputs.setPotAcceleration(3, 3);
//	AnalogInputs.setPotAcceleration(4, 3);

	//AnalogInputs.begin(100);

	//....................................................
	// ENCODER
	Encoder.begin(ENC_SWITCH,onButtonChange);
	//	Encoder.testMode(1);

	Encoder.setResolution(12);
	Encoder.setAcceleration(1);

	//....................................................
	// Haptic on
	//mtHaptic.enable();
	//....................................................
	// Seq buttons
	////////////////// IO7326 A
	seqButtonsA.setButtonPushFunc(onButtonPush);
	seqButtonsA.setButtonReleaseFunc(onButtonRelease);
	seqButtonsA.setButtonHoldFunc(onButtonHold);
	seqButtonsA.setButtonDoubleFunc(onButtonDouble);
	seqButtonsA.setHoldTime(200);
	seqButtonsA.setDoubleTime(300);

	tactButtons.setButtonPushFunc(onButtonChange);
	tactButtons.setButtonReleaseFunc(onButtonChange);
	tactButtons.setButtonHoldFunc(onButtonChange);
	tactButtons.setButtonDoubleFunc(onButtonChange);
	tactButtons.setHoldTime(200);
	tactButtons.setDoubleTime(300);


	////////////////// IO7326
	tactButtons.begin(IO7326_ADDR3,I2C_SDA,I2C_SCL,TACTILE_INT,tactileToKeyMapping,IO7326_TACT_INT_FUNCT);
	seqButtonsA.begin(IO7326_ADDR1,I2C_SDA,I2C_SCL,GRID_A,gridToKeyMapping,IO7326_INT_FUNCT_A);

	tactButtons.testMode(0);

	//LEDS
	leds.begin();
	leds.setAllLEDPWM(leds.ledPWM, 0);

	//....................................................

	// power switch
	pinMode(TACT_SWITCH, INPUT);
	//attachInterrupt(TACT_SWITCH, TactSwitchAction, FALLING);

	hid.set_sendButtonState(hidSendButtonState);
	sdCardDetector.begin();

	midiInit();

	BlinkLed.blinkOnce();


}

void hidSendButtonState(uint16_t button, uint16_t state)
{
	if (button < 100)
	{
		onButtonChange(button, state);
	}
	else if (button < 102)
	{
		if (button == 100 && state == 1)
		onPotChange(0, -1);
		else if (button == 101 && state == 1)
			onPotChange(0, 1);
	}
	else if (button < 103)
	{
		onButtonChange(33, state);
	}

}


elapsedMicros i2cRefreshTimer;
uint8_t i2c_switch;

void updateHardware()
{
	updateEncoder();
	Encoder.switchRead();

	if(i2cRefreshTimer > 500)
	{
		i2c_switch++;
		if(i2c_switch >= 3) i2c_switch = 0;

		if (Wire2.done())
		{
			if(i2c_switch == 0)
			{
				if(!tactButtons.update()) 	i2c_switch++;
			}
			if(i2c_switch == 1)
			{
				if(!seqButtonsA.update())  	i2c_switch++;
			}
			if(i2c_switch == 2)
			{
				if(!leds.update_all_leds())	i2c_switch++;
			}

			if(i2c_switch < 3) i2cRefreshTimer = 0;
		}
	}

	display.update();
	//mtDisplay.updateHaptic();
	BlinkLed.update();

	TactSwitchRead();
	hid.handle();
	sdCardDetector.update();

	midiUpdate();
}

elapsedMillis encTimer;

void updateEncoder()
{
	if (encTimer > 100)
	{
		encTimer = 0;
		int32_t encValue = Encoder.read();
		if (encValue != 0) onPotChange(0, encValue);
	}
}

void TactSwitchRead()
{
	if (digitalRead(TACT_SWITCH) == LOW)
	{
		if (powerSwitchDebounce <= 0 && powerSwitchDebounce > (0 - 10))
		{
			powerSwitchDebounce--;
		}
		else if (powerSwitchDebounce <= 0 && powerSwitchLastState != 1)
		{
			powerSwitchDebounce = 1;
			powerSwitchLastState = 1;
			powerSwitchHoldTimer = 0;
			// press
			onPowerButtonChange(1);
		}
		else if(powerSwitchHoldTimer > 800 &&  powerSwitchLastState != 2)
		{
			powerSwitchLastState = 2;
			// hold
			onPowerButtonChange(2);
		}
	}
	else
	{
		if(powerSwitchLastState > 0 && powerSwitchDebounce < 10)
		{
			powerSwitchDebounce++;
		}
		else if(powerSwitchDebounce > 0 && powerSwitchLastState != 0)
		{
			powerSwitchDebounce = 0;
			powerSwitchLastState = 0;
			//buttons[i].hold_time = 0;
			// release
			onPowerButtonChange(0);
		}


	}
}
