


#include <interface.h>
#include "mtLED.h"
#include "SD.h"



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
	uiFM.processPotsInput(n, value);


}

//=======================================================================
void cInterface::buttonChange(uint8_t n, uint8_t value)
{
	if(value==2 && (n == 26 || n == 30 || n == 31 || n == 32 || n == 29))
	{
		uiFM.processButtonsInput(n, 1);
	}
	else
	{
		uiFM.processButtonsInput(n, value);
	}
}

//=======================================================================
void cInterface::powerButtonChange(uint8_t value)
{

}

//=======================================================================
void cInterface::padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{
	uiFM.processPadsInput(n, 1, velo);

//	leds.setLEDgrid(n,1,31);
}

//=======================================================================
void cInterface::padReleased(uint8_t n)
{
	uiFM.processPadsInput(n, 0, 0);

//	leds.setLEDgrid(n,0,31);
}

//=======================================================================
void cInterface::padHold(uint8_t n)
{
	uiFM.processPadsInput(n, 2, 0);
}

//=======================================================================
void cInterface::SDCardChange(uint8_t state)
{
	/*
			strPopupStyleConfig config;
			config.time = 2;
			config.w = 300;
			config.h = 100;
			config.x = 800/2-150;
			config.y = 480/2-50;
			config.lineColor[0] = 0xff0000;
			config.lineColor[1] = 0x00ff00;
			config.lineStyle[0] = controlStyleCenterX;
			config.lineStyle[1] = controlStyleCenterX;


			mtPopups.config(0, &config);
			mtPopups.show(0, "Warning!", "Edit mode enabled");
	*/
}

