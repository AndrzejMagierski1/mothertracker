

#ifndef LIBS_NEWDISPLAY_SELECTION_WINDOW_H_
#define LIBS_NEWDISPLAY_SELECTION_WINDOW_H_

#include "displayControls.h"

struct strSelectWindowData
{
	char textButton1[10];
	char textButton2[10];
	char secondLine[35];
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cSelectionWindow: public cDisplayControl
{
public:

	cSelectionWindow(strControlProperties* properties = nullptr);
	virtual ~cSelectionWindow();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	strSelectWindowData * data = nullptr;

	uint16_t textStyle;
	int16_t textFont;
	uint8_t fontHight;
};








#endif
