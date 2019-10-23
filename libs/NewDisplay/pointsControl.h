

#ifndef LIBS_NEWDISPLAY_POINTS_H_
#define LIBS_NEWDISPLAY_POINTS_H_

#include "displayControls.h"

enum
{
	selectStart = 0x01U,
	selectLoop1 = 0x02U,
	selectLoop2 = 0x04U,
	selectEnd = 0x08U,
};

struct strTrackerPoints
{
	uint8_t pointsType = 0;
	int16_t startPoint;
	int16_t endPoint;
	int16_t loopPoint1;
	int16_t loopPoint2;

	uint8_t selected = 0;
};



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cPoints: public cDisplayControl
{
public:

	cPoints(strControlProperties* properties = nullptr);
	virtual ~cPoints();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);


	strTrackerPoints* points;

	uint16_t textStyle;
	int16_t textFont;
};








#endif
