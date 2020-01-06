#include "simpleTextControl.h"
#include "FT812.h"
#include "Arduino.h"


static uint32_t defaultColors[] =
{
	0x0a0a0a, // ramka okna
	0xFFFFFF  // tekst
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cSimpleText::cSimpleText(strControlProperties* properties)
{
	colorsCount = 5;
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

cSimpleText::~cSimpleText()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cSimpleText::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cSimpleText::setText(char* text)
{
	this->text = text;
}

void cSimpleText::setValue(int value)
{
	this->value = value;
}

void cSimpleText::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cSimpleText::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cSimpleText::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cSimpleText::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    if(text != nullptr)
    {
    	API_CMD_TEXT(posX, posY, 29, OPT_CENTER, text);
    }

    API_LIB_EndCoProList();

	return 0;
}

uint8_t cSimpleText::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cSimpleText::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}




