
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

	int16_t position = 0;
	uint16_t length = 0;
	int16_t part = 0; // max 744

	int8_t select = 0;
	uint8_t select_param = 0;

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

	uint16_t ramPartSize[5] = {0};
	uint8_t refreshStep;
	void refresh1();
	void refresh2();
	void refresh3();
	void refresh4();
	void refresh5();

	uint8_t firstVisibleTrack;
	int16_t visibleTracksOffset;
	uint8_t visibleCharOffset;

};




#endif



