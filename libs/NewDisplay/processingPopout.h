#ifndef LIBS_NEWDISPLAY_PROCESSING_POPOUT_H_
#define LIBS_NEWDISPLAY_PROCESSING_POPOUT_H_

#include "displayControls.h"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cProcessingPop: public cDisplayControl
{
public:

	void string2Bitmaps(int16_t x, int16_t y, char* string, int8_t length);
	cProcessingPop(strControlProperties* properties = nullptr);
	virtual ~cProcessingPop();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	uint8_t dots;
	char localBaseText[30];
	char localModifiedText[30];
	uint16_t textStyle;
	int16_t textFont;

	uint8_t fontWidth;
	uint8_t fontHeight;
};




#endif
