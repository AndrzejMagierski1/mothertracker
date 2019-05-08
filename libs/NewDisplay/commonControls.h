#ifndef LIBS_NEWDISPLAY_COMMONCONTROLS_H_
#define LIBS_NEWDISPLAY_COMMONCONTROLS_H_

#include "displayControls.h"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cLabel: public cDisplayControl
{
public:

	cLabel(strControlProperties* properties = nullptr);
	cLabel(char* text = nullptr, uint16_t style = 0, uint16_t x = 0, uint16_t y = 0, uint16_t w = 0, uint16_t h = 0);
	virtual ~cLabel();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint16_t style);
	virtual void setText(char* text);

	//void setColors(uint32_t colorsTable[]);
	static uint8_t colorsCount;
	static uint32_t defaultColors[];
	uint32_t* colors = defaultColors;

	uint16_t textStyle;
	int16_t textFont;
};

//--------------------------------------------------------------------
class cEdit: public cDisplayControl
{
public:

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint16_t style);
	virtual void setText(char* text);
};

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------










#endif
