
#include "FT812.h"

#include "displayControls.h"
#include "notePopoutControl.h"



static uint32_t defaultColors[] =
{
	0x0a0a0a, // tlo kafelkow
	0xffffff, // tekst
	one_true_red, // ramka
};

void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cNotePopout::cNotePopout(strControlProperties* properties): cDisplayControl(controlIdNotePopup)
{
	colorsCount = 3;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		value = -1;
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

	padNames = (strPadNames*)properties->data;

	setStyle(properties->style);
	fillpadMap();
}

cNotePopout::~cNotePopout()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cNotePopout::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cNotePopout::setText(const char* text)
{
	this->text = text;
}

void cNotePopout::setValue(int value)
{
	this->value = value;
}

void cNotePopout::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cNotePopout::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cNotePopout::setData(void* data)
{
	padNames = (strPadNames*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cNotePopout::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	API_COLOR(colors[0]);
	API_BEGIN(RECTS);

//	char localString[10];

	for(uint8_t i = 0 ; i < PADMAP_SIZE; i++)
	{
		API_VERTEX2F(padMap[i].x, padMap[i].y);
		API_VERTEX2F(padMap[i].x + padMap[i].w , padMap[i].y + padMap[i].h);
	}

	API_END();
	API_COLOR(colors[1]);

	for(uint8_t i = 0 ; i < PADMAP_SIZE; i++)
	{
//		for(int j=0;j<padNames->length;j++)
//		{
//			localString[j] = padNames->name[i][j];
//		}

//		localString[4]=0; // 5 chararacter is max

		API_CMD_TEXT(padMap[i].x + padMap[i].w/2, padMap[i].y + padMap[i].h/2, textFont, OPT_CENTER, padNames->name[i]);
	}

	if(value >= 0)
	{
		API_COLOR(colors[2]);
		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);

		uint16_t localX, localY;
		localX = padMap[value].x -1;
		localY = padMap[value].y -1;
		API_VERTEX2F(localX, localY);
		localX = padMap[value].x + padMap[value].w +1;
		localY = padMap[value].y -1;
		API_VERTEX2F(localX, localY);
		localX = padMap[value].x + padMap[value].w + 1;
		localY = padMap[value].y + padMap[value].h + 1;
		API_VERTEX2F(localX, localY);
		localX = padMap[value].x - 1;
		localY = padMap[value].y + padMap[value].h + 1;
		API_VERTEX2F(localX, localY);
		localX = padMap[value].x -1;
		localY = padMap[value].y -1;
		API_VERTEX2F(localX, localY);
		API_END();
	}

    API_LIB_EndCoProList();

	return 0;
}

uint8_t cNotePopout::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cNotePopout::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

void cNotePopout::fillpadMap()
{
	uint16_t offset_x = width/12;
	uint16_t offset_y = height/4;

	for(uint8_t i = 0; i< PADMAP_SIZE; i++)
	{
		if(i >=0 && (i<=11))
		{
			if(i ==0) padMap[i].x = posX;
			else padMap[i].x = padMap[i-1].x + offset_x;

			padMap[i].y = posY;
			padMap[i].w = offset_x - offset_x * 0.0667;
			padMap[i].h = offset_y* 0.9330;
		}
		else if(i >= 12 && (i<=23))
		{
			if(i == 12) padMap[i].x = padMap[0].x;
			else padMap[i].x = padMap[i-1].x + offset_x;

			padMap[i].y = posY + offset_y;
			padMap[i].w = offset_x - offset_x*0.0667;
			padMap[i].h = offset_y* 0.9330;
		}
		else if((i >= 24) && (i <= 35))
		{
			if(i == 24) padMap[i].x = padMap[0].x;
			else padMap[i].x = padMap[i-1].x + offset_x;

			padMap[i].y = posY + 2*offset_y;
			padMap[i].w = offset_x - offset_x*0.0667;
			padMap[i].h = offset_y* 0.9330;
		}
		else if((i >= 36) && (i <= 47))
		{
			if(i == 36) padMap[i].x = padMap[0].x;
			else padMap[i].x = padMap[i-1].x + offset_x;

			padMap[i].y = posY + 3*offset_y;
			padMap[i].w = offset_x - offset_x*0.0667;
			padMap[i].h = offset_y* 0.9330;
		}
	}

}
