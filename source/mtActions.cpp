
#include <stdint.h>

#include <Arduino.h>
#include "keyScanner.h"
#include "mtLED.h"
#include "mtAudioEngine.h"

#include "mtProjectEditor.h"

#include "mtInstrumentEditor.h"

#include "mtSequencer.h"
#include "mtDisplay.h"



#include "mtInterface.h"







void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{

	mtInterface.padPressed(n,x,y,velo);




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
	mtInterface.padReleased(n);

}

//-----------------------------------------------------------------
//-------------------------|   POTS   |----------------------------
//-----------------------------------------------------------------
void onPotChange(uint8_t n, int16_t value)
{


	mtInterface.potChange(n, value);

	mtHaptic.start(15,150,0x01,56);



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


	//mtInstrumentEditor.buttonChange(n,value);


		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].startPoint=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].endPoint=65533;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].loopPoint1=2716;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].loopPoint2=12224;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].playMode=loopPingPong;

		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].delay=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].attack=1;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].hold=1;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].decay=1;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].sustain=0.5;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].release=5000;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].envelope[envAmp].amount=1.0;

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




		mtInterface.buttonChange(n,value);


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

