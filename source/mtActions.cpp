
#include <stdint.h>

#include <Arduino.h>
#include <interface.h>
#include "keyScanner.h"
#include "mtLED.h"
#include "mtAudioEngine.h"

#include "mtSequencer.h"



#include "mtPadsBacklight.h"


#include "mtEffector.h"
#include "mtExporterWAV.h"


extern AudioControlSGTL5000 audioShield;

uint16_t licznik=32000; //todo: wyrzucic



void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{
	//mtInterface.padPressed(n,x,y,velo);

	//padsBacklight.setFrontLayer(1, 31, n);


	//mtPrint("Pad press: ");
	//mtPrintln(n);

}

//-----------------------------------------------------------------
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f)
{

}

//-----------------------------------------------------------------
void onPadRelease(uint8_t n)
{
	//mtInterface.padReleased(n);
	//padsBacklight.setFrontLayer(0, 31, n);
	//padsBacklight.setFrontLayer(0,0,n);

}

//-----------------------------------------------------------------
//-------------------------|   POTS   |----------------------------
//-----------------------------------------------------------------
void onPotChange(uint8_t n, int16_t value)
{

//Serial.println(value);
	mtInterface.potChange(n, value);
//
//	if(value>0)
//	{
//		if(licznik<31900) licznik+=100;
//	}
//	else if(value<0)
//	{
//		if(licznik>100) licznik-=100;
//	}
//	limiter[0].setThreshold(licznik);
//	limiter[1].setThreshold(licznik);
////	audioShield.volume(licznik);
//	Serial.println(licznik);
//	mtHaptic.start(15,150,0x01,56);



/*	if(value > 0 )

	{
		if(mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow  < MAX_WAVETABLE_WINDOW)
		{
			mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow++;
			Serial.print("pot: ");
			Serial.print(n);
			Serial.print(" wavetableWindow: ");
			Serial.println(mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow);
			instrumentPlayer[n].modWavetableWindow(mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow);

		}

	}
	else if( value < 0)
	{
		if(mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow  > 0)
		{
			mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow--;
			Serial.print("pot: ");
			Serial.print(n);
			Serial.print(" wavetableWindow: ");
			Serial.println(mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow);
			instrumentPlayer[n].modWavetableWindow(mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow);
		}

	}

*/



/*	mtPrint("pot change: ");
	mtPrint(n);
	mtPrint(" value: ");
	mtPrintln(value);
*/

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
	mtInterface.buttonChange(n,value);
}
//-----------------------------------------------------------------
//----------------------|   POWER BUTTON   |-----------------------
//-----------------------------------------------------------------
void onPowerButtonChange(uint8_t value)
{
	mtInterface.powerButtonChange(value);


//	mtPrint("power button: ");
//	mtPrintln(value);
}



//--------------------------------- BUTTONS SEQ ----------------------------
void onButtonPush (uint8_t x,uint8_t state)
{	
	mtInterface.padPressed(x,0,0,0);

	leds.setLED(x, 1, 31);
	//Serial.print("ButtonPush: x = ");
	//Serial.print(x);

}

void onButtonRelease(uint8_t x,uint8_t state)
{	
	mtInterface.padReleased(x);

	leds.setLED(x, 0, 0);
	//Serial.print("ButtonRelease: x = ");
	//Serial.print(x);

}

void onButtonHold(uint8_t x,uint8_t state)
{	
//	mtInterface.seqButtonHold(x,y);


//	Serial.print("ButtonHold: x = ");
//	Serial.print(x);

}

void onButtonDouble(uint8_t x,uint8_t state)
{	
//	mtInterface.seqButtonDouble(x,y);


//	`Serial.print("ButtonDouble: x = ");
//	Serial.print(x);

}

