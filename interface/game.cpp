
#include "game.h"
#include "Arcanoid.h"
#include "mtAudioEngine.h"
#include "Encoder.h"

cGameModule gameModule;
static  cGameModule* GM = &gameModule;

int16_t prevEncValue;

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();




static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

void audio_handler(uint8_t type, uint32_t sampleNum);




void cGameModule::update()
{

	ARKANOID_gameLoop();



}



void cGameModule::start(uint32_t options)
{
	moduleRefresh = 1;

	display.clear();
	//display.disable();

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
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

	ARKANOID_setAudioHandler(audio_handler);

	Encoder.setResolution(96);
	Encoder.setAcceleration(1);

	for(int i=0;i<48;i++)
	{
		ARKANOID_updateSamplesLoaded(i,mtProject.instrument[i].isActive);
	}

}

void cGameModule::stop()
{
	//display.enable();
	moduleRefresh = 0;
	ARKANOID_resetAudioHandler();

	Encoder.setResolution(24);
	Encoder.setAcceleration(3);
}

void cGameModule::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();


	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);




	FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);




}

static uint8_t functSwitchModule(uint8_t button)
{

	GM->eventFunct(eventSwitchModule,GM,&button,0);

	return 1;
}



uint8_t functEncoder(int16_t value)
{
	if(value > 0)
	{
		ARKANOID_moveBarRight();
	}
	else if(value < 0)
	{
		ARKANOID_moveBarLeft();
	}

	prevEncValue=value;

	return 1;
}


static  uint8_t functEnter()
{
	ARKANOID_releaseFromPaddle();

	return 1;
}

static  uint8_t functShift(uint8_t state)
{
	if(state == buttonPress)
	{
		ARKANOID_gameStart();
	}
	else if(state == buttonRelease)
	{

	}

	return 1;
}


static  uint8_t functLeft()
{
	ARKANOID_moveBarLeft();
	return 1;
}

static  uint8_t functRight()
{
	ARKANOID_moveBarRight();
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
//0-48
void audio_handler(uint8_t type, uint32_t sampleNum)
{
	if(mtProject.instrument[sampleNum].isActive)
	{
		if(type ==7)
		{
			instrumentPlayer[0].noteOn(sampleNum, 47, 100);
		}
		else
		{
			instrumentPlayer[0].noteOn(sampleNum, 24, 100);
		}
	}
}






