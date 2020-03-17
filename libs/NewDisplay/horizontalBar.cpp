
#include "FT812.h"

#include "displayControls.h"
#include "horizontalBarControl.h"



static uint32_t defaultColors[] =
{
	0x222222, // ramka okna
	0x222222, // tło okna
	0xFFFFFF, // ramka kontrolki
	0xFFFFFF,  // tło kontrolki
	0xFFFFFF  // tekst
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cHorizontalBar::cHorizontalBar(strControlProperties* properties): cDisplayControl(controlIdHorizontalBar)
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

cHorizontalBar::~cHorizontalBar()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cHorizontalBar::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cHorizontalBar::setText(char* text)
{
	this->text = text;
}

void cHorizontalBar::setValue(int value)
{
	this->value = value;
}

void cHorizontalBar::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cHorizontalBar::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cHorizontalBar::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cHorizontalBar::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    uint16_t frameWidth = width;
    uint16_t frameX = posX; //(width > 800/8) ? posX+5 : posX + width/2;

    uint16_t frameHeight = height;
    uint16_t frameY = posY;

    uint16_t barWidth = (5*width)/7;
    uint16_t barX = posX+ (width/7);

    uint16_t barHeight = 2 ;
    uint16_t barY = posY + ((3*height)/5);

    uint16_t textX = posX + (width/2);
    uint16_t textY = posY + ((2*height)/6);

	if((style & controlStyleBackground))
	{
		API_COLOR(colors[0]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(frameX+1, frameY+1);
		API_VERTEX2F(frameX+frameWidth-1, frameY+frameHeight-1);
		API_END();

		API_COLOR(colors[1]);

		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(frameX, frameY);
		API_VERTEX2F(frameX+frameWidth, frameY);
		API_VERTEX2F(frameX+frameWidth, frameY+frameHeight);
		API_VERTEX2F(frameX, frameY+frameHeight);
		API_VERTEX2F(frameX, frameY);
		API_END();
	}


	if((style & controlStyleValue_0_100) && value <= 100)
	{
	    uint16_t barFillY = (barWidth * value) / 100;


		API_BLEND_FUNC(SRC_ALPHA, ZERO);

		API_COLOR(colors[2]);

		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX, barY);
		API_VERTEX2F(barX+barFillY, barY+barHeight);
		API_END();

		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

		if(text != nullptr) API_CMD_TEXT(textX, textY, textFont, OPT_CENTER, text);
	}

	//API_COLOR(colors[0]);
	//if(style & controlStyleShowValue) API_CMD_NUMBER(posX+data->xValue, posY+5+data->yValue, textFont, data->styleValue, value);


    API_LIB_EndCoProList();


	return 0;
}

uint8_t cHorizontalBar::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cHorizontalBar::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
