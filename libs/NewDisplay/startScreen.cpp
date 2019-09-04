
#include "FT812.h"

#include "displayControls.h"
#include "startScreenControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0xFFFFFF, // ramka
	0x222222, // tło
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cStartScreen::cStartScreen(strControlProperties* properties)
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

	data = (strStartScreenData*)(properties->data);

	setStyle(properties->style);
}

cStartScreen::~cStartScreen()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cStartScreen::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cStartScreen::setText(char* text)
{
	this->text = text;
}

void cStartScreen::setValue(int value)
{
	this->value = value;
}

void cStartScreen::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cStartScreen::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cStartScreen::setData(void* data)
{
	data = (strStartScreenData*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cStartScreen::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();


	#define bitmap_index 0

   // API_BITMAP_HANDLE(0);
	API_BITMAP_SOURCE(bitmaps[bitmap_index].address);
	API_BITMAP_LAYOUT(bitmaps[bitmap_index].format, bitmaps[bitmap_index].linestride, bitmaps[bitmap_index].height);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmaps[bitmap_index].width, bitmaps[bitmap_index].height);

	API_BEGIN(BITMAPS);
	API_VERTEX2F((800/2)-(bitmaps[bitmap_index].width/2), 150);
	API_END();


    uint16_t barWidth = (5*width)/7;
    uint16_t barX = posX+ (width/7);

    uint16_t barHeight = (2*height)/7 ;
    uint16_t barY = posY + ((3*height)/7);

    uint16_t textX = posX + (width/2);
    uint16_t textY = posY + ((2*height)/7);

    if(value < 0) value = 0;
    else  if(value > 100) value = 100;

	uint16_t barFillY = (barWidth * value) / 100;


	API_COLOR(colors[1]);

	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);
	API_VERTEX2F(barX+1, barY+1);
	API_VERTEX2F(barX+barFillY-1, barY+barHeight-1);
	API_END();

	API_COLOR(colors[1]);

	API_LINE_WIDTH(16);
	API_BEGIN(LINE_STRIP);
	API_VERTEX2F(barX, barY);
	API_VERTEX2F(barX+barWidth, barY);
	API_VERTEX2F(barX+barWidth, barY+barHeight);
	API_VERTEX2F(barX, barY+barHeight);
	API_VERTEX2F(barX, barY);
	API_END();

	API_COLOR(colors[0]);

	if(text != nullptr) API_CMD_TEXT(textX, textY, textFont, OPT_CENTER, text);


	if(data != nullptr) API_CMD_TEXT((800/8)*0+(800/16), 465, textFont, OPT_CENTER, data->versionLabel);


	API_CMD_TEXT((800/8)*7+(800/16), 465, textFont, OPT_CENTER, (char*)"Hide");











    API_LIB_EndCoProList();


	return 0;
}

uint8_t cStartScreen::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cStartScreen::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
