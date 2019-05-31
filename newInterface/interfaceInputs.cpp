


#include "mtLED.h"
#include "SD.h"


#include "interface.h"

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

	uiFM.processButtonsInput(n, value);



}

//=======================================================================
void cInterface::powerButtonChange(uint8_t value)
{
	if(value == 1)
	{
		SD.begin(SdioConfig(DMA_SDIO));
	}
}

//=======================================================================
void cInterface::padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{
	uiFM.processPadsInput(n, 1, velo);

	leds.setLEDgrid(n,1,31);
}

//=======================================================================
void cInterface::padReleased(uint8_t n)
{
	uiFM.processPadsInput(n, 0, 0);

	leds.setLEDgrid(n,0,31);
}

//=======================================================================
void cInterface::seqButtonPressed(uint8_t x, uint8_t y)
{

}

//=======================================================================
void cInterface::seqButtonReleased(uint8_t x, uint8_t y)
{

}

//=======================================================================
void cInterface::seqButtonHold(uint8_t x, uint8_t y)
{

}

//=======================================================================
void cInterface::seqButtonDouble(uint8_t x, uint8_t y)
{

}
