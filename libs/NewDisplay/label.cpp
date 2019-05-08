
#include "FT812.h"

#include "commonControls.h"


uint8_t cLabel::colorsCount = 1;
uint32_t cLabel::defaultColors[] =
{
	0xFFFFFF, // tekst
};

cLabel::cLabel(strControlProperties* properties)
{
	if(properties == nullptr)
	{
		cLabel(nullptr, 0,0,0,0);
		return;
	}

	style = properties->style;
	posX = properties->x;
	posY = properties->y;

	text = properties->text;

	cWidth = properties->w;
	cHeight = properties->h;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont =  font[((style >> 8) & 15)-1].handle;
}

cLabel::cLabel(char* text, uint16_t style, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	this->style = style;
	posX = x;
	posY = y;

	this->text = text;

	cWidth = w;
	cHeight = h;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont =  font[((style >> 8) & 15)-1].handle;
}

cLabel::~cLabel()
{

}

void cLabel::setStyle(uint16_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont =  font[((style >> 8) & 15)-1].handle;
}

void cLabel::setText(char* text)
{
	this->text = text;
}

uint8_t cLabel::update()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	API_BLEND_FUNC(SRC_ALPHA, ZERO);
	API_COLOR_A(128);



	API_COLOR(colors[0]);
	API_CMD_TEXT(posX, posY, textFont, textStyle, text);


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
	API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cLabel::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
