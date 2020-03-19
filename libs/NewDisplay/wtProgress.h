
#ifndef _WT_PROGRESS_H_
#define _WT_PROGRESS_H_


#include "displayControls.h"

typedef struct
{
	uint32_t position;
	uint32_t window;
	uint32_t positionMax;

}wt_position_data_t;

//--------------------------------------------------------------------
class cWtProgress: public cDisplayControl
{
public:

	cWtProgress(strControlProperties* properties);
	virtual ~ cWtProgress();

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

private:

	wt_position_data_t *wt_data = nullptr;

};




#endif



