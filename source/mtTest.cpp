
#include "core/modulesBase.h"
#include "mtPadsBacklight.h"
#include "FT812.h"
#include "MIDI.h"
#include "mtMidi.h"
#include "mtSleep.h"
#include "keyScanner.h"
#include "Si4703.h"
#include <Audio.h>
#include  "mtRecorder.h"

#include "mtTest.h"

#include "debugLog.h"

cTest mtTest;
static cTest* TP = &mtTest;

extern AudioControlSGTL5000 audioShield;

static uint8_t functButtons(uint8_t button, uint8_t state);
static uint8_t functEncoder(int16_t value);
static uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);
static uint8_t functPowerButton(uint8_t state);

static void receiveTestNoteOn(byte channel, byte pitch, byte velocity);
static void radioSeek();

const char checkList[checkCount][20] =
{
		"Start",
		"Screen",
		"Inputs",
		"USB/SD",
		"MIDI",
		"Audio",
		"Radio",

		"Finished",
};



//==========================================================================================
//
//==========================================================================================
void cTest::runTestingProcedure(cFunctionMachine* _fm, void (*func)(uint8_t, void*, void*, void*))
{
	FM = _fm;
	eventFunct = func;

	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
	}

	radio.setFrequency(93.7);

	radio.setSeekCallback(radioSeek);
	tactButtons.setHoldTime(1200);
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

	mainStatus = checkStart;
	//mainStatus = checkAudio;//checkInputs;
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
	case checkUSB:		showUSBTest();		break;
	case checkMidi:		showMidiTest();		break;
	case checkRadio:	showRadioTest();	break;
	case checkAudio:	showAudioTest();	break;


	case checkEnd:		showEnd();			break;
	}



	debugLog.processLog();

    API_DISPLAY();
    API_CMD_SWAP();

	API_LIB_EndCoProList();
}


void cTest::doTasks()
{
	switch(mainStatus)
	{
	case checkStart: 		break;
	case checkScreen:	runScreenTest();	break;
	case checkInputs: 	runInputsTest();	break;
	case checkUSB:			break;
	case checkMidi:		runMidiTest();		break;
	case checkRadio:	runRadioTest();		break;
	case checkAudio:	runAudioTest();		break;

	case checkEnd:			break;
	}
}



//==========================================================================================
//
//==========================================================================================
void cTest::showStart()
{
	showMessage("Pres run to start the test", "", "Run", "");
}

void cTest::showEnd()
{
	showMessage("Test Done", "Sending data to Polyend...", "Restart device", "Run again");
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
	else if(testPhase%3 == 2) showMessage("Is screen OK?", "", "Yes", "No");

	else if(testPhase == lastPhase)
	{
		showMessage("Is screen OK?", "", "Yes", "No");
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

void cTest::runMidiTest()
{
	if(testPhase == 0)
	{
		midiCounter = 0;

		midiResults = 0;

		MIDI.setHandleNoteOn(receiveTestNoteOn);
	}
	else if(testPhase == 1)
	{
		testTimer = 0;

		for(uint8_t i = 0; i<8; i++)
		{
			MIDI.sendNoteOn(midiNotes[i], midiVelos[i], midiChannels[i]);
		}

		testPhase = 2;
	}
	else if(testPhase == 2)
	{
		if(midiResults == 255) nextTest();

		if(testTimer > 3000)  testPhase = 3;
	}
	else if(testPhase == 3)
	{

	}
}

void cTest::runRadioTest()
{
	if(testPhase == 0)
	{
	}
	else if(testPhase == 1)
	{
		radioSeek();
		setAudiIO(2);
		testPhase = 2;
		testTimer = 0;
	}
	else if(testPhase == 2)
	{
		if(testTimer > 3000)
		{
			radio.seekUp();
			testTimer = 0;
		}
		radio.stateMachineSeek();
	}
	else if(testPhase == 3)
	{
		setAudiIO(0);
		nextTest();
	}
}

void cTest::runAudioTest()
{
	if(testPhase == 1)
	{
		setAudiIO(0);
		delay(1);

		SdFile wavHeader = SD.open("/line_in_test.wav");
		if(wavHeader)
		{
			wavHeader.close();
			playSdFile("/line_in_test.wav");
		}
		else
		{
			engine.startTestSignal(0.1, 100);
		}

		testTimer = 0;
		recorder.startRecording(sdram_effectsBank);
		testPhase = 2;
	}
	else if(testPhase == 2 || testPhase == 5)
	{
		if(testTimer > 2000)
		{
			recorder.stopRecording();
			stopPlayingSdFile();
			engine.stopTestSignal();
			testPhase++;
		}
	}
	if(testPhase == 4)
	{
		setAudiIO(1);
		delay(1);

		SdFile wavHeader = SD.open("/mic_test.wav");
		if(wavHeader)
		{
			wavHeader.close();
			playSdFile("/mic_test.wav");
		}
		else
		{
			engine.startTestSignal(0.1, 1000);
		}

		testTimer = 0;
		recorder.startRecording(sdram_effectsBank+200000);
		testPhase = 5;
	}

	else if(testPhase == 7)
	{
		instrumentPlayer[0].noteOff();
		instrumentPlayer[0].noteOnforPrev(sdram_effectsBank, 70000, mtSampleTypeWaveFile);
		testTimer = 0;
		testPhase = 8;
	}
	else if(testPhase == 8)
	{
		if(testTimer > 1600)
		{
			instrumentPlayer[0].noteOff();
			instrumentPlayer[0].noteOnforPrev(sdram_effectsBank, 70000, mtSampleTypeWaveFile);
			testTimer = 0;
		}
	}
	else if(testPhase == 9)
	{
		instrumentPlayer[0].noteOff();
		instrumentPlayer[0].noteOnforPrev(sdram_effectsBank+200000, 70000, mtSampleTypeWaveFile);
		testTimer = 0;
		testPhase = 10;
	}
	else if(testPhase == 10)
	{
		if(testTimer > 1600)
		{
			instrumentPlayer[0].noteOff();
			instrumentPlayer[0].noteOnforPrev(sdram_effectsBank+200000, 70000, mtSampleTypeWaveFile);
			testTimer = 0;
		}
	}
	else if(testPhase == 11)
	{
		instrumentPlayer[0].noteOff();
		testPhase = 12;
	}
	else if(testPhase == 12)
	{

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
		showMessage("Press all buttons, pads and move encoder", "", "", "Hold to skip");
	}
	if(testPhase == 2)
	{
		showMessage("Is pads backlight ok?", "", "Yes", "");
	}
}

void cTest::showUSBTest()
{
	showMessage("Is MTP working?", "", "Yes", "No");
}


void cTest::showMidiTest()
{
	if(testPhase == 0)
	{
		showMessage("Connect MIDI cable", "", "Ready", "");
	}
	else if(testPhase == 1 || testPhase == 2)
	{
		showMessage("Testing MIDI", "", "", "");
	}
	else if(testPhase == 3)
	{
		showMessage("Test failed", "Is cable connected properly?", "Retry", "Skip");
	}
}



void cTest::showAudioTest()
{
	if(testPhase == 0)
	{
		showMessage("Connect audio output to line input", "", "Ready", "");
	}
	else if(testPhase == 1 || testPhase == 2)
	{
		showMessage("Recording in progress", "Wait...", "", "");
	}
	else if(testPhase == 3)
	{
		showMessage("Connect audio output to mic input", "", "Ready", "");
	}
	else if(testPhase == 4 || testPhase == 5)
	{
		showMessage("Recording in progress", "Wait...", "", "");
	}
	else if(testPhase == 6)
	{
		showMessage("Connect Headphones", "", "Ready", "");
	}
	else if(testPhase == 7 || testPhase == 8)
	{
		showMessage("Listen and chcek quality of recording", "-line in", "All good", "");
	}
	else if(testPhase == 9 || testPhase == 10)
	{
		showMessage("Listen and chcek quality of recording", "-mic", "All good", "");
	}
	else if(testPhase == 11 || testPhase == 12)
	{
		showMessage("Audio test complited", "", "Ok", "Retry");
	}
}

void cTest::showRadioTest()
{
	if(testPhase == 0)
	{
		showMessage("Connect Headphones", "", "Connected", "");
	}
	else if(testPhase == 1 || testPhase == 2)
	{
		showMessage((char*)"Is radio Quailty ok?", radioFreq, (char*)"Yes", (char*)"");
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
	padsBacklight.clearAllPads(1, 1, 1);
	if(mainStatus < checkEnd)  mainStatus++;
	testPhase = 0;
	hideStatus  = 0;

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
		else if(testPhase >= 2) nextTest();
		break;
	}
	case checkUSB:
	{
		nextTest();
		break;
	}
	case checkMidi:
	{
		if(testPhase == 1) break;
		if(testPhase < 3) testPhase++;
		else if(testPhase >= 3) testPhase = 0; // nextTest();
		break;
	}
	case checkRadio:
	{
		if(testPhase < 4) testPhase++;
		else nextTest();
		break;
	}
	case checkAudio:
	{
		if(testPhase == 0 || testPhase == 3 || testPhase == 6 || testPhase == 7 || testPhase == 8 || testPhase == 9 || testPhase == 10) testPhase++;
		else if(testPhase == 11 || testPhase == 12) nextTest();
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
		else nextTest();
		break;
	}
	case checkInputs:
	{
		if(testPhase >= 2)
		{
			results[checkInputs] = 1;
			nextTest();
		}
		break;
	}
	case checkUSB:
	{
		results[checkUSB] = 1;
		nextTest();
		break;
	}
	case checkMidi:
	{
		if(testPhase >= 3)
		{
			results[checkMidi] = 1;
			nextTest();
		}
		break;
	}
	case checkRadio:
	{
		results[checkRadio] = 1;
		nextTest();
		break;
	}
	case checkAudio:
	{
		if(testPhase == 11 || testPhase == 12) testPhase = 0;
		break;
	}

	case checkEnd:
		mainStatus = checkStart;
		memset(&results,0,checkCount);
		testPhase = 0;
	break;
	}

}

void cTest::skipButton()
{
	if(blockSkip == 1) return;
	results[mainStatus] = 1;
	nextTest();
	blockSkip = 1;
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
	else if(pos == 4) // == 5
	{
		pos = 0;
		return 1;
	}
	return 0;
}

void cTest::systemReset()
{
	lowPower.resetMCU();
}


static void receiveTestNoteOn(byte channel, byte pitch, byte velocity)
{

	if(TP->midiNotes[TP->midiCounter] == pitch
		&& TP->midiVelos[TP->midiCounter] == velocity
		&& TP->midiChannels[TP->midiCounter] == channel)
	{
		TP->midiResults |=  (1 << TP->midiCounter);
	}


	TP->midiCounter++;

	if(TP->midiCounter >= 8) TP->midiCounter = 0;

}

static void radioSeek()
{
	sprintf(TP->radioFreq, "%.2f", radio.getFrequency());
}


void cTest::setAudiIO(uint8_t config)
{
	if(config == 0)
	{
		audioShield.inputSelect(0);
		digitalWrite(SI4703_KLUCZ,HIGH);
		audioShield.headphoneSourceSelect(0);
		audioShield.lineInLevel(15);
		engine.setHeadphonesVolume(70);
	}
	else if(config == 1)
	{
		//audioShield.muteHeadphone();
		audioShield.inputSelect(1);
		audioShield.headphoneSourceSelect(0);
		audioShield.micGain(20);
		engine.setHeadphonesVolume(70);
	}
	else if(config == 2)
	{
		audioShield.inputSelect(0);
		digitalWrite(SI4703_KLUCZ,LOW);
		audioShield.headphoneSourceSelect(1);
		audioShield.lineInLevel(15);
		engine.setHeadphonesVolume(70);
	}
}

void cTest::playSdFile(const char* filePath)
{
	stopPlayingSdFile();

	SdFile wavHeader = SD.open(filePath);

	if(!wavHeader)
	{
		wavHeader.close();
		SD.begin(SdioConfig(DMA_SDIO));
		return;
	}

	strWavFileHeader header;
	readHeader(&header,&wavHeader);
	wavHeader.close();

	if(header.AudioFormat == 3) playSdWavFloat.play(filePath);
	else
	{
		if(header.bitsPerSample == 16) playSdWav.play(filePath);
		else if (header.bitsPerSample == 24) playSdWav24Bit.play(filePath);
	}
}

void cTest::stopPlayingSdFile()
{
	playSdWav.stop();
	playSdWavFloat.stop();
	playSdWav24Bit.stop();
}

//==========================================================================================
//
//==========================================================================================
static uint8_t functButtons(uint8_t button, uint8_t state)
{

	if(state == buttonPress && TP->inputs.buttons[button] == 0) TP->inputs.buttons[button] = 1;
	else if(state == buttonRelease && TP->inputs.buttons[button] == 1) TP->inputs.buttons[button] = 2;

	if(button == 3 && state == buttonHold) TP->skipButton();
	if(button == 3 && state == buttonRelease) TP->blockSkip = 0;
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

