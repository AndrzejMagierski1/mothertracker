
#include "FT812.h"

#include "displayControls.h"
#include "multiLabelControl.h"

#include <string.h>

void labelString2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);


static uint32_t defaultColors[] =
{
	0x000000, // tekst
	0x0a0a0a, // tło
	one_true_red, // zaznaczenie
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cMultiLabel::cMultiLabel(strControlProperties* properties): cDisplayControl(controlIdMultiLabel)
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

	data = (strMultiLabelData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cMultiLabel::~cMultiLabel()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cMultiLabel::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);


	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	fontWidth = fonts[textFont].width;
	fontHeight = fonts[textFont].height;
	textFont =  fonts[textFont].handle;
}

void cMultiLabel::setText(char* text)
{
	this->text = text;
}

void cMultiLabel::setValue(int value)
{
	this->value = value;
}

void cMultiLabel::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cMultiLabel::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cMultiLabel::setData(void* data)
{
	this->data = (strMultiLabelData*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cMultiLabel::update()
{

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    int16_t border_x = posX;//, border_y = posY-2;
    int16_t border_y = posY;
    int16_t lines_space = fontHeight;

    int16_t text_y = posY + data->yOffset;
    int16_t text_h = height - data->yOffset;

    if(data->textLinesCount > 0 && data->textLinesCount < 5 && height > fontHeight)
    {
    	lines_space = (text_h - fontHeight*data->textLinesCount) / (data->textLinesCount+1);
    }


	if(style & controlStyleCenterX)
	{
		border_x = posX - (width/2);
	}
	if(style & controlStyleCenterY)
	{
		border_y = posY - (height/2);
		text_y = text_y + lines_space + fontHeight/2;
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


		if(style & controlStyleBottomShadow)
		{
//			API_COLOR(colors[4]);
//			API_LINE_WIDTH(16);
//			API_BEGIN(RECTS);
//			API_VERTEX2F(posX, posY);
//			API_VERTEX2F(posX+width, posY+height-2);
//			API_END();

			API_BLEND_FUNC(SRC_ALPHA , ZERO);

			API_SAVE_CONTEXT();
			uint16_t grad_y = posY+height-10;
			uint16_t grad_h = 10;
			API_SCISSOR_XY(border_x-1, grad_y);
			API_SCISSOR_SIZE(width+2, grad_h);
			API_CMD_GRADIENT(0, grad_y, colors[1], 0, grad_y+grad_h, 0x0);
			API_RESTORE_CONTEXT();

			API_COLOR(0x000000);
			API_LINE_WIDTH(1);
			API_BEGIN(LINES);
			API_VERTEX2F(border_x-1, posY);
			API_VERTEX2F(border_x-1, posY+height);
			API_VERTEX2F(border_x+width+1, posY);
			API_VERTEX2F(border_x+width+1, posY+height);
			API_END();

			API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

		}

		if(style & controlStyleNoTransparency)
		{
			API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		}
	}
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


	//API_COLOR(colors[0]);


	for(uint8_t i = 0; i<data->textLinesCount; i++)
	{


/*
		if(style & controlStyleVerticalText)
		{
			API_SAVE_CONTEXT();

			API_BITMAP_HANDLE(textFont);
			API_BEGIN(BITMAPS);

			API_BITMAP_SIZE(NEAREST, BORDER, BORDER, fontHeight, fontHeight);

			API_CMD_LOADIDENTITY();
			API_CMD_TRANSLATE(65536 * (fontHeight/2), 65536 * (fontHeight/2));
			API_CMD_ROTATE(270*65536 / 360);
			API_CMD_TRANSLATE(65536 * (-fontHeight/2), 65536 * (-fontHeight/2));
			API_CMD_SETMATRIX();


			uint8_t strPtr = 0;
			int16_t x = posX, y = posY;
			int16_t txtLen = strlen(data->text[i]);

			if(style & controlStyleCenterX)
			{
				x-=fontHeight/2;
			}
			if(style & controlStyleCenterY)
			{

			}

			API_BITMAP_HANDLE(textFont);
			API_BEGIN(BITMAPS);

			for(uint8_t i = 0; i < txtLen; i++)
			{
				if(x > 799 || y > 479 || x < 0 || y < 0)
				{
					strPtr++;
				}
				else if((x > 511 || y > 511) && *(data->text[i]+(strPtr)) >=32)
				{
					API_CELL(*(data->text[i]+(strPtr++)));
					API_VERTEX2F(x, y);
				}
				else if(*(data->text[i]+(strPtr)) >=32)
				{
					API_VERTEX2II(x,y, textFont, *(data->text[i]+(strPtr++)));
				}

				y-=fontWidth;
			}

			API_END();
			API_RESTORE_CONTEXT();
		}
		else if(style & controlStyleManualText)
		{
			int16_t tempX = posX, tempY = posY;
			int16_t txtLen = strlen(data->text[i]);

			if(style & controlStyleCenterX)
			{
				tempX = posX-(txtLen*fontWidth)/2;
			}

			if(style & controlStyleCenterX)
			{
				tempY = posY + ((height/2) - fontHeight/2);
			}

			string2Bitmaps(tempX, tempY, data->text[i], txtLen);
		}
		else
*/		//{
		if(data->text[i] != nullptr &&  *data->text[i] != 0)
		{

			if(data->selected > 0 && data->selected-1 == i) API_COLOR(colors[2]);
			else API_COLOR(colors[0]);

			API_CMD_TEXT(posX+data->xOffset, text_y, textFont, textStyle, data->text[i]);
		}
		//}

		text_y = text_y+(lines_space+fontHeight);
	}

/*
	if(style & controlStyleShowBitmap)
	{
		// API_BITMAP_HANDLE(0);
		API_BITMAP_SOURCE(bitmaps[data->bitmapIndex].address);
		API_BITMAP_LAYOUT(bitmaps[data->bitmapIndex].format, bitmaps[data->bitmapIndex].linestride, bitmaps[data->bitmapIndex].height);
		API_BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmaps[data->bitmapIndex].width, bitmaps[data->bitmapIndex].height);

		API_BEGIN(BITMAPS);
		API_VERTEX2F(data->xValue-(bitmaps[data->bitmapIndex].width/2), data->yValue);
		API_END();

	}
*/
    API_LIB_EndCoProList();


	return 0;
}

uint8_t cMultiLabel::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cMultiLabel::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

void cMultiLabel::string2Bitmaps(int16_t x, int16_t y, char* string, int8_t length)
{
	//y = y - fontHeight/2;
	uint8_t strPtr = 0;

	API_BITMAP_HANDLE(textFont);
	API_BEGIN(BITMAPS);

	for(uint8_t i = 0; i < length; i++)
	{
		if(x > 799 || y > 479 || x < 0 || y < 0)
		{
			strPtr++;
		}
		else if((x > 511 || y > 511) && string[strPtr] >=32)
		{
			API_CELL(string[strPtr++]);
			//API_CMD_LOADIDENTITY();
			//API_CMD_ROTATE(90*65536 / 360);
			//API_CMD_SETMATRIX();
			API_VERTEX2F(x, y);

		}
		else if(string[strPtr] >=32)
		{
			API_VERTEX2II(x,y, textFont, (char)string[strPtr++]);
		}
		x+=fontWidth;
	}

	API_END();
}
