



#include "mtPadBoard.h"
#include "mtAudioEngine.h"


#include "configEditor.h"



cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();



static  uint8_t functConfigGroup(uint8_t button);



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);






void cConfigEditor::update()
{



}

void cConfigEditor::start(uint32_t options)
{
	moduleRefresh = 1;


//--------------------------------------------------------------------



//--------------------------------------------------------------------


	listConfigGroups();

	showConfigGroupList();


	// ustawienie funkcji
	FM->setButtonObj(interfaceButton8, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton9, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton10, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton11, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton12, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton14, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton15, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton16, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton22, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	activateLabelsBorder();
}


void cConfigEditor::stop()
{

	moduleRefresh = 0;
}


void cConfigEditor::setDefaultScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton23, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButton24, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);

/*
	FM->setButtonObj(interfaceButton0, buttonPress, functSelectStart);
	FM->setButtonObj(interfaceButton1, buttonPress, functSelectLoop1);
	FM->setButtonObj(interfaceButton2, buttonPress, functSelectLoop2);
	FM->setButtonObj(interfaceButton3, buttonPress, functSelectEnd);

	FM->setButtonObj(interfaceButton5, buttonPress, functSelectZoom);
*/

	FM->setButtonObj(interfaceButton6, buttonPress, functConfigGroup);
	FM->setButtonObj(interfaceButton7, buttonPress, functConfigGroup);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

//==============================================================================================================


void cConfigEditor::listConfigGroups()
{

	for(uint8_t i = 0; i < groupCount; i++)
	{
		configGroupsNames[i] = (char*)&groupNamesLabels[i][0];
	}


}



//==============================================================================================================





static  uint8_t functConfigGroup(uint8_t button)
{
	if(button == interfaceButton6)
	{
		CE->changeConfigGroupSelection(-1);
	}
	else //if(button == interfaceButton7)
	{
		CE->changeConfigGroupSelection(1);
	}

	CE->selectedPlace = 6;
	CE->activateLabelsBorder();


	return 1;
}

static  uint8_t functEncoder(int16_t value)
{


	switch(CE->selectedPlace)
	{
	case 0: break;
	case 1: break;
	case 2: break;
	case 3: break;
	case 5: break;
	case 6: CE->changeConfigGroupSelection(value);	break;
	}




	return 1;
}




static  uint8_t functLeft()
{
	if(CE->selectedPlace > 0) CE->selectedPlace--;

	switch(CE->selectedPlace)
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6:
		{
		CE->selectedPlace = 6;
		CE->activateLabelsBorder();

		break;
		}
	}


	return 1;
}


static  uint8_t functRight()
{
	if(CE->selectedPlace < CE->frameData.placesCount-1) CE->selectedPlace++;

	switch(CE->selectedPlace)
	{
		case 0: 	break;
		case 1: 	break;
		case 2: 	break;
		case 3: 	break;
		case 4: 	break;
		case 5: 	break;
		case 6:
		{
		CE->selectedPlace = 6;
		CE->activateLabelsBorder();
		break;
		}
	}



	return 1;
}


static  uint8_t functUp()
{
	switch(CE->selectedPlace)
	{
	case 0: 	break;
	case 1: 	break;
	case 2: 	break;
	case 3: 	break;
	case 5: 	break;
	case 6: CE->changeConfigGroupSelection(-1);	break;
	}

	return 1;
}


static  uint8_t functDown()
{
	switch(CE->selectedPlace)
	{
	case 0: 	break;
	case 1: 	break;
	case 2: 	break;
	case 3: 	break;
	case 5: 	break;
	case 6: CE->changeConfigGroupSelection(1);	break;
	}

	return 1;
}




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0)
	{
		sequencer.play();
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

	CE->eventFunct(eventSwitchModule,CE,&button,0);

	return 1;
}


//======================================================================================================================


void cConfigEditor::changeConfigGroupSelection(int16_t value)
{
	if(selectedConfigGroup + value < 0) selectedConfigGroup = 0;
	else if(selectedConfigGroup + value > groupCount-1) selectedConfigGroup = groupCount-1;
	else  selectedConfigGroup += value;


	display.setControlValue(configGroupsListControl, selectedConfigGroup);
	display.refreshControl(configGroupsListControl);

}



