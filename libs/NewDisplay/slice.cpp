
#include "FT812.h"

#include "displayControls.h"
#include "sliceControl.h"



static uint32_t defaultColors[] =
{
	0xFC6600, // linie
	0x0a0a0a, // tło
	one_true_red, // zaznaczenie
};

//--------------------------------------------------------------------------------
cSlicePoints::cSlicePoints(strControlProperties* properties): cDisplayControl(controlIdSlicePoints)
{
	colorsCount = 3;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		points = nullptr;
		value = 0;
		width = 0;
		height = 0;
		style = 0;
		return;
	}

	points = (strSlicePoints*)properties->data;

	posX = properties->x;
	posY = properties->y;

	text = properties->text;
	value = properties->value;

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cSlicePoints::~cSlicePoints()
{

}

//--------------------------------------------------------------------------------
void cSlicePoints::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cSlicePoints::setText(const char* text)
{
	this->text = text;
}

void cSlicePoints::setValue(int value)
{

}

void cSlicePoints::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cSlicePoints::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cSlicePoints::setData(void* data)
{
	points = (strSlicePoints*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cSlicePoints::update()
{
	if(points == nullptr) return 0;

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	API_COLOR(colors[0]);
	//API_LINE_WIDTH(8);
	API_LINE_WIDTH(8);

	uint16_t start = 0;

	for(uint8_t i = 0 ; i < points->sliceNumber ; i++)
	{
		if(points->sliceTab[i] >= 0)
		{
			start = points->sliceTab[i] + posX;

			if(points->selected == i) API_COLOR(colors[2]);
			else API_COLOR(colors[0]);
			//point
			API_BEGIN(LINES);
			API_VERTEX2F(start, posY);
			API_VERTEX2F(start, posY+height);
			API_END();
			//znaczek
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(start-3, posY-3);
			API_VERTEX2F(start+3, posY-3);
			API_VERTEX2F(start, posY);
			API_VERTEX2F(start-3, posY-3);
			API_END();
		}
	}

    API_LIB_EndCoProList();

	return 0;
}

uint8_t cSlicePoints::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cSlicePoints::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
