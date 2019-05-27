


#include "mtFileManager.h"


#include "patternEditor.h"


cPatternEditor patternEditor;
cPatternEditor* PTE = &patternEditor;




uint8_t functPlayAction();
uint8_t functStopAction();
uint8_t functLeft();
uint8_t functRight();
uint8_t functUp();
uint8_t functDown();

static  uint8_t functSwitchModule(uint8_t button);



void cPatternEditor::update()
{
	uint8_t sequencerState = sequencer.getSeqState();

	if(sequencerState != 1) return;

	readPatternState();

	if(patternPosition == lastPatternPosition)  return;

	refreshPattern();


	lastPatternPosition = patternPosition;




}

void cPatternEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	readPatternState();
	refreshPattern();

	// inicjalizacja kontrolek
/*
	for(uint8_t i = 0; i<4; i++)
	{
		strControlProperties prop1;
		prop1.text = (char*)"";
		prop1.style = 	( controlStyleCenterX);
		prop1.x = (800/4)*i+(800/8);
		prop1.y = 5;
		prop1.w = 800/4;
		prop1.h = 25;

		if(topLabel[0] == nullptr) topLabel[0] = display.createControl<cLabel>(&prop1);
	}
*/

	for(uint8_t i = 0; i<4; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.style = 	( controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/4)*i+(800/8);
		prop2.y = 450;
		prop2.w = 800/4-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}



	strControlProperties prop;
	prop.text = (char*)"Test";
	prop.style = 	(controlStyleShow );//| controlStyleFont2 | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
	prop.x = 0;
	prop.y = 0;
	prop.w = 50;
	prop.h = 25;
	prop.data = &trackerPattern;
	if(patternControl == nullptr)  patternControl = display.createControl<cTracker>(&prop);
	//hTrackControl = display.createControl<cLabel>(&prop);
	//display.refreshControl(hTrackControl);


	// ustawienie funkcji
	FM->clearAllButtons();
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);


	showDefaultScreen();

	//typedef void (cProjectEditor::*funct1) (void);
	//funct1 = &cProjectEditor::functOpenProject;
	//(this->*funct1)();

}

void cPatternEditor::stop()
{
	display.destroyControl(patternControl);
	patternControl = nullptr;
/*
	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}
*/
	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}

	FM->clearAllButtons();
	FM->clearAllPots();

}

void cPatternEditor::showDefaultScreen()
{
	//lista
	display.setControlShow(patternControl);
	display.refreshControl(patternControl);

	// top label listy
	//display.setControlText(SI->topLabel[0], "");
	//display.setControlHide(topLabel[0]);
	//display.refreshControl(topLabel[0]);

	// bottom labels
	display.setControlText(bottomLabel[0], "-   BPM: 120   +");
	display.setControlText(bottomLabel[1], "-  Pattern: 0  +");
	display.setControlText(bottomLabel[2], "-  Length: 16  +");
	display.setControlText(bottomLabel[3], "-    Step: 1   +");

	display.setControlShow(bottomLabel[0]);
	display.setControlShow(bottomLabel[1]);
	display.setControlShow(bottomLabel[2]);
	display.setControlShow(bottomLabel[3]);

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton8, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButton9, buttonPress, functStopAction);


	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);


	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);


	FM->setPotObj(interfacePot0, (uint16_t*)(&trackerPattern.part), 0, 744, 5, patternControl);


	//FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject, nullptr);

}
//==============================================================================================================

void cPatternEditor::refreshPattern()
{
	trackerPattern.length = patternLength;
	trackerPattern.position = patternPosition+1;
	//trackerPattern.part = patternPart;


	uint8_t steps_down = patternLength - patternPosition;
	steps_down = (steps_down < 8) ? steps_down : 8;

	uint8_t steps_up = (patternPosition < 7) ? patternPosition : 7;



	for(uint8_t i = 0; i < 8; i++) //track
	{
		for(uint8_t j = 0; j < 15; j++) // step
		{

			if(j-6 > steps_down || j<7-steps_up)
			{
				trackerPattern.track[i].row[j].note[0] = 0;
				trackerPattern.track[i].row[j].instr[0] = 0;
				trackerPattern.track[i].row[j].vol[0] = 0;
				trackerPattern.track[i].row[j].fx[0] = 0;
				continue;
			}

			if(seq->track[i].step[patternPosition-7+j].isOn)
			{
				trackerPattern.track[i].row[j].note[0] = mtNotes[seq->track[i].step[patternPosition-7+j].note][0];
				trackerPattern.track[i].row[j].note[1] = mtNotes[seq->track[i].step[patternPosition-7+j].note][1];
				trackerPattern.track[i].row[j].note[2] = mtNotes[seq->track[i].step[patternPosition-7+j].note][2];
				trackerPattern.track[i].row[j].note[3] = mtNotes[seq->track[i].step[patternPosition-7+j].note][3];
				trackerPattern.track[i].row[j].note[4] = mtNotes[seq->track[i].step[patternPosition-7+j].note][4];


				char inst0 = (seq->track[i].step[patternPosition-7+j].instrument+1)/10;
				char inst1 = (seq->track[i].step[patternPosition-7+j].instrument+1)%10;;

				trackerPattern.track[i].row[j].instr[0] = inst0+48;
				trackerPattern.track[i].row[j].instr[1] = inst1+48;
				trackerPattern.track[i].row[j].instr[2] = 0;


				char velo0 = seq->track[i].step[patternPosition-7+j].velocity/1000;
				char velo1 = seq->track[i].step[patternPosition-7+j].velocity%10;

				trackerPattern.track[i].row[j].vol[0] = velo0+48;
				trackerPattern.track[i].row[j].vol[1] = velo1+48;
				trackerPattern.track[i].row[j].vol[0] = 0;

			}
			else
			{
				trackerPattern.track[i].row[j].note[0] = '-';
				trackerPattern.track[i].row[j].note[1] = '-';
				trackerPattern.track[i].row[j].note[2] = '-';
				trackerPattern.track[i].row[j].note[3] = 0;


				trackerPattern.track[i].row[j].instr[0] = '-';
				trackerPattern.track[i].row[j].instr[1] = '-';
				trackerPattern.track[i].row[j].instr[2] = 0;


				trackerPattern.track[i].row[j].vol[0] = '-';
				trackerPattern.track[i].row[j].vol[1] = '-';
				trackerPattern.track[i].row[j].vol[2] = 0;

			}

			if(seq->track[i].step[patternPosition-7+j].fx[0].isOn)
			{
				trackerPattern.track[i].row[j].fx[0] = seq->track[i].step[patternPosition-7+j].fx[0].type + 59;
				trackerPattern.track[i].row[j].fx[1] = '0';
				trackerPattern.track[i].row[j].fx[2] = '0';
				trackerPattern.track[i].row[j].fx[3] = 0;
			}
			else
			{
				trackerPattern.track[i].row[j].fx[0] = '-';
				trackerPattern.track[i].row[j].fx[1] = '-';
				trackerPattern.track[i].row[j].fx[2] = '-';
				trackerPattern.track[i].row[j].fx[3] = 0;
			}

		}
	}

	display.refreshControl(patternControl);

}



void cPatternEditor::readPatternState()
{
	seq = sequencer.getPatternToUI();

	patternLength = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		if(seq->track[i].isOn)
		{
			if(patternLength < seq->track[i].length) patternLength = seq->track[i].length;
		}
	}


	patternPosition = sequencer.ptrPlayer->row[0].actual_pos;




}


//==============================================================================================================

uint8_t functLeft()
{
	PTE->trackerPattern.part -= 186;
	if(PTE->trackerPattern.part < 0 ) PTE->trackerPattern.part = 0;

	display.refreshControl(PTE->patternControl);


	return 1;
}


uint8_t functRight()
{
	PTE->trackerPattern.part += 186;
	if(PTE->trackerPattern.part > 744 ) PTE->trackerPattern.part= 744;

	display.refreshControl(PTE->patternControl);

	return 1;
}


uint8_t functUp()
{
	//PTE->readPatternState();


	if(PTE->patternPosition > 0 ) PTE->patternPosition--;

	PTE->refreshPattern();

	return 1;
}


uint8_t functDown()
{
	//PTE->readPatternState();

	if(PTE->patternPosition <  PTE->patternLength-1) PTE->patternPosition++;

	PTE->refreshPattern();

	return 1;
}


uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0) sequencer.play();

	return 1;
}


uint8_t functStopAction()
{
	if(sequencer.getSeqState() == 1) sequencer.stop();

	return 1;
}


static uint8_t functSwitchModule(uint8_t button)
{

	PTE->eventFunct(eventSwitchModule,PTE,&button,0);

	return 1;
}


//======================================================================================================================


