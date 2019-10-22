
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H



#include "mtStructs.h"


#define MT_INTERFACE_STARTUP_TIME 10000


//-------------------------------------------------------------
enum mtInterfacePadsDef
{
//1
	interfacePad0,
	interfacePad1,
	interfacePad2,
	interfacePad3,
	interfacePad4,
	interfacePad5,
	interfacePad6,
	interfacePad7,
//2
	interfacePad8,
	interfacePad9,
	interfacePad10,
	interfacePad11,
	interfacePad12,
	interfacePad13,
	interfacePad14,
	interfacePad15,
//3
	interfacePad16,
	interfacePad17,
	interfacePad18,
	interfacePad19,
	interfacePad20,
	interfacePad21,
	interfacePad22,
	interfacePad23,
//4
	interfacePad24,
	interfacePad25,
	interfacePad26,
	interfacePad27,
	interfacePad28,
	interfacePad29,
	interfacePad30,
	interfacePad31,
//5
	interfacePadKey0,

	interfacePadsCount
};

enum mtInterfaceButtonsDef
{
	interfaceButton0,
	interfaceButton1,
	interfaceButton2,
	interfaceButton3,
	interfaceButton4,
	interfaceButton5,
	interfaceButton6,
	interfaceButton7,

	interfaceButton8,
	interfaceButton9,
	interfaceButton10,
	interfaceButton11,
	interfaceButton12,

	interfaceButton13,
	interfaceButton14,
	interfaceButton15,
	interfaceButton16,
	interfaceButton17,

	interfaceButton18,
	interfaceButton19,
	interfaceButton20,
	interfaceButton21,
	interfaceButton22,

	interfaceButton23,
	interfaceButton24,
	interfaceButton25,
	interfaceButton26,
	interfaceButton27,

	interfaceButton28,
	interfaceButton29,
	interfaceButton30,
	interfaceButton31,
	interfaceButton32,

	interfaceButton33,  // encoder switch

	interfaceButtonsCount
};

enum mtInterfacePotsDef
{
	interfacePot0,
	interfacePot1,
	interfacePot2,
	interfacePot3,
	interfacePot4,

	interfacePotsCount
};





enum mtInterfaceButtonsFunctions
{
//  stary układ
//	interfaceButtonPlay 		= interfaceButton23,
//	interfaceButtonRec 			= interfaceButton24,
//	interfaceButtonCopy 		= interfaceButton25,
//	interfaceButtonUp			= interfaceButton26,
//	interfaceButtonPaste 		= interfaceButton27,
//
//	interfaceButtonShift 		= interfaceButton28,
//	interfaceButtonEnter 		= interfaceButton29,
//	interfaceButtonLeft 		= interfaceButton30,
//	interfaceButtonDown 		= interfaceButton31,
//	interfaceButtonRight 		= interfaceButton32,

	interfaceButtonPlay 		= interfaceButton23,
	interfaceButtonCopy 		= interfaceButton24,
	interfaceButtonInsert		= interfaceButton25,
	interfaceButtonUp			= interfaceButton26,
	interfaceButtonDelete		= interfaceButton27,

	interfaceButtonRec 			= interfaceButton28,
	interfaceButtonShift		= interfaceButton29,
	interfaceButtonLeft 		= interfaceButton30,
	interfaceButtonDown 		= interfaceButton31,
	interfaceButtonRight 		= interfaceButton32,

	interfaceButtonEncoder 		= interfaceButton33,


	interfaceButtonParams 		= interfaceButton8,
	interfaceButtonPerformance 	= interfaceButton9,
	interfaceButtonFile 		= interfaceButton10,
	interfaceButtonConfig		= interfaceButton11,
	interfaceButtonMaster 		= interfaceButton12,
	interfaceButtonSampleLoad 	= interfaceButton13,
	interfaceButtonSamplePlay 	= interfaceButton14,
	interfaceButtonSampleEdit 	= interfaceButton15,
	interfaceButtonSampleRec 	= interfaceButton16,
	interfaceButtonSong			= interfaceButton17,
	interfaceButtonNote 		= interfaceButton18,
	interfaceButtonInstr 		= interfaceButton19,
	interfaceButtonVol 			= interfaceButton20,
	interfaceButtonFx 			= interfaceButton21,
	interfaceButtonPattern 		= interfaceButton22,


};

//-------------------------------------------------------------
enum buttonStates
{
	buttonRelease,
	buttonPress,
	buttonHold,
	buttonDouble,
};

enum padStates
{
	padRelease,
	padPress,
	padChange,
};


//-------------------------------------------------------------
enum interfacePopups
{
	mtNotePopup,
	mtIntrPopup,
	mtVolPopup,
	mtFxPopup,
	mtGlobalPopup,


};


//-------------------------------------------------------------


enum interfaceEvents
{
	eventNone,
	eventLoadLastProjFailed,
	eventEventPadPress,
	eventLoadSampleBank,

	eventSwitchModule,
	eventSwitchToPreviousModule,
	eventSwitchModuleWithOptions,
	eventToggleActiveModule,

	eventActivateGameModule,



	interfaceEventsCount
};



enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeRun,
};


struct strSpectrumParams
{
	int16_t* address = nullptr;
	uint32_t length = 0;
	uint8_t recordInProgressFlag = 0;
};


struct strZoomParams
{
	uint16_t zoomPosition = 0;
	uint16_t zoomWidth = MAX_16BIT;
	int32_t zoomStart =  0;
	int32_t zoomEnd = MAX_16BIT;
	float zoomValue = 1;
	int32_t zoomResolution;
	uint8_t lastChangedPoint = 0;
};


struct strInterfaceGlobals
{
	char *padNamesPointer[48];

	char intrumentsNames[INSTRUMENTS_COUNT][SAMPLE_NAME_SIZE+4];
	char *ptrIntrumentsNames[INSTRUMENTS_COUNT];

//-----------------------------------------------------
	static const uint8_t fillTypeListCount = 3;
	static const uint8_t fillScaleFilterCount = 2;

	const char fillTypeListLabels[fillTypeListCount][11] =
	{
			"Constant",
			"From-To",
			"Random",
	};

	const char fillScaleFilterLabels[fillScaleFilterCount][5] =
	{
			"Yes",
			"No",
	};

	const char* ptrfillTypeListNames[fillTypeListCount] =
	{
			&fillTypeListLabels[0][0],
			&fillTypeListLabels[1][0],
			&fillTypeListLabels[2][0],
	};

	const char* ptrfillScaleFilterNames[fillScaleFilterCount] =
	{
			&fillScaleFilterLabels[0][0],
			&fillScaleFilterLabels[1][0],
	};

//-----------------------------------------------------
	const char fxNames[FX_COUNT][FX_NAME_SIZE+4] =
	{
			"--- None",
			"A - Nudge",
			"B - pusty",
			"C - Roll",
			"D - Chance",
			"E - Random note",
			"F - Random instrument",
			"G - Random velocity",
			"H - Roll up",
			"I - Roll down",
			"J - Roll random",
			"K - Send CC[1]",
			"L - Send CC[2]",
			"M - 3",
			"N - 4",
			"O - 5",
			"P - 6",
			"Q - 7",
			"R - 8",
			"S - 9",
			"T - 10",
			"U - xyz",
			"V - glide",
			"W - sample start",
			"X - loop start",
			"Y - loop end",
			"Z - slice",
			"Z - reverse playback",
			"Z - filer lowpass",
			"Z - filter highpass",
			"Z - filter bandpass",
			"Z - send reverb",
			"Z - panning",
			"Z - amp attack",
			"Z - amp release",
			"Z - microtuning",
			"Z - tremolo fast",
			"Z - tremolo slow",
			"Z - vibrato fast",
			"Z - vibrato slow",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
	};

	const char *ptrFxNames[FX_COUNT] =
	{
			&fxNames[0][0],
			&fxNames[1][0],
			&fxNames[2][0],
			&fxNames[3][0],
			&fxNames[4][0],
			&fxNames[5][0],
			&fxNames[6][0],
			&fxNames[7][0],
			&fxNames[8][0],
			&fxNames[9][0],
			&fxNames[10][0],
			&fxNames[11][0],
			&fxNames[12][0],
			&fxNames[13][0],
			&fxNames[14][0],
			&fxNames[15][0],
			&fxNames[16][0],
			&fxNames[17][0],
			&fxNames[18][0],
			&fxNames[19][0],
			&fxNames[20][0],
			&fxNames[21][0],
			&fxNames[22][0],
			&fxNames[23][0],
			&fxNames[24][0],
			&fxNames[25][0],
			&fxNames[26][0],
			&fxNames[27][0],
			&fxNames[28][0],
			&fxNames[29][0],
			&fxNames[30][0],
			&fxNames[31][0],
			&fxNames[32][0],
			&fxNames[33][0],
			&fxNames[34][0],
			&fxNames[35][0],
			&fxNames[36][0],
			&fxNames[37][0],
			&fxNames[38][0],
			&fxNames[39][0],
			&fxNames[40][0],
			&fxNames[41][0],
			&fxNames[42][0],
			&fxNames[43][0],
			&fxNames[44][0],
			&fxNames[45][0],
			&fxNames[46][0],
			&fxNames[47][0]
	};

//-----------------------------------------------------


};

typedef void (*editFunct_t)(int16_t);

typedef struct node
{
	uint8_t 		isActive;
	editFunct_t	 	editFunct;

}select_node_t;

extern strInterfaceGlobals interfaceGlobals;







#endif
