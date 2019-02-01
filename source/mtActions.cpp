
#include <stdint.h>

#include <Arduino.h>
#include "keyScanner.h"
#include "mtLED.h"

#include "mtAudioEngine.h"
#include "mtProjectEditor.h"
#include "mtSequencer.h"


extern uint32_t wavLen1;
extern int16_t wavBuf1[500800];
extern AudioPlayMemory playMem1;
extern playerEngine instrumentPlayer[8];
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
		sequencer.seq[0].track[n].step[0].velocity=100;

		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoA].rate=600;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoA].wave=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoA].amount=4095;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoA].sync=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoA].enable=lfoOff;

		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoF].rate=600;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoF].wave=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoF].amount=4095;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoF].sync=0;
		mtProject.instrument[sequencer.seq[0].track[n].step[0].instrument].lfo[lfoF].enable=lfoOff;



	/*	mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].startPoint=0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].endPoint=65533;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].loopPoint1=2716;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].loopPoint2=12224;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].playMode=loopPingPong;

		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].delay=0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].attack=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].hold=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].decay=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].sustain=4095;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].release=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envAmp].amount=4095;

		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].delay=0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].attack=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].hold=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].decay=1;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].sustain=4095;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].release=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].amount=2000;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].envelope[envFilter].enable=envelopeOff;

		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].panning=50;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].glide=50;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].cutOff=1.0;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].resonance=0.7;
		mtProject.instrument[mtPatern.track[n+3].step[0].instrumentIndex].filterType=highPass;
		mtPatern.track[n+3].step[0].volume=100;*/


		sequencer.seq[0].track[1].step[0].note=24;
		sequencer.seq[0].track[2].step[0].note=25;
		sequencer.seq[0].track[3].step[0].note=26;
		sequencer.seq[0].track[4].step[0].note=27;
		sequencer.seq[0].track[5].step[0].note=40;
		sequencer.seq[0].track[6].step[0].note=41;
		sequencer.seq[0].track[7].step[0].note=42;
		sequencer.seq[0].track[8].step[0].note=43;



		if( (value == 1))
			{

			instrumentPlayer[n].noteOn(sequencer.seq[0].track[0].step[0].instrument,sequencer.seq[0].track[0].step[0].note,sequencer.seq[0].track[0].step[0].velocity);
//			instrumentPlayer[n+3].play(&mtPatern.track[n+3].step[0],&modAudioEngine[0]);
			}
		else if(value == 0)
			{
			instrumentPlayer[n].noteOff();
//			instrumentPlayer[n+3].stop();
			}





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

