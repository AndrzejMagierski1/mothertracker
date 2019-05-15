
#include "FT812.h"

#include "trackerControl.h"

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);


static uint32_t defaultColors[] =
{
	0xFFFFFF, // linie
	0xFFFFFF, // numery wierszy
	0xFF0000, // nuta
	0x00FF00, // instrument
	0x0000FF, // volume
	0xFFFF30, // effekt
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cTracker::cTracker(strControlProperties* properties)
{
	visibleCharOffset = 0;
	firstVisibleTrack = 0;
	visibleTracksOffset = 0;

	colorsCount = 6;
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
		return;
	}

	tracks = (strTrackerSeqDisplay*)properties->data;
	posX = properties->x;
	posY = properties->y;
	width = properties->w;
	height = properties->y;

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
	tracks = (strTrackerSeqDisplay*)data;
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
	posX = tracks->part;

	firstVisibleTrack = tracks->part/186;
	visibleTracksOffset = tracks->part%186;
	visibleCharOffset = visibleTracksOffset/12;  // font width = 12

	//API_BLEND_FUNC(SRC_ALPHA, ZERO);
	//API_COLOR_A(128);

	// linie
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);


	// PIONOWE
	API_VERTEX2F(27, 0);
	API_VERTEX2F(27, 28*15+25);

	API_VERTEX2F(27+186*4, 0);
	API_VERTEX2F(27+186*4, 28*15+25);


	uint16_t x;
	for(uint8_t i = 1; i < 5; i++)
	{
		x = (27+186*i)-(visibleTracksOffset);
		API_VERTEX2F(x, 0);
		API_VERTEX2F(x, 28*15+25);
	}


	// POZIOME
	API_VERTEX2F(0, posY+29);
	API_VERTEX2F(799, posY+29);

	API_VERTEX2F(0, posY+28*7+25);
	API_VERTEX2F(799, posY+28*7+25);

	API_VERTEX2F(0, posY+28*8+25);
	API_VERTEX2F(799, posY+28*8+25);

	API_VERTEX2F(0, posY+28*15+25);
	API_VERTEX2F(799, posY+28*15+25);

	API_END();


	uint8_t row = tracks->position;

	API_COLOR(colors[1]);
	API_BITMAP_HANDLE(fonts[0].handle);
	API_BEGIN(BITMAPS);


	for(uint16_t i = 0; i < 15; i++)
	{
		Number2Bitmaps(0, (i*28)+40, 8, 18, row);

		Number2Bitmaps((799-25), posY+(i*28)+40, 8, 18, row);

		row++;
	}

	//API_SAVE_CONTEXT();


	//API_SCISSOR_XY(28, 0);
	//API_SCISSOR_SIZE(799-28*2, 28*15+24);


	API_BITMAP_HANDLE(fonts[1].handle);

	#define TRACK_NAME_L 7
	char trackName[TRACK_NAME_L+1] = "Track 1";
	trackName[TRACK_NAME_L-1] = 49+firstVisibleTrack;


	int8_t text_length = (TRACK_NAME_L-visibleCharOffset >= 0) ? TRACK_NAME_L-visibleCharOffset : 0;

	String2Bitmaps(33-visibleTracksOffset+12*(7-text_length), 14, 12, 26, trackName+(7-text_length), text_length);


	for(uint8_t i = 1; i < 5; i++)
	{
		trackName[6] = 49+i+firstVisibleTrack;
		String2Bitmaps(33+(i*186)-visibleTracksOffset, 14, 12, 26, trackName, 7);
	}




	API_END();
}

//------------------------------------------------------------------------------------------
void cTracker::refresh2()
{

	API_VERTEX_FORMAT(0);
	API_COLOR(colors[2]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < 5; i++)
		{
			String2Bitmaps(36+i*186-visibleTracksOffset, 40+j*28, 12, 26, tracks->track[firstVisibleTrack+i].row[j].note, 3);
		}
	}
	API_END();
}

void cTracker::refresh3()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[3]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < 5; i++)
		{
			String2Bitmaps(36+50+i*186-visibleTracksOffset, 40+j*28, 12, 26, tracks->track[firstVisibleTrack+i].row[j].instr, 2);
		}
	}
	API_END();
}

void cTracker::refresh4()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[4]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < 5; i++)
		{
			String2Bitmaps(33+90+i*186-visibleTracksOffset, 40+j*28, 12, 26, tracks->track[firstVisibleTrack+i].row[j].vol, 2);
		}
	}
	API_END();
}

void cTracker::refresh5()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[5]);
	API_BITMAP_HANDLE(fonts[1].handle);
	API_BEGIN(BITMAPS);

	for(uint16_t j = 0; j < 15; j++)
	{
		for(uint16_t i = 0; i < 5; i++)
		{
			String2Bitmaps(33+130+i*186-visibleTracksOffset, 40+j*28, 12, 26, tracks->track[firstVisibleTrack+i].row[j].fx, 3);
		}
	}
	API_END();
}

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
		else if(x > 511 || y > 511)
		{
			API_CELL(string[strPtr++]);
			API_VERTEX2F(x, y);
		}
		else
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
