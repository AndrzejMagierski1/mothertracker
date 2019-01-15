
#ifndef MTDISPALYDEFS_H
#define MTDISPALYDEFS_H

#include "mtStructs.h"


#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))

// COLORY
#define MT_DISP_BG_COLOR 			DISP_RGB(0,0,0) //DISP_RGB(100,10,100)

#define MT_DISP_TITLE_F_COLOR 		DISP_RGB(255,255,255)

#define MT_DISP_LABEL_F_COLOR 		DISP_RGB(0,0,0)
#define MT_DISP_LABEL_BG_COLOR 		DISP_RGB(255,255,255)

#define MT_DISP_VALUE_ROUND_COLOR 	DISP_RGB(255,255,255)
#define MT_DISP_VALUE_F_COLOR 		DISP_RGB(255,255,255)




//-------------------------------------------------------------------
// ENUMs
enum enumMtDisplayMode
{
	mtDisplayModeBlank,
	mtDisplayModeLogo,
	mtDisplayModeBlocks5,
	mtDisplayModeFileManager,
	mtDisplayModeInstrumentEditor,
	mtDisplayModePrint,
	mtDisplayModePolyLogo,
};

enum enumMtDisplayBlockType
{
	mtDisplayBlockTypeBlank,
	mtDisplayBlockTypeRound,
	mtDisplayBlockTypeMenu
};

//-------------------------------------------------------------------
// STRUKTURY

// ODSWIEZANIE
struct strMtDisplayRefreshTable
{
	uint8_t all;
	uint8_t background;
	uint8_t logo;

	struct strBlockRefresh
	{
		uint8_t title;
		uint8_t label;
		uint8_t center;
	} block[5];

	struct strFmanager
	{
		uint8_t rootTitle;
		uint8_t mainTitle;
		uint8_t rootList;
		uint8_t mainList;
		uint8_t labels;
	} fManager;

	struct strInstrumentEditor
	{
		uint8_t points;
		uint8_t spectrum;
		uint8_t labels;
	} instrumentEditor;




};



struct strMtDisplayBlock
{
	uint8_t type;

	char title[20] = {0};
	char label[20] = {0};

	char * menu;
	uint16_t value   = 0;
	uint16_t max;
	uint16_t start;
	int8_t state;
	int8_t animationStep = 0;

	uint32_t fontTitleColor = MT_DISP_TITLE_F_COLOR;
	uint32_t fontLabelColor = MT_DISP_LABEL_F_COLOR;
	uint32_t labelColor 	= MT_DISP_LABEL_BG_COLOR;
	uint32_t valueColor 	= MT_DISP_VALUE_ROUND_COLOR;
	uint32_t fontValueColor = MT_DISP_VALUE_F_COLOR;

	uint32_t ramTitleSize = 0;
	uint32_t ramLabelSize = 0;
	uint32_t ramCenterSize = 0;
};



struct strMtFmanager
{
	char rootTitle[100];
	char mainTitle[60];

	char * rootList; // 40 liter max na linie
	char * mainList;

	uint16_t rootPosition;
	uint16_t rootCount;
	uint16_t rootStart;
	int8_t rootState;
	uint8_t rootListFilenameLength;
	uint8_t rootListEnable;

	uint16_t mainPosition;
	uint16_t mainCount;
	uint16_t mainStart;
	int8_t mainState;
	uint8_t mainListFilenameLength;
	uint8_t mainListEnable;

	char * labels; //wielksoc 20 liter max na jeden label

	int8_t animationStepRoot = 0;
	int8_t animationStepMain = 0;

	uint32_t fontTitleColor = MT_DISP_TITLE_F_COLOR;
	uint32_t fontLabelColor = MT_DISP_LABEL_F_COLOR;
	uint32_t labelColor 	= MT_DISP_LABEL_BG_COLOR;
	uint32_t listColor 		= MT_DISP_TITLE_F_COLOR;

	uint32_t ramRootTitleSize = 0;
	uint32_t ramMainTitleSize = 0;
	uint32_t ramLabelsSize = 0;
	uint32_t ramRootListSize = 0;
	uint32_t ramMainListSize = 0;
};


struct strMtInstrumentEditor
{
	char * buttonsLabels; //wielksoc 20 liter max na jeden label
	char ** potsLabels; //wielksoc 20 liter max na jeden label

	strSpectrum *spectrum;

	int8_t animationStep = 0;

	uint32_t fontTitleColor = MT_DISP_TITLE_F_COLOR;
	uint32_t fontLabelColor = MT_DISP_LABEL_F_COLOR;
	uint32_t labelColor 	= MT_DISP_LABEL_BG_COLOR;
	uint32_t listColor 		= MT_DISP_TITLE_F_COLOR;
	uint32_t pointsColor 		= DISP_RGB(255,255,255);
	uint32_t spectrumColor 		= DISP_RGB(255,255,255);

	uint32_t ramPointsSize = 0;
	uint32_t ramLabelsSize = 0;
	uint32_t ramSpectrumSize = 0;
};



struct strMtHaptic
{
	uint16_t time;
	uint8_t amplitude;
	uint8_t effect;
	uint8_t pitch;
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

//#define MT_DISP_BLOCK_VALUE_CENTER_X	(MT_DISP_BLOCK_W/2)
#define MT_DISP_BLOCK_VALUE_CENTER_Y 	60
#define MT_DISP_BLOCK_VALUE_ROUND_SIZE	35
#define MT_DISP_BLOCK_VALUE_ROUND_W	 	3


//#define MT_DISP_BLOCK_MENU_LEFT_X 		(MT_DISP_BLOCK_W/2)
#define MT_DISP_BLOCK_MENU_TOP_Y 		30
//#define MT_DISP_BLOCK_MENU_W	 		90
#define MT_DISP_BLOCK_MENU_Y_SPACE	 	16
#define MT_DISP_BLOCK_MENU_OFFSET 		3
#define MT_DISP_BLOCK_MENU_TEXT_OFFSET 	5
#define MT_DISP_BLOCK_MENU_ROWS			5


#define MT_DISP_FMANAGER_ROOTTITLE_X	8
#define MT_DISP_FMANAGER_MAINTITLE_X	((MT_DISP_BLOCK_W * 2) + 8)
#define MT_DISP_FMANAGER_TITLE_Y		10


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

#define MT_GPU_RAM_BLOCK_TITLES_ADRESS	60000
#define MT_GPU_RAM_BLOCK_TITLES_SIZE	200

#define MT_GPU_RAM_BLOCK_LABELS_ADRESS	(MT_GPU_RAM_BLOCK_TITLES_ADRESS+(5*MT_GPU_RAM_BLOCK_TITLES_SIZE))
#define MT_GPU_RAM_BLOCK_LABELS_SIZE	200

#define MT_GPU_RAM_BLOCK_CENTERS_ADRESS	(MT_GPU_RAM_BLOCK_LABELS_ADRESS+(5*MT_GPU_RAM_BLOCK_LABELS_SIZE))
#define MT_GPU_RAM_BLOCK_CENTERS_SIZE	1000

#define MT_GPU_RAM_BLOCK_VALUES_END_ADRESS	(MT_GPU_RAM_BLOCK_CENTERS_ADRESS+(5*MT_GPU_RAM_BLOCK_CENTERS_SIZE))




#define MT_GPU_RAM_FMANAGER_ROOTTITLE_ADRESS	MT_GPU_RAM_BLOCK_VALUES_END_ADRESS
#define MT_GPU_RAM_FMANAGER_ROOTTITLE_SIZE		500

#define MT_GPU_RAM_FMANAGER_LABELS_ADRESS		(MT_GPU_RAM_FMANAGER_ROOTTITLE_ADRESS+MT_GPU_RAM_FMANAGER_ROOTTITLE_SIZE)
#define MT_GPU_RAM_FMANAGER_LABELS_SIZE			1000

#define MT_GPU_RAM_FMANAGER_ROOTLIST_ADRESS		(MT_GPU_RAM_FMANAGER_LABELS_ADRESS+MT_GPU_RAM_FMANAGER_LABELS_SIZE)
#define MT_GPU_RAM_FMANAGER_ROOTLIST_SIZE		2000

#define MT_GPU_RAM_FMANAGER_END_ADRESS			(MT_GPU_RAM_FMANAGER_ROOTLIST_ADRESS+MT_GPU_RAM_FMANAGER_ROOTLIST_SIZE)



#define MT_GPU_RAM_INSTRUMENT_EDITOR_POINTS_ADRESS		MT_GPU_RAM_FMANAGER_END_ADRESS
#define MT_GPU_RAM_INSTRUMENT_EDITOR_POINTS_SIZE		1000

#define MT_GPU_RAM_INSTRUMENT_EDITOR_LABELS_ADRESS		(MT_GPU_RAM_INSTRUMENT_EDITOR_POINTS_ADRESS+MT_GPU_RAM_INSTRUMENT_EDITOR_POINTS_SIZE)
#define MT_GPU_RAM_INSTRUMENT_EDITOR_LABELS_SIZE		1000

#define MT_GPU_RAM_INSTRUMENT_EDITOR_SPECTRUM_ADRESS	(MT_GPU_RAM_INSTRUMENT_EDITOR_LABELS_ADRESS+MT_GPU_RAM_INSTRUMENT_EDITOR_LABELS_SIZE)
#define MT_GPU_RAM_INSTRUMENT_EDITOR_SPECTRUM_SIZE		2000

#define MT_GPU_RAM_INSTRUMENT_EDITOR_END_ADRESS			(MT_GPU_RAM_INSTRUMENT_EDITOR_SPECTRUM_ADRESS+MT_GPU_RAM_INSTRUMENT_EDITOR_SPECTRUM_SIZE)




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


