#include "lineIndicatorControl.h"
#include "FT812.h"

cLineIndicator::cLineIndicator(strControlProperties* properties)
	:cProgressCursor(properties)
{

}
cLineIndicator::~cLineIndicator()
{

}
uint8_t cLineIndicator::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	if(value >= 0 && value <= width)
	{
		API_COLOR(colors[1]);
		API_BEGIN(LINES);
		API_VERTEX2F( posX + value, posY);
		API_VERTEX2F( posX + value, posY + height);
		API_END();
	}

	API_LIB_EndCoProList();
	return 0;
}

