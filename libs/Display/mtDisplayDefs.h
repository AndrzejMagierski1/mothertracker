
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
	mtDispValueValue1,
	mtDispValueValue2,
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
#define MT_GPU_RAM_SPECTRUM_POINTS_SIZE		1000

#define MT_GPU_RAM_SPECTRUM_VIEW_ADRESS		(MT_GPU_RAM_SPECTRUM_POINTS_ADRESS+MT_GPU_RAM_SPECTRUM_POINTS_SIZE)
#define MT_GPU_RAM_SPECTRUM_VIEW_SIZE		1000

#define MT_GPU_RAM_ENVELOPE_ADRESS			(MT_GPU_RAM_SPECTRUM_VIEW_ADRESS+MT_GPU_RAM_SPECTRUM_VIEW_SIZE)
#define MT_GPU_RAM_ENVELOPE_SIZE			1000




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
	uint8_t lists[MT_DISP_LISTS_MAX];

	uint8_t envelope;
};

//KOLORY
struct strMtDisplayColors
{
	uint32_t bgColor			= MT_DISP_BG_COLOR;
	uint32_t fontPrint			= MT_DISP_F_COLOR;

	uint32_t fontPotLabel 		= MT_DISP_POT_F_COLOR;
	uint32_t fontButtonLabel 	= MT_DISP_BUTTON_F_COLOR;
	uint32_t labelButton 		= MT_DISP_BUTTON_BG_COLOR;

	uint32_t spectrumView		= MT_DISP_SPECTRUM_COLOR;
	uint32_t spectrumPoint		= MT_DISP_POINTS_COLOR;
	uint32_t spectrumPointsBG	= MT_DISP_POINTS_BG_COLOR;

	uint32_t fontValue			= MT_DISP_VALUE_F_COLOR;
	uint32_t valueBar			= DISP_RGB(130,130,130);

	uint32_t listItemFrame 		= DISP_RGB(255,255,255);
	uint32_t listItemFrameBG 	= DISP_RGB(0,0,0);
	uint32_t listScrollBar 		= DISP_RGB(255,255,255);
	uint32_t listBG 			= DISP_RGB(85,74,25);
	uint32_t fontList 			= DISP_RGB(255,255,255);


	uint32_t envelopeLines 		= DISP_RGB(255,255,255);
	uint32_t envelopeBGLines	= DISP_RGB(85,74,25);
	uint32_t fontEnvelope 		= DISP_RGB(255,255,255);
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
};


struct strMtElementsState
{
	uint8_t potsLabels = 0;
	uint8_t buttonsLabels = 0;

	uint8_t spectrumView = 0;
	uint8_t spectrumPoints = 0;

	uint8_t values[MT_DISP_VALUES_MAX] = {0,0,0,0,0};
	uint8_t lists[MT_DISP_LISTS_MAX] = {0,0,0,0,0};

	uint8_t envelope = 0;
};

// -------------------------------------------------------------------------

struct strMtDispSpectrum
{
	uint8_t spectrumType = 0;
	int8_t upperData[480];
	int8_t lowerData[480];

	uint8_t pointsType = 0;
	uint16_t startPoint;
	uint16_t endPoint;
	uint16_t loopPoint1;
	uint16_t loopPoint2;
};

struct strMtDispValues
{
	uint8_t type[MT_DISP_VALUES_MAX];
	int16_t value[MT_DISP_VALUES_MAX];
};

struct strMtDispEnvelope
{
	uint8_t type;

	uint8_t delay;
	uint8_t attack;
	uint8_t hold;
	uint8_t decay;
	uint8_t	sustain;
	uint8_t release;

	uint8_t amount;
};

struct strMtHaptic
{
	uint16_t time;
	uint8_t amplitude;
	uint8_t effect;
	uint8_t pitch;
};

// -------------------------------------------------------------------------

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


#endif


