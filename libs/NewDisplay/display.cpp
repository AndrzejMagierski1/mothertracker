

#include "FT812.h"

#include "display.h"




#include "poly_logo_inv.h"
#include "Roboto_Regular_12_L4.h"
#include "Roboto_Regular_22_L4.h"


// CZCIONKI
#define MT_GPU_RAM_FONT1_ADRESS	1000
#define MT_GPU_RAM_FONT1_HANDLE	13
#define MT_GPU_RAM_FONT2_ADRESS	11000
#define MT_GPU_RAM_FONT2_HANDLE	14
// handle nie moze byc wikesze niz 14


// OBRAZY
#define MT_GPU_RAM_POLY_LOGO_ADRESS	50000
#define MT_GPU_RAM_POLY_LOGO_HANDLE	12

#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))



cDisplay display;



void cDisplay::begin()
{
	FT812_Init();

//####################################

	API_LIB_WriteDataRAMG(Roboto_Regular_12_L4, sizeof(Roboto_Regular_12_L4), MT_GPU_RAM_FONT1_ADRESS);
	API_LIB_WriteDataRAMG(Roboto_Regular_22_L4, sizeof(Roboto_Regular_22_L4), MT_GPU_RAM_FONT2_ADRESS);

	API_LIB_WriteDataRAMG(poly_logo_inv_128x128, sizeof(poly_logo_inv_128x128), MT_GPU_RAM_POLY_LOGO_ADRESS);

//####################################

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(DISP_RGB(0,0,0));
	API_CLEAR(1,1,1);

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BITMAP_SOURCE(-1924);
	API_BITMAP_LAYOUT(L4,6,16);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 12,16);
	API_CMD_SETFONT(MT_GPU_RAM_FONT1_HANDLE, MT_GPU_RAM_FONT1_ADRESS);

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BITMAP_SOURCE(1868);
	API_BITMAP_LAYOUT(L4,10,29);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 20,29);
	API_CMD_SETFONT(MT_GPU_RAM_FONT2_HANDLE, MT_GPU_RAM_FONT2_ADRESS);



	API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
    API_LIB_AwaitCoProEmpty();
}

void cDisplay::update()
{
	cDisplayControl* ptrControl = controlsTable[0];
	while(ptrControl++)
	{
		ptrControl->update();
	}
}

hControl cDisplay::createControl(uint16_t type, char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

}

void cDisplay::setControlState(hControl handle, uint8_t state)
{

}

void cDisplay::destroyControl(hControl handle)
{

}

void cDisplay::refreshControl(hControl handle)
{

}


