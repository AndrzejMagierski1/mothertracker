
#include "FT812.h"

#include "displayControls.h"
#include "imgControl.h"

//extern strGetProps getProps;


static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFFFFFF, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cImg::cImg(strControlProperties* properties): cDisplayControl(controlIdImg)
{
	colorsCount = 3;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		value = 0;
		width = 0;
		height = 0;
		style = 0;
		return;
	}

	posX = properties->x;
	posY = properties->y;

	text = properties->text;
	value = properties->value;

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cImg::~cImg()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cImg::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cImg::setText(const char* text)
{
	this->text = text;
}

void cImg::setValue(int value)
{
	this->value = value;
}

void cImg::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cImg::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cImg::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cImg::update()
{
	//if(display.)
	if(value < 300000 || value > 1000000) return 0;

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    int16_t x = posX, y = posY;


	if(style & controlStyleCenterX)
	{
		x = posX-getProps.Width/2;
	}
	if(style & controlStyleCenterY)
	{

		y = posY-getProps.Height/2;
	}

	//API_LIB_WriteDataToCMD
	//API_CMD_LOADIMAGE(value, 0);
	//API_CMD_SETBITMAP(value, RGB565, 150, 150);

	//API_BITMAP_HANDLE(12);
	//API_CMD_SETBITMAP(value, PALETTED4444, 150, 150);

	//API_BITMAP_HANDLE(12);
	//API_BITMAP_SOURCE(value);
	API_CMD_SETBITMAP(value, RGB565, getProps.Width, getProps.Height);

	if(style & controlStyleTransformImg)
	{
		uint16_t transform = 276;
		API_BITMAP_TRANSFORM_A(transform);
	}

//	API_BITMAP_LAYOUT(RGB565, getProps.Width*2, getProps.Height); // Format, Stride, Height
//	API_BITMAP_SIZE(BILINEAR, BORDER, BORDER, getProps.Width, getProps.Height);
//	API_BITMAP_LAYOUT_H((getProps.Width * 2) >> 10, getProps.Height >> 9);
//	API_BITMAP_SIZE_H(getProps.Width >> 9, getProps.Width >> 9);

	API_BEGIN(BITMAPS);
	API_VERTEX2F(x, y);
	API_END();

	if(style & controlStyleTransformImg)
	{
		API_BITMAP_TRANSFORM_A(256);
	}

    API_LIB_EndCoProList();


	return 0;
}

uint8_t cImg::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}

uint8_t cImg::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

