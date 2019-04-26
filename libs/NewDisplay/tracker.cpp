
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



cTracker::cTracker(char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	cState = state;
	posX = x;
	posY = y;

}

cTracker::~cTracker()
{

}


uint8_t cTracker::update()
{
    API_LIB_BeginCoProList();
    API_CMD_DLSTART();


	// linie
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);


	//API_VERTEX2II(20, 0, 0, 0);
	//API_VERTEX2II(20, 479, 0, 0);


	for(uint16_t i = posX+20; i < 800; i+=95)
	{
		API_VERTEX2F(i*16, 0*16);
		API_VERTEX2F(i*16, 479*16);

	}

	API_VERTEX2F(0*16, (posY+230)*16);
	API_VERTEX2F(799*16, (posY+230)*16);

	API_VERTEX2F(0*16, (posY+250)*16);
	API_VERTEX2F(799*16, (posY+250)*16);

	API_END();




	uint8_t row = 201;

	API_COLOR(colors[1]);

	for(uint16_t i = 1; i < 24; i++)
	{


			API_CMD_NUMBER(posX+(799-21) , posY+(i*20), MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), row);
			API_CMD_NUMBER(posX+0 , posY+(i*20), MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), row);

			row++;
	}

/*
	API_COLOR(colors[2]);
	for(uint16_t i = 22; i < 93*8; i+=95)
	{

			API_CMD_TEXT  ((i) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "C#1");
			API_CMD_TEXT  ((i) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "D-2");
			//API_CMD_TEXT  ((i) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "F#3");

	}

	API_COLOR(colors[3]);
	for(uint16_t i = 22; i < 93*8; i+=95)
	{

			API_CMD_TEXT  ((i+28) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "01");
			API_CMD_TEXT  ((i+27) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "03");
			//API_CMD_TEXT  ((i+28) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "18");

	}

	API_COLOR(colors[4]);
	for(uint16_t i = 22; i < 93*8; i+=95)
	{

			API_CMD_TEXT  ((i+45) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "00");
			API_CMD_TEXT  ((i+45) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "60");
			//API_CMD_TEXT  ((i+45) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "99");

	}

	API_COLOR(colors[5]);
	for(uint16_t i = 22; i < 93*8; i+=95)
	{

			API_CMD_TEXT  ((i+64) , 220, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "H43");
			API_CMD_TEXT  ((i+64) , 240, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "A30");
			//API_CMD_TEXT  ((i+64) , 260, MT_GPU_RAM_FONT1_HANDLE, (OPT_CENTERY), "B12");
	}

*/

    API_LIB_EndCoProList();

	//updateAdress = ramAddress.trackTable;
	//updateSize = &ramSize.trackTable;





	return 1;
}
