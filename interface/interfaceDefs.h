
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
	interfaceButtonVol 			= interfaceButton20, // <<
	interfaceButtonFx 			= interfaceButton21, // <<
	interfaceButtonPattern 		= interfaceButton22,

	interfaceButtonFx1 			= interfaceButton21,
	interfaceButtonFx2 			= interfaceButton20,
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

	char intrumentsNames[INSTRUMENTS_COUNT+16][SAMPLE_NAME_SIZE+4];
	char *ptrIntrumentsNames[INSTRUMENTS_COUNT+16];

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
	uint8_t fxIdToName(uint8_t name)
	{
//		uint8_t count;
		for (uint8_t count = 0; count < FX_COUNT; count++)
		{
			if (fxIDs[count] == name) return count;
		}

		return 0;
	}
	const uint8_t fxIDs[FX_COUNT] = {
			Sequencer::strFxConsts::enFxType::FX_TYPE_NONE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_1,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_2,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_3,
			Sequencer::strFxConsts::enFxType::FX_TYPE_NUDGE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_VELOCITY,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL,
			Sequencer::strFxConsts::enFxType::FX_TYPE_STEP_CHANCE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_NOTE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_INSTRUMENT,
			Sequencer::strFxConsts::enFxType::FX_TYPE_RANDOM_VELOCITY,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_NOTE_UP,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_NOTE_DOWN,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_NOTE_RANDOM,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_4,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_5,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_6,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_7,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_8,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_9,
			Sequencer::strFxConsts::enFxType::FX_TYPE_SEND_CC_10,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_VOL_UP,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_VOL_DOWN,
			Sequencer::strFxConsts::enFxType::FX_TYPE_ROLL_VOL_RANDOM,

			Sequencer::strFxConsts::enFxType::FX_TYPE_NOT_SEQ_FX,
			Sequencer::strFxConsts::enFxType::FX_TYPE_GLIDE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_FADE,
			Sequencer::strFxConsts::enFxType::FX_TYPE_CUT,
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
	};
	const char fxNames[FX_COUNT][FX_NAME_SIZE+4] =
	{
			"--- None",
			"K - Send CC[1]",
			"L - Send CC[2]",
			"M - Send CC[3]",
			"A - Nudge",
			"B - Velocity",
			"C - Roll",
			"D - Chance",
			"E - Random note",
			"F - Random instrument",
			"G - Random velocity",
			"H - Roll up",
			"I - Roll down",
			"J - Roll random",
			"N - Send CC[4]",
			"O - Send CC[5]",
			"P - Send CC[6]",
			"Q - Send CC[7]",
			"R - Send CC[8]",
			"S - Send CC[9]",
			"T - Send CC[10]",
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
			"n - ",
			"o - ",
			"p - ",
			"q - ",
			"r - ",
			"s - ",
			"t - ",
			"u - ",
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
