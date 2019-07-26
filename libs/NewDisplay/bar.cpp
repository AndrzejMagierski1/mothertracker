
#include "FT812.h"

#include "displayControls.h"
#include "barControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFFFFFF, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cBar::cBar(strControlProperties* properties)
{
	colorsCount = 3;
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

    uint16_t barWidth = (width >= 800/8) ? 800/8-10 : width-10;
    uint16_t barX = posX+(width-barWidth)/2; //(width > 800/8) ? posX+5 : posX + width/2;
    uint16_t barY = posY + 10;
    uint16_t barHeight = height - 20;



	if(style & controlStyleCompareTwoValues && value >= 0 && value <= 100 && data->value >= 0 && data->value <= 100 )
	{
	    uint16_t barFillY = barHeight - (barHeight * value) / 100;
	    int8_t valueSub = data->value - value;
	    int8_t absValueSub = (valueSub >= 0) ? valueSub: -valueSub;
	    uint16_t compareBarHeight = (barHeight * absValueSub) / 100;

		API_COLOR(colors[0]);

		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barY+barFillY+1);
		API_VERTEX2F(barX+barWidth-1, barY+barHeight-1);
		API_END();

		API_LINE_WIDTH(8);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(barX, barY);
		API_VERTEX2F(barX+barWidth, barY);
		API_VERTEX2F(barX+barWidth, barY+barHeight);
		API_VERTEX2F(barX, barY+barHeight);
		API_VERTEX2F(barX, barY);
		API_END();

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

		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barY+barFillY+1);
		API_VERTEX2F(barX+barWidth-1, barY+barHeight-1);
		API_END();

		API_LINE_WIDTH(8);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(barX, barY);
		API_VERTEX2F(barX+barWidth, barY);
		API_VERTEX2F(barX+barWidth, barY+barHeight);
		API_VERTEX2F(barX, barY+barHeight);
		API_VERTEX2F(barX, barY);
		API_END();
	}
	else if(style & controlStyleValueLeftRight_100_100 && value >= -100 && value <= 100)
	{

		uint8_t halfFill = barHeight/2 - 5;

		uint16_t barFillYtop  = barY +  			(  value > 0 ? ((halfFill * (100-value)) / 100) : halfFill);
		uint16_t barFillYbott = barY + barHeight -  (  value < 0 ? ((halfFill * (100+value)) / 100) : halfFill);

		API_COLOR(colors[0]);

		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barFillYtop+1);
		API_VERTEX2F(barX+barWidth-1, barFillYbott-1);
		API_END();

		API_LINE_WIDTH(8);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(barX, barY);
		API_VERTEX2F(barX+barWidth, barY);
		API_VERTEX2F(barX+barWidth, barY+barHeight);
		API_VERTEX2F(barX, barY+barHeight);
		API_VERTEX2F(barX, barY);
		API_END();
	}
	else if(style & controlStyleValueLeftRight_24_24 && value >= -24 && value <= 24)
	{
		uint16_t barFillY = barHeight - (barHeight * value) / 100;

		API_COLOR(colors[0]);

		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2F(barX+1, barY+barFillY+1);
		API_VERTEX2F(barX+barWidth-1, barY+barHeight-1);
		API_END();

		API_LINE_WIDTH(8);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(barX, barY);
		API_VERTEX2F(barX+barWidth, barY);
		API_VERTEX2F(barX+barWidth, barY+barHeight);
		API_VERTEX2F(barX, barY+barHeight);
		API_VERTEX2F(barX, barY);
		API_END();
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
