
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

	interfaceButtonPlay 		= interfaceButton23,
	interfaceButtonRec 			= interfaceButton24,
	interfaceButtonCopy 		= interfaceButton25,
	interfaceButtonUp			= interfaceButton26,
	interfaceButtonPaste 		= interfaceButton27,

	interfaceButtonShift 		= interfaceButton28,
	interfaceButtonEnter 		= interfaceButton29,
	interfaceButtonLeft 		= interfaceButton30,
	interfaceButtonDown 		= interfaceButton31,
	interfaceButtonRight 		= interfaceButton32,

	interfaceButtonEncoder 		= interfaceButton33,


	interfaceButtonParams 		= interfaceButton8,
	interfaceButtonEnvelopes 	= interfaceButton9,
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



enum interfaceEvents
{
	eventNone,
	eventLoadLastProjFailed,
	eventEventPadPress,
	eventLoadSampleBank,

	eventSwitchModule,
	eventSwitchToPreviousModule,
	eventSwitchModuleWithOptions,

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
			"A - Aaaaaaaaaaaaaaaaaaaaa",
			"B - Bbbbbbbbbbbbbbbbbbbbb",
			"C - Ccccccccccccccccccccc",
			"D - Ddddddddddddddddddddd",
			"E - Eeeeeeeeeeeeeeeeeeeee",
			"F - Fffffffffffffffffffff",
			"G - Ggggggggggggggggggggg",
			"H - Hhhhhhhhhhhhhhhhhhhhh",
			"I - Iiiiiiiiiiiiiiiiiiiii",
			"J - Jjjjjjjjjjjjjjjjjjjjj",
			"K - Kkkkkkkkkkkkkkkkkkkkk",
			"L - Lllllllllllllllllllll",
			"M - Mmmmmmmmmmmmmmmmmmmmm",
			"N - Nnnnnnnnnnnnnnnnnnnnn",
			"O - Ooooooooooooooooooooo",
			"P - Ppppppppppppppppppppp",
			"Q - Qqqqqqqqqqqqqqqqqqqqq",
			"R - Rrrrrrrrrrrrrrrrrrrrr",
			"S - Sssssssssssssssssssss",
			"T - Ttttttttttttttttttttt",
			"U - Uuuuuuuuuuuuuuuuuuuuu",
			"V - Vvvvvvvvvvvvvvvvvvvvv",
			"W - Wwwwwwwwwwwwwwwwwwwww",
			"X - Xxxxxxxxxxxxxxxxxxxxx",
			"Y - Yyyyyyyyyyyyyyyyyyyyy",
			"Z - Zzzzzzzzzzzzzzzzzzzzz",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
			"Z - ",
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

extern strInterfaceGlobals interfaceGlobals;







#endif
