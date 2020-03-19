
#ifndef LIBS_NEWDISPLAY_DISPALYCONTROLS_H_
#define LIBS_NEWDISPLAY_DISPALYCONTROLS_H_

#include "displayStructs.h"

//--------------------------------------------------------------------
// klasa wirtualna bazowa dla kontrolek
class cDisplayControl
{
	friend class cDisplay;
	friend class cDebugLog;

public:

	virtual uint8_t update() = 0;
	virtual uint8_t append(uint32_t address) = 0;
	virtual uint8_t memCpy(uint32_t address) = 0;

/*
	void setColors(uint32_t colorsTable[])
	{
		for(uint8_t i = 0; i < colorsCount;i++) if(colorsTable+i == nullptr || colorsTable[i] > 0xFFFFFF) return;
		colors = colorsTable;
	}
*/

	virtual void setStyle(uint32_t style) = 0;
	virtual void setText(const char* text) = 0;
	virtual void setValue(int value) = 0;
	virtual void setColors(uint32_t* colors) = 0;
	virtual void setDefaultColors(uint32_t* colors) = 0;
	virtual void setData(void* data) = 0;

	cDisplayControl(control_id_t _id):id(_id)
	{
		refresh = 0;
		ramMapPosition = 0;
		ramSize = 0;
		style = 0;
		text = nullptr;
		value = 0;
		posX = 0;
		posY = 0;
		width = 0;
		height = 0;
		colorsCount = 10;
		colors = nullptr;
	}
	virtual ~cDisplayControl() {}


protected:

	uint8_t refresh;

	uint8_t ramMapPosition;
	uint32_t ramSize;

	uint32_t style;
	const char* text;
	int value;

	uint16_t posX;
	uint16_t posY;

	uint16_t width;
	uint16_t height;

	uint8_t colorsCount;
	uint32_t* colors;

	control_id_t id;

};




#endif
