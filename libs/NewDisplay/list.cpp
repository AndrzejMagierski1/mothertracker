
#include "FT812.h"

#include "displayControls.h"
#include "list.h"


static uint32_t defaultColors[] =
{
	0xFFFFFF,	//	uint32_t listItemFrame 			= DISP_RGB(255,255,255);
	0x000000,	//	uint32_t listItemFrameBG 		= DISP_RGB(0,0,0);
	0x575757,	//	uint32_t listScrollBar 			= DISP_RGB(255,255,255);
	0x0a0a0a,	//	uint32_t listBG 				= DISP_RGB(85,74,25);
	0xFFFFFF,	//	uint32_t fontList 				= DISP_RGB(255,255,255);
	0xFF0000,   //  select color
	0x080808	//	scrollParCont
};





//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cList::cList(strControlProperties* properties)
{
	colorsCount = 7;
	colors = (properties->colors == nullptr) ? defaultColors : properties->colors;
	selfRefresh = 0;

	listPosition = 0;

	listAnimationStep = 0;
	disableBar = 0;

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


	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font = &fonts[textFont];

	//textFont = FONT_INDEX_FROM_STYLE;
	//textFont = (textFont>=0) ? textFont : 0;
	//textFont =  fonts[textFont].handle;
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

		disableBar = 0;
	}
	else if(value < 0)
	{
		listPosition = 0;
		disableBar = 1;
	}
	else
	{
		listPosition = value;
		disableBar = 0;
	}
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

	if(list->start < 0)
	{
		list->start = 0;
		listPosition = 0;
		disableBar = 1;
	}
	else
	{
		listPosition = list->start;
		disableBar = 0;
	}

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
		textListPos = (list->length > list->linesCount ? list->length-(list->linesCount) : 0);
	}
	else if(listPosition >= list->length-2) // przed ostatnia pozycja listy
	{
		barPos  = (list->length > list->linesCount ? list->linesCount-2 : list->length-2);
		textListPos = (list->length > list->linesCount ? list->length-(list->linesCount) : 0);
	}
	else
	{
		barPos  = (list->length > list->linesCount ? (listPosition > list->linesCount-2 ? list->linesCount-2 : listPosition) : listPosition);
		textListPos = (list->length > list->linesCount ? (listPosition > list->linesCount-2 ? listPosition-(list->linesCount-2) : 0) : 0);
	}

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t cList::update()
{
	if(list == nullptr) return 0;

	//int16_t posX = this->posX;
	int16_t posY = this->posY;


	if(style & controlStyleCenterY)
	{
		posY = this->posY-(height/2);
	}



	int16_t  x_pos = posX, y_pos, h_row = 27; //font->height+8;
	uint8_t lines;

	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();


	//tlo listy
    if(style & controlStyleBackground)
    {
		API_BLEND_FUNC(SRC_ALPHA , ZERO);
		API_COLOR(colors[3]);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(posX, posY);
		API_VERTEX2F(posX+width, posY+height);
		API_END();
		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


		API_SAVE_CONTEXT();

		//API_LINE_WIDTH(16);
		API_SCISSOR_XY(posX, posY+height-10);
		API_SCISSOR_SIZE(width+1, 10);
		API_CMD_GRADIENT(0, 413, colors[3], 0, 423, 0x0);

		API_RESTORE_CONTEXT();
    }

    posY +=1;

	if(list->start == listPosition)
	{
		// geneorwanie poczatkowego menu
		listAnimationStep = 0;
		selfRefresh = 0;

		if(!disableBar)
		{
			x_pos = posX+3;
			y_pos = posY + (barPos * h_row);

			//ramka
			if(list->selectionActive)
			{
				API_COLOR(colors[5]);
			}
			else
			{
				API_COLOR(colors[0]);
			}

			API_LINE_WIDTH(8);
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(x_pos, y_pos);
			API_VERTEX2F(x_pos + width-15, y_pos);
			API_VERTEX2F(x_pos + width-15, y_pos + h_row);
			API_VERTEX2F(x_pos, y_pos + h_row);
			API_VERTEX2F(x_pos, y_pos);
			API_END();
		}


		//lista tekstow
		x_pos = posX+13;
		y_pos = posY+h_row/2;

		lines = (list->length >= list->linesCount)  ? list->linesCount : list->length;

		API_SAVE_CONTEXT();

		//uint8_t txt_offset;
		API_COLOR(colors[4]);

		API_SCISSOR_XY(posX, posY);
		API_SCISSOR_SIZE(width-10, h_row*list->linesCount);

		for(uint8_t i = 0; i < lines; i++)
		{
			if(list->selectTab != NULL)
			{
				if(list->selectTab[i+textListPos])
				{
					API_COLOR(colors[5]);
				}
				else
				{
					API_COLOR(colors[0]);
				}
			}

			API_CMD_TEXT(x_pos,
					y_pos + (i * h_row),
					font->handle,
					textStyle | OPT_CENTERY,
					*(list->data + (i +  textListPos)));

			if(list->icon.useSpecialIcons)
			{
				if((list->icon.iconPositionInList) == i+textListPos)
				{
					uint8_t idx = list->icon.iconNum;

					API_BITMAP_SOURCE(bitmaps[idx].address);
					API_BITMAP_LAYOUT(bitmaps[idx].format, bitmaps[idx].linestride, bitmaps[idx].height);
					API_BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmaps[idx].width, bitmaps[idx].height);

					API_BEGIN(BITMAPS);
					API_VERTEX2F(x_pos, y_pos + ((i*h_row) - bitmaps[idx].height/2) + 1);
					API_END();
				}
			}
		}

		API_RESTORE_CONTEXT();
	}
	else
	{
		int8_t dir, mode;    // kieurnek przewijania ; tryb/rodzaj przewijania
		uint16_t diffrence; // skok belki/listy w pixelach przy jedym odsiwezeniu listy

		// Geneorwanie zmieny menu.

		// okreslenie kierunku i skoku listy
		if(list->start > listPosition) {dir = -1;	diffrence = list->start - listPosition; }
		else {dir = 1;	diffrence = listPosition - list->start; }

		if(diffrence > 8) diffrence = 75;
		else if(diffrence > 4) diffrence = 50;
		else if(diffrence > 1) diffrence = 25;
		else diffrence = 8;//2;

		mode = 0;
		if(barPos == 0 || barPos == list->linesCount-1) 	mode = 0;
		else if(barPos == 1) 					mode = ((dir ==  1 || list->start == 1) ? 0 : 1);
		else if(barPos == list->linesCount-2) 	mode = ((dir == -1 || list->start == list->length-2) ? 0 : 1);
		else mode = 0;


		if(mode == 0) // ruch belka(mode = 0)
		{
			listAnimationStep = listAnimationStep + (dir * diffrence);
			if(listAnimationStep >= h_row)
			{
				listAnimationStep = 0;
				barPos = barPos +1;
				list->start = list->start +1;
			}
			else if(listAnimationStep <= -1*h_row)
			{
				listAnimationStep = 0;
				barPos = barPos -1;
				list->start = list->start -1;
			}
		}
		else // ruch tekstem(mode = 1)
		{
			listAnimationStep = listAnimationStep + (dir * diffrence);
			if(listAnimationStep >= h_row)
			{
				listAnimationStep = 0;
				list->start = list->start +1;
				textListPos = textListPos+1;
			}
			else if(listAnimationStep <= -1*h_row)
			{
				listAnimationStep = 0;
				list->start = list->start -1;
				textListPos = textListPos-1;
			}
		}

		selfRefresh = 3;

		if(!disableBar)
		{
			x_pos = posX+3;
			y_pos = posY + (barPos * h_row) + (mode ? 0 : listAnimationStep);

			//ramka
			if(list->selectionActive)
			{
				API_COLOR(colors[5]);
			}
			else
			{
				API_COLOR(colors[0]);
			}
			API_LINE_WIDTH(8);
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(x_pos, y_pos);
			API_VERTEX2F(x_pos + width-15, y_pos);
			API_VERTEX2F(x_pos + width-15, y_pos + h_row);
			API_VERTEX2F(x_pos, y_pos + h_row);
			API_VERTEX2F(x_pos, y_pos);
			API_END();
		}

		x_pos = posX+13;
		y_pos = posY + h_row/2 + (mode ? ((-1*h_row) - listAnimationStep) : 0);

		API_SAVE_CONTEXT();

		API_COLOR(colors[4]);

		API_SCISSOR_XY(posX, posY);
		API_SCISSOR_SIZE(width-10, h_row*list->linesCount);

		lines = (list->length >= list->linesCount)  ? list->linesCount : list->length;

		if(mode == 1)
		{
			if(dir == 1) // tekst porusza sie w gore (wartosci w dol)
			{
				y_pos+=h_row;

				for(int8_t i = 0; i < lines; i++)
				{
					if(list->selectTab != NULL)
					{
						if(list->selectTab[i+textListPos])
						{
							API_COLOR(colors[5]);
						}
						else
						{
							API_COLOR(colors[0]);
						}
					}

					API_CMD_TEXT(x_pos,
							y_pos + (i * h_row),
							font->handle,
							textStyle | OPT_CENTERY,
							*(list->data +  i + textListPos ) );

					if(list->icon.useSpecialIcons)
					{
						if((list->icon.iconPositionInList) == i+textListPos)
						{
							uint8_t idx = list->icon.iconNum;

							API_BITMAP_SOURCE(bitmaps[idx].address);
							API_BITMAP_LAYOUT(bitmaps[idx].format, bitmaps[idx].linestride, bitmaps[idx].height);
							API_BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmaps[idx].width, bitmaps[idx].height);

							API_BEGIN(BITMAPS);
							API_VERTEX2F(x_pos, y_pos + ((i*h_row) - bitmaps[idx].height/2 + 1));
							API_END();
						}
					}
				}
			}
			else if(dir == -1) // tekst porusza sie w dol (wartosci w gore)
			{
				for(int8_t i = 0; i < (lines+1); i++)
				{
					if(list->selectTab != NULL)
					{
						if(list->selectTab[i+textListPos-1])
						{
							API_COLOR(colors[5]);
						}
						else
						{
							API_COLOR(colors[0]);
						}
					}

					int8_t table_offset = textListPos - 1;
					if(table_offset  < 0) table_offset = 0;

					API_CMD_TEXT(x_pos,
							y_pos + (i * h_row),
							font->handle,
							textStyle | OPT_CENTERY,
							*(list->data + i + table_offset) );

					if(list->icon.useSpecialIcons)
					{
						if((list->icon.iconPositionInList) == i+textListPos)
						{
							uint8_t idx = list->icon.iconNum;

							API_BITMAP_SOURCE(bitmaps[idx].address);
							API_BITMAP_LAYOUT(bitmaps[idx].format, bitmaps[idx].linestride, bitmaps[idx].height);
							API_BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmaps[idx].width, bitmaps[idx].height);

							API_BEGIN(BITMAPS);
							API_VERTEX2F(x_pos, y_pos + ((i*h_row) - bitmaps[idx].height/2 + 1));
							API_END();
						}
					}
				}
			}
		}
		else
		{
			for(uint8_t i = 0; i < lines; i++)
			{
				if(list->selectTab != NULL)
				{
					if(list->selectTab[i+textListPos])
					{
						API_COLOR(colors[5]);
					}
					else
					{
						API_COLOR(colors[0]);
					}
				}

				API_CMD_TEXT(x_pos,
						y_pos + (i * h_row),
						font->handle,
						textStyle | OPT_CENTERY,
						*(list->data + i  + textListPos ) );

				if(list->icon.useSpecialIcons)
				{
					if((list->icon.iconPositionInList) == i+textListPos)
					{
						uint8_t idx = list->icon.iconNum;

						API_BITMAP_SOURCE(bitmaps[idx].address);
						API_BITMAP_LAYOUT(bitmaps[idx].format, bitmaps[idx].linestride, bitmaps[idx].height);
						API_BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmaps[idx].width, bitmaps[idx].height);

						API_BEGIN(BITMAPS);
						API_VERTEX2F(x_pos, y_pos + ((i*h_row) - bitmaps[idx].height/2 + 1));
						API_END();
					}
				}
			}
		}

		API_RESTORE_CONTEXT();
	}

	// pasek przewijania
	if(list->length > list->linesCount)
	{
		int16_t y_length = (((  h_row * list->linesCount  ) * list->linesCount)  / (list->length-1)) - 4 ;
		y_pos = posY + 6 + (list->start * (( h_row * list->linesCount )-(y_length+2))) / (list->length-1) ;

		//API_BLEND_FUNC(SRC_ALPHA , ZERO);

		API_COLOR(colors[6]);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(posX + (width - 8) , posY+4);
		API_VERTEX2F(posX + (width - 5) , (posY+height)-10);
		API_END();

		API_COLOR(colors[2]);
		API_LINE_WIDTH(8);
		API_BEGIN(LINES);
		//API_LINE_WIDTH(24);
		//API_BEGIN(RECTS);
		API_VERTEX2F(posX + (width - 7) , y_pos);
		API_VERTEX2F(posX + (width - 7) , y_pos+y_length);

		API_VERTEX2F(posX + (width - 6) , y_pos);
		API_VERTEX2F(posX + (width - 6) , y_pos+y_length);

		API_END();

		//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	}


    API_LIB_EndCoProList();


	return selfRefresh;
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
