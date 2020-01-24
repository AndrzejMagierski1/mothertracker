

#include "mtTest.h"
#include "mtPadBoard.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"
#include "configEditor/configEditor.h"
#include "mtSequencer.h"
#include "mtConfig.h"

#include "patternEditor/patternEditor.h"

#include "mtAudioEngine.h"

#include "SI4703.h"

#include "core/interfacePopups.h"
#include "debugLog.h"


cConfigEditor configEditor;
static cConfigEditor* CE = &configEditor;

extern strMtProject mtProject;
extern AudioControlSGTL5000 audioShield;


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



//master tracks



static  uint8_t functEncoder(int16_t value);

static  uint8_t functSwitchModule(uint8_t button);

static  uint8_t functSwitchModeConfig(uint8_t state);


uint8_t checkIfFirmwareValid(char *name);
//static uint8_t selectFirmware();

static uint8_t prepareAndFlash();
static uint8_t hideFlashingWarning();


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

	createConfigMenu();

//--------------------------------------------------------------------

	clearAllNodes();
	cancelMultiFrame();


	// ustawienie funkcji
	FM->setButtonObj(interfaceButtonParams, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPerformance, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonFile, buttonPress, functSwitchModule);
	//FM->setButtonObj(interfaceButtonConfig, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonMaster, buttonPress, functSwitchModule);

	//FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
	//FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

	FM->setButtonObj(interfaceButtonSamplePlay, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleEdit, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleRec, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSampleLoad, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonSong, buttonPress, functSwitchModule);
	FM->setButtonObj(interfaceButtonPattern, buttonPress, functSwitchModule);





	selectedPlace = 0;
	resizeLabelConfigDefault();

	showDefaultConfigScreen();
	setConfigScreenFunct();

	for(uint8_t i = 0; i < 6; i++)
	{
		listDataForLists(0, i, &groupNamesLabels[i][0]);
	}

	setDataForLists(0, 6);

	chainProcessChange(0, 1);

	activateLabelsBorder();
}


void cConfigEditor::stop()
{
	exitOnButtonRelease = 0;
	moduleRefresh = 0;
}

void cConfigEditor::turnOffPerformanceMode()
{
	if(sequencer.isPerformanceMode())
	{
		fileManager.loadPattern(mtProject.values.actualPattern);
		sequencer.switchRamPatternsNow();
		sequencer.exitPerformanceMode();
	}

	engine.performanceModeEndAll();

}

void cConfigEditor::turnOffRadio()
{
	audioShield.headphoneSourceSelect(0);
	radio.clearRDS();
	radio.resetSeekCallback();
	engine.setHeadphonesVolume(mtProject.values.volume);
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


//	FM->setButtonObj(interfaceButtonConfig, functSwitchModeConfig);
//	FM->setButtonObj(interfaceButtonMaster, functSwitchModeMaster);

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

	if(groupNum == 1)
	{

		display.setControlValue(configSubmenuListControl, selectedConfigGroup[groupNum]);
		display.refreshControl(configSubmenuListControl);
	}
	else
	{
		display.setControlValue(configGroupsListControl[groupNum], selectedConfigGroup[groupNum]);
		display.refreshControl(configGroupsListControl[groupNum]);
	}
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
		listDataForLists(1, i, &generalConfig[i][0]);
	}

	submenuList.linesCount = 14;
	submenuList.start = selectedConfigGroup[1];
	submenuList.length = GERERAL_SUBMENUS;
	submenuList.params = configGroupsNames[1];
	submenuList.values = configGroupsNames[1];

	selectedConfigGroupMax[1] = GERERAL_SUBMENUS;

	display.setControlData(configSubmenuListControl, &submenuList);
	display.setControlShow(configSubmenuListControl);
	display.refreshControl(configSubmenuListControl);

	listsActive[1] = 1;

	changeLabelText(2, "General");
}

void cConfigEditor::showFirmwares(uint8_t listPosition)
{
	listAllFirmwares();

	for(uint8_t i = 0; i < firmwareFoundNum; i++)
	{
		listDataForLists(listPosition, i, &firmwareNamesList[i][0]);
	}

	selectedConfigGroupMax[1] = firmwareFoundNum;

	submenuList.linesCount = 14;
	submenuList.start = selectedConfigGroup[1];
	submenuList.length = firmwareFoundNum;
	submenuList.params = configGroupsNames[1];
	submenuList.values = configGroupsNames[1];

	display.setControlData(configSubmenuListControl, &submenuList);
	display.setControlShow(configSubmenuListControl);
	display.refreshControl(configSubmenuListControl);

	listsActive[1] = 1;

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
	for(uint8_t i = 0; i < configMIDI_count; i++)
	{
		listDataForLists(listPosition, i, &txtlistMidiConfig[i][0]);
	}

	selectedConfigGroupMax[1] = MIDI_SUBMENUS;

	submenuList.linesCount = 14;
	submenuList.start = selectedConfigGroup[1];
	submenuList.length = MIDI_SUBMENUS;
	submenuList.params = configGroupsNames[1];
	submenuList.values = configGroupsNames[1];

	display.setControlData(configSubmenuListControl, &submenuList);
	display.setControlShow(configSubmenuListControl);
	display.refreshControl(configSubmenuListControl);

	listsActive[1] = 1;

	//setDataForLists(listPosition, MIDI_SUBMENUS);

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
	for(uint8_t i = 0; i < clockOut_count; i++)
	{
		listDataForLists(listPosition, i, &clockOut[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.clkOut;

	setDataForLists(listPosition, clockOut_count);
	changeLabelText(4, "Clock out");
}

void cConfigEditor::showTransportIn(uint8_t listPosition)
{
	for(uint8_t i = 0; i < clockOut_count; i++)
	{
		listDataForLists(listPosition, i, &clockOut[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.transportIn;

	setDataForLists(listPosition, clockOut_count);
	changeLabelText(4, "Transport in");
}

void cConfigEditor::showTransportOut(uint8_t listPosition)
{
	for(uint8_t i = 0; i < clockOut_count; i++)
	{
		listDataForLists(listPosition, i, &clockOut[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.transportOut;

	setDataForLists(listPosition, clockOut_count);
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


void cConfigEditor::showMidiNotesIn_mode(uint8_t listPosition)
{

	for(uint8_t i = 0; i < NOTES_FROM_MODE_NUM; i++)
	{
		listDataForLists(listPosition, i, &notesFrom[i][0]);
	}
	selectedConfigGroup[listPosition] = mtConfig.midi.notesIn;

	setDataForLists(listPosition, NOTES_FROM_MODE_NUM);
	changeLabelText(4, "Notes in mode");
}
void cConfigEditor::showMidiNotesOut_mode(uint8_t listPosition)
{

	for(uint8_t i = 0; i < NOTES_OUT_NUM; i++)
	{
		listDataForLists(listPosition, i, &notesOut_mode[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.notesOutMode ;

	setDataForLists(listPosition, NOTES_OUT_NUM);
	changeLabelText(4, "Notes out");
}
void cConfigEditor::showMidiNotesInChannels(uint8_t listPosition)
{
	for(uint8_t i = 0; i < NOTES_FROM_CHANNEL_NUM; i++)
	{
		listDataForLists(listPosition, i, &notesInChannel[i][0]);

	}
	selectedConfigGroup[listPosition] = mtConfig.midi.notesInChannel ;

	setDataForLists(listPosition, NOTES_FROM_CHANNEL_NUM);
	changeLabelText(4, "Notes channel");
}
void cConfigEditor::showMidiNotesOutChannels(uint8_t listPosition)
{
	for(uint8_t i = 0; i < NOTES_OUT_CHANNEL_NUM; i++)
	{
		listDataForLists(listPosition, i, &notesOutChannel[i][0]);
	}

	selectedConfigGroup[listPosition] = mtConfig.midi.notesOutChannel ;

	setDataForLists(listPosition, NOTES_OUT_CHANNEL_NUM);
	changeLabelText(4, "Notes out channel");
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
		patternEditor.trackerPattern.stepDevider = mtConfig.general.patternDiv + 1;

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
uint8_t cConfigEditor::setNotesIn(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.notesIn != val)
	{
		mtConfig.midi.notesIn = val;
		isChanged = 1;
	}

	return isChanged;
}
uint8_t cConfigEditor::setNotesInChannel(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.notesInChannel != val)
	{
		mtConfig.midi.notesInChannel = val;
		isChanged = 1;
	}

	return isChanged;
}
uint8_t cConfigEditor::setNotesOutMode(uint32_t val)
{
	uint8_t isChanged = 0;
	if(mtConfig.midi.notesOutMode != val)
	{
		mtConfig.midi.notesOutMode = val;
		isChanged = 1;
	}

	return isChanged;
}
uint8_t cConfigEditor::setNotesOutChannel(uint32_t val)
{

	val++;
	uint8_t isChanged = 0;
	if(mtConfig.midi.notesOutChannel != val)
	{
		mtConfig.midi.notesOutChannel = val;
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
		case configMIDINotesFrom_mode:
			showMidiNotesIn_mode(2);
			break;
		case configMIDINotesFrom_channel:
			showMidiNotesInChannels(2);
			break;
		case configMIDINotesOut_mode:
			showMidiNotesOut_mode(2);
			break;
		case configMIDINotesOut_channel:
			showMidiNotesOutChannels(2);
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

	else if (selectedConfigGroup[0] == configDefaultMIDI)
	{

		switch (selectedConfigGroup[1])
		{
		case configMIDIClockIn:
			hasConfigChanged = setClockIn(dataSource);
			break;
		case configMIDIClockOut:
			hasConfigChanged = setClockOut(dataSource);
			break;
		case configMIDITransportIn:
			hasConfigChanged = setTransportIn(dataSource);
			break;
		case configMIDITansportOut:
			hasConfigChanged = setTransportOut(dataSource);
			break;
		case configMIDICcout:
			showCCnumber(3);
			break;


		case configMIDINotesFrom_mode:
			hasConfigChanged = setNotesIn(dataSource);
			break;
		case configMIDINotesFrom_channel:
			hasConfigChanged = setNotesInChannel(dataSource);
			break;
		case configMIDINotesOut_mode:
			hasConfigChanged = setNotesOutMode(dataSource);
			break;
		case configMIDINotesOut_channel:
			hasConfigChanged = setNotesOutChannel(dataSource);
			break;
		default:
			break;
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
			if(CE->selectedPlace == 0)
			{
				CE->changeSelectionInGroup(-1, 0);
			}
			else
			{
				CE->selectedPlace = 0;
			}
			break;
		}
		case 1:
		{
			if(CE->selectedPlace == 0)
			{
				CE->changeSelectionInGroup(1, 0);
			}
			else
			{
				CE->selectedPlace = 0;
			}
			break;
		}
		case 2:
		{
			if(CE->listsActive[1])
			{
				if(CE->selectedPlace == 1)
				{
					CE->changeSelectionInGroup(-1, 1);
				}
				else
				{
					CE->selectedPlace = 1;
				}
			}
			break;
		}
		case 3:
		{
			if(CE->listsActive[1])
			{
				if(CE->selectedPlace == 1)
				{
					CE->changeSelectionInGroup(1, 1);
				}
				else
				{
					CE->selectedPlace = 1;
				}
			}
			break;
		}
		case 4:
		{
			if(CE->listsActive[2])
			{
				if(CE->selectedPlace == 2)
				{
					CE->changeSelectionInGroup(-1, 2);
				}
				else
				{
					CE->selectedPlace = 2;
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
				if(CE->selectedPlace == 2)
				{
					CE->changeSelectionInGroup(1, 2);
				}
				else
				{
					CE->selectedPlace = 2;
				}
			}
			//CE->eventFunct(eventActivateImageViewer,CE,0,0);
			break;
		}
		case 6:
		{
			if(CE->listsActive[3])
			{
				if(CE->selectedPlace == 3)
				{
					CE->changeSelectionInGroup(-1, 3);
				}
				else
				{
					CE->selectedPlace = 3;
				}
			}
			break;
		}
		case 7:
		{
			if(CE->listsActive[3])
			{
				if(CE->selectedPlace == 3)
				{
					CE->changeSelectionInGroup(1, 3);
				}
				else
				{
					CE->selectedPlace = 3;
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

	CE->selectedPlace = 6;
	CE->activateLabelsBorder();


	return 1;
}*/




static  uint8_t functPads(uint8_t pad, uint8_t state, int16_t velo)
{
	if(state != buttonPress) return 1;

	if(mtTest.runTestByCombinaion(pad)) 	CE->eventFunct(eventActivateTestingProcedure,CE,0,0);


	if(pad == 0)
	{
		mtPopups.config(4, (strPopupStyleConfig*)&popupDebugConfig);
		if(debugLog.toggleState())
		{
			mtPopups.show(4, "Debug log enabled");
		}
		else
		{
			mtPopups.show(4, "Debug log disabled");
		}

	}




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
		switch(CE->selectedPlace)
		{
		case 0: 	 CE->changeSelectionInGroup(value, 0); break;
		case 1:		 CE->changeSelectionInGroup(value, 1); break;
		case 2: 	 CE->changeSelectionInGroup(value, 2); break;
		case 3: 	 CE->changeSelectionInGroup(value, 3); break;

		}
	}

	return 1;
}






static  uint8_t functLeft()
{
	if(CE->selectionActive) return 1;
	if(CE->frameData.multiSelActiveNum != 0) return 1;


	if(CE->selectedPlace > 0) CE->selectedPlace--;


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
		if(CE->selectedPlace < (maxPlaces-1)) CE->selectedPlace++;
	}
	else
	{
		if(CE->selectedPlace < CE->frameData.placesCount-1) CE->selectedPlace++;
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
		switch(CE->selectedPlace)
		{
		case 0: 	 CE->changeSelectionInGroup(-1, 0); break;
		case 1:		 CE->changeSelectionInGroup(-1, 1); break;
		case 2: 	 CE->changeSelectionInGroup(-1, 2); break;
		case 3: 	 CE->changeSelectionInGroup(-1, 3); break;

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
		switch(CE->selectedPlace)
		{
		case 0: 	 CE->changeSelectionInGroup(1, 0); break;
		case 1:		 CE->changeSelectionInGroup(1, 1); break;
		case 2: 	 CE->changeSelectionInGroup(1, 2); break;
		case 3: 	 CE->changeSelectionInGroup(1, 3); break;

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

static uint8_t functSwitchModule(uint8_t button)
{
	if(CE->selectionActive) return 1;

//	if(button != interfaceButtonPerformance) CE->turnOffPerformanceMode();
//	if(button != interfaceButtonSampleRec) CE->turnOffRadio();

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

	SdFile fwinfo;

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




