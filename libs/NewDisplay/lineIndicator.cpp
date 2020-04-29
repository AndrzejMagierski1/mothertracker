#include "lineIndicatorControl.h"
#include "FT812.h"

cLineIndicator::cLineIndicator(strControlProperties* properties)
	:cProgressCursor(properties)
{
	lineIndicatorSelection = (strLineIndicatorSelection *) properties->data;
}
cLineIndicator::~cLineIndicator()
{

}
uint8_t cLineIndicator::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();


    if(lineIndicatorSelection != nullptr)
    {
    	if(((lineIndicatorSelection->start >= 0) && (lineIndicatorSelection->start  <= width)) &&
    	   ((lineIndicatorSelection->end >= 0) && (lineIndicatorSelection->end <= width)))
    	{
        	API_COLOR(colors[0]);
        	API_BLEND_FUNC(SRC_ALPHA, ONE);
        	API_LINE_WIDTH(16);
        	API_BEGIN(RECTS);
        	API_VERTEX2F(lineIndicatorSelection->start, posY);
        	API_VERTEX2F(lineIndicatorSelection->end, posY+height-2);
        	API_END();
        	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
    	}
    }

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

void cLineIndicator::setData(void* data)
{
	lineIndicatorSelection = (strLineIndicatorSelection *) data;
}

