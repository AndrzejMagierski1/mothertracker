
#include "FT812.h"

#include "displayControls.h"
#include "keyboardControl.h"



static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0x0000FF, // ramka
};

void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cKeyboard::cKeyboard(strControlProperties* properties)
{
	colorsCount = 3;
	colors = defaultColors;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		value = 0;
		width = 0;
		height = 0;
		style = 0;
		return;
	}

	posX = properties->x;
	posY = properties->y;

	text = properties->text;
	value = properties->value;

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
	fillKeyboard();
}

cKeyboard::~cKeyboard()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cKeyboard::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cKeyboard::setText(char* text)
{
	this->text = text;
}

void cKeyboard::setValue(int value)
{
	this->value = value;
}

void cKeyboard::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cKeyboard::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cKeyboard::setData(void* data)
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cKeyboard::update()
{
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();




	API_COLOR(colors[0]);

	API_BEGIN(RECTS);

	char localString[2];
	localString[1] = 0;
	for(uint8_t i = 0 ; i < KEYBOARD_SIZE; i++)
	{
		API_VERTEX2F(keyboard[i].x, keyboard[i].y);
		API_VERTEX2F(keyboard[i].x + keyboard[i].w , keyboard[i].y + keyboard[i].h);

	}
	API_END();
	API_COLOR(colors[1]);
	if(value <= 41) keyboardShift = 0;
	else
	{
		keyboardShift = 1;
		value -=42;
	}
	for(uint8_t i = 0 ; i < KEYBOARD_SIZE; i++)
	{
		if(keyboardShift)
		{
			if(charOnKeyboardBig[i] > 2)
			{
				localString[0] = charOnKeyboardBig[i];
				API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, localString);
			}
			else if(charOnKeyboardBig[i] == 0) API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, "Backspace");
			else if(charOnKeyboardBig[i] == 1) API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, "Shift");
			else if(charOnKeyboardBig[i] == 2) API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, "Space");
		}
		else
		{
			if(charOnKeyboardSmall[i] > 2)
			{
				localString[0] = charOnKeyboardSmall[i];
				API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, localString);
			}
			else if(charOnKeyboardSmall[i] == 0) API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, "Backspace");
			else if(charOnKeyboardSmall[i] == 1) API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, "Shift");
			else if(charOnKeyboardSmall[i] == 2) API_CMD_TEXT(keyboard[i].x + keyboard[i].w/2, keyboard[i].y + keyboard[i].h/2, textFont, OPT_CENTER, "Space");
		}

	}

	if(value >= 0)
	{
		API_COLOR(colors[2]);
		API_LINE_WIDTH(20);
		API_BEGIN(LINE_STRIP);

		uint16_t localX, localY;
		localX = keyboard[value].x -1;
		localY = keyboard[value].y -1;
		API_VERTEX2F(localX, localY);
		localX = keyboard[value].x + keyboard[value].w +1;
		localY = keyboard[value].y -1;
		API_VERTEX2F(localX, localY);
		localX = keyboard[value].x + keyboard[value].w + 1;
		localY = keyboard[value].y + keyboard[value].h + 1;
		API_VERTEX2F(localX, localY);
		localX = keyboard[value].x - 1;
		localY = keyboard[value].y + keyboard[value].h + 1;
		API_VERTEX2F(localX, localY);
		localX = keyboard[value].x -1;
		localY = keyboard[value].y -1;
		API_VERTEX2F(localX, localY);
		API_END();
	}




    API_LIB_EndCoProList();


	return 0;
}

uint8_t cKeyboard::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cKeyboard::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

void cKeyboard::fillKeyboard()
{

	uint16_t offset_x = width/12;
	uint16_t offset_y = height/4;


	for(uint8_t i = 0; i< KEYBOARD_SIZE; i++)
	{
		if(i == 0)
		{
			keyboard[i].x = posX;
			keyboard[i].y = posY;
			keyboard[i].w = offset_x - offset_x * 0.2;
			keyboard[i].h = offset_y* 0.8;
		}
		else if((i >= 1) && (i <=9))
		{
			keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY;
			keyboard[i].w = offset_x - offset_x * 0.2;
			keyboard[i].h = offset_y* 0.8;
		}
		else if( i == 10)
		{
			keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY;
			keyboard[i].w = 2*offset_x - offset_x*0.2;
			keyboard[i].h = offset_y* 0.8;
		}
		else if((i >= 11) && (i <= 22))
		{
			if(i == 11) keyboard[i].x = keyboard[0].x;
			else keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY + offset_y;
			keyboard[i].w = offset_x - offset_x*0.2;
			keyboard[i].h = offset_y* 0.8;
		}
		else if((i >= 23) && (i <= 32))
		{
			if(i == 23) keyboard[i].x = keyboard[0].x;
			else keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY + 2*offset_y;
			keyboard[i].w = offset_x - offset_x*0.2;
			keyboard[i].h = offset_y* 0.8;
		}
		else if(i == 33)
		{
			keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY + 2*offset_y;
			keyboard[i].w = 2*offset_x - offset_x*0.2;;
			keyboard[i].h = offset_y* 0.8;
		}
		else if((i >= 34) && (i <= 40))
		{
			if(i == 34) keyboard[i].x = keyboard[0].x;
			else keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY + 3*offset_y;
			keyboard[i].w = offset_x - offset_x*0.2;
			keyboard[i].h = offset_y* 0.8;
		}
		else if( i == 41)
		{
			keyboard[i].x = keyboard[i-1].x + offset_x;
			keyboard[i].y = posY + 3*offset_y;
			keyboard[i].w = 5*offset_x - offset_x*0.2;
			keyboard[i].h = offset_y*0.8;
		}
	}

}

//void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length)
//{
//	y = y - font_y/2;
//	uint8_t strPtr = 0;
//
//
//	for(uint8_t i = 0; i < length; i++)
//	{
//		if(x > 755 || y > 480 || x < 30 || y < 0)
//		{
//			strPtr++;
//		}
//		else if((x > 511 || y > 511) && string[strPtr] >=32)
//		{
//			API_CELL(string[strPtr++]);
//			API_VERTEX2F(x, y);
//		}
//		else if(string[strPtr] >=32)
//		{
//			API_VERTEX2II(x,y,fonts[1].handle, (char)string[strPtr++]);
//		}
//		x+=font_x;
//	}
//}
