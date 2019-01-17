
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

    fManagerList.setSeflRefreshPtr(&displayRefreshTable.fManager.rootList, &screenAnimation);
    instrumentEditorSampleList.setSeflRefreshPtr(&displayRefreshTable.instrumentEditor.sampleList, &screenAnimation);

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

		case mtDisplayModeBlank: 				dl_load_blank_main(); break;
		case mtDisplayModeBlocks5: 				dl_load_block_main(); break;
		case mtDisplayModeFileManager: 			dl_load_fmanager_main(); break;
		case mtDisplayModeInstrumentEditor: 	dl_load_instrument_editor_main(); break;

		default : dl_load_blank_main(); break;
		}


		return;
	}

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//HAPTIC
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
	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
/*	// TOUCH SCREEN
	int32_t temp_touch_xy;
	int16_t temp_touch_x;
	int16_t temp_touch_y;
	//-------------------------------------------------
	temp_touch_xy = EVE_MemRead32(REG_CTOUCH_TOUCH0_XY);
	temp_touch_y = temp_touch_xy & 0xFFFF;
	temp_touch_x = (temp_touch_xy >> 16) & 0xFFFF;
	if(touchPanel[0].y !=  temp_touch_y ||  touchPanel[0].x != temp_touch_x)
	{
		screenRefresh = 1;
		touchPanel[0].y = temp_touch_y;
		touchPanel[0].x = temp_touch_x;

		if(temp_touch_y > 100 && temp_touch_x < 96) onButtonPress(0);
		else if(temp_touch_y > 100 && temp_touch_x > 96*1 && temp_touch_x < 96*2) onButtonPress(1);
		else if(temp_touch_y > 100 && temp_touch_x > 96*2 && temp_touch_x < 96*3) onButtonPress(2);
		else if(temp_touch_y > 100 && temp_touch_x > 96*3 && temp_touch_x < 96*4) onButtonPress(3);
		else if(temp_touch_y > 100 && temp_touch_x > 96*4 && temp_touch_x < 96*5) onButtonPress(4);

	}
	//-------------------------------------------------
	temp_touch_xy = EVE_MemRead32(REG_CTOUCH_TOUCH1_XY);
	temp_touch_y = temp_touch_xy & 0xFFFF;
	temp_touch_x = (temp_touch_xy >> 16) & 0xFFFF;
	if(touchPanel[1].y !=  temp_touch_y ||  touchPanel[1].x != temp_touch_x)
	{
		screenRefresh = 1;
		touchPanel[1].y = temp_touch_y;
		touchPanel[1].x = temp_touch_x;
	}
	//-------------------------------------------------
	temp_touch_xy = EVE_MemRead32(REG_CTOUCH_TOUCH2_XY);
	temp_touch_y = temp_touch_xy & 0xFFFF;
	temp_touch_x = (temp_touch_xy >> 16) & 0xFFFF;
	if(touchPanel[2].y !=  temp_touch_y ||  touchPanel[2].x != temp_touch_x)
	{
		screenRefresh = 1;
		touchPanel[2].y = temp_touch_y;
		touchPanel[2].x = temp_touch_x;
	}
	//-------------------------------------------------
	temp_touch_xy = EVE_MemRead32(REG_CTOUCH_TOUCH3_XY);
	temp_touch_y = temp_touch_xy & 0xFFFF;
	temp_touch_x = (temp_touch_xy >> 16) & 0xFFFF;
	if(touchPanel[3].y !=  temp_touch_y ||  touchPanel[3].x != temp_touch_x)
	{
		screenRefresh = 1;
		touchPanel[3].y = temp_touch_y;
		touchPanel[3].x = temp_touch_x;
	}
	//-------------------------------------------------
	temp_touch_x = EVE_MemRead32(REG_CTOUCH_TOUCH4_X) & 0xFFFF;
	temp_touch_y = EVE_MemRead32(REG_CTOUCH_TOUCH4_Y) & 0xFFFF;
	if(touchPanel[4].y !=  temp_touch_y ||  touchPanel[4].x != temp_touch_x)
	{
		screenRefresh = 1;
		touchPanel[4].y = temp_touch_y;
		touchPanel[4].x = temp_touch_x;
	}
*/
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
		if(!screenRefresh
			&& (!screenAnimation || animationTimer < MT_DISP_ANIMATION_T)
			) return;
	/*
	if(!screenRefresh
		&&
		) return;
	if(!screenAnimation) if(animationTimer > MT_DISP_ANIMATION_T) return;
	if(!displayPrint) return;
	*/

	if(!API_LIB_IsCoProEmpty()) return;

	displayPrint = 0;
	animationTimer = 0;
	screenRefresh = 0;
	//-------------------------------------------------
	//-------------------------------------------------

	switch(screenMode)
	{
		case mtDisplayModeBlank:
		{
			dl_load_blank_main();

			break;
		}
		//-------------------------------------------------
		//-------------------------------------------------
		case mtDisplayModeBlocks5:
		{
			for(uint8_t i = 0; i < 5; i++)
			{
				if(displayRefreshTable.block[i].center)
				{
					displayRefreshTable.block[i].center = 0;
					ramg_blocks_center(i);
					if(updateStep > 0) return;
				}
			}
			//-------------------------------------------------
			for(uint8_t i = 0; i < 5; i++)
			{
				if(displayRefreshTable.block[i].title)
				{
					displayRefreshTable.block[i].title = 0;
					ramg_blocks_title(i);
				}
			}
			//-------------------------------------------------
			for(uint8_t i = 0; i < 5; i++)
			{
				if(displayRefreshTable.block[i].label)
				{
					displayRefreshTable.block[i].label = 0;
					ramg_blocks_label(i);
				}
			}
			//-------------------------------------------------
			dl_load_block_main();

			break;
		}
		//-------------------------------------------------
		//-------------------------------------------------
		case mtDisplayModeInstrumentEditor:
		{
			//-------------------------------------------------
/*			if(displayRefreshTable.instrumentEditor.background)
			{
				displayRefreshTable.instrumentEditor.background = 0;
				ramg_instrument_editor_background();
				if(updateStep > 0) return;
			}
*/			//-------------------------------------------------
			if(displayRefreshTable.instrumentEditor.points)
			{
				displayRefreshTable.instrumentEditor.points = 0;
				ramg_instrument_editor_points();
				if(updateStep > 0) return;
			}
			//-------------------------------------------------
			if(displayRefreshTable.instrumentEditor.spectrum)
			{
				displayRefreshTable.instrumentEditor.spectrum = 0;
				ramg_instrument_editor_spectrum();
				if(updateStep > 0) return;
			}
			//-------------------------------------------------
			if(displayRefreshTable.instrumentEditor.labels)
			{
				displayRefreshTable.instrumentEditor.labels = 0;
				ramg_instrument_editor_labels();
				if(updateStep > 0) return;
			}
			if(displayRefreshTable.instrumentEditor.sampleList)
			{
				displayRefreshTable.instrumentEditor.sampleList = 0;
				if(instrumentEditorSampleList.enabled())
				{
					instrumentEditorSampleList.update();
				    updateAdress = MT_GPU_RAM_INSTRUMENT_EDITOR_SAMPLE_ADRESS;
					updateSize = &instrumentEditor.ramSampleListSize;
					updateStep = 1;
					return;
				}
			}
			//-------------------------------------------------
			dl_load_instrument_editor_main();

			break;
		}
		//-------------------------------------------------
		//-------------------------------------------------
		case mtDisplayModeFileManager:
		{
			if(displayRefreshTable.fManager.rootList)
			{
				displayRefreshTable.fManager.rootList = 0;
				if(fManagerList.enabled())
				{
					fManagerList.update();
				    updateAdress = MT_GPU_RAM_FMANAGER_ROOTLIST_ADRESS;
					updateSize = &displayFmanager.ramRootListSize;
					updateStep = 1;
					return;
				}
			}
			if(displayRefreshTable.fManager.rootTitle)
			{
				displayRefreshTable.fManager.rootTitle = 0;
				ramg_fmanager_roottitle();
				if(updateStep > 0) return;
			}
			if(displayRefreshTable.fManager.labels)
			{
				displayRefreshTable.fManager.labels = 0;
				ramg_fmanager_labels();
				if(updateStep > 0) return;
			}

			dl_load_fmanager_main();

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

//#########################################################################################################
//###################################### metody glowne trybow pracy #######################################
//#########################################################################################################
void cMtDisplay::dl_load_blank_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayBgColor);
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

	API_CLEAR_COLOR(displayBgColor);
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

//---------------------------------------------------------------------------
void cMtDisplay::dl_load_print_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayBgColor);
	API_CLEAR(1,1,1);

	uint8_t first_line = (totalLinesPrint < MAX_PRINT_LINES ? 0 : (lastPrintLine < MAX_PRINT_LINES-1 ? lastPrintLine+1 : 0));
	uint8_t max_lines = (totalLinesPrint >= MAX_PRINT_LINES ? MAX_PRINT_LINES : totalLinesPrint);

	for(uint8_t i = 0; i < max_lines; i++)
	{
		API_COLOR(MT_DISP_TITLE_F_COLOR);
		API_CMD_TEXT(3,(i*20)+3, MT_GPU_RAM_FONT1_HANDLE, 0, &text[first_line][0]);

		first_line++;
		if(first_line == MAX_PRINT_LINES) first_line  = 0;
	}

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();



}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------


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



