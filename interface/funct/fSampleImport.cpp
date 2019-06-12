

#include <sampleImporter.h>
#include "mtFileManager.h"

#include "mtAudioEngine.h"
#include <projectEditor.h>

cSampleImporter sampleImporter;
static cSampleImporter* SI = &sampleImporter;



static  uint8_t functPlayAction();
static  uint8_t functStopAction();
static  uint8_t functRecAction();



static  uint8_t functChangeFolder(uint8_t button);
static  uint8_t functChangeFile(uint8_t button);
static  uint8_t functChangeInstrument(uint8_t button);

static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);

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

	listInstrumentSlots();
	showInstrumentsList();

	calculateMemoryUsage();

	//SI->selectedPlace = 0;
	SI->activateLabelsBorder();

	// ustawienie funkcji
	FM->setButtonObj(interfaceButton10, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton11, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton12, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton13, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton14, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton15, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton16, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton17, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButton23, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton24, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButton25, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();
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

	FM->setButtonObj(interfaceButton28, functShift);
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

	SI->selectedPlace = 0;
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

	SI->selectedPlace = 1;
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functChangeInstrument(uint8_t button)
{
	if(button == interfaceButton4)
	{
		SI->changeInstrumentSelection(-1);
	}
	else //if(button == interfaceButton3)
	{
		SI->changeInstrumentSelection(1);
	}

	SI->selectedPlace = 2;
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functInstrumentAdd()
{
	 SI->SelectFile();


	return 1;
}

static  uint8_t functInstrumentDelete()
{

	if(mtProject.instrument[SI->selectedSlot].sample.loaded)
	{
		mtProject.used_memory = mtProject.used_memory - mtProject.instrument[SI->selectedSlot].sample.length;
	}

	fileManager.deleteInstrument(SI->selectedSlot);
	//importSampleToProject(actualPath,&locationFileList[selectedFile][0], selectedSlot);
	//fileManager.loadSamplesMemory();

	SI->listInstrumentSlots();
	SI->showInstrumentsList();

	SI->calculateMemoryUsage();

	return 1;
}



//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFolderSelection(value); break;
	case 1: SI->changeFileSelection(value); break;
	case 2: SI->changeInstrumentSelection(value); break;

	}


	return 1;
}

static  uint8_t functEnter()
{

	switch(SI->selectedPlace)
	{
	case 0: SI->BrowseFolder(); break;
	case 1: SI->SelectFile(); break;

	}

	return 1;
}

static  uint8_t functShift(uint8_t state)
{

	if(state == 0)  SI->stopPlaying();
	else if (state == 1)
	{

		switch(SI->selectedPlace)
		{
		case 1: SI->playSdFile(); 			break;
		case 2: SI->playSampleFromBank(); 	break;

		}
	}

	return 1;
}


static  uint8_t functLeft()
{
	if(SI->selectedPlace > 0) SI->selectedPlace--;
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	if(SI->selectedPlace < 2) SI->selectedPlace++;
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFolderSelection(-1); break;
	case 1: SI->changeFileSelection(-1); break;
	case 2: SI->changeInstrumentSelection(-1); break;
	}

	return 1;
}

static  uint8_t functDown()
{
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFolderSelection(1); break;
	case 1: SI->changeFileSelection(1); break;
	case 2: SI->changeInstrumentSelection(1); break;
	}

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

uint8_t cSampleImporter::changeInstrumentSelection(int16_t value)
{
	if(selectedSlot+value < 0) selectedSlot = 0;
	else if(selectedSlot+value > INSTRUMENTS_COUNT-1) selectedSlot = INSTRUMENTS_COUNT-1;
	else  selectedSlot += value;

	mtProject.values.lastUsedInstrument = selectedSlot;

	display.setControlValue(instrumentListControl, selectedSlot);
	display.refreshControl(instrumentListControl);

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

	actualPath[(ptr-actualPath)] = 0;
}




void cSampleImporter::BrowseFolder()
{
	if(dirLevel == 0)
	{
		dirLevel = 1;

		strcpy(actualPath, &locationFolderList[selectedFolder][0]);

		listOnlyFolderNames(actualPath);
		showFolderTree();
	}
	else
	{
		if(selectedFolder > 0)
		{
			dirLevel++;

			strcat(actualPath, &locationFolderList[selectedFolder][0]);

			listOnlyFolderNames(actualPath);
			showFolderTree();
		}
		else
		{
			dirLevel--;

			if(dirLevel > 0)
			{
				goUpInActualPath();
			}
			else
			{
				strcpy(actualPath, "/");
			}

			listOnlyFolderNames(actualPath);
			showFolderTree();
		}
	}

	selectedFolder = 0;
	selectedFile = 0;

	listOnlyWavFromActualPath();
	showFilesTree();
}

void cSampleImporter::SelectFile()
{
	fileManager.importSampleToProject(actualPath,&locationFileList[selectedFile][0], selectedSlot);

	fileManager.samplesLoader.start(selectedSlot);

	calculateMemoryUsage();

//	selectedSlot++;

	listInstrumentSlots();
	showInstrumentsList();
}


//==============================================================================================
void cSampleImporter::listInstrumentSlots()
{

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		if(i<9)
		{
			slotNames[i][0] = (i+1)%10 + 48;
			slotNames[i][1] = '.';
			slotNames[i][2] = ' ';
			slotNames[i][3] = 0;
		}
		else
		{
			slotNames[i][0] = ((i+1)/10) + 48;
			slotNames[i][1] = (i+1)%10 + 48;
			slotNames[i][2] = '.';
			slotNames[i][3] = ' ';
			slotNames[i][4] = 0;
		}

		if(mtProject.instrument[i].sample.loaded)
		{
			strcat(&slotNames[i][0], mtProject.instrument[i].sample.file_name);
		}

		ptrSlotNames[i] = &slotNames[i][0];
	}
}


//==============================================================================================
void cSampleImporter::calculateMemoryUsage()
{

	if(mtProject.used_memory > mtProject.max_memory)
	{
		memoryUsage = 100;
	}
	else
	{
		memoryUsage = (mtProject.used_memory*100)/mtProject.max_memory;
	}


	showMemoryUsage();

}


//==============================================================================================
void cSampleImporter::playSdFile()
{
	if(!isWavFile(&locationFileList[selectedFile][0])) return;

	char file_path[255];

	strcpy(file_path, actualPath);
	if(dirLevel > 0)strcat(file_path, "/");
	strcat(file_path, &locationFileList[selectedFile][0]);

	playMode = playModeSdFile;

	engine.prevSdConnect();

	playSdWav.play(file_path);

}


void cSampleImporter::playSampleFromBank()
{
	playMode = playModeSampleBank;

	instrumentPlayer[0].noteOnforPrev(mtProject.instrument[selectedSlot].sample.address,
									  mtProject.instrument[selectedSlot].sample.length);
}


void cSampleImporter::stopPlaying()
{
	if(playMode == playModeSdFile)
	{
		playSdWav.stop();
		engine.prevSdDisconnect();
	}
	else if(playMode == playModeSampleBank)
	{
		instrumentPlayer[0].noteOff();
	}

	playMode = playModeStop;
}
