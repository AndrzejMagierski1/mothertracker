


#include "core/interface.h"
#include "mtLED.h"
#include "keyScanner.h"

#include "core/interfacePopups.h"

#include "debugLog.h"
/*
#include "mtInstrumentEditor.h"
#include "mtProjectEditor.h"
#include "mtSampleBankEditor.h"
#include "mtStepEditor.h"
#include "mtConfigEditor.h"

*/




//=======================================================================
void cInterface::potChange(uint8_t n, int16_t value)
{
	FM->processPotsInput(n, value);

}

//=======================================================================
void cInterface::buttonChange(uint8_t n, uint8_t value)
{
	if(value == 2 && ( (n >= 25 && n <= 27) || (n >= 30 && n <= 32 ) ) )
	{
		FM->processButtonsInput(n, 1);
	}
	else
	{
		FM->processButtonsInput(n, value);
	}
}

//=======================================================================
void cInterface::powerButtonChange(uint8_t state)
{
	if(FM->isPowerButtonBlocked()) return;
	if(state == 1 && tactButtons.isButtonPressed(interfaceButtonShift))
	{
		display.doScreenShot();
	}


	FM->processPowerButton(state);
}

//=======================================================================
void cInterface::padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{
	FM->processPadsInput(n, 1, velo);

//	leds.setLEDgrid(n,1,31);
}

//=======================================================================
void cInterface::padReleased(uint8_t n)
{
	FM->processPadsInput(n, 0, 0);

//	leds.setLEDgrid(n,0,31);
}

//=======================================================================
void cInterface::padHold(uint8_t n)
{
	FM->processPadsInput(n, 2, 0);
}

//=======================================================================

