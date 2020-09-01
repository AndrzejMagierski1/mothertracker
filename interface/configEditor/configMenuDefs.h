
#ifndef INTERFACE_CONFIGEDITOR_CONFIGMENUDEFS_H_
#define INTERFACE_CONFIGEDITOR_CONFIGMENUDEFS_H_

#include <stdint.h>
#include "scales.h"


typedef enum enMenuType
{
	menuTypeEmpty,
	menuTypeGroup,
	menuTypeItem,

} menu_t;

typedef enum enMenuItemType
{
	menuItemTypeEmpty,
	menuItemTypeValueU8,
	menuTypeItemListValues,
	menuTypeItemListText,
	menuTypeItemListTextWithAction,
	menuTypeItemLabel,
	menuTypeItemActionButton,

} menu_item_t;


struct strItemTypeValue8
{
	uint8_t* value;
	uint8_t min;
	uint8_t max;
	uint8_t interval; // narazie nie uzywane
	uint8_t dev;
};

struct strItemTypeListValues
{
	uint8_t* value;
	uint8_t min;
	uint8_t max;
	uint8_t interval; // narazie nie uzywane
	uint8_t dev;
};

struct strItemTypeListText
{
	uint8_t* value;
	uint8_t count;
	const char*const*  ptrText;
};

struct strItemTypeListTextWithAction
{
	uint8_t* value;
	uint8_t count;
	const char*const*  ptrText;
	void (*funct1)(void) = nullptr;
};

struct strItemTypeLabel
{
	char* text;
};

struct strItemTypeActionButton
{
	void (*funct1)(void) = nullptr;
	void (*funct2)(void) = nullptr;
};


const char emptyName[2] = "";
const char* const  ptrEmptyName = emptyName;


//===========================================================
//===========================================================
//===========================================================
// templates

const char onOffStateTexts[2][7] =
{
		"Off",
		"On",
};



// general


const char brightness[3][7] =
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
		&radioRegion[0][0],
		&radioRegion[1][0],
		&radioRegion[2][0],
		&radioRegion[3][0],
};


const char recOptionsTexts[4][20] =
{
		"Only notes",
		"Microtiming",
		"Velocity",
		"Microtim + Velo",
};

const char* const ptrRecQuantization[4] =
{
		&recOptionsTexts[0][0],
		&recOptionsTexts[1][0],
		&recOptionsTexts[2][0],
		&recOptionsTexts[3][0],
};

const char performanceSourceTexts[2][12] =
{
		"Global",
		"Per Project",
};

const char* const ptrPerformanceSource[2] =
{
	&performanceSourceTexts[0][0],
	&performanceSourceTexts[1][0],
};

const char padsLayoutTexts[12][3] =
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
};

const char* const ptrPadsLayout[12] =
{
	&padsLayoutTexts[0][0],
	&padsLayoutTexts[1][0],
	&padsLayoutTexts[2][0],
	&padsLayoutTexts[3][0],
	&padsLayoutTexts[4][0],
	&padsLayoutTexts[5][0],
	&padsLayoutTexts[6][0],
	&padsLayoutTexts[7][0],
	&padsLayoutTexts[8][0],
	&padsLayoutTexts[9][0],
	&padsLayoutTexts[10][0],
	&padsLayoutTexts[11][0],
};

const char* const ptrAntyaliasingEnable[2] =
{
	&onOffStateTexts[0][0],
	&onOffStateTexts[1][0]
};


const char limiterModeTexts[2][12] =
{
		"Subtle",
		"Extreme",
};

const char* const ptrLimiterMode[2] =
{
	&limiterModeTexts[0][0],
	&limiterModeTexts[1][0]
};
/*
const char Texts[2][7] =
{
		"",
		"",
};

const char* const ptr[2] =
{
	&Texts[0][0],
	&Texts[1][0],
};
*/

const char* const ptrMtpState[2] =
{
		&onOffStateTexts[0][0],
		&onOffStateTexts[1][0],
};


// metronome

const char* const ptrMetroState[2] =
{
		&onOffStateTexts[0][0],
		&onOffStateTexts[1][0],
};
const char* const ptrPreRollState[2] =
{
		&onOffStateTexts[0][0],
		&onOffStateTexts[1][0],
};

const char metroTimeSig[12][3] =
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
};

const char* const ptrMetroTimeSig[12] =
{
	&padsLayoutTexts[0][0],
	&padsLayoutTexts[1][0],
	&padsLayoutTexts[2][0],
	&padsLayoutTexts[3][0],
	&padsLayoutTexts[4][0],
	&padsLayoutTexts[5][0],
	&padsLayoutTexts[6][0],
	&padsLayoutTexts[7][0],
	&padsLayoutTexts[8][0],
	&padsLayoutTexts[9][0],
	&padsLayoutTexts[10][0],
	&padsLayoutTexts[11][0],
};



//===========================================================
//===========================================================
//===========================================================
// midi


const char clockIn[3][13] =
{
		"Internal",
		"USB",
		"MIDI In jack",
};

const char* const ptrClockIn[3] =
{
		&clockIn[0][0],
		&clockIn[1][0],
		&clockIn[2][0],
};

const char midiOut[4][20] =
{
		"Off",
		"USB",
		"MIDI Out jack",
		"USB + MIDI Out jack",
};

const char* const ptrMidiOut[4] =
{
		&midiOut[0][0],
		&midiOut[1][0],
		&midiOut[2][0],
		&midiOut[3][0],
};

const char midiIn[4][19] =
{
		"Off",
		"USB",
		"MIDI In jack",
		"USB + MIDI In jack",
};

const char* const ptrMidiIn[4] =
{
		&midiIn[0][0],
		&midiIn[1][0],
		&midiIn[2][0],
		&midiIn[3][0],
};

const char notesChannel[17][5] =
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

const char* const ptrInChannel[17] =
{
		&notesChannel[0][0],
		&notesChannel[1][0],
		&notesChannel[2][0],
		&notesChannel[3][0],
		&notesChannel[4][0],
		&notesChannel[5][0],
		&notesChannel[6][0],
		&notesChannel[7][0],
		&notesChannel[8][0],
		&notesChannel[9][0],
		&notesChannel[10][0],
		&notesChannel[11][0],
		&notesChannel[12][0],
		&notesChannel[13][0],
		&notesChannel[14][0],
		&notesChannel[15][0],
		&notesChannel[16][0],
};


const char* const ptrOutChannel[16] =
{
		&notesChannel[1][0],
		&notesChannel[2][0],
		&notesChannel[3][0],
		&notesChannel[4][0],
		&notesChannel[5][0],
		&notesChannel[6][0],
		&notesChannel[7][0],
		&notesChannel[8][0],
		&notesChannel[9][0],
		&notesChannel[10][0],
		&notesChannel[11][0],
		&notesChannel[12][0],
		&notesChannel[13][0],
		&notesChannel[14][0],
		&notesChannel[15][0],
		&notesChannel[16][0],
};

const char CCouts[10][5] =
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


//===========================================================
//===========================================================
//===========================================================



const char CCnumber[127][4] =
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
