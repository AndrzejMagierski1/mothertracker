


#include <sampleImporter.h>
#include "mtFileManager.h"




cSampleImporter sampleImporter;
static cSampleImporter* SI = &sampleImporter;





static  uint8_t functPlayAction();
static  uint8_t functStopAction();
static  uint8_t functRecAction();





static  uint8_t functChangeFolder(uint8_t button);
static  uint8_t functChangeFile(uint8_t button);
static  uint8_t functChangeInstrument(uint8_t button);

static  uint8_t functEnter();


static  uint8_t functInstrumentAdd();
static  uint8_t functInstrumentDelete();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();


static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);



void cSampleImporter::update()
{



}

void cSampleImporter::start(uint32_t options)
{
	moduleRefresh = 1;

	selectedFolder = 0;
	selectedFile = 0;
	dirLevel = 0;

	actualPath[0] = '/';
	actualPath[1] = 0;

	listOnlyFolderNames(actualPath);
	showFolderTree();

	listOnlyWavFromActualPath();
	showFilesTree();


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

void cSampleImporter::stop()
{

	moduleRefresh = 0;
}

void cSampleImporter::setDefaultScreenFunct()
{

	//funkcje
	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setButtonObj(interfaceButton8, buttonPress, functPlayAction);
	//FM->setButtonObj(interfaceButton9, buttonPress, functStopAction);
	FM->setButtonObj(interfaceButton9, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButton30, buttonPress, functLeft);
	FM->setButtonObj(interfaceButton32, buttonPress, functRight);
	FM->setButtonObj(interfaceButton26, buttonPress, functUp);
	FM->setButtonObj(interfaceButton31, buttonPress, functDown);


	FM->setButtonObj(interfaceButton0, buttonPress, functChangeFolder);
	FM->setButtonObj(interfaceButton1, buttonPress, functChangeFolder);

	FM->setButtonObj(interfaceButton2, buttonPress, functChangeFile);
	FM->setButtonObj(interfaceButton3, buttonPress, functChangeFile);

	FM->setButtonObj(interfaceButton4, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton5, buttonPress, functChangeInstrument);

	FM->setButtonObj(interfaceButton6, buttonPress, functInstrumentAdd);
	FM->setButtonObj(interfaceButton7, buttonPress, functInstrumentDelete);


	FM->setButtonObj(interfaceButton29, buttonPress, functEnter);
	FM->setButtonObj(interfaceButton33, buttonPress, functEnter);


}



//==============================================================================================================

static  uint8_t functChangeFolder(uint8_t button)
{
	if(button == interfaceButton0)
	{
		SI->changeFolderSelection(-1);
	}
	else //if(button == interfaceButton1)
	{
		SI->changeFolderSelection(1);
	}

	SI->selectedLabel = 1;
	SI->activateLabelsBorder();


	return 1;
}



static  uint8_t functChangeFile(uint8_t button)
{
	if(button == interfaceButton2)
	{
		SI->changeFileSelection(-1);
	}
	else //if(button == interfaceButton3)
	{
		SI->changeFileSelection(1);
	}

	SI->selectedLabel = 2;
	SI->activateLabelsBorder();



	return 1;
}

static  uint8_t functChangeInstrument(uint8_t button)
{

	return 1;
}

static  uint8_t functInstrumentAdd()
{

	return 1;
}

static  uint8_t functInstrumentDelete()
{

	return 1;
}





//==============================================================================================================


static  uint8_t functEncoder(int16_t value)
{
	if(SI->selectedLabel > 0)
	{
		switch(SI->selectedLabel)
		{
		case 1: SI->changeFolderSelection(value); break;
		case 2: SI->changeFileSelection(value); break;
		}

		return 1;
	}


	//display.refreshControl(PTE->patternControl);

	return 1;
}


static  uint8_t functEnter()
{
	if(SI->selectedLabel > 0)
	{
		switch(SI->selectedLabel)
		{
		case 1: SI->BrowseFolder(); break;
		case 2: SI->SelectFile(); break;

		}

		return 1;
	}





	return 1;
}


static  uint8_t functLeft()
{

	return 1;
}


static  uint8_t functRight()
{


	return 1;
}


static  uint8_t functUp()
{

	return 1;
}


static  uint8_t functDown()
{


	return 1;
}




static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0) sequencer.play();

	return 1;
}


static  uint8_t functStopAction()
{
	if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}



	return 1;
}

static  uint8_t functRecAction()
{


	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{

	SI->eventFunct(eventSwitchModule,SI,&button,0);

	return 1;
}


//======================================================================================================================
uint8_t cSampleImporter::changeFolderSelection(int16_t value)
{
	if(selectedFolder+value < 0) selectedFolder = 0;
	else if(selectedFolder+value > locationFolderCount-1) selectedFolder = locationFolderCount-1;
	else  selectedFolder += value;

	display.setControlValue(folderListControl, selectedFolder);
	display.refreshControl(folderListControl);

	return 1;
}

uint8_t cSampleImporter::changeFileSelection(int16_t value)
{
	if(selectedFile+value < 0) selectedFile = 0;
	else if(selectedFile+value > locationFileCount-1) selectedFile = locationFileCount-1;
	else  selectedFile += value;

	display.setControlValue(fileListControl, selectedFile);
	display.refreshControl(fileListControl);

	return 1;
}

//======================================================================================================================
void cSampleImporter::listOnlyFolderNames(char* path)
{


	sdLocation.close();
	sdLocation.open(path, O_READ); //"/"

	if(dirLevel == 0)
	{
		locationFolderCount = sdLocation.createFilesList(0,locationFolderList, list_length_max);
	}
	else
	{
		locationFolderCount = sdLocation.createFilesList(1,locationFolderList, list_length_max);
		strcpy(&locationFolderList[0][0], "/..");
	}

	sdLocation.close();

	uint8_t foundFolderCount = 0;

	for(uint8_t i = 0; i < locationFolderCount; i++)
	{
		if(locationFolderList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(&locationFolderList[foundFolderCount][0],&locationFolderList[i][0]);

			foundFolderCount++;
		}
	}


	locationFolderCount = foundFolderCount;

	for(uint8_t i = 0; i < locationFolderCount; i++)
	{
		folderNames[i] = &locationFolderList[i][0];
	}

}



void cSampleImporter::listOnlyWaveNames(char* folder)
{
	char filePath[256] = {0};
	strcpy(filePath, folder);
	strcat(filePath,"/");
	sdLocation.close();
	sdLocation.open(folder, O_READ); //"/"
	locationFileCount = sdLocation.createFilesList(0,locationFileList, list_length_max);
	sdLocation.close();

	uint8_t foundFolderCount = 0;

	for(uint8_t i = 0; i < locationFileCount; i++)
	{
		if(locationFileList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(filePath, folder);
			strcat(filePath,&locationFileList[i][0]); //doklej nazwe folderu

			strcpy(&locationFileList[foundFolderCount][0],&locationFileList[i][0]);

			foundFolderCount++;
		}
	}


	locationFileCount = foundFolderCount;

	for(uint8_t i = 0; i < locationFileCount; i++)
	{
		fileNames[i] = &locationFileList[i][0];
	}

}

void cSampleImporter::listOnlyWavFromActualPath()
{

	sdLocation.close();
	sdLocation.open(actualPath, O_READ);


	locationFileCount = sdLocation.createFilesList(0,locationFileList, list_length_max);


	sdLocation.close();


	uint8_t foundFilesCount = 0;
	for(uint8_t i = 0; i < locationFileCount; i++)
	{
		if(isWavFile(&locationFileList[i][0]))	//tylko jesli plik wav
		{
			strcpy(&locationFileList[foundFilesCount][0],&locationFileList[i][0]);
			foundFilesCount++;
		}
	}


	locationFileCount = foundFilesCount;

	for(uint8_t i = 0; i < locationFileCount; i++)
	{
		fileNames[i] = &locationFileList[i][0];
	}
}

uint8_t cSampleImporter::isWavFile(char* fileName)
{
	uint8_t endPos = 0;
	char temp_name[32];
	strcpy(temp_name, fileName);

	while(temp_name[endPos] != 0 && endPos < 19)
	{
		if(temp_name[endPos] > 96 && temp_name[endPos] < 123) temp_name[endPos] = temp_name[endPos] - 32;
		endPos++;
	}

	endPos--;

	if(temp_name[endPos] == 'V' && temp_name[endPos-1] == 'A' && temp_name[endPos-2] == 'W' && temp_name[endPos-3] == '.')
	{
		return 1;
	}

	return 0;
}



void cSampleImporter::goUpInActualPath()
{

	uint8_t length = strlen(actualPath);

	actualPath[length] = 0;

	char* ptr = strrchr(actualPath, '/');

	if(ptr == nullptr)
	{
		strcpy(actualPath, "/");
		return;
	}

	actualPath[(ptr-actualPath)+1] = 0;




}




void cSampleImporter::BrowseFolder()
{
	if(dirLevel == 0)
	{
		dirLevel = 1;
		//strcpy(actualPath,"/");
		strcpy(actualPath, &locationFolderList[selectedFolder][0]);
		//strcat(actualPath, "/");

		listOnlyFolderNames(actualPath);
		showFolderTree();

	}
	else
	{
		if(selectedFolder > 0)
		{
			dirLevel++;

			strcat(actualPath, &locationFolderList[selectedFolder][0]);
			//strcat(actualPath, "/");

			listOnlyFolderNames(actualPath);
			showFolderTree();
		}
		else
		{
			dirLevel--;

			goUpInActualPath();

			listOnlyFolderNames(actualPath);
			showFolderTree();
		}

	}


	listOnlyWavFromActualPath();
	showFilesTree();


}

void cSampleImporter::SelectFile()
{

}

