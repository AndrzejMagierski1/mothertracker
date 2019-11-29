
#include "FT812.h"

#include "displayControls.h"
#include "commonControls.h"

#include <string.h>


static uint32_t defaultColors[] =
{
	0x000000, // linie
	0xFFFFFF, // tło
	0xFF0000, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cBgLabel::cBgLabel(strControlProperties* properties)
{
	colorsCount = 3;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		data = nullptr;
		width = 0;
		height = 0;
		style = 0;
		value = 0;
		return;
	}

	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;



	posX = properties->x;
	posY = properties->y;

	text = properties->text;

	value = properties->value;

	data = (strBgLabelData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cBgLabel::~cBgLabel()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cBgLabel::setStyle(uint32_t style)
{
	this->style = style;

}

void cBgLabel::setText(char* text)
{
	this->text = text;
}

void cBgLabel::setValue(int value)
{
	this->value = value;
}

void cBgLabel::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cBgLabel::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cBgLabel::setData(void* data)
{
	this->data = (strBgLabelData*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cBgLabel::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    int16_t border_x = posX;
    int16_t border_y = posY;

    int16_t lines_step_y = 0;

	if(style & controlStyleCenterX)
	{
		border_x = posX - (width/2);
	}
	if(style & controlStyleCenterY)  // 17 px
	{
		border_y = posY - (height/2);
	}

	if(style & controlStyleNoTransparency)
	{
		API_BLEND_FUNC(SRC_ALPHA, ZERO);
	}

	//------------------------------------------

	API_COLOR(colors[1]);
	API_LINE_WIDTH(16);

	API_BEGIN(RECTS);
	API_VERTEX2F(border_x+1, border_y);
	API_VERTEX2F(border_x+width-2, border_y+height);
	API_END();



	if(style & controlStyleNoTransparency)
	{
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	}




	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);

	API_BEGIN(LINES);

	for(uint8_t i = 1; i<8; i++)
	{
		if(value & (1<<i))
		{
			API_VERTEX2F((width/8)*i+posX, posY+height/4);
			API_VERTEX2F((width/8)*i+posX, posY+height);
		}
	}

	API_END();

	//------------------------------------------



	if(style & controlStyleBorder)
	{
		API_COLOR(colors[2]);
		API_LINE_WIDTH(24);

		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x , border_y);
		API_VERTEX2F(border_x+width , border_y);
		API_VERTEX2F(border_x+width , border_y+height);
		API_VERTEX2F(border_x , border_y+height);
		API_VERTEX2F(border_x , border_y);
		API_END();
	}



    API_LIB_EndCoProList();


	return 0;
}

uint8_t cBgLabel::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cBgLabel::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
