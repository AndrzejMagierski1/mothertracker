


#include "mtFileManager.h"



#include "projectEditor.h"


void sBegin() {};
void sUpdate() {};

/*
static cStateMachine::strSate cStateMachine::sates[5] =
{
		{sBegin,sUpdate},
		{sBegin,sUpdate},
		{sBegin,sUpdate},
		{sBegin,sUpdate},
		{sBegin,sUpdate},
};
*/

cProjectEditor projectEditor;
cProjectEditor* PE = &projectEditor;

cStateMachine<5> peSM;



uint8_t functShowProjectsList();
uint8_t functShowTemplatesList();
uint8_t functCancelList();
uint8_t functSaveProject();
uint8_t functOpenProject();
uint8_t functOpenTemplate();
uint8_t functCreateNewTemplate();





void cProjectEditor::update()
{



}

void cProjectEditor::start(uint32_t options)
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

		if(PE->topLabel[0] == nullptr) PE->topLabel[0] = display.createControl<cLabel>(&prop1);
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

void cProjectEditor::stop()
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

void cProjectEditor::showDefaultScreen()
{
	//lista
	display.setControlHide(PE->fileListControl);

	// top label listy
	//display.setControlText(PE->topLabel[0], "");
	display.setControlHide(PE->topLabel[0]);
	display.refreshControl(PE->topLabel[0]);

	// bottom labels
	display.setControlText(bottomLabel[0], "New");
	display.setControlText(bottomLabel[1], "Open");
	display.setControlText(bottomLabel[4], "Save");

	for(uint8_t i = 0; i<8; i++)
	{
		display.refreshControl(PE->bottomLabel[i]);
	}

	//funkcje
	FM->clearAllButtons();
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton1, buttonPress, functShowProjectsList, nullptr);
	FM->setButtonObj(interfaceButton0, buttonPress, functShowTemplatesList, nullptr);
	FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject, nullptr);

}
//==============================================================================================================

uint8_t functShowProjectsList()
{
	PE->listOnlyFolderNames("/Projects/");

// lista
	PE->projectList.start = 0;
	PE->projectList.length = PE->locationFilesCount;
	PE->projectList.linesCount = 5;
	PE->projectList.data = PE->filesNames;

	display.setControlData(PE->fileListControl,  &PE->projectList);
	display.setControlShow(PE->fileListControl);
	display.refreshControl(PE->fileListControl);

// top label listy
	display.setControlText(PE->topLabel[0], "Open project");
	display.setControlShow(PE->topLabel[0]);
	display.refreshControl(PE->topLabel[0]);

// bottom labels
	display.setControlText(PE->bottomLabel[0], "Open");
	display.setControlText(PE->bottomLabel[1], "Cancel");

	display.refreshControl(PE->bottomLabel[0]);
	display.refreshControl(PE->bottomLabel[1]);

// funkcje
	PE->FM->clearAllButtons();
	PE->FM->clearAllPots();

	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenProject, nullptr);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList, nullptr);


	return 0;
}


uint8_t functShowTemplatesList()
{
	PE->listOnlyFolderNames("/Templates/");

// lista
	PE->projectList.start = 0;
	PE->projectList.length = PE->locationFilesCount;
	PE->projectList.linesCount = 5;
	PE->projectList.data = PE->filesNames;

	display.setControlData(PE->fileListControl,  &PE->projectList);
	display.setControlShow(PE->fileListControl);
	display.refreshControl(PE->fileListControl);

// top label listy
	display.setControlText(PE->topLabel[0], "Choose template");
	display.setControlShow(PE->topLabel[0]);
	display.refreshControl(PE->topLabel[0]);

// bottom labels
	display.setControlText(PE->bottomLabel[0], "Create");
	display.setControlText(PE->bottomLabel[1], "Cancel");
	display.setControlText(PE->bottomLabel[4], "New");

	display.refreshControl(PE->bottomLabel[0]);
	display.refreshControl(PE->bottomLabel[1]);
	display.refreshControl(PE->bottomLabel[4]);


// funkcje
	PE->FM->clearAllButtons();
	PE->FM->clearAllPots();

	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenTemplate, nullptr);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList, nullptr);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate, nullptr);


	return 0;
}

uint8_t functCancelList()
{
	PE->showDefaultScreen();

	return 0;
}

uint8_t functSaveProject()
{
	fileManager.saveProject();

	return 0;
}

uint8_t functOpenProject()
{

	fileManager.openProject(&PE->locationFilesList[PE->selectedLocation][0],projectTypeUserMade);
	PE->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);


	PE->showDefaultScreen();

	return 0;
}

uint8_t functOpenTemplate()
{
	fileManager.openProject(&PE->locationFilesList[PE->selectedLocation][0],projectTypeExample);
	//loadSamplesBank();
	fileManager.saveAsProject((char*)"fromTemplate");
	fileManager.openProject((char*)"fromTemplate", projectTypeUserMade);
	//loadSamplesBank();
	PE->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);

	PE->showDefaultScreen();


	return 0;
}

uint8_t functCreateNewTemplate()
{
	fileManager.createEmptyTemplateProject((char*)"New");
	functShowTemplatesList();

	return 0;
}


//======================================================================================================================


void cProjectEditor::listOnlyFolderNames(const char* folder)
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
