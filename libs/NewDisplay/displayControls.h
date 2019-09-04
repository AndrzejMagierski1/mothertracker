
#ifndef LIBS_NEWDISPLAY_DISPALYCONTROLS_H_
#define LIBS_NEWDISPLAY_DISPALYCONTROLS_H_

#include <stdint.h>
#include <stddef.h>

extern "C" {
extern void *memcpy (void *dst, const void *src, size_t count);
}

enum controlsStyle
{
	controlStyleHide = 0,
	controlStyleShow = 1,

	controlStyleCreateSubcontrols = 2,

	controlStyleCenterX = 4,
	controlStyleCenterY = 8,
	controlStyleRightX 	= 16,

	controlStyleShowValue	= 32,
	controlStyleManualText	= 64,


	controlStyleFont1 	= 256,
	controlStyleFont2 	= 512,
	controlStyleFont3 	= 1024,
	controlStyleFont4 	= 1280,

	controlStyleBorder 			= (1<<12),
	controlStyleBackground 		= (1<<13),
	controlStyleRoundedBorder 	= (1<<14),
	controlStyleNoTransparency 	= (1<<15),

	controlStyleValueLeftRight_100_100 	= (1<<16),
	controlStyleValueLeftRight_24_24 	= (1<<17),
	controlStyleValue_0_100 			= (1<<18),
	controlStyleValueNumberOnly			= (1<<19),
	controlStyleCompareTwoValues 		= (1<<20)
};


struct strControlProperties
{
	char* text = nullptr;
	int value = 0;
	void* data = nullptr;

	uint32_t style = 0;
	uint32_t* colors = nullptr;

	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t w = 0;
	uint16_t h = 0;
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


extern strFont fonts[];
extern strBitmap bitmaps[];


#define FONT_INDEX_FROM_STYLE (((style >> 8) & 15)-1)


uint16_t getTextWidth(uint8_t font, char* text);

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

	virtual void setStyle(uint32_t style) = 0;
	virtual void setText(char* text) = 0;
	virtual void setValue(int value) = 0;
	virtual void setColors(uint32_t* colors) = 0;
	virtual void setDefaultColors(uint32_t* colors) = 0;
	virtual void setData(void* data) = 0;

	cDisplayControl()
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
	char* text;
	int value;

	uint16_t posX;
	uint16_t posY;

	uint16_t width;
	uint16_t height;

	uint8_t colorsCount;
	uint32_t* colors;


};




#endif
