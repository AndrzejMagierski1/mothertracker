
#include "FT812.h"

#include "displayControls.h"
#include "commonControls.h"

#include <string.h>

void labelString2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, const char* string, int8_t length);


static uint32_t defaultColors[] =
{
	0x000000, // tekst
	0xFFFFFF, // tło
	one_true_red, // ramka
	0x000000, // tekst2
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cLabel::cLabel(strControlProperties* properties): cDisplayControl(controlIdLabel)
{
	colorsCount = 4;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		data = nullptr;
		width = 0;
		height = 0;
		style = 0;
		style2 = 0;
		value = 0;
		return;
	}

	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;



	posX = properties->x;
	posY = properties->y;

	text = properties->text;

	value = properties->value;

	data = (strLabelData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
	setStyle2(properties->style);
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

	textStyle1 =   (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font1 = &fonts[textFont];

	//fontWidth = fonts[textFont].width;
	//fontHeight = fonts[textFont].height;
	//textFont =  fonts[textFont].handle;
}


void cLabel::setStyle2(uint32_t style)
{
	this->style2 = style;

	textStyle2 =   (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font2 = &fonts[textFont];
}

void cLabel::setText(const char* text)
{
	this->text = text;
}

void cLabel::setText2(const char* text)
{
	this->text2 = text;
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
	this->data = (strLabelData*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cLabel::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

    int16_t border_x = posX;//, border_y = posY-2;
    int16_t border_y = posY;

    int16_t lines_step_y = 0;
    int16_t text_y = posY+10;
    //int16_t text_h = height;


    if(value > 0 && value < 2 && height > font1->height)
    {
    	//lines_step_y = (text_h - font1->height*(value+1)) / (value+2) + font1->height;
    	lines_step_y = 18;
    }

	if(style & controlStyleCenterX)
	{
		border_x = posX - (width/2);
	}
	if(style & controlStyleCenterY)
	{
		//border_y = posY - (height/2);

		if(value == 0)
		{
			text_y = posY + (height/2);
		}
		else
		{
			text_y = posY + ((lines_step_y*value)/2);
		}
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
			API_BLEND_FUNC(SRC_ALPHA , ZERO);

			API_SAVE_CONTEXT();
			uint16_t grad_y = border_y+height-10;
			uint16_t grad_h = 10;
			API_SCISSOR_XY(border_x-1, grad_y);
			API_SCISSOR_SIZE(width+2, grad_h);
			API_CMD_GRADIENT(0, grad_y, colors[1], 0, grad_y+grad_h, 0x0);
			API_RESTORE_CONTEXT();

			API_COLOR(0x000000);
			API_LINE_WIDTH(1);
			API_BEGIN(LINES);
			API_VERTEX2F(border_x-1, border_y);
			API_VERTEX2F(border_x-1, border_y+height);
			API_VERTEX2F(border_x+width+1, border_y);
			API_VERTEX2F(border_x+width+1, border_y+height);
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

		API_LINE_WIDTH(8);

		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x , border_y);
		API_VERTEX2F(border_x+width , border_y);
		API_VERTEX2F(border_x+width , border_y+height);
		API_VERTEX2F(border_x , border_y+height);
		API_VERTEX2F(border_x , border_y);
		API_END();
/*
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x-1 , border_y-1);
		API_VERTEX2F(border_x+width+1 , border_y-1);
		API_VERTEX2F(border_x+width+1 , border_y+height+1);
		API_VERTEX2F(border_x-1 , border_y+height+1);
		API_VERTEX2F(border_x-1 , border_y-1);
		API_END();
*/
	}


	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

	//API_RESTORE_CONTEXT();
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

	//API_LINE_WIDTH(16);

	API_COLOR(colors[0]);


	if(style & controlStyleVerticalText)
	{
		API_SAVE_CONTEXT();

		API_BITMAP_HANDLE(font1->handle);
		API_BEGIN(BITMAPS);

		API_BITMAP_SIZE(NEAREST, BORDER, BORDER, font1->height, font1->height);

		API_CMD_LOADIDENTITY();
		API_CMD_TRANSLATE(65536 * (font1->height/2), 65536 * (font1->height/2));
		API_CMD_ROTATE(270*65536 / 360);
		API_CMD_TRANSLATE(65536 * (-font1->height/2), 65536 * (-font1->height/2));
		API_CMD_SETMATRIX();


		uint8_t strPtr = 0;
		int16_t x = posX, y = posY;
		int16_t txtLen = strlen(text);

		if(style & controlStyleCenterX)
		{
			x-=font1->height/2;
		}
		if(style & controlStyleCenterY)
		{

		}

		API_BITMAP_HANDLE(font1->handle);
		API_BEGIN(BITMAPS);

		for(uint8_t i = 0; i < txtLen; i++)
		{
			if(x > 799 || y > 479 || x < 0 || y < 0)
			{
				strPtr++;
			}
			else if((x > 511 || y > 511) && text[strPtr] >=32)
			{
				API_CELL(text[strPtr++]);
				API_VERTEX2F(x, y);
			}
			else if(text[strPtr] >=32)
			{
				API_VERTEX2II(x,y, font1->handle, (char)text[strPtr++]);
			}

			y-=font1->width;
		}

		API_END();
		API_RESTORE_CONTEXT();
	}
	else if(style & controlStyleManualText)
	{
		if(text != nullptr)
		{
			int16_t tempX = posX, tempY = posY;
			int16_t txtLen = strlen(text);

			if(style & controlStyleCenterX)
			{
				tempX = posX-(txtLen*font1->height)/2;
			}

			if(style & controlStyleCenterX)
			{
				tempY = posY + ((height/2) - font1->height/2);
			}

			string2Bitmaps(tempX, tempY, text, txtLen);
		}
	}
	else
	{
		if(text != nullptr) API_CMD_TEXT(posX, text_y, font1->handle, textStyle1, text);
	}


	if(value > 0 && text2 != nullptr && text2[0] != 0)
	{
		text_y = text_y+lines_step_y;

		API_COLOR(colors[3]);
		API_CMD_TEXT(posX, text_y, font2->handle, textStyle2, text2);
	}


/*-
	if(style & controlStyleShowValue)
	{
		API_CMD_NUMBER(posX+data->xValue, posY+data->yValue, textFont, data->styleValue, value);
	}
	else
*/
	if(style & controlStyleShowBitmap && data != nullptr)
	{
		for(uint8_t i = 0; i<labelBitmapsCount; i++)
		{
			if(data->bitmaps[i].bitmapIndex)
			{
				//API_SAVE_CONTEXT();

				API_CMD_SETBITMAP(	bitmaps[data->bitmaps[i].bitmapIndex].address,
									bitmaps[data->bitmaps[i].bitmapIndex].format,
									bitmaps[data->bitmaps[i].bitmapIndex].width,
									bitmaps[data->bitmaps[i].bitmapIndex].height);

/*				API_BITMAP_HANDLE(0);
				API_BITMAP_LAYOUT(	bitmaps[data->bitmaps[i].bitmapIndex].format,
									bitmaps[data->bitmaps[i].bitmapIndex].linestride,
									bitmaps[data->bitmaps[i].bitmapIndex].height);

				API_BITMAP_SIZE(NEAREST, BORDER, BORDER,
						bitmaps[data->bitmaps[i].bitmapIndex].width,
						bitmaps[data->bitmaps[i].bitmapIndex].height);

				API_BITMAP_SOURCE(bitmaps[data->bitmaps[i].bitmapIndex].address);
*/

				API_BEGIN(BITMAPS);
				API_VERTEX2F(data->bitmaps[i].xValue-(bitmaps[data->bitmaps[i].bitmapIndex].width/2), data->bitmaps[i].yValue);
				API_END();

				//API_RESTORE_CONTEXT();
			}
		}
	}


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

void cLabel::string2Bitmaps(int16_t x, int16_t y, const char* string, int8_t length)
{
	//y = y - fontHeight/2;
	uint8_t strPtr = 0;

	API_BITMAP_HANDLE(font1->handle);
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
			API_VERTEX2II(x,y, font1->handle, (char)string[strPtr++]);
		}
		x+=font1->width;
	}

	API_END();
}
