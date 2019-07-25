
#include "FT812.h"

#include "displayControls.h"
#include "list.h"


static uint32_t defaultColors[] =
{
	0xFFFFFF,	//	uint32_t listItemFrame 			= DISP_RGB(255,255,255);
	0x000000,	//	uint32_t listItemFrameBG 		= DISP_RGB(0,0,0);
	0xFFFFFF,	//	uint32_t listScrollBar 			= DISP_RGB(255,255,255);
	0x554A19,	//	uint32_t listBG 				= DISP_RGB(85,74,25);
	0xFFFFFF,	//	uint32_t fontList 				= DISP_RGB(255,255,255);
};





//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cList::cList(strControlProperties* properties)
{
	colorsCount = 3;
	colors = defaultColors;
	selfRefresh = 0;

	listPosition = 0;

	listAnimationStep = 0;


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

	//text = properties->text;
	value = properties->value;

	width = properties->w;
	height = properties->h;


	setStyle(properties->style);

	setData(properties->data);
}

cList::~cList()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cList::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				// | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cList::setText(char* text)
{
	this->text = text;
}

void cList::setValue(int value)
{
	if(value >= list->length)
	{
		if(list->length > 0) listPosition = list->length-1;
		else listPosition = 0;

	}
	else if(value < 0) listPosition = 0;
	else  listPosition = value;
}

void cList::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cList::setDefaultColors(uint32_t* colors)
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cList::setData(void* data)
{
	if(data == nullptr) return;
	list = (strList*)data;

	listPosition = list->start;

	if(listPosition == 0) 		//pierwsza pozycja listy*
	{
		barPos = 0;
		textListPos = 0;
	}
	else if(listPosition == 1) 	//druga pozycja listy
	{
		barPos = 1;
		textListPos = 0;
	}
	else if(listPosition >= list->length-1) // ostatnia pozycja listy
	{
		barPos  = (list->length > list->linesCount ? list->linesCount-1 : list->length-1);
		textListPos = (list->length > list->linesCount ? list->length-list->linesCount-1 : 0);
	}
	else if(listPosition >= list->length-2) // przed ostatnia pozycja listy
	{
		barPos  = (list->length > list->linesCount ? list->linesCount-2 : list->length-2);
		textListPos = (list->length > list->linesCount ? list->length-list->linesCount-2 : 0);
	}
	else
	{
		barPos  = (list->length > list->linesCount ? (listPosition>list->linesCount-2 ? list->linesCount-2 : listPosition) : listPosition);
		textListPos = (list->length > list->linesCount ? (listPosition>list->linesCount-2 ? listPosition-(list->linesCount-2) : 0) : 0);
	}

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cList::update()
{
	if(list == nullptr) return 0;


	int16_t  x_pos = posX, y_pos;
	uint8_t lines;

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();


	//tlo listy
	API_COLOR(colors[3]);
	API_BLEND_FUNC(DST_ALPHA , ZERO);
	API_LINE_WIDTH(8);
	API_BEGIN(RECTS);
	API_VERTEX2F(posX, posY);
	API_VERTEX2F(posX+width, posY+(height*list->linesCount));
	API_END();
	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


	if(list->start == listPosition)
	{
		// geneorwanie poczatkowego menu
		listAnimationStep = 0;
		selfRefresh = 0;

		x_pos = posX;
		y_pos = posY + (barPos * height);

		//ramka
		API_COLOR(colors[0]);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(x_pos, y_pos);
		API_VERTEX2F(x_pos + width, y_pos);
		API_VERTEX2F(x_pos + width, y_pos + height);
		API_VERTEX2F(x_pos, y_pos + height);
		API_VERTEX2F(x_pos, y_pos);
		API_END();

		//lista tekstow
		x_pos = posX+5;
		y_pos = posY+height/2;

		lines = (list->length >= list->linesCount)  ? list->linesCount : list->length;

		//uint8_t txt_offset;
		API_COLOR(colors[4]);
		for(uint8_t i = 0; i < lines; i++)
		{
			API_CMD_TEXT(x_pos,
					y_pos + (i * height),
					textFont,
					textStyle | OPT_CENTERY,
					*(list->data + (i +  textListPos)));
		}
	}
	else
	{
		int8_t dir, mode;    // kieurnek przewijania ; tryb/rodzaj przewijania
		uint16_t diffrence; // skok belki/listy w pixelach przy jedym odsiwezeniu listy

		// Geneorwanie zmieny menu.

		// okreslenie kierunku i skoku listy
		if(list->start > listPosition) {dir = -1;	diffrence = list->start - listPosition; }
		else {dir = 1;	diffrence = listPosition - list->start; }

		if(diffrence > 5) diffrence = 8;
		else if(diffrence > 1) diffrence = 4;
		else diffrence = 2;

		mode = 0;
		if(barPos == 0 || barPos == list->linesCount-1) 	mode = 0;
		else if(barPos == 1) 					mode = ((dir ==  1 || list->start == 1) ? 0 : 1);
		else if(barPos == list->linesCount-2) 	mode = ((dir == -1 || list->start == list->length-2) ? 0 : 1);
		else mode = 0;


		if(mode == 0) // ruch belka(mode = 0)
		{
			listAnimationStep = listAnimationStep + (dir * diffrence);
			if(listAnimationStep >= height)
			{
				listAnimationStep = 0;
				barPos = barPos +1;
				list->start = list->start +1;
			}
			else if(listAnimationStep <= -1*height)
			{
				listAnimationStep = 0;
				barPos = barPos -1;
				list->start = list->start -1;
			}
		}
		else // ruch tekstem(mode = 1)
		{
			listAnimationStep = listAnimationStep + (dir * diffrence);
			if(listAnimationStep >= height)
			{
				listAnimationStep = 0;
				list->start = list->start +1;
				textListPos = textListPos+1;
			}
			else if(listAnimationStep <= -1*height)
			{
				listAnimationStep = 0;
				list->start = list->start -1;
				textListPos = textListPos-1;
			}
		}

		selfRefresh = 2;

		x_pos = posX;
		y_pos = posY + (barPos * height) + (mode ? 0 : listAnimationStep);

		//ramka
		API_COLOR(colors[0]);
		API_LINE_WIDTH(4);
		API_BEGIN(LINE_STRIP);
		API_VERTEX2F(x_pos, y_pos);
		API_VERTEX2F(x_pos + width, y_pos);
		API_VERTEX2F(x_pos + width, y_pos + height);
		API_VERTEX2F(x_pos, y_pos + height);
		API_VERTEX2F(x_pos, y_pos);
		API_END();


		x_pos = posX+5;
		y_pos = posY + height/2 + (mode ? ((-1*height) - listAnimationStep) : 0);

		API_SAVE_CONTEXT();

		API_COLOR(colors[4]);

		API_SCISSOR_XY(posX, posY);
		API_SCISSOR_SIZE(width, height*list->linesCount);

		lines = (list->length >= list->linesCount)  ? list->linesCount : list->length;

		if(mode == 1)
		{
			if(dir == 1) // tekst porusza sie w gore (wartosci w dol)
			{
				y_pos+=height;

				for(int8_t i = 0; i < (lines+1); i++)
				{
					API_CMD_TEXT(x_pos,
							y_pos + (i * height),
							textFont,
							textStyle | OPT_CENTERY,
							*(list->data +  i + textListPos ) );
				}
			}
			else if(dir == -1) // tekst porusza sie w dol (wartosci w gore)
			{
				for(int8_t i = 0; i < (lines+1); i++)
				{
					API_CMD_TEXT(x_pos,
							y_pos + (i * height),
							textFont,
							textStyle | OPT_CENTERY,
							*(list->data + i + textListPos -1) );
				}
			}
		}
		else
		{
			for(uint8_t i = 0; i < lines; i++) // belka porusa sie - teks bez ruchu
			{
				API_CMD_TEXT(x_pos,
						y_pos + (i * height),
						textFont,
						textStyle | OPT_CENTERY,
						*(list->data + i  + textListPos ) );
			}
		}

		API_RESTORE_CONTEXT();
	}

	// pasek przewijania
	if(list->length > list->linesCount)
	{
		int16_t y_length = (((  height * list->linesCount  ) * list->linesCount)  / (list->length-1)) - 4 ;
		y_pos = posY + 2 + (list->start * (( height * list->linesCount )-(y_length+2))) / (list->length-1) ;


		API_COLOR(colors[2]);
		API_LINE_WIDTH(24);
		API_BEGIN(RECTS);
		API_VERTEX2F(posX + (width - 3) , y_pos);
		API_VERTEX2F(posX + (width - 3) , y_pos+y_length);
		API_END();
	}


    API_LIB_EndCoProList();


	return 0;
}

uint8_t cList::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();

	API_CMD_MEMCPY(address, RAM_DL, dlOffset);

	ramSize = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	return selfRefresh;
}


uint8_t cList::append(uint32_t address)
{
	API_CMD_APPEND(address, ramSize);

	return 0;
}
