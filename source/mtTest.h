
#ifndef SOURCE_MTTEST_H_
#define SOURCE_MTTEST_H_

#include <stdint.h>

#include "elapsedMillis.h"

enum enChecklist
{
	checkStart,
	checkScreen,
	checkInputs,
	checkUSB,
	checkMidi,
	checkAudio,
	checkSd,


	checkEnd,

	checkCount
};


enum enTestPhases
{
	startPhase = 0,

	lastPhase = 255,
};

const uint8_t startCode[5] = {12,13,14,15,16};

class cFunctionMachine;

class cTest
{
public:

	void runTestingProcedure(cFunctionMachine* _fm, void (*func)(uint8_t, void*, void*, void*));
	void testLoop();

	uint8_t runTestByCombinaion(uint8_t pad);

	void AcceptButton();
	void DeclineButton();
	uint8_t getStatusViewState() {	return hideStatus; }

	struct strInputs
	{
		uint8_t pads[48];
		uint8_t buttons[33];
		uint8_t powerButton;
		uint8_t encoderL;
		uint8_t encoderR;

	} inputs;

private:
	void drawGui();
	void doTasks();

	void nextTest();

	void showStatus();
	void showMessage(char* question1, char* question2, char* answer1, char* answer2);
	void showMessage(const char* question1, const char* question2, const char* answer1, const char* answer2)
	{ showMessage((char*)question1, (char*)question2, (char*)answer1, (char*)answer2); }

	void showStart();
	void showEnd();

	void showScreenTest();
	void runScreenTest();

	void showInputsTest();
	void runInputsTest();

	// funct
	void systemReset();

	//
	cFunctionMachine* FM;
	void (*eventFunct)(uint8_t, void*, void*, void*);


	// ogolny
	elapsedMillis guiRefresh;
	uint8_t procedureRunning = 0;
	uint8_t mainStatus = 0;
	uint8_t results[checkCount] = {0,0,0,0,0,0,0,0};
	uint8_t hideStatus = 0;

	// konkretny aktualny test konkretnego elementu
	elapsedMillis testTimer;
	uint8_t testPhase = 0;




	//run pad combination
	uint8_t pos = 0;

	uint8_t ledBright = 0;


};

#define CM4_SCB_AIRCR 					(*(uint32_t*)0xE000ED0C)
#define CM4_SCB_AIRCR_VECTKEY_POS 		16
#define CM4_SCB_AIRCR_SYSRESETREQ_MASK	0x04U

extern cTest mtTest;

#endif /* SOURCE_MTTEST_H_ */
