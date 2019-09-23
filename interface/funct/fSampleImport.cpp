

#include <sampleImporter.h>
#include "mtFileManager.h"

#include "Encoder.h"

#include "mtAudioEngine.h"
#include <projectEditor.h>



cSampleImporter sampleImporter;
static cSampleImporter* SI = &sampleImporter;



static  uint8_t functPlayAction();
static  uint8_t functStopAction();
static  uint8_t functRecAction();



static  uint8_t functChangeFolder(uint8_t button);
static  uint8_t functChangeInstrument(uint8_t button);

static  uint8_t functEnter();
static  uint8_t functShift(uint8_t state);

static  uint8_t functInstrumentAdd();
static  uint8_t functInstrumentDelete();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static uint8_t functShiftCopy();
static uint8_t functShiftPaste();

uint8_t preview(uint8_t state);



static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);




void cSampleImporter::update()
{
	if(fileManager.samplesLoader.getMemoryUsageChangeFlag())
	{
		fileManager.samplesLoader.clearMemoryUsageChangeFlag();
		handleMemoryBar();
	}

	if(fileManager.samplesLoader.getLoadChangeFlag())
	{
		fileManager.samplesLoader.clearLoadChangeFlag();
		listInstrumentSlots();
		showInstrumentsList();
	}

	handleSequenceCopyingLoading();

	if(firstMemBarLoadFlag==1)
	{
		handleMemoryBar();
	}
}

void cSampleImporter::start(uint32_t options)
{
	moduleRefresh = 1;

	Encoder.setAcceleration(0);

	//selectedFile = 0;

	selectedSlot = mtProject.values.lastUsedInstrument;



	listAllFoldersFirst();

	listInstrumentSlots();
	showInstrumentsList();

	handleMemoryBar();

	activateLabelsBorder();

	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);

	showDefaultScreen();
	setDefaultScreenFunct();

	AddOrEnter();

	selectionLength=0;
	resetInstrSel();
	setSelect(selectedPlace);
	displayDelete(selectedPlace);
}

void cSampleImporter::stop()
{
	Encoder.setAcceleration(3);

	moduleRefresh = 0;
}

void cSampleImporter::setDefaultScreenFunct()
{
	//funkcje
	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	//FM->setButtonObj(interfaceButtonEnter, buttonPress, functEnter);
	FM->setButtonObj(interfaceButtonShift, functShift);
	//FM->setButtonObj(interfaceButtonEncoder, buttonPress, functEnter);


	FM->setButtonObj(interfaceButton0, buttonPress, functChangeFolder);
	FM->setButtonObj(interfaceButton1, buttonPress, functChangeFolder);

	FM->setButtonObj(interfaceButton2, buttonPress, functEnter);
	FM->setButtonObj(interfaceButton3, preview);

	//FM->setButtonObj(interfaceButton4, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton5, buttonPress, functInstrumentDelete);

	FM->setButtonObj(interfaceButton6, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton7, buttonPress, functChangeInstrument);

	FM->setButtonObj(interfaceButtonCopy, buttonPress, functShiftCopy);
	FM->setButtonObj(interfaceButtonPaste, buttonPress, functShiftPaste);



}


//==============================================================================================================
static  uint8_t functChangeFolder(uint8_t button)
{
	if(SI->selectedPlace == 0)
	{
		if(button == interfaceButton0)
		{
			SI->changeFileSelection(-1);
		}
		else //if(button == interfaceButton1)
		{
			SI->changeFileSelection(1);
		}
	}

	SI->selectedPlace = 0;
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functChangeInstrument(uint8_t button)
{
	if(SI->selectedPlace == 1)
	{
		if(button == interfaceButton6)
		{
			SI->changeInstrumentSelection(-1);
		}
		else //if(button == interfaceButton3)
		{
			SI->changeInstrumentSelection(1);
		}
	}

	SI->selectedPlace = 1;
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
	if(SI->selectedPlace == 1)
	{
		if(SI->currSelectPlace == 1 && SI->selectionLength)
		{
			uint8_t selectionStart = SI->getSelectionStart();

			for(int i=0;i<SI->selectionLength;i++)
			{
				if(mtProject.instrument[selectionStart+i].isActive)
				{
					mtProject.used_memory -= 2* mtProject.instrument[selectionStart+i].sample.length;
				}

				fileManager.deleteInstrument(selectionStart+i);
			}
		}
		else
		{
			if(mtProject.instrument[SI->selectedSlot].isActive)
			{
				mtProject.used_memory -= 2* mtProject.instrument[SI->selectedSlot].sample.length;
			}

			fileManager.deleteInstrument(SI->selectedSlot);
		}

		SI->cancelSelect();
		SI->handleMemoryBar();

		SI->listInstrumentSlots();
		SI->showInstrumentsList();

	}

	return 1;
}



//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFileSelection(value); break;
	case 1: SI->changeInstrumentSelection(value); break;

	}


	return 1;
}

static  uint8_t functEnter()
{

	switch(SI->selectedPlace)
	{
	case 0: SI->BrowseOrAdd(); break;
	case 1: SI->BrowseOrAdd(); break;

	}

	return 1;
}

static uint8_t functShiftCopy()
{
	if(SI->selectionActive)
	{
		if(SI->selectedPlace == 1 && SI->currSelectPlace == 1)
		{
			SI->instrCopyStart=SI->getSelectionStart();
			SI->copyElementMax=SI->selectionLength;
		}
	}

	return 1;
}

static uint8_t functShiftPaste()
{
	if(SI->currentCopyStatusFlag || SI->currentLoadStatusFlag) return 1;

	if(SI->selectionActive)
	{
		char projectSamplePath[255];
		uint8_t willFit = 1;
		uint8_t selectOverMax = 0;

		if(SI->copyElementMax && SI->instrCopyStart != SI->selectedSlot)
		{
			SI->instrActiveInSel = SI->getActiveInstrInSelection();
			SI->instrCopied=0;
			SI->copyElement = 0;

			if(SI->instrActiveInSel)
			{
				while(SI->selectedSlot + SI->copyElementMax > 48)
				{
					SI->copyElementMax--;
					selectOverMax++;
				}

				for(int i = (SI->copyElementMax - selectOverMax); i>0; i--)
				{
					willFit=SI->willCopyInstrFit(i);

					if(willFit)
					{
						break;
					}
					else
					{
						SI->copyElementMax--;
						/*TODO: popout ze kopiowanie zostalo uciete*/
					}
				}

				if(willFit)
				{
					SI->copyType = 2;

					strcpy(projectSamplePath,fileManager.currentProjectPatch);
					strcat(projectSamplePath,"/samples");

					fileManager.clearAutoLoadFlag();

					while(SI->copyElement < SI->copyElementMax)
					{
						if(mtProject.instrument[SI->instrCopyStart + SI->copyElement].isActive == 1)
						{
							break;
						}
						else
						{
							SI->copyElement++;
						}
					}

					if(SI->copyElement == (SI->copyElementMax-1))
					{
						fileManager.setAutoLoadFlag();
					}

					fileManager.setStart(SI->selectedSlot);
					fileManager.assignSampleToInstrument(projectSamplePath, SI->parseNewName(SI->instrCopyStart+SI->copyElement), SI->selectedSlot + SI->copyElement);
					memcpy(&mtProject.instrument[SI->selectedSlot+SI->copyElement],&mtProject.instrument[SI->instrCopyStart+SI->copyElement],sizeof(mtProject.instrument[0]));

					SI->copyElement++;
					SI->instrCopied++;
				}
			}
		}
	}

	return 1;
}

static  uint8_t functShift(uint8_t state)
{
	if(state ==0)
	{
		SI->selectionActive=0;
		SI->frameSelectMode(SI->currSelectPlace,0);

		if(SI->selectionLength == 1)
		{
			SI->cancelSelect();
		}
	}
	else if(state == 1)
	{
		if(SI->currSelectPlace != SI->selectedPlace)
		{
			SI->currSelectPlace = SI->selectedPlace;
			SI->selectionLength=0;
			SI->setSelect(SI->selectedPlace);
		}

		SI->frameSelectMode(SI->currSelectPlace,1);

		if(SI->selectedPlace==0)
		{
			SI->selectionActive=1;

			if(SI->checkIfValidSelection(SI->selectedFile))// nie mozna zaczac zaznaczac od folderu
			{
				SI->selectionTab[SI->selectedFile]=1;

				if(!SI->selectionLength)
				{
					SI->selectionLength=1;
				}
			}
		}
		else
		{
			SI->selectionTab[SI->selectedSlot]=1;

			SI->selectionActive=1;

			if(!SI->selectionLength)
			{
				SI->selectionLength=1;
			}
		}

		SI->updateSelection();
	}

	SI->handleMemoryBar();

	return 1;
}


static  uint8_t functLeft()
{
	if(SI->selectedPlace > 0)
	{
		SI->selectedPlace--;

		if(SI->currSelectPlace == 1)
		{
			SI->cancelSelect();
		}

		SI->handleMemoryBar();
		SI->displayDelete(0);

		SI->resetInstrSel();
	}
	else
	{
		SI->cancelSelect();

		SI->rewindListToBeggining();
		SI->handleMemoryBar();
		SI->AddOrEnter();
	}

	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	if(SI->selectedPlace < 1)
	{
		SI->displayDelete(1);
		SI->selectedPlace++;
	}

	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFileSelection(-1); break;
	case 1: SI->changeInstrumentSelection(-1); break;
	}
	SI->stopPlaying();
	return 1;
}

static  uint8_t functDown()
{
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFileSelection(1); break;
	case 1: SI->changeInstrumentSelection(1); break;
	}
	SI->stopPlaying();
	return 1;
}

static  uint8_t functPlayAction()
{
	if(sequencer.getSeqState() == 0)
	{
		sequencer.play();
	}
	else if(sequencer.getSeqState() == 1)
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
	if(SI->currentLoadStatusFlag || SI->currentCopyStatusFlag) return 1;
	SI->eventFunct(eventSwitchModule,SI,&button,0);

	return 1;
}


//======================================================================================================================
/*uint8_t cSampleImporter::changeFolderSelection(int16_t value)
{
	if(selectedFolder+value < 0) selectedFolder = 0;
	else if(selectedFolder+value > locationFolderCount-1) selectedFolder = locationFolderCount-1;
	else  selectedFolder += value;

	AddOrEnter();

	display.setControlValue(folderListControl, selectedFolder);
	display.refreshControl(folderListControl);

	return 1;
}*/

uint8_t cSampleImporter::changeFileSelection(int16_t value)
{
	if(selectionActive)
	{
		selectedFile=handleSelecting(selectedFile, locationExplorerCount, value, 1);
		updateSelection();
	}
	else
	{
		if(selectedFile+value < 0) selectedFile = 0;
		else if(selectedFile+value > locationExplorerCount-1) selectedFile = locationExplorerCount-1;
		else  selectedFile += value;

		if(selectedPlace == currSelectPlace && selectionLength !=0)
		{
			cancelSelect();
		}
	}


	display.setControlValue(explorerListControl, selectedFile);
	display.refreshControl(explorerListControl);

	AddOrEnter();

	handleMemoryBar();

	return 1;
}

uint8_t cSampleImporter::changeInstrumentSelection(int16_t value)
{
	if(selectionActive)
	{
		selectedSlot=handleSelecting(selectedSlot, 48, value, 0);
	}
	else
	{
		if(selectedSlot+value < 0) selectedSlot = 0;
		else if(selectedSlot+value > INSTRUMENTS_COUNT-1) selectedSlot = INSTRUMENTS_COUNT-1;
		else  selectedSlot += value;

		if(selectedPlace == currSelectPlace)
		{
			cancelSelect();
		}
	}

	updateSelection();

	mtProject.values.lastUsedInstrument = selectedSlot;

	display.setControlValue(instrumentListControl, selectedSlot);
	display.refreshControl(instrumentListControl);

	handleMemoryBar();

	return 1;
}

//======================================================================================================================
void cSampleImporter::listOnlyFolderNames(char* path, uint8_t startPoint)
{
	uint8_t allFilesNum;
	sdLocation.close();
	sdLocation.open(path, O_READ); //"/"
	sdLocation.read();

	if(dirLevel == 0)
	{
		allFilesNum = sdLocation.createFilesList(0,locationExplorerList, list_length_max);
	}
	else
	{
		allFilesNum = sdLocation.createFilesList(1,locationExplorerList, list_length_max);
		allFilesNum+=1;
		strcpy(&locationExplorerList[0][0], "/..");
	}

	sdLocation.close();

	uint8_t foundFolderCount = 0;

	for(uint8_t i = 0; i < allFilesNum; i++)
	{
		if(locationExplorerList[i][0] == '/')	//tylko jesli folder
		{
			strcpy(&locationExplorerList[foundFolderCount][0],&locationExplorerList[i][0]);
			foundFolderCount++;
		}
	}

	if(foundFolderCount == 0 && dirLevel !=0) // /.. na poczatku jak nie znajdzie zadnego pliku
	{
		foundFolderCount =1;
	}

	locationExplorerCount += foundFolderCount;

	for(uint8_t i = startPoint; i < (startPoint+locationExplorerCount); i++)
	{
		explorerNames[i] = &locationExplorerList[i-startPoint][0];
	}
}


/*
void cSampleImporter::listOnlyWaveNames(char* folder, uint8_t startPoint)
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

	for(uint8_t i = startPoint; i < (startPoint+locationFileCount); i++)
	{
		fileNames[i] = &locationFileList[i][0];
	}

}*/

void cSampleImporter::listOnlyWavFromActualPath(uint8_t startPoint)
{
	char filePath[255];

	sdLocation.close();
	sdLocation.open(actualPath, O_READ);

	locationExplorerCount += sdLocation.createFilesList(startPoint,locationExplorerList, (list_length_max-startPoint),2);

	for(int i=startPoint;i<locationExplorerCount;i++)
	{
		strcpy(filePath, actualPath);
		if(dirLevel > 0)
		{
			strcat(filePath, "/");
		}

		strcat(filePath, &locationExplorerList[i][0]);

		currentFolderMemoryFileUsage[i]= 2* fileManager.samplesLoader.waveLoader.getInfoAboutWave(filePath);
	}

	sdLocation.close();

	for(uint8_t i = startPoint; i < locationExplorerCount; i++)
	{
		explorerNames[i] = &locationExplorerList[i][0];
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

void cSampleImporter::listAllFoldersFirst()
{
	locationExplorerCount=0;

	for(int i=0;i<list_length_max;i++)
	{
		explorerNames[i]=NULL;
	}

	listOnlyFolderNames(actualPath,0);
	listOnlyWavFromActualPath(locationExplorerCount);

	showFilesTree();
}

void cSampleImporter::BrowseOrAdd()
{
	if(locationExplorerList[selectedFile][0] == '/')
	{
		if(selectedPlace ==0)
		{
			if(dirLevel == 0)
			{
				dirLevel = 1;

				strcpy(actualPath, &locationExplorerList[selectedFile][0]);


				explorerPositionTable[explorerCurrentPosition]=selectedFile;
				explorerCurrentPosition++;

				selectedFile=0;


				listAllFoldersFirst();
			}
			else
			{
				if(selectedFile > 0)
				{
					if(explorerCurrentPosition<PREVIOUS_POSITION_LIFO)
					{
						dirLevel++;

						explorerPositionTable[explorerCurrentPosition]=selectedFile;
						explorerCurrentPosition++;


						strcat(actualPath, &locationExplorerList[selectedFile][0]);

						selectedFile=0;

						listAllFoldersFirst();
					}

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

					explorerCurrentPosition--;
					selectedFile=explorerPositionTable[explorerCurrentPosition];


					listAllFoldersFirst();
				}
			}
		}
	}
	else
	{
		functInstrumentAdd();
	}
}

void cSampleImporter::SelectFile()
{
	if(currentCopyStatusFlag || currentLoadStatusFlag) return;

	if(!fullMemoryFlag)
	{
		copyType = 1;

		fileManager.clearAutoLoadFlag();

		if(currSelectPlace==0)
		{
			uint8_t position;

			copyElement=0;
			copyElementMax=selectionLength;

			if(copyElementMax==0)
			{
				position=selectedFile;
			}
			else
			{
				position=getSelectionStart();

				while(selectedSlot + (copyElementMax-1) >= 48)//sprawdzic
				{
					copyElementMax--;
				}
			}

			if((copyElement == (copyElementMax-1)) || copyElementMax ==0)
			{
				fileManager.setAutoLoadFlag();
			}

			fileManager.setStart(selectedSlot);
			fileManager.assignSampleToInstrument(actualPath, &locationExplorerList[position + copyElement][0], selectedSlot +  copyElement);
			copyElement++;
		}
		else
		{
			fileManager.setAutoLoadFlag();
			fileManager.setStart(selectedSlot);
			fileManager.assignSampleToInstrument(actualPath, &locationExplorerList[selectedFile][0], selectedSlot);
		}
	}
}



//==============================================================================================
void cSampleImporter::listInstrumentSlots()
{

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(&interfaceGlobals.intrumentsNames[i][0], "%d. ", i+1);

		if(mtProject.instrument[i].isActive)
		{
			strncat(&interfaceGlobals.intrumentsNames[i][0], mtProject.instrument[i].sample.file_name,SAMPLE_NAME_SIZE);
		}

		interfaceGlobals.ptrIntrumentsNames[i] = &interfaceGlobals.intrumentsNames[i][0];
	}
}

void cSampleImporter::handleMemoryBar()
{

	if(checkIfValidSelection(selectedFile))
	{
		calculateFileMemUsage();
	}
	else
	{
		currentSelectMemorySize=0;
		fullMemoryFlag = 0;
	}

	calculateMemUsage();
	showMemoryUsage();
}

void cSampleImporter::calculateFileMemUsage()
{
	uint32_t tempSelectMemorySize=0;
	uint32_t tempInstrumentMemorySize=0;
	uint8_t tempSelectionLength;

	if(selectionLength && currSelectPlace == 0)
	{
		uint8_t selectionStart = getSelectionStart();

		if(selectedSlot+selectionLength>48)
		{
			tempSelectionLength= selectionLength - (selectedSlot + selectionLength - 48);
		}
		else
		{
			tempSelectionLength = selectionLength;
		}

		for(int i=selectionStart;i<(selectionStart+tempSelectionLength);i++)//get file selection size
		{
			tempSelectMemorySize += currentFolderMemoryFileUsage[i];
		}

		for(int i=selectedSlot;i<selectedSlot+tempSelectionLength;i++)//get size when pasted on current isntrument slot
		{
			if(mtProject.instrument[i].isActive)
			{
				tempInstrumentMemorySize += 2*mtProject.instrument[i].sample.length;
			}
		}

		currentSelectMemorySize = tempSelectMemorySize - tempInstrumentMemorySize;
	}
	else
	{
		currentSelectMemorySize = currentFolderMemoryFileUsage[selectedFile];

		if(mtProject.instrument[selectedSlot].isActive)
		{
			currentSelectMemorySize -= 2*mtProject.instrument[selectedSlot].sample.length;
		}
	}

}

void cSampleImporter::calculateMemUsage()
{
	int32_t memoryUsageAddCurrentSelect=0;

	if(mtProject.used_memory > mtProject.max_memory)
	{
		memoryUsage = 100;
	}
	else
	{
		memoryUsage = (mtProject.used_memory*100.0)/mtProject.max_memory;
	}

	memoryUsageAddCurrentSelect= mtProject.used_memory + currentSelectMemorySize;

	if(memoryUsageAddCurrentSelect < 0)
	{
		memoryUsageAddCurrentSelect=0;
	}

	if((uint32_t)memoryUsageAddCurrentSelect > mtProject.max_memory)
	{
		memoryUsageAdd.value = 100;
		fullMemoryFlag = 1;
	}
	else
	{
		memoryUsageAdd.value = (memoryUsageAddCurrentSelect*100.0)/mtProject.max_memory;
		fullMemoryFlag = 0;
	}
}

/*void cSampleImporter::calculateCurrentSelectMemorySize()
{
//	if(!isWavFile(&locationFileList[selectedFile][0])) return;

	char file_path[255];

	strcpy(file_path, actualPath);
	if(dirLevel > 0)strcat(file_path, "/");
	strcat(file_path, &locationExplorerList[selectedFile][0]);

	currentSelectMemorySize = 2* fileManager.samplesLoader.waveLoader.getInfoAboutWave(file_path);
	if(mtProject.instrument[selectedSlot].isActive) currentSelectMemorySize -= 2*mtProject.instrument[selectedSlot].sample.length;
}*/

void cSampleImporter::calculateLoadProgress()
{
	loadProgress = fileManager.samplesLoader.getCurrentProgress();
}

void cSampleImporter::calculateCopyingProgress()
{
	copyingProgress = fileManager.samplesImporter.getProgress();
}

//==============================================================================================
void cSampleImporter::playSdFile()
{
	if(currentCopyStatusFlag || currentLoadStatusFlag) return;
//	if(!isWavFile(&locationFileList[selectedFile][0])) return;

	char file_path[255];

	strcpy(file_path, actualPath);
	if(dirLevel > 0)strcat(file_path, "/");
	strcat(file_path, &locationExplorerList[selectedFile][0]);


	if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	stopPlaying();

	if(playMode != playModeSdFile)
	{
		engine.prevSdConnect();
	}
	playMode = playModeSdFile;

	FsFile wavHeader = SD.open(file_path);

	if(!wavHeader)
	{
		wavHeader.close();
		SD.begin(SdioConfig(DMA_SDIO));
		return;
	}

	strWavFileHeader header;
	readHeader(&header,&wavHeader);

	wavHeader.close();
	if(header.AudioFormat == 3) playSdWavFloat.play(file_path);
	else
	{
		if(header.bitsPerSample == 16) playSdWav.play(file_path);
		else if (header.bitsPerSample == 24) playSdWav24Bit.play(file_path);
	}


}


void cSampleImporter::playSampleFromBank()
{
	if(currentCopyStatusFlag || currentLoadStatusFlag) return;
	if(!mtProject.instrument[selectedSlot].isActive) return;

	if(sequencer.getSeqState() == 1)
	{
		sequencer.stop();
	}

	stopPlaying();

	playMode = playModeSampleBank;

	instrumentPlayer[0].noteOnforPrev(mtProject.instrument[selectedSlot].sample.address,
									  mtProject.instrument[selectedSlot].sample.length);
}


void cSampleImporter::stopPlaying()
{
	if(playMode == playModeSdFile)
	{
		playSdWav.stop();
		playSdWavFloat.stop();
		playSdWav24Bit.stop();
	}
	else if(playMode == playModeSampleBank)
	{
		instrumentPlayer[0].noteOff();
	}

	playMode = playModeStop;
}

uint8_t preview(uint8_t state)
{
	if(state == 0)  SI->stopPlaying();
	else if (state == 1)
	{

		switch(SI->selectedPlace)
		{
		case 0: SI->playSdFile(); 			break;
		case 1: SI->playSampleFromBank(); 	break;

		}
	}

	return 1;
}

void cSampleImporter::updateSelection()
{
	display.refreshControl(explorerListControl);
	display.refreshControl(instrumentListControl);
}

bool cSampleImporter::checkIfValidSelection(uint8_t positionToCheck)
{
	if(!(SI->locationExplorerList[positionToCheck][0] == '/'))
	{
		return true;
	}

	return false;
}

uint8_t cSampleImporter::handleSelecting(uint8_t selectStart, uint8_t selectMax, int16_t value, uint8_t fileCheck)
{
	int8_t sign=1;
	int16_t position;

	if(value<0)
	{
		sign=-1;
		value *= -1;
	}

	position = selectStart;

	for(int i=0;i<value;i++)
	{
		if((position + (1*sign)) >= 0 && (position + (1*sign)) < selectMax)
		{
			position += (1 *sign);

			if(!fileCheck || checkIfValidSelection(position))
			{
				if(selectionTab[position]==1)
				{
					if(sign<0)
					{
						selectionTab[position+1]=0;
					}
					if(sign>0)
					{
						selectionTab[position-1]=0;
					}

					if(selectionLength)
					{
						selectionLength--;
					}
				}
				else
				{
					if(selectionLength < MAX_SELECT)
					{
						selectionLength++;
						selectionTab[position]=1;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return position;
}

void cSampleImporter::cancelSelect()
{
	memset(SI->selectionTab,0,sizeof(SI->selectionTab));
	selectionLength=0;

	updateSelection();
}

int16_t cSampleImporter::getSelectionStart()
{
	int16_t selStart=-1;

	for(size_t i=0;i<sizeof(selectionTab);i++)
	{
		if(selectionTab[i]==1)
		{
			selStart=i;
			break;
		}
	}

	return selStart;
}

char *cSampleImporter::parseNewName(uint8_t num)
{
	if(num<10)
	{
		sprintf(instrToCreate,"instr0%d.wav",num);
	}
	else
	{
		sprintf(instrToCreate,"instr%d.wav",num);
	}

	return instrToCreate;
}

uint8_t cSampleImporter::getActiveInstrInSelection()
{
	uint8_t activeInstr=0;

	for(uint8_t instr = instrCopyStart; instr < (instrCopyStart + copyElementMax); instr++)
	{
		if(mtProject.instrument[instr].isActive == 1)
		{
			activeInstr++;
		}
	}

	return activeInstr;
}

void cSampleImporter::handleSequenceCopyingLoading()
{
	//////////////////////////////////////COPYING////////////////////////////
	currentCopyStatusFlag = fileManager.samplesImporter.getState();

	if(currentCopyStatusFlag)
	{
		calculateCopyingProgress();
		showCopyingHorizontalBar();
	}

	if((!currentCopyStatusFlag) && (lastCopyStatusFlag))
	{
		if(copyElementMax)
		{
			char projectSamplePath[255];

			if(copyType == 2)
			{
				while(copyElement < copyElementMax)
				{
					if(mtProject.instrument[instrCopyStart+copyElement].isActive == 1)
					{
						break;
					}
					else
					{
						copyElement++;
					}
				}
			}

			if((copyElement == (copyElementMax-1)) || (instrCopied == (instrActiveInSel - 1)))
			{
				fileManager.setLoadLength(copyElementMax);
				fileManager.setAutoLoadFlag();
			}

			if(copyElement != copyElementMax)
			{
				if(copyType == 1)
				{
					fileManager.assignSampleToInstrument(actualPath, &locationExplorerList[getSelectionStart() + copyElement][0], selectedSlot + copyElement);
				}
				else if(copyType == 2)
				{
					if(mtProject.instrument[instrCopyStart + copyElement].isActive == 1)
					{
						strcpy(projectSamplePath,fileManager.currentProjectPatch);
						strcat(projectSamplePath,"/samples");

						fileManager.assignSampleToInstrument(projectSamplePath,SI->parseNewName(instrCopyStart + copyElement), selectedSlot + copyElement);
						memcpy(&mtProject.instrument[selectedSlot+copyElement],&mtProject.instrument[instrCopyStart+copyElement],sizeof(mtProject.instrument[0]));
						instrCopied++;
					}
				}

				copyElement++;
			}
		}

		showDefaultScreen();
	}

	lastCopyStatusFlag = currentCopyStatusFlag;

	//////////////////////////////////////LOADING////////////////////////////
	currentLoadStatusFlag = fileManager.samplesLoader.getStateFlag();
	if(currentLoadStatusFlag)
	{
		calculateLoadProgress();
		showLoadHorizontalBar();
	}
	if((!currentLoadStatusFlag) && (lastLoadStatusFlag))
	{
		if(!fileManager.samplesLoader.getStateFlag())
		{
			firstMemBarLoadFlag=2;
			showDefaultScreen();
		}

		if(firstMemBarLoadFlag == 0)
		{
			firstMemBarLoadFlag = 1;
		}
	}

	if(currentLoadStatusFlag && !lastLoadStatusFlag)
	{
		if(currSelectPlace==1)
		{
			fileManager.saveProject();
		}

		listAllFoldersFirst();

		copyType=0;
	}

	lastLoadStatusFlag=currentLoadStatusFlag;
}

uint8_t cSampleImporter::willCopyInstrFit(uint8_t length)
{
	int32_t memoryUsedBySelection=0;
	int32_t pasteLocationMemUsage=0;
	int32_t memoryUsageEstimate=0;
	uint8_t retValue;

	for(int i=instrCopyStart;i<(length+instrCopyStart);i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			memoryUsedBySelection += 2*mtProject.instrument[i].sample.length;
		}
	}

	for(int i=selectedSlot;i<(length+selectedSlot);i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			pasteLocationMemUsage += 2*mtProject.instrument[i].sample.length;
		}
	}

	memoryUsageEstimate = memoryUsedBySelection - pasteLocationMemUsage;

	if((int32_t)mtProject.used_memory + memoryUsageEstimate<(int32_t)mtProject.max_memory)
	{
		retValue=1;
	}
	else
	{
		retValue=0;
	}

	return retValue;
}

void cSampleImporter::resetInstrSel()
{
	instrCopyStart=0;
	copyElementMax=0;
}





