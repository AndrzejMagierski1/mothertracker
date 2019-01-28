
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

uint8_t licznik=0; // wyrzucic po testach
uint16_t freqTest=0; // to samo

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

		if(value>= 1)
		{
			if(mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].cutOff<1.0) mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].cutOff+=0.01;
//			if(mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].cutOff<1.0) mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].cutOff+=0.01;
		}

		if(value<= -1)
		{

			if(mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].cutOff >0.0) mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].cutOff-=0.01;
//			if(mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].cutOff >0.0) mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].cutOff-=0.01;
		}


/*	if(n==2)
	{
		if(value>= 1)
		{
			if(mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].resonance < 4.95) mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].resonance+=0.05;
		}
		else if(value <= -1)
		{
			if(mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].resonance > 0.75) mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].resonance-=0.05;
		}
	}
	if(n==3)
	{
		if(value>= 1)
		{
			if(mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].envelope[envFilter].amount < 1.0) mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].envelope[envFilter].amount+=0.01;
		}
		else if(value <= -1)
		{
			if(mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].envelope[envFilter].amount> 0.0) mtProject.instrument[mtPatern.track[1].step[licznik].instrumentIndex].envelope[envFilter].amount-=0.01;
		}
	}*/


	instrumentPlayer[n].change(&mtPatern.track[n].step[0],&modAudioEngine[0]);
	//instrumentPlayer[n+3].change(&mtPatern.track[n+3].step[0],&modAudioEngine[0]);
/*	mtPrint("pot change: ");
	mtPrint(n);
	mtPrint(" value: ");
	mtPrintln(value); */
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
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].loopPoint1=2716;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].loopPoint2=12224;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].playMode=loopPingPong;

		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].delay=0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].attack=1;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].hold=1;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].decay=1;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].sustain=1.0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].release=2000;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envAmp].amount=1.0;

		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].delay=0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].attack=2000;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].hold=1;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].decay=1;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].sustain=1.0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].release=2000;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].amount=1.0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].envelope[envFilter].enable=envelopeOn;

		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].panning=50;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].glide=50;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].cutOff=1.0;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].resonance=0.7;
		mtProject.instrument[mtPatern.track[n].step[0].instrumentIndex].filterType=lowPass;
		mtPatern.track[n].step[0].volume=100;





/*
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].startPoint=0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].endPoint=65533;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].loopPoint1=2716;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].loopPoint2=12224;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].playMode=loopPingPong;

		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].delay=0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].attack=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].hold=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].decay=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].sustain=1.0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].release=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].amount=1.0;

		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].delay=0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].attack=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].hold=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].decay=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].sustain=1.0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].release=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].amount=1.0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].enable=envelopeOff;

		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].panning=50;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].glide=50;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].cutOff=0.0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].resonance=0.7;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].filterType=highPass;
		mtPatern.track[n+3].step[0].volume=100;
*/






		mtPatern.track[4].step[0].note=24;
		mtPatern.track[3].step[0].note=25;
		mtPatern.track[2].step[0].note=26;
		mtPatern.track[1].step[0].note=27;
		mtPatern.track[5].step[0].note=40;
		mtPatern.track[6].step[0].note=41;
		mtPatern.track[7].step[0].note=42;
		mtPatern.track[8].step[0].note=43;




		if( (value == 1))
			{
			instrumentPlayer[n].play(&mtPatern.track[n].step[0],&modAudioEngine[0]);
//			instrumentPlayer[n+3].play(&mtPatern.track[n+3].step[0],&modAudioEngine[0]);
			}
		else if(value == 0)
			{
			instrumentPlayer[n].stop();
//			instrumentPlayer[n+3].stop();
			}

if(licznik>31) licznik=0;




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

