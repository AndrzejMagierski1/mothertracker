
#include "FT812.h"

#include "displayControls.h"
#include "pointsControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // linie
	0x111111, // tło
	0xFF0000, // zaznaczenie
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cPoints::cPoints(strControlProperties* properties)
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

	points = (strTrackerPoints*)properties->data;

	posX = properties->x;
	posY = properties->y;

	text = properties->text;
	value = properties->value;

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cPoints::~cPoints()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cPoints::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cPoints::setText(char* text)
{
	this->text = text;
}

void cPoints::setValue(int value)
{

}

void cPoints::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cPoints::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cPoints::setData(void* data)
{
	points = (strTrackerPoints*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cPoints::update()
{
	if(points == nullptr) return 0;

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	// linie start end
	API_COLOR(colors[0]);
	//API_LINE_WIDTH(8);
	API_LINE_WIDTH(16);

	if(points->startPoint >= 0)
	{
		uint16_t start = points->startPoint+posX;

		if(points->selected == 1) API_COLOR(colors[2]);
		// start point
		API_BEGIN(LINES);
		API_VERTEX2F(start, posY);
		API_VERTEX2F(start, posY+height);
		API_END();
		//znaczek
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(start+1, posY);
		API_VERTEX2F(start+5, posY+3);
		API_VERTEX2F(start+1, posY+6);
		API_END();

		if(points->selected == 1) API_COLOR(colors[0]);
	}

	if(points->endPoint >= 0)
	{
		uint16_t end = points->endPoint+posX;

		if(points->selected == 2) API_COLOR(colors[2]);
		// end point
		API_BEGIN(LINES);
		API_VERTEX2F(end, posY);
		API_VERTEX2F(end, posY+height);
		API_END();
		//znaczek
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(end-1, posY);
		API_VERTEX2F(end-5, posY+3);
		API_VERTEX2F(end-1, posY+6);
		API_END();

		if(points->selected == 2) API_COLOR(colors[0]);
	}

	if(points->pointsType == 0)
	{
		uint16_t start = points->startPoint + posX;
		uint16_t end = points->endPoint + posX;
		//tlo start - end
		API_COLOR(colors[1]);
		API_BLEND_FUNC(SRC_ALPHA, ONE);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F((points->startPoint >= 0) ? start : posX, posY);
		API_VERTEX2F((points->endPoint >= 0) ? end : posX+width, posY+height);
		API_END();
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	}
	if(points->pointsType > 0 && (points->loopPoint1 >= 0 || points->loopPoint2 >= 0))
	{
		uint16_t loop1 = points->loopPoint1+posX;
		uint16_t loop2 = points->loopPoint2+posX;

		// tlo loop
		API_COLOR(colors[1]);
		API_BLEND_FUNC(SRC_ALPHA, ONE);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F((points->loopPoint1 >= 0) ? loop1 : posX, posY);
		API_VERTEX2F((points->loopPoint2 >= 0) ? loop2 : posX+width, posY+height);
		API_END();
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		//API_RESTORE_CONTEXT();

		API_LINE_WIDTH(16);

		if(points->loopPoint1 >= 0)
		{
			if(points->selected == 3) API_COLOR(colors[2]);
			else API_COLOR(colors[0]);

			// loop point 1
			API_BEGIN(LINES);
			API_VERTEX2F(loop1, posY);
			API_VERTEX2F(loop1, posY+height);
			API_END();
			// znaczek loop
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(loop1+1, posY+height-6);
			API_VERTEX2F(loop1+5, posY+height-3);
			API_VERTEX2F(loop1+1, posY+height);
			API_END();

			if(points->selected == 3) API_COLOR(colors[0]);
		}
		if(points->loopPoint2 >= 0)
		{
			if(points->selected == 4) API_COLOR(colors[2]);
			else API_COLOR(colors[0]);

			// loop point 2
			API_BEGIN(LINES);
			API_VERTEX2F(loop2, posY);
			API_VERTEX2F(loop2, posY+height);
			API_END();
			// znaczek loop
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(loop2-1, posY+height-6);
			API_VERTEX2F(loop2-5, posY+height-3);
			API_VERTEX2F(loop2-1, posY+height);
			API_END();
		}
	}

    API_LIB_EndCoProList();


	return 0;
}

uint8_t cPoints::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cPoints::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
