
#include <modulesBase.h>
#include "mtPadsBacklight.h"
#include "mtTest.h"
#include "FT812.h"



cTest mtTest;
static cTest* TP = &mtTest;

static uint8_t functButtons(uint8_t button, uint8_t state);
static uint8_t functEncoder(int16_t value);
static uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);
static uint8_t functPowerButton(uint8_t state);


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
	memset(&results,0,checkCount);

	for(uint8_t button = 0; button<interfaceButtonsCount; button++)
	{
		FM->setButtonObj(button, functButtons);
	}

	FM->setPotObj(interfacePot0, functEncoder, nullptr);
	FM->setPadsGlobal(functPads);
	FM->setPowerButtonObj(functPowerButton);

	mainStatus = checkStart;//checkInputs;
	procedureRunning = 1;
}

//==========================================================================================
//
//==========================================================================================
void cTest::testLoop()
{
	if(!procedureRunning) return;

	if(guiRefresh < 100) return;
	guiRefresh = 0;

// Graficzne
	drawGui();

// Funkcyjne
	doTasks();

}

//==========================================================================================
//
//==========================================================================================
void cTest::drawGui()
{
	API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_VERTEX_FORMAT(0);
	API_CLEAR_COLOR(0x000000);
	API_CLEAR(1,1,1);



    if(!hideStatus) showStatus();

	switch(mainStatus)
	{
	case checkStart: 	showStart(); 		break;
	case checkScreen: 	showScreenTest(); 	break;
	case checkInputs:	showInputsTest();	break;
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


void cTest::doTasks()
{
	switch(mainStatus)
	{
	case checkStart: 		break;
	case checkScreen: runScreenTest();	break;
	case checkInputs: runInputsTest();	break;
	case checkUSB:			break;
	case checkMidi:			break;
	case checkAudio:		break;
	case checkSd:			break;

	case checkEnd:			break;
	}
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
	showMessage("Test Done", "", "Restart device", "Run again");
}

//==========================================================================================
//
//==========================================================================================
void cTest::showScreenTest()
{
	if(testPhase == 1)
	{
		API_CLEAR_COLOR(0x000000);
		API_CLEAR(1,1,1);


		API_CMD_GRADIENT(0, 0, 0x0, 800, 0, 0xff0000);

		//API_CMD_GRADIENT(0, 0, 0xffffff, 800, 0, 0x00ffff);
	}
	else if(testPhase == 4)
	{
		API_CLEAR_COLOR(0x000000);
		API_CLEAR(1,1,1);

		API_CMD_GRADIENT(0, 0, 0x0, 800, 0, 0x00ff00);
		//API_CMD_GRADIENT(0, 0, 0xffffff, 800, 0, 0xff00ff);
	}
	else if(testPhase == 7)
	{
		API_CLEAR_COLOR(0x000000);
		API_CLEAR(1,1,1);

		API_CMD_GRADIENT(0, 0, 0x0, 800, 0, 0x0000ff);

		//API_CMD_GRADIENT(0, 0, 0xffffff, 800, 0, 0xffff00);
	}
	else if(testPhase == 10)
	{
		API_CLEAR_COLOR(0x000000);
		API_CLEAR(1,1,1);

		API_CMD_GRADIENT(0, 0, 0x0, 800, 0, 0xffffff);
	}
	else if(testPhase%3 == 2) showMessage("Result?", "", "Ok", "Not ok");

	else if(testPhase == lastPhase)
	{
		showMessage("Result?", "", "Ok", "Not ok");
	}
}

void cTest::runScreenTest()
{
	if(testPhase == 11 || testPhase == lastPhase)
	{
		testTimer = 0;
		hideStatus  = 0;
		testPhase = lastPhase;
	}
	else if(testPhase%3 == 0)
	{
		testTimer = 0;
		hideStatus  = 1;
		testPhase++;
	}
	else if(testPhase%3 == 1  && testTimer > 2000)
	{
		testTimer = 0;
		hideStatus  = 1;
		testPhase++;
	}
	else if(testPhase%3 == 2)
	{
		testTimer = 0;
		hideStatus  = 0;
	}
}

//==========================================================================================
//
//==========================================================================================
void cTest::showInputsTest()
{
	API_BEGIN(RECTS);

	uint16_t baseX = 420, baseY = 320;

	ledBright++;
	if(ledBright > 31) ledBright= 0;

	// pady
	for(uint8_t i = 0; i<48; i++)
	{
		if(inputs.pads[i] == 0) API_COLOR(0xff0000);
		else if(inputs.pads[i] == 1) API_COLOR(0xffff00);
		else if(inputs.pads[i] == 2)
		{
			API_COLOR(0x00ff00);
			padsBacklight.setFrontLayer(1, ledBright, i);
		}
		API_VERTEX2F(baseX+((i%12)*18), baseY+(i/12)*18);
		API_VERTEX2F(baseX+((i%12)*18)+13, baseY+((i/12)*18)+13);
	}

	//buttony
	for(uint8_t i = 0; i<33; i++)
	{
		if(inputs.buttons[i] == 0) API_COLOR(0xff0000);
		else if(inputs.buttons[i] == 1) API_COLOR(0xffff00);
		else if(inputs.buttons[i] == 2) API_COLOR(0x00ff00);

		if(i<8)
		{
			API_VERTEX2F(baseX+((i)*27), baseY-40);
			API_VERTEX2F(baseX+((i)*27)+21, baseY-19);
		}
		else if(i<23)
		{
			uint8_t xy = i-8;
			API_VERTEX2F(baseX+230+((xy%5)*27), baseY-175+(xy/5)*27);
			API_VERTEX2F(baseX+230+((xy%5)*27)+21, baseY-175+((xy/5)*27)+21);
		}
		else
		{
			uint8_t xy = i-23;
			API_VERTEX2F(baseX+230+((xy%5)*27), baseY-75+(xy/5)*27);
			API_VERTEX2F(baseX+230+((xy%5)*27)+21, baseY-75+((xy/5)*27)+21);
		}
	}

	// power button
	if(inputs.powerButton == 0) API_COLOR(0xff0000);
	else if(inputs.powerButton == 1) API_COLOR(0xffff00);
	else if(inputs.powerButton == 2) API_COLOR(0x00ff00);
	API_VERTEX2F(baseX, baseY-195);
	API_VERTEX2F(baseX+15, baseY-195+15);
	API_END();

	//encoder
	API_BEGIN(FTPOINTS);
	API_POINT_SIZE(40*16);
	if(inputs.encoderL == 0 && inputs.encoderR == 1) API_COLOR(0xffff00);
	else if(inputs.encoderR == 0 && inputs.encoderL == 1) API_COLOR(0xffff00);
	else if(inputs.encoderR == 1 && inputs.encoderL == 1) API_COLOR(0x00ff00);
	else API_COLOR(0xff0000);
	API_VERTEX2F(baseX+69+230, baseY+36);
	API_END();

	if(testPhase == 0 || testPhase == 1)
	{
		showMessage("Press all buttons, pads and move encoder", "", "", "");
	}
	if(testPhase == 2)
	{
		showMessage("Pads backlight ok?", "", "Ok", "No ok");
	}
}

void cTest::runInputsTest()
{
	if(testPhase == 0)
	{
		memset(&inputs,0,sizeof(inputs));
		testPhase++;
	}
	if(testPhase == 1)
	{
		uint8_t input_result = 1;

		for(uint8_t i = 0; i<48; i++)
		{
			if(inputs.pads[i] < 2) input_result = 0;
		}
		for(uint8_t i = 0; i<33; i++)
		{
			if(inputs.buttons[i] < 2) input_result = 0;
		}
		if(inputs.powerButton < 2) input_result = 0;
		if(inputs.encoderL < 1) input_result = 0;
		if(inputs.encoderR < 1) input_result = 0;

		if(input_result == 1 ) testPhase++;
	}
	else if(testPhase == 2)
	{

	}
	else if(testPhase == 3)
	{
		testPhase = lastPhase;
	}

}

//==========================================================================================
//
//==========================================================================================
void cTest::showStatus()
{

	API_CLEAR_COLOR(0);
	API_CLEAR(1,1,1);

	API_COLOR(0xFFFFFF);

	API_CMD_TEXT(30,30,28,0,"Polyend Tracker testing procedure");

	for(uint8_t step = 0 ; step<checkCount; step++)
	{
		if(step > mainStatus) 			API_COLOR(0xFF0000);
		else if(step == mainStatus) 	API_COLOR(0xFFFF00);
		else if(results[step] == 1) 	API_COLOR(0xFF0000);
		else						 	API_COLOR(0x00FF00);

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
	API_CMD_TEXT(5,350,28,0,question1);
	API_CMD_TEXT(5,380,28,0,question2);

	API_CMD_TEXT(5,450,28,0,answer1);
	API_CMD_TEXT(400-5,450,28,OPT_RIGHTX,answer2);

}


void cTest::nextTest()
{
	 mainStatus++;
	 testPhase = 0;
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
		if(testPhase < lastPhase) testPhase++;
		else nextTest();
		break;
	}
	case checkInputs:
	{
		if(testPhase == 0) break;
		else if(testPhase == 1) break;
		else if(testPhase == 2) testPhase++;
		else if(testPhase < lastPhase) testPhase++;
		else nextTest();
		break;
	}
	case checkUSB:
	{

		mainStatus++;
		break;
	}
	case checkMidi:
	{

		mainStatus++;
		break;
	}
	case checkAudio:
	{

		mainStatus++;
		break;
	}
	case checkSd:
	{

		mainStatus++;
		break;
	}
	case checkEnd: systemReset(); break;
	}



}

void cTest::DeclineButton()
{
	switch(mainStatus)
	{
	case checkStart:
	{

		break;
	}
	case checkScreen:
	{
		results[checkScreen] = 1;
		if(testPhase < lastPhase) testPhase++;
		else mainStatus++;
		break;
	}
	case checkInputs:
	{

		break;
	}
	case checkUSB:
	{
		break;
	}
	case checkMidi:
	{
		break;
	}
	case checkAudio:
	{
		break;
	}
	case checkSd:
	{
		break;
	}

	case checkEnd:
		mainStatus = checkStart;
		memset(&results,0,checkCount);
		testPhase = 0;
	break;
	}

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
	else if(pos == 1) // == 5
	{
		pos = 0;
		return 1;
	}
	return 0;
}

void cTest::systemReset()
{
	__asm volatile ("dsb");
	CM4_SCB_AIRCR = (uint32_t)((0x5FAUL << CM4_SCB_AIRCR_VECTKEY_POS) | CM4_SCB_AIRCR_SYSRESETREQ_MASK);
	__asm volatile ("dsb");

	for(;;)
	{
		__asm volatile ("nop");
	}

}

//==========================================================================================
//
//==========================================================================================
static uint8_t functButtons(uint8_t button, uint8_t state)
{

	if(state == buttonPress && TP->inputs.buttons[button] == 0) TP->inputs.buttons[button] = 1;
	else if(state == buttonRelease && TP->inputs.buttons[button] == 1) TP->inputs.buttons[button] = 2;


	if(state != buttonPress) return 1;
	if(TP->getStatusViewState()) return 1;

	if(button == 0) TP->AcceptButton();
	else if(button == 3) TP->DeclineButton();

	return 1;
}

static uint8_t functEncoder(int16_t value)
{
	if(value > 0 && TP->inputs.encoderR == 0) TP->inputs.encoderR = 1;
	if(value < 0 && TP->inputs.encoderL == 0) TP->inputs.encoderL = 1;

	return 1;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == buttonPress && TP->inputs.pads[pad] == 0) TP->inputs.pads[pad] = 1;
	else if(state == buttonRelease && TP->inputs.pads[pad] == 1) TP->inputs.pads[pad] = 2;

	return 1;
}

static uint8_t functPowerButton(uint8_t state)
{
	if(state == buttonPress && TP->inputs.powerButton == 0) TP->inputs.powerButton = 1;
	else if(state == buttonRelease && TP->inputs.powerButton == 1) TP->inputs.powerButton = 2;

	return 1;
}

