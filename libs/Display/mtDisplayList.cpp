


#include <stdint.h>

#include "mtDisplay.h"


void cMtDisplayList::setSeflRefreshPtr(uint8_t *refresh, uint8_t *animate)
{
	selfAnimate   = animate;
	selfRefresh   = refresh;
}

void cMtDisplayList::setListPos(uint16_t position)
{
	listPosition = position;
}



void cMtDisplayList::setList(uint8_t block, uint8_t width, uint16_t start, char ** list, uint16_t count, strMtDisplayColors * colors)
{
	ptrColors = colors;
	listBlockWidth = width;
	listBlock = block;
	listPosition = start;
	listCount = count;
	listStart = start;
	listTable = list;
	listState = 2;
}

void cMtDisplayList::update()
{

	int16_t x_pos, y_pos;
	uint8_t lines;

	API_LIB_BeginCoProList();
    API_CMD_DLSTART();

    x_pos = MT_DISP_BLOCK_W * (listBlock) + ( MT_DISP_BLOCK_MENU_OFFSET);

	//tlo listy
	API_COLOR(ptrColors->listBG);
	API_BLEND_FUNC(DST_ALPHA , ZERO);
	API_LINE_WIDTH(8);
	API_BEGIN(RECTS);
	API_VERTEX2II(x_pos, MT_DISP_BLOCK_MENU_TOP_Y-8,0,0);
	API_VERTEX2II(x_pos+(listBlockWidth*MT_DISP_BLOCK_W)-3, MT_DISP_BLOCK_MENU_TOP_Y+MT_DISP_BLOCK_MENU_Y_SPACE*5-8,0,0);
	API_END();
	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

	if(listStart == listPosition)
	{
		// geneorwanie poczatkowego menu
		listAnimationStep = 0;
		*selfAnimate = 0;

		int8_t sel_row = listState;
		if(listPosition == 0) 					sel_row = 0;
		else if(listPosition == 1) 				sel_row = 1;
		else if(listPosition >= listCount-1)	sel_row = 4;
		else if(listPosition >= listCount-2)	sel_row = 3;

		listState = sel_row;
		x_pos = MT_DISP_BLOCK_W * (listBlock) + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (sel_row * MT_DISP_BLOCK_MENU_Y_SPACE);

		//ramka
		API_COLOR(ptrColors->listItemFrame);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (listBlockWidth*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (listBlockWidth*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();

		//lista tekstow
		x_pos = MT_DISP_BLOCK_W * (listBlock) + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y;

		lines = (listCount >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : listCount;

		//uint8_t txt_offset;
		API_COLOR(ptrColors->fontList);
		for(uint8_t i = 0; i < lines; i++)
		{
			//txt_offset = 1;
			//API_CMD_TEXT(x_pos, y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE), MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), (displayBlock[block].menu)+20*(displayBlock[block].value+row+i));
			//if(*((listTable)+(listRowLength*(listStart  + i -  listState))) == '/') txt_offset = 0;

			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					*(listTable/*+txt_offset*/+(listStart  + i -  listState)));
		}


	}
	else
	{
		int8_t dir, mode;
		uint16_t diffrence;

		// Geneorwanie zmieny menu.
		// I. sprawdzenie czy ruch belka(mode = 0) czy ruch tekstem(mode = 1)
		// 1) okreslenie kierunku
		if(listStart > listPosition) {dir = -1;	diffrence = listStart - listPosition; }
		else {dir = 1;	diffrence = listPosition - listStart; }

		if(diffrence > 5) diffrence = 8;
		else if(diffrence > 1) diffrence = 4;
		else diffrence = 2;

		mode = 0;
		if(listState == 0 || listState == 4 || listState == 2) 	mode = 0;
		else if(listState  == 1) mode = ((dir ==  1 || listStart == 1) ? 0 : 1);
		else if(listState  == 3) mode = ((dir == -1 || listStart == listCount-2) ? 0 : 1);

		if(mode == 0)
		{
			listAnimationStep = listAnimationStep + (dir * diffrence);
			if(listAnimationStep >= 16)
			{
				listAnimationStep = 0;
				listState = listState +1;
				listStart = listStart +1;
			}
			else if(listAnimationStep <= -16)
			{
				listAnimationStep = 0;
				listState = listState -1;
				listStart = listStart -1;
			}
		}
		else
		{
			listAnimationStep = listAnimationStep + (dir * diffrence);
			if(listAnimationStep >= 16)
			{
				listAnimationStep = 0;
				listStart = listStart +1;
			}
			else if(listAnimationStep <= -16)
			{
				listAnimationStep = 0;
				listStart = listStart -1;
			}
		}

		*selfAnimate = 1;
		*selfRefresh = 1;

		//uint8_t y_move = (dir > 0 ? listAnimationStep : (-1)*listAnimationStep);
		x_pos = MT_DISP_BLOCK_W * (listBlock) + ( MT_DISP_BLOCK_MENU_OFFSET);
		y_pos = (MT_DISP_BLOCK_MENU_TOP_Y - (MT_DISP_BLOCK_MENU_Y_SPACE/2)) + (listState * MT_DISP_BLOCK_MENU_Y_SPACE) + (mode ? 0 : listAnimationStep);

		//ramka
		API_COLOR(ptrColors->listItemFrame);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_VERTEX2II(x_pos + (listBlockWidth*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos, 0, 0);
		API_VERTEX2II(x_pos + (listBlockWidth*MT_DISP_BLOCK_W - (MT_DISP_BLOCK_MENU_OFFSET + 6)), y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos + MT_DISP_BLOCK_MENU_Y_SPACE, 0, 0);
		API_VERTEX2II(x_pos, y_pos, 0, 0);
		API_END();


		x_pos = MT_DISP_BLOCK_W * (listBlock) + ( MT_DISP_BLOCK_MENU_OFFSET + MT_DISP_BLOCK_MENU_TEXT_OFFSET);
		y_pos = MT_DISP_BLOCK_MENU_TOP_Y - 16 - (mode ? listAnimationStep : 0);

		API_SAVE_CONTEXT();

		API_COLOR(ptrColors->fontList);

		API_SCISSOR_XY(MT_DISP_BLOCK_W * (listBlock), MT_DISP_BLOCK_MENU_TOP_Y-8);
		API_SCISSOR_SIZE(MT_DISP_BLOCK_W * (listBlockWidth), MT_DISP_BLOCK_MENU_Y_SPACE*5);

		lines = (listCount >= MT_DISP_BLOCK_MENU_ROWS)  ? MT_DISP_BLOCK_MENU_ROWS : listCount;
		//uint8_t txt_offset;


// TODO: TUTEJ MAGIA DO ZROBIENIA
// TODO:
// TODO:
// TODO:
// TODO:
// TODO:


		for(uint8_t i = 0; i < lines; i++)
		{
			//API_CMD_TEXT(x_pos, y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE), MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), (displayBlock[block].menu)+20*(displayBlock[block].value+row+i));

			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					*(listTable+(listStart  + i -  listState)));
		}

/*
		for(int8_t i = -1; i < (lines+1); i++)
		{
			API_CMD_TEXT(x_pos,
					y_pos + (i * MT_DISP_BLOCK_MENU_Y_SPACE),
					MT_GPU_RAM_FONT1_HANDLE,
					(OPT_CENTERY),
					*(listTable+ (listStart  + i -  (listState+1) ) ) );
		}
*/








		API_RESTORE_CONTEXT();

	}

	// pasek przewijania tylko jesli (max > 5)
	if(listCount >= 6)
	{
		uint8_t y_length = ((( MT_DISP_BLOCK_MENU_Y_SPACE * 5 ) * 5)  / (listCount-1)) - 4 ;
		y_pos = 23 + (listStart * (( MT_DISP_BLOCK_MENU_Y_SPACE * 5 )-(y_length+2))) / (listCount-1) ;


		API_COLOR(ptrColors->listScrollBar);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2II((MT_DISP_BLOCK_W * (listBlock)) + (MT_DISP_BLOCK_W - 3) , y_pos ,0,0);
		API_VERTEX2II((MT_DISP_BLOCK_W * (listBlock)) + (MT_DISP_BLOCK_W - 2) , y_pos+y_length,0,0);
		API_END();
	}

    API_LIB_EndCoProList();

}
