
#ifndef _PROGRESS_CURSOR_CONTROL_H_
#define _PROGRESS_CURSOR_CONTROL_H_


#include "displayControls.h"

//--------------------------------------------------------------------
class cProgressCursor: public cDisplayControl
{
public:

	cProgressCursor(strControlProperties* properties);
	virtual ~cProgressCursor();

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

};




#endif



