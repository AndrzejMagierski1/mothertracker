
#include "FT812.h"

#include "displayControls.h"
#include "processingPopout.h"
#include "Arduino.h"



static uint32_t defaultColors[] =
{
	0x0a0a0a, // ramka okna
	0xFFFFFF  // tekst
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cProcessingPop::cProcessingPop(strControlProperties* properties)
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

	memset(localBaseText, 0, sizeof(localBaseText));
	memset(localModifiedText, 0, sizeof(localModifiedText));
}

cProcessingPop::~cProcessingPop()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cProcessingPop::setStyle(uint32_t style)
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

void cProcessingPop::setText(char* text)
{
	this->text = text;
	memset(localBaseText,0,strlen(localBaseText));
	memcpy(localBaseText,text,strlen(text));
	dots = 0;
}

void cProcessingPop::setValue(int value)
{
	this->value = value;
}

void cProcessingPop::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cProcessingPop::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cProcessingPop::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cProcessingPop::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    uint16_t frameWidth = width;
    uint16_t frameX = posX; //(width > 800/8) ? posX+5 : posX + width/2;

    uint16_t frameHeight = height;
    uint16_t frameY = posY;

    uint16_t textY = posY + (height / 2);

    dots = dots % 4;

    API_COLOR(colors[0]);

    API_LINE_WIDTH(16);
    API_BEGIN(RECTS);
    API_VERTEX2F(frameX+1, frameY+1);
    API_VERTEX2F(frameX+frameWidth-1, frameY+frameHeight-1);
    API_END();

    API_COLOR(colors[1]);

    memset(localModifiedText, 0, strlen(localModifiedText));
	strcpy(localModifiedText, localBaseText);

	uint16_t text_offset = strlen(localModifiedText) * (fontWidth/2);

    switch(dots)
    {
    case 0:
    	break;
    case 1:
    	strcat(localModifiedText, ".");
    	break;
    case 2:
    	strcat(localModifiedText, "..");
    	break;
    case 3:
    	strcat(localModifiedText, "...");
    	break;
    }

    dots++;

    string2Bitmaps((400 - text_offset), textY-10, localModifiedText, strlen(localModifiedText));

    API_LIB_EndCoProList();

	return 0;
}

uint8_t cProcessingPop::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cProcessingPop::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

void cProcessingPop::string2Bitmaps(int16_t x, int16_t y, char* string, int8_t length)
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
