


#include <stdint.h>

#include "mtDisplay.h"


//#############################################################################
//#############################################################################
//#############################################################################

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

//--------------------------
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

//---------------------------

    API_DISPLAY();
    API_CMD_SWAP();
    API_LIB_EndCoProList();
}


//#############################################################################
//#############################################################################
//#############################################################################

void cMtDisplay::setFmanagerRootTitle(char text[])
{
	for(uint8_t i = 0; i < 20; i++)	displayFmanager.rootTitle[i] = text[i];

	displayRefreshTable.fManager.rootTitle = 1;
	screenRefresh = 1;
}

void cMtDisplay::setFmanagerRootList(uint16_t start, char ** list, uint16_t count, uint8_t filename_length)
{
	fManagerList.setList(0, 2, start, list, count);

	displayRefreshTable.fManager.rootList = 1;
	screenRefresh = 1;
}


void cMtDisplay::setFmanagerRootListPos(uint16_t position)
{
	fManagerList.setListPos(position);

	displayRefreshTable.fManager.rootList = 1;
	screenRefresh = 1;
}


void cMtDisplay::setFmanagerLabels(char * labels)
{
	displayFmanager.labels = labels;

	displayRefreshTable.fManager.labels = 1;
	screenRefresh = 1;
}

//#############################################################################
//#############################################################################
//#############################################################################


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




