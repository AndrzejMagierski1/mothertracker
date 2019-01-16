
#include <stdint.h>

#include <Arduino.h>
#include "keyScanner.h"
#include "mtLED.h"

#include "mtAudioEngine.h"
#include "mtProjectEditor.h"


extern uint32_t wavLen1;
extern int16_t wavBuf1[500800];
extern AudioPlayMemory playMem1;
extern instrumentEngine instrumentPlayer[8];
extern strMtModAudioEngine modAudioEngine[8];
#include "mtDisplay.h"



void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{


Serial.println(n);



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
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].endPoint=65533;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].loopPoint1=1000;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].loopPoint2=20000;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].playMode=1;

		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].ampDelay=0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].ampAttack=300;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].ampHold=50;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].ampDecay=100;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].ampSustain=0.7;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].ampRelease=500;

		mtPatern.track[4].step[0].note=20;
		mtPatern.track[3].step[0].note=21;
		mtPatern.track[2].step[0].note=22;
		mtPatern.track[1].step[0].note=23;

		if( (value == 1)) instrumentPlayer[0].play(&mtPatern.track[n].step[0],&modAudioEngine[0]);
		else if(value == 0) instrumentPlayer[0].stop();






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

