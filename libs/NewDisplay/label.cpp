
#include "FT812.h"
#include "displayControls.h"


uint8_t cLabel::colorsCount = 1;
uint32_t cLabel::defaultColors[] =
{
	0xFFFFFF, // tekst
};


cLabel::cLabel(char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	cState = state;
	posX = x;
	posY = y;

	cText = text;

	cWidth = 2;
	cHeight = h;

}

cLabel::~cLabel()
{

}


uint8_t cLabel::update()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_COLOR(colors[0]);
	API_CMD_TEXT(posX, posY, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), cText);


    API_LIB_EndCoProList();


	return 1;
}
