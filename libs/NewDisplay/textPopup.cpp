
#include "FT812.h"

#include "displayControls.h"
#include "textPopupControl.h"

#include <string.h>

void labelString2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);


static uint32_t defaultColors[] =
{
	0x000000, // tekst
	0x0a0a0a, // tło
	one_true_red, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cTextPopup::cTextPopup(strControlProperties* properties)
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

	data = (strTextPopupData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cTextPopup::~cTextPopup()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cTextPopup::setStyle(uint32_t style)
{
//	this->style = style;
//
//	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
//				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
//	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);
//
//
//	textFont = FONT_INDEX_FROM_STYLE;
//	textFont = (textFont>=0) ? textFont : 0;
//	fontWidth = fonts[textFont].width;
//	fontHeight = fonts[textFont].height;
//	textFont =  fonts[textFont].handle;
//
	this->style = style;

	textStyle =   (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);


	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font = &fonts[textFont];

}

void cTextPopup::setText(char* text)
{
	this->text = text;
}

void cTextPopup::setValue(int value)
{
	this->value = value;
}

void cTextPopup::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cTextPopup::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cTextPopup::setData(void* data)
{
	this->data = (strTextPopupData*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cTextPopup::update()
{

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

   // uint16_t text_x = posX, text_y = posY;
    int16_t border_x = posX;//, border_y = posY-2;
    int16_t border_y = posY;

	if(style & controlStyleCenterX)
	{
		//border_x = posX - (getTextWidth(FONT_INDEX_FROM_STYLE,text)/2 + 2);

		border_x = posX - (width/2);
	}
	if(style & controlStyleCenterY)
	{
		border_y = posY - (height/2);
	}

	if(style & controlStyleBackground)
	{
		if(style & controlStyleNoTransparency)
		{
			API_BLEND_FUNC(SRC_ALPHA, ZERO);
		}

		API_COLOR(colors[1]);

		if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
		else API_LINE_WIDTH(16);

		API_BEGIN(RECTS);
		API_VERTEX2F(border_x , border_y);
		API_VERTEX2F(border_x+width , border_y+height);
		API_END();

		if(style & controlStyleNoTransparency)
		{
			API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		}
	}
	if(style & controlStyleBorder)
	{
		API_COLOR(colors[2]);

		API_LINE_WIDTH(32);

		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x-1 , border_y-1);
		API_VERTEX2F(border_x+width+1 , border_y-1);
		API_VERTEX2F(border_x+width+1 , border_y+height+1);
		API_VERTEX2F(border_x-1 , border_y+height+1);
		API_VERTEX2F(border_x-1 , border_y-1);
		API_END();
	}


	uint16_t line_textStyle;
	uint8_t line_fontWidth;
	uint8_t line_fontHeight;
	int16_t line_textFont;
	int16_t line_posX;
	int16_t line_posY = posY;

	for(uint8_t i = 0; i < data->textLinesCount; i++)
	{
		if(data->multiLineText[i] != nullptr)
		{
			// color
			if(*data->multiLineColors[i] <= 0xffffff) API_COLOR(*data->multiLineColors[i]);
			else API_COLOR(colors[0]);

			// styl :OOOOOOOOO
			line_textStyle = 0;
			line_fontWidth = font->width;
			line_fontHeight = font->height;
			line_textFont = font->handle;

			uint32_t tempStyle = *data->multiLineStyle[i];
			if(tempStyle > 0)
			{
				line_textStyle =    ( tempStyle & controlStyleCenterX ? OPT_CENTERX : 0)
									|(tempStyle & controlStyleCenterY ? OPT_CENTERY : 0)
									|(tempStyle & controlStyleRightX  ? OPT_RIGHTX  : 0);


				int8_t textFont = (((tempStyle >> 8) & 15)-1);
				textFont = (textFont>=0) ? textFont : 0;

				line_fontWidth = fonts[textFont].width;
				line_fontHeight = fonts[textFont].height;
				line_textFont =  fonts[textFont].handle;
			}

			// pozycja z uwzglednieniem stylu
			if(line_textStyle & OPT_CENTERX) line_posX = posX+(width/2);
			else if(line_textStyle & OPT_RIGHTX) line_posX = posX+width-10;
			else line_posX = posX+10;


			line_posY += (10+line_fontHeight);


			// wkoncu text
			API_CMD_TEXT(line_posX, line_posY, line_textFont, line_textStyle, data->multiLineText[i]);

/*
			if(style & controlStyleManualText)
			{
				int16_t tempX = posX, tempY = posY;
				int16_t txtLen = strlen(text);

				if(style & controlStyleCenterX)
				{
					tempX = posX-(txtLen*line_fontWidth)/2;
				}

				if(style & controlStyleCenterX)
				{
					tempY = posY + ((height/2) - line_fontHeight/2);
				}

				string2Bitmaps(tempX, tempY, data->multiLineText[i], txtLen);
			}
			else
			{
				API_CMD_TEXT(line_posX, line_posY, line_textFont, line_textStyle, data->multiLineText[i]);
			}
*/

		}

	}




    API_LIB_EndCoProList();


	return 0;
}

uint8_t cTextPopup::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cTextPopup::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
//
//void cTextPopup::string2Bitmaps(int16_t x, int16_t y, char* string, int8_t length)
//{
//	//y = y - fontHeight/2;
//	uint8_t strPtr = 0;
//
//	API_BITMAP_HANDLE(textFont);
//	API_BEGIN(BITMAPS);
//
//	for(uint8_t i = 0; i < length; i++)
//	{
//		if(x > 799 || y > 479 || x < 0 || y < 0)
//		{
//			strPtr++;
//		}
//		else if((x > 511 || y > 511) && string[strPtr] >=32)
//		{
//			API_CELL(string[strPtr++]);
//			API_VERTEX2F(x, y);
//		}
//		else if(string[strPtr] >=32)
//		{
//			API_VERTEX2II(x,y, textFont, (char)string[strPtr++]);
//		}
//		x+=fontWidth;
//	}
//
//	API_END();
//}
