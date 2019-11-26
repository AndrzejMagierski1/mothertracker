
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H



#include "mtStructs.h"
#include "mtSequencer.h"


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

	//interfaceButton33,  // encoder switch

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
	interfaceButtonVol 			= interfaceButton20, // <<
	interfaceButtonFx 			= interfaceButton21, // <<
	interfaceButtonPattern 		= interfaceButton22,

	interfaceButtonFx1 			= interfaceButton21,
	interfaceButtonFx2 			= interfaceButton20,

	//	interfaceButtonEncoder 		= interfaceButton33,
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

	eventActivateImageViewer,
	eventActivateGameModule,


	eventActivateTestingProcedure,

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

	char intrumentsNames[INSTRUMENTS_COUNT+16][SAMPLE_NAME_SIZE+4];
	char *ptrIntrumentsNames[INSTRUMENTS_COUNT+16];

//-----------------------------------------------------


//-----------------------------------------------------
	uint8_t fxIdToName(uint8_t id)
	{
//		uint8_t count;
		for (uint8_t count = 0; count < FX_COUNT; count++)
		{
			if (fxIDs[count] == id) return count;
		}

		return 0;
	}
	uint8_t fxNameToId(uint8_t name)
	{
		return fxIDs[name];
	}



	const uint8_t fxIDs[FX_COUNT] = {
			Sequencer::strFxConsts::enFxType::FX_TYPE_NONE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_OFF,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_VALUE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_NOTE_UP,
			Sequencer::strFxConsts::enFxType::FX_TYPE_NUDGE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_VELOCITY,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL,
			Sequencer::strFxConsts::enFxType::FX_TYPE_STEP_CHANCE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_NOTE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_INSTRUMENT,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_VELOCITY,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_NOTE_DOWN,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_NOTE_RANDOM,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_1,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_2,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_3,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_4,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_5,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_6,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_7,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_8,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_9,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_10,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_VOL_UP,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_VOL_DOWN,
			Sequencer::strFxConsts::enFxType::FX_TYPE_TEMPO,
			Sequencer::strFxConsts::enFxType::FX_TYPE_NOT_SEQ_FX,//////////
			Sequencer::strFxConsts::enFxType::FX_TYPE_GLIDE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SAMPLE_START,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SAMPLE_LOOP_START,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SAMPLE_LOOP_END,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SAMPLE_SLICE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_REVERSE_PLAYBACK,
			Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_LOWPASS,
			Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_HIGHPASS,
			Sequencer::strFxConsts::enFxType::FX_TYPE_FILTER_BANDPASS,
			Sequencer::strFxConsts::enFxType::FX_TYPE_REVERB_SEND,
			Sequencer::strFxConsts::enFxType::FX_TYPE_PANNING,
			Sequencer::strFxConsts::enFxType::FX_TYPE_AMP_ATTACK,
			Sequencer::strFxConsts::enFxType::FX_TYPE_AMP_RELEASE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_MICROTUNING,
			Sequencer::strFxConsts::enFxType::FX_TYPE_TREMOLO_FAST,
			Sequencer::strFxConsts::enFxType::FX_TYPE_TREMOLO_SLOW,
			Sequencer::strFxConsts::enFxType::FX_TYPE_VIBRATO_FAST,
			Sequencer::strFxConsts::enFxType::FX_TYPE_VIBRATO_SLOW,
			Sequencer::strFxConsts::enFxType::FX_TYPE_FADE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_CUT,
	};

	const char rndFxName[FX_NAME_SIZE+4] = "Random Fx";

	const char fxNames[FX_COUNT][FX_NAME_SIZE+4] =
	{
			"--- None",
			"! - OFF",
			"r - random next value",
			"H - Roll up",
			"A - Nudge",
			"B - Velocity",
			"C - Roll",
			"D - Chance",
			"E - Random note",
			"F - Random instrument",
			"G - Random velocity",
			"I - Roll down",
			"J - Roll random",
			"K - Send CC[1]",
			"L - Send CC[2]",
			"M - Send CC[3]",
			"N - Send CC[4]",
			"O - Send CC[5]",
			"P - Send CC[6]",
			"Q - Send CC[7]",
			"R - Send CC[8]",
			"S - Send CC[9]",
			"T - Send CC[10]",
			"p - ",
			"q - ",
			"s - tempo",
			"U - xyz",
			"V - glide",
			"W - sample start",
			"X - loop start",
			"Y - loop end",
			"Z - slice",
			"a - reverse playback",
			"b - filer lowpass",
			"c - filter highpass",
			"d - filter bandpass",
			"e - send reverb",
			"f - panning",
			"g - amp attack",
			"h - amp release",
			"i - microtuning",
			"j - tremolo fast",
			"k - tremolo slow",
			"l - vibrato fast",
			"m - vibrato slow",
			"n - fade",
			"o - cut",
			"t - ",
	};

	const char *ptrAllFxNames[FX_COUNT+1] =
	{
			rndFxName,
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

	const char **ptrFxNames = &ptrAllFxNames[1];

	uint32_t activeLabelsColors[4] =
	{
		0x999999, // tekst
		0x323132, // tło
		0xFF0000, // ramka
		0xFFFFFF, // tekst2
	};

	uint32_t inactiveLabelsColors[4] =
	{
		0x555555, // tekst
		0x323132, // tło //0x555555
		0xFF0000, // ramka
		0x555555, // tekst2
	};

	uint32_t disabledLabelsColors[4]=
	{
			0x000000, // tekst
			0x222222, // tło
			0xFF0000, // ramka
			0x000000, // tekst2
	};

	uint32_t activeBgLabelsColors[3] =
	{
		0x444344, // linie
		0x2b2a2b,
		0xFF0000, // ramka
	};


//-----------------------------------------------------


};

typedef void (*editFunct_t)(int16_t);
typedef uint8_t (*editFunct1_t)(int16_t);

typedef struct node
{
	uint8_t 		isActive;
	editFunct_t	 	editFunct;

}select_node_t;

typedef struct node1
{
	uint8_t 		isActive;
	editFunct1_t	editFunct;

}select_node1_t;

extern strInterfaceGlobals interfaceGlobals;







#endif
