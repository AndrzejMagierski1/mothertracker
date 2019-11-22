
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

const uint8_t startCode[5] = {12,13,14,15,16};

class cFunctionMachine;

class cTest
{
public:

	void runTestingProcedure(cFunctionMachine* _fm, void (*func)(uint8_t, void*, void*, void*));
	void testLoop();

	uint8_t runTestByCombinaion(uint8_t pad);

private:
	void drawGui();

	void showStatus();
	void showStart();
	void showEnd();

	void showScreenTest();

	//
	cFunctionMachine* FM;
	void (*eventFunct)(uint8_t, void*, void*, void*);


	//

	uint8_t testRunning = 0;
	uint8_t mainStatus = 0;

	//
	elapsedMillis guiRefresh;


	//run pad combination
	uint8_t pos = 0;




};


extern cTest mtTest;

#endif /* SOURCE_MTTEST_H_ */
