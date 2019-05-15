
#include "FT812.h"

#include "displayControls.h"
#include "templateControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFFFFFF, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cTemplate::cTemplate(strControlProperties* properties)
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

cTemplate::~cTemplate()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cTemplate::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cTemplate::setText(char* text)
{
	this->text = text;
}

void cTemplate::setValue(int value)
{

}

void cTemplate::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cTemplate::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cTemplate::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cTemplate::update()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();


	if(style & controlStyleCenterX)
	{

	}



	API_COLOR(colors[0]);
	API_CMD_TEXT(posX, posY, textFont, textStyle, text);


    API_LIB_EndCoProList();


	return 0;
}

uint8_t cTemplate::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cTemplate::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
