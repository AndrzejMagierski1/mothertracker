
#include "mtHardware.h"
#include "AnalogInputs.h"

#include "keyScanner.h"
#include "mtLED.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


#include "mtDisplay.h"
#include "SD.h"


#include "sdram.h"

uint8_t hardwareTest;


void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo);
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f);
void onPadRelease(uint8_t n);
void onPotChange(uint8_t n, int16_t value);
void onButtonChange(uint8_t n, uint8_t value);

//keyScanner
void onButtonPush			(uint8_t x, uint8_t y);
void onButtonRelease		(uint8_t x, uint8_t y);
void onButtonHold			(uint8_t x, uint8_t y);
void onButtonDouble			(uint8_t x, uint8_t y);

keyScanner seqButtonsA,seqButtonsB,seqButtonsC;
AudioControlSGTL5000 audioShield;

uint8_t audioOutStatus;
uint8_t audioInStatus;

void IO7326_INT_FUNCT_A() { seqButtonsA.intAction(); }
void IO7326_INT_FUNCT_B() { seqButtonsB.intAction(); }
void IO7326_INT_FUNCT_C() { seqButtonsC.intAction(); }

void initHardware()
{
	hardwareTest=1;

	Serial.begin(9600);

	//....................................................
	//CODAC AUDIO
	audioShield.enable();
	audioShield.volume(1.0);
	audioShield.lineOutLevel(26);
	AudioMemory(200);

	// LCD
	mtDisplay.begin(mtDisplayModePolyLogo);

	//SD CARD
	//....................................................
	if (!SD.begin(SdioConfig(DMA_SDIO)))	//FIFO_SDIO
	{
		if(hardwareTest)
		{
		 Serial.println("SD card init error");
		 mtPrint("SD card init error");
		}
	}
	else
	{
		if(hardwareTest)
		{
		 Serial.println("SD card init succesfull");
		 mtPrint("SD card init succesfull");
		}
	}



	//....................................................
	//SDRAM
	Extern_SDRAM_Init();

	//....................................................
	// Pady, Poty, Buttony
	AnalogInputs.setPadPressFunc(onPadPress);
	AnalogInputs.setPadChangeFunc(onPadChange);
	AnalogInputs.setPadReleaseFunc(onPadRelease);
	AnalogInputs.setPotChangeFunc(onPotChange);
	AnalogInputs.setButtonChangeFunc(onButtonChange);

	AnalogInputs.testMode(hardwareTest); // (1 = on; 0 = off) test mode
	AnalogInputs.setPadxMode(0);
	AnalogInputs.setPadyMode(0);
	AnalogInputs.setPotDeathZone(7);
/*
	AnalogInputs.setPotResolution(0, 100);
	AnalogInputs.setPotResolution(1, 100);
	AnalogInputs.setPotResolution(2, 100);
	AnalogInputs.setPotResolution(3, 100);
	AnalogInputs.setPotResolution(4, 100);
*/

	AnalogInputs.setPotAcceleration(0, 3);
	AnalogInputs.setPotAcceleration(1, 3);
	AnalogInputs.setPotAcceleration(2, 3);
	AnalogInputs.setPotAcceleration(3, 3);
	AnalogInputs.setPotAcceleration(4, 3);

	AnalogInputs.begin(100);

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
	////////////////// IO7326 B
	seqButtonsB.setButtonPushFunc(onButtonPush);
	seqButtonsB.setButtonReleaseFunc(onButtonRelease);
	seqButtonsB.setButtonHoldFunc(onButtonHold);
	seqButtonsB.setButtonDoubleFunc(onButtonDouble);
	seqButtonsB.setHoldTime(200);
	seqButtonsB.setDoubleTime(300);
	////////////////// IO7326 C
	seqButtonsC.setButtonPushFunc(onButtonPush);
	seqButtonsC.setButtonReleaseFunc(onButtonRelease);
	seqButtonsC.setButtonHoldFunc(onButtonHold);
	seqButtonsC.setButtonDoubleFunc(onButtonDouble);
	seqButtonsC.setHoldTime(200);
	seqButtonsC.setDoubleTime(300);

	seqButtonsA.testMode(hardwareTest);
	seqButtonsB.testMode(hardwareTest);
	seqButtonsC.testMode(hardwareTest);

	////////////////// IO7326 A
	seqButtonsA.begin(IO7326_ADDR1,I2C_SDA,I2C_SCL,GRID_A,IO7326_INT_FUNCT_A);
	////////////////// IO7326 B
	seqButtonsB.begin(IO7326_ADDR3,I2C_SDA,I2C_SCL,GRID_B,IO7326_INT_FUNCT_B);
	////////////////// IO7326 C
	seqButtonsC.begin(IO7326_ADDR2,I2C_SDA,I2C_SCL,GRID_C,IO7326_INT_FUNCT_C);

	//LEDS
	leds.begin();
	leds.setAllLEDPWM(leds.ledPWMseq,leds.ledPWMgrid, 0);

	//....................................................



	while(1)
	{

		for(int i=1;i<=20;i++)
		{
			for(int j=1;j<=8;j++)
			{
				leds.setLEDseq(j,i,1,31);
				leds.updateSeq();
				delay(50);
			}
/*			for(int j=1;j<=8;j++)
			{
				leds.setLEDseq(j,i,0,31);
				leds.updateSeq();
				delay(50);
			}*/
		}
		for(int i=1;i<=20;i++)
		{
			for(int j=1;j<=8;j++)
			{
				leds.setLEDseq(j,i,0,31);
				leds.updateSeq();
				delay(50);
			}
/*			for(int j=1;j<=8;j++)
			{
				leds.setLEDseq(j,i,0,31);
				leds.updateSeq();
				delay(50);
			}*/
		}

	}
}



void updateHardware()
{
	AnalogInputs.update();


	if(Wire2.done())
	{
			// odczyt przyciskow
			if(seqButtonsA.update())
			{
				return;
			}
			if(seqButtonsB.update())
			{
				return;
			}
			if(seqButtonsC.update())
			{
				return;
			}
			leds.updateSeq();
			leds.updateGrid();

	}

	mtDisplay.updateDisplay();
	mtDisplay.updateHaptic();

}
