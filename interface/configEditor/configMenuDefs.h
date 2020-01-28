
#ifndef INTERFACE_CONFIGEDITOR_CONFIGMENUDEFS_H_
#define INTERFACE_CONFIGEDITOR_CONFIGMENUDEFS_H_



typedef enum enMenuType
{
	menuTypeEmpty,
	menuTypeGroup,
	menuTypeItem,

} menu_t;

typedef enum enMenuItemType
{
	menuItemTypeEmpty,
	menuItemTypeValue,
	menuTypeItemListValues,
	menuTypeItemListText,
	menuTypeItemLabel,
	menuTypeItemActionButton,

} menu_item_t;


struct strItemTypeValue
{
	int16_t* value;
	int16_t interval;
	int16_t min;
	int16_t max;
};

struct strItemTypeListValues
{
	uint8_t* value;
	uint8_t min;
	uint8_t max;
	uint8_t interval;

};

struct strItemTypeListText
{
	uint8_t* value;
	uint8_t count;
	const char*const*  ptrText;
};

struct strItemTypeLabel
{

};

struct strItemTypeActionButton
{

};

//===========================================================
//===========================================================
//===========================================================
// general


const char brightness[3][5] =
{
		"Low",
		"Medium",
		"High",
};

const char* const ptrBrightness[3] =
{
		&brightness[0][0],
		&brightness[1][0],
		&brightness[2][0],
};

const char radioRegion[4][10] =
{
		"Europe",
		"US",
		"Australia",
		"Japan",
};

const char* const ptrRadioRegion[4] =
{
		&ptrRadioRegion[0],
		&ptrRadioRegion[1],
		&ptrRadioRegion[2],
		&ptrRadioRegion[3],
};
//===========================================================
//===========================================================
//===========================================================











constexpr uint8_t GERERAL_SUBMENUS = 3;
constexpr uint8_t MIDI_SUBMENUS = 7;
constexpr uint8_t PATTERN_DIVIDERS = 16;
constexpr uint8_t RADIO_REGIONS = 4;
constexpr uint8_t BRIGHTNESS_LEVELS = 3;
constexpr uint8_t CLOCK_IN = 3;
//constexpr uint8_t CLOCK_OUT = 4;
constexpr uint8_t CC_OUTS_NUM = 10;
constexpr uint8_t CC_NUMBERS = 127;
constexpr uint8_t NOTES_FROM_MODE_NUM = 4;
constexpr uint8_t NOTES_FROM_CHANNEL_NUM = 17;

constexpr uint8_t NOTES_OUT_NUM = 4;
constexpr uint8_t NOTES_OUT_CHANNEL_NUM = 16;


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
	configMIDINotesFrom_mode,
	configMIDINotesFrom_channel,
	configMIDINotesOut_mode,
	configMIDINotesOut_channel,

	configMIDI_count
};

enum mtConfigGeneral
{
	configGeneralPatternDiv,
	configGeneralRadioRegion,
	configGeneralBrightness,

};



enum enClockOut
{
	clockOut_Off,
	clockOut_usb,
	clockOut_jack,
	clockOut_usb_and_jack,
	clockOut_count
};

enum enNotesIn
{
	notesIn_None,
	notesIn_usb,
	notesIn_jack,
	notesIn_usb_and_jack,
};

enum enNotesOut
{
	notesOut_mode_None,
	notesOut_mode_usb,
	notesOut_mode_jack,
	notesOut_mode_usb_and_jack,
};

enum enClockIn
{
	clockIn_Internal,
	clockIn_Usb,
	clockIn_Jack,

};

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

const char txtlistMidiConfig[configMIDI_count][20] =
{
		"Clock in",
		"Clock out",
		"Transport in",
		"Transport out",
		"CC out",
		"Notes in mode",
		"Notes in channnel",
		"Notes Out mode",
		"Notes Out channel",
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




const char clockIn[CLOCK_IN][9] =
{
		"Internal",
		"USB",
		"Midi in",
};

const char clockOut[][20] =
{
	"Off",
	"USB",
	"Midi out",
	"USB and Midi out",
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
const char notesInChannel[NOTES_FROM_CHANNEL_NUM][5] =
{
		"ALL",
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
const char notesOutChannel[NOTES_OUT_CHANNEL_NUM][5] =
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
const char notesOut_mode[NOTES_OUT_NUM][10] =
{
		"None",
		"USB",
		"Jack",
		"USB+Jack",
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



#endif /* INTERFACE_CONFIGEDITOR_CONFIGMENUDEFS_H_ */
