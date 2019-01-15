
#include <stdint.h>

#include <Arduino.h>
#include "keyScanner.h"
#include "mtLED.h"

#include "mtAudioEngine.h"
#include "mtProjectEditor.h"


extern uint32_t wavLen1;
extern int16_t wavBuf1[500800];
extern AudioPlayMemory playMem1;


#include "mtDisplay.h"



void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{


	if(n==3)
	{
		for(int i=1;i<=64;i++)
		{

				leds.setLEDgrid(i,1,31);
				//delayMicroseconds(100000);

		}
		leds.updateGrid();
	}




//	mtPrint("pad press: ");
//	mtPrintln(n);

}

//-----------------------------------------------------------------
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f)
{

}

//-----------------------------------------------------------------
void onPadRelease(uint8_t n)
{

	if(n==3)
	{
		for(int i=1;i<=64;i++)
		{

				leds.setLEDgrid(i,0,31);
				//delayMicroseconds(100000);

		}
		leds.updateGrid();
	}


}

//-----------------------------------------------------------------
//-------------------------|   POTS   |----------------------------
//-----------------------------------------------------------------
void onPotChange(uint8_t n, int16_t value)
{





	mtHaptic.start(15,150,0x01,56);

	mtPrint("pot change: ");
	mtPrint(n);
	mtPrint(" value: ");
	mtPrintln(value);
}

//-----------------------------------------------------------------
//------------------------|   BUTTONS   |--------------------------
//-----------------------------------------------------------------
// 0 - puszczenie
// 1 - wcisniecie
// 2 - hold
// 3 - double click
void onButtonChange(uint8_t n, uint8_t value)
{
	if(n == 5)
	{
		onPowerButtonChange(value);
		return;
	}



		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].startPoint=0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].endPoint=100;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].loopPoint1=10;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].loopPoint2=60;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].playMode=1;

		mtPatern.track[4].step[0].pitchCtrl=2;
		mtPatern.track[3].step[0].pitchCtrl=1.25;
		mtPatern.track[2].step[0].pitchCtrl=1.5;
		mtPatern.track[1].step[0].pitchCtrl=1.75;

		if( (value == 1)) playMem1.play(&mtPatern.track[n].step[0]);
		else if(value == 0) playMem1.stopLoopMode();






//	mtPrint("button: ");
//	mtPrint(n);
//	mtPrint(" value: ");
//	mtPrintln(value);

}
//-----------------------------------------------------------------
//----------------------|   POWER BUTTON   |-----------------------
//-----------------------------------------------------------------
void onPowerButtonChange(uint8_t value)
{



//	mtPrint("power button: ");
//	mtPrintln(value);
}



extern keyScanner Buttons;

//--------------------------------- BUTTONS SEQ ----------------------------
void onButtonPush (uint8_t x, uint8_t y)
{	
	Serial.print("ButtonPush: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

void onButtonRelease(uint8_t x, uint8_t y)
{	
	Serial.print("ButtonRelease: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

void onButtonHold(uint8_t x, uint8_t y)
{	
	Serial.print("ButtonHold: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

void onButtonDouble	(uint8_t x, uint8_t y)
{	
	Serial.print("ButtonDouble: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

