
//#include "Arduino.h"

#include "FT812.h"

#include "progressCursorControl.h"

#include "Arduino.h"

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

static uint32_t defaultColors[] =

{
	0xFFFFFF, // linie
	one_true_red
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cProgressCursor::cProgressCursor(strControlProperties* properties): cDisplayControl(controlIdProgressCursor)
{
	colorsCount = 2;
	colors = defaultColors;


	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		width = 0;
		height = 0;

		this->style = 0;
		return;
	}

	posX = properties->x;
	posY = properties->y;
	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}


cProgressCursor::~cProgressCursor()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cProgressCursor::setStyle(uint32_t style)
{
	this->style = style;
}

void cProgressCursor::setText(const char* text)
{

}

void cProgressCursor::setValue(int value)
{
	this->value = value;
}

void cProgressCursor::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cProgressCursor::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cProgressCursor::setData(void* data)
{
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cProgressCursor::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	if(value > 0 && value < width)
	{
		API_COLOR(colors[1]);
		API_BEGIN(LINES);
		API_VERTEX2F( posX + value, posY);
		API_VERTEX2F( posX + value, posY + height);
		API_END();
	}

	API_LIB_EndCoProList();
	return 0;
}

uint8_t cProgressCursor::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}

uint8_t cProgressCursor::append(uint32_t address)
{


	API_CMD_APPEND(address, ramSize);
	//API_CMD_APPEND(address+ ramPartSize[0], ramPartSize[1]);
	//API_CMD_APPEND(address+ ramPartSize[0] + ramPartSize[1], ramPartSize[2]);



	return 0;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
