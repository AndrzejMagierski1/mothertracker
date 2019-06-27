
#include "FT812.h"

#include "displayControls.h"
#include "commonControls.h"


static uint32_t defaultColors[] =
{
	0x000000, // tekst
	0xFFFFFF, // tło
	0xFF0000, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cLabel::cLabel(strControlProperties* properties)
{
	colorsCount = 3;
	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;

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

	posX = properties->x;
	posY = properties->y;

	text = properties->text;

	value = properties->value;

	data = (strLabelData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cLabel::~cLabel()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cLabel::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);


	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cLabel::setText(char* text)
{
	this->text = text;
}

void cLabel::setValue(int value)
{
	this->value = value;
}

void cLabel::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cLabel::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cLabel::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cLabel::update()
{

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

   // uint16_t text_x = posX, text_y = posY;
    int16_t border_x = posX-2;//, border_y = posY-2;

	if(style & controlStyleCenterX)
	{
		//border_x = posX - (getTextWidth(FONT_INDEX_FROM_STYLE,text)/2 + 2);

		border_x = posX - (width/2);
	}


	if(style & controlStyleBackground)
	{
		API_COLOR(colors[1]);

		if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
		else API_LINE_WIDTH(8);



		API_BEGIN(RECTS);
		API_VERTEX2F(border_x , posY);
		API_VERTEX2F(border_x+width , posY+height);
		API_END();

		//text_x = posX + 2;
		//text_y = posY + 2;
	}
	if(style & controlStyleBorder)
	{
		API_COLOR(colors[2]);

		API_LINE_WIDTH(32);

		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x-1 , posY-1);
		API_VERTEX2F(border_x+width+1 , posY-1);
		API_VERTEX2F(border_x+width+1 , posY+height+1);
		API_VERTEX2F(border_x-1 , posY+height+1);
		API_VERTEX2F(border_x-1 , posY-1);
		API_END();

		//text_x = posX + 2;
		//text_y = posY + 2;
	}


	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

	//API_RESTORE_CONTEXT();
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

	//API_LINE_WIDTH(16);
	API_COLOR(colors[0]);
	if(text != nullptr) API_CMD_TEXT(posX, posY+5, textFont, textStyle, text);

	if(style & controlStyleShowValue) API_CMD_NUMBER(posX+data->xValue, posY+5+data->yValue, textFont, data->styleValue, value);




    API_LIB_EndCoProList();


	return 0;
}

uint8_t cLabel::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cLabel::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
