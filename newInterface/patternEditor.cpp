


#include "mtFileManager.h"


#include "patternEditor.h"


cPatternEditor patternEditor;
cPatternEditor* PTE = &patternEditor;



uint8_t functShowProjectsList();
uint8_t functShowTemplatesList();
uint8_t functCancelList();
uint8_t functSaveProject();
uint8_t functOpenProject();
uint8_t functOpenTemplate();
uint8_t functCreateNewTemplate();




void cPatternEditor::update()
{



}

void cPatternEditor::start(uint32_t options)
{

	// inicjalizacja kontrolek
	for(uint8_t i = 0; i<4; i++)
	{
		strControlProperties prop1;
		prop1.text = (char*)"";
		prop1.style = 	(/*controlStyleShow |*/ controlStyleCenterX);
		prop1.x = (800/4)*i+(800/8);
		prop1.y = 5;
		prop1.w = 800/4;
		prop1.h = 25;

		if(topLabel[0] == nullptr) topLabel[0] = display.createControl<cLabel>(&prop1);
	}

	for(uint8_t i = 0; i<8; i++)
	{
		strControlProperties prop2;
		prop2.text = (char*)"";
		prop2.style = 	(controlStyleShow | controlStyleBackground | controlStyleCenterX | controlStyleRoundedBorder);
		prop2.x = (800/8)*i+(800/16);
		prop2.y = 450;
		prop2.w = 800/8-10;
		prop2.h = 30;

		if(bottomLabel[i] == nullptr) bottomLabel[i] = display.createControl<cLabel>(&prop2);
	}


	projectList.linesCount = 5;
	projectList.start = 0;
	projectList.length = 0;
	strControlProperties prop;
	prop.x = 0;
	prop.y = 35;
	prop.w = (800/4);
	prop.h = 25;
	prop.data = &projectList;
	if(fileListControl == nullptr)  fileListControl = display.createControl<cList>(&prop);

	// ustawienie funkcji

	showDefaultScreen();

	//typedef void (cProjectEditor::*funct1) (void);
	//funct1 = &cProjectEditor::functOpenProject;
	//(this->*funct1)();

}

void cPatternEditor::stop()
{
	display.destroyControl(fileListControl);
	fileListControl = nullptr;

	for(uint8_t i = 0; i<4; i++)
	{
		display.destroyControl(topLabel[i]);
		topLabel[i] = nullptr;
	}

	for(uint8_t i = 0; i<8; i++)
	{
		display.destroyControl(bottomLabel[i]);
		bottomLabel[i] = nullptr;
	}


}

void cPatternEditor::showDefaultScreen()
{
	//lista
	display.setControlHide(fileListControl);

	// top label listy
	//display.setControlText(SI->topLabel[0], "");
	display.setControlHide(topLabel[0]);
	display.refreshControl(topLabel[0]);

	// bottom labels
	display.setControlText(bottomLabel[0], "New");
	display.setControlText(bottomLabel[1], "Open");
	display.setControlText(bottomLabel[4], "Save");

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(bottomLabel[i]);
	}

	display.synchronizeRefresh();

	//funkcje
	FM->clearAllButtons();
	FM->clearAllPots();

	//FM->setButtonObj(interfaceButton1, buttonPress, functShowProjectsList, nullptr);
	//FM->setButtonObj(interfaceButton0, buttonPress, functShowTemplatesList, nullptr);
	//FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject, nullptr);

}
//==============================================================================================================
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


void cPatternEditor::listOnlyFolderNames(const char* folder)
{
	strcpy(filePath, folder);
	strcat(filePath,"/");
	sdLocation.close();
	sdLocation.open(folder, O_READ); //"/"
	locationFilesCount = sdLocation.createFilesList(0,locationFilesList, files_list_length_max);
	sdLocation.close();


	uint8_t foundProjectsCount = 0;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		if(locationFilesList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(filePath, folder);
			strcat(filePath,&locationFilesList[i][0]); //doklej nazwe folderu

			sdLocation.open(filePath, O_READ);

			if(sdLocation.exists("project.bin"))	//tylko jesli w folderze jest plik projektu
			{
				strcpy(&locationFilesList[foundProjectsCount][0],&locationFilesList[i][1]);

				foundProjectsCount++;
			}


			sdLocation.close();
		}
	}


	locationFilesCount = foundProjectsCount;

	for(uint8_t i = 0; i < locationFilesCount; i++)
	{
		filesNames[i] = &locationFilesList[i][0];
	}

}
