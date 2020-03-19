

#ifndef LIBS_BAR_CONTROL_H_
#define LIBS_BAR_CONTROL_H_

#include "displayControls.h"

struct strCompareValue
{
	int value;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cBar: public cDisplayControl
{
public:

	cBar(strControlProperties* properties = nullptr);
	virtual ~cBar();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	strCompareValue * data;

	uint16_t textStyle;
	int16_t textFont;
};








#endif
