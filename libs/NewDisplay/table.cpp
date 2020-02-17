
#include "FT812.h"



//=============================================================================
// STRUKTURY

uint32_t colors[] =
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
			char fx[4] = "Z38";

		} row[21];

	} track[8];

	uint16_t position = 201;

} tableDisplay;

//=============================================================================
// CZCIONKI
#define MT_GPU_RAM_FONT1_ADRESS	1000
#define MT_GPU_RAM_FONT1_HANDLE	13
#define MT_GPU_RAM_FONT2_ADRESS	11000
#define MT_GPU_RAM_FONT2_HANDLE	14
//

//=============================================================================
void NumberToBitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void StringToBitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

void refresh1();
void refresh2();
void refresh3();
void refresh4();
void refresh5();


extern const uint8_t Roboto_Mono_10_L4[];
extern const uint8_t Roboto_Mono_14_L4[];

uint8_t step   = 0;
uint32_t dlOffsets[5];
uint32_t freeRAMCmd;



//=============================================================================
void display_table()
{
	if(step == 0)
	{
		//-------------------------------------------------
		// stworzenie DL z 1 czescia danych
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();
		refresh1();
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		// skopiowanie DL z czescia 1 danych do pamieci g_ram pod adress 100000
		dlOffsets[0] = EVE_MemRead32(REG_CMD_DL);
		API_LIB_BeginCoProList();
		API_CMD_MEMCPY(100000, RAM_DL, dlOffsets[0]);
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		//-------------------------------------------------
		// stworzenie DL z 2 czescia danych
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();
		refresh2();
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		// skopiowanie DL z czescia 1 danych do pamieci g_ram pod adress 105000
		dlOffsets[1] = EVE_MemRead32(REG_CMD_DL);
		API_LIB_BeginCoProList();
		API_CMD_MEMCPY(100000+dlOffsets[0], RAM_DL, dlOffsets[1]);
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		//-------------------------------------------------
		// stworzenie DL z 3 czescia danych
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();
		refresh3();
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		// skopiowanie DL z czescia 1 danych do pamieci g_ram pod adress 110000
		dlOffsets[2] = EVE_MemRead32(REG_CMD_DL);
		API_LIB_BeginCoProList();
		API_CMD_MEMCPY(100000+dlOffsets[0]+dlOffsets[1], RAM_DL, dlOffsets[2]);
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		//-------------------------------------------------
		// stworzenie DL z 4 czescia danych
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();
		refresh4();
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		// skopiowanie DL z czescia 1 danych do pamieci g_ram pod adress 115000
		dlOffsets[3] = EVE_MemRead32(REG_CMD_DL);
		API_LIB_BeginCoProList();
		API_CMD_MEMCPY(100000+dlOffsets[0]+dlOffsets[1]+dlOffsets[2], RAM_DL, dlOffsets[3]);
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		//-------------------------------------------------
		// stworzenie DL z 5 czescia danych
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();
		refresh5();
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		// skopiowanie DL z czescia 1 danych do pamieci g_ram pod adress 120000
		dlOffsets[4] = EVE_MemRead32(REG_CMD_DL);
		API_LIB_BeginCoProList();
		API_CMD_MEMCPY(100000+dlOffsets[0]+dlOffsets[1]+dlOffsets[2]+dlOffsets[3], RAM_DL, dlOffsets[4]);
		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();

		step=1;
	}
	else
	{
		//-------------------------------------------------
		// wyswietlenie ostatecznej DL
		API_LIB_BeginCoProList();
		API_CMD_DLSTART();

		API_CLEAR_COLOR(0x000000);
		API_CLEAR(1,1,1);

		//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

		API_CMD_APPEND(100000, 														dlOffsets[0]);
		API_CMD_APPEND(100000+ dlOffsets[0], 										dlOffsets[1]);
		API_CMD_APPEND(100000+ dlOffsets[0]+dlOffsets[1], 							dlOffsets[2]);

		// tylko z powyzszymi dziala porpawnie
		// z ponizszymi wyswietla błędnie
		// (kolejnosc mozna zmieniac dowlnie, na bład wpltywa tylko ilosc danych)

		API_CMD_APPEND(100000+ dlOffsets[0]+dlOffsets[1]+dlOffsets[2], 				dlOffsets[3]);
		API_CMD_APPEND(100000+ dlOffsets[0]+dlOffsets[1]+dlOffsets[2]+dlOffsets[3], dlOffsets[4]);


		// zastepcza wersja działa tak samo
		//API_CMD_APPEND(100000, dlOffsets[0]+dlOffsets[1]+dlOffsets[2]+dlOffsets[3]+dlOffsets[4]);

		//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

		API_DISPLAY();
		API_CMD_SWAP();

		API_LIB_EndCoProList();
		API_LIB_AwaitCoProEmpty();


	}





}

//=============
void refresh1()
{
	// linie
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[0]);
	API_LINE_WIDTH(8);
	API_BEGIN(LINES);

	for(uint16_t i = 24; i < 800; i+=94)
	{
		API_VERTEX2F(i, 0);
		API_VERTEX2F(i, 479);
	}

	API_VERTEX2F(0, 20*8);
	API_VERTEX2F(799, 20*8);

	API_VERTEX2F(0, 20*8+20);
	API_VERTEX2F(799, 20*8+20);

	API_END();

	uint8_t row = tableDisplay.position;

	API_COLOR(colors[1]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BEGIN(BITMAPS);


	for(uint16_t i = 0; i < 17; i++)
	{
		NumberToBitmaps(0, (i*20)+10, 7, 13, row);
		NumberToBitmaps((799-19), (i*20)+10, 7, 13, row);

		row++;
	}

	API_END();
}
//=============
void refresh2()
{
	API_VERTEX_FORMAT(0);
	API_COLOR(colors[2]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BEGIN(BITMAPS);

	uint8_t track = 0;
	uint8_t row = 0;

	for(uint16_t j = 10; j < (20*17); j+=20)
	{
		for(uint16_t i = 26; i < 93*8; i+=94)
		{
			StringToBitmaps(i, j, 8, 18, tableDisplay.track[track].row[row].note, 3);
			//API_CMD_TEXT(i , j, MT_GPU_RAM_FONT2_HANDLE, (OPT_CENTERY), tableDisplay.track[track].row[row].note);
			track++;
		}
		row++;
		track = 0;
	}
	API_END();
}
//=============
void refresh3()
{
	uint8_t track = 0;
	uint8_t row = 0;

	API_VERTEX_FORMAT(0);
	API_COLOR(colors[3]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BEGIN(BITMAPS);

	for(uint16_t y = 10; y < (20*17); y+=20)
	{
		for(uint16_t x = 26+28; x < 93*8; x+=94)
		{
			StringToBitmaps(x, y, 8, 18, tableDisplay.track[track].row[row].instr, 2);
			track++;
		}
		row++;
		track = 0;
	}
	API_END();
}

//=============
void refresh4()
{
	uint8_t track = 0;
	uint8_t row = 0;

	API_VERTEX_FORMAT(0);
	API_COLOR(colors[4]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BEGIN(BITMAPS);

	for(uint16_t y = 10; y < (20*17); y+=20)
	{
		for(uint16_t x = 26+45; x < 93*8; x+=94)
		{
			StringToBitmaps(x, y, 8, 18, tableDisplay.track[track].row[row].vol, 2);
			track++;
		}
		row++;
		track = 0;
	}
	API_END();
}

//=============
void refresh5()
{
	uint8_t track = 0;
	uint8_t row = 0;

	API_VERTEX_FORMAT(0);
	API_COLOR(colors[5]);
	API_BITMAP_HANDLE(MT_GPU_RAM_FONT2_HANDLE);
	API_BEGIN(BITMAPS);

	for(uint16_t y = 10; y < (20*17); y+=20)
	{
		for(uint16_t x = 26+64; x < 93*8; x+=94)
		{
			StringToBitmaps(x, y, 8, 18, tableDisplay.track[track].row[row].fx, 3);
			track++;
		}
		row++;
		track = 0;
	}
	API_END();
}

//============================================================================================

// uzycie wlasnych funkcji do przesylania pojednynczych znakow zmniejsza zuzycie pamieci DL
// wbudowany widget TEXT/NUMBER dodaje zbednie dla kazdego znaku kilka komend
void StringToBitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length)
{
	y = y - font_y/2;
	uint8_t strPtr = 0;


	for(uint8_t i = 0; i < length; i++)
	{
		if(x > 755 || y > 480 || x < 25 || y < 0) // nie rysuje znakow poza wymaganymi wspolrzednymi
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
			API_VERTEX2II(x,y,MT_GPU_RAM_FONT2_HANDLE, (char)string[strPtr++]);
		}
		x+=font_x;
	}
}



static inline void draw_char(uint16_t x, uint16_t y, uint8_t charr)
{
	if(x > 511 || y > 511)
	{
		API_CELL(charr);
		API_VERTEX2F(x, y);
	}
	else
	{
		API_VERTEX2II(x,y,MT_GPU_RAM_FONT1_HANDLE,charr);
	}
}


void NumberToBitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number)
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

