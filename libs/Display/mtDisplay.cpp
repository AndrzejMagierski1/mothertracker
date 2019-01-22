
#include <stdint.h>
#include "mtStructs.h"
#include "Arduino.h"

#include "poly_logo_inv.h"

#define font_def 1

#if font_def == 1
#include "Roboto_Regular_12_L4.h"
#include "Roboto_Regular_22_L4.h"
#elif font_def == 2
#include "UnicaPro_Regular_12_L4.h"
#include "UnicaPro_Regular_22_L4.h"
#elif font_def == 3
#include "OpenSans-Regular_12_L4.h"
#include "OpenSans-Regular_22_L4.h"
#endif

#include "mtDisplay.h"


cMtDisplay mtDisplay;


//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void cMtDisplay::begin(uint8_t mode)
{
	FT812_Init();

//####################################

#if font_def == 1
	API_LIB_WriteDataRAMG(Roboto_Regular_12_L4, sizeof(Roboto_Regular_12_L4), MT_GPU_RAM_FONT1_ADRESS);
	API_LIB_WriteDataRAMG(Roboto_Regular_22_L4, sizeof(Roboto_Regular_22_L4), MT_GPU_RAM_FONT2_ADRESS);
#elif font_def == 2
	API_LIB_WriteDataRAMG(UnicaPro_Regular_12_L4, sizeof(UnicaPro_Regular_12_L4), 1000);
	API_LIB_WriteDataRAMG(UnicaPro_Regular_22_L4, sizeof(UnicaPro_Regular_22_L4), 11000);
#elif font_def == 3
	API_LIB_WriteDataRAMG(Sans_Regular_12_L4, sizeof(Sans_Regular_12_L4), 1000);
	API_LIB_WriteDataRAMG(Sans_Regular_22_L4, sizeof(Sans_Regular_22_L4), 11000);
#endif

	API_LIB_WriteDataRAMG(poly_logo_inv_128x128, sizeof(poly_logo_inv_128x128), MT_GPU_RAM_POLY_LOGO_ADRESS);

//####################################

    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(DISP_RGB(0,0,0));
	API_CLEAR(1,1,1);

#if font_def == 1

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

#elif font_def == 2

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BITMAP_SOURCE(-2308);
	API_BITMAP_LAYOUT(L4,6,18);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 12,18);
	API_CMD_SETFONT(MT_GPU_RAM_FONT1_HANDLE, MT_GPU_RAM_FONT1_ADRESS);

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BITMAP_SOURCE(588);
	API_BITMAP_LAYOUT(L4,10,33);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 20,33);
	API_CMD_SETFONT(MT_GPU_RAM_FONT2_HANDLE, MT_GPU_RAM_FONT2_ADRESS);

#elif font_def == 3

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BITMAP_SOURCE(-1924);
	API_BITMAP_LAYOUT(L4,6,16);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 12,16);
	API_CMD_SETFONT(MT_GPU_RAM_FONT1_HANDLE, MT_GPU_RAM_FONT1_ADRESS);

	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BITMAP_SOURCE(1548);
	API_BITMAP_LAYOUT(L4,10,30);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 20,30);
	API_CMD_SETFONT(MT_GPU_RAM_FONT2_HANDLE, MT_GPU_RAM_FONT2_ADRESS);

#endif


/*	API_BITMAP_HANDLE(MT_GPU_RAM_POLY_LOGO_HANDLE);
	API_BITMAP_SOURCE(MT_GPU_RAM_POLY_LOGO_ADRESS);
	API_BITMAP_LAYOUT(L4,128,128);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 128,128);
*/

	API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
    API_LIB_AwaitCoProEmpty();

    for(uint8_t i = 0; i < MT_DISP_LISTS_MAX; i++)
    {
    	lists[i].setSeflRefreshPtr(&displayRefreshTable.lists[i], &screenAnimation);
    }

	screenMode = mode;
	screenRefresh = 1;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void cMtDisplay::updateDisplay()
{
	// skopiwoanie wczesniej utworzeonej display listy do pamieci ram_g ukladu graficznego
	// + wyswietlenie nowego ekranu
	if(updateStep > 0)
	{
		if(!API_LIB_IsCoProEmpty()) return;

		API_LIB_AwaitCoProEmpty();

		uint16_t dlOffset = EVE_MemRead32(REG_CMD_DL);

		API_LIB_BeginCoProList();

		API_CMD_MEMCPY(updateAdress, RAM_DL, dlOffset);
		*updateSize = dlOffset;

		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		updateStep = 0;
		screenRefresh = 1;

		switch(screenMode)
		{
		case mtDisplayModeNormal: 	dl_load_normal_main(); 		break;
		default : 					dl_load_blank_main(); 	break;
		}


		return;
	}

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	if(screenMode == mtDisplayModePrint)
	{
		if(printTimer > PRINT_SHOW_TIME)
		{
			displayPrint = 0;
			screenMode = modeSave;
			screenRefresh = 1;
			printClear();
		}
	}
	//-------------------------------------------------
	//-------------------------------------------------
	// sprawdzanei czy wymagane jest odswiezanie
	if(!screenRefresh && (!screenAnimation || animationTimer < MT_DISP_ANIMATION_T)) return;

	if(!API_LIB_IsCoProEmpty()) return;

	displayPrint = 0;
	animationTimer = 0;
	screenRefresh = 0;
	//-------------------------------------------------
	//-------------------------------------------------

	switch(screenMode)
	{
		//-------------------------------------------------
		//-------------------------------------------------
		case mtDisplayModeNormal:
		{
			normalModeDisplayRefresh();

			break;
		}
		//-------------------------------------------------
		//-------------------------------------------------
		case mtDisplayModePolyLogo:
		{
			dl_load_poly_logo_main();

			break;
		}
		//-------------------------------------------------
		//-------------------------------------------------
		case mtDisplayModePrint:
		{
			dl_load_print_main();

			break;
		}
		//-------------------------------------------------
		//-------------------------------------------------
		default:
		{
			dl_load_blank_main();

			break;
		}
	}

}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//HAPTIC

void cMtDisplay::updateHaptic()
{
	if(mtHaptic.enabled)
	{
		if(mtHaptic.active == 1)
		{
			EVE_MemWrite8(REG_VOL_SOUND,mtHaptic.params.amplitude); //set the volume
			EVE_MemWrite16(REG_SOUND, (mtHaptic.params.pitch << 8) | mtHaptic.params.effect);
			EVE_MemWrite8(REG_PLAY, 1); // play the sound

			mtHaptic.active = 2;
		}
		else if(mtHaptic.active > 1 && mtHaptic.timer > mtHaptic.params.time)
		{
			EVE_MemWrite16(REG_SOUND, 0);
			EVE_MemWrite8(REG_PLAY, 1);

			mtHaptic.active = 0;
		}
	}

}

//#########################################################################################################
//###################################### metody glowne trybow pracy #######################################
//#########################################################################################################
void cMtDisplay::dl_load_blank_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayColors.bgColor);
	API_CLEAR(1,1,1);

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
}

//---------------------------------------------------------------------------
void cMtDisplay::dl_load_poly_logo_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayColors.bgColor);
	API_CLEAR(1,1,1);


	API_BEGIN(BITMAPS);
	API_BITMAP_SOURCE(MT_GPU_RAM_POLY_LOGO_ADRESS);
	API_BITMAP_LAYOUT(L4,64,128);
	API_BITMAP_SIZE(NEAREST, BORDER, BORDER, 128,128);
	API_VERTEX2II((480/2)-64,0,0,0);
	API_END();


    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();

}


//#########################################################################################################
//################################ metody publiczne zarzadzania ekranem ###################################
//#########################################################################################################
void cMtDisplay::setMode(uint8_t mode)
{
	if(screenMode == mtDisplayModePrint)
	{
		modeSave = mode;
		screenRefresh = 1;
		return;
	}
	screenMode = mode;
	screenRefresh = 1;
}



