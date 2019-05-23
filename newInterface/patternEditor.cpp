


#include "mtFileManager.h"


#include "patternEditor.h"


cPatternEditor patternEditor;
cPatternEditor* PTE = &patternEditor;




uint8_t functPlayAction();
uint8_t functStopAction();
static  uint8_t functSwitchModule(uint8_t button);



void cPatternEditor::update()
{



}

void cPatternEditor::start(uint32_t options)
{

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
	//FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject, nullptr);

}
//==============================================================================================================

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
/*
uint8_t functShowProjectsList()
{
	SI->listOnlyFolderNames("/Projects/");

// lista
	SI->projectList.start = 0;
	SI->projectList.length = SI->locationFilesCount;
	SI->projectList.linesCount = 5;
	SI->projectList.data = SI->filesNames;

	display.setControlData(SI->fileListControl,  &SI->projectList);
	display.setControlShow(SI->fileListControl);
	display.refreshControl(SI->fileListControl);

// top label listy
	display.setControlText(SI->topLabel[0], "Open project");
	display.setControlShow(SI->topLabel[0]);
	display.refreshControl(SI->topLabel[0]);

// bottom labels
	display.setControlText(SI->bottomLabel[0], "Open");
	display.setControlText(SI->bottomLabel[1], "Cancel");

	display.refreshControl(SI->bottomLabel[0]);
	display.refreshControl(SI->bottomLabel[1]);

	display.synchronizeRefresh();

// funkcje
	SI->FM->clearAllButtons();
	SI->FM->clearAllPots();

	SI->FM->setPotObj(interfacePot0, &SI->selectedLocation, 0, SI->locationFilesCount-1, 1, SI->fileListControl);

	SI->FM->setButtonObj(interfaceButton0, buttonPress, functOpenProject, nullptr);
	SI->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList, nullptr);


	return 0;
}


uint8_t functShowTemplatesList()
{
	SI->listOnlyFolderNames("/Templates/");

// lista
	SI->projectList.start = 0;
	SI->projectList.length = SI->locationFilesCount;
	SI->projectList.linesCount = 5;
	SI->projectList.data = SI->filesNames;

	display.setControlData(SI->fileListControl,  &SI->projectList);
	display.setControlShow(SI->fileListControl);
	display.refreshControl(SI->fileListControl);

// top label listy
	display.setControlText(SI->topLabel[0], "Choose template");
	display.setControlShow(SI->topLabel[0]);
	display.refreshControl(SI->topLabel[0]);

// bottom labels
	display.setControlText(SI->bottomLabel[0], "Create");
	display.setControlText(SI->bottomLabel[1], "Cancel");
	display.setControlText(SI->bottomLabel[4], "New");

	display.refreshControl(SI->bottomLabel[0]);
	display.refreshControl(SI->bottomLabel[1]);
	display.refreshControl(SI->bottomLabel[4]);

	display.synchronizeRefresh();

// funkcje
	SI->FM->clearAllButtons();
	SI->FM->clearAllPots();

	SI->FM->setPotObj(interfacePot0, &SI->selectedLocation, 0, SI->locationFilesCount-1, 1, SI->fileListControl);

	SI->FM->setButtonObj(interfaceButton0, buttonPress, functOpenTemplate, nullptr);
	SI->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList, nullptr);
	SI->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate, nullptr);

	return 0;
}

uint8_t functCancelList()
{
	SI->showDefaultScreen();

	return 0;
}

uint8_t functSaveProject()
{
	fileManager.saveProject();

	return 0;
}

uint8_t functOpenProject()
{

	fileManager.openProject(&SI->locationFilesList[SI->selectedLocation][0],projectTypeUserMade);
	//SI->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);


	SI->showDefaultScreen();

	return 0;
}

uint8_t functOpenTemplate()
{
	fileManager.openProject(&SI->locationFilesList[SI->selectedLocation][0],projectTypeExample);
	//loadSamplesBank();
	fileManager.saveAsProject((char*)"fromTemplate");
	fileManager.openProject((char*)"fromTemplate", projectTypeUserMade);
	//loadSamplesBank();
	//SI->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);

	SI->showDefaultScreen();


	return 0;
}

uint8_t functCreateNewTemplate()
{
	fileManager.createEmptyTemplateProject((char*)"New");
	functShowTemplatesList();

	return 0;
}

*/
//======================================================================================================================


