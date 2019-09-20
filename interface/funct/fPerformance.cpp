


#include "performanceMode.h"

#include "keyScanner.h"
#include "mtPadBoard.h"

#include "mtPadsBacklight.h"

#include "mtAudioEngine.h"

#include "interfacePopups.h"

#include "mtSequencer.h"



cPerformanceMode performanceMode;
static cPerformanceMode* PM = &performanceMode;




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



static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);




void cPerformanceMode::update()
{


	moduleRefresh = 0;
}

void cPerformanceMode::start(uint32_t options)
{
	moduleRefresh = 1;






	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchMode);


	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	setDefaultScreenFunct();


	if(mode == mtPerformanceMaster)
	{
		PM->showPerformanceMaster();
		PM->setPerformanceMaster();
	}
	else
	{
		PM->showPerformanceFxes();
		PM->setPerformanceFxes();
	}

}

void cPerformanceMode::stop()
{
	moduleRefresh = 0;
	mtPadBoard.releaseAllInstrument();
	padsBacklight.clearAllPads(1, 1, 1);
}

void cPerformanceMode::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);



	FM->setPadsGlobal(functPads);

}



//==============================================================================================================
void cPerformanceMode::setPerformanceMaster()
{

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);


	activateLabelsBorder();


	padsBacklight.clearAllPads(0, 1, 1);
}

void cPerformanceMode::setPerformanceFxes()
{

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	activateLabelsBorder();


	padsBacklight.clearAllPads(0, 1, 1);

}






//==============================================================================================================
static  uint8_t functEncoder(int16_t value)
{
	uint8_t mode_places = PM->selectedPlace[PM->mode] + PM->mode*10;

	switch(mode_places)
	{
	case 0:  		 break;

	}

	return 1;
}



//=========================================================================================================
static  uint8_t functLeft()
{
	if(PM->selectedPlace[PM->mode] > 0) PM->selectedPlace[PM->mode]--;
	PM->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(PM->selectedPlace[PM->mode] < PM->frameData.placesCount-1) PM->selectedPlace[PM->mode]++;
	PM->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	uint8_t mode_places = PM->selectedPlace[PM->mode] + PM->mode*10;

	switch(mode_places)
	{
	case 0:	 break;


	}

	return 1;
}


static  uint8_t functDown()
{
	uint8_t mode_places = PM->selectedPlace[PM->mode] + PM->mode*10;

	switch(mode_places)
	{
	case 0:    break;

	}


	return 1;
}



//=========================================================================================================
static  uint8_t functPlayAction()
{

	if (sequencer.getSeqState() == 0)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
		}
		else
		{
			sequencer.playPattern();
		}

		//PTE->lastPlayedPattern = 0;
	}
	else if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	return 1;
}




static  uint8_t functRecAction()
{


	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{

	PM->eventFunct(eventSwitchModule,PM,&button,0);

	return 1;
}


static  uint8_t functSwitchMode(uint8_t button)
{

	if(PM->mode == mtPerformanceMaster)
	{
		PM->mode = 1;
		PM->showPerformanceMaster();
		PM->setPerformanceMaster();
	}
	else
	{
		PM->mode = 0;
		PM->showPerformanceFxes();
		PM->setPerformanceFxes();
	}


	PM->activateLabelsBorder();

	return 1;
}


//##############################################################################################
//###############################          PAD BOARD           #################################
//##############################################################################################


void cPerformanceMode::lightUpPadBoard()
{
	padsBacklight.clearAllPads(0, 1, 1);


	if(mtProject.values.lastUsedInstrument >= 0 && mtProject.values.lastUsedInstrument <= 48)
	{
		padsBacklight.setBackLayer(1, 20, mtProject.values.lastUsedInstrument);
	}

}


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}

	if(state == 1)
	{
		//uint8_t note = mtPadBoard.convertPadToNote(pad);
		//if(note > 48) note = 48;
		//editorInstrument->tune = note;

		padsBacklight.setFrontLayer(1,20, pad);
		mtPadBoard.startInstrument(pad, mtProject.values.lastUsedInstrument,-1);

	}
	else if(state == 0)
	{
		padsBacklight.setFrontLayer(0,0, pad);
		mtPadBoard.stopInstrument(pad);

	}

	return 1;
}


