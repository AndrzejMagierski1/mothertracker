
#ifndef MTDISPALYDEFS_H
#define MTDISPALYDEFS_H

#include "mtStructs.h"


#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))

// COLORY
#define MT_DISP_BG_COLOR 			DISP_RGB(0,0,0) //DISP_RGB(100,10,100)
#define MT_DISP_F_COLOR				DISP_RGB(255,255,255)

#define MT_DISP_POT_F_COLOR 		DISP_RGB(255,255,255)

#define MT_DISP_BUTTON_F_COLOR 		DISP_RGB(0,0,0)
#define MT_DISP_BUTTON_BG_COLOR 	DISP_RGB(255,255,255)

#define MT_DISP_VALUE_ROUND_COLOR 	DISP_RGB(255,255,255)
#define MT_DISP_VALUE_F_COLOR 		DISP_RGB(255,255,255)

#define MT_DISP_SPECTRUM_COLOR 		DISP_RGB(255,255,255)
#define MT_DISP_POINTS_COLOR 		DISP_RGB(255,255,255)
#define MT_DISP_POINTS_BG_COLOR 	DISP_RGB(45,38,20) //DISP_RGB(170,148,51)


// wilekosci

#define MT_DISP_VALUES_MAX			5
#define MT_DISP_LISTS_MAX			5

//-------------------------------------------------------------------
// ENUMs
enum enumMtDisplayMode
{
	mtDisplayModeBlank,
	mtDisplayModeNormal,
	mtDisplayModePrint,
	mtDisplayModePolyLogo,
};

enum enumMtDisplayBlockType
{
	mtDisplayBlockTypeBlank,
	mtDisplayBlockTypeRound,
	mtDisplayBlockTypeMenu
};

enum enumMtDispValueTypes
{
	mtDispValueValueNone,
	mtDispValueValueNumberOnly,
	mtDispValueValue_0_100,
	mtDispValueValueLeftRight_0_100,
	mtDispValueValueLeftRight_100_100,
	mtDispValueValueLeftRight_24_24,
	mtDispValueMultiValue4Row,
	mtDispValueMultiValue3Row,
};


//-------------------------------------------------------------------
// OPCJE ANIMACJI
#define MT_DISP_ANIMATION_T 	1



// rozmieszczenie elementnow na ekranie

#define MT_DISP_H 	128
#define MT_DISP_W 	480

#define MT_DISP_BLOCK_COUNT 	5
#define MT_DISP_BLOCK_W			(MT_DISP_W)/(MT_DISP_BLOCK_COUNT)

#define MT_DISP_BLOCK_TITLE_Y 	10

#define MT_DISP_BLOCK_LABEL_Y 		118
#define MT_DISP_BLOCK_LABEL_H 		20
#define MT_DISP_BLOCK_LABEL_OFFSET 	3
#define MT_DISP_BLOCK_LABEL_W 		(MT_DISP_BLOCK_W - (2 * MT_DISP_BLOCK_LABEL_OFFSET))

#define MT_DISP_BLOCK_VALUE_CENTER_Y 	60
#define MT_DISP_BLOCK_VALUE_ROUND_SIZE	35
#define MT_DISP_BLOCK_VALUE_ROUND_W	 	3


#define MT_DISP_BLOCK_MENU_TOP_Y 		30
#define MT_DISP_BLOCK_MENU_Y_SPACE	 	16
#define MT_DISP_BLOCK_MENU_OFFSET 		3
#define MT_DISP_BLOCK_MENU_TEXT_OFFSET 	5
#define MT_DISP_BLOCK_MENU_ROWS			5




#define MT_DISP_IEDITOR_SPECTRUM_Y		64





// CZCIONKI
#define MT_GPU_RAM_FONT1_ADRESS	1000
#define MT_GPU_RAM_FONT1_HANDLE	13
#define MT_GPU_RAM_FONT2_ADRESS	11000
#define MT_GPU_RAM_FONT2_HANDLE	14
// handle nie moze byc wikesze niz 14


// OBRAZY
#define MT_GPU_RAM_POLY_LOGO_ADRESS	50000
#define MT_GPU_RAM_POLY_LOGO_HANDLE	12



// rozmiesczenie elementow w pamieci ram_g

#define MT_GPU_RAM_POTS_LABELS_ADDRESS		60000
#define MT_GPU_RAM_POTS_LABELS_SIZE			1000

#define MT_GPU_RAM_BUTTONS_LABELS_ADDRESS	(MT_GPU_RAM_POTS_LABELS_ADDRESS + MT_GPU_RAM_POTS_LABELS_SIZE)
#define MT_GPU_RAM_BUTTONS_LABELS_SIZE		1000

#define MT_GPU_RAM_VALUES_ADRESS			(MT_GPU_RAM_BUTTONS_LABELS_ADDRESS + MT_GPU_RAM_BUTTONS_LABELS_SIZE)
#define MT_GPU_RAM_VALUES_SIZE				500

#define MT_GPU_RAM_LISTS_ADRESS				(MT_GPU_RAM_VALUES_ADRESS+(5*MT_GPU_RAM_VALUES_SIZE))
#define MT_GPU_RAM_LISTS_SIZE				1000

#define MT_GPU_RAM_SPECTRUM_POINTS_ADRESS	(MT_GPU_RAM_LISTS_ADRESS+(5*MT_GPU_RAM_LISTS_SIZE))
#define MT_GPU_RAM_SPECTRUM_POINTS_SIZE		2000

#define MT_GPU_RAM_SPECTRUM_VIEW_ADRESS		(MT_GPU_RAM_SPECTRUM_POINTS_ADRESS+MT_GPU_RAM_SPECTRUM_POINTS_SIZE)
#define MT_GPU_RAM_SPECTRUM_VIEW_SIZE		5000

#define MT_GPU_RAM_ENVELOPE_ADRESS			(MT_GPU_RAM_SPECTRUM_VIEW_ADRESS+MT_GPU_RAM_SPECTRUM_VIEW_SIZE)
#define MT_GPU_RAM_ENVELOPE_SIZE			2000

#define MT_GPU_RAM_MR_VALUES_ADRESS			(MT_GPU_RAM_ENVELOPE_ADRESS+MT_GPU_RAM_ENVELOPE_SIZE)
#define MT_GPU_RAM_MR_VALUES_SIZE			1000

#define MT_GPU_RAM_TRACK_TABLE_ADRESS		(MT_GPU_RAM_MR_VALUES_ADRESS+(5*MT_GPU_RAM_MR_VALUES_SIZE))
#define MT_GPU_RAM_TRACK_TABLE_SIZE			5000

#define MT_GPU_RAM_TEXT_EDIT_ADRESS			(MT_GPU_RAM_TRACK_TABLE_ADRESS+MT_GPU_RAM_TRACK_TABLE_SIZE)
#define MT_GPU_RAM_TEXT_EDIT_SIZE			2000



#define MT_GPU_RAM_TRACK___NEXT___			(MT_GPU_RAM_TEXT_EDIT_ADRESS+MT_GPU_RAM_TEXT_EDIT_SIZE)


//-------------------------------------------------------------------
// STRUKTURY

// ODSWIEZANIE
struct strMtDisplayRefreshTable
{
	uint8_t all;
	uint8_t background;
	uint8_t logo;

	uint8_t potsLabels;
	uint8_t buttonsLabels;

	uint8_t spectrumPoints;
	uint8_t spectrumView;

	uint8_t values[MT_DISP_VALUES_MAX];
	uint8_t multiRowValues[MT_DISP_VALUES_MAX];

	uint8_t lists[MT_DISP_LISTS_MAX];

	uint8_t envelope;

	uint8_t trackTable;

	uint8_t textEdit;
};

//KOLORY
struct strMtDisplayColors
{
	uint32_t bgColor				= MT_DISP_BG_COLOR;
	uint32_t fontPrint				= MT_DISP_F_COLOR;

	uint32_t fontPotLabel 			= MT_DISP_POT_F_COLOR;
	uint32_t fontButtonLabel 		= MT_DISP_BUTTON_F_COLOR;
	uint32_t labelButton 			= MT_DISP_BUTTON_BG_COLOR;

	uint32_t spectrumView			= MT_DISP_SPECTRUM_COLOR;
	uint32_t spectrumPoint			= MT_DISP_POINTS_COLOR;
	uint32_t spectrumPointsBG		= MT_DISP_POINTS_BG_COLOR;

	uint32_t fontValue				= DISP_RGB(255,255,255);
	uint32_t valueBar				= DISP_RGB(130,130,130);

	uint32_t multiRowValueFrame		= DISP_RGB(255,255,255);
	uint32_t multiRowValueFrameBG 	= DISP_RGB(0,0,0);
	uint32_t fontMultiRowValue		= DISP_RGB(255,255,255);

	uint32_t listItemFrame 			= DISP_RGB(255,255,255);
	uint32_t listItemFrameBG 		= DISP_RGB(0,0,0);
	uint32_t listScrollBar 			= DISP_RGB(255,255,255);
	uint32_t listBG 				= DISP_RGB(85,74,25);
	uint32_t fontList 				= DISP_RGB(255,255,255);

	uint32_t envelopeLines 			= DISP_RGB(200,200,200);
	uint32_t envelopeGradTop		= DISP_RGB(90,90,90);//DISP_RGB(200,200,200);
	uint32_t envelopeGradBott		= DISP_RGB(90,90,90);
	uint32_t fontEnvelope 			= DISP_RGB(255,255,255);
	uint32_t envelopeBGGrid 		= DISP_RGB(50,50,50);
	uint32_t envelopeValueBar 		= DISP_RGB(130,130,130);

	uint32_t trackTableFrame		= DISP_RGB(255,255,255);
	uint32_t fontTrackTable			= DISP_RGB(255,255,255);

	uint32_t textEditFrame			= DISP_RGB(255,255,255);
	uint32_t fontTextEdit			= DISP_RGB(255,255,255);
};


struct strMtRamSize
{
	uint32_t potsLabels = 0;
	uint32_t buttonsLabels = 0;

	uint32_t spectrumView = 0;
	uint32_t spectrumPoints = 0;

	uint32_t values[MT_DISP_VALUES_MAX] = {0,0,0,0,0};
	uint32_t lists[MT_DISP_LISTS_MAX] = {0,0,0,0,0};

	uint32_t envelope = 0;

	uint32_t multiRowValues[MT_DISP_VALUES_MAX] = {0,0,0,0,0};

	uint32_t trackTable = 0;

	uint32_t textEdit = 0;
};


struct strMtRamAddres
{
	uint32_t potsLabels = MT_GPU_RAM_POTS_LABELS_ADDRESS;
	uint32_t buttonsLabels = MT_GPU_RAM_BUTTONS_LABELS_ADDRESS;

	uint32_t spectrumView = MT_GPU_RAM_SPECTRUM_VIEW_ADRESS;
	uint32_t spectrumPoints = MT_GPU_RAM_SPECTRUM_POINTS_ADRESS;

	uint32_t values[MT_DISP_VALUES_MAX] =
			{MT_GPU_RAM_VALUES_ADRESS,
			MT_GPU_RAM_VALUES_ADRESS+MT_GPU_RAM_VALUES_SIZE,
			MT_GPU_RAM_VALUES_ADRESS+(MT_GPU_RAM_VALUES_SIZE*2),
			MT_GPU_RAM_VALUES_ADRESS+(MT_GPU_RAM_VALUES_SIZE*3),
			MT_GPU_RAM_VALUES_ADRESS+(MT_GPU_RAM_VALUES_SIZE*4)};

	uint32_t lists[MT_DISP_LISTS_MAX] =
			{MT_GPU_RAM_LISTS_ADRESS,
			MT_GPU_RAM_LISTS_ADRESS+MT_GPU_RAM_LISTS_SIZE,
			MT_GPU_RAM_LISTS_ADRESS+(MT_GPU_RAM_LISTS_SIZE*2),
			MT_GPU_RAM_LISTS_ADRESS+(MT_GPU_RAM_LISTS_SIZE*3),
			MT_GPU_RAM_LISTS_ADRESS+(MT_GPU_RAM_LISTS_SIZE*4)};

	uint32_t envelope = MT_GPU_RAM_ENVELOPE_ADRESS;

	uint32_t multiRowValues[MT_DISP_VALUES_MAX] =
			{MT_GPU_RAM_MR_VALUES_ADRESS,
				MT_GPU_RAM_MR_VALUES_ADRESS+MT_GPU_RAM_MR_VALUES_SIZE,
				MT_GPU_RAM_MR_VALUES_ADRESS+(MT_GPU_RAM_MR_VALUES_SIZE*2),
				MT_GPU_RAM_MR_VALUES_ADRESS+(MT_GPU_RAM_MR_VALUES_SIZE*3),
				MT_GPU_RAM_MR_VALUES_ADRESS+(MT_GPU_RAM_MR_VALUES_SIZE*4)};


	uint32_t trackTable = MT_GPU_RAM_TRACK_TABLE_ADRESS;

	uint32_t textEdit = MT_GPU_RAM_TEXT_EDIT_ADRESS;
};


struct strMtElementsState
{
	uint8_t potsLabels = 0;
	uint8_t buttonsLabels = 0;

	uint8_t spectrumView = 0;
	uint8_t spectrumPoints = 0;

	uint8_t values[MT_DISP_VALUES_MAX] = {0,0,0,0,0};
	uint8_t multiRowValues[MT_DISP_VALUES_MAX] = {0,0,0,0,0};
	uint8_t lists[MT_DISP_LISTS_MAX] = {0,0,0,0,0};

	uint8_t envelope = 0;
	uint8_t trackTable = 0;
	uint32_t textEdit = 0;



	uint8_t waitSpinner = 0;
};

// -------------------------------------------------------------------------

struct strMtDispSpectrum
{
	uint8_t spectrumType = 0;
	int8_t upperData[480];
	int8_t lowerData[480];

	uint8_t pointsType = 0;
	int16_t startPoint;
	int16_t endPoint;
	int16_t loopPoint1;
	int16_t loopPoint2;
};

struct strMtDispValues
{
	uint8_t type[MT_DISP_VALUES_MAX];
	int16_t value1[MT_DISP_VALUES_MAX];
//	int16_t value2[MT_DISP_VALUES_MAX];
//	int16_t value3[MT_DISP_VALUES_MAX];
//	int16_t value4[MT_DISP_VALUES_MAX];
//	int16_t value5[MT_DISP_VALUES_MAX];
};

struct strMtDispMultiRowValues
{
	uint8_t type[MT_DISP_VALUES_MAX];
	char** labels[MT_DISP_VALUES_MAX];
	int16_t values[MT_DISP_VALUES_MAX][5];
};


struct strMtDispEnvelope
{
	uint8_t type;
	char * names[INSTRUMEN_ENVELOPES_MAX];

	uint8_t delay;
	uint8_t attack;
	uint8_t hold;
	uint8_t decay;
	uint8_t	sustain;
	uint8_t release;

	uint8_t amount;
};

struct strMtDispTrackTable
{
	struct strStepParams
	{
		uint8_t mode;
		uint8_t iVal1;
		uint8_t iVal2;
		uint8_t iVal3;
		int8_t  iVal4;  // velocity
	}params[5];

	struct strStepFx
	{
		uint8_t mode;
		char* name;
		char* cVal1;
		char* cVal2;
		uint8_t iVal1;
		uint8_t iVal2;

	}fx1[5],fx2[5],fx3[5],fx4[5];

	uint8_t active[5]; // aktualne edytowana wartosc w kazdej kolumnie (params/fxs)
	uint8_t state[5];  // czy step lezy w sekwencji
};


struct strMtDispTextEdit
{
	uint16_t x;
	uint8_t y;
	uint8_t length;

	uint8_t editPos;

	char* label;
	char* text;

};



struct strMtHaptic
{
	uint16_t time;
	uint8_t amplitude;
	uint8_t effect;
	uint8_t pitch;
};

// -------------------------------------------------------------------------
const char mtDispNotes[128][5] =
{
	"C0",	"C#0",	"D0",	"D#0",	"E0",	"F0",	"F#0",	"G0",	"G#0",	"A0",	"A#0",	"B0",
	"C1",	"C#1",	"D1",	"D#1",	"E1",	"F1",	"F#1",	"G1",	"G#1",	"A1",	"A#1",	"B1",
	"C2",	"C#2",	"D2",	"D#2",	"E2",	"F2",	"F#2",	"G2",	"G#2",	"A2",	"A#2",	"B2",
	"C3",	"C#3",	"D3",	"D#3",	"E3",	"F3",	"F#3",	"G3",	"G#3",	"A3",	"A#3",	"B3",
	"C4",	"C#4",	"D4",	"D#4",	"E4",	"F4",	"F#4",	"G4",	"G#4",	"A4",	"A#4",	"B4",
	"C5",	"C#5",	"D5",	"D#5",	"E5",	"F5",	"F#5",	"G5",	"G#5",	"A5",	"A#5",	"B5",
	"C6",	"C#6",	"D6",	"D#6",	"E6",	"F6",	"F#6",	"G6",	"G#6",	"A6",	"A#6",	"B6",
	"C7",	"C#7",	"D7",	"D#7",	"E7",	"F7",	"F#7",	"G7",	"G#7",	"A7",	"A#7",	"B7",
	"C8",	"C#8",	"D8",	"D#8",	"E8",	"F8",	"F#8",	"G8",	"G#8",	"A8",	"A#8",	"B8",
	"C9",	"C#9",	"D9",	"D#9",	"E9",	"F9",	"F#9",	"G9",	"G#9",	"A9",	"A#9",	"B9",
	"C10",	"C#10",	"D10",	"D#10",	"E10",	"F10",	"F#10",	"G10"
};


// funkcje matematyczne

#define arc_deg ((-1)*3.14159265/180)

const float cos_deg[] =
{
1.00,
1.00,
1.00,
1.00,
1.00,
1.00,
0.99,
0.99,
0.99,
0.99,
0.98,
0.98,
0.98,
0.97,
0.97,
0.97,
0.96,
0.96,
0.95,
0.95,
0.94,
0.93,
0.93,
0.92,
0.91,
0.91,
0.90,
0.89,
0.88,
0.87,
0.87,
0.86,
0.85,
0.84,
0.83,
0.82,
0.81,
0.80,
0.79,
0.78,
0.77,
0.75,
0.74,
0.73,
0.72,
0.71,
0.69,
0.68,
0.67,
0.66,
0.64,
0.63,
0.62,
0.60,
0.59,
0.57,
0.56,
0.54,
0.53,
0.52,
0.50,
0.48,
0.47,
0.45,
0.44,
0.42,
0.41,
0.39,
0.37,
0.36,
0.34,
0.33,
0.31,
0.29,
0.28,
0.26,
0.24,
0.22,
0.21,
0.19,
0.17,
0.16,
0.14,
0.12,
0.10,
0.09,
0.07,
0.05,
0.03,
0.02,
0.00,
};

const float sin_deg[] =
{
0.00,
-0.02,
-0.03,
-0.05,
-0.07,
-0.09,
-0.10,
-0.12,
-0.14,
-0.16,
-0.17,
-0.19,
-0.21,
-0.22,
-0.24,
-0.26,
-0.28,
-0.29,
-0.31,
-0.33,
-0.34,
-0.36,
-0.37,
-0.39,
-0.41,
-0.42,
-0.44,
-0.45,
-0.47,
-0.48,
-0.50,
-0.52,
-0.53,
-0.54,
-0.56,
-0.57,
-0.59,
-0.60,
-0.62,
-0.63,
-0.64,
-0.66,
-0.67,
-0.68,
-0.69,
-0.71,
-0.72,
-0.73,
-0.74,
-0.75,
-0.77,
-0.78,
-0.79,
-0.80,
-0.81,
-0.82,
-0.83,
-0.84,
-0.85,
-0.86,
-0.87,
-0.87,
-0.88,
-0.89,
-0.90,
-0.91,
-0.91,
-0.92,
-0.93,
-0.93,
-0.94,
-0.95,
-0.95,
-0.96,
-0.96,
-0.97,
-0.97,
-0.97,
-0.98,
-0.98,
-0.98,
-0.99,
-0.99,
-0.99,
-0.99,
-1.00,
-1.00,
-1.00,
-1.00,
-1.00,
-1.00,
};



const uint8_t adsr_log[101] =
{
0  ,
5  ,
9  ,
13 ,
17 ,
20 ,
23 ,
26 ,
28 ,
31 ,
33 ,
35 ,
37 ,
39 ,
41 ,
42 ,
44 ,
46 ,
47 ,
49 ,
50 ,
51 ,
53 ,
54 ,
55 ,
56 ,
57 ,
58 ,
60 ,
61 ,
62 ,
63 ,
64 ,
64 ,
65 ,
66 ,
67 ,
68 ,
69 ,
70 ,
70 ,
71 ,
72 ,
73 ,
73 ,
74 ,
75 ,
76 ,
76 ,
77 ,
78 ,
78 ,
79 ,
79 ,
80 ,
81 ,
81 ,
82 ,
82 ,
83 ,
84 ,
84 ,
85 ,
85 ,
86 ,
86 ,
87 ,
87 ,
88 ,
88 ,
89 ,
89 ,
90 ,
90 ,
91 ,
91 ,
91 ,
92 ,
92 ,
93 ,
93 ,
94 ,
94 ,
94 ,
95 ,
95 ,
96 ,
96 ,
96 ,
97 ,
97 ,
97 ,
98 ,
98 ,
98 ,
99 ,
99 ,
99 ,
99 ,
99 ,
100,
};


#endif


