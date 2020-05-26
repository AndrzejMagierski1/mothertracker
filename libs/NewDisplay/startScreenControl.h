

#ifndef LIBS_STARTSCREEN_CONTROL_H_
#define LIBS_STARTSCREEN_CONTROL_H_

#include "displayControls.h"



struct strStartScreenData
{
	char versionLabel[18] = {0};
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cStartScreen: public cDisplayControl
{
public:

	cStartScreen(strControlProperties* properties = nullptr);
	virtual ~cStartScreen();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	strStartScreenData * data;

	uint8_t dots;
	uint16_t textStyle;
	int16_t textFont;
};








#endif
