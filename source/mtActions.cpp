
#include <stdint.h>
#include <Arduino.h>


#include "keyScanner.h"
#include "mtLED.h"




#include "mtPadsBacklight.h"
#include "interface.h"




void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{
	mtInterface.padPressed(n,x,y,velo);

	padsBacklight.setFrontLayer(1, 31, n);

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
	mtInterface.padReleased(n);
	//padsBacklight.setFrontLayer(0, 31, n);
	padsBacklight.setFrontLayer(0,0,n);
}

//-----------------------------------------------------------------
//-------------------------|   POTS   |----------------------------
//-----------------------------------------------------------------
void onPotChange(uint8_t n, int16_t value)
{


	mtInterface.potChange(n, value);

//	if(value>0)
//	{
//		if(licznik<1.0f) licznik+=0.01;
//	}
//	else if(value<0)
//	{
//		if(licznik>0.0f) licznik-=0.01;
//	}
//	audioShield.volume(licznik);

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
	if(n == 5)
	{
		onPowerButtonChange(value);
		return;
	}


	mtInterface.buttonChange(n,value);


/*
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].delay=0;

		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].attack=1;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].hold=1;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].decay=1;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].sustain=0.5;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].release=5000;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].amount=1.0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].enable=envelopeOff;

		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].delay=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].attack=2000;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].hold=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].decay=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].sustain=0.5;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].release=2000;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].amount=1.0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envFilter].enable=envelopeOff;

		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].panning=50;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].glide=50;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].cutOff=0.0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].resonance=0.7;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].filterType=highPass;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].filterEnable=filterOn;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].volume=100;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].tune=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].fineTune=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].wavetableCurrentWindow=0;

		mtProject.sampleBank.sample[mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].sampleIndex].wavetable_window_size = 2048;
*/

/*		if(!n)
		{
			if(value)instrumentPlayer[4].modFineTune(50);
			else instrumentPlayer[4].modFineTune(0);

		}
		else
		{*/
//			if(value) instrumentPlayer[n].noteOn(sequencer.seq[0].track[n].step[0].instrument,24,100);
//			else instrumentPlayer[n].noteOff();
		//}
		//mtInterface.buttonChange(n,value);



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
	mtInterface.powerButtonChange(value);


//	mtPrint("power button: ");
//	mtPrintln(value);
}



//--------------------------------- BUTTONS SEQ ----------------------------
void onButtonPush (uint8_t x, uint8_t y)
{	
	mtInterface.seqButtonPressed(x,y);


	Serial.print("ButtonPush: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

void onButtonRelease(uint8_t x, uint8_t y)
{	
	mtInterface.seqButtonReleased(x,y);

	Serial.print("ButtonRelease: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

void onButtonHold(uint8_t x, uint8_t y)
{	
	mtInterface.seqButtonHold(x,y);


	Serial.print("ButtonHold: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

void onButtonDouble	(uint8_t x, uint8_t y)
{	
	mtInterface.seqButtonDouble(x,y);


	Serial.print("ButtonDouble: x = ");
	Serial.print(x);
	Serial.print(" y = ");
	Serial.println(y);
}

