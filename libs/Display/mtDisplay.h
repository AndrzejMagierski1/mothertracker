

#ifndef MTDISPLAY_H
#define MTDISPLAY_H

#include "FT812.h"
#include "mtDisplayDefs.h"
#include "elapsedMillis.h"



class cMtDisplayList
{
public:
	void setSeflRefreshPtr(uint8_t *refresh, uint8_t *animate);
	void setList(uint8_t block, uint8_t blockWidth, uint16_t start, char ** list, uint16_t count);
	void setListPos(uint16_t position);
	void update();
	uint8_t enabled() { return listEnable; }

private:
	char ** listTable;

	uint8_t  listBlock;
	uint8_t  listBlockWidth;
	uint16_t listPosition;
	uint16_t listCount;
	uint16_t listStart;
	int8_t 	 listState;
//	uint8_t  listRowLength;
	uint8_t  listEnable;
	int8_t   listAnimationStep = 0;

	uint32_t listColor 		= MT_DISP_TITLE_F_COLOR;

	uint8_t *selfAnimate   = 0;
	uint8_t *selfRefresh   = 0;
};


class cMtHaptic
{
public:
	void enable();
	void disable();
	void set(uint16_t time, uint8_t amplitude, uint8_t effect, uint8_t pitch);
	void start(uint16_t time);
	void start(uint16_t time, uint8_t amplitude);
	void start(uint16_t time, uint8_t amplitude, uint8_t effect, uint8_t pitch);
	void stop();

	elapsedMillis timer;
	strMtHaptic params;
	uint8_t active = 0;
	uint8_t enabled = 0;
};


class cMtDisplay
{
public:
	void begin(uint8_t mode);
	void updateDisplay();

	//metody publiczne zarzadzania ekranem
	void setMode(uint8_t mode);

	void setBlockType (uint8_t number, uint32_t type, uint16_t start, char * menu, uint16_t max);
	void setBlockTitle(uint8_t number, char text[]);
	void setBlockLabel(uint8_t number, char text[]);
	void setBlockCenter(uint8_t number, int32_t value);

	void setFmanagerRootTitle(char text[]);
	void setFmanagerRootList(uint16_t start, char ** list, uint16_t count, uint8_t filename_length);
	void setFmanagerRootListPos(uint16_t position);
	void setFmanagerLabels(char * labels);

	void setInstrumentEditorPoints(strSpectrum *spectrum);
	void setInstrumentEditorSpectrum(strSpectrum *spectrum);
	void setInstrumentEditorPotsLabels(char ** labels);
	void setInstrumentEditorButtonsLabels(char ** labels);
	void setInstrumentEditorSampleList(uint16_t start, char ** list, uint16_t count);
	void setInstrumentEditorSampleListPos(uint16_t position);

	// print
	void print(const char * s);
	void print(int n);
	void print(unsigned int n);
	void printNumber(unsigned long n, uint8_t base, uint8_t sign);
	void println(const char * s);
	void println(int n);
	void println();
	void printClear();
	void printShow();

private:
	// metody glowne trybow pracy
	void dl_load_blank_main();
	void dl_load_block_main();
	void dl_load_fmanager_main();
	void dl_load_print_main();
	void dl_load_poly_logo_main();
	void dl_load_instrument_editor_main();

	// metody poszczegolnych trybow
	void ramg_blocks_title(uint8_t block);
	void ramg_blocks_label(uint8_t block);
	void ramg_blocks_center(uint8_t block);
	void ramg_blocks_blank(uint8_t block);
	void ramg_blocks_value(uint8_t block);
	void ramg_blocks_menu(uint8_t block);

	void ramg_fmanager_roottitle();
	void ramg_fmanager_maintitle();
	void ramg_fmanager_labels();
	void ramg_fmanager_rootlist();
	void ramg_fmanager_mainlist();

	void ramg_instrument_editor_points();
	void ramg_instrument_editor_spectrum();
	void ramg_instrument_editor_labels();
	void ramg_instrument_editor_background();

	// zmienne glowne
	uint8_t screenMode = mtDisplayModeBlank;
	uint8_t screenRefresh = 0;
	strMtDisplayRefreshTable displayRefreshTable;
	uint32_t  displayBgColor = MT_DISP_BG_COLOR;

	//---------------------------------------------
	// tryb blokowy
	strMtDisplayBlock displayBlock[5];

	//---------------------------------------------
	// tryb menadzera plikow
	strMtFmanager displayFmanager;
	cMtDisplayList fManagerList;

	//---------------------------------------------
	// tryb edytora instrumentu
	strMtInstrumentEditor instrumentEditor;
	cMtDisplayList instrumentEditorSampleList;

	// animowane przejscia
	uint8_t screenAnimation = 0;
	elapsedMicros animationTimer;
	uint8_t updateStep = 0;

	uint32_t updateAdress = 0;
	uint32_t *updateSize;

	struct strTouch
	{
		int16_t x;
		int16_t y;

	} touchPanel[5];

	//print
	#ifdef DEBUG

	#define MAX_PRINT_LINES 6
	#define PRINT_SHOW_TIME 1500
	char text[MAX_PRINT_LINES][256];
	uint8_t	lastPrintLine = 0;
	uint8_t	firstPrintLine = 0;
	uint8_t displayPrint = 0;
	uint8_t totalLinesPrint = 0;
	elapsedMillis printTimer;
	uint8_t	modeSave = 0;

	#endif

};

extern cMtDisplay mtDisplay;
extern cMtHaptic mtHaptic;

#endif
