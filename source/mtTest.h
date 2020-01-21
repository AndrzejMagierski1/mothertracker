
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
	checkRadio,
	checkAudio,

	checkEnd,

	checkCount
};


enum enTestPhases
{
	startPhase = 0,

	lastPhase = 255,
};

const uint8_t startCode[5] = {47,13,14,15,16};

class cFunctionMachine;

class cTest
{
public:

	void runTestingProcedure(cFunctionMachine* _fm, void (*func)(uint8_t, void*, void*, void*));
	void testLoop();

	uint8_t runTestByCombinaion(uint8_t pad);

	void AcceptButton();
	void DeclineButton();
	void skipButton();
	uint8_t blockSkip = 0;
	uint8_t getStatusViewState() {	return hideStatus; }


	struct strInputs
	{
		uint8_t pads[48];
		uint8_t buttons[33];
		uint8_t powerButton;
		uint8_t encoderL;
		uint8_t encoderR;

	} inputs;

	const uint8_t midiNotes[8] = {12,48,17,2,37,56,16,112};
	const uint8_t midiVelos[8] = {0,48,17,2,37,56,16,127};
	const uint8_t midiChannels[8] = {1,3,5,7,9,11,13,15};
	uint8_t midiResults = 0;
	uint8_t midiCounter = 0;

	// radio
	char radioFreq[10];

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

	void showUSBTest();

	void showMidiTest();
	void runMidiTest();

	void showAudioTest();
	void runAudioTest();

	void showRadioTest();
	void runRadioTest();


	// funct
	void systemReset();
	void setAudiIO(uint8_t config);
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




	// run pad combination
	uint8_t pos = 0;

	uint8_t ledBright = 0;

	// midi


};

#define CM4_SCB_AIRCR 					(*(uint32_t*)0xE000ED0C)
#define CM4_SCB_AIRCR_VECTKEY_POS 		16
#define CM4_SCB_AIRCR_SYSRESETREQ_MASK	0x04U

extern cTest mtTest;

#endif /* SOURCE_MTTEST_H_ */
