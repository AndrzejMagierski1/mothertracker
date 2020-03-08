
#include "FT812.h"

#include "displayControls.h"
#include "commonControls.h"
#include "string.h"

static uint32_t defaultColors[] =
{
	0xffffff, 		// tekst
	0x080808, 		// kontener
	one_true_red, 		// ramka
	one_true_red, 	// kursor
	0x0a0a0a, 		//tlo
};

void localString2Bitmaps(int16_t x, int16_t y, const strFont* font, char* string, int8_t length);
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cEdit::cEdit(strControlProperties* properties): cDisplayControl(controlIdEdit)
{
	colorsCount = 3;
	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
//		data = nullptr;
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

//	data = (strLabelData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cEdit::~cEdit()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cEdit::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);




	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font = &fonts[textFont];


}

void cEdit::setText(char* text)
{
	this->text = text;
}

void cEdit::setValue(int value)
{
	this->value = value;
}

void cEdit::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cEdit::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cEdit::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cEdit::update()
{

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

   // uint16_t text_x = posX, text_y = posY;
    int16_t border_x = posX-2;//, border_y = posY-2;

	if(style & controlStyleCenterX)
	{
		//border_x = posX - (getTextWidth(FONT_INDEX_FROM_STYLE,text)/2 + 2);

		border_x = posX - (width/2) -1;
	}


	if(style & controlStyleBackground)
	{
		if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
		else API_LINE_WIDTH(16);

		API_COLOR(colors[4]);
		//tlo
		API_BEGIN(RECTS);
		API_VERTEX2F(border_x, posY);
		API_VERTEX2F(border_x+width, posY+height);

		API_COLOR(colors[1]);
		// kontener
		API_VERTEX2F(border_x+10 , posY+10);
		API_VERTEX2F(border_x+width-10 , posY+height-10);
		API_END();
	}
	if(style & controlStyleBorder)
	{
		API_COLOR(colors[2]);

		API_LINE_WIDTH(8);

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

	API_COLOR(colors[2]);
	API_LINE_WIDTH(16);
	API_BEGIN(LINES);
	uint8_t localTextLength = strlen(text);
	uint8_t font_y = posY+(height/2)-(font->height/2);

	if(textStyle & OPT_CENTERX)
	{

		API_VERTEX2F(posX + font->width *(value - localTextLength/2  ) , font_y);
		API_VERTEX2F(posX + font->width *(value - localTextLength/2  ) ,  font_y + font->height);
	}
	else
	{
		API_VERTEX2F(posX + value*font->width , font_y);
		API_VERTEX2F(posX + value*font->width , font_y + font->height);
	}

	API_END();


	API_COLOR(colors[0]);
	API_BITMAP_HANDLE(font->handle);
	API_BEGIN(BITMAPS);
	if(text != nullptr) localString2Bitmaps(posX - font->width * (localTextLength/2) , posY+height/2, font, text, localTextLength);
	API_END();
//	API_CMD_TEXT(posX, posY+5, textFont, textStyle, text);

//	if(style & controlStyleShowValue) API_CMD_NUMBER(posX+data->xValue, posY+5+data->yValue, textFont, data->styleValue, value);




    API_LIB_EndCoProList();


	return 0;
}

uint8_t cEdit::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cEdit::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

void localString2Bitmaps(int16_t x, int16_t y, const strFont* font, char* string, int8_t length)
{
	y = y - font->height/2;
	uint8_t strPtr = 0;


	for(uint8_t i = 0; i < length; i++)
	{
		if(x > 755 || y > 480 || x < 30 || y < 0)
		{
			strPtr++;
		}
		else if((x > 511 || y > 511) && string[strPtr] >=32)
		{
			API_CELL(string[strPtr++]);
			API_VERTEX2F(x, y);
		}
		else if(string[strPtr] >=32)
		{
			API_VERTEX2II(x,y,font->handle, (char)string[strPtr++]);
		}
		x+=font->width;
	}
}
