
#ifndef _TACKER_CONTROL_H_
#define _TACKER_CONTROL_H_


#include "displayControls.h"

struct strTrackerPattern
{
	struct strTracks
	{
		struct strRow
		{
			char note[5] = "";
			char instr[3] = "";
			char vol[3] = "";
			char fx[4] = "";

		} row[21];

	} track[8];

	//-------------------------------
	// aktualna pozycja kursora -
	int8_t actualTrack = 0;		// <- ramka bedzie widziana zaleznie od firstVisibleTrack (0-7)
	int16_t actualStep = 0; 	// <- od tej wartosci zalezy zawsze pozycja w pionie (0-xxx)

	int16_t patternLength = 1;		// dlugosc patternu (1-xxx)
	int8_t firstVisibleTrack = 0;	// pierwszy track od lewej widziany na wyswitlaczu (0-4)
									// - steruje przesunieciem na boki
	int16_t playheadPosition = 0;	// linia odtwarzania - wartosc stepa (0-xxx)

	int8_t selectState = 0;			// 1 = kursor ; 2 = kursor + zaznaczenie
	int8_t selectedParam = 0;		// wybrany do modyfikacji parametr (0-4; 4 = wszsytkie)

	int8_t selectStartTrack = 0;	// start zaznaczenia 	//
	int16_t selectStartStep = 0;							// <= ustawienie obojetne
	int8_t selectEndTrack = 0;		// koniec zaznaczenia	//
	int16_t selectEndStep = 0;
};





//--------------------------------------------------------------------
class cTracker: public cDisplayControl
{
public:

	cTracker(strControlProperties* properties);
	virtual ~cTracker();

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

private:

	strTrackerPattern* tracks;

	int16_t select1_x, select2_x, select1_y, select2_y;
	uint8_t selectActive = 0;

	uint16_t ramPartSize[5] = {0};
	uint8_t refreshStep;
	void refresh1();
	void refresh2();
	void refresh3();
	void refresh4();
	void refresh5();

//	uint8_t firstVisibleTrack;
//	int16_t visibleTracksOffset;
//	uint8_t visibleCharOffset;

};




#endif



