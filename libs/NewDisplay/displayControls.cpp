
#include "FT812.h"


#include "displayControls.h"


uint8_t cDisplayControl::colorsCount = 10;
uint32_t cDisplayControl::defaultColors[] =
{
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
	0xFFFFFF,
};

inline void draw_char(uint16_t x, uint16_t y, uint8_t charr)
{
	if(x > 511 || y > 511)
	{
		//API_CELL(charr);
		//API_VERTEX2F(x, y);
	}
	else
	{
		API_VERTEX2II(x,y,MT_GPU_RAM_FONT2_HANDLE,charr);
	}
}

void Char2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, uint8_t length)
{
	y = y - font_y/2;
	uint8_t strPtr = 0;

	if(length > 2)
	{
		if(x > 500 || y > 500)
		{
			//API_CELL(string[strPtr++]);
			//API_VERTEX2F(x, y);
		}
		else
		{
			API_VERTEX2II(x,y,MT_GPU_RAM_FONT1_HANDLE, (char)string[strPtr++]);
		}
		x+=font_x;
	}
	if(length > 1)
	{
		if(x > 500 || y > 500)
		{
			//API_CELL((char)string[strPtr++]);
			//API_VERTEX2F(x, y);
		}
		else
		{
			API_VERTEX2II(x,y,MT_GPU_RAM_FONT1_HANDLE,(char)string[strPtr++]);
		}
		x+=font_x;
	}
	if(length > 0)
	{
		if(x > 500 || y > 500)
		{
			//API_CELL((char)string[strPtr++]);
			//API_VERTEX2F(x, y);
		}
		else
		{
			API_VERTEX2II(x,y,MT_GPU_RAM_FONT1_HANDLE,(char)string[strPtr++]);
		}
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
