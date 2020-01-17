#ifndef LIBS_NEWDISPLAY_TEXTBOXCONTROL_H_
#define LIBS_NEWDISPLAY_TEXTBOXCONTROL_H_


#include "displayControls.h"


struct strTextBoxData
{
	char* text = {nullptr};

};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cTextBox: public cDisplayControl
{
public:

	cTextBox(strControlProperties* properties = nullptr);
	virtual ~cTextBox();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	void string2Bitmaps(int16_t x, int16_t y, strFont* font, char* string, int8_t length);
	int16_t getNextLineLength();

	uint16_t textStyle;
	const strFont* font;

	strTextBoxData* data;


	uint32_t textLength;
	char* position;

};








#endif
