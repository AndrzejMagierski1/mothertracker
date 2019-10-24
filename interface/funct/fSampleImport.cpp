

#include <sampleImporter.h>
#include "mtFileManager.h"

#include "Encoder.h"

#include "mtAudioEngine.h"
#include <projectEditor.h>
#include "mtLED.h"

#include "keyScanner.h"

enum valueMapDirecion
{
	valueMapDirectionLeft,
	valueMapDirectionRight,
	valueMapDirectionUp,
	valueMapDirectionDown
};

constexpr char smallKeyboard[KEYBOARD_SIZE] =
{
		 	 '1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' , 0 ,
		 	 'q', 'w', 'e' ,'r' ,'t' ,'y' ,'u' ,'i' ,'o' ,'p' ,'-' ,'+',
			 'a', 's', 'd' ,'f' ,'g' ,'h' ,'j' ,'k' ,'l' ,'@' , 1 ,
			 'z', 'x', 'c' ,'v' ,'b' ,'n' ,'m' , ' '
};

constexpr char bigKeyboard[KEYBOARD_SIZE] =
{
			 '1', '2', '3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'0' , 0 ,
		 	 'Q', 'W', 'E' ,'R' ,'T' ,'Y' ,'U' ,'I' ,'O' ,'P' ,'-' ,'+',
			 'A', 'S', 'D' ,'F' ,'G' ,'H' ,'J' ,'K' ,'L', '@', 1 ,
			 'Z', 'X', 'C' ,'V' ,'B' ,'N' ,'M' , ' '
};

//constexpr uint8_t valueMap[4][42] = przechodzenie przez sciany
//{
//		{
//			10,0,1,2,3,4,5,6,7,8,9,
//			22,11,12,13,14,15,16,17,18,19,20,21,
//			33,23,24,25,26,27,28,29,30,31,32,
//			41,34,35,36,37,38,39,40
//		},
//
//		{
//			1,2,3,4,5,6,7,8,9,10,0,
//			12,13,14,15,16,17,18,19,20,21,22,11,
//			24,25,26,27,28,29,30,31,32,33,23,
//			35,36,37,38,39,40,41,34
//		},
//
//		{
//			34,35,36,37,38,39,40,41,41,41,41,
//			0,1,2,3,4,5,6,7,8,9,10,10,
//			11,12,13,14,15,16,17,18,19,20,21,
//			23,24,25,26,27,28,29,30,
//		},
//
//		{
//			11,12,13,14,15,16,17,18,19,20,21,
//			23,24,25,26,27,28,29,30,31,32,33,33,
//			34,35,36,37,38,39,40,41,41,41,41,
//			0,1,2,3,4,5,6,7
//		},
//};

constexpr uint8_t valueMap[4][42] =
{
		{
			0,0,1,2,3,4,5,6,7,8,9,
			11,11,12,13,14,15,16,17,18,19,20,21,
			23,23,24,25,26,27,28,29,30,31,32,
			34,34,35,36,37,38,39,40
		},

		{
			1,2,3,4,5,6,7,8,9,10,10,
			12,13,14,15,16,17,18,19,20,21,22,22,
			24,25,26,27,28,29,30,31,32,33,33,
			35,36,37,38,39,40,41,41
		},

		{
			0,1,2,3,4,5,6,7,8,9,10,
			0,1,2,3,4,5,6,7,8,9,10,10,
			11,12,13,14,15,16,17,18,19,20,21,
			23,24,25,26,27,28,29,30,
		},

		{
			11,12,13,14,15,16,17,18,19,20,21,
			23,24,25,26,27,28,29,30,31,32,33,33,
			34,35,36,37,38,39,40,41,41,41,41,
			34,35,36,37,38,39,40,41
		},
};

constexpr uint8_t valueMapPads[48] =
{
	0,1,2,3,4,5,6,7,8,9,10,10,
	11,12,13,14,15,16,17,18,19,20,21,22,
	23,24,25,26,27,28,29,30,31,32,33,33,
	34,35,36,37,38,39,40,41,41,41,41,41
};

constexpr uint8_t keyPositionToPads[42] =
{
	0,1,2,3,4,5,6,7,8,9,10,
	12,13,14,15,16,17,18,19,20,21,22,23,
	24,25,26,27,28,29,30,31,32,33,34,
	36,37,38,39,40,41,42,43
};

constexpr uint8_t BACKSPACE_PAD_1 = 10;
constexpr uint8_t BACKSPACE_PAD_2 = 11;

constexpr uint8_t CAPS_LOCK_PAD_1 = 34;
constexpr uint8_t CAPS_LOCK_PAD_2 = 35;

constexpr uint8_t SPACE_PAD_1 = 43;
constexpr uint8_t SPACE_PAD_2 = 44;
constexpr uint8_t SPACE_PAD_3 = 45;
constexpr uint8_t SPACE_PAD_4 = 46;
constexpr uint8_t SPACE_PAD_5 = 47;

constexpr uint8_t F_PAD = 27;

constexpr uint8_t J_PAD = 30;

cSampleImporter sampleImporter;
static cSampleImporter* SI = &sampleImporter;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functChangeFolder(uint8_t button);
static  uint8_t functChangeInstrument(uint8_t button);

static  uint8_t functEnter();
static  uint8_t functRename();
static  uint8_t functShift(uint8_t state);

static  uint8_t functInstrumentAdd();
static  uint8_t functInstrumentWavetableAdd();
static  uint8_t functInstrumentDelete();


static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static uint8_t functCopyPaste();
static uint8_t functPaste();

static uint8_t functConfirmKey();

uint8_t preview(uint8_t state);



static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functConfirmRename();
static  uint8_t functCancelRename();

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

	processDeleting();
	processDirFileSizes();
}

void cSampleImporter::start(uint32_t options)
{
	moduleRefresh = 1;

	Encoder.setAcceleration(0);

	//selectedFile = 0;
	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
	{
		//mtProject.values.lastUsedInstrument = 0;
		selectedSlot = 0;
	}
	else
	{

		selectedSlot = mtProject.values.lastUsedInstrument;
	}





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

	FM->setPadsGlobal(functPads);

	showDefaultScreen();
	setDefaultScreenFunct();

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

	if(selectedPlace == 1) FM->setButtonObj(interfaceButton2, buttonPress, functRename);
	else FM->setButtonObj(interfaceButton2, buttonPress, functEnter);

	FM->setButtonObj(interfaceButton3, buttonPress, functInstrumentWavetableAdd);
	FM->setButtonObj(interfaceButton4, preview);

	//FM->setButtonObj(interfaceButton4, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton5, buttonPress, functInstrumentDelete);

	FM->setButtonObj(interfaceButton6, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton7, buttonPress, functChangeInstrument);

	FM->setButtonObj(interfaceButtonCopy, buttonPress, functCopyPaste);
//	FM->setButtonObj(interfaceButtonPaste, buttonPress, functPaste);

}


//==============================================================================================================
static  uint8_t functChangeFolder(uint8_t button)
{
	if(SI->isBusy) return 1;

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
	SI->checkWavetableLabel();
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functChangeInstrument(uint8_t button)
{
	if(SI->isBusy) return 1;

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
	SI->hideAddWT();
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functInstrumentAdd()
{
	if(SI->isBusy) return 1;
	SI->sampleType = mtSampleTypeWaveFile;
	SI->SelectFile();

	return 1;
}

static  uint8_t functInstrumentWavetableAdd()
{
	if(SI->isBusy) return 1;
	uint8_t length = SI->selectionLength ? SI->selectionLength : 1;

	for(uint8_t i = 0; i < length; i++ )
	{
		if( ! SI->currentFolderIsWavetableFlag[SI->selectedFile + i] ) return 1;
	}

	for(uint8_t i = 0; i < length; i++ )
	{
		mtProject.instrument[SI->selectedSlot + i].sample.type = mtSampleTypeWavetable;
	}
	SI->sampleType = mtSampleTypeWavetable;
	SI->SelectFile();

	return 1;
}


static  uint8_t functInstrumentDelete()
{
	if(SI->isBusy) return 1;

	if(SI->selectedPlace == 1)
	{
		SI->isBusy = 1; // processDeleting() powinna zdjac blokade

		if(SI->currSelectPlace == 1 && SI->selectionLength)
		{
			SI->deleteInProgress = 1;
			SI->deleteStart = SI->getSelectionStart();
			SI->deleteCurrentPos = SI->deleteStart;
		}
		else
		{
			if(mtProject.instrument[SI->selectedSlot].isActive)
			{
				mtProject.used_memory -= 2* mtProject.instrument[SI->selectedSlot].sample.length;
			}

			fileManager.deleteInstrument(SI->selectedSlot);

			SI->handleMemoryBar();

			SI->listInstrumentSlots();
			SI->showInstrumentsList();

			SI->isBusy = 0;
		}
	}

	return 1;
}



//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	if(SI->keyboardActiveFlag == 1) return 1;
	if(SI->isBusy) return 1;

	switch(SI->selectedPlace)
	{
	case 0: SI->changeFileSelection(value); break;
	case 1: SI->changeInstrumentSelection(value); break;

	}


	return 1;
}

static  uint8_t functEnter()
{
	if(SI->isBusy) return 1;

	switch(SI->selectedPlace)
	{
	case 0: SI->BrowseOrAdd(); break;
	case 1: SI->BrowseOrAdd(); break;

	}

	return 1;
}

static  uint8_t functRename()
{
	if(SI->isBusy) return 1;

	strncpy(SI->name,mtProject.instrument[SI->selectedSlot].sample.file_name,32);
	SI->editPosition = strlen(SI->name);
	SI->keyboardPosition = BACKSPACE_PAD_1;
	SI->lastPressedPad = BACKSPACE_PAD_1;
	leds.setLED(BACKSPACE_PAD_1, 1, 31);
	leds.setLED(BACKSPACE_PAD_2, 1, 31);

	SI->keyboardActiveFlag = 1;

	SI->FM->clearButton(interfaceButton2);
	SI->FM->clearButton(interfaceButton5);

	SI->FM->setButtonObj(interfaceButton2, buttonPress, functCancelRename);
	SI->FM->setButtonObj(interfaceButton5, buttonPress, functConfirmRename);

	SI->showRenameKeyboard();
	return 1;
}

static  uint8_t functConfirmRename()
{
	strncpy(mtProject.instrument[SI->selectedSlot].sample.file_name,SI->name,32);

	SI->showFileList();

	SI->listInstrumentSlots();
	SI->showInstrumentsList();

	SI->handleMemoryBar();

	SI->activateLabelsBorder();

	SI->setDefaultScreenFunct();
	SI->showDefaultScreen();

	SI->displayDelete(SI->selectedPlace);
	fileManager.instrumentIsChangedFlag[SI->selectedSlot] = 1;
	mtProject.values.projectNotSavedFlag = 1;
	SI->keyboardActiveFlag = 0;

	return 1;
}
static  uint8_t functCancelRename()
{
	SI->showFileList();

	SI->listInstrumentSlots();
	SI->showInstrumentsList();

	SI->handleMemoryBar();

	SI->activateLabelsBorder();
	SI->keyboardActiveFlag = 0;

	SI->setDefaultScreenFunct();
	SI->showDefaultScreen();

	SI->displayDelete(SI->selectedPlace);

	SI->keyboardActiveFlag = 0;
	return 1;
}

static uint8_t functCopyPaste()
{
	if(SI->isBusy) return 1;

	if(tactButtons.isButtonPressed(interfaceButtonShift))
	{
		functPaste();
	}
	else
	{
		if((SI->selectedPlace == 1) && (SI->currSelectPlace == 1))
		{
			SI->instrCopyStart = SI->getSelectionStart();
			SI->copyElementMax = SI->selectionLength;
		}
	}

	return 1;
}

static uint8_t functPaste()
{
	if(SI->currentCopyStatusFlag || SI->currentLoadStatusFlag || SI->keyboardActiveFlag) return 1;

	if(SI->copyElementMax && SI->instrCopyStart != SI->selectedSlot)
	{
		SI->isBusy = 1;
		char projectSamplePath[255];
		uint8_t willFit = 1;
		uint8_t selectOverMax = 0;

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

				strcpy(projectSamplePath,"Workspace/samples");

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
				fileManager.assignSampleToInstrument(projectSamplePath, SI->parseNewName(SI->instrCopyStart+SI->copyElement), SI->selectedSlot + SI->copyElement,SI->sampleType);
				memcpy(&mtProject.instrument[SI->selectedSlot+SI->copyElement],&mtProject.instrument[SI->instrCopyStart+SI->copyElement],sizeof(mtProject.instrument[0]));

				SI->copyElement++;
				SI->instrCopied++;
			}
		}
	}

	return 1;
}

static  uint8_t functShift(uint8_t state)
{
	if(state ==0)
	{
		if(SI->keyboardActiveFlag == 1)
		{
			return 1;
		}

		SI->selectionActive=0;
		SI->frameSelectMode(SI->currSelectPlace,0);

		if(SI->selectionLength == 1)
		{
			if(SI->currSelectPlace == 0)
			{
				SI->cancelSelect();
			}
		}
	}
	else if(state == 1)
	{
		if(SI->keyboardActiveFlag == 1)
		{
			functConfirmKey();
			return 1;
		}

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
	if(SI->isBusy) return 1;

	if(SI->keyboardActiveFlag)
	{
		SI->keyboardPosition = valueMap[valueMapDirectionLeft][SI->keyboardPosition];
		SI->showKeyboard();
		return 1;
	}
	if(SI->selectedPlace > 0)
	{
		SI->selectedPlace--;
		if(SI->selectedPlace != 0) SI->hideAddWT();
		else if(SI->selectedPlace == 0)
		{
			SI->checkWavetableLabel();
		}

		if(SI->currSelectPlace == 1)
		{
			SI->cancelSelect();
		}

		SI->handleMemoryBar();
		SI->displayDelete(0);
		SI->displayRename(0);
		SI->resetInstrSel();
		SI->FM->clearButton(interfaceButton2);
		SI->FM->setButtonObj(interfaceButton2, buttonPress, functEnter);

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
	if(SI->isBusy) return 1;

	if(SI->keyboardActiveFlag)
	{
		SI->keyboardPosition = valueMap[valueMapDirectionRight][SI->keyboardPosition];
		SI->showKeyboard();
		return 1;
	}
	if(SI->selectedPlace < 1)
	{
		SI->displayRename(1);
		SI->displayDelete(1);
		SI->FM->clearButton(interfaceButton2);
		SI->FM->setButtonObj(interfaceButton2, buttonPress, functRename);
		SI->selectedPlace++;
	}
	if(SI->selectedPlace != 0) SI->hideAddWT();
	else if(SI->selectedPlace == 0)
	{
		SI->checkWavetableLabel();
	}

	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	if(SI->isBusy) return 1;

	if(SI->keyboardActiveFlag)
	{
		SI->keyboardPosition = valueMap[valueMapDirectionUp][SI->keyboardPosition];
		SI->showKeyboard();
		return 1;
	}
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
	if(SI->isBusy) return 1;

	if(SI->keyboardActiveFlag)
	{
		SI->keyboardPosition = valueMap[valueMapDirectionDown][SI->keyboardPosition];
		SI->showKeyboard();
		return 1;
	}
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
	if(SI->isBusy) return 1;

	if(sequencer.getSeqState() == 0)
	{
		sequencer.play();
	}
	else
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
	if(SI->isBusy) return 1;

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

	checkWavetableLabel();

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

void cSampleImporter::processDirFileSizes()
{
	if(openingInProgress == 1)
	{
		char filePath[255];

		strcpy(filePath, actualPath);
		if(dirLevel > 0)
		{
			strcat(filePath, "/");
		}

		strcat(filePath, &locationExplorerList[openCurrentPos][0]);

		currentFolderMemoryFileUsage[openCurrentPos]= 2* fileManager.samplesLoader.waveLoader.getInfoAboutWave(filePath);
		currentFolderIsWavetableFlag[openCurrentPos] = fileManager.samplesLoader.wavetableLoader.isWavetable(filePath);
		//todo: wywoływane są dwie funkcje które otwierają 2 razy plik - trzeba to zoptymalizowac
		openCurrentPos++;

		uint8_t progress = (((openCurrentPos - openCalcStart) * 100) / (openCalcEnd-openCalcStart));

		showOpeningHorizontalBar(progress);

		if(openCurrentPos == openCalcEnd)
		{
			sdLocation.close();

			for(uint8_t i = openCalcStart; i < openCalcEnd; i++)
			{
				explorerNames[i] = &locationExplorerList[i][0];
			}

			openingInProgress = 0;
			isBusy = 0;

			hideHorizontalBar();
			showFilesTree();
		}
	}
}

void cSampleImporter::listOnlyWavFromActualPath(uint8_t startPoint)
{
	sdLocation.close();
	sdLocation.open(actualPath, O_READ);

	locationExplorerCount += sdLocation.createFilesList(startPoint,locationExplorerList, (list_length_max-startPoint),2);

	if(locationExplorerCount <= startPoint)
	{
		isBusy = 0;
		openingInProgress = 0;
		showFilesTree();
	}
	else
	{
		openCalcStart = startPoint;
		openCalcEnd = locationExplorerCount;

		openCurrentPos = openCalcStart;
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
	isBusy = 1; // processDirFileSizes() powinna zdjac flage tutaj ustawiona
	locationExplorerCount=0;

	for(int i=0;i<list_length_max;i++)
	{
		explorerNames[i]=NULL;
	}

	openingInProgress = 1;

	listOnlyFolderNames(actualPath,0);
	listOnlyWavFromActualPath(locationExplorerCount);
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
		isBusy = 1;

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
			fileManager.assignSampleToInstrument(actualPath, &locationExplorerList[position + copyElement][0], selectedSlot +  copyElement,sampleType);
			copyElement++;
		}
		else
		{
			fileManager.setAutoLoadFlag();
			fileManager.setStart(selectedSlot);
			fileManager.assignSampleToInstrument(actualPath, &locationExplorerList[selectedFile][0], selectedSlot,sampleType);
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


	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
	{
		sequencer.stop();
	}

	stopPlaying();


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

	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
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
	if(SI->isBusy) return 1;

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
					fileManager.assignSampleToInstrument(actualPath, &locationExplorerList[getSelectionStart() + copyElement][0], selectedSlot + copyElement,sampleType);
				}
				else if(copyType == 2)
				{
					if(mtProject.instrument[instrCopyStart + copyElement].isActive == 1)
					{
						//strcpy(projectSamplePath,fileManager.currentProjectPatch);
						strcpy(projectSamplePath,"Workspace/samples");

						fileManager.assignSampleToInstrument(projectSamplePath,SI->parseNewName(instrCopyStart + copyElement), selectedSlot + copyElement,sampleType);
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

			for(uint32_t i = selectedSlot ; i < (selectedSlot + copyElementMax); i++)
			{
				fileManager.instrumentIsChangedFlag[i] = 1;
			}

			fileManager.instrumentForcedSaveFlag = 1;

			mtProject.values.projectNotSavedFlag = 1;

			isBusy = 0;
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
			//fileManager.saveProject(); todo: saveWorkspace
		}

		//listAllFoldersFirst();//?

		copyType=0;
	}

	lastLoadStatusFlag=currentLoadStatusFlag;
}

void cSampleImporter::processDeleting()
{
	if(deleteInProgress)
	{
		if(mtProject.instrument[deleteCurrentPos].isActive)
		{
			mtProject.used_memory -= 2* mtProject.instrument[deleteCurrentPos].sample.length;
		}

		uint8_t progress = ((deleteCurrentPos - deleteStart) * 100) / selectionLength;

		showDeletingHorizontalBar(progress);

		fileManager.deleteInstrument(deleteCurrentPos);

		deleteCurrentPos++;

		if(deleteCurrentPos == (deleteStart + selectionLength))
		{
			hideHorizontalBar();

			cancelSelect();
			handleMemoryBar();

			listInstrumentSlots();
			showInstrumentsList();

			deleteInProgress = 0;
			isBusy = 0;
		}
	}
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

static uint8_t functConfirmKey()
{
	if(SI->keyboardActiveFlag)
	{
		if(SI->editPosition > 31) return 1;

		//****************************************************ledy
		if(SI->lastPressedPad == BACKSPACE_PAD_1 || SI->lastPressedPad == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 0, 0);
			leds.setLED(BACKSPACE_PAD_2, 0, 0);
		}
		else if(SI->lastPressedPad == CAPS_LOCK_PAD_1 || SI->lastPressedPad == CAPS_LOCK_PAD_2) //capslock
		{
			if(SI->keyboardShiftFlag)
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, 10);
				leds.setLED(CAPS_LOCK_PAD_2, 1, 10);
			}
			else
			{
				leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
				leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
			}

		}
		else if(SI->lastPressedPad >= SPACE_PAD_1 && SI->lastPressedPad <= SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 0, 0);
			}
		}
		else
		{
			if(SI->lastPressedPad != F_PAD && SI->lastPressedPad != J_PAD) leds.setLED(SI->lastPressedPad,0,0);
			else leds.setLED(SI->lastPressedPad,1,10);
		}


		SI->lastPressedPad = keyPositionToPads[SI->keyboardPosition];

		if(keyPositionToPads[SI->keyboardPosition] == BACKSPACE_PAD_1 || keyPositionToPads[SI->keyboardPosition] == BACKSPACE_PAD_2) //backspace
		{
			leds.setLED(BACKSPACE_PAD_1, 1, 31);
			leds.setLED(BACKSPACE_PAD_2, 1, 31);
		}
		else if(keyPositionToPads[SI->keyboardPosition] == CAPS_LOCK_PAD_1 || keyPositionToPads[SI->keyboardPosition] == CAPS_LOCK_PAD_2) //capslock
		{
			leds.setLED(CAPS_LOCK_PAD_1, 1, 31);
			leds.setLED(CAPS_LOCK_PAD_2, 1, 31);
		}
		else if(keyPositionToPads[SI->keyboardPosition] >= SPACE_PAD_1 && keyPositionToPads[SI->keyboardPosition] <=SPACE_PAD_5) //space
		{
			for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
			{
				leds.setLED(i, 1, 31);
			}
		}
		else
		{
			leds.setLED(keyPositionToPads[SI->keyboardPosition],1,31);
		}
		//////////////////////////////////////
		if(smallKeyboard[SI->keyboardPosition] > 1)
		{
			if(SI->editPosition == 31) return 1;
			uint8_t localNameLen = strlen(SI->name);
			if(SI->editPosition < localNameLen)
			{
				for(uint8_t i = localNameLen; i >= SI->editPosition ; i-- )
				{
					SI->name[i+1] = SI->name[i];
				}
			}

			SI->name[SI->editPosition] = SI->keyboardShiftFlag ? bigKeyboard[SI->keyboardPosition] : smallKeyboard[SI->keyboardPosition];

			SI->editPosition++;
		}
		else if(smallKeyboard[SI->keyboardPosition] == 0)
		{
			if(SI->editPosition == 0 ) return 1;

			SI->name[SI->editPosition-1] = 0;
			SI->editPosition--;


		}
		else if(smallKeyboard[SI->keyboardPosition] == 1)
		{
			SI->keyboardShiftFlag = ! SI->keyboardShiftFlag;
			SI->showKeyboard();

		}

		SI->showKeyboardEditName();
		return 1;
	}
	return 0;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if((state == 1) || (state == 2))
	{
		if(SI->keyboardActiveFlag)
		{
			if(SI->lastPressedPad == BACKSPACE_PAD_1 || SI->lastPressedPad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 0, 0);
				leds.setLED(BACKSPACE_PAD_2, 0, 0);
			}
			else if(SI->lastPressedPad == CAPS_LOCK_PAD_1 || SI->lastPressedPad == CAPS_LOCK_PAD_2) //capslock
			{
				if(SI->keyboardShiftFlag)
				{
					leds.setLED(CAPS_LOCK_PAD_1, 1, 10);
					leds.setLED(CAPS_LOCK_PAD_2, 1, 10);
				}
				else
				{
					leds.setLED(CAPS_LOCK_PAD_1, 0, 0);
					leds.setLED(CAPS_LOCK_PAD_2, 0, 0);
				}

			}
			else if(SI->lastPressedPad >= SPACE_PAD_1 && SI->lastPressedPad <= SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 0, 0);
				}
			}
			else
			{
				if(SI->lastPressedPad != F_PAD && SI->lastPressedPad != J_PAD) leds.setLED(SI->lastPressedPad,0,0);
				else leds.setLED(SI->lastPressedPad,1,10);
			}


			SI->lastPressedPad = pad;

			if(pad == BACKSPACE_PAD_1 || pad == BACKSPACE_PAD_2) //backspace
			{
				leds.setLED(BACKSPACE_PAD_1, 1, 31);
				leds.setLED(BACKSPACE_PAD_2, 1, 31);
			}
			else if(pad == CAPS_LOCK_PAD_1 || pad == CAPS_LOCK_PAD_2) //capslock
			{
				leds.setLED(CAPS_LOCK_PAD_1, 1, 31);
				leds.setLED(CAPS_LOCK_PAD_2, 1, 31);
			}
			else if(pad >= SPACE_PAD_1 && pad <=SPACE_PAD_5) //space
			{
				for(uint8_t i = SPACE_PAD_1; i<= SPACE_PAD_5; i++)
				{
					leds.setLED(i, 1, 31);
				}
			}
			else
			{
				leds.setLED(pad,1,31);
			}

			SI->keyboardPosition = valueMapPads[pad];


			if(SI->editPosition > 31) return 1;
			if(smallKeyboard[SI->keyboardPosition] > 1)
			{
				if(SI->editPosition == 31) return 1;
				uint8_t localNameLen = strlen(SI->name);
				if(SI->editPosition < localNameLen)
				{
					for(uint8_t i = localNameLen; i >= SI->editPosition ; i-- )
					{
						SI->name[i+1] = SI->name[i];
					}
				}

				SI->name[SI->editPosition] = SI->keyboardShiftFlag ? bigKeyboard[SI->keyboardPosition] : smallKeyboard[SI->keyboardPosition];
				SI->name[SI->editPosition + 1] = 0;
				SI->editPosition++;
			}
			else if(smallKeyboard[SI->keyboardPosition] == 0)
			{
				if(SI->editPosition == 0 ) return 1;

				SI->name[SI->editPosition-1] = 0;
				SI->editPosition--;


			}
			else if(smallKeyboard[SI->keyboardPosition] == 1)
			{
				SI->keyboardShiftFlag = ! SI->keyboardShiftFlag;
//				PE->showKeyboard();
			}
			SI->showKeyboard();
			SI->showKeyboardEditName();
			return 1;
		}

		return 1;
	}
	else if(state == 0)
	{
		if(SI->keyboardActiveFlag)
		{

		}
	}




	return 1;
}
void cSampleImporter::checkWavetableLabel()
{
	uint8_t length = selectionLength ? selectionLength : 1;
	uint8_t wavetableFlag = 1;
	for(uint8_t i = 0; i < length; i++)
	{
		if(!currentFolderIsWavetableFlag[selectedFile + i])
		{
			wavetableFlag = 0;
			break;
		}
	}

	if(wavetableFlag) showAddWT();
	else hideAddWT();
}


