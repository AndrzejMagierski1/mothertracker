//#include "Arduino.h"

#include "FT812.h"
#include "wtProgress.h"
#include "Arduino.h"

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

static uint32_t defaultColors[] =

{
	0xFFFFFF, // linie
	0xFFFFFF
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cWtProgress::cWtProgress(strControlProperties* properties)
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


cWtProgress::~cWtProgress()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cWtProgress::setStyle(uint32_t style)
{
	this->style = style;
}

void cWtProgress::setText(char* text)
{

}

void cWtProgress::setValue(int value)
{
	this->value = value;
}

void cWtProgress::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cWtProgress::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cWtProgress::setData(void* data)
{
	wt_data = (wt_position_data_t*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cWtProgress::update()
{
	uint32_t bar_heigth = 15;
	uint32_t bar_length;

	API_LIB_BeginCoProListNoCheck();
	API_CMD_DLSTART();

	API_COLOR(colors[1]);
	API_BEGIN(LINES);
	API_VERTEX2F(posX, posY);
	API_VERTEX2F(posX + width, posY + height);

	float lengthOfWindow = (width / (float)wt_data->positionMax);
	uint32_t bar_start = (wt_data->position * lengthOfWindow + 0.5f);/* 0.5f for rounding*/

	if(lengthOfWindow < 0)
	{
		bar_length = 1;
	}
	else
	{
		bar_length = lengthOfWindow;
	}

	API_BEGIN(RECTS);
	API_VERTEX2F(posX + bar_start, posY - (bar_heigth/2) + (height/2));
	API_VERTEX2F(posX + bar_start + bar_length , posY + (bar_heigth)/2);
	API_END();

	API_END();

	API_LIB_EndCoProList();
	return 0;
}

uint8_t cWtProgress::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}

uint8_t cWtProgress::append(uint32_t address)
{


	API_CMD_APPEND(address, ramSize);
	//API_CMD_APPEND(address+ ramPartSize[0], ramPartSize[1]);
	//API_CMD_APPEND(address+ ramPartSize[0] + ramPartSize[1], ramPartSize[2]);



	return 0;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
