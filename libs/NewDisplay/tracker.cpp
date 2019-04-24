
#include "FT812.h"
#include "displayControls.h"

uint8_t cTracker::update()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	//API_COLOR(displayColors.fontPotLabel);
	//API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_TITLE_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), *(ptrPotsLabels+i));


    API_LIB_EndCoProList();



	return 1;
}
