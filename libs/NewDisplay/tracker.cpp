
#include "FT812.h"

#include "trackerControl.h"

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

static uint32_t defaultColors[] =

{
	0xFFFFFF, // linie
	0xFFFFFF, // numery wierszy
	0xFFFFFF, // nuta
	0xFFFFFF, // instrument
	0xFFFFFF, // volume
	0xFFFFFF, // effekt
	0xFF0000, // zaznaczenie
	0x111111, // podzialka
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cTracker::cTracker(strControlProperties* properties)
{
	//visibleCharOffset = 0;
	//firstVisibleTrack = 0;
	//visibleTracksOffset = 0;

	colorsCount = 7;
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

    switch(refreshStep)
    {
    case 0: refresh1(); break;
    case 1: refresh2(); break;
    case 2: refresh3(); break;
    case 3: refresh4(); break;
    case 4: refresh5(); break;
    default: refreshStep = 0; break;
    }

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

	return 0;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cTracker::refresh1()
{
	colors[6] = tracks->selectColor;
	displayMode = value;


	if(displayMode > 0)
	{
		tracksSpace = (800 - 2*28) / 8;
		columnsCount = 8;
		paramCount = (displayMode & 1) + ((displayMode & 2) >> 1) + ((displayMode & 4) >> 2) + ((displayMode & 8) >> 3);
	}
	else
	{
		columnsCount = 4;
		tracksSpace = (800 - 2*28) / 4;
		paramCount = 4;
	}


	backgroundDivider();
	lines();
	playHead();
	selection();
	rowNumbers();
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
		volumes();
	}
}

void cTracker::refresh5()
{
	if(displayMode == 0 || displayMode & 8)
	{
		fxes();
	}

	tracksNumbers();
}


//-------------------------------------------------------------------------------------
// podzialka
void cTracker::backgroundDivider()
{
	int16_t div_row = tracks->actualStep-7;

	API_COLOR(colors[7]);
	//API_BLEND_FUNC(DST_ALPHA , ZERO);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);

	for(uint16_t i = 0; i < 15; i++)
	{
		if(div_row < 0 || div_row > tracks->patternLength-1)
		{
			div_row++;
			continue;
		}

		if(div_row % tracks->stepDevider == 0)
		{
			API_VERTEX2F(0, (i*28)+1);
			API_VERTEX2F(799, (i*28)+27);
		}

		div_row++;
	}

	API_END();
	//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

//-------------------------------------------------------------------------------------
// // linie
void cTracker::lines()
{
	//API_VERTEX_FORMAT(0);
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);

	// PIONOWE
	API_VERTEX2F(27, 0);
	API_VERTEX2F(27, 28*15);

	API_VERTEX2F(799-28, 0);
	API_VERTEX2F(799-28, 28*15);

	uint16_t x;
	for(uint8_t i = 1; i < columnsCount; i++)
	{
		x = (27+tracksSpace*i);
		API_VERTEX2F(x, 0);
		API_VERTEX2F(x, 28*15);
	}

	// POZIOME
	//API_VERTEX2F(0, posY+25);
	//API_VERTEX2F(799, posY+25);
}


//-------------------------------------------------------------------------------------
// playhead
void cTracker::playHead()
{
	if(tracks->playheadPosition > tracks->actualStep-8 &&  tracks->playheadPosition < tracks->actualStep+8)
	{
		uint8_t row = tracks->playheadPosition - (tracks->actualStep-8);

		API_VERTEX2F(0, posY+28*(row-1));
		API_VERTEX2F(799, posY+28*(row-1));

		API_VERTEX2F(0, posY+28*row);
		API_VERTEX2F(799, posY+28*row);
	}

	API_END();
}


//-------------------------------------------------------------------------------------
// ZAZNACZENIE ACTUAL
void cTracker::selection()
{
	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

	//--------------------
	// ZAZNACZENIE

	//if(tracks->selectState
	//   && (tracks->selectStartTrack != tracks->selectEndTrack || tracks->selectStartStep != tracks->selectEndStep))
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
		select1_x = 27+select1_x*tracksSpace;

		select2_x = select2_x - tracks->firstVisibleTrack;
		select2_x = 27+ select2_x*tracksSpace + tracksSpace;

		select1_y = select1_y - tracks->actualStep + 7;
		select1_y = select1_y*28;

		select2_y = select2_y - tracks->actualStep + 7;
		select2_y = select2_y*28 + 28;


		API_COLOR(colors[6]);
		API_LINE_WIDTH(12);
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


		if(select2_x <= 27+tracksSpace*columnsCount) // prawa
		{
			API_VERTEX2F(select2_x, select1_y);
			API_VERTEX2F(select2_x, select2_y);
		}
		if(select1_x >= 27) // lewa
		{
			API_VERTEX2F(select1_x, select2_y);
			API_VERTEX2F(select1_x, select1_y);
		}

		API_END();
	}
	else selectActive = 0;



	// aktualnie modyfikowany step
	if(tracks->selectState && tracks->actualTrack >= tracks->firstVisibleTrack && tracks->actualTrack <= tracks->firstVisibleTrack+columnsCount)
	{
		uint16_t select_x = 27;
		uint16_t select_w = tracksSpace;


		select_x = 27 + (tracks->actualTrack - tracks->firstVisibleTrack) * tracksSpace;


		API_COLOR(colors[6]);
		API_LINE_WIDTH(20);
		API_BEGIN(LINE_STRIP);

		API_VERTEX2F(select_x, posY+28*7);
		API_VERTEX2F(select_x+select_w, posY+28*7);
		API_VERTEX2F(select_x+select_w, posY+28*8);
		API_VERTEX2F(select_x, posY+28*8);
		API_VERTEX2F(select_x, posY+28*7);
		API_END();
	}


	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

//-------------------------------------------------------------------------------------
// step number
void cTracker::rowNumbers()
{
	int16_t row = tracks->actualStep-6;

	API_COLOR(colors[1]);
	API_BITMAP_HANDLE(fonts[0].handle);
	API_BEGIN(BITMAPS);

	for(uint16_t i = 0; i < 15; i++)
	{
		if(row < 1 || row > tracks->patternLength)
		{
			row++;
			continue;
		}
		Number2Bitmaps(0, (i*28)+15, 8, 18, row);
		Number2Bitmaps((799-25), posY+(i*28)+15, 8, 18, row);
		row++;
	}

	API_END();
}


//-------------------------------------------------------------------------------------
// tracks numbers
void cTracker::tracksNumbers()
{
	uint16_t x, y,h ,w;

	w = 19;
	h = 28;

	for(uint8_t i = 0; i < columnsCount; i++)
	{
		x = 27+(i*tracksSpace);
		y = 0;

		//pole/tlo
		API_COLOR(0x000000);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(x, y);
		API_VERTEX2F(x+w, y+h);
		API_END();

		//ramka
		API_COLOR(colors[0]);
		API_LINE_WIDTH(8);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(x, y);
		API_VERTEX2F(x + w, y);
		API_VERTEX2F(x + w, y + h);
		API_VERTEX2F(x, y + h);
		API_VERTEX2F(x, y);
		API_END();


		API_COLOR(colors[1]);

		API_BITMAP_HANDLE(fonts[1].handle);
		API_BEGIN(BITMAPS);

		x += 3;
		y = 1;


		if(x > 511 || y > 511)
		{
			API_CELL(49+i+tracks->firstVisibleTrack);
			API_VERTEX2F(x, y);
		}
		else
		{
			API_VERTEX2II(x,y,fonts[1].handle, 49+i+tracks->firstVisibleTrack);
		}
		API_END();

	}

}

//===============================================================================================================================
void cTracker::notes()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[2]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = 27+9;

	//if(paramCount == 1) 		offset_x = 27 + ((tracksSpace/2)-(param_length[1]*12)/2);
	if(paramCount == 1) 		offset_x = 27 + ((tracksSpace/2)-18);
	else if(paramCount == 2)  	offset_x = 27 + 9;

	int8_t mark = -1;
	if((tracks->selectedParam == 0 || tracks->selectedParam == 4) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < columnsCount; i++)
		{
			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 15+j*28;
			uint8_t change_color = 0;

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) change_color = 1;
			else if(j == 7 && i == mark) change_color = 1;

			if(change_color) API_COLOR(colors[6]);

			String2Bitmaps(param_x, param_y, 12, 26, tracks->track[tracks->firstVisibleTrack+i].row[j].note, param_length[0]);

			if(change_color) API_COLOR(colors[2]);
		}
	}
	API_END();
}

//===============================================================================================================================
void cTracker::instruments()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[3]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = 27+59;
	if(paramCount == 1) 		offset_x = 27 + ((tracksSpace/2)-12);
	else if(paramCount == 2)  	offset_x = (displayMode & 1) ? 27+50 : 27+9;

	int8_t mark = -1;
	if((tracks->selectedParam == 1 || tracks->selectedParam == 4) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < columnsCount; i++)
		{
			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 15+j*28;
			uint8_t change_color = 0;

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) change_color = 1;
			else if(j == 7 && i == mark) change_color = 1;

			if(change_color) API_COLOR(colors[6]);

			String2Bitmaps(param_x, param_y, 12, 26, tracks->track[tracks->firstVisibleTrack+i].row[j].instr, param_length[1]);

			if(change_color) API_COLOR(colors[3]);
		}
	}
	API_END();
}

//===============================================================================================================================
void cTracker::volumes()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[4]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = 27+99;
	if(paramCount == 1) 		offset_x = 27 + ((tracksSpace/2)-12);
	else if(paramCount == 2)  	offset_x = (displayMode & 8) ? 27+9 : 27+50;

	int8_t mark = -1;
	if((tracks->selectedParam == 2 || tracks->selectedParam == 4) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < columnsCount; i++)
		{
			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 15+j*28;
			uint8_t change_color = 0;

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) change_color = 1;
			else if(j == 7 && i == mark) change_color = 1;

			if(change_color) API_COLOR(colors[6]);

			String2Bitmaps(param_x, param_y, 12, 26, tracks->track[tracks->firstVisibleTrack+i].row[j].vol, param_length[2]);

			if(change_color) API_COLOR(colors[4]);
		}
	}
	API_END();
}

//===============================================================================================================================
void cTracker::fxes()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[5]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	uint8_t offset_x = 27+136;
	if(paramCount == 1) 		offset_x = 27 + ((tracksSpace/2)-18);
	else if(paramCount == 2)  	offset_x = 27 + 50;

	int8_t mark = -1;
	if((tracks->selectedParam == 3 || tracks->selectedParam == 4) && tracks->selectState)
	{
		mark = tracks->actualTrack - tracks->firstVisibleTrack;
	}

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < columnsCount; i++)
		{
			int16_t param_x = offset_x+i*tracksSpace;
			int16_t param_y = 15+j*28;
			uint8_t change_color = 0;

			if(selectActive && mark>=0 && param_x > select1_x && param_x < select2_x && param_y > select1_y && param_y < select2_y) change_color = 1;
			else if(j == 7 && i == mark) change_color = 1;

			if(change_color) API_COLOR(colors[6]);

			String2Bitmaps(param_x, param_y, 12, 26, tracks->track[tracks->firstVisibleTrack+i].row[j].fx, param_length[3]);

			if(change_color) API_COLOR(colors[5]);
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

void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length)
{
	y = y - font_y/2;
	uint8_t strPtr = 0;


	for(uint8_t i = 0; i < length; i++)
	{
		if(x > 755 || y > 480 || x < 30 || y < 0)
		{
			strPtr++;
		}
		else if((x > 511 || y > 511) && string[strPtr] >=32)
		{
			API_CELL(string[strPtr++]);
			API_VERTEX2F(x, y);
		}
		else if(string[strPtr] >=32)
		{
			API_VERTEX2II(x,y,fonts[1].handle, (char)string[strPtr++]);
		}
		x+=font_x;
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
