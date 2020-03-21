

#ifndef SLICE_CONTROL_H_
#define SLICE_CONTROL_H_

#include "displayControls.h"
#include "mtStructs.h"


struct strSlicePoints
{
	uint8_t sliceNumber = 0;
	int16_t sliceTab[MAX_SLICE_NUMBER] = {0};
	uint8_t selected = 0;
};



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cSlicePoints: public cDisplayControl
{
public:

	cSlicePoints(strControlProperties* properties = nullptr);
	virtual ~cSlicePoints();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);


	strSlicePoints * points;

	uint16_t textStyle;
	int16_t textFont;
};








#endif
