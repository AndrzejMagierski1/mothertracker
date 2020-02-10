
#include "FT812.h"

#include "trackerControl.h"

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void String2Bitmaps(int16_t x, int16_t y, const strFont* font, char* string, int8_t length);

static uint32_t defaultColors[] =

{
	0x000000, // 0 linie
	0x232323, // 1 background  /numery wierszy juz nie bo nie ma/

	0x00e8be, // 2 nuta
	0xfef749, // 3 instrument
	0xbb58f1, // 4 effekt1
	0x57f1ff, // 5 effekt2

	0x2c6861, // 6 nuta nieaktywna
	0x756a47, // 7 instrument nieaktywny
	0x614b69, // 8 effekt1 nieaktywny
	0x4b6977, // 9 effekt2 nieaktywny

	0xf13c3c, // 10 zaznaczenie
	0x2a2a2a, // 11 podzialka
	0x333333, // 12 nieaktywny
	0x000000, // 13 playhead

	0xffffff, // 14 tekst zaznaczenia
	0xf13c3c, // 15 playhead rec mode
};



#define ROW_NUM 1





#if ROW_NUM
//uint8_t fourParamsOffset[4] =		{	4,44,73,130,	};	// stara koncepcja z numerami wierszy
//uint8_t twoParamsOffsetFirst[4] =	{	9,9,9,9,		};
//uint8_t twoParamsOffsetSecond[4] =	{	50,50,50,50,	};

uint8_t fourParamsOffset[4] =		{	10,  52,  86, 140,	};
uint8_t twoParamsOffsetFirst[4] =	{	 7,  10,   5,   0,	};
uint8_t twoParamsOffsetSecond[4] =	{	 0,  61,  50,  50,	};
uint8_t oneParamsOffset[4] =		{	32,  37,  27,  27,	};


const uint8_t fourParamsSelOffset[4] =	{  1,   44,  78, 131,	};
const uint8_t fourParamsSelWidth[4] =	{  40,  30,  50,  50,	};

const uint8_t twoParamsSel1Offset[4] =	{   1,   1,   1,   0,	};
const uint8_t twoParamsSel1Width[4] =	{  40,  33,  43,   0,	};

const uint8_t twoParamsSel2Offset[4] =	{	0,  51,  43,  43,	};
const uint8_t twoParamsSel2Width[4] =	{	0,  37,  45,  45,	};

const uint8_t oneParamsSelOffset[4] =	{  24,  29,  19,  19,	};
const uint8_t oneParamsSelWidth[4] =	{  40,  30,  50,  50,	};



#else

uint8_t fourParamsOffset[4] =		{	18,  60,  92, 144,	};
uint8_t twoParamsOffsetFirst[4] =	{	 9,   9,   6,   0,	};
uint8_t twoParamsOffsetSecond[4] =	{	 0,  65,  55,  55,	};
uint8_t oneParamsOffset[4] =		{	35,  40,  30,  30,	};


const uint8_t fourParamsSelOffset[4] =	{  10,  52,  84, 136,	};
const uint8_t fourParamsSelWidth[4] =	{  40,  30,  50,  50,	};

const uint8_t twoParamsSel1Offset[4] =	{   1,   1,   1,   0,	};
const uint8_t twoParamsSel1Width[4] =	{  40,  30,  47,   0,	};

const uint8_t twoParamsSel2Offset[4] =	{	0,  57,  49,  49,	};
const uint8_t twoParamsSel2Width[4] =	{	0,  30,  47,  47,	};

const uint8_t oneParamsSelOffset[4] =	{  27,  32,  22,  22,	};
const uint8_t oneParamsSelWidth[4] =	{  40,  30,  50,  50,	};


const uint8_t paramToStateOffset[4] =	{  1,  2,  4,  8,	};

#endif


const uint8_t paramToStateOffset[4] =	{  1,  2,  4,  8,	};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cTracker::cTracker(strControlProperties* properties)
{
	//visibleCharOffset = 0;
	//firstVisibleTrack = 0;
	//visibleTracksOffset = 0;

	colorsCount = 16;
	colors = defaultColors;

	refreshStep =  0;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		width = 0;
		height = 0;
		tracks = nullptr;
		this->style = 0;
		value = 0;
		return;
	}

	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;

	tracks = (strTrackerPattern*)properties->data;
	posX = properties->x;
	posY = properties->y;
	width = properties->w;
	height = properties->h;
	value = properties->value;

	setStyle(properties->style);
}


cTracker::~cTracker()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cTracker::setStyle(uint32_t style)
{
	this->style = style;
}

void cTracker::setText(char* text)
{

}

void cTracker::setValue(int value)
{
	this->value = value;
}

void cTracker::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cTracker::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cTracker::setData(void* data)
{
	tracks = (strTrackerPattern*)data;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cTracker::update()
{
	//API_LIB_BeginDirectDL();
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

	API_SAVE_CONTEXT();

    switch(refreshStep)
    {
    case 0: refresh1(); break;
    case 1: refresh2(); break;
    case 2: refresh3(); break;
    case 3: refresh4(); break;
    case 4: refresh5(); break;
    default: refreshStep = 0; break;
    }

	API_RESTORE_CONTEXT();

    API_LIB_EndCoProList();

	return 0;
}

uint8_t cTracker::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();


	switch(refreshStep)
	{
	case 0: API_CMD_MEMCPY(address, 		 		RAM_DL, dlOffset); break;
	case 1: API_CMD_MEMCPY(address+ramPartSize[0], 	RAM_DL, dlOffset); break;
	case 2: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1], 	RAM_DL, dlOffset); break;
	case 3: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2], 	RAM_DL, dlOffset); break;
	case 4: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3], 	RAM_DL, dlOffset); break;
	default: break;
	}

	ramPartSize[refreshStep] = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();


/*
	if(dlOffset > 4000)
	{
		__asm__ __volatile__ ("bkpt #0");
	}
*/

    refreshStep++;
    if(refreshStep > 4)
    {
    	refreshStep = 0;
    	return 0;
    }

	return 1;
}

uint8_t cTracker::append(uint32_t address)
{
	//length = ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3]+ramPartSize[4];
	//API_CMD_APPEND(address		, 3900);

//	API_CMD_APPEND(address, ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3]+ramPartSize[4]);

	API_CMD_APPEND(address, ramPartSize[0]);
	API_CMD_APPEND(address+ ramPartSize[0], ramPartSize[1]);
	API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1], ramPartSize[2]);
	API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1] +ramPartSize[2], ramPartSize[3]);
	API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1] +ramPartSize[2] +ramPartSize[3], ramPartSize[4]);

/*
	if(ramPartSize[0] +ramPartSize[1] +ramPartSize[2] +ramPartSize[3] +ramPartSize[4] > 8000)
	{
	//	Serial.println("ft800 cmd overload");
		__asm__ __volatile__ ("bkpt #0");
	}
*/
	return 0;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cTracker::refresh1()
{
	//colors[10] = tracks->selectColor;
	displayMode = value;

	rightOffset = 0;
	leftOffset = 0;

#if ROW_NUM
	rightOffset = 27;
	leftOffset = 27;
#endif


	if(displayMode > 0)
	{
		tracksSpace = (800 - (rightOffset + leftOffset)) / 8;
		columnsCount = (tracks->popupMode & 2) ? 6 : 8;
		columnsCount = (tracks->popupMode & 4) ? 4 : columnsCount;
		paramCount = (displayMode & 1) + ((displayMode & 2) >> 1) + ((displayMode & 4) >> 2) + ((displayMode & 8) >> 3);
	}
	else
	{
		columnsCount = (tracks->popupMode & 2) ? 3 : 4;
		columnsCount = (tracks->popupMode & 4) ? 2 : columnsCount;
		tracksSpace = (800 - (rightOffset + leftOffset)) / 4;
		paramCount = 4;
	}

	if(tracks->popupMode & 1)
	{
		rowCount = 8;
	}
	else
	{
		rowCount = 15;
	}

	backgroundDivider();

	calculateSelection();

	playHead();

	//API_BLEND_FUNC(SRC_ALPHA, ZERO);

	selection();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

#if ROW_NUM
	rowNumbers();
#endif

}

//------------------------------------------------------------------------------------------
void cTracker::refresh2()
{
	if(displayMode == 0 || displayMode & 1)
	{
		notes();
	}

}

void cTracker::refresh3()
{
	if(displayMode == 0 || displayMode & 2)
	{
		instruments();
	}
}

void cTracker::refresh4()
{
	if(displayMode == 0 || displayMode & 4)
	{
		//volumes();
		fxes1();

	}
}

void cTracker::refresh5()
{
	if(displayMode == 0 || displayMode & 8)
	{
		fxes2();
	}


	tracksNumbers();




	API_BLEND_FUNC(SRC_ALPHA, ZERO);
	lines();

	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}


//-------------------------------------------------------------------------------------
// podzialka
void cTracker::backgroundDivider()
{
	int16_t div_row = tracks->actualStep-7;

	//tlo calosci
	API_COLOR(colors[1]);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);

	API_VERTEX2F(posX+1, posY);
	API_VERTEX2F(width-2, posY+height-2);

	//podzialka
	API_LINE_WIDTH(16);
	API_COLOR(colors[11]);

	for(uint8_t i = 0; i < rowCount; i++)
	{
		if(div_row < 0 || div_row > tracks->patternLength-1)
		{
			div_row++;
			continue;
		}

		if(div_row % tracks->stepDevider == 0)
		{
			API_VERTEX2F(1, (i*28)+1);
			API_VERTEX2F(798, (i*28)+27);
		}

		div_row++;
	}

	API_END();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);

	// gradient-cien dolny
	API_SAVE_CONTEXT();
	API_SCISSOR_XY(1, 413);
	API_SCISSOR_SIZE(798, 10);
	API_CMD_GRADIENT(0, 413, colors[1], 0, 423, 0x0);
	API_RESTORE_CONTEXT();


	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

//-------------------------------------------------------------------------------------
// // linie
void cTracker::lines()
{
	//API_VERTEX_FORMAT(0);
//	API_COLOR(colors[0]);
//	API_LINE_WIDTH(8);
//	API_BEGIN(LINES);

	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	////API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


	// PIONOWE


	API_COLOR(colors[0]);
	API_LINE_WIDTH(1);
	API_BEGIN(RECTS);

#if ROW_NUM
	API_VERTEX2F(rightOffset-1, 0);
	API_VERTEX2F(rightOffset, 28*15);

	API_VERTEX2F(799-leftOffset-1, 0);
	API_VERTEX2F(799-leftOffset, 28*15);
#endif



	uint16_t x;
	for(uint8_t i = 0; i < columnsCount; i++)
	{
		x = (rightOffset+tracksSpace*i);
		//w = 800/columnsCount-3;
		API_VERTEX2F(x-1, 0);
		API_VERTEX2F(x, 423);

		//API_VERTEX2F(x, 0);
		//API_VERTEX2F(x+w, 28*15);

	}

	API_END();

	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

}

#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))




//-------------------------------------------------------------------------------------
// playhead
void cTracker::playHead()
{
	if(tracks->playheadPosition > tracks->actualStep-8 &&  tracks->playheadPosition < tracks->actualStep+8)
	{
		uint8_t row = tracks->playheadPosition - (tracks->actualStep-8);
		uint16_t phy1 = posY+28*(row-1);
		uint16_t phy2 = posY+28*row;
		uint16_t phx1 = 1;
		uint16_t phw = 798;
		//uint32_t playheadColor = colors[13];

		if(selectActive && tracks->playheadSelectMode)
		{
			phx1 = select1_x;
			phw = select2_x-select1_x;
		}
		else if(tracks->playheadRecMode) // pozostalosc narazie nie uzywana
		{
			//playheadColor = colors[15];

			API_COLOR(colors[15]);
			API_LINE_WIDTH(8);
			API_BEGIN(RECTS);

			if(paramCount == 1 && ((1 << tracks->selectedParam) & displayMode))// jesli jest na ekranie zaznaczony parametr
			{
				//for(uint8_t i = 0; i<8; i++)
				//{
				//	uint16_t recPosX = 1+oneParamsSelOffset[tracks->selectedParam]+(tracksSpace*i);
					uint16_t recPosX = 1+oneParamsSelOffset[tracks->selectedParam]+(tracksSpace*(tracks->actualTrack-tracks->firstVisibleTrack));
					if(recPosX < 800 && recPosX > 0)
					{
						API_VERTEX2F(recPosX, 7*28);
						API_VERTEX2F(recPosX+oneParamsSelWidth[tracks->selectedParam], 8*28-1);
					}
				//}
			}
			else if(paramCount == 2 && ((1 << tracks->selectedParam) & displayMode))
			{
				uint8_t param = whichParamFromTwoSelected();
				uint8_t offset = param==0 ? twoParamsSel1Offset[tracks->selectedParam] : twoParamsSel2Offset[tracks->selectedParam];
				uint8_t width  = param==0 ? twoParamsSel1Width[tracks->selectedParam] : twoParamsSel2Width[tracks->selectedParam];

				//for(uint8_t i = 0; i<8; i++)
				//{
					uint16_t recPosX = 1 + offset + (tracksSpace*(tracks->actualTrack-tracks->firstVisibleTrack));
					if(recPosX < 800 && recPosX > 0)
					{
						API_VERTEX2F(recPosX, 7*28);
						API_VERTEX2F(recPosX+width, 8*28-1);
					}
				//}
			}
			else if (paramCount == 4)
			{
				//for(uint8_t i = 0; i<8; i++)
				//{
					uint16_t recPosX = 1+fourParamsSelOffset[tracks->selectedParam]+(tracksSpace*(tracks->actualTrack-tracks->firstVisibleTrack));
					if(recPosX < 800 && recPosX > 0)
					{
						API_VERTEX2F(recPosX, 7*28);
						API_VERTEX2F(recPosX+fourParamsSelWidth[tracks->selectedParam], 8*28-1);
					}
				//}
			}
			API_END();


		}




		API_SAVE_CONTEXT();

		//API_LINE_WIDTH(16);
		API_SCISSOR_XY(phx1, phy1-11);
		API_SCISSOR_SIZE(phw, 11);
		API_CMD_GRADIENT(0, phy1-11, colors[1], 0, phy1, 0x0);

		API_RESTORE_CONTEXT();

		API_SAVE_CONTEXT();

		//API_LINE_WIDTH(16);
		API_SCISSOR_XY(phx1, phy2);
		API_SCISSOR_SIZE(phw, 11);
		API_CMD_GRADIENT(0, phy2, 0x0, 0, phy2+11, colors[1]);

		API_RESTORE_CONTEXT();


	}



	API_END();


}


void cTracker::calculateSelection()
{
	if(tracks->selectState == 2)
	{
		selectActive = 1;

		if(tracks->selectStartTrack > tracks->selectEndTrack)
		{
			select1_x = tracks->selectEndTrack;
			select2_x = tracks->selectStartTrack;
		}
		else
		{
			select1_x = tracks->selectStartTrack;
			select2_x = tracks->selectEndTrack;
		}

		if(tracks->selectStartStep > tracks->selectEndStep)
		{
			select1_y = tracks->selectEndStep;
			select2_y = tracks->selectStartStep;
		}
		else
		{
			select1_y = tracks->selectStartStep;
			select2_y = tracks->selectEndStep;
		}

		select1_x = select1_x - tracks->firstVisibleTrack;
		select1_x = rightOffset+select1_x*tracksSpace+1;

		select2_x = select2_x - tracks->firstVisibleTrack;
		select2_x = rightOffset+ select2_x*tracksSpace + tracksSpace - 2;

		select1_y = select1_y - tracks->actualStep + 7;
		select1_y = select1_y*28;

		select2_y = select2_y - tracks->actualStep + 7;
		select2_y = select2_y*28 + 28;
	}
	else
	{
		selectActive = 0;
	}
}
//-------------------------------------------------------------------------------------
// ZAZNACZENIE ACTUAL
void cTracker::selection()
{
	//API_BLEND_FUNC(SRC_ALPHA, ZERO);

	//--------------------
	// ZAZNACZENIE

	if(tracks->selectState == 2)
	{
		//pole pod edytowanym parametrem
		API_COLOR(colors[10]);
		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);

		if(paramCount == 1 && ((1 << tracks->selectedParam) & displayMode))// jesli jest na ekranie zaznaczony parametr
		{
			for(uint8_t i = 0; i<8; i++)
			{
				uint16_t SelPosX = select1_x+oneParamsSelOffset[tracks->selectedParam]+(tracksSpace*i);
				if(SelPosX < select2_x && SelPosX > select1_x)
				{
					API_VERTEX2F(SelPosX, select1_y+1);
					API_VERTEX2F(SelPosX+oneParamsSelWidth[tracks->selectedParam], select2_y-1);
				}
			}
		}
		else if(paramCount == 2 && ((1 << tracks->selectedParam) & displayMode))
		{
			uint8_t param = whichParamFromTwoSelected();
			uint8_t offset = param==0 ? twoParamsSel1Offset[tracks->selectedParam] : twoParamsSel2Offset[tracks->selectedParam];
			uint8_t width  = param==0 ? twoParamsSel1Width[tracks->selectedParam] : twoParamsSel2Width[tracks->selectedParam];

			for(uint8_t i = 0; i<8; i++)
			{
				uint16_t SelPosX = select1_x + offset +(tracksSpace*i);
				if(SelPosX < select2_x && SelPosX > select1_x)
				{
					API_VERTEX2F(SelPosX, select1_y+1);
					API_VERTEX2F(SelPosX+width, select2_y-1);
				}
			}
		}
		else if (paramCount == 4)
		{
			for(uint8_t i = 0; i<8; i++)
			{
				uint16_t SelPosX = select1_x+fourParamsSelOffset[tracks->selectedParam]+(tracksSpace*i);
				if(SelPosX < select2_x && SelPosX > select1_x)
				{
					API_VERTEX2F(SelPosX, select1_y+1);
					API_VERTEX2F(SelPosX+fourParamsSelWidth[tracks->selectedParam], select2_y-1);
				}
			}
		}
		API_END();

		// ramka zaznaczenia
		API_BEGIN(LINES);

		if(select1_y >= 0) // gorna
		{
			API_VERTEX2F(select1_x, select1_y);
			API_VERTEX2F(select2_x, select1_y);
		}
		else
		{
			select1_y = 0;
		}

		if(select2_y <= 28*15) // dolna
		{
			API_VERTEX2F(select2_x, select2_y);
			API_VERTEX2F(select1_x, select2_y);
		}
		else
		{
			select2_y = 28*15;
		}


		if(select2_x <= rightOffset+tracksSpace*columnsCount) // prawa
		{
			API_VERTEX2F(select2_x, select1_y);
			API_VERTEX2F(select2_x, select2_y);
		}
		if(select1_x >= rightOffset) // lewa
		{
			API_VERTEX2F(select1_x, select2_y);
			API_VERTEX2F(select1_x, select1_y);
		}

		API_END();



	}


	// aktualnie modyfikowany step
	if(tracks->selectState && tracks->actualTrack >= tracks->firstVisibleTrack && tracks->actualTrack <= tracks->firstVisibleTrack+columnsCount)
	{
		uint16_t select_x = rightOffset + (tracks->actualTrack - tracks->firstVisibleTrack) * tracksSpace + 2;
		uint16_t select1_y = posY+28*7;
		uint16_t select2_y = select1_y+27;
		uint16_t select_w = tracksSpace-5;
		uint16_t rect_select_x = 0;
		uint16_t rect_select_width = 0;

		if(paramCount == 1)
		{
			rect_select_x = select_x+oneParamsSelOffset[tracks->selectedParam];
			rect_select_width = oneParamsSelWidth[tracks->selectedParam];
		}
		else if(paramCount == 2)
		{
			uint8_t param = whichParamFromTwoSelected();
			rect_select_x = select_x + (param==0 ? twoParamsSel1Offset[tracks->selectedParam] : twoParamsSel2Offset[tracks->selectedParam]);
			rect_select_width =        (param==0 ? twoParamsSel1Width[tracks->selectedParam]  : twoParamsSel2Width[tracks->selectedParam]);
		}
		else if(paramCount == 4)
		{
			rect_select_x = select_x+fourParamsSelOffset[tracks->selectedParam];
			rect_select_width = fourParamsSelWidth[tracks->selectedParam];
		}

		API_COLOR(colors[10]);
		API_LINE_WIDTH(8);
		API_BEGIN(RECTS);
		API_VERTEX2F(rect_select_x, select1_y+1);
		API_VERTEX2F(rect_select_x+rect_select_width-1, select2_y-1);
		API_END();


		// ramka tylko w okolo aktualnego stepa
		if(tracks->selectState == 1)
		{

			API_BEGIN(LINE_STRIP);

	/*
			API_LINE_WIDTH(16);
			API_VERTEX2F(rect_select_x-2, select1_y);
			API_VERTEX2F(rect_select_x+rect_select_width, select1_y);
			API_VERTEX2F(rect_select_x+rect_select_width, select2_y);
			API_VERTEX2F(rect_select_x-2, select2_y);
			API_VERTEX2F(rect_select_x-2, select1_y);
	*/

			API_LINE_WIDTH(8);
			API_VERTEX2F(select_x, select1_y);
			API_VERTEX2F(select_x+select_w, select1_y);
			API_VERTEX2F(select_x+select_w, select2_y);
			API_VERTEX2F(select_x, select2_y);
			API_VERTEX2F(select_x, select1_y);


			API_END();

		}
	}
}

uint8_t cTracker::whichParamFromTwoSelected()
{
	if(displayMode & 1)
	{
		if(tracks->selectedParam == 0) return 0;
		else return 1;
	}
	else if(displayMode & 2)
	{
		if(tracks->selectedParam == 1) return 0;
		else return 1;
	}
	else if(displayMode & 4)
	{
		if(tracks->selectedParam == 2) return 0;
		else return 1;
	}

	return 1;
}


//-------------------------------------------------------------------------------------
// step number
void cTracker::rowNumbers()
{
	int16_t row = tracks->actualStep-6;

	API_COLOR(0x555555);
	API_BITMAP_HANDLE(fonts[0].handle);
	API_BEGIN(BITMAPS);

	uint8_t showRigt = !((tracks->popupMode & 4) | (tracks->popupMode & 2));

	for(uint16_t i = 0; i < rowCount; i++)
	{
		if(row < 1 || row > tracks->patternLength)
		{
			row++;
			continue;
		}
		Number2Bitmaps(1, (i*28)+15, 8, 18, row);
		if(showRigt) Number2Bitmaps((799-25), posY+(i*28)+15, 8, 18, row); // nie wyswietla prawego jesli popup go zaslania
		row++;
	}

	API_END();
}


//-------------------------------------------------------------------------------------
// tracks numbers
void cTracker::tracksNumbers()
{
	int16_t x, y,h ,w;
	w = 24;
	h = 27;

	//pole/tlo
	y = -1;
	API_COLOR(0xffffff);
	API_LINE_WIDTH(24);
	API_BEGIN(RECTS);

	for(uint8_t i = 0; i < columnsCount; i++)
	{
		x = rightOffset+(i*tracksSpace)+1;

		API_VERTEX2F(x, y);
		API_VERTEX2F(x+w, y+h);
	}

	API_END();

	// numer
	y = 1;
	API_COLOR(0x000000);
	API_BITMAP_HANDLE(fonts[3].handle);
	API_BEGIN(BITMAPS);

	for(uint8_t i = 0; i < columnsCount; i++)
	{
		x = rightOffset+(i*tracksSpace)+9;

		if(x > 511 || y > 511)
		{
			API_CELL(49+i+tracks->firstVisibleTrack);
			API_VERTEX2F(x, y);
		}
		else
		{
			API_VERTEX2II(x,y,fonts[3].handle, 49+i+tracks->firstVisibleTrack);
		}
	}

	API_END();
}

//===============================================================================================================================
void cTracker::notes()
{
	API_COLOR(colors[2]);
	actualColor = lastColor = colors[2];
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = rightOffset+fourParamsOffset[0];

	//if(paramCount == 1) 		offset_x = 27 + ((tracksSpace/2)-(param_length[1]*12)/2);
	if(paramCount == 1) 		offset_x = rightOffset + oneParamsOffset[0];
	else if(paramCount == 2)  	offset_x = rightOffset + twoParamsOffsetFirst[0];

	int8_t mark = -1;
	if((tracks->selectedParam == 0 /*|| tracks->selectedParam == 4*/) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t i = 0; i < columnsCount; i++)
	{
		for(uint16_t j = 0; j < rowCount; j++)
		{
			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 14+j*28;

			char* text = tracks->track[tracks->firstVisibleTrack+i].row[j].note;

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) actualColor = colors[14];
			else if(j == 7 && i == mark) actualColor = colors[14];
			else
			{
				if(tracks->inactive[tracks->firstVisibleTrack+i]) actualColor = colors[12];
				else if(*text == '-') actualColor = colors[6];
				else actualColor = colors[2];
			}

			String2Bitmaps(param_x, param_y, &fonts[1], text, param_length[0]);

			//if(change_color) API_COLOR(colors[2]);
		}
	}
	API_END();
}

//===============================================================================================================================
void cTracker::instruments()
{
	API_COLOR(colors[3]);
	actualColor = lastColor = colors[3];
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = rightOffset+fourParamsOffset[1]; // 59
	if(paramCount == 1) 		offset_x = rightOffset + oneParamsOffset[1];
	else if(paramCount == 2)  	offset_x = (displayMode & 1) ? rightOffset+twoParamsOffsetSecond[1] : rightOffset+twoParamsOffsetFirst[1];

	int8_t mark = -1;
	if((tracks->selectedParam == 1 /*|| tracks->selectedParam == 4*/) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t i = 0; i < columnsCount; i++)
	{
		for(uint16_t j = 0; j < rowCount; j++)
		{
			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 14+j*28;
			uint8_t midi_channel  =  tracks->track[tracks->firstVisibleTrack+i].row[j].instr[3];
			char* text  = tracks->track[tracks->firstVisibleTrack+i].row[j].instr;
			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) actualColor =  colors[14];
			else if(j == 7 && i == mark) actualColor =  colors[14];
			else
			{
				if(tracks->inactive[tracks->firstVisibleTrack+i]) actualColor = colors[12];
				else if(*text == '-') actualColor = colors[7];
				else actualColor = colors[3];
			}

			String2Bitmaps(param_x, param_y, (midi_channel)?&fonts[0]:&fonts[1], text, param_length[1]);

			//if(change_color) API_COLOR(colors[3]);
			//if(midi_channel > 0)	API_BITMAP_HANDLE(fonts[1].handle);
		}
	}
	API_END();
}

//===============================================================================================================================
void cTracker::fxes1()
{
	API_COLOR(colors[4]);
	actualColor = lastColor = colors[4];
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = rightOffset+fourParamsOffset[2]; // 99
	if(paramCount == 1) 		offset_x = rightOffset + oneParamsOffset[2];
	else if(paramCount == 2)  	offset_x = (displayMode & 8) ? rightOffset+twoParamsOffsetFirst[2] : rightOffset+twoParamsOffsetSecond[2];

	int8_t mark = -1;
	if((tracks->selectedParam == 2 /*|| tracks->selectedParam == 4*/) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t i = 0; i < columnsCount; i++)
	{
		for(uint16_t j = 0; j < rowCount; j++)
		{

			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 14+j*28;
			char* text = &tracks->track[tracks->firstVisibleTrack+i].row[j].fx[0][0];

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) actualColor =  colors[14];
			else if(j == 7 && i == mark) actualColor =  colors[14];
			else
			{
				if(tracks->inactive[tracks->firstVisibleTrack+i]) actualColor = colors[12];
				else if(*text == '-') actualColor = colors[8];
				else actualColor = colors[4];
			}
			String2Bitmaps(param_x, param_y, &fonts[1],  text, param_length[3]);

		}
	}

}
void cTracker::fxes2()
{
	API_COLOR(colors[5]);
	actualColor = lastColor = colors[5];
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = rightOffset+fourParamsOffset[3];
	if(paramCount == 1) 		offset_x = rightOffset + oneParamsOffset[3];
	else if(paramCount == 2)  	offset_x = rightOffset + twoParamsOffsetSecond[3];

	int8_t mark = -1;
	if((tracks->selectedParam == 3 /*|| tracks->selectedParam == 4*/) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t i = 0; i < columnsCount; i++)
	{
		for(uint16_t j = 0; j < rowCount; j++)
		{

			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 14+j*28;
			char* text  = &tracks->track[tracks->firstVisibleTrack+i].row[j].fx[1][0];

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) actualColor =  colors[14];
			else if(j == 7 && i == mark) actualColor =  colors[14];
			else
			{
				if(tracks->inactive[tracks->firstVisibleTrack+i]) actualColor = colors[12];
				else if(*text == '-') actualColor = colors[9];
				else actualColor = colors[5];
			}

			String2Bitmaps(param_x, param_y, &fonts[1], text, param_length[3]);
		}
	}
}
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
inline void draw_char(uint16_t x, uint16_t y, uint8_t charr)
{
	if(x > 511 || y > 511)
	{
		API_CELL(charr);
		API_VERTEX2F(x, y);
	}
	else
	{
		API_VERTEX2II(x,y,fonts[0].handle,charr);
	}
}


void cTracker::String2Bitmaps(int16_t x, int16_t y, const strFont* font, char* string, int8_t length)
{
	if(actualColor != lastColor)
	{
		API_COLOR(actualColor);
		lastColor = actualColor;
	}

	y = y - font->height/2;
	uint8_t strPtr = 0;

	x+=fonts[1].width-font->width;

	for(uint8_t i = 0; i < length; i++)
	{
		if(x > 799 || y > 480 || x < 0 || y < 0)
		{
			strPtr++;
		}
		else if((x > 511 || y > 511) && string[strPtr] >=32)
		{
			if(lastHandle != font->handle)
			{
				API_BITMAP_HANDLE(font->handle);
				lastHandle = font->handle;
			}
			API_CELL(string[strPtr++]);
			API_VERTEX2F(x, y);
		}
		else if(string[strPtr] >=32)
		{
			API_VERTEX2II(x,y,font->handle, (char)string[strPtr++]);
		}
		x+=font->width;
	}
}


void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number)
{
	y = y - font_y/2;
	uint8_t digit, flag = 0;

	if(number < 0)
	{
		number = number*(-1);
		draw_char(x,y,45);
		x+=font_x;
	}


	digit =  number/10000;
	number = number%10000;
	if(digit > 0) {draw_char(x,y,digit+48); x+=font_x; flag = 1;}

	digit =  number/1000;
	number = number%1000;
	if(digit > 0 || flag) {draw_char(x,y,digit+48); x+=font_x; flag = 1;}

	digit =  number/100;
	number = number%100;
	if(digit > 0 || flag) {draw_char(x,y,digit+48); x+=font_x; flag = 1;}

	digit =  number/10;
	number = number%10;
	if(digit > 0 || flag) {draw_char(x,y,digit+48); x+=font_x; flag = 1;}

	draw_char(x,y,number+48);

}
