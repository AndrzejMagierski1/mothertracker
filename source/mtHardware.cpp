
#include "mtHardware.h"

#include "keyScanner.h"
#include "mtLED.h"
#include "BlinkLed.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Si4703.h>

#include "Encoder.h"

#include "display.h"
#include "SD.h"

#include "sdram.h"
#include "hidConnection.h"
#include "sdCardDetect.h"

#include "mtMidi.h"
#include "mtSleep.h"
#include "MTP.h"


//----------------------------------------------------------

uint8_t hardwareTest;

//-------- TACT POWER SWITCH -------------
uint8_t lastState = 1;;

void TactSwitchRead();
void updateEncoder();


void onPotChange(uint8_t n, int16_t value);
void onEncoderButtonChange(uint8_t n, uint8_t value);


void onPowerButtonChange(uint8_t value);

//keyScanner
void onButtonPush(uint8_t n);
void onButtonRelease(uint8_t n);
void onButtonHold(uint8_t n);


// tca8418 new pad driver
void onPadPush(uint8_t n);
void onPadRelease(uint8_t n);
void onPadHold(uint8_t n);
void onSDCardSlotChange(uint8_t n);

void ENC_SW_INT_FUNCT() { }


///------------------------------------------------------------------------------------


///------------------------------------------------------------------------------------
AudioControlSGTL5000 audioShield;


///------------------------------------------------------------------------------------
KEYS Keypad;
KEYS tactButtons;

void KeypadISR()
{  //Keypad Interrupt Service Routine
	Keypad.keyInt = 1;
	//todo:odczytywac tu stany i buforowac jezeli by jeszcze kiedys zamulalo
}

void ButtonsISR()
{  //Keypad Interrupt Service Routine
	tactButtons.keyInt = 1;
	//todo:odczytywac tu stany i buforowac jezeli by jeszcze kiedys zamulalo
}


// hid connection  --------------------------------------------------------------------
hidConnection hid(0);
void hidSendButtonState(uint16_t button, uint16_t state);


///------------------------------------------------------------------------------------
void initHardware()
{

	//noInterrupts();
	hardwareTest=0;

	//reset filter
	RCM_RPFC = 1;

	//....................................................
	//SDRAM
	pinMode(EXTERNAL_RAM_KEY,OUTPUT);
	digitalWrite(EXTERNAL_RAM_KEY,HIGH);
	delay(100);
	Extern_SDRAM_Init();

	BlinkLed.begin(BLINK_LED);
	//BlinkLed.blinkOnceWithDelay();



	Serial.begin(9600);



	pinMode(SI4703_KLUCZ,OUTPUT);
	digitalWrite(SI4703_KLUCZ,LOW);


//	RAM_CTRL_PCR = PORT_PCR_MUX(1);
//	RAM_CTRL_GPIO_DDR |= (1 << RAM_CTRL);
//	RAM_CTRL_GPIO_SET = (1 << RAM_CTRL);



	//Wire.begin(I2C_MASTER, 0x00, I2C_PINS_47_48, I2C_PULLUP_EXT, 400000);//,I2C_OP_MODE_IMM);
	//Wire.begin(I2C_MASTER, 0x00, AUDIO_I2C_SCL, AUDIO_I2C_SDA, I2C_PULLUP_EXT, 400000);
	//Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
	//Wire2.begin(I2C_MASTER, 0x00, GRID_I2C_SCL, GRID_I2C_SDA, I2C_PULLUP_EXT, 400000);


	//....................................................
	//CODEC AUDIO
	audioShield.enable();
	AudioMemory(250);



	//engine.setOut(1);

	// LCD
	display.begin();


	sdCardDetector.begin();
	sdCardDetector.setOnDetectFunction(onSDCardSlotChange);

	//SD CARD
	//....................................................
/*	if (! SD.begin( SdioConfig(DMA_SDIO) ) )	//FIFO_SDIO
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
	}*/


	//mtpd.begin(&storage);



	//....................................................
	// ENCODER
	Encoder.begin(ENC_SWITCH,onEncoderButtonChange);
	//	Encoder.testMode(1);



	Encoder.setResolution(24);
	Encoder.setAcceleration(3);


	//....................................................
	// Haptic on
	//mtHaptic.enable();


	//....................................................
	// Buttons

	Keypad.setOnPush(onPadPush);
	Keypad.setOnRelease(onPadRelease);
	Keypad.setOnHold(onPadHold);

	Wire2.begin(I2C_MASTER, 0x00, GRID_I2C_SCL, GRID_I2C_SDA, I2C_PULLUP_EXT, 400000);

	Keypad.begin(ROW0 | ROW1 | ROW2 | ROW3 | ROW4 | ROW5 | ROW6 | ROW7 , COL0 | COL1 | COL2 | COL3 | COL4 | COL5 | COL6 | COL7 | COL8 | COL9,
	CFG_KE_IEN | CFG_OVR_FLOW_IEN | CFG_INT_CFG | CFG_OVR_FLOW_M, &Wire2 , (uint8_t*)convertToGridKey4x12);

	Keypad.enableInterrupt(GRID_PADS_INT, KeypadISR);


	tactButtons.setOnPush(onButtonPush);
	tactButtons.setOnRelease(onButtonRelease);
	tactButtons.setOnHold(onButtonHold);

	tactButtons.begin(ROW0 | ROW1 | ROW2 | ROW3 | ROW4 | ROW5 | ROW6 | ROW7 , COL0 | COL1 | COL2 | COL3 | COL4 | COL5 | COL6 | COL7 | COL8 | COL9,
	CFG_KE_IEN | CFG_OVR_FLOW_IEN | CFG_INT_CFG | CFG_OVR_FLOW_M, &Wire, (uint8_t*)convertToControlButtons);

	tactButtons.enableInterrupt(CONTROL_BUTTONS_INT, ButtonsISR);

	radio.powerOn();
	radio.setRegion((radio_region_t)mtConfig.general.radioRegion, 1);
	radio.setVolume(10);



	//LEDS
	leds.begin();
	leds.setAllLEDPWM(leds.ledPWM, 0);

	//....................................................

	// power switch
	pinMode(TACT_SWITCH, INPUT); //INPUT_PULLUP
	//attachInterrupt(TACT_SWITCH, TactSwitchAction, FALLING);

	hid.set_sendButtonState(hidSendButtonState);

	midiInit();

	BlinkLed.blinkOnce();


}

void hidSendButtonState(uint16_t button, uint16_t state)
{
	if (button < 100)
	{
		if(state == 1)		onButtonPush(button);
		else if(state == 0)	onButtonRelease(button);
	}
	else if (button < 102)
	{
		if (button == 100 && state == 1) 		onPotChange(0, -1);
		else if (button == 101 && state == 1) 	onPotChange(0, 1);
	}
	else if (button < 103)
	{
		if(state == 1)		onButtonPush(33);
		else if(state == 0)	onButtonRelease(33);
	}

}



elapsedMicros i2cRefreshTimer;
uint8_t i2c_switch;


void updateHardware()
{
	if(!lowPower.isLowPower())
	{
		updateEncoder();
		Encoder.switchRead();

		if(i2cRefreshTimer > 500)
		{
			i2cRefreshTimer = 0;

			i2c_switch++;
			if(i2c_switch >= 2) i2c_switch = 0;

			if (Wire2.done())
			{
				if(i2c_switch == 0)
				{
					if(!Keypad.update()) i2c_switch++;
				}
				if(i2c_switch == 1)
				{
					if(!leds.update_all_leds())	i2c_switch++;
				}

				/*if(i2c_switch < 2) i2cRefreshTimer = 0;*/
			}

			if (Wire.done())
			{
				tactButtons.update();
			}
		}

		display.update();
		//mtDisplay.updateHaptic();
		BlinkLed.update();

		hid.handle();
		sdCardDetector.update(0);

	    if(mtpd.state) mtpd.loop();

//		midiUpdate();

		//lowPower.update();
	}


	TactSwitchRead();
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
	uint8_t state = digitalRead(TACT_SWITCH);

	if(state == LOW)
	{
		onPowerButtonChange(1);

		//lastState = LOW;
	}
	else if(state == HIGH)
	{
		//lastState = HIGH;
		onPowerButtonChange(0);
	}
}
