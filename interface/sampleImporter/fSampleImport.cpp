

#include "sampleImporter/sampleImporter.h"
//#include "mtFileManager.h"
#include "fileManager.h"

#include "Encoder.h"

#include "mtAudioEngine.h"
#include "projectEditor/projectEditor.h"
#include "mtLED.h"

#include "keyScanner.h"

#include "sdCardDetect.h"

#include "debugLog.h"
#include "mtPadsBacklight.h"
#include "mtPadBoard.h"

cSampleImporter sampleImporter;
static cSampleImporter* SI = &sampleImporter;

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);

static  uint8_t functPlayAction();



static  uint8_t functChangeFolder(uint8_t button);
static  uint8_t functChangeInstrument(uint8_t button);

static  uint8_t functEnter();
static  uint8_t functRename();
static  uint8_t functShift(uint8_t state);

static  uint8_t functInstrumentAdd();
static 	uint8_t functInstrumentAddNext();
//static  uint8_t functInstrumentWavetableAdd();
static  uint8_t functInstrumentDelete();

static uint8_t functDelete(uint8_t state);

static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

static uint8_t functCopyPaste();

static uint8_t functConfirmKey();

static uint8_t previewFile(uint8_t state);



static  uint8_t functEncoder(int16_t value);


static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functConfirmRename();
static  uint8_t functCancelRename();
static  uint8_t functAutoNameRename();

static uint8_t functDeleteBackspace(uint8_t state);

static uint8_t functStopPatternYes();
static uint8_t functStopPatternNo();

static void cancelAnotherSelect();



void cSampleImporter::update()
{
	uint8_t managerStatus = newFileManager.getStatus();

	if(managerStatus == fmBrowseSamplesEnd)
	{
		locationExplorerCount = newFileManager.getBrowsedFilesList(&explorerNames, &currentFolderMemoryFileUsage);

		showFilesTree();
		handleMemoryBar();
		AddEnterOrRename();
		AddNextControl();
		previewColorControl();
		cancelSelect(listFiles);
		setSelect();
		FM->unblockAllInputs();

		newFileManager.clearStatus();
	}
	else if(managerStatus == fmImportSamplesEnd)
	{
		if(addNextFlag)
		{
			addNextFlag = 0;
			selectedSlot = (selectedSlot+addNextOffset >= INSTRUMENTS_COUNT) ? INSTRUMENTS_COUNT-1 : selectedSlot + addNextOffset;
		}

		cancelSelect(listInstruments);
		SI->listInstrumentSlots();
		SI->refreshInstrumentsList();
		SI->handleMemoryBar();
		FM->unblockAllInputs();

		newFileManager.clearStatus();
	}
	else if(managerStatus == fmCopyingInstrumentsEnd)
	{
		SI->listInstrumentSlots();
		SI->refreshInstrumentsList();
		renameColorControl();
		previewColorControl();
		selectionActive[listInstruments] = 0;
		SI->handleMemoryBar();
		FM->unblockAllInputs();

		newFileManager.clearStatus();
	}
	else if(managerStatus == fmDeleteInstrumentsEnd)
	{
		SI->listInstrumentSlots();
		SI->refreshInstrumentsList();
		SI->handleMemoryBar();
		FM->unblockAllInputs();

		newFileManager.clearStatus();
	}
	else if(managerStatus >=  fmError)
	{
		debugLog.addLine("Operation Error");
		FM->unblockAllInputs();

		newFileManager.clearStatus();
	}



}

void cSampleImporter::start(uint32_t options)
{
	moduleRefresh = 1;

	Encoder.setAcceleration(0);

	if(mtProject.values.lastUsedInstrument > INSTRUMENTS_MAX)
	{
		//mtProject.values.lastUsedInstrument = 0;
		selectedSlot = 0;
	}
	else
	{
		selectedSlot = mtProject.values.lastUsedInstrument;
	}

	keyboardManager.init(keyboardControl,editName);

	// focus na wybranym insturmencie
	if(options)
	{
		selectedFile  = 0; // slabe takie ustawianie tu ale przy imporcie zaznaczenia zabezpiecza przez zlym zaznaczeniem powodujacym zawiech
		selectedPlace = 1;
		selectedSlot = options - 1;

		mtProject.values.lastUsedInstrument = selectedSlot;
	}

	// przeladuj pierwszy raz xxx rzeba to popawic
	if(locationExplorerCount == 0 || interfaceGlobals.refreshFileExplorer)
	{
		interfaceGlobals.refreshFileExplorer = false;

		if(newFileManager.browseSdCard(nullptr))
		{
			SI->FM->blockAllInputs();
		}
	}
	else
	{
		showFilesTree();
		handleMemoryBar();
		previewColorControl();
		cancelSelect(listFiles);
		setSelect();


	}


	listInstrumentSlots();
	showInstrumentsList();




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

	cancelSelect(listInstruments);

	updateSelection();
	resetInstrSel();
	setSelect();

}

void cSampleImporter::stop()
{


	stopPlayingAll();
	Encoder.setAcceleration(3);
	keyboardManager.deinit();

	moduleRefresh = 0;
}

void cSampleImporter::setDefaultScreenFunct()
{
	//funkcje
	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);

	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);

	FM->setButtonObj(interfaceButtonDelete, functDelete);

	FM->setButtonObj(interfaceButtonShift, functShift);

	FM->setButtonObj(interfaceButtonInsert, buttonPress, functEnter);
	FM->setButtonObj(interfaceButton0, buttonPress, functChangeFolder);
	FM->setButtonObj(interfaceButton1, buttonPress, functChangeFolder);

	if(selectedPlace == 1) FM->setButtonObj(interfaceButton2, buttonPress, functRename);
	else FM->setButtonObj(interfaceButton2, buttonPress, functEnter);

	FM->setButtonObj(interfaceButton3, buttonPress, functInstrumentAddNext);
	FM->setButtonObj(interfaceButton4, previewFile);

	//FM->setButtonObj(interfaceButton4, buttonPress, functChangeInstrument);
	//FM->setButtonObj(interfaceButton5, buttonPress, functInstrumentDelete);

	FM->setButtonObj(interfaceButton6, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton7, buttonPress, functChangeInstrument);

	FM->setButtonObj(interfaceButtonCopy, buttonPress, functCopyPaste);
//	FM->setButtonObj(interfaceButtonPaste, buttonPress, functPaste);

	FM->setPadsGlobal(functPads);

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
	else
	{
		if(SI->checkIfValidSelection(SI->selectedFile))
		{
			SI->selectionTab[listFiles][SI->selectedFile] = 1;
		}
	}


	SI->selectedPlace = 0;

	SI->FM->setButtonObj(interfaceButton2, buttonPress, functEnter);

	SI->AddNextControl();
	SI->AddEnterOrRename();
	SI->previewColorControl();
	//SI->displayDelete(SI->selectedPlace);

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
	else
	{
		SI->selectionTab[listInstruments][SI->selectedSlot] = 1;
	}

	SI->FM->setButtonObj(interfaceButton2, buttonPress, functRename);

	SI->selectedPlace = 1;

	//SI->displayDelete(1);
	SI->AddEnterOrRename();
	SI->previewColorControl();
	//SI->displayDelete(SI->selectedPlace);

	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functInstrumentAdd()
{
//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		SI->showStopPatternPopup();
//		SI->setStopPatternFunction();
//		SI->stopPatternAction = cSampleImporter::enStopPatternAction::instrAdd;
//		return 1;
//	}
//	SI->stopPlayingAll();

	if(SI->explorerNames != nullptr && *SI->explorerNames[SI->selectedFile] != '/')
	{
		SI->sampleType = mtSampleTypeWaveFile;
		SI->addNextFlag = 0;
		SI->importSamples();
	}

	return 1;
}


static uint8_t functDelete(uint8_t state)
{
	if(SI->keyboardManager.getState())
	{
		functDeleteBackspace(state);
	}
	else
	{
		if(state == buttonPress)
		{
			functInstrumentDelete();
		}
	}

	return 1;
}


static  uint8_t functInstrumentDelete()
{
	if(SI->selectedPlace != 1) return 1;

//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		SI->showStopPatternPopup();
//		SI->setStopPatternFunction();
//		SI->stopPatternAction = cSampleImporter::enStopPatternAction::instrDelete;
//		return 1;
//	}
//	SI->stopPlayingAll();

	uint8_t deleteStart = SI->getSelectionStart(listInstruments);
	uint8_t deleteEnd = SI->getSelectionEnd(listInstruments);

	if(deleteStart >= INSTRUMENTS_COUNT || deleteEnd >= INSTRUMENTS_COUNT || deleteEnd < deleteStart) return 1;

	uint8_t instrToDeleteCount = 0;
	for(uint8_t i = deleteStart; i <= deleteEnd; i++)
	{
		if(mtProject.instrument[i].isActive)
		{
			instrToDeleteCount++;
		}
	}

	if(instrToDeleteCount > 0)
	{
		if(newFileManager.deleteInstruments(SI->getSelectionStart(listInstruments), SI->getSelectionEnd(listInstruments)))
		{
			SI->FM->blockAllInputs();
		}
	}

	return 1;
}



//==============================================================================================================

static  uint8_t functEncoder(int16_t value)
{
	if(SI->keyboardManager.getState() == 1) return 1;

	switch (SI->selectedPlace)
	{
	case 0:
		SI->changeFileSelection(value);
		break;
	case 1:
		SI->changeInstrumentSelection(value);
		break;

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

static uint8_t functInstrumentAddNext()
{
//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		SI->showStopPatternPopup();
//		SI->setStopPatternFunction();
//		SI->stopPatternAction = cSampleImporter::enStopPatternAction::instrAddNext;
//		return 1;
//	}
//
//	SI->stopPlayingAll();


	if(SI->explorerNames != nullptr  && *SI->explorerNames[SI->selectedFile] != '/')
	{
		SI->sampleType = mtSampleTypeWaveFile;
		SI->addNextFlag = 1;
		SI->importSamples();
	}

	return 1;
}

static  uint8_t functRename()
{
	if(mtProject.instrument[SI->selectedSlot].isActive != 1) return 1;

	SI->keyboardManager.fillName(mtProject.instrument[SI->selectedSlot].sample.file_name);
	SI->keyboardManager.activateKeyboard();

	SI->FM->clearButtonsRange(interfaceButton0,interfaceButton7);

	SI->FM->setButtonObj(interfaceButtonInsert, buttonPress, functConfirmKey);
	SI->FM->setButtonObj(interfaceButton0, buttonPress, functConfirmKey);
	SI->FM->setButtonObj(interfaceButton5, buttonPress, functAutoNameRename);
	SI->FM->setButtonObj(interfaceButton6, buttonPress, functCancelRename);
	SI->FM->setButtonObj(interfaceButton7, buttonPress, functConfirmRename);

	SI->showRenameKeyboard();
	return 1;
}

static  uint8_t functConfirmRename()
{

	char * const localName = SI->keyboardManager.getName();
	uint8_t length = strlen(localName);
	if(length >= 1)
	{
		uint8_t allow = 0;

		for(uint8_t i = 0; i < length; i++)
		{
			// sprawdza czy zawiera jakikolwiek logiczny znak(nie spacje)
			if(localName[i] != ' ')
			{
				allow = 1;
				break;
			}
		}

		if(allow)
		{
			strncpy(mtProject.instrument[SI->selectedSlot].sample.file_name,localName,SAMPLE_NAME_SIZE);

			SI->showFileList();

			SI->listInstrumentSlots();
			SI->showInstrumentsList();

			SI->handleMemoryBar();

			SI->activateLabelsBorder();

			SI->setDefaultScreenFunct();
			SI->showDefaultScreen();

			//SI->displayDelete(SI->selectedPlace);

		//	fileManager.setInstrumentChangeFlag(mtProject.values.lastUsedInstrument);
			newFileManager.setInstrumentStructChanged(mtProject.values.lastUsedInstrument);

			SI->keyboardManager.deactivateKeyboard();
		}
	}

	return 1;
}
static  uint8_t functCancelRename()
{
	SI->showFileList();

	SI->listInstrumentSlots();
	SI->showInstrumentsList();

	SI->handleMemoryBar();

	SI->activateLabelsBorder();
	SI->keyboardManager.deactivateKeyboard();

	SI->setDefaultScreenFunct();
	SI->showDefaultScreen();

	//SI->displayDelete(SI->selectedPlace);

	return 1;
}

static  uint8_t functAutoNameRename()
{
	SI->keyboardManager.setRandomName();

	return 1;
}

static uint8_t functCopyPaste()
{
	if(SI->keyboardManager.getState()) return 1;

//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		SI->showStopPatternPopup();
//		SI->setStopPatternFunction();
//		SI->stopPatternAction = cSampleImporter::enStopPatternAction::instrCopy;
//		return 1;
//	}
//	SI->stopPlayingAll();


	if(tactButtons.isButtonPressed(interfaceButtonShift))
	{
		SI->pasteInstruments();
	}
	else
	{
		SI->copyInstruments();
	}

	return 1;
}

void cSampleImporter::copyInstruments()
{
	if(SI->selectedPlace == 1)
	{
		SI->instrCopyStart = SI->getSelectionStart(listInstruments);
		SI->copyElementsCount = SI->selectionLength[listInstruments];
	}
}

void cSampleImporter::pasteInstruments()
{
	if(!copyElementsCount || instrCopyStart == selectedSlot) return;

	uint8_t willFit = 0;
	uint8_t selectOverMax = 0;

	uint8_t instrActiveInSel = SI->getActiveInstrInSelection();

	SI->copyElement = 0;

	if(!instrActiveInSel) return;


	while(SI->selectedSlot + SI->copyElementsCount > 48)
	{
		SI->copyElementsCount--;
		selectOverMax++;
	}

	for(int i = (SI->copyElementsCount - selectOverMax); i > 0; i--)
	{
		willFit = SI->willCopyInstrFit(i);

		if(willFit)
		{
			break;
		}
		else
		{
			SI->copyElementsCount--;
			instrActiveInSel--;
			//TODO: popout ze kopiowanie zostalo uciete
		}
	}

	if(willFit)
	{
		if(newFileManager.copyInstrumentsInWorkspace(instrCopyStart, copyElementsCount, selectedSlot))
		{
			SI->FM->blockAllInputs();
		}
	}
}

static void cancelAnotherSelect()
{
	if(SI->currSelectPlace != SI->selectedPlace)
	{
		if(SI->selectedPlace == 1)
		{
			SI->selectionActive[listFiles] = 0;
			SI->selectionLength[listFiles] = 1;
			memset(&SI->selectionTab[listFiles][0], 0, SI->maxListLength);
		}
		else
		{
			SI->selectionActive[listInstruments] = 0;
			SI->selectionLength[listInstruments] = 1;
			memset(&SI->selectionTab[listInstruments][0], 0, SI->maxListLength);
		}
	}
}

static  uint8_t functShift(uint8_t state)
{
	if(SI->keyboardManager.getState() == 1)
	{
		if(state == 1)
		{
			functConfirmKey();
		}
		return 1;
	}

	//select
	if(state == buttonPress)
	{
		cancelAnotherSelect();

		SI->selectionActive[SI->selectedPlace] = 1;

		if(SI->currSelectPlace != SI->selectedPlace)
		{
			if(SI->selectedPlace == 1)
			{
				SI->cancelSelect(listFiles);
			}
			else
			{
				SI->cancelSelect(listInstruments);
			}

			SI->currSelectPlace = SI->selectedPlace;
			SI->selectionLength[SI->selectedPlace] = 1;
		}

		if(SI->selectedPlace == 0)
		{
			if(SI->explorerNames != nullptr && *SI->explorerNames[SI->selectedFile] != '/')// nie mozna zaczac zaznaczac od folderu
			{
				SI->selectionTab[SI->selectedPlace][SI->selectedFile] = 1;
			}
		}
		else
		{
			SI->selectionActive[SI->selectedPlace] = 1;
			SI->selectionTab[SI->selectedPlace][SI->selectedSlot] = 1;
		}

		SI->updateSelection();
	}
	else if(state == buttonRelease)
	{
		SI->selectionActive[SI->selectedPlace] = 0;
	}

	SI->handleMemoryBar();

	return 1;
}


static  uint8_t functLeft()
{
	SI->keyboardManager.makeMove('a');
	if(SI->keyboardManager.getState()) return 1;
	if(SI->selectedPlace > 0)
	{
		if(SI->checkIfValidSelection(SI->selectedFile))
		{
			SI->selectionTab[listFiles][SI->selectedFile] = 1;
		}

		SI->selectedPlace--;
//		if(SI->selectedPlace != 0) SI->hideAddWT();
//		else if(SI->selectedPlace == 0)
//		{
////			SI->checkWavetableLabel();
//		}

		if(SI->currSelectPlace == 1)
		{
			SI->cancelSelect(listInstruments);
		}

		SI->handleMemoryBar();
		//SI->displayDelete(0);
		SI->resetInstrSel();
		SI->FM->clearButton(interfaceButton2);
		SI->FM->setButtonObj(interfaceButton2, buttonPress, functEnter);

		SI->previewColorControl();

	}
	else
	{
		SI->rewindListToBeggining();
		SI->cancelSelect(listFiles);

		SI->handleMemoryBar();

		SI->AddNextControl();
	}

	SI->AddEnterOrRename();
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functRight()
{
	SI->keyboardManager.makeMove('d');
	if(SI->keyboardManager.getState()) return 1;
	if(SI->selectedPlace < 1)
	{
		SI->selectionTab[listInstruments][SI->selectedSlot] = 1;
		SI->selectedPlace++;

		//SI->displayDelete(1);
		SI->FM->clearButton(interfaceButton2);
		SI->FM->setButtonObj(interfaceButton2, buttonPress, functRename);

		SI->previewColorControl();
	}

	SI->AddEnterOrRename();
	SI->activateLabelsBorder();

	return 1;
}

static  uint8_t functUp()
{
	SI->keyboardManager.makeMove('w');
	if(SI->keyboardManager.getState()) return 1;
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFileSelection(-1); break;
	case 1: SI->changeInstrumentSelection(-1); break;
	default:
			SI->stopPlayingAll();
	}
	return 1;
}

static  uint8_t functDown()
{
	SI->keyboardManager.makeMove('s');
	if(SI->keyboardManager.getState()) return 1;
	switch(SI->selectedPlace)
	{
	case 0: SI->changeFileSelection(1); break;
	case 1: SI->changeInstrumentSelection(1); break;
	default:
			SI->stopPlayingAll();
	}
	return 1;
}

static  uint8_t functPlayAction()
{
	if (sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
	{
		if (tactButtons.isButtonPressed(interfaceButtonShift))
		{
			sequencer.playSong();
		}
		else
		{
			sequencer.playPattern();
		}
	}
	else
	{
		sequencer.stop();
	}

	return 1;
}




static uint8_t functSwitchModule(uint8_t button)
{
	SI->eventFunct(eventSwitchModule,SI,&button,0);

	return 1;
}


//======================================================================================================================
/*uint8_t cSampleImporter::changeFolderSelection(int16_t value)
{
	if(selectedFolder+value < 0) selectedFolder = 0;
	else if(selectedFolder+value > locationFolderCount-1) selectedFolder = locationFolderCount-1;
	else  selectedFolder += value;

	AddEnterOrRename();

	display.setControlValue(folderListControl, selectedFolder);
	display.refreshControl(folderListControl);

	return 1;
}*/

uint8_t cSampleImporter::changeFileSelection(int16_t value)
{
	if(selectionActive[selectedPlace])
	{
		selectedFile=handleSelecting(selectedFile, locationExplorerCount, value, 1);
		updateSelection();
	}
	else
	{
		if(selectedFile+value < 0) selectedFile = 0;
		else if(selectedFile+value > locationExplorerCount-1) selectedFile = locationExplorerCount-1;
		else  selectedFile += value;

		memset(&selectionTab[listFiles][0], 0, SI->maxListLength);

		if(checkIfValidSelection(selectedFile))
		{
			selectionTab[listFiles][selectedFile] = 1;
			selectionLength[listFiles] = 1;
		}
		else
		{
			selectionLength[listFiles] = 0;
		}
	}

	//	checkWavetableLabel();

	display.setControlValue(explorerListControl, selectedFile);
	display.refreshControl(explorerListControl);

	AddNextControl();
	AddEnterOrRename();
	previewColorControl();


	handleMemoryBar();

	if (tactButtons.isButtonPressed(interfaceButton4))
	{
		SI->playSelectedSdFile();
	}

	return 1;
}


uint8_t cSampleImporter::changeInstrumentSelection(int16_t value)
{
	if(selectionActive[selectedPlace])
	{
		selectedSlot=handleSelecting(selectedSlot, 48, value, 0);
	}
	else
	{
		if(selectedSlot+value < 0) selectedSlot = 0;
		else if(selectedSlot+value > INSTRUMENTS_COUNT-1) selectedSlot = INSTRUMENTS_COUNT-1;
		else  selectedSlot += value;

		memset(&selectionTab[listInstruments][0], 0, SI->maxListLength);
		selectionTab[listInstruments][selectedSlot] = 1;
		selectionLength[listInstruments] = 1;
	}

	mtProject.values.lastUsedInstrument = selectedSlot;

	//displayDelete(1);
	AddEnterOrRename();
	updateSelection();

	mtProject.values.lastUsedInstrument = selectedSlot;

	display.setControlValue(instrumentListControl, selectedSlot);
	display.refreshControl(instrumentListControl);

	handleMemoryBar();
	previewColorControl();


	if (tactButtons.isButtonPressed(interfaceButton4))
	{
		SI->playSampleFromBank(INTERFACE_BUTTON_PREVIEW,buttonPress,-1);
	}

	return 1;
}


//======================================================================================================================


void cSampleImporter::BrowseOrAdd()
{
	if(SI->explorerNames == nullptr) return;

	if(*explorerNames[selectedFile] == '/')
	{
		if(selectedPlace == 0)
		{
			if(newFileManager.browseSdCard(&selectedFile))
			{
				SI->FM->blockAllInputs();
			}
		}
	}
	else
	{
		functInstrumentAdd();
	}
}

void cSampleImporter::importSamples()
{
	if(!fullMemoryFlag)
	{
		uint8_t selStart = getSelectionStart(listFiles);
		uint8_t selEnd = getSelectionEnd(listFiles);

		bool result = newFileManager.importSamplesToProject(selStart, selEnd, selectedSlot);
		addNextOffset = (selEnd - selStart) + 1;

		if(result)
		{
			SI->FM->blockAllInputs();
		}

	}
}



//==============================================================================================
void cSampleImporter::AddNextControl()
{
	uint8_t active = 0;
	if(SI->explorerNames != nullptr  && *explorerNames[selectedFile] != '/')
	{
		active = 1;
	}

	AddNextDisplay(active);
}

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

	if(selectionLength[listFiles] && currSelectPlace == 0)
	{
		uint8_t selectionStart = getSelectionStart(listFiles);

		if(selectedSlot + selectionLength[listFiles] > 48)
		{
			tempSelectionLength= selectionLength[listFiles] - (selectedSlot + selectionLength[listFiles] - 48);
		}
		else
		{
			tempSelectionLength = selectionLength[listFiles];
		}

		for(int i = selectionStart; i < (selectionStart + tempSelectionLength); i++)//get file selection size
		{
			tempSelectMemorySize += currentFolderMemoryFileUsage[i];
		}

		for(uint32_t i = selectedSlot; i < (selectedSlot + tempSelectionLength); i++)//get size when pasted on current isntrument slot
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
	//loadProgress = fileManager.samplesLoader.getCurrentProgress();//todo
}

void cSampleImporter::calculateCopyingProgress()
{
	//copyingProgress = fileManager.samplesImporter.getProgress();///todo
}

//==============================================================================================
void cSampleImporter::playSelectedSdFile()
{


//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		showStopPatternPopup();
//		setStopPatternFunction();
//		stopPatternAction = enStopPatternAction::previewSD;
//		return;
//	}

	stopPlayingAll();

	if(newFileManager.previevSamplefromSD(selectedFile))
	{
//		FM->blockAllInputsExcept(interfaceButton4);
//		FM->unblockPads();
	}

	playMode = playModeSdFile;


}


void cSampleImporter::playSampleFromBank(uint8_t pad, uint8_t state, int16_t velo)
{
	if(currentCopyStatusFlag || currentLoadStatusFlag) return;
	if(!mtProject.instrument[selectedSlot].isActive) return;

//	if(sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP)
//	{
//		showStopPatternPopup();
//		setStopPatternFunction();
//		stopPatternAction = enStopPatternAction::previewBank;
//		return;
//	}
//
//	stopPlaying();

	playMode = playModeSampleBank;


	uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);

	if (tactButtons.isButtonPressed(interfaceButton4))
	{

		sequencer.handleNoteOff(
								Sequencer::GRID_OUTSIDE_PATTERN,
								noteFromPad,
								-1,
								pad);
	}

	sequencer.handleNoteOn(
							Sequencer::GRID_OUTSIDE_PATTERN,
							noteFromPad,
							-1,
							pad);
}


void cSampleImporter::stopPlaying(uint8_t pad, uint8_t state, int16_t velo)
{
	if(playMode == playModeSdFile)
	{
		newFileManager.stopPrevievSamplefromSD();
		SI->FM->unblockAllInputs();
	}
	else if(playMode == playModeSampleBank)
	{
//		mtPadBoard.stopInstrument(pad);
		uint8_t noteFromPad = mtPadBoard.getNoteFromPad(pad);
		sequencer.handleNoteOff(Sequencer::GRID_OUTSIDE_PATTERN, noteFromPad, 0, pad);
	}

//	playMode = playModeStop;
}

void cSampleImporter::stopPlayingAll()
{

	if(playMode == playModeSdFile)
	{
		stopPlaying(0, 0, 0);
	}
	else if( playMode == playModeSampleBank)
	{
		sequencer.stopManualNotes();
	}

}

static uint8_t previewFile(uint8_t state)
{
	SI->playFromPads(INTERFACE_BUTTON_PREVIEW,state,-1);
	return 1;
}

void cSampleImporter::playFromPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state == 0)  SI->stopPlaying(pad,state,velo);
	else if (state == 1)
	{

		switch(SI->selectedPlace)
		{
		case 0: SI->playSelectedSdFile(); 							break;
		case 1: SI->playSampleFromBank(pad,state,velo); 	break;

		}
	}
}

void cSampleImporter::updateSelection()
{
	display.refreshControl(explorerListControl);
	display.refreshControl(instrumentListControl);
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
				if(selectionTab[selectedPlace][position] == 1)
				{
					uint8_t flag = 1;
					if(sign<0)
					{
						selectionTab[selectedPlace][position+1] = 0;
					}
					else if(sign>0)
					{
						if(selectionTab[selectedPlace][position-1] == 1)
						{
							selectionTab[selectedPlace][position-1] = 0;
						}
						else
						{
							flag = 0;
						}
					}

					if(selectionLength[selectedPlace] && flag)
					{
						selectionLength[selectedPlace]--;
					}
				}
				else
				{
					if(selectionLength[selectedPlace] < MAX_SELECT)
					{
						selectionLength[selectedPlace]++;
						selectionTab[selectedPlace][position] = 1;
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

bool cSampleImporter::checkIfAnyInstrActive()
{
	uint8_t selectionStart = getSelectionStart(listInstruments);
	bool isActive = false;

	for(uint32_t i = selectionStart; i < (selectionStart + selectionLength[listInstruments]); i++)
	{
		if(mtProject.instrument[i].isActive == 1)
		{
			isActive = true;
			break;
		}
	}

	return isActive;
}

void cSampleImporter::cancelSelect(uint8_t placeToCancel)
{
	memset(&SI->selectionTab[placeToCancel][0], 0, maxListLength);
	selectionLength[selectedPlace] = 1;

	if(placeToCancel == listFiles)
	{
		if(checkIfValidSelection(selectedFile))
		{
			selectionTab[listFiles][selectedFile] = 1;
		}
	}
	else
	{
		selectionTab[listInstruments][selectedSlot] = 1;
	}


	updateSelection();
}

int16_t cSampleImporter::getSelectionStart(uint8_t whichSelect)
{
	int16_t selStart = SI->maxListLength;

	for(size_t i = 0; i < SI->maxListLength; i++)
	{
		if(selectionTab[whichSelect][i] == 1)
		{
			selStart = i;
			break;
		}
	}

	return selStart;
}

int16_t cSampleImporter::getSelectionEnd(uint8_t whichSelect)
{
	int16_t selEnd = 0;

	for(int i = SI->maxListLength-1; i > 0; i--)
	{
		if(selectionTab[whichSelect][i] == 1)
		{
			selEnd = i;
			break;
		}
	}

	return selEnd;
}


bool cSampleImporter::checkIfValidSelection(uint8_t positionToCheck)
{
	if(explorerNames != nullptr && !(*explorerNames[positionToCheck] == '/'))
	{
		return true;
	}

	return false;
}

char* cSampleImporter::parseNewName(uint8_t num)
{
	sprintf(instrToCreate,"instr%02d.wav",num);

	return instrToCreate;
}

uint8_t cSampleImporter::getActiveInstrInSelection()
{
	uint8_t activeInstr=0;

	for(uint8_t instr = instrCopyStart; instr < (instrCopyStart + copyElementsCount); instr++)
	{
		if(mtProject.instrument[instr].isActive == 1)
		{
			activeInstr++;
		}
	}

	return activeInstr;
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
	copyElementsCount=0;
}

void cSampleImporter::setStopPatternFunction()
{
	FM->clearButton(interfaceButton6);
	FM->clearButton(interfaceButton7);

	FM->setButtonObj(interfaceButton6, buttonPress, functStopPatternNo);
	FM->setButtonObj(interfaceButton7, buttonPress, functStopPatternYes);
	FM->blockAllInputsExcept(interfaceButton6, interfaceButton7);

}
void cSampleImporter::clearStopPatternFunction()
{
	FM->clearButton(interfaceButton6);
	FM->clearButton(interfaceButton7);

	FM->setButtonObj(interfaceButton6, buttonPress, functChangeInstrument);
	FM->setButtonObj(interfaceButton7, buttonPress, functChangeInstrument);

	FM->unblockAllInputs();
}

static uint8_t functConfirmKey()
{
	SI->keyboardManager.confirmKey();
	return 1;
}

static uint8_t functDeleteBackspace(uint8_t state)
{
	if((state == buttonPress) || (state == buttonHold))
	{
		SI->keyboardManager.makeBackspace();
	}

	return 1;
}

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(SI->keyboardManager.getState())
	{
		SI->keyboardManager.onPadChange(pad, state);
	}
	else
	{
		if((state == 1) || (state == 0))
		{
			SI->playFromPads(pad,state,velo);
			if(state) padsBacklight.setFrontLayer(1,mtConfig.values.padsLightFront, pad);
			else padsBacklight.setFrontLayer(0,0,pad);
		}
	}


	return 1;
}

static uint8_t functStopPatternYes()
{
	sequencer.stop();
	SI->hideStopPatternPopup();
	SI->clearStopPatternFunction();
	switch(SI->stopPatternAction)
	{
		case cSampleImporter::enStopPatternAction::previewBank: 	break;
		case cSampleImporter::enStopPatternAction::previewSD: 		break;
		case cSampleImporter::enStopPatternAction::instrAdd: 		functInstrumentAdd();		break;
		case cSampleImporter::enStopPatternAction::instrAddNext: 	functInstrumentAddNext();	break;
		case cSampleImporter::enStopPatternAction::instrCopy: 		functCopyPaste();			break;
		case cSampleImporter::enStopPatternAction::instrDelete: 	functInstrumentDelete();	break;
		default: break;
	}
	return 1;
}
static uint8_t functStopPatternNo()
{

	SI->hideStopPatternPopup();
	SI->clearStopPatternFunction();
	return 1;
}


