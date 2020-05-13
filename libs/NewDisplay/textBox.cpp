
#include "FT812.h"


#include "textBoxControl.h"

#include <string.h>

void labelString2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);


static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x0a0a0a, // tło
	one_true_red, // zaznaczenie
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cTextBox::cTextBox(strControlProperties* properties): cDisplayControl(controlIdTextBox)
{
	colorsCount = 3;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		data = nullptr;
		width = 0;
		height = 0;
		style = 0;
		value = 0;
		return;
	}

	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;

	posX = properties->x;
	posY = properties->y;

	text = properties->text;

	value = properties->value;

	data = (strTextBoxData*)(properties->data);

	width = properties->w;
	height = properties->h;

	setStyle(properties->style);
}

cTextBox::~cTextBox()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cTextBox::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =   (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font = &fonts[textFont];
}

void cTextBox::setText(const char* text)
{
	this->text = text;
}

void cTextBox::setValue(int value)
{
	this->value = value;
}

void cTextBox::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cTextBox::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cTextBox::setData(void* data)
{
	this->data = (strTextBoxData*)(data);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cTextBox::update()
{

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    int16_t border_x = posX;//, border_y = posY-2;
    int16_t border_y = posY;
    int16_t lines_step =  font->height+value;
    uint8_t lines_max = height/lines_step;
    int16_t lines_space = font->height;

    int16_t text_y; //= posY + data->yOffset;
    int16_t text_h; //= height - data->yOffset;

//    if(data->textLinesCount > 0 && data->textLinesCount < 5 && height > fontHeight)
//    {
//    	lines_space = (text_h - fontHeight*data->textLinesCount) / (data->textLinesCount+1);
//    }

	if(style & controlStyleCenterX)
	{
		border_x = posX - (width/2);
	}
	if(style & controlStyleCenterY)
	{
		border_y = posY - (height/2);
		text_y = text_y + lines_space + font->height/2;
	}

	if(style & controlStyleBackground)
	{
		if(style & controlStyleNoTransparency)
		{
			API_BLEND_FUNC(SRC_ALPHA, ZERO);
		}

		API_COLOR(colors[1]);

		if(style & controlStyleRoundedBorder) API_LINE_WIDTH(32);
		else API_LINE_WIDTH(16);

		API_BEGIN(RECTS);
		API_VERTEX2F(border_x , border_y);
		API_VERTEX2F(border_x+font->width , border_y+font->height);
		API_END();


		if(style & controlStyleBottomShadow)
		{
			API_BLEND_FUNC(SRC_ALPHA , ZERO);

			API_SAVE_CONTEXT();
			uint16_t grad_y = posY+height-10;
			uint16_t grad_h = 10;
			API_SCISSOR_XY(border_x-1, grad_y);
			API_SCISSOR_SIZE(width+2, grad_h);
			API_CMD_GRADIENT(0, grad_y, colors[1], 0, grad_y+grad_h, 0x0);
			API_RESTORE_CONTEXT();

			API_COLOR(0x000000);
			API_LINE_WIDTH(1);
			API_BEGIN(LINES);
			API_VERTEX2F(border_x-1, posY);
			API_VERTEX2F(border_x-1, posY+height);
			API_VERTEX2F(border_x+width+1, posY);
			API_VERTEX2F(border_x+width+1, posY+height);
			API_END();

			API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

		}

		if(style & controlStyleNoTransparency)
		{
			API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		}
	}
	if(style & controlStyleBorder)
	{
		API_COLOR(colors[2]);

		API_LINE_WIDTH(24);

		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(border_x , border_y);
		API_VERTEX2F(border_x+width , border_y);
		API_VERTEX2F(border_x+width , border_y+height);
		API_VERTEX2F(border_x , border_y+height);
		API_VERTEX2F(border_x , border_y);
		API_END();
	}

	textLength = strlen(text);
	position = text;


	API_COLOR(colors[0]);


	if(text != nullptr || text[0] != 0)
	{

		for(uint8_t line = 0; line < lines_max; line++)
		{
			int16_t length  = getNextLineLength();
			if(length > 0) API_CMD_TEXT(posX+5, posY+5+(line*lines_step), font->handle, textStyle, position, length);
			else if(length  < 0) break;
			// 0 = pusta linia

			position += length+1;
		}


	}


    API_LIB_EndCoProList();


	return 0;
}

uint8_t cTextBox::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return 0;
}


uint8_t cTextBox::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}

void cTextBox::string2Bitmaps(int16_t x, int16_t y, strFont* font, char* string, int8_t length)
{
	//y = y - fontHeight/2;
	uint8_t strPtr = 0;

	API_BITMAP_HANDLE(font->handle);
	API_BEGIN(BITMAPS);

	for(uint8_t i = 0; i < length; i++)
	{
		if(x > 799 || y > 479 || x < 0 || y < 0)
		{
			strPtr++;
		}
		else if((x > 511 || y > 511) && string[strPtr] >=32)
		{
			API_CELL(string[strPtr++]);
			//API_CMD_LOADIDENTITY();
			//API_CMD_ROTATE(90*65536 / 360);
			//API_CMD_SETMATRIX();
			API_VERTEX2F(x, y);

		}
		else if(string[strPtr] >=32)
		{
			API_VERTEX2II(x,y, font->handle, (char)string[strPtr++]);
		}
		x+=font->width;
	}

	API_END();
}


int16_t cTextBox::getNextLineLength()
{
	//width; // niby powinno patrzec na szerokosc miejsca i jesli dlugi wyraz nie wyjsc poza i ciąc wyraz wtedy ale...

	const char* temp_pos = position;
	int16_t last_length = -1;
	//int8_t

	for(uint8_t i = 0; i<30; i++) // 30 wystapien na linie max
	{
		//gdzie jest enter
		char* enter_pos = strchr(temp_pos,'\n');
		if(enter_pos == position) return 0;
		//if(enter_pos == nullptr) return strlen(text+position);

		//gdzie jest spacja
		char* space_pos = strchr(temp_pos,' ');
		if(space_pos == nullptr && enter_pos == nullptr) return strlen(position); // jesli brak spacji i entera to podaj dlugosc reszy tekstu

		//jesli pierwszy enter to zwroc do entera
		if(enter_pos != nullptr && enter_pos < space_pos)
		{
			return enter_pos-position; // moze powodowac blad, bo nie patrzty czy enter nie wychodzi poza szerokosc
		}

		//  sprawdza czy do spacji miesci sie w szerokosci
		//if(space_pos != nullptr)
		if(getTextWidth((strFont*)font, position, space_pos-position) > width-10) //jesli nie to konczy
		{
			if(i > 0) return last_length;   // kolejny przebieg
			else return space_pos-position; // pierweszy przebieg
		}

		//jesli tak to leci daje nastepne spacje/entery

		last_length = space_pos-position;
		temp_pos = space_pos+1;
	}








	return -1;
}
