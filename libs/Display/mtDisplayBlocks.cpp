



#include <stdint.h>

#include "mtDisplay.h"

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

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

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

void cMtDisplay::setBlockType (uint8_t number, uint32_t type, uint16_t start, char * menu, uint16_t max)
{
	displayBlock[number].type = type;
	displayBlock[number].menu = menu;
	displayBlock[number].max = max;
	displayBlock[number].start = start;
	displayBlock[number].value = start;

	displayRefreshTable.block[number].center = 1;
	displayRefreshTable.block[number].title = 1;
	displayRefreshTable.block[number].label = 1;
	screenRefresh = 1;
}

void cMtDisplay::setBlockTitle(uint8_t number, char text[])
{
	for(uint8_t i = 0; i < 20; i++)	displayBlock[number].title[i] = text[i];

	displayRefreshTable.block[number].title = 1;
	screenRefresh = 1;
}

void cMtDisplay::setBlockLabel(uint8_t number, char text[])
{
	for(uint8_t i = 0; i < 20; i++)	displayBlock[number].label[i] = text[i];

	displayRefreshTable.block[number].label = 1;
	screenRefresh = 1;
}

void cMtDisplay::setBlockCenter(uint8_t number, int32_t value)
{
	displayBlock[number].value = value;

	displayRefreshTable.block[number].center = 1;
	screenRefresh = 1;
}

//#########################################################################################################
//#########################################################################################################
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












