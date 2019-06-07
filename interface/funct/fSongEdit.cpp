

#include <songEditor.h>
//#include "mtFileManager.h"

#include "mtAudioEngine.h"


cSongEditor songEditor;

static cSongEditor* SE = &songEditor;





static  uint8_t functChangePattern(uint8_t button);


static  uint8_t functAddPattern();
static  uint8_t functRemovePattern();
static  uint8_t functSelectReverbSize();
static  uint8_t functSelectReverbDamping();
static  uint8_t functSelectLimiterAttack();
static  uint8_t functSelectLimiterTreshold();

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);

static  uint8_t functPlayAction();
static  uint8_t functRecAction();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);



void cSongEditor::update()
{



}

void cSongEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	selectedPattern = 0;


	showPatternsList();
	showReverbSize();
	showReverbDamping();
	showLimiterAttack();
	showLimiterTreshold();

	listPatterns();
	showPatternsList();

	//selectedPlace = 0;
	activateLabelsBorder();

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


}

void cSongEditor::stop()
{


	moduleRefresh = 0;
}

void cSongEditor::setDefaultScreenFunct()
{
	//funkcje
	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setButtonObj(interfaceButton8, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButton9, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);

	FM->setButtonObj(interfaceButton29, buttonPress, functEnter);
	FM->setButtonObj(interfaceButton33, buttonPress, functEnter);

	FM->setButtonObj(interfaceButton28, functShift);


	FM->setButtonObj(interfaceButton0, buttonPress, functAddPattern);
	FM->setButtonObj(interfaceButton1, buttonPress, functRemovePattern);

	FM->setButtonObj(interfaceButton2, buttonPress, functChangePattern);
	FM->setButtonObj(interfaceButton3, buttonPress, functChangePattern);

	FM->setButtonObj(interfaceButton4, buttonPress, functSelectReverbSize);
	FM->setButtonObj(interfaceButton5, buttonPress, functSelectReverbDamping);

	FM->setButtonObj(interfaceButton6, buttonPress, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton7, buttonPress, functSelectLimiterTreshold);



}


//==============================================================================================================

static  uint8_t functChangePattern(uint8_t button)
{
	if(button == interfaceButton2)
	{
		SE->changePatternsSelection(-1);
	}
	else //if(button == interfaceButton3)
	{
		SE->changePatternsSelection(1);
	}

	SE->selectedPlace = 0;
	SE->activateLabelsBorder();

	return 1;
}


static  uint8_t functAddPattern()
{




	SE->selectedPlace = 0;
	SE->activateLabelsBorder();
	return 1;
}

static  uint8_t functRemovePattern()
{



	SE->selectedPlace = 0;
	SE->activateLabelsBorder();
	return 1;
}

static  uint8_t functSelectReverbSize()
{
	SE->selectedPlace = 1;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbDamping()
{
	SE->selectedPlace = 2;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterAttack()
{
	SE->selectedPlace = 3;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterTreshold()
{
	SE->selectedPlace = 4;
	SE->activateLabelsBorder();

	return 1;
}



//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	switch(SE->selectedPlace)
	{
	case 0: SE->changePatternsSelection(value); break;
	case 1: SE->changeReverbRoomSize(value); break;
	case 2: SE->changeReverbDamping(value); break;
	case 3: SE->changeLimiterAttack(value); break;
	case 4: SE->changeLimiterTreshold(value); break;

	}


	return 1;
}

static  uint8_t functEnter()
{


	return 1;
}

static  uint8_t functShift(uint8_t state)
{



	return 1;
}


static  uint8_t functLeft()
{
	if(SE->selectedPlace > 0) SE->selectedPlace--;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	if(SE->selectedPlace < 4) SE->selectedPlace++;
	SE->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	switch(SE->selectedPlace)
	{
	case 0: SE->changePatternsSelection(-1); break;
	case 1: SE->changeReverbRoomSize(1); break;
	case 2: SE->changeReverbDamping(1); break;
	case 3: SE->changeLimiterAttack(1); break;
	case 4: SE->changeLimiterTreshold(1); break;
	}

	return 1;
}

static  uint8_t functDown()
{
	switch(SE->selectedPlace)
	{
	case 0: SE->changePatternsSelection(1); break;
	case 1: SE->changeReverbRoomSize(-1); break;
	case 2: SE->changeReverbDamping(-1); break;
	case 3: SE->changeLimiterAttack(-1); break;
	case 4: SE->changeLimiterTreshold(-1); break;
	}

	return 1;
}

static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0) sequencer.play();

	return 1;
}

static  uint8_t functStopAction()
{
	if(sequencer.getSeqState() == 1)
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

	SE->eventFunct(eventSwitchModule,SE,&button,0);

	return 1;
}


//======================================================================================================================
uint8_t cSongEditor::changePatternsSelection(int16_t value)
{
	if(selectedPattern + value < 0) selectedPattern = 0;
	else if(selectedPattern + value > songLength-1) selectedPattern = songLength-1;
	else  selectedPattern += value;

	display.setControlValue(patternsListControl, selectedPattern);
	display.refreshControl(patternsListControl);

	return 1;
}


void cSongEditor::changeReverbRoomSize(int16_t value)
{
	if(mtProject.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.values.reverbRoomSize += value;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);

	showReverbSize();

}

void cSongEditor::changeReverbDamping(int16_t value)
{
	if(mtProject.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.values.reverbDamping += value;

	engine.setReverbDamping(mtProject.values.reverbDamping);

	showReverbDamping();
}

void cSongEditor::changeLimiterAttack(int16_t value)
{
	if(mtProject.values.limiterAttak + value < LIMITER_ATTACK_MIN) mtProject.values.limiterAttak = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterAttak + value > LIMITER_ATTACK_MAX) mtProject.values.limiterAttak = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterAttak += value;

	//engine.setReverbRoomsize(mtProject.values.limiterAttak);

	showLimiterAttack();

}

void cSongEditor::changeLimiterTreshold(int16_t value)
{
	if(mtProject.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.values.limiterTreshold += value;

	//engine.setReverbDamping(mtProject.values.limiterTreshold);

	showLimiterTreshold();
}

//======================================================================================================================
//==============================================================================================
void cSongEditor::listPatterns()
{

	songLength = 1;
	strcpy(	&patternsNamesList[0][0], "             1                    1");
	patternNames[0] = &patternsNamesList[0][0];



}



