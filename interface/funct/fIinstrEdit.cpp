



#include <instrumentEditor.h>


cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;




static  uint8_t functPlayAction();
static  uint8_t functStopAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);



void cInstrumentEditor::update()
{


	moduleRefresh = 0;
}

void cInstrumentEditor::start(uint32_t options)
{
	moduleRefresh = 1;







	// ustawienie funkcji
	FM->setButtonObj(interfaceButton10, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton11, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton12, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton14, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton15, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton16, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	//typedef void (cProjectEditor::*funct1) (void);
	//funct1 = &cProjectEditor::functOpenProject;
	//(this->*funct1)();

}

void cInstrumentEditor::stop()
{



}

void cInstrumentEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton8, buttonPress, functPlayAction);
//	FM->setButtonObj(interfaceButton9, buttonPress, functStopAction);
	FM->setButtonObj(interfaceButton9, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);



	//FM->setPotObj(interfacePot0, (uint16_t*)(&trackerPattern.part), 0, 744, 5, patternControl);


	FM->setPotObj(interfacePot0, functEncoder, nullptr);



}
//==============================================================================================================






//==============================================================================================================


static  uint8_t functEncoder(int16_t value)
{



	//display.refreshControl(PTE->patternControl);

	return 1;
}





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




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		sequencer.play();
	}
	else if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
	{
		sequencer.stop();
	}



	return 1;
}


static  uint8_t functStopAction()
{
	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_PLAY)
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

	IE->eventFunct(eventSwitchModule,IE,&button,0);

	return 1;
}


//======================================================================================================================


