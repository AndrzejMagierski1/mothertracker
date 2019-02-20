
#include "mtProjectEditor.h"
#include "mtInstrumentEditor.h"
#include "mtStepEditor.h"

#include "mtLED.h"


#include "mtInterface.h"





//=======================================================================
void cMtInterface::potChange(uint8_t n, int16_t value)
{
	switch(onScreenModule)
	{
		case mtModuleProjectEditor:		mtProjectEditor.potChange(n,value);		break;
		case mtModuleInstrumentEditor:	mtInstrumentEditor.potChange(n,value);	break;
		case mtModuleStepEditor:		mtStepEditor.potChange(n,value);		break;

		default : break;
	}


}

//=======================================================================
void cMtInterface::buttonChange(uint8_t n, uint8_t value)
{
	switch(onScreenModule)
	{
		case mtModuleProjectEditor:		mtProjectEditor.buttonChange(n,value);		break;
		case mtModuleInstrumentEditor:	mtInstrumentEditor.buttonChange(n,value);	break;
		case mtModuleStepEditor:		mtStepEditor.buttonChange(n,value);			break;

		default : break;
	}


}

//=======================================================================
void cMtInterface::powerButtonChange(uint8_t value)
{
	if(value == 1)
	{
		mtInterface.activateModule(mtModuleInstrumentEditor);
		mtInstrumentEditor.startExisting(0);
	}
}

//=======================================================================
void cMtInterface::padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo)
{
	switch(onScreenModule)
	{
		case mtModuleProjectEditor:		mtProjectEditor.padsChange(1,n,velo);		break;
		case mtModuleInstrumentEditor:	mtInstrumentEditor.padsChange(1,n,velo);	break;
		case mtModuleStepEditor:		mtStepEditor.padsChange(1,n,velo);			break;

		default : break;
	}



	leds.setLEDgrid(n+1,1,31);



	//if(n == 3) setOperatingMode(mtOperatingModeInstrumentEditor);
}

//=======================================================================
void cMtInterface::padReleased(uint8_t n)
{
	switch(onScreenModule)
	{
		case mtModuleProjectEditor:		mtProjectEditor.padsChange(0,n,0);		break;
		case mtModuleInstrumentEditor:	mtInstrumentEditor.padsChange(0,n,0);	break;
		case mtModuleStepEditor:		mtStepEditor.padsChange(0,n,0);			break;

		default : break;
	}

	leds.setLEDgrid(n+1,0,31);
}

//=======================================================================
void cMtInterface::seqButtonPressed(uint8_t x, uint8_t y)
{
	switch(onScreenModule)
	{
		case mtModuleProjectEditor:		mtProjectEditor.seqButtonChange(1,x,y);		break;
		case mtModuleInstrumentEditor:	mtInstrumentEditor.seqButtonChange(1,x,y);	break;
		case mtModuleStepEditor:		mtStepEditor.seqButtonChange(1,x,y);		break;

		default : break;
	}




	// TODO
	// zrobic stopowanie  edytoira instrumentu

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
