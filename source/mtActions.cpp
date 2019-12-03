
#include <stdint.h>


#include <interface.h>

#include "keyScanner.h"


#include "mtSleep.h"
#include "mtCardChecker.h"




//-----------------------------------------------------------------
//-------------------------|   POTS   |----------------------------
//-----------------------------------------------------------------
void onPotChange(uint8_t n, int16_t value)
{
	//Serial.println(value);

	mtInterface.potChange(n, value);

}

void onEncoderButtonChange(uint8_t n, uint8_t value)
{

}

//-----------------------------------------------------------------
//------------------------|   BUTTONS   |--------------------------
//-----------------------------------------------------------------
// 0 - puszczenie
// 1 - wcisniecie
// 2 - hold
// 3 - double click
void onButtonPush(uint8_t n)
{
	mtInterface.buttonChange(n,1);
}

void onButtonRelease(uint8_t n)
{
	mtInterface.buttonChange(n,0);
}

void onButtonHold(uint8_t n)
{
	mtInterface.buttonChange(n,2);
}

//-----------------------------------------------------------------
//----------------------|   POWER BUTTON   |-----------------------
//-----------------------------------------------------------------
void onPowerButtonChange(uint8_t value)
{
	mtInterface.powerButtonChange(value);

	lowPower.handlePowerState(value);

//	mtPrint("power button: ");
//	mtPrintln(value);
}



//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------

// TCA8418 NEW DRIVER PADS

void onPadPush(uint8_t n)
{
	mtInterface.padPressed(n,0,0,0);
}

void onPadRelease(uint8_t n)
{
	mtInterface.padReleased(n);
}

void onPadHold(uint8_t n)
{
	mtInterface.padHold(n);
}

void onSDCardSlotChange(uint8_t n)
{
	//mtInterface.SDCardChange(n);
	if(n == 1)
	{
		mtCardHandler.sdCardInsertedAction();
	}
	else
	{
		mtCardHandler.noSdCardAction();
	}

	//if(!n) Serial.println("SD OUT");
	//else Serial.println("SD IN");
}
