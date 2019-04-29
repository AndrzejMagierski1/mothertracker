
#include "FT812.h"


#include "displayControls.h"


uint8_t cTracker::colorsCount = 6;
uint32_t cTracker::defaultColors[] =
{
	0xFFFFFF, // linie
	0xFFFFFF, // numery wierszy
	0xFF0000, // nuta
	0x00FF00, // instrument
	0x0000FF, // volume
	0xFFFF30, // effekt
};


struct strTrackerSeqDisplay
{
	struct strTracks
	{
		struct strRow
		{
			char note[4] = "C-1";
			char instr[3] = "02";
			char vol[3] = "80";
			char fx[4] = "C38";

		} row[21];

	} track[8];

	uint16_t position = 201;

} trackerSeqDisplay;



cTracker::cTracker(char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	cState = state;
	posX = x;
	posY = y;
	refreshStep = 0;
}

cTracker::~cTracker()
{

}

//--------------------------------------------------------------------------------
uint8_t cTracker::update()
{
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
	case 0: API_CMD_MEMCPY(address, 				RAM_DL, dlOffset); break;
	case 1: API_CMD_MEMCPY(address+ramPartSize[0], 	RAM_DL, dlOffset); break;
	case 2: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1], 	RAM_DL, dlOffset); break;
	case 3: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2], 	RAM_DL, dlOffset); break;
	case 4: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3], 	RAM_DL, dlOffset); break;
	default: break;
	}

	ramPartSize[refreshStep] = dlOffset;

	API_LIB_EndCoProList();
	API_LIB_AwaitCoProEmpty();

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
	uint16_t length = ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3]+ramPartSize[4];
	API_CMD_APPEND(address		, 4000);




	//API_CMD_APPEND(address+ 1000, ramPartSize[1]);
	//API_CMD_APPEND(address+ 4000, ramPartSize[2]);
	//API_CMD_APPEND(address+ 6000, ramPartSize[3]);
	//API_CMD_APPEND(address+ 8000, ramPartSize[4]);

	return 0;
}

//---------------------------------------------------------------------------------
void cTracker::refresh1()
{

	// linie
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);

	for(uint16_t i = posX+24; i < 800; i+=94)
	{
		API_VERTEX2F(i, 0);
		API_VERTEX2F(i, 479);
	}

	API_VERTEX2F(0, posY+20*10);
	API_VERTEX2F(799, posY+20*10);

	API_VERTEX2F(0, posY+20*10+20);
	API_VERTEX2F(799, posY+20*10+20);

	API_END();


	uint8_t row = trackerSeqDisplay.position;

	API_COLOR(colors[1]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BEGIN(BITMAPS);


	for(uint16_t i = 0; i < 21; i++)
	{
		Number2Bitmaps(posX+0, (i*20)+10, 7, 13, row);
		Number2Bitmaps(posX+(799-19), posY+(i*20)+10, 7, 13, row);

		//API_CMD_NUMBER(posX+(799-21) , posY+(i*20), MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), row);
		//API_CMD_NUMBER(posX+0 , posY+(i*20), MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), row);

		row++;
	}


	API_END();
}

void cTracker::refresh2()
{

	API_VERTEX_FORMAT(0);
	API_COLOR(colors[2]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BEGIN(BITMAPS);


	uint8_t track = 0;
	uint8_t row = 0;

	for(uint16_t j = 10; j < (20*21); j+=20)
	{
		for(uint16_t i = 26; i < 93*8; i+=94)
		{
			Char2Bitmaps(i, j, 8, 18, trackerSeqDisplay.track[track].row[row].note, 3);
			track++;
		}
		row++;
		track = 0;
	}
	//API_CMD_TEXT  ((i) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "C#1");
	API_END();
}

void cTracker::refresh3()
{
	uint8_t track = 0;
	uint8_t row = 0;


	API_VERTEX_FORMAT(0);
	API_COLOR(colors[3]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BEGIN(BITMAPS);

	for(uint16_t y = 10; y < (20*21); y+=20)
	{
		for(uint16_t x = 26+28; x < 93*8; x+=94)
		{
			Char2Bitmaps(x, y, 8, 18, trackerSeqDisplay.track[track].row[row].instr, 2);
			track++;
		}
		row++;
		track = 0;
	}
	/*
	API_COLOR(colors[3]);
	for(uint16_t i = 22; i < 93*8; i+=95)
	{
			API_CMD_TEXT  ((i+28) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "01");
			API_CMD_TEXT  ((i+27) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "03");
	}
	*/
	API_END();
}

void cTracker::refresh4()
{
	uint8_t track = 0;
	uint8_t row = 0;


	API_VERTEX_FORMAT(0);
	API_COLOR(colors[4]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BEGIN(BITMAPS);

	for(uint16_t y = 10; y < (20*21); y+=20)
	{
		for(uint16_t x = 26+45; x < 93*8; x+=94)
		{
			Char2Bitmaps(x, y, 8, 18, trackerSeqDisplay.track[track].row[row].vol, 2);
			track++;
		}
		row++;
		track = 0;
	}
	API_END();
	/*
	API_COLOR(colors[4]);
	for(uint16_t i = 22; i < 93*8; i+=95)
	{
			API_CMD_TEXT  ((i+45) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "00");
			API_CMD_TEXT  ((i+45) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "60");
			//API_CMD_TEXT  ((i+45) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "99");
	}
	*/
}

void cTracker::refresh5()
{
	uint8_t track = 0;
	uint8_t row = 0;


	API_VERTEX_FORMAT(0);
	API_COLOR(colors[5]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT1_HANDLE);
	API_BEGIN(BITMAPS);

	for(uint16_t y = 10; y < (20*21); y+=20)
	{
		for(uint16_t x = 26+64; x < 93*8; x+=94)
		{
			Char2Bitmaps(x, y, 8, 18, trackerSeqDisplay.track[track].row[row].fx, 3);
			track++;
		}
		row++;
		track = 0;
	}
	API_END();
	/*
		API_COLOR(colors[5]);
		for(uint16_t i = 22; i < 93*8; i+=95)
		{

				API_CMD_TEXT  ((i+64) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "H43");
				API_CMD_TEXT  ((i+64) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "A30");
				//API_CMD_TEXT  ((i+64) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "B12");
		}
	*/
}

