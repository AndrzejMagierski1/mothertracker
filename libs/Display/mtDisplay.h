

#ifndef MTDISPLAY_H
#define MTDISPLAY_H

#include "FT812.h"
#include "mtDisplayDefs.h"
#include "elapsedMillis.h"



class cMtDisplayList
{
public:
	void setSeflRefreshPtr(uint8_t *refresh, uint8_t *animate);
	void setList(uint8_t block, uint8_t width, uint16_t start, char ** list, uint16_t count, strMtDisplayColors * colors);
	void setListPos(uint16_t position);
	void update();

private:
	char ** listTable;
	strMtDisplayColors * ptrColors;

	uint16_t listPosition;
	uint16_t listCount;
	uint16_t listStart;
	int8_t 	 listState;
	uint8_t  listRowLength;
	uint8_t  listBlockWidth;
	uint8_t  listBlock;
	int8_t   listAnimationStep = 0;

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
	void updateHaptic();

	//metody publiczne zarzadzania ekranem
	void setMode(uint8_t mode);

	void setButtonsLabels(uint8_t state);
	void setPotsLabels(uint8_t state);
	void setSpectrum(uint8_t state);
	void setSpectrumPoints(uint8_t state);
	void setList(uint8_t index, uint8_t block, uint8_t width, uint16_t start, char ** list, uint16_t count);
	void setValue(uint8_t state);
	void setMultiRowValue(uint8_t state);
	void setEnvelopes(uint8_t state);
	void setTrackTable(uint8_t state);
	void setTextEdit(uint8_t x, uint8_t y, uint8_t length, char* text, char* label);

	void changeButtonsLabels(char ** labels);
	void changePotsLabels(char ** labels);
	void changeSpectrum(strMtDispSpectrum *spectrum);
	void changeSpectrumPoints(strMtDispSpectrum *spectrum);
	void changeList(uint8_t index, uint16_t position);
	void changeValues(strMtDispValues * values);
	void changeMultiRowValues(strMtDispMultiRowValues * values);
	void changeEnvelopes(strMtDispEnvelope * envelope);
	void changeTrackTable(strMtDispTrackTable * trackTable);
	void changeTextEdit(char* text, uint8_t pos);

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
	// metody łądujące okreslony ekran z ramu
	void dl_load_blank_main();
	void dl_load_normal_main();
	void dl_load_poly_logo_main();
	void dl_load_print_main();

	void normalModeDisplayRefresh();

	// metody łądujące elementy do ramu wyswietlacza
	void ramg_spectrum_points();
	void ramg_spectrum_view();
	void ramg_pots_labels();
	void ramg_buttons_labels();
	void ramg_values(uint8_t index);
	void ramg_multi_row_values(uint8_t index);
	void ramg_lists(uint8_t index);
	void ramg_envelope();
	void ramg_track_table();
	void ramg_text_edit();

	// zmienne glowne
	uint8_t screenMode = mtDisplayModeBlank;
	uint8_t screenRefresh = 0;
	strMtDisplayRefreshTable displayRefreshTable;
	uint8_t valuesDisplayMode = 0;

	strMtDisplayColors 	displayColors;
	strMtRamSize		ramSize;
	strMtRamAddres		ramAddress;
	strMtElementsState	elementsState;



	//---------------------------------------------
	// pots labels
	char ** ptrPotsLabels;

	//---------------------------------------------
	// pots buttons
	char ** ptrButtonsLabels;

	//---------------------------------------------
	// spectrum / points
	strMtDispSpectrum * ptrSpectrum;

	//---------------------------------------------
	// values
	strMtDispValues * ptrValues;
	strMtDispValues lastValues;

	//---------------------------------------------
	// multi row values
	strMtDispMultiRowValues * ptrMultiRowValues;
	strMtDispMultiRowValues lastMultiRowValues;

	//---------------------------------------------
	// lists
	cMtDisplayList lists[MT_DISP_LISTS_MAX];

	//---------------------------------------------
	// envelope
	strMtDispEnvelope * ptrEnvelope;

	//---------------------------------------------
	// trackTable
	strMtDispTrackTable * ptrTrackTable;

	//---------------------------------------------
	// editText
	strMtDispTextEdit textEditData;


	//---------------------------------------------
	// animowane przejscia
	uint8_t screenAnimation = 0;
	elapsedMillis animationTimer;
	uint8_t updateStep = 0;

	uint32_t updateAdress = 0;
	uint32_t *updateSize;


	//print
	#ifdef DEBUG

	#define MAX_PRINT_LINES 6
	#define PRINT_SHOW_TIME 1000
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
