


#include "imageViewer.h"

#include "keyScanner.h"
#include "mtPadBoard.h"

#include "mtPadsBacklight.h"






cImageViewer imageViewer;
static cImageViewer* IV = &imageViewer;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functCopy();
static uint8_t functShift(uint8_t value);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);
static  uint8_t functSwitchMode(uint8_t button);



static  uint8_t functInstrument(uint8_t state);
static uint8_t functStepNote(uint8_t value);



static uint8_t functActionButton(uint8_t button, uint8_t state);


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


void cImageViewer::update()
{



}

void cImageViewer::start(uint32_t options)
{
	moduleRefresh = 1;


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchMode);


	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();



}

void cImageViewer::stop()
{
	moduleRefresh = 0;
	mtPadBoard.releaseAllInstrument();
	padsBacklight.clearAllPads(1, 1, 1);
}

void cImageViewer::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButtonShift, functShift);

	FM->setButtonObj(interfaceButton0, functActionButton);
	FM->setButtonObj(interfaceButton1, functActionButton);
	FM->setButtonObj(interfaceButton2, functActionButton);
	FM->setButtonObj(interfaceButton3, functActionButton);
	FM->setButtonObj(interfaceButton4, functActionButton);
	FM->setButtonObj(interfaceButton5, functActionButton);
	FM->setButtonObj(interfaceButton6, functActionButton);
	FM->setButtonObj(interfaceButton7, functActionButton);


	FM->setPadsGlobal(functPads);

}




//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{



	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	return 1;
}


static  uint8_t functRight()
{

	return 1;
}


static  uint8_t functUp()
{


	return 1;
}


static  uint8_t functDown()
{



	return 1;
}



//=========================================================================================================
static  uint8_t functPlayAction()
{


	return 1;
}




static  uint8_t functRecAction()
{


	return 1;
}

static uint8_t functShift(uint8_t value)
{




	return 1;
}


static uint8_t functSwitchModule(uint8_t button)
{

	IV->eventFunct(eventSwitchModule,IV,&button,0);

	return 1;
}







//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################



static uint8_t functActionButton(uint8_t button, uint8_t state)
{

	if(state == buttonPress)
	{

	}
	else if(state == buttonRelease)
	{

	}





	return 1;
}


//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################




static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{

	if(state == 1)
	{

	}
	else if(state == 0)
	{



	}

	return 1;
}


