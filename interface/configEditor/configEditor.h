#ifndef INTERFACE_CONFIGEDITOR_H_
#define INTERFACE_CONFIGEDITOR_H_


#include "core/modulesBase.h"
#include "SD.h"



#include "mtStructs.h"


enum mtConfigMode
{
	mtConfigModeDefault,
	mtConfigModeMaster,



};

enum mtConfigSpecific
{
	configDefaultGeneral,
	configDefaultFirmware,
	configDefaultMIDI,
	configDefaultSD,
	configDefaultHelp,
	configDefaultCredits,

	mtConfigGroupsCount
};

enum mtConfigMIDI
{
	configMIDIClockIn,
	configMIDIClockOut,
	configMIDITransportIn,
	configMIDITansportOut,
	configMIDICcout,
	configMIDINotesFrom,
	configMIDINotesChannel,
};

enum mtConfigGeneral
{
	configGeneralPatternDiv,
	configGeneralRadioRegion,
	configGeneralBrightness,

};

constexpr uint8_t GERERAL_SUBMENUS = 3;
constexpr uint8_t MIDI_SUBMENUS = 7;
constexpr uint8_t PATTERN_DIVIDERS = 16;
constexpr uint8_t RADIO_REGIONS = 4;
constexpr uint8_t BRIGHTNESS_LEVELS = 3;
constexpr uint8_t CLOCK_IN = 3;
constexpr uint8_t CLOCK_OUT = 4;
constexpr uint8_t CC_OUTS_NUM = 10;
constexpr uint8_t CC_NUMBERS = 127;
constexpr uint8_t NOTES_FROM_NUM = 4;
constexpr uint8_t NOTES_FROM_CHANNEL_NUM = 16;


const char groupNamesLabels[mtConfigGroupsCount][15] =
{
		"General",
		"Firmware",
		"MIDI",
		"SD",
		"Help",
		"Credits"
};

const char generalConfig[GERERAL_SUBMENUS][20] =
{
		"Pattern divider",
		"Radio region",
		"Brightness"
};

const char midiConfig[MIDI_SUBMENUS][20] =
{
		"Clock in",
		"Clock out",
		"Transport in",
		"Transport out",
		"CC out",
		"Notes from",
		"Notes chan",
};

const char patternDivider[PATTERN_DIVIDERS][3] =
{
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
};

const char radioRegion[RADIO_REGIONS][10] =
{
		"Europe",
		"US",
		"Australia",
		"Japan",
};

const char brightness[BRIGHTNESS_LEVELS][5] =
{
		"Low",
		"Mid",
		"High",
};

const char clockIn[CLOCK_IN][9] =
{
		"Internal",
		"USB",
		"Midi in",
};

const char clockOut[CLOCK_OUT][20] =
{
		"Off",
		"USB",
		"Midi out",
		"USB and Midi out",
};

enum enClockOut
{
	clockOut_Off,
	clockOut_usb,
	clockOut_jack,
	clockOut_usb_and_jack,
};

enum enClockIn
{
	clockIn_Internal,
	clockIn_Usb,
	clockIn_Jack,

};

const char CCouts[CC_OUTS_NUM][5] =
{
		"CC1",
		"CC2",
		"CC3",
		"CC4",
		"CC5",
		"CC6",
		"CC7",
		"CC8",
		"CC9",
		"CC10",
};
const char notesFromChannel[NOTES_FROM_CHANNEL_NUM][5] =
{
		"CH1",
		"CH2",
		"CH3",
		"CH4",
		"CH5",
		"CH6",
		"CH7",
		"CH8",
		"CH9",
		"CH10",
		"CH11",
		"CH12",
		"CH13",
		"CH14",
		"CH15",
		"CH16",
};
const char notesFrom[NOTES_FROM_CHANNEL_NUM][10] =
{
		"None",
		"USB",
		"Jack",
		"USB+Jack",
};
enum enMidiNotesFrom
{
	enMidiNotesFrom_none,
	enMidiNotesFrom_USB,
	enMidiNotesFrom_jack,
	enMidiNotesFrom_USB_and_jack,

};
const char CCnumber[CC_NUMBERS][4] =
{
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"17",
		"18",
		"19",
		"20",
		"21",
		"22",
		"23",
		"24",
		"25",
		"26",
		"27",
		"28",
		"29",
		"30",
		"31",
		"32",
		"33",
		"34",
		"35",
		"36",
		"37",
		"38",
		"39",
		"40",
		"41",
		"42",
		"43",
		"44",
		"45",
		"46",
		"47",
		"48",
		"49",
		"50",
		"51",
		"52",
		"53",
		"54",
		"55",
		"56",
		"57",
		"58",
		"59",
		"60",
		"61",
		"62",
		"63",
		"64",
		"65",
		"66",
		"67",
		"68",
		"69",
		"70",
		"71",
		"72",
		"73",
		"74",
		"75",
		"76",
		"77",
		"78",
		"79",
		"80",
		"81",
		"82",
		"83",
		"84",
		"85",
		"86",
		"87",
		"88",
		"89",
		"90",
		"91",
		"92",
		"93",
		"94",
		"95",
		"96",
		"97",
		"98",
		"99",
		"100",
		"101",
		"102",
		"103",
		"104",
		"105",
		"106",
		"107",
		"108",
		"109",
		"110",
		"111",
		"112",
		"113",
		"114",
		"115",
		"116",
		"117",
		"118",
		"119",
		"120",
		"121",
		"122",
		"123",
		"124",
		"125",
		"126",
		"127",
};


#undef MAX_SELECT_NODES
#define MAX_SELECT_NODES	7

const uint8_t firmware_list_max=10;
const uint8_t firmware_name_length=15;


class cConfigEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cConfigEditor()
	{
		selectedConfigGroup[4] = {0};
		label[8] = {nullptr};
		barControl[8] = {nullptr};
		configGroupsListControl[4] = {nullptr};
		editorInstrument = nullptr;
		frameControl = nullptr;
		firmwareListControl = nullptr;
		popoutWindowLabel = nullptr;

	}
	virtual ~cConfigEditor() {}


	//-----------------------------------------

	void showDefaultConfigScreen();
	void showMasterScreen();

	void turnOffPerformanceMode();
	void turnOffRadio();

	void changeSelectionInGroup(int16_t value, uint8_t groupNum);


	//config
	void showConfigGroupList(strList *data , uint8_t listNum);

	//master
	void showVolume();
	void showReverbSize();
	void showReverbDamping();
	void showLimiterAttack();
	void showLimiterRelease();
	void showLimiterTreshold();
	void showBitDepth();

	//master tracks



	//

	//-----------------------------------------

	void setConfigScreenFunct();
	void setMasterScreenFunct();


	// config
	void changeConfigGroupSelection(int16_t value);

/*	//master
	void changeVolume(int16_t value);
	void changeReverbRoomSize(int16_t value);
	void changeReverbDamping(int16_t value);
	void changeLimiterAttack(int16_t value);
	void changeLimiterRelease(int16_t value);
	void changeLimiterTreshold(int16_t value);
	void changeBitDepth(int16_t value);*/


	//master tracks



	//
	void activateLabelsBorder();

	// firmware
	uint8_t firmwareFoundNum;
	SdFile sdLocation;
	uint8_t listInitFlag=0;
	char firmwareNamesList[firmware_list_max][firmware_name_length];
	hControl firmwareListControl;
	hControl popoutWindowLabel;

	strList firmwareList;
	uint8_t selectionActive;
	void showFirmwareMenu();
	void hideFirmwareMenu();

	void listAllFirmwares();

	void showFlashingWarning();

	void showFirmwareUpdatePopout();
	void hideFirmwareUpdatePopout();

	void resizeLabelConfigDefault();
	void resizeLabelConfigMaster();
	void resizeFirmwareLabel(uint8_t control);// 0 - revert to normal, 1 - rescale to double label;

	uint8_t processUpdate = 0;

//----------------------------------

	strFrameData frameData;

	hControl label[8];
	hControl barControl[8];

	hControl configGroupsListControl[4];

	hControl titleBar = nullptr;
	hControl titleLabel = nullptr;
	hControl instrumentLabel = nullptr;

	hControl bgLabel;
	hControl frameControl;


	uint8_t selectedPlace[3] = {6,0,0};

	// typ trybu/ekranu
	uint8_t mode = 0;

	strInstrument * editorInstrument;

//----------------------------------
// lista play mode
	strList configGroupList[4];

	void listConfigGroups();

	char *configGroupsNames[4][255];

	uint8_t selectedConfigGroup[4];
	uint8_t selectedConfigGroupMax[4];
	uint8_t listsActive[4];
	uint8_t previousSelectedConfigGroup=UINT8_MAX;

//----------------------------------

	char volumeVal[4];
	char reverbSizeVal[4];
	char reverbDampVal[4];
	char limitAttackVal[8];
	char limitReleaseVal[8];
	char limitThresholdVal[4];
	char bitDepthVal[4];

//----------------------------------

	uint8_t exitOnButtonRelease = 0;

	// MULTISEL
	select_node_t selectNodes[MAX_SELECT_NODES];

	void addNode(editFunct_t funct , uint8_t nodeNum);
	void removeNode(uint8_t nodeNum);
	void stepThroughNodes(int16_t value);
	void clearAllNodes();
	void cancelMultiFrame();

	uint8_t getListsActive();
	void setDataForLists(uint8_t listNum, uint8_t max);
	void listDataForLists(uint8_t listNum, uint8_t nameNum, const char *names);


	void processChangeInGroup0();
	void processChangeInGroup1();
	void processChangeInGroup2();
	void processChangeInGroup3();
	void chainProcessChange(uint8_t groupNum, uint8_t forceProcess);

	void resizeToDefaultConfig();
	void resizeToDefaultMaster();
	void resizeToSmallConfig(uint8_t labelIdx);

	void showGeneralSubmenus(uint8_t listPosition);
	void showFirmwares(uint8_t listPosition);
	void showMidiSubmenus(uint8_t listPosition);
	void showFirmwareFlashButton();


	void showPatternDividers(uint8_t listPosition);
	void showRadioRegions(uint8_t listPosition);
	void showBrightnessLevels(uint8_t listPosition);

	void showClockIn(uint8_t listPosition);
	void showClockOut(uint8_t listPosition);
	void showTransportIn(uint8_t listPosition);
	void showTransportOut(uint8_t listPosition);
	void showCCouts(uint8_t listPosition);
	void showCCnumber(uint8_t listPosition);
	void showMidiNotesFrom(uint8_t listPosition);
	void showMidiNotesChannels(uint8_t listPosition);




	//setters
	uint8_t setPatternDivider(uint32_t val);
	uint8_t setBrightness(uint32_t val);
	uint8_t setRadioRegion(uint32_t val);
	uint8_t setClockIn(uint32_t val);
	uint8_t setClockOut(uint32_t val);
	uint8_t setTransportIn(uint32_t val);
	uint8_t setTransportOut(uint32_t val);
	uint8_t setCCout(uint8_t ccNum, uint32_t val);

	uint8_t hasConfigChanged = 0;


	void showExecute();



	void changeLabelText(uint8_t labelIdx, const char *text);

};

extern cConfigEditor configEditor;


typedef enum enMenuUnitType
{
	menuUnitEmpty,
	menuUnitParent,
	menuUnitChild,


} menu_t;

class configMenuElement
{
public:



private:
	menu_t type;



};


















#endif /* INTERFACE_CONFIGEDITOR_H_ */
