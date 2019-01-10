
#include <stdint.h>

#include "mtDisplay.h"


void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
//------------------------|   PADS   |-----------------------------
//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{




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





	mtHaptic.start(25,200,0x02,56);

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



