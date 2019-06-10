


#include <projectEditor.h>
#include "mtFileManager.h"






cProjectEditor projectEditor;
cProjectEditor* PE = &projectEditor;



uint8_t functShowProjectsList();
uint8_t functShowTemplatesList();
uint8_t functCancelList();
uint8_t functSaveProject();
uint8_t functOpenProject();
uint8_t functOpenTemplate();
uint8_t functCreateNewTemplate();

static uint8_t functSwitchModule(uint8_t button);


void cProjectEditor::update()
{
	if(projectOptions > 0)
	{
		switch(projectOptions)
		{
			case mtProjectStartModeOpenLast:
			{
				listOnlyFolderNames("/Projects/");

				fileManager.openProject(&locationFilesList[selectedLocation][0],projectTypeUserMade);
				fileManager.loadSamplesMemory();

				functSwitchModule(interfaceButton17); //13
				break;
			}
			default: break;
		}

		projectOptions = 0;
	}
}

void cProjectEditor::start(uint32_t options)
{

	selectedLocation = 0;

	if(options == mtProjectStartModeOpenLast)
	{
		moduleRefresh = 1;

		projectOptions = mtProjectStartModeOpenLast;

		return;
	}

	// ustawienie funkcji
	FM->setButtonObj(interfaceButton10, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton11, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton12, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton14, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton15, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton16, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);


	showDefaultScreen();
	setDefaultScreenFunct();



	//typedef void (cProjectEditor::*funct1) (void);
	//funct1 = &cProjectEditor::functOpenProject;
	//(this->*funct1)();

}

void cProjectEditor::stop()
{
	moduleRefresh = 0;
	projectOptions = 0;

}

void cProjectEditor::setDefaultScreenFunct()
{
	//funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButton1, buttonPress, functShowProjectsList);
	FM->setButtonObj(interfaceButton0, buttonPress, functShowTemplatesList);
	FM->setButtonObj(interfaceButton4, buttonPress, functSaveProject);

}
//==============================================================================================================

uint8_t functShowProjectsList()
{
	PE->listOnlyFolderNames("/Projects/");


	PE->showProjectsList();


// funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	PE->FM->clearAllPots();

	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenProject);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);


	return 1;
}


uint8_t functShowTemplatesList()
{
	PE->listOnlyFolderNames("/Templates/");


	PE->showTemplatesList();




// funkcje
	PE->FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	PE->FM->clearAllPots();

	PE->FM->setPotObj(interfacePot0, &PE->selectedLocation, 0, PE->locationFilesCount-1, 1, PE->fileListControl);

	PE->FM->setButtonObj(interfaceButton0, buttonPress, functOpenTemplate);
	PE->FM->setButtonObj(interfaceButton1, buttonPress, functCancelList);
	PE->FM->setButtonObj(interfaceButton4, buttonPress, functCreateNewTemplate);

	return 1;
}

uint8_t functCancelList()
{
	PE->showDefaultScreen();

	return 1;
}

uint8_t functSaveProject()
{
	fileManager.saveProject();

	return 1;
}

uint8_t functOpenProject()
{

	fileManager.openProject(&PE->locationFilesList[PE->selectedLocation][0],projectTypeUserMade);
//
//			for(uint8_t i=0; i < 8; i++)
//			{
//				mtProject.mtProjectRemote.instrumentFile[i].index = -1;
//			}
//			for(uint8_t i=0; i < 8; i++)
//			{
//				mtProject.mtProjectRemote.sampleFile[i].index = -1;
//			}


/*
	fileManager.createNewProject("Project_Test1");

	fileManager.importSampleToProject(NULL,"1.WAV","1.WAV",0,0,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"2.WAV","2.WAV",1,1,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"3.WAV","3.WAV",2,2,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"4.WAV","4.WAV",3,3,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"5.WAV","5.WAV",4,4,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"6.WAV","6.WAV",5,5,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"7.WAV","7.WAV",6,6,mtSampleTypeWaveFile);
	fileManager.importSampleToProject(NULL,"8.WAV","8.WAV",7,7,mtSampleTypeWaveFile);


	fileManager.saveProject();
*/


	fileManager.loadSamplesMemory();
	//PE->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);


	PE->showDefaultScreen();

	return 1;
}

uint8_t functOpenTemplate()
{
	fileManager.openProject(&PE->locationFilesList[PE->selectedLocation][0],projectTypeExample);
	//loadSamplesBank();
	fileManager.saveAsProject((char*)"fromTemplate");
	fileManager.openProject((char*)"fromTemplate", projectTypeUserMade);
	//loadSamplesBank();
	//PE->eventFunct(mtProjectEditorEventLoadSampleBank, 0, 0, 0);

	PE->showDefaultScreen();


	return 1;
}

uint8_t functCreateNewTemplate()
{
	fileManager.createEmptyTemplateProject((char*)"New");
	functShowTemplatesList();

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{

	PE->eventFunct(eventSwitchModule,PE,&button,0);

	return 1;
}
//======================================================================================================================


void cProjectEditor::listOnlyFolderNames(const char* folder)
{
	char filePath[256] = {0};
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




