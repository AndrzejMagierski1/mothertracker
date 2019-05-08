
#ifndef LIBS_NEWDISPLAY_DISPALYCONTROLS_H_
#define LIBS_NEWDISPLAY_DISPALYCONTROLS_H_

#include <stdint.h>



enum controlsStyle
{
	controlStyleHide = 0,
	controlStyleShow = 1,

	controlStyleCenterX = 4,
	controlStyleCenterY = 8,
	controlStyleRightX 	= 16,


	controlStyleFont1 	= 256,
	controlStyleFont2 	= 512,
	controlStyleFont3 	= 1024,
	controlStyleFont4 	= 1280,

};


struct strControlProperties
{
	char* text;
	int number;

	int16_t style;

	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};


struct strFont
{
	const uint8_t* data;
	uint32_t size;
	int16_t handle;
	uint32_t address;
	int32_t source;
	uint16_t width;
	uint16_t height;
	uint8_t format;
	uint16_t linestride;
};

struct strBitmap
{
	const uint8_t* data;
	uint32_t address;
	uint32_t source;
	uint16_t width;
	uint16_t height;
	uint8_t format;
	uint16_t linestride;
};


extern strFont font[];
extern strBitmap bitmap[];


//--------------------------------------------------------------------
// klasa wirtualna bazowa dla kontrolek
class cDisplayControl
{
	friend class cDisplay;

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

	virtual void setStyle(uint16_t style) = 0;
	virtual void setText(char* text) = 0;

	cDisplayControl()
	{
		ramMapPosition = 0;
		ramSize = 0;
		style = 0;
		text = nullptr;
		posX = 0;
		posY = 0;
		cWidth = 0;
		cHeight = 0;
	}
	virtual ~cDisplayControl() {}


protected:

	uint8_t ramMapPosition;
	uint32_t ramSize;

	uint16_t style;
	char* text;

	uint16_t posX;
	uint16_t posY;

	uint16_t cWidth;
	uint16_t cHeight;

	static uint8_t colorsCount;
	static uint32_t defaultColors[];
	uint32_t* colors = defaultColors;


};




#endif
