
#include <modulesBase.h>

#include "mtTest.h"
#include "FT812.h"

cTest mtTest;
static cTest* TP = &mtTest;

static uint8_t functButtons(uint8_t button, uint8_t state);
static uint8_t functEncoder(int16_t value);
static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);



const char checkList[checkCount][50] =
{
		"Start",
		"Screen",
		"Inputs",
		"USB Communication",
		"MIDI",
		"Audio",
		"SD Card",



		"Finished",
};



//==========================================================================================
//
//==========================================================================================
void cTest::runTestingProcedure(cFunctionMachine* _fm, void (*func)(uint8_t, void*, void*, void*))
{
	FM = _fm;
	eventFunct = func;

	display.disable();
	display.clear();


	for(uint8_t button = 0; button<interfaceButtonsCount; button++)
	{
		FM->setButtonObj(button, functButtons);
	}

	FM->setPotObj(interfacePot0, functEncoder, nullptr);
	FM->setPadsGlobal(functPads);



	procedureRunning = 1;
}

//==========================================================================================
//
//==========================================================================================
void cTest::testLoop()
{
	if(!procedureRunning) return;

// Graficzne
	drawGui();

// Funkcyjne



}

void cTest::drawGui()
{
	if(guiRefresh < 100) return;
	guiRefresh = 0;

	API_LIB_BeginCoProList();
    API_CMD_DLSTART();





	showStatus();

	switch(mainStatus)
	{
	case checkStart: 	showStart(); 		break;
	case checkScreen: 	showScreenTest(); 	break;
	case checkInputs:	break;
	case checkUSB:		break;
	case checkMidi:		break;
	case checkAudio:	break;
	case checkSd:		break;

	case checkEnd:		showEnd();			break;
	}



    API_DISPLAY();
    API_CMD_SWAP();

	API_LIB_EndCoProList();

}

//==========================================================================================
//
//==========================================================================================
void cTest::showStatus()
{
	API_VERTEX_FORMAT(0);
	API_CLEAR_COLOR(0);
	API_CLEAR(1,1,1);

	API_COLOR(0xFFFFFF);

	API_CMD_TEXT(30,30,28,0,"Polyend Tracker testing procedure");

	for(uint8_t step = 0 ; step<checkCount; step++)
	{
		if(step < mainStatus) 			API_COLOR(0x00FF00);
		else if(step  == mainStatus) 	API_COLOR(0xFFFF00);
		else 							API_COLOR(0xFF0000);
		API_CMD_TEXT(50,80+step*25,28,0, &checkList[step][0]);
	}


	API_COLOR(0xFFFFFF);
	API_BEGIN(LINES);

	API_VERTEX2F(0,330);
	API_VERTEX2F(400,330);

	API_VERTEX2F(400,0);
	API_VERTEX2F(400,479);

	API_END();

}


void cTest::showMessage(char* question1, char* question2, char* answer1, char* answer2)
{
	API_COLOR(0xFFFFFF);
	API_CMD_TEXT(10,350,28,0,question1);
	API_CMD_TEXT(10,380,28,0,question2);

	API_CMD_TEXT(10,450,28,0,answer1);
	API_CMD_TEXT(300,450,28,0,answer2);

}


//==========================================================================================
//
//==========================================================================================
void cTest::showStart()
{
	showMessage("Pres run to start test", "", "Run", "");


}

void cTest::showEnd()
{

}

//==========================================================================================
//
//==========================================================================================
void cTest::showScreenTest()
{


}

void cTest::runScreenTest()
{
	if(testStatus == 0)
	{
		testTimer = 0;
	}
}




//==========================================================================================
//
//==========================================================================================
void cTest::AcceptButton()
{
	switch(mainStatus)
	{
	case checkStart:
	{
		mainStatus++;
		break;
	}
	case checkScreen:
	{

		mainStatus++;
		break;
	}
	}



}

void cTest::DeclineButton()
{

}
//==========================================================================================
//
//==========================================================================================
uint8_t cTest::runTestByCombinaion(uint8_t pad)
{
	if(startCode[pos++] != pad)
	{
		pos = 0;
	}
	else if(pos == 5)
	{
		pos = 0;
		return 1;
	}





	return 0;
}

//==========================================================================================
//
//==========================================================================================
static uint8_t functButtons(uint8_t button, uint8_t state)
{
	if(state != buttonPress) return 1;

	if(button == 0) TP->AcceptButton();
	else if(button == 2) TP->DeclineButton();








	return 1;
}

static uint8_t functEncoder(int16_t value)
{

	return 1;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{

	return 1;
}

