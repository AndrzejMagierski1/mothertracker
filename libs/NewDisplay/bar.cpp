
#include "FT812.h"

#include "displayControls.h"
#include "barControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // kolor glowny
	0xFFFFFF, // kolor dodatkowy 1 ziel
	0xf13c3c, // kolor dodatkowy 2 czer
	0x080808, // kontener
	0x0a0a0a, // tlo
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cBar::cBar(strControlProperties* properties)
{
	colorsCount = 5;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		data = nullptr;
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

	data = (strCompareValue*)(properties->data);

	setStyle(properties->style);
}

cBar::~cBar()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cBar::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cBar::setText(char* text)
{
	this->text = text;
}

void cBar::setValue(int value)
{
	this->value = value;
}

void cBar::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cBar::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cBar::setData(void* data)
{
	data = (strCompareValue*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cBar::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    uint16_t barWidth = (width >= 800/8) ? 800/8-20 : width-20;
    uint16_t barX = posX+(width-barWidth)/2; //(width > 800/8) ? posX+5 : posX + width/2;
    uint16_t barY = posY + 10;
    uint16_t barHeight = height - 25;

    if(style & controlStyleBackground)
    {
		API_BLEND_FUNC(SRC_ALPHA , ZERO);
		API_COLOR(colors[4]);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(posX, posY);
		API_VERTEX2F(posX+width, posY+height);
		API_END();
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


		API_SAVE_CONTEXT();

		API_SCISSOR_XY(posX-1, posY+height-10);
		API_SCISSOR_SIZE(width+2, 10);
		API_CMD_GRADIENT(0, 413, colors[4], 0, 423, 0x0);

		API_RESTORE_CONTEXT();
    }

    //kontener
//	API_BLEND_FUNC(SRC_ALPHA , ZERO);
    API_COLOR(colors[3]);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);
	API_VERTEX2F(posX+10, posY+11);
	API_VERTEX2F(posX+width-10,  posY+height-14);
	API_END();


	API_SAVE_CONTEXT();


	API_SCISSOR_XY(posX+10, posY+11);
	API_SCISSOR_SIZE(8, height-25);
	API_CMD_GRADIENT(posX+10, 0, 0x0, posX+18, 0, colors[3]);


	API_RESTORE_CONTEXT();

//	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	//

	if(style & controlStyleCompareTwoValues && value >= 0 && value <= 100 && data->value >= 0 && data->value <= 100 )
	{
	    uint16_t barFillY = barHeight - (barHeight * value) / 100;
	    int8_t valueSub = data->value - value;
	    int8_t absValueSub = (valueSub >= 0) ? valueSub: -valueSub;
	    uint16_t compareBarHeight = (barHeight * absValueSub) / 100;

		API_COLOR(colors[0]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barY+barFillY+1);
		API_VERTEX2F(barX+barWidth-1, barY+barHeight-1);
		API_END();

//		API_LINE_WIDTH(16);
//		API_BEGIN(LINE_STRIP);
//		API_VERTEX2F(barX, barY);
//		API_VERTEX2F(barX+barWidth, barY);
//		API_VERTEX2F(barX+barWidth, barY+barHeight);
//		API_VERTEX2F(barX, barY+barHeight);
//		API_VERTEX2F(barX, barY);
//		API_END();

		if( valueSub > 0)
		{
			API_COLOR(colors[1]);
			API_BEGIN(RECTS);
			API_VERTEX2F(barX+1, barY+barFillY);
			API_VERTEX2F(barX+barWidth-1, barY+barFillY-compareBarHeight-1);

			API_END();
		}
		else if( valueSub < 0)
		{
			API_COLOR(colors[2]);
			API_BEGIN(RECTS);
			API_VERTEX2F(barX+barWidth-1, barY+barFillY+compareBarHeight-1);
			API_VERTEX2F(barX+1, barY+barFillY+1);
			API_END();
		}


	}
	if(style & controlStyleValue_0_100 && value >= 0 && value <= 100)
	{
	    uint16_t barFillY = barHeight - (barHeight * value) / 100;

		API_COLOR(colors[0]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barY+barFillY+1);
		API_VERTEX2F(barX+barWidth-1, barY+barHeight-1);
		API_END();

//		API_COLOR(colors[1]);
//		API_LINE_WIDTH(16);
//		API_BEGIN(LINE_STRIP);
//		API_VERTEX2F(barX, barY);
//		API_VERTEX2F(barX+barWidth, barY);
//		API_VERTEX2F(barX+barWidth, barY+barHeight);
//		API_VERTEX2F(barX, barY+barHeight);
//		API_VERTEX2F(barX, barY);
//		API_END();
	}
	else if(style & controlStyleValueLeftRight_100_100 && value >= -100 && value <= 100)
	{

		uint8_t halfFill = barHeight/2 - 5;

		uint16_t barFillYtop  = barY +  			(  value > 0 ? ((halfFill * (100-value)) / 100) : halfFill);
		uint16_t barFillYbott = barY + barHeight -  (  value < 0 ? ((halfFill * (100+value)) / 100) : halfFill);

		API_COLOR(colors[0]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barFillYtop+1);
		API_VERTEX2F(barX+barWidth-1, barFillYbott-1);
		API_END();

//		API_LINE_WIDTH(16);
//		API_BEGIN(LINE_STRIP);
//		API_VERTEX2F(barX, barY);
//		API_VERTEX2F(barX+barWidth, barY);
//		API_VERTEX2F(barX+barWidth, barY+barHeight);
//		API_VERTEX2F(barX, barY+barHeight);
//		API_VERTEX2F(barX, barY);
//		API_END();
	}
	else if(style & controlStyleValueLeftRight_24_24 && value >= -24 && value <= 24)
	{
		uint16_t barFillY = barHeight - (barHeight * value) / 100;

		API_COLOR(colors[0]);

		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barY+barFillY+1);
		API_VERTEX2F(barX+barWidth-1, barY+barHeight-1);
		API_END();

//		API_LINE_WIDTH(16);
//		API_BEGIN(LINE_STRIP);
//		API_VERTEX2F(barX, barY);
//		API_VERTEX2F(barX+barWidth, barY);
//		API_VERTEX2F(barX+barWidth, barY+barHeight);
//		API_VERTEX2F(barX, barY+barHeight);
//		API_VERTEX2F(barX, barY);
//		API_END();
	}








	//API_COLOR(colors[0]);

	//if(style & controlStyleShowValue) API_CMD_NUMBER(posX+data->xValue, posY+5+data->yValue, textFont, data->styleValue, value);


    API_LIB_EndCoProList();


	return 0;
}

uint8_t cBar::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cBar::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
