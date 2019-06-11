



#include <instrumentEditor.h>


cInstrumentEditor instrumentEditor;
static cInstrumentEditor* IE = &instrumentEditor;




static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functSwitchMode(uint8_t button);

static  uint8_t functSelectVolume(uint8_t button);
static  uint8_t functSelectFilter(uint8_t button);
static  uint8_t functSelectParams(uint8_t button);



static  uint8_t functSelectVolumeAttack();
static  uint8_t functSelectVolumeDecay();
static  uint8_t functSelectVolumeSustain();
static  uint8_t functSelectVolumeRelease();
static  uint8_t functSelectVolumeAmount();

static  uint8_t functSelectFilterAttack();
static  uint8_t functSelectFilterDecay();
static  uint8_t functSelectFilterSustain();
static  uint8_t functSelectFilterRelease();
static  uint8_t functSelectFilterAmount();
static  uint8_t functSelectFilterFilterType();
static  uint8_t functSelectFilterCutOff();
static  uint8_t functSelectFilterResonance();


static  uint8_t functSelectParamsVolume();
static  uint8_t functSelectParamsTune();
static  uint8_t functSelectParamsFineTune();
static  uint8_t functSelectParamsGlide();
static  uint8_t functSelectParamsVibrato();
static  uint8_t functSelectParamsTremolo();
static  uint8_t functSelectParamsEffect1();
static  uint8_t functSelectParamsEffect2();










void cInstrumentEditor::update()
{


	moduleRefresh = 0;
}

void cInstrumentEditor::start(uint32_t options)
{
	moduleRefresh = 1;


	mode = options;





	// ustawienie funkcji
	FM->setButtonObj(interfaceButton10, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton11, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton12, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton14, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton15, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton16, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButton23, buttonPress, functSwitchMode);
	FM->setButtonObj(interfaceButton24, buttonPress, functSwitchMode);
	FM->setButtonObj(interfaceButton25, buttonPress, functSwitchMode);


	switch(mode)
	{
	case 0:
	{
		IE->showInstrumentVolume();
		IE->setInstrumentVolumeFunct();
		break;
	}
	case 1:
	{
		IE->showInstrumentFilter();
		IE->setInstrumentFilterFunct();
		break;
	}
	case 2:
	{
		IE->showInstrumentParams();
		IE->setInstrumentParamsFunct();
		break;
	}
	}

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
	FM->setButtonObj(interfaceButton9, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);



}
//==============================================================================================================
void cInstrumentEditor::setInstrumentVolumeFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectVolume);
	}

}


void cInstrumentEditor::setInstrumentFilterFunct()
{

	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectFilter);
	}

}


void cInstrumentEditor::setInstrumentParamsFunct()
{
	for(uint8_t i = interfaceButton0; i < interfaceButton8; i++)
	{
		FM->setButtonObj(i, buttonPress, functSelectParams);
	}

}

static  uint8_t functSelectVolume(uint8_t button)
{
	IE->selectedPlace = button;
	IE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectFilter(uint8_t button)
{
	IE->selectedPlace = button;
	IE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectParams(uint8_t button)
{
	IE->selectedPlace = button;
	IE->activateLabelsBorder();

	return 1;
}



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


static  uint8_t functSwitchMode(uint8_t button)
{
	switch(button)
	{
	case 23:
	{
		if(IE->mode != 0)
		{
			IE->mode = 0;
			IE->showInstrumentVolume();
			IE->setInstrumentVolumeFunct();
		}
		break;
	}
	case 24:
	{
		if(IE->mode != 1)
		{
			IE->mode = 1;
			IE->showInstrumentFilter();
			IE->setInstrumentFilterFunct();
		}
		break;
	}
	case 25:
	{
		if(IE->mode != 2)
		{
			IE->mode = 2;
			IE->showInstrumentParams();
			IE->setInstrumentParamsFunct();
		}
		break;
	}


	}



	return 1;
}

//======================================================================================================================


