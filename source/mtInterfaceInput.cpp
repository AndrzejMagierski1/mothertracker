
#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"

#include "mtInterface.h"





//=======================================================================
void cMtInterface::potChange(uint8_t n, int16_t value)
{

	mtInstrumentEditor.potChange(n,value);
}

//=======================================================================
void cMtInterface::buttonChange(uint8_t n, uint8_t value)
{


}

//=======================================================================
void cMtInterface::powerButtonChange(uint8_t value)
{


}

//=======================================================================
void cMtInterface::padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{

	if(n = 3) setOperatingMode(mtOperatingModeInstrumentEditor);
}

//=======================================================================
void cMtInterface::padReleased(uint8_t n)
{


}

//=======================================================================
void cMtInterface::seqButtonPressed(uint8_t x, uint8_t y)
{


}

//=======================================================================
void cMtInterface::seqButtonReleased(uint8_t x, uint8_t y)
{


}

//=======================================================================
void cMtInterface::seqButtonHold(uint8_t x, uint8_t y)
{


}

//=======================================================================
void cMtInterface::seqButtonDouble(uint8_t x, uint8_t y)
{


}
