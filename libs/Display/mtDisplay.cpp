
#include <stdint.h>

#include "mtDisplay.h"

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

		case mtDisplayModeBlank: 		dl_load_blank_main(); break;
		case mtDisplayModeBlocks5: 		dl_load_block_main(); break;
		case mtDisplayModeFileManager: 	dl_load_fmanager_main(); break;

		default : dl_load_blank_main(); break;
		}


		return;
	}

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//HAPTIC
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

	//-------------------------------------------------
	//-------------------------------------------------
	// sprawdzanei czy wymagane jest odswiezanie
	if(!screenRefresh
		&& (!screenAnimation || animationTimer < MT_DISP_ANIMATION_T)
		&& !displayPrint) return;
	if(!API_LIB_IsCoProEmpty()) return;

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


    if(printTimer > PRINT_SHOW_TIME)
    {
    	displayPrint = 0;
    	screenMode = modeSave;
    	screenRefresh = 1;
    	printClear();
	}

}


void cMtDisplay::dl_load_block_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayBgColor);
	API_CLEAR(1,1,1);

	for(uint8_t i = 0; i < MT_DISP_BLOCK_COUNT ; i++)
	{
		// values
		API_CMD_APPEND(MT_GPU_RAM_BLOCK_CENTERS_ADRESS + (MT_GPU_RAM_BLOCK_CENTERS_SIZE * i), displayBlock[i].ramCenterSize);

		// buttons labels
		API_CMD_APPEND(MT_GPU_RAM_BLOCK_LABELS_ADRESS + (MT_GPU_RAM_BLOCK_LABELS_SIZE * i), displayBlock[i].ramLabelSize);

		// top titles
		API_CMD_APPEND(MT_GPU_RAM_BLOCK_TITLES_ADRESS + (MT_GPU_RAM_BLOCK_TITLES_SIZE * i), displayBlock[i].ramTitleSize);
	}

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
}

void cMtDisplay::dl_load_fmanager_main()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_CLEAR_COLOR(displayBgColor);
	API_CLEAR(1,1,1);

	// rootList
	if(fManagerList.enabled()) API_CMD_APPEND(MT_GPU_RAM_FMANAGER_ROOTLIST_ADRESS, displayFmanager.ramRootListSize);

	// mainList
//	if(displayFmanager.mainListEnable) API_CMD_APPEND(MT_GPU_RAM_FMANAGER_MAINLIST_ADRESS, displayFmanager.ramMainListSize);

	// buttons labels
	API_CMD_APPEND(MT_GPU_RAM_FMANAGER_LABELS_ADRESS, displayFmanager.ramLabelsSize);

	// root titles
	API_CMD_APPEND(MT_GPU_RAM_FMANAGER_ROOTTITLE_ADRESS, displayFmanager.ramRootTitleSize);

	// main titles
//	API_CMD_APPEND(MT_GPU_RAM_FMANAGER_MAINTITLE_ADRESS, displayFmanager.ramMainTitleSize);

//---------------------------------------------------------------------------
// TOUCH PANEL DEMO


/*

for(uint8_t i = 0; i < 5 ; i++)
{
	if(touchPanel[i].x > -32768 && touchPanel[i].y > -32768 )
	{

		API_COLOR(0xFF0000);
		API_BEGIN(FTPOINTS);
		API_POINT_SIZE(30 * 16);
		API_VERTEX2II(touchPanel[i].x, touchPanel[i].y, 0, 0) ;
		API_END();


	}

}
*/

//---------------------------------------------------------------------------

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
}

//#########################################################################################################
//################################## metody poszczegolnych trybow #########################################
//#########################################################################################################
void cMtDisplay::ramg_blocks_title(uint8_t block)
{
    API_LIB_BeginCoProList();

    API_CMD_DLSTART();
    //API_SAVE_CONTEXT();
	API_COLOR(displayBlock[block].fontTitleColor);
	API_CMD_TEXT(MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_TITLE_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), displayBlock[block].title);
	//API_RESTORE_CONTEXT();

    API_LIB_EndCoProList();
    API_LIB_AwaitCoProEmpty();

	uint16_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProList();
	API_CMD_MEMCPY(MT_GPU_RAM_BLOCK_TITLES_ADRESS+(MT_GPU_RAM_BLOCK_TITLES_SIZE*block), RAM_DL, dlOffset);
	displayBlock[block].ramTitleSize = dlOffset;

	API_LIB_EndCoProList();
   // API_LIB_AwaitCoProEmpty();
}

void cMtDisplay::ramg_blocks_label(uint8_t block)
{
    API_LIB_BeginCoProList();

    API_CMD_DLSTART();
	API_COLOR(displayBlock[block].labelColor);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);
	API_VERTEX2II((MT_DISP_BLOCK_W * block + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_H - MT_DISP_BLOCK_LABEL_H) ,0,0);
	API_VERTEX2II((MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) , MT_DISP_H,0,0);
	API_END();
	API_COLOR(displayBlock[block].fontLabelColor);
	API_CMD_TEXT(MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_LABEL_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), displayBlock[block].label);

    API_LIB_EndCoProList();
    API_LIB_AwaitCoProEmpty();

	uint16_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProList();
	API_CMD_MEMCPY(MT_GPU_RAM_BLOCK_LABELS_ADRESS+(MT_GPU_RAM_BLOCK_LABELS_SIZE*block), RAM_DL, dlOffset);
	displayBlock[block].ramLabelSize = dlOffset;

	API_LIB_EndCoProList();
   // API_LIB_AwaitCoProEmpty();

}

void cMtDisplay::ramg_blocks_center(uint8_t block)
{
	switch(displayBlock[block].type)
	{
		case mtDisplayBlockTypeBlank:	ramg_blocks_blank(block);	break;
		case mtDisplayBlockTypeRound:	ramg_blocks_value(block);	break;
		case mtDisplayBlockTypeMenu:	ramg_blocks_menu(block);	break;
		default: 						ramg_blocks_blank(block); 	break;
	}
}

void cMtDisplay::ramg_blocks_blank(uint8_t block)
{
	API_LIB_BeginCoProList();

    API_CMD_DLSTART();
    API_COLOR(displayBgColor);

    API_LIB_EndCoProList();
    API_LIB_AwaitCoProEmpty();

	uint16_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProList();
	API_CMD_MEMCPY(MT_GPU_RAM_BLOCK_CENTERS_ADRESS+(MT_GPU_RAM_BLOCK_CENTERS_SIZE*block), RAM_DL, dlOffset);
	displayBlock[block].ramCenterSize = dlOffset;

	API_LIB_EndCoProList();
    //API_LIB_AwaitCoProEmpty();
}

void cMtDisplay::ramg_blocks_value(uint8_t block)
{
	API_LIB_BeginCoProList();

	API_CMD_DLSTART();
	API_COLOR(displayBlock[block].valueColor);
	API_BEGIN(FTPOINTS);
	API_POINT_SIZE(MT_DISP_BLOCK_VALUE_ROUND_SIZE * 16);
	API_VERTEX2II(MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, 0, 0) ;
	API_END();

	API_COLOR(displayBgColor);
	API_BEGIN(FTPOINTS);
	API_POINT_SIZE((MT_DISP_BLOCK_VALUE_ROUND_SIZE - MT_DISP_BLOCK_VALUE_ROUND_W) * 16);
	API_VERTEX2II(MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, 0, 0) ;
	API_END();

	API_LINE_WIDTH(24);

//	API_COLOR(0x00ff0000);  // debug red
	if(displayBlock[block].value > 66)
	{
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2)) 								, MT_DISP_BLOCK_VALUE_CENTER_Y -  MT_DISP_BLOCK_VALUE_ROUND_SIZE, 0, 0);
		API_VERTEX2II((MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) 	, MT_DISP_BLOCK_VALUE_CENTER_Y, 0,0);
		API_END();

		int16_t center_x = (MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2));
		int16_t center_y = MT_DISP_BLOCK_VALUE_CENTER_Y;
		int16_t  cut_p_x  = (MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2));
		int16_t  cut_p_y  = MT_DISP_BLOCK_VALUE_CENTER_Y-(MT_DISP_BLOCK_VALUE_ROUND_SIZE+2);
		uint16_t cut_alfa = ((100-displayBlock[block].value)*90)/34;

		for(uint8_t i = 0; i < cut_alfa ; i+=2)
		{
			API_BEGIN(LINES);
			API_VERTEX2II( center_x, center_y, 0, 0);
			API_VERTEX2II( center_x + ((cut_p_x-center_x)*cos_deg[i] - (cut_p_y-center_y) * sin_deg[i] ),  center_y +(  (cut_p_x-center_x) * sin_deg[i] + (cut_p_y-center_y) * cos_deg[i]), 0, 0);
			API_END();
		}

	}
	else if(displayBlock[block].value > 33)
	{
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * block)+3 													, MT_DISP_BLOCK_VALUE_CENTER_Y -  MT_DISP_BLOCK_VALUE_ROUND_SIZE, 0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W - 6))	, MT_DISP_BLOCK_VALUE_CENTER_Y, 0,0);
		API_END();

		int16_t center_x = (MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2));
		int16_t center_y = MT_DISP_BLOCK_VALUE_CENTER_Y;
		int16_t  cut_p_x  = (MT_DISP_BLOCK_W * block )+ 11;
		int16_t  cut_p_y  = MT_DISP_BLOCK_VALUE_CENTER_Y;
		uint16_t cut_alfa = ((67-displayBlock[block].value)*90)/34;


		for(uint8_t i = 0; i < cut_alfa ; i+=2)
		{
			API_BEGIN(LINES);
			API_VERTEX2II( center_x, center_y, 0, 0);
			API_VERTEX2II( center_x + ((cut_p_x-center_x)*cos_deg[i]  - (cut_p_y-center_y) * sin_deg[i] ),  center_y +(  (cut_p_x-center_x) * sin_deg[i] + (cut_p_y-center_y) * cos_deg[i]), 0, 0);
			API_END();
		}
	}
	else
	{
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * block) 													, MT_DISP_BLOCK_VALUE_CENTER_Y -  MT_DISP_BLOCK_VALUE_ROUND_SIZE, 0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET))	, MT_DISP_BLOCK_VALUE_CENTER_Y, 0,0);
		API_END();
		API_BEGIN(RECTS);
		API_VERTEX2II(MT_DISP_BLOCK_W * block  														, MT_DISP_BLOCK_VALUE_CENTER_Y, 0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2)) 								, MT_DISP_BLOCK_VALUE_CENTER_Y +  MT_DISP_BLOCK_VALUE_ROUND_SIZE, 0,0);
		API_END();


		int16_t center_x = (MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2));
		int16_t center_y = MT_DISP_BLOCK_VALUE_CENTER_Y;
		int16_t  cut_p_x  = (MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2));
		int16_t  cut_p_y  = MT_DISP_BLOCK_VALUE_CENTER_Y+(MT_DISP_BLOCK_VALUE_ROUND_SIZE+2);
		uint16_t cut_alfa = ((33-displayBlock[block].value)*90)/34;


		for(uint8_t i = 0; i < cut_alfa ; i+=2)
		{
			API_BEGIN(LINES);
			API_VERTEX2II( center_x, center_y, 0, 0);
			API_VERTEX2II( center_x + ((cut_p_x-center_x)*cos_deg[i] - (cut_p_y-center_y) * sin_deg[i] ),  center_y +(  (cut_p_x-center_x) * sin_deg[i] + (cut_p_y-center_y) * cos_deg[i]), 0, 0);
			API_END();
		}

	}

	API_COLOR(displayBlock[block].fontValueColor);
	API_CMD_NUMBER(MT_DISP_BLOCK_W * block + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_VALUE_CENTER_Y, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERX | OPT_CENTERY), displayBlock[block].value);


	API_LIB_EndCoProList();

	updateAdress = MT_GPU_RAM_BLOCK_CENTERS_ADRESS+(MT_GPU_RAM_BLOCK_CENTERS_SIZE*block);
	updateSize = &displayBlock[block].ramCenterSize;


	updateStep = 1;
}

void cMtDisplay::ramg_blocks_menu(uint8_t block)
{
	int16_t x_pos, y_pos;
	uint8_t lines;

	API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	if(displayBlock[block].start == displayBlock[block].value)
	{

		// geneorwanie poczatkowego menu
		displayBlock[block].animationStep = 0;
		screenAnimation = 0;

		int8_t sel_row = displayBlock[block].state;
		if(displayBlock[block].value == 0) 								sel_row = 0;
		else if(displayBlock[block].value == 1) 						sel_row = 1;
		else if(displayBlock[block].value >= displayBlock[block].max)	sel_row = 4;
		else if(displayBlock[block].value >= displayBlock[block].max-1)	sel_row = 3;

		displayBlock[block].state = sel_row;
		x_pos = MT_DISP_BLOCK_W * block + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (sel_row * MT_DISP_BLOCK_MENU_Y_SPACE);

//------------------------------------------------
/*		API_SAVE_CONTEXT();
		API_SCISSOR_XY(x_pos, y_pos);
		API_SCISSOR_SIZE((MT_DISP_BLOCK_W - 2*MT_DISP_BLOCK_MENU_OFFSET), MT_DISP_BLOCK_MENU_Y_SPACE);
		API_CMD_GRADIENT(x_pos, 0, 0x202020, x_pos + (MT_DISP_BLOCK_W - 2*MT_DISP_BLOCK_MENU_OFFSET), 0,0x101010);
		API_RESTORE_CONTEXT();
//------------------------------------------------
*/
		//ramka
		API_COLOR(0xFFFFFF);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();
//------------------------------------------------

		x_pos = MT_DISP_BLOCK_W * block + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y;

		lines = (displayBlock[block].max >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : displayBlock[block].max+1;
		API_COLOR(displayBlock[block].valueColor);
		for(uint8_t i = 0; i < lines; i++)
		{
			//API_CMD_TEXT(x_pos, y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE), MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), (displayBlock[block].menu)+20*(displayBlock[block].value+row+i));
			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					(displayBlock[block].menu)+20*(displayBlock[block].start  + i -  displayBlock[block].state));
		}


	}
	else
	{
		int8_t dir, mode;
		uint16_t diffrence;

		// Geneorwanie zmieny menu.
		// I. sprawdzenie czy ruch belka(mode = 0) czy ruch tekstem(mode = 1)
		// 1) okreslenie kierunku
		if(displayBlock[block].start > displayBlock[block].value) {dir = -1;	diffrence = displayBlock[block].start - displayBlock[block].value; }
		else {dir = 1;	diffrence = displayBlock[block].value - displayBlock[block].start; }

		if(diffrence > 5) diffrence = 8;
		else if(diffrence > 1) diffrence = 4;
		else diffrence = 2;

		mode = 0;
		if(displayBlock[block].state == 0 || displayBlock[block].state == 4 || displayBlock[block].state == 2) 	mode = 0;
		else if(displayBlock[block].state  == 1) mode = ((dir ==  1 || displayBlock[block].start == 1) ? 0 : 1);
		else if(displayBlock[block].state  == 3) mode = ((dir == -1 || displayBlock[block].start == displayBlock[block].max-1) ? 0 : 1);

		if(mode == 0)
		{
			displayBlock[block].animationStep = displayBlock[block].animationStep + (dir * diffrence);
			if(displayBlock[block].animationStep >= 16)
			{
				displayBlock[block].animationStep = 0;
				displayBlock[block].state = displayBlock[block].state +1;
				displayBlock[block].start = displayBlock[block].start +1;
			}
			else if(displayBlock[block].animationStep <= -16)
			{
				displayBlock[block].animationStep = 0;
				displayBlock[block].state = displayBlock[block].state -1;
				displayBlock[block].start = displayBlock[block].start -1;
			}

		}
		else
		{
			//if(dir > 0) 	displayBlock[block].start = displayBlock[block].start +1;
			//else 			displayBlock[block].start = displayBlock[block].start -1;

			displayBlock[block].animationStep = displayBlock[block].animationStep + (dir * diffrence);
			if(displayBlock[block].animationStep >= 16)
			{
				displayBlock[block].animationStep = 0;
				displayBlock[block].start = displayBlock[block].start +1;
			}
			else if(displayBlock[block].animationStep <= -16)
			{
				displayBlock[block].animationStep = 0;
				displayBlock[block].start = displayBlock[block].start -1;
			}

		}




		displayRefreshTable.block[block].center = 1;
		screenAnimation = 1;

		//uint8_t y_move = (dir > 0 ? displayBlock[block].animationStep : (-1)*displayBlock[block].animationStep);
		x_pos = MT_DISP_BLOCK_W * block + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (displayBlock[block].state * MT_DISP_BLOCK_MENU_Y_SPACE) + (mode ? 0 : displayBlock[block].animationStep);

		//ramka
		API_COLOR(0xFFFFFF);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();


		x_pos = MT_DISP_BLOCK_W * block + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y - 16 - (mode ? displayBlock[block].animationStep : 0);

		API_SAVE_CONTEXT();

		API_COLOR(displayBlock[block].valueColor);

		API_SCISSOR_XY(MT_DISP_BLOCK_W * block, MT_DISP_BLOCK_MENU_TOP_Y-8);
		API_SCISSOR_SIZE(MT_DISP_BLOCK_W, MT_DISP_BLOCK_MENU_Y_SPACE*5);

		for(int8_t i = -1; i < (MT_DISP_BLOCK_MENU_ROWS+1); i++)
		{

			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					(displayBlock[block].menu)+20*(displayBlock[block].start  + i -  (displayBlock[block].state+1)));
		}

		API_RESTORE_CONTEXT();

	}

	// pasek przewijania tylko jesli (max > 5)
	if(displayBlock[block].max >= 5)
	{
		uint8_t y_length = (( MT_DISP_BLOCK_MENU_Y_SPACE * 5 ) * 5)  / displayBlock[block].max ;
		y_pos = 24 + (displayBlock[block].start * (( MT_DISP_BLOCK_MENU_Y_SPACE * 5 )-(y_length+3))) / displayBlock[block].max ;


		API_COLOR(displayBlock[block].labelColor);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * block) + (MT_DISP_BLOCK_W - 3) , y_pos ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * block) + (MT_DISP_BLOCK_W - 2) , y_pos+y_length,0,0);
		API_END();
	}

    API_LIB_EndCoProList();

	updateAdress = MT_GPU_RAM_BLOCK_CENTERS_ADRESS+(MT_GPU_RAM_BLOCK_CENTERS_SIZE*block);
	updateSize = &displayBlock[block].ramCenterSize;

	updateStep = 1;
}


















void cMtDisplay::ramg_fmanager_roottitle()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	API_COLOR(displayFmanager.fontTitleColor);
	API_CMD_TEXT(MT_DISP_FMANAGER_ROOTTITLE_X, MT_DISP_FMANAGER_TITLE_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), displayFmanager.rootTitle);

    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_FMANAGER_ROOTTITLE_ADRESS;
	updateSize = &displayFmanager.ramRootTitleSize;

	updateStep = 1;
}



void cMtDisplay::ramg_fmanager_labels()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    for(uint8_t i = 0; i < 5; i++)
    {
		API_COLOR(displayFmanager.labelColor);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + MT_DISP_BLOCK_LABEL_OFFSET) , (MT_DISP_H - MT_DISP_BLOCK_LABEL_H) ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W - MT_DISP_BLOCK_LABEL_OFFSET)) , MT_DISP_H,0,0);
		API_END();
		API_COLOR(displayFmanager.fontLabelColor);
		API_CMD_TEXT(MT_DISP_BLOCK_W * i + (MT_DISP_BLOCK_W/2), MT_DISP_BLOCK_LABEL_Y, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERX | OPT_CENTERY), displayFmanager.labels+(i*20));
    }

    API_LIB_EndCoProList();

    /**/

	updateAdress = MT_GPU_RAM_FMANAGER_LABELS_ADRESS;
	updateSize = &displayFmanager.ramLabelsSize;

	updateStep = 1;
}

void cMtDisplay::ramg_fmanager_rootlist()
{
	int16_t x_pos, y_pos;
	uint8_t lines;

	API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	if(displayFmanager.rootStart == displayFmanager.rootPosition)
	{
		// geneorwanie poczatkowego menu
		displayFmanager.animationStepRoot = 0;
		screenAnimation = 0;

		int8_t sel_row = displayFmanager.rootState;
		if(displayFmanager.rootPosition == 0) 								sel_row = 0;
		else if(displayFmanager.rootPosition == 1) 							sel_row = 1;
		else if(displayFmanager.rootPosition >= displayFmanager.rootCount-1)	sel_row = 4;
		else if(displayFmanager.rootPosition >= displayFmanager.rootCount-2)	sel_row = 3;

		displayFmanager.rootState = sel_row;
		x_pos = MT_DISP_BLOCK_W * (0) + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (sel_row * MT_DISP_BLOCK_MENU_Y_SPACE);


		//ramka
		API_COLOR(0xFFFFFF);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (2*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (2*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();

		//lista tekstow
		x_pos = MT_DISP_BLOCK_W * (0) + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y;

		lines = (displayFmanager.rootCount >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : displayFmanager.rootCount;

		API_COLOR(displayFmanager.listColor);
		for(uint8_t i = 0; i < lines; i++)
		{
			//API_CMD_TEXT(x_pos, y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE), MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), (displayBlock[block].menu)+20*(displayBlock[block].value+row+i));
			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					(displayFmanager.rootList)+1+(displayFmanager.rootListFilenameLength*(displayFmanager.rootStart  + i -  displayFmanager.rootState)));
		}


	}
	else
	{
		int8_t dir, mode;
		uint16_t diffrence;

		// Geneorwanie zmieny menu.
		// I. sprawdzenie czy ruch belka(mode = 0) czy ruch tekstem(mode = 1)
		// 1) okreslenie kierunku
		if(displayFmanager.rootStart > displayFmanager.rootPosition) {dir = -1;	diffrence = displayFmanager.rootStart - displayFmanager.rootPosition; }
		else {dir = 1;	diffrence = displayFmanager.rootPosition - displayFmanager.rootStart; }

		if(diffrence > 5) diffrence = 8;
		else if(diffrence > 1) diffrence = 4;
		else diffrence = 2;

		mode = 0;
		if(displayFmanager.rootState == 0 || displayFmanager.rootState == 4 || displayFmanager.rootState == 2) 	mode = 0;
		else if(displayFmanager.rootState  == 1) mode = ((dir ==  1 || displayFmanager.rootStart == 1) ? 0 : 1);
		else if(displayFmanager.rootState  == 3) mode = ((dir == -1 || displayFmanager.rootStart == displayFmanager.rootCount-2) ? 0 : 1);

		if(mode == 0)
		{
			displayFmanager.animationStepRoot = displayFmanager.animationStepRoot + (dir * diffrence);
			if(displayFmanager.animationStepRoot >= 16)
			{
				displayFmanager.animationStepRoot = 0;
				displayFmanager.rootState = displayFmanager.rootState +1;
				displayFmanager.rootStart = displayFmanager.rootStart +1;
			}
			else if(displayFmanager.animationStepRoot <= -16)
			{
				displayFmanager.animationStepRoot = 0;
				displayFmanager.rootState = displayFmanager.rootState -1;
				displayFmanager.rootStart = displayFmanager.rootStart -1;
			}
		}
		else
		{
			displayFmanager.animationStepRoot = displayFmanager.animationStepRoot + (dir * diffrence);
			if(displayFmanager.animationStepRoot >= 16)
			{
				displayFmanager.animationStepRoot = 0;
				displayFmanager.rootStart = displayFmanager.rootStart +1;
			}
			else if(displayFmanager.animationStepRoot <= -16)
			{
				displayFmanager.animationStepRoot = 0;
				displayFmanager.rootStart = displayFmanager.rootStart -1;
			}
		}




		displayRefreshTable.fManager.rootList = 1;
		screenAnimation = 1;

		//uint8_t y_move = (dir > 0 ? displayFmanager.animationStepRoot : (-1)*displayFmanager.animationStepRoot);
		x_pos = MT_DISP_BLOCK_W * (0) + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (displayFmanager.rootState * MT_DISP_BLOCK_MENU_Y_SPACE) + (mode ? 0 : displayFmanager.animationStepRoot);

		//ramka
		API_COLOR(0xFFFFFF);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (2*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (2*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();


		x_pos = MT_DISP_BLOCK_W * (0) + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y - 16 - (mode ? displayFmanager.animationStepRoot : 0);

		API_SAVE_CONTEXT();

		API_COLOR(displayFmanager.listColor);

		API_SCISSOR_XY(MT_DISP_BLOCK_W * (0), MT_DISP_BLOCK_MENU_TOP_Y-8);
		API_SCISSOR_SIZE(MT_DISP_BLOCK_W * (2), MT_DISP_BLOCK_MENU_Y_SPACE*5);

		lines = (displayFmanager.rootCount >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : displayFmanager.rootCount;

		for(int8_t i = -1; i < (lines+1); i++)
		{

			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					(displayFmanager.rootList)+1+(displayFmanager.rootListFilenameLength*(displayFmanager.rootStart  + i -  (displayFmanager.rootState+1))));
		}

		API_RESTORE_CONTEXT();

	}

	// pasek przewijania tylko jesli (max > 5)
	if(displayFmanager.rootCount >= 6)
	{
		uint8_t y_length = ((( MT_DISP_BLOCK_MENU_Y_SPACE * 5 ) * 5)  / (displayFmanager.rootCount-1)) - 4 ;
		y_pos = 23 + (displayFmanager.rootStart * (( MT_DISP_BLOCK_MENU_Y_SPACE * 5 )-(y_length))) / (displayFmanager.rootCount-1) ;


		API_COLOR(displayFmanager.listColor);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * (1)) + (MT_DISP_BLOCK_W - 3) , y_pos ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * (1)) + (MT_DISP_BLOCK_W - 2) , y_pos+y_length,0,0);
		API_END();
	}

    API_LIB_EndCoProList();

	updateAdress = MT_GPU_RAM_FMANAGER_ROOTLIST_ADRESS;
	updateSize = &displayFmanager.ramRootListSize;

	updateStep = 1;
}

/*
void cMtDisplay::ramg_fmanager_mainlist()
{

	int16_t x_pos, y_pos;
	uint8_t lines;
	API_LIB_BeginCoProList();
    API_CMD_DLSTART();

	if(displayFmanager.mainStart == displayFmanager.mainPosition)
	{
		// geneorwanie poczatkowego menu
		displayFmanager.animationStepMain = 0;
		screenAnimation = 0;

		int8_t sel_row = displayFmanager.mainState;
		if(displayFmanager.mainPosition == 0) 								sel_row = 0;
		else if(displayFmanager.mainPosition == 1) 							sel_row = 1;
		else if(displayFmanager.mainPosition >= displayFmanager.mainCount-1)	sel_row = 4;
		else if(displayFmanager.mainPosition >= displayFmanager.mainCount-2)	sel_row = 3;

		displayFmanager.mainState = sel_row;
		x_pos = MT_DISP_BLOCK_W * (2) + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (sel_row * MT_DISP_BLOCK_MENU_Y_SPACE);


		//ramka
		API_COLOR(0xFFFFFF);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (3*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (3*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();

		//lista tekstow
		x_pos = MT_DISP_BLOCK_W * (2) + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y;

		lines = (displayFmanager.mainCount >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : displayFmanager.mainCount;
		API_COLOR(displayFmanager.listColor);
		for(uint8_t i = 0; i < lines; i++)
		{
			//API_CMD_TEXT(x_pos, y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE), MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), (displayBlock[block].menu)+20*(displayBlock[block].value+row+i));
			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					(displayFmanager.mainList)+1+(displayFmanager.mainListFilenameLength*(displayFmanager.mainStart  + i -  displayFmanager.mainState)));
		}
	}
	else
	{
		int8_t dir, mode;
		uint16_t diffrence;

		// Geneorwanie zmieny menu.
		// I. sprawdzenie czy ruch belka(mode = 0) czy ruch tekstem(mode = 1)
		// 1) okreslenie kierunku
		if(displayFmanager.mainStart > displayFmanager.mainPosition) {dir = -1;	diffrence = displayFmanager.mainStart - displayFmanager.mainPosition; }
		else {dir = 1;	diffrence = displayFmanager.mainPosition - displayFmanager.mainStart; }

		if(diffrence > 5) diffrence = 8;
		else if(diffrence > 1) diffrence = 4;
		else diffrence = 2;

		mode = 0;
		if(displayFmanager.mainState == 0 || displayFmanager.mainState == 4 || displayFmanager.mainState == 2) 	mode = 0;
		else if(displayFmanager.mainState  == 1) mode = ((dir ==  1 || displayFmanager.mainStart == 1) ? 0 : 1);
		else if(displayFmanager.mainState  == 3) mode = ((dir == -1 || displayFmanager.mainStart == displayFmanager.mainCount-2) ? 0 : 1);

		if(mode == 0)
		{
			displayFmanager.animationStepMain = displayFmanager.animationStepMain + (dir * diffrence);
			if(displayFmanager.animationStepMain >= 16)
			{
				displayFmanager.animationStepMain = 0;
				displayFmanager.mainState = displayFmanager.mainState +1;
				displayFmanager.mainStart = displayFmanager.mainStart +1;
			}
			else if(displayFmanager.animationStepMain <= -16)
			{
				displayFmanager.animationStepMain = 0;
				displayFmanager.mainState = displayFmanager.mainState -1;
				displayFmanager.mainStart = displayFmanager.mainStart -1;
			}
		}
		else
		{
			displayFmanager.animationStepMain = displayFmanager.animationStepMain + (dir * diffrence);
			if(displayFmanager.animationStepMain >= 16)
			{
				displayFmanager.animationStepMain = 0;
				displayFmanager.mainStart = displayFmanager.mainStart +1;
			}
			else if(displayFmanager.animationStepMain <= -16)
			{
				displayFmanager.animationStepMain = 0;
				displayFmanager.mainStart = displayFmanager.mainStart -1;
			}
		}




		displayRefreshTable.fManager.mainList = 1;
		screenAnimation = 1;

		//uint8_t y_move = (dir > 0 ? displayFmanager.animationStepMain : (-1)*displayFmanager.animationStepMain);
		x_pos = MT_DISP_BLOCK_W * (2) + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (displayFmanager.mainState * MT_DISP_BLOCK_MENU_Y_SPACE) + (mode ? 0 : displayFmanager.animationStepMain);

		//ramka
		API_COLOR(0xFFFFFF);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + ((3)*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + ((3)*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();


		x_pos = MT_DISP_BLOCK_W * (2) + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y - 16 - (mode ? displayFmanager.animationStepMain : 0);

		API_SAVE_CONTEXT();

		API_COLOR(displayFmanager.listColor);

		API_SCISSOR_XY(MT_DISP_BLOCK_W * (2), MT_DISP_BLOCK_MENU_TOP_Y-8);
		API_SCISSOR_SIZE(MT_DISP_BLOCK_W * (3), MT_DISP_BLOCK_MENU_Y_SPACE*5);

		lines = (displayFmanager.mainCount >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : displayFmanager.mainCount;

		for(int8_t i = -1; i < (lines+1); i++)
		{

			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					(displayFmanager.mainList)+1+(displayFmanager.mainListFilenameLength*(displayFmanager.mainStart  + i -  (displayFmanager.mainState+1))));
		}

		API_RESTORE_CONTEXT();

	}

	// pasek przewijania tylko jesli (max > 5)
	if(displayFmanager.mainCount >= 6)
	{
		uint8_t y_length = ((( MT_DISP_BLOCK_MENU_Y_SPACE * 5 ) * 5)  / (displayFmanager.mainCount-1)) - 4 ;
		y_pos = 23 + (displayFmanager.mainStart * (( MT_DISP_BLOCK_MENU_Y_SPACE * 5 )-(y_length))) / (displayFmanager.mainCount - 1) ;


		API_COLOR(displayFmanager.listColor);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * (4)) + (MT_DISP_BLOCK_W - 3) , y_pos ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * (4)) + (MT_DISP_BLOCK_W - 2) , y_pos+y_length,0,0);
		API_END();
	}

    API_LIB_EndCoProList();

	updateAdress = MT_GPU_RAM_FMANAGER_MAINLIST_ADRESS;
	updateSize = &displayFmanager.ramMainListSize;

	updateStep = 1;

}

*/


