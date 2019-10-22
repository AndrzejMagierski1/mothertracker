
#include "FT812.h"

#include "displayControls.h"
#include "commonControls.h"


static uint32_t defaultColors[] =
{
	0xFF0000, // ramka
	0x3F3F3F, // tło
	0x000000, // ramka
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cFrame::cFrame(strControlProperties* properties)
{
	colorsCount = 3;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		data = nullptr;
		width = 0;
		height = 0;
		style = 0;
		return;
	}

	posX = properties->x;
	posY = properties->y;

	text = properties->text;
	data = (strFrameData*)properties->data;

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cFrame::~cFrame()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cFrame::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);


	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cFrame::setText(char* text)
{
	this->text = text;
}

void cFrame::setValue(int value)
{
	this->value = value;
}

void cFrame::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cFrame::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cFrame::setData(void* data)
{
	this->data = (strFrameData*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cFrame::update()
{
	if(data == nullptr) return 0;

	API_LIB_BeginCoProListNoCheck();
	API_CMD_DLSTART();

	if(value > data->placesCount) value = data->placesCount-1;
	else if(value < 0) value = 0;

	uint8_t loopCount = 0;
	uint8_t foundActive = 0;

	if(data->multiSelActiveNum == 0)
	{
		loopCount = 1;
	}
	else
	{
		loopCount =  8;
	}

	for(uint8_t i = 0 ; i < loopCount; i++)
	{
		if((data->multisel[i].isActive) || (i == 0 && data->multiSelActiveNum  == 0))
		{
			if((foundActive == data->multiSelActiveNum) && (data->multiSelActiveNum != 0))
			{
				break;
			}

			foundActive++;
			uint8_t source;

			if(data->multiSelActiveNum)
			{
				source = data->multisel[i].frameNum;
			}
			else
			{
				source = value;
			}

			int16_t border_x = *(data->places[source]) + 1;
			int16_t border_y = *((data->places[source]) + 1) + 1;
			int16_t border_w = *((data->places[source]) + 2) - 2;
			int16_t border_h = *((data->places[source]) + 3) - 3;

			if(style & controlStyleCenterX)
			{
				//border_x = posX - (getTextWidth(FONT_INDEX_FROM_STYLE,text)/2 + 2);

				border_x = posX - (width/2);
			}


			if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
			else API_LINE_WIDTH(16);


			if(style & controlStyleBackground)
			{
				API_COLOR(colors[1]);

				API_BEGIN(RECTS);
				API_VERTEX2F(border_x, border_y);
				API_VERTEX2F(border_x+border_w, border_y+border_h);
				API_END();
			}

			API_COLOR(colors[0]);

			API_LINE_WIDTH(16);
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(border_x, border_y);
			API_VERTEX2F(border_x+border_w, border_y);
			API_VERTEX2F(border_x+border_w, border_y+border_h);
			API_VERTEX2F(border_x, border_y+border_h);
			API_VERTEX2F(border_x, border_y);
			API_END();



			//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

			//API_RESTORE_CONTEXT();
			//API_BLEND_FUNC(SRC_ALPHA, ZERO);
			//API_COLOR_A(128);

			//API_LINE_WIDTH(16);
			//API_COLOR(colors[0]);
			//if(text != nullptr) API_CMD_TEXT(posX, posY, textFont, textStyle, text);

		}

	}

	API_LIB_EndCoProList();

	return 0;
}

uint8_t cFrame::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cFrame::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
