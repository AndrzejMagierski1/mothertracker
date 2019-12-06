



#include "mtTest.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"
#include "configEditor.h"
#include "mtSequencer.h"
#include "mtConfig.h"


#include "SI4703.h"

cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;


static  uint8_t functPlayAction();
static  uint8_t functRecAction();



static  uint8_t functLeft();
static  uint8_t functRight();
static  uint8_t functUp();
static  uint8_t functDown();

//static  uint8_t functAction0();
//static  uint8_t functAction5();

static  uint8_t functActionButton(uint8_t button, uint8_t state);

static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo);


// config
//static  uint8_t functConfigGroup(uint8_t button);




//master
static  uint8_t functSelectVolume(uint8_t state);
static  uint8_t functSelectReverbSize(uint8_t state);
static  uint8_t functSelectReverbDamping(uint8_t state);
static  uint8_t functSelectLimiterAttack(uint8_t state);
static  uint8_t functSelectLimiterRelease(uint8_t state);
static  uint8_t functSelectLimiterTreshold(uint8_t state);
static  uint8_t functSelectBitDepth(uint8_t state);



//master tracks



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);


static  uint8_t functSwitchModeConfig(uint8_t state);
static  uint8_t functSwitchModeMaster(uint8_t state);

uint8_t checkIfFirmwareValid(char *name);
//static uint8_t selectFirmware();

static uint8_t prepareAndFlash();
static uint8_t hideFlashingWarning();


// MASTER EDIT FUNCTIONS
void changeVolume(int16_t value);
void changeReverbRoomSize(int16_t value);
void changeReverbDamping(int16_t value);
void changeLimiterAttack(int16_t value);
void changeLimiterRelease(int16_t value);
void changeLimiterTreshold(int16_t value);
void changeBitDepth(int16_t value);


void cConfigEditor::update()
{

	if(processUpdate)
	{
		processUpdate = 0;
		prepareAndFlash();
	}

}

void cConfigEditor::start(uint32_t options)
{
	moduleRefresh = 1;

	mode = options;
	exitOnButtonRelease = 0;
//--------------------------------------------------------------------

//--------------------------------------------------------------------

	clearAllNodes();
	cancelMultiFrame();


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);

	FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);




	switch(mode)
	{
	case mtConfigModeDefault:
	{
		selectedPlace[0] = 0;
		resizeLabelConfigDefault();

		showDefaultConfigScreen();
		setConfigScreenFunct();

		for(uint8_t i = 0; i < 6; i++)
		{
			listDataForLists(0, i, &groupNamesLabels[i][0]);
		}

		setDataForLists(0, 6);

		chainProcessChange(0, 1);

		break;
	}
	case mtConfigModeMaster:
	{
		selectedPlace[1] = 0;
		resizeLabelConfigMaster();

		showMasterScreen();
		setMasterScreenFunct();


		break;
	}
	}


	activateLabelsBorder();
}


void cConfigEditor::stop()
{
	exitOnButtonRelease = 0;
	moduleRefresh = 0;
}


void cConfigEditor::setConfigScreenFunct()
{
	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);


	FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setPotObj(interfacePot0, functEncoder, nullptr);

	FM->setButtonObj(interfaceButton0, functActionButton);
	FM->setButtonObj(interfaceButton1, functActionButton);
	FM->setButtonObj(interfaceButton2, functActionButton);
	FM->setButtonObj(interfaceButton3, functActionButton);
	FM->setButtonObj(interfaceButton4, functActionButton);
	FM->setButtonObj(interfaceButton5, functActionButton);
	FM->setButtonObj(interfaceButton6, functActionButton);

	FM->setPadsGlobal(functPads);

}

void cConfigEditor::setMasterScreenFunct()
{

	//funkcje
	FM->clearButtonsRange(interfaceButton0,interfaceButton7);
	FM->clearAllPots();

	FM->setButtonObj(interfaceButtonPlay, buttonPress, functPlayAction);
	FM->setButtonObj(interfaceButtonRec, buttonPress, functRecAction);


	FM->setButtonObj(interfaceButtonLeft, buttonPress, functLeft);
	FM->setButtonObj(interfaceButtonRight, buttonPress, functRight);
	FM->setButtonObj(interfaceButtonUp, buttonPress, functUp);
	FM->setButtonObj(interfaceButtonDown, buttonPress, functDown);





	FM->setButtonObj(interfaceButton0, functSelectVolume);

	FM->setButtonObj(interfaceButton1, functSelectReverbSize);
	FM->setButtonObj(interfaceButton2, functSelectReverbDamping);

	FM->setButtonObj(interfaceButton3, functSelectBitDepth);
	FM->setButtonObj(interfaceButton4, functSelectLimiterAttack);
	FM->setButtonObj(interfaceButton5, functSelectLimiterRelease);
	FM->setButtonObj(interfaceButton6, functSelectLimiterTreshold);



	FM->setPotObj(interfacePot0, functEncoder, nullptr);


}

//##############################################################################################
//###############################        ACTION BUTTONS        #################################
//##############################################################################################

uint8_t cConfigEditor::getListsActive()
{
	uint8_t activeNum = 0;
	for(uint8_t i = 0; i < 4; i++)
	{
		if(listsActive[i])
		{
			activeNum++;
		}
	}

	return activeNum;
}

void cConfigEditor::setDataForLists(uint8_t listNum, uint8_t max)
{
	configGroupList[listNum].linesCount = 14;
	configGroupList[listNum].start = selectedConfigGroup[listNum];
	configGroupList[listNum].length = max;
	configGroupList[listNum].data = configGroupsNames[listNum];

	selectedConfigGroupMax[listNum] = max;

	display.setControlData(configGroupsListControl[listNum], &configGroupList[listNum]);
	display.setControlShow(configGroupsListControl[listNum]);
	display.refreshControl(configGroupsListControl[listNum]);

	listsActive[listNum] = 1;
}

void cConfigEditor::listDataForLists(uint8_t listNum, uint8_t nameNum, const char *names)
{
	configGroupsNames[listNum][nameNum] = (char*)names;
}

void cConfigEditor::changeSelectionInGroup(int16_t value, uint8_t groupNum)
{
	if(selectedConfigGroup[groupNum] + value < 0) selectedConfigGroup[groupNum] = 0;
	else if(selectedConfigGroup[groupNum] + value > selectedConfigGroupMax[groupNum] - 1)
	{
		if(selectedConfigGroupMax[groupNum])
		{
			selectedConfigGroup[groupNum] = selectedConfigGroupMax[groupNum] - 1;
		}
	}
	else  selectedConfigGroup[groupNum] += value;

	chainProcessChange(groupNum, 0);

	display.setControlValue(configGroupsListControl[groupNum], selectedConfigGroup[groupNum]);
	display.refreshControl(configGroupsListControl[groupNum]);
}

void cConfigEditor::chainProcessChange(uint8_t groupNum, uint8_t forceProcess)
{
	if(groupNum < 1 || forceProcess)
	{
		resizeToDefaultConfig();
		listsActive[1] = 0;
		/* by default next list is hidden with no label text*/
		display.setControlHide(configGroupsListControl[1]);
		display.setControlText(label[2], "");

		if(forceProcess == 0)
		{
			selectedConfigGroup[1] = 0;
			selectedConfigGroup[2] = 0;
			selectedConfigGroup[3] = 0;
		}

		processChangeInGroup0();
	}

	if(groupNum < 2 || forceProcess)
	{
		listsActive[2] = 0;
		display.setControlHide(configGroupsListControl[2]);
		display.setControlText(label[4], "");

		if(forceProcess == 0)
		{
			selectedConfigGroup[2] = 0;
			selectedConfigGroup[3] = 0;
		}

		processChangeInGroup1();
	}

	if(groupNum < 3 || forceProcess)
	{
		listsActive[3] = 0;
		display.setControlHide(configGroupsListControl[3]);
		display.setControlText(label[6], "");

		if(forceProcess == 0)
		{
			selectedConfigGroup[3] = 0;
		}

		processChangeInGroup2();
	}

	if(groupNum < 4 || forceProcess)
	{
		processChangeInGroup3();
	}

	for(uint32_t i = 0 ; i < 4; i++)
	{
		display.refreshControl(label[i*2]);
	}

	if(hasConfigChanged)
	{
		forceSaveConfig();
	}

	display.synchronizeRefresh();
}

void cConfigEditor::showGeneralSubmenus(uint8_t listPosition)
{
	for(uint8_t i = 0; i < GERERAL_SUBMENUS; i++)
	{
		listDataForLists(listPosition, i, &generalConfig[i][0]);
	}

	setDataForLists(listPosition, GERERAL_SUBMENUS);
	changeLabelText(2, "General");
}

void cConfigEditor::showFirmwares(uint8_t listPosition)
{
	listAllFirmwares();

	for(uint8_t i = 0; i < firmwareFoundNum; i++)
	{
		listDataForLists(listPosition, i, &firmwareNamesList[i][0]);
	}

	setDataForLists(listPosition, firmwareFoundNum);
	changeLabelText(2, "Firmware list");
}

void cConfigEditor::showFirmwareFlashButton()
{
	resizeToSmallConfig(4);
	resizeToSmallConfig(6);

	showExecute();
}

void cConfigEditor::showMidiSubmenus(uint8_t listPosition)
{
	for(uint8_t i = 0; i < MIDI_SUBMENUS; i++)
	{
		listDataForLists(listPosition, i, &midiConfig[i][0]);
	}

	setDataForLists(listPosition, MIDI_SUBMENUS);
	changeLabelText(2, "MIDI");
}

void cConfigEditor::showPatternDividers(uint8_t listPosition)
{
	for(uint8_t i = 0; i < PATTERN_DIVIDERS; i++)
	{
		listDataForLists(listPosition, i, &patternDivider[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.general.patternDiv;

	setDataForLists(listPosition, PATTERN_DIVIDERS);
	changeLabelText(4, "Pattern dividers");
}

void cConfigEditor::showRadioRegions(uint8_t listPosition)
{
	for(uint8_t i = 0; i < RADIO_REGIONS; i++)
	{
		listDataForLists(listPosition, i, &radioRegion[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.general.radioRegion;

	setDataForLists(listPosition, RADIO_REGIONS);
	changeLabelText(4, "Radio regions");
}

void cConfigEditor::showBrightnessLevels(uint8_t listPosition)
{
	for(uint8_t i = 0; i < BRIGHTNESS_LEVELS; i++)
	{
		listDataForLists(listPosition, i, &brightness[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.general.brightness;

	setDataForLists(listPosition, BRIGHTNESS_LEVELS);
	changeLabelText(4, "Brightness");
}

void cConfigEditor::showClockIn(uint8_t listPosition)
{
	for(uint8_t i = 0; i < CLOCK_IN; i++)
	{
		listDataForLists(listPosition, i, &clockIn[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.clkIn;

	setDataForLists(listPosition, CLOCK_IN);
	changeLabelText(4, "Clock in");
}

void cConfigEditor::showClockOut(uint8_t listPosition)
{
	for(uint8_t i = 0; i < CLOCK_OUT; i++)
	{
		listDataForLists(listPosition, i, &clockOut[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.clkOut;

	setDataForLists(listPosition, CLOCK_OUT);
	changeLabelText(4, "Clock out");
}

void cConfigEditor::showTransportIn(uint8_t listPosition)
{
	for(uint8_t i = 0; i < CLOCK_OUT; i++)
	{
		listDataForLists(listPosition, i, &clockOut[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.transportIn;

	setDataForLists(listPosition, CLOCK_OUT);
	changeLabelText(4, "Transport in");
}

void cConfigEditor::showTransportOut(uint8_t listPosition)
{
	for(uint8_t i = 0; i < CLOCK_OUT; i++)
	{
		listDataForLists(listPosition, i, &clockOut[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.transportOut;

	setDataForLists(listPosition, CLOCK_OUT);
	changeLabelText(4, "Transport out");
}

void cConfigEditor::showCCouts(uint8_t listPosition)
{
	for(uint8_t i = 0; i < CC_OUTS_NUM; i++)
	{
		listDataForLists(listPosition, i, &CCouts[i][0]);
	}

	setDataForLists(listPosition, CC_OUTS_NUM);
	changeLabelText(4, "CC out");
}

void cConfigEditor::showCCnumber(uint8_t listPosition)
{
	for(uint8_t i = 0; i < CC_NUMBERS; i++)
	{
		listDataForLists(listPosition, i, &CCnumber[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.ccOut[selectedConfigGroup[listPosition-1]];

	setDataForLists(listPosition, CC_NUMBERS);
	changeLabelText(6, "CC number");
}

uint8_t cConfigEditor::setPatternDivider(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.general.patternDiv != val)
	{
		mtConfig.general.patternDiv = val;
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setBrightness(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.general.brightness != val)
	{
		mtConfig.general.brightness = val;
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setRadioRegion(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.general.radioRegion != val)
	{
		mtConfig.general.radioRegion = val;
		radio.setRegion((radio_region_t)mtConfig.general.radioRegion, 0);
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setClockIn(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.clkIn != val)
	{
		mtConfig.midi.clkIn = val;
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setClockOut(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.clkOut != val)
	{
		mtConfig.midi.clkOut = val;
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setTransportIn(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.transportIn != val)
	{
		mtConfig.midi.transportIn = val;
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setTransportOut(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.transportOut != val)
	{
		mtConfig.midi.transportOut = val;
		isChanged = 1;
	}

	return isChanged;
}

uint8_t cConfigEditor::setCCout(uint8_t ccNum, uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.ccOut[ccNum] != val)
	{
		mtConfig.midi.ccOut[ccNum] = val;
		isChanged = 1;
	}

	return isChanged;
}

void cConfigEditor::processChangeInGroup0()
{
	switch(selectedConfigGroup[0])
	{
	case configDefaultGeneral:
		showGeneralSubmenus(1);
		break;
	case configDefaultFirmware:
		showFirmwares(1);
		break;
	case configDefaultMIDI:
		showMidiSubmenus(1);
		break;
	case configDefaultSD:
		break;
	case configDefaultHelp:
		break;
	case configDefaultCredits:
		break;
	default:
		break;
	}
}

void cConfigEditor::processChangeInGroup1()
{
	//selectedConfigGroup[2] = 0;
	//selectedConfigGroup[3] = 0;

	if(selectedConfigGroup[0] == configDefaultGeneral)
	{
		switch(selectedConfigGroup[1])
		{
		case configGeneralPatternDiv:
			showPatternDividers(2);
			break;
		case configGeneralRadioRegion:
			showRadioRegions(2);
			break;
		case configGeneralBrightness:
			showBrightnessLevels(2);
			break;
		default:
			break;
		}
	}
	else if(selectedConfigGroup[0] == configDefaultFirmware)
	{
		showFirmwareFlashButton();
	}
	else if(selectedConfigGroup[0] == configDefaultMIDI)
	{
		switch(selectedConfigGroup[1])
		{
		case configMIDIClockIn:
			showClockIn(2);
			break;
		case configMIDIClockOut:
			showClockOut(2);
			break;
		case configMIDITransportIn:
			showTransportIn(2);
			break;
		case configMIDITansportOut:
			showTransportOut(2);
			break;
		case configMIDICcout:
			showCCouts(2);
			break;
		default:
			break;
		}
	}
}

void cConfigEditor::processChangeInGroup2()
{
	uint32_t dataSource = selectedConfigGroup[2];

	//selectedConfigGroup[3] = 0;

	if(selectedConfigGroup[0] == configDefaultGeneral)
	{
		if(selectedConfigGroup[1] == configGeneralPatternDiv)
		{
			hasConfigChanged = setPatternDivider(dataSource);
		}
		else if(selectedConfigGroup[1] == configGeneralRadioRegion)
		{
			hasConfigChanged = setRadioRegion(dataSource);
		}
		else if(selectedConfigGroup[1] == configGeneralBrightness)
		{
			hasConfigChanged = setBrightness(dataSource);
		}
	}

	else if(selectedConfigGroup[0] == configDefaultMIDI)
	{
		if(selectedConfigGroup[1] == configMIDIClockIn)
		{
			hasConfigChanged = setClockIn(dataSource);
		}
		else if(selectedConfigGroup[1] == configMIDIClockOut)
		{
			hasConfigChanged = setClockOut(dataSource);
		}
		else if(selectedConfigGroup[1] == configMIDITransportIn)
		{
			hasConfigChanged = setTransportIn(dataSource);
		}
		else if(selectedConfigGroup[1] == configMIDITansportOut)
		{
			hasConfigChanged = setTransportOut(dataSource);
		}
		else if(selectedConfigGroup[1] == configMIDICcout)
		{
			showCCnumber(3);
		}
	}
}

void cConfigEditor::processChangeInGroup3()
{
	uint32_t dataSource = selectedConfigGroup[3];

	if(selectedConfigGroup[0] == configDefaultMIDI)
	{
		if(selectedConfigGroup[1] == configMIDICcout)
		{
			hasConfigChanged = setCCout(selectedConfigGroup[2], dataSource);
		}
	}
}

static uint8_t functActionButton(uint8_t button, uint8_t state)
{
	uint8_t updateFrame = 1;
	if(state == buttonPress)
	{
		switch(button)
		{
		case 0:
		{
			if(CE->selectedPlace[0] == 0)
			{
				CE->changeSelectionInGroup(-1, 0);
			}
			else
			{
				CE->selectedPlace[0] = 0;
			}
			break;
		}
		case 1:
		{
			if(CE->selectedPlace[0] == 0)
			{
				CE->changeSelectionInGroup(1, 0);
			}
			else
			{
				CE->selectedPlace[0] = 0;
			}
			break;
		}
		case 2:
		{
			if(CE->listsActive[1])
			{
				if(CE->selectedPlace[0] == 1)
				{
					CE->changeSelectionInGroup(-1, 1);
				}
				else
				{
					CE->selectedPlace[0] = 1;
				}
			}
			break;
		}
		case 3:
		{
			if(CE->listsActive[1])
			{
				if(CE->selectedPlace[0] == 1)
				{
					CE->changeSelectionInGroup(1, 1);
				}
				else
				{
					CE->selectedPlace[0] = 1;
				}
			}
			break;
		}
		case 4:
		{
			if(CE->listsActive[2])
			{
				if(CE->selectedPlace[0] == 2)
				{
					CE->changeSelectionInGroup(-1, 2);
				}
				else
				{
					CE->selectedPlace[0] = 2;
				}
			}

			if(CE->selectedConfigGroup[0] == configDefaultFirmware)
			{
				updateFrame = 0;
				CE->showFlashingWarning();
			}
			break;
		}
		case 5:
		{
			if(CE->listsActive[2])
			{
				if(CE->selectedPlace[0] == 2)
				{
					CE->changeSelectionInGroup(1, 2);
				}
				else
				{
					CE->selectedPlace[0] = 2;
				}
			}
			//CE->eventFunct(eventActivateImageViewer,CE,0,0);
			break;
		}
		case 6:
		{
			if(CE->listsActive[3])
			{
				if(CE->selectedPlace[0] == 3)
				{
					CE->changeSelectionInGroup(-1, 3);
				}
				else
				{
					CE->selectedPlace[0] = 3;
				}
			}
			break;
		}
		case 7:
		{
			if(CE->listsActive[3])
			{
				if(CE->selectedPlace[0] == 3)
				{
					CE->changeSelectionInGroup(1, 3);
				}
				else
				{
					CE->selectedPlace[0] = 3;
				}
			}
			break;
		}

		default:
			break;
		}
	}
	else if(state == buttonRelease)
	{

	}

	if(updateFrame)
	{
		CE->activateLabelsBorder();
	}

	return 1;
}

//==============================================================================================================

/*static  uint8_t functConfigGroup(uint8_t button)
{
	if(CE->selectionActive) return 1;

	if(button == interfaceButton6)
	{
		CE->changeConfigGroupSelection(-1);
	}
	else //if(button == interfaceButton7)
	{
		CE->changeConfigGroupSelection(1);
	}

	CE->selectedPlace[0] = 6;
	CE->activateLabelsBorder();


	return 1;
}*/


static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state != buttonPress) return 1;

	if(mtTest.runTestByCombinaion(pad)) 	CE->eventFunct(eventActivateTestingProcedure,CE,0,0);



	return 1;
}


static  uint8_t functEncoder(int16_t value)
{
	if(CE->selectionActive) return 1;

	if(CE->frameData.multiSelActiveNum != 0)
	{
		CE->stepThroughNodes(value);
	}
	else
	{
		uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

		switch(mode_places)
		{
		case 0: 	 CE->changeSelectionInGroup(value, 0); break;
		case 1:		 CE->changeSelectionInGroup(value, 1); break;
		case 2: 	 CE->changeSelectionInGroup(value, 2); break;
		case 3: 	 CE->changeSelectionInGroup(value, 3); break;

		case 10: changeVolume(value);			break;
		case 11: changeReverbRoomSize(value);	break;
		case 12: changeReverbDamping(value);	break;
		case 13: changeBitDepth(value);			break;
		case 14: changeLimiterAttack(value);	break;
		case 15: changeLimiterRelease(value);	break;
		case 16: changeLimiterTreshold(value);	break;
		case 17: 	break;

		}
	}

	return 1;
}






static  uint8_t functLeft()
{
	if(CE->selectionActive) return 1;
	if(CE->frameData.multiSelActiveNum != 0) return 1;


	if(CE->selectedPlace[CE->mode] > 0) CE->selectedPlace[CE->mode]--;


	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functRight()
{
	if(CE->selectionActive) return 1;
	if(CE->frameData.multiSelActiveNum != 0) return 1;


	if(CE->mode == mtConfigModeDefault)
	{
		uint8_t maxPlaces;
		maxPlaces = CE->getListsActive();
		if(CE->selectedPlace[CE->mode] < (maxPlaces-1)) CE->selectedPlace[CE->mode]++;
	}
	else
	{
		if(CE->selectedPlace[CE->mode] < CE->frameData.placesCount-1) CE->selectedPlace[CE->mode]++;
	}


	CE->activateLabelsBorder();

	return 1;
}


static  uint8_t functUp()
{
	if(CE->selectionActive) return 1;

	if(CE->frameData.multiSelActiveNum != 0)
	{
		CE->stepThroughNodes(1);
	}
	else
	{
		uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

		switch(mode_places)
		{
		case 0: 	 CE->changeSelectionInGroup(-1, 0); break;
		case 1:		 CE->changeSelectionInGroup(-1, 1); break;
		case 2: 	 CE->changeSelectionInGroup(-1, 2); break;
		case 3: 	 CE->changeSelectionInGroup(-1, 3); break;

		case 10: changeVolume(1);			break;
		case 11: changeReverbRoomSize(1);	break;
		case 12: changeReverbDamping(1);	break;
		case 13: changeBitDepth(1);			break;
		case 14: changeLimiterAttack(1);	break;
		case 15: changeLimiterRelease(1);	break;
		case 16: changeLimiterTreshold(1);	break;
		case 17: 	break;

		}
	}

	return 1;
}


static  uint8_t functDown()
{
	if(CE->selectionActive) return 1;

	if(CE->frameData.multiSelActiveNum != 0)
	{
		CE->stepThroughNodes(-1);
	}
	else
	{
		uint8_t mode_places = CE->selectedPlace[CE->mode] + CE->mode*10;

		switch(mode_places)
		{
		case 0: 	 CE->changeSelectionInGroup(1, 0); break;
		case 1:		 CE->changeSelectionInGroup(1, 1); break;
		case 2: 	 CE->changeSelectionInGroup(1, 2); break;
		case 3: 	 CE->changeSelectionInGroup(1, 3); break;

		case 10: changeVolume(-1);			break;
		case 11: changeReverbRoomSize(-1);	break;
		case 12: changeReverbDamping(-1);	break;
		case 13: changeBitDepth(-1);		break;
		case 14: changeLimiterAttack(-1);	break;
		case 15: changeLimiterRelease(-1);	break;
		case 16: changeLimiterTreshold(-1);	break;
		case 17: 	break;

		}
	}

	return 1;
}




static  uint8_t functPlayAction()
{
	if(CE->selectionActive) return 1;

	if(sequencer.getSeqState() == Sequencer::SEQ_STATE_STOP)
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
	if(CE->selectionActive) return 1;

	return 1;
}

static  uint8_t functSelectVolume(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 0;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeVolume, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbSize(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 1;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeReverbRoomSize, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectReverbDamping(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 2;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeReverbDamping, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectBitDepth(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 3;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeBitDepth, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterAttack(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 4;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeLimiterAttack, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterRelease(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 5;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeLimiterRelease, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static  uint8_t functSelectLimiterTreshold(uint8_t state)
{
	if(state > buttonPress) return 1;

	uint8_t node = 6;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = node;
		CE->addNode(changeLimiterTreshold, node);
		CE->frameData.multisel[node].frameNum = node;
		CE->frameData.multisel[node].isActive = 1;
		CE->frameData.multiSelActiveNum  += 1;
	}
	else
	{
		CE->removeNode(node);

		if(CE->frameData.multiSelActiveNum)
		{
			if(CE->frameData.multisel[node].isActive)
			{
				CE->removeNode(node);
				CE->frameData.multiSelActiveNum  -= 1;

				CE->frameData.multisel[node].isActive = 0;

				if(CE->frameData.multiSelActiveNum == 0)
				{
					CE->selectedPlace[mtConfigModeMaster] = node;
				}
			}
		}
	}

	CE->activateLabelsBorder();

	return 1;
}

static uint8_t functSwitchModule(uint8_t button)
{
	if(CE->selectionActive) return 1;

	CE->eventFunct(eventSwitchModule,CE,&button,0);

	return 1;
}

static  uint8_t functSwitchModeConfig(uint8_t state)
{
	if(CE->selectionActive) return 1;

	if(state == buttonPress)
	{
		if(CE->mode != mtConfigModeDefault)
		{
			//CE->mode = 0;
			CE->start(0);
		}
	}

	return 1;
}

static  uint8_t functSwitchModeMaster(uint8_t state)
{
	if(CE->selectionActive) return 1;

	if(state == buttonPress)
	{
		CE->selectedPlace[mtConfigModeMaster] = 0;

/*		CE->clearAllNodes();
		CE->cancelMultiFrame();*/

		if(CE->mode != mtConfigModeMaster)
		{
			CE->start(1);
			return 0;
		}
	}
	else if(state == buttonHold)
	{
//		CE->resizeLabelConfigMaster();

		CE->exitOnButtonRelease = 1;
		//CE->selectedPlace[mtConfigModeMaster] = 0;
		CE->activateLabelsBorder();
	}
	else if(state == buttonRelease)
	{
		if(CE->exitOnButtonRelease) CE->eventFunct(eventSwitchToPreviousModule,CE,0,0);
	}

	return 1;
}

/*
static uint8_t functAction0()
{
	if(CE->selectedConfigGroup == configDefaultFirmware)
	{
		if(CE->selectionActive)
		{
			CE->processUpdate = 1;

			//while(1);// program sie zatrzymuje do czasu przejecia kontroli przez bootloader
		}
	}

	return 1;
}

static uint8_t functAction5()
{
	if(CE->selectedConfigGroup == configDefaultFirmware)
	{
		if(CE->selectionActive)
		{
			CE->hideWarning();
		}
	}

	return 1;
}
*/


//======================================================================================================================


void cConfigEditor::changeConfigGroupSelection(int16_t value)
{

}

/*void cConfigEditor::changeSelectionInGroup(int16_t value)
{
	switch(selectedConfigGroup)
	{
	case configDefaultGeneral: break;
	case configDefaultAudio: break;
	case configDefaultMIDI: break;
	case configDefaultInterface: break;
	case configDefaultSD: break;
	case configDefaultFirmware: changeFirmwareSelection(value); break;
	}
}*/


//======================================================================================================================
//master
void changeVolume(int16_t value)
{
	if(mtProject.values.volume + value < MASTER_VOLUME_MIN) mtProject.values.volume = MASTER_VOLUME_MIN;
	else if(mtProject.values.volume + value > MASTER_VOLUME_MAX) mtProject.values.volume = MASTER_VOLUME_MAX;
	else mtProject.values.volume += value;


	engine.setHeadphonesVolume(mtProject.values.volume);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showVolume();
}

void changeReverbRoomSize(int16_t value)
{
	if(mtProject.values.reverbRoomSize + value < REVERB_ROOM_SIZE_MIN) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MIN;
	else if(mtProject.values.reverbRoomSize + value > REVERB_ROOM_SIZE_MAX) mtProject.values.reverbRoomSize = REVERB_ROOM_SIZE_MAX;
	else mtProject.values.reverbRoomSize += value;

	engine.setReverbRoomsize(mtProject.values.reverbRoomSize);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showReverbSize();
}

void changeReverbDamping(int16_t value)
{
	if(mtProject.values.reverbDamping + value < REVERB_DAMPING_MIN) mtProject.values.reverbDamping = REVERB_DAMPING_MIN;
	else if(mtProject.values.reverbDamping + value > REVERB_DAMPING_MAX) mtProject.values.reverbDamping = REVERB_DAMPING_MAX;
	else mtProject.values.reverbDamping += value;

	engine.setReverbDamping(mtProject.values.reverbDamping);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showReverbDamping();
}

void changeLimiterAttack(int16_t value)
{
	value *= LIMITER_ATTACK_MAX/100;

	if(mtProject.values.limiterAttack + value < LIMITER_ATTACK_MIN) mtProject.values.limiterAttack = LIMITER_ATTACK_MIN;
	else if(mtProject.values.limiterAttack + value > LIMITER_ATTACK_MAX) mtProject.values.limiterAttack = LIMITER_ATTACK_MAX;
	else mtProject.values.limiterAttack += value;

	engine.setLimiterAttack(mtProject.values.limiterAttack);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;

	CE->showLimiterAttack();
}

void changeLimiterRelease(int16_t value)
{
	float fvalue = value * (LIMITER_RELEASE_MAX/100);

	if(mtProject.values.limiterRelease + fvalue < LIMITER_RELEASE_MIN) mtProject.values.limiterRelease = LIMITER_RELEASE_MIN;
	else if(mtProject.values.limiterRelease + fvalue > LIMITER_RELEASE_MAX) mtProject.values.limiterRelease = LIMITER_RELEASE_MAX;
	else mtProject.values.limiterRelease += fvalue;


	engine.setLimiterRelease(mtProject.values.limiterRelease);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showLimiterRelease();
}

void changeLimiterTreshold(int16_t value)
{
	value *= LIMITER_TRESHOLD_MAX/100;

	if(mtProject.values.limiterTreshold + value < LIMITER_TRESHOLD_MIN) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MIN;
	else if(mtProject.values.limiterTreshold + value > LIMITER_TRESHOLD_MAX) mtProject.values.limiterTreshold = LIMITER_TRESHOLD_MAX;
	else mtProject.values.limiterTreshold += value;

	engine.setLimiterTreshold(mtProject.values.limiterTreshold);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showLimiterTreshold();
}

void changeBitDepth(int16_t value)
{
	float localValf = value * (BIT_DEPTH_MAX - BIT_DEPTH_MIN)/100.0;
	int8_t localVal;

	if(localValf < 1.0f && localValf > 0.0f ) localVal = 1;
	else if(localValf > -1.0f && localValf < 0.0f ) localVal = -1;
	else localVal = round(localValf);

	if(mtProject.values.bitDepth + localVal < BIT_DEPTH_MIN) mtProject.values.bitDepth = BIT_DEPTH_MIN;
	else if(mtProject.values.bitDepth + localVal > BIT_DEPTH_MAX) mtProject.values.bitDepth = BIT_DEPTH_MAX;
	else mtProject.values.bitDepth += localVal;

	engine.setBitDepth(mtProject.values.bitDepth);
	mtProject.values.projectNotSavedFlag = 1;
	fileManager.configIsChangedFlag = 1;
	CE->showBitDepth();
}

void cConfigEditor::listAllFirmwares()
{
	uint8_t locationFileCount=0;
	uint8_t validFilesCount=0;

	sdLocation.close();
	if(sdLocation.open("/firmware", O_READ))
	{
		locationFileCount = sdLocation.createFilesListShort(0,&firmwareNamesList[0][0], firmware_list_max,firmware_name_length);

		sdLocation.close();

		if(locationFileCount > firmware_list_max)
		{
			locationFileCount = firmware_list_max;
		}

		for(uint8_t i = 0; i < locationFileCount; i++)
		{
			if(checkIfFirmwareValid(&firmwareNamesList[i][0]))
			{
				strncpy(&firmwareNamesList[validFilesCount][0], &firmwareNamesList[i][0], firmware_name_length);
				validFilesCount++;
			}
		}

		firmwareFoundNum = validFilesCount;
	}
	else
	{
		firmwareFoundNum=0;
	}
}

uint8_t checkIfFirmwareValid(char *name)
{
	uint8_t nameLength;

	nameLength=strlen(name);

	if(nameLength>12)//8.3 convetion
	{
		return 0;
	}

	if(name[0] != 'm' || name[1] != 't')
	{
		return 0;
	}

	if(name[nameLength-1] != 'x' || name[nameLength-2] != 'e' || name[nameLength-3] != 'h')
	{
		return 0;
	}

	return 1;

}

static uint8_t prepareAndFlash()
{
	if(SD.exists("/firmware/_fwinfo")) // plik nie powinien istniec, bootloader sam go usunie
	{
		SD.remove("/firmware/_fwinfo");
	}

	FsFile fwinfo;

	fwinfo = SD.open("/firmware/_fwinfo", FILE_WRITE);
	fwinfo.write(&CE->firmwareNamesList[CE->selectedConfigGroup[1]][0], 13);
	fwinfo.close();

	pinMode(BOOTLOADER_PIN,OUTPUT);
	digitalWrite(BOOTLOADER_PIN,LOW);

	while(1);
}

void cConfigEditor::showFlashingWarning()
{
	if(firmwareFoundNum)
	{
		FM->clearButtonsRange(interfaceButton0,interfaceButton7);

		FM->setButtonObj(interfaceButton7, buttonPress, prepareAndFlash);
		FM->setButtonObj(interfaceButton6, buttonPress, hideFlashingWarning);
		selectionActive=1;
		showFirmwareUpdatePopout();
	}
}

static uint8_t hideFlashingWarning()
{
	CE->selectionActive = 0;
	CE->start(0);

	return 1;
}

// MULTISEL
void cConfigEditor::addNode(editFunct_t funct , uint8_t nodeNum)
{
	if(selectNodes[nodeNum].isActive == 0)
	{
		selectNodes[nodeNum].isActive = 1;
		selectNodes[nodeNum].editFunct = funct;
	}
}

void cConfigEditor::removeNode(uint8_t nodeNum)
{
	selectNodes[nodeNum].isActive = 0;
	selectNodes[nodeNum].editFunct = NULL;
}

void cConfigEditor::stepThroughNodes(int16_t value)
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		if(selectNodes[node].isActive)
		{
			if(selectNodes[node].editFunct != NULL)
			{
				selectNodes[node].editFunct(value);
			}
		}
	}
}

void cConfigEditor::clearAllNodes()
{
	for(uint8_t node = 0; node < MAX_SELECT_NODES; node++)
	{
		selectNodes[node].isActive = 0;
		selectNodes[node].editFunct = NULL;
	}
}

void cConfigEditor::cancelMultiFrame()
{
	for(uint8_t i = 0; i < MAX_SELECT_NODES; i++)
	{
		CE->frameData.multisel[i].isActive = 0;
	}

	CE->frameData.multiSelActiveNum = 0;
}



