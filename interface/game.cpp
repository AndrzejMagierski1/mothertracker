
#include "game.h"
#include "Arcanoid.h"
#include "mtAudioEngine.h"
#include "Encoder.h"
#include "mtSequencer.h"

cGameModule gameModule;
static  cGameModule* GM = &gameModule;

//int16_t prevEncValue;

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();




static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

static void audio_handler(uint8_t pitch, uint8_t sampleNum);
static void killGameSounds();







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

	Encoder.setResolution(24);
	Encoder.setAcceleration(3);

	for(int i=0;i<48;i++)
	{
		ARKANOID_updateSamplesLoaded(i,mtProject.instrument[i].isActive);
	}

	if(sequencer.getSeqState() == 1)
	{
	   sequencer.stop();
	}
}

void cGameModule::stop()
{
	//display.enable();
	moduleRefresh = 0;
	ARKANOID_resetAudioHandler();
	ARKANOID_pauseControl(pause);
	killGameSounds();

	Encoder.setResolution(24);
	Encoder.setAcceleration(0);
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
		ARKANOID_moveBarRight(value);
	}
	else if(value < 0)
	{
		ARKANOID_moveBarLeft(-(value));
	}

	//prevEncValue=value;

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
	//ARKANOID_moveBarLeft();
	return 1;
}

static  uint8_t functRight()
{
	//ARKANOID_moveBarRight();
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

static void audio_handler(uint8_t pitch, uint8_t sampleNum)
{
	static uint8_t instrumentNum;

	if(mtProject.instrument[sampleNum].isActive)
	{
		instrumentPlayer[instrumentNum].noteOn(sampleNum, pitch, 100);

		instrumentNum++;
		instrumentNum = instrumentNum % 8;
	}
}

static void killGameSounds()
{
	for(uint32_t i = 0; i<8; i++)
	{
		instrumentPlayer[i].noteOff();
	}
}






