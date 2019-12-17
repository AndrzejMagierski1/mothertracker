#include "FT812.h"
#include "displayControls.h"
#include "songPlayerControl.h"

#define BLOCK_WIDTH				56
#define BLOCK_HEIGHT			23
#define SPACING_X				4
#define SPACING_Y				SPACING_X


static uint32_t defaultColors[] =
{
	0xFFFFFF, // tekst
	0x222222, // tło
	0xFF0000, // ramka
};

void String2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, int8_t length);

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
cSongPlayer::cSongPlayer(strControlProperties* properties)
{
	colorsCount = 3;
	colors = defaultColors;
	selfRefresh = 0;

	refreshStep =  0;

	if(properties == nullptr)
	{
		posX = 0;
		posY = 0;
		text = nullptr;
		value = -1;
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

	//padNames = (strPadNames*)properties->data;

	setStyle(properties->style);
}

cSongPlayer::~cSongPlayer()
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void cSongPlayer::setStyle(uint32_t style)
{
	this->style = style;

	textStyle =    (style & controlStyleCenterX ? OPT_CENTERX : 0)
				 | (style & controlStyleCenterY ? OPT_CENTERY : 0)
	 	 	 	 | (style & controlStyleRightX  ? OPT_RIGHTX  : 0);

	int8_t textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	font = &fonts[textFont];
}

void cSongPlayer::setText(char* text)
{
	this->text = text;
}

void cSongPlayer::setValue(int value)
{
	/* ta funkcja ustawia tylko pozycje listy playera*/
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

void cSongPlayer::setColors(uint32_t* colors)
{
	this->colors = colors;
}

void cSongPlayer::setDefaultColors(uint32_t colors[])
{
	memcpy(defaultColors, colors, colorsCount*4);
}

void cSongPlayer::setData(void* data)
{
	controlData = (player_data_t*)data;

	list = controlData->list;
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
uint8_t cSongPlayer::update()
{
	//API_LIB_BeginDirectDL();
	API_LIB_BeginCoProListNoCheck();
    API_CMD_DLSTART();

    switch(refreshStep)
    {
    case 0: refresh1(); break;
    case 1:	refresh2(); break;
    case 2: refresh3(); break;
    //case 3: refresh4(); break;
    //case 4: refresh5(); break;
    default: refreshStep = 0; break;
    }

    API_LIB_EndCoProList();

	return selfRefresh;
}

uint8_t cSongPlayer::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();


	switch(refreshStep)
	{
	case 0: API_CMD_MEMCPY(address, 		 		RAM_DL, dlOffset); break;
	case 1: API_CMD_MEMCPY(address+ramPartSize[0], 	RAM_DL, dlOffset); break;
	case 2: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1], 	RAM_DL, dlOffset); break;
	//case 3: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2], 	RAM_DL, dlOffset); break;
	//case 4: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3], 	RAM_DL, dlOffset); break;
	default: break;
	}

	ramPartSize[refreshStep] = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

    refreshStep++;
    if(refreshStep > 2)
    {
    	refreshStep = 0;
    	return selfRefresh;
    }
    else
    {
    	return 1;
    }
}


uint8_t cSongPlayer::append(uint32_t address)
{
	API_CMD_APPEND(address, ramPartSize[0]);
	API_CMD_APPEND(address+ ramPartSize[0], ramPartSize[1]);
	API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1], ramPartSize[2]);
	//API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1] +ramPartSize[2], ramPartSize[3]);
	//API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1] +ramPartSize[2] +ramPartSize[3], ramPartSize[4]);

	return 0;
}

uint8_t cSongPlayer::refresh1()
{
	return showList();
}

uint8_t cSongPlayer::refresh2()
{
	drawBlocks();

	return 0;
}

uint8_t cSongPlayer::refresh3()
{
	if((controlData->songData != NULL) || (controlData->patternsBitmask != NULL))
	{
		fillBlocks();
	}

	drawProgressLine();

	return 0;
}

uint8_t cSongPlayer::showList()
{
	if(list == nullptr) return 0;

	int16_t posY = 37;
	int16_t posX = 508;
	int16_t width = (800/4-16);
	int16_t height = 25;


	int16_t  x_pos = posX, y_pos, h_row = 27; //font->height+8;
	uint16_t w_bar = width-6; // szerokosc ramki
	uint8_t lines;

	if(list->length > list->linesCount) w_bar = width-13;

	//tlo listy
	if(style & controlStyleBackground)
	{
		API_COLOR(colors[3]);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(posX, posY);
		API_VERTEX2F(posX+width, posY+height-2);
		API_END();

		API_BLEND_FUNC(SRC_ALPHA , ZERO);

		API_SAVE_CONTEXT();
		API_SCISSOR_XY(posX, posY+height-10);
		API_SCISSOR_SIZE(width+1, 10);
		API_CMD_GRADIENT(0, 413, colors[3], 0, 423, 0x0);
		API_RESTORE_CONTEXT();

		API_COLOR(0x000000);
		API_LINE_WIDTH(1);
		API_BEGIN(LINES);
		API_VERTEX2F(posX-1, posY);
		API_VERTEX2F(posX-1, posY+height);
		API_VERTEX2F(posX+width+1, posY);
		API_VERTEX2F(posX+width+1, posY+height);
		API_END();

		API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
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
			API_COLOR(colors[0]);


			API_LINE_WIDTH(8);
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(x_pos, y_pos);
			API_VERTEX2F(x_pos + w_bar, y_pos);
			API_VERTEX2F(x_pos + w_bar, y_pos + h_row);
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
		API_SCISSOR_SIZE(w_bar, h_row*list->linesCount);

		for(uint8_t i = 0; i < lines; i++)
		{

			API_COLOR(colors[0]);
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

				if(textListPos)
				{
					textListPos = textListPos-1;
				}

			}
		}

		selfRefresh = 3;

		if(!disableBar)
		{
			x_pos = posX+3;
			y_pos = posY + (barPos * h_row) + (mode ? 0 : listAnimationStep);
			if(list->length > list->linesCount) w_bar = width-13;

			//ramka

			API_COLOR(colors[0]);

			API_LINE_WIDTH(8);
			API_BEGIN(LINE_STRIP);
			API_VERTEX2F(x_pos, y_pos);
			API_VERTEX2F(x_pos + w_bar, y_pos);
			API_VERTEX2F(x_pos + w_bar, y_pos + h_row);
			API_VERTEX2F(x_pos, y_pos + h_row);
			API_VERTEX2F(x_pos, y_pos);
			API_END();
		}

		x_pos = posX+13;
		y_pos = posY + h_row/2 + (mode ? ((-1*h_row) - listAnimationStep) : 0);

		API_SAVE_CONTEXT();

		API_COLOR(colors[4]);

		API_SCISSOR_XY(posX, posY);
		API_SCISSOR_SIZE(w_bar, h_row*list->linesCount);

		lines = (list->length >= list->linesCount)  ? list->linesCount : list->length;

		if(mode == 1)
		{
			if(dir == 1) // tekst porusza sie w gore (wartosci w dol)
			{
				y_pos+=h_row;

				for(int8_t i = 0; i < lines; i++)
				{
					API_COLOR(colors[0]);

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

					API_COLOR(colors[0]);

					int16_t table_offset = textListPos - 1;
					if(table_offset  < 0) table_offset = 0;

					API_CMD_TEXT(x_pos,
							y_pos + (i * h_row),
							font->handle,
							textStyle | OPT_CENTERY,
							*(list->data + i + table_offset) );

					if(list->icon.useSpecialIcons)
					{
						if((list->icon.iconPositionInList) == i + table_offset)
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

				API_COLOR(colors[0]);

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
		int16_t y_length = (( height-14 ) * list->linesCount)  / list->length;
		y_pos = posY + 6 + (list->start * (( height-14) - y_length)) / list->length;

		//API_BLEND_FUNC(SRC_ALPHA , ZERO);

		API_COLOR(colors[6]);
		API_LINE_WIDTH(16);
		API_BEGIN(RECTS);
		API_VERTEX2F(posX + (width - 6) , posY+4);
		API_VERTEX2F(posX + (width - 3) , (posY+height)-10);
		API_END();

		API_COLOR(colors[2]);
		API_LINE_WIDTH(8);
		API_BEGIN(LINES);
		//API_LINE_WIDTH(24);
		//API_BEGIN(RECTS);
		API_VERTEX2F(posX + (width - 5) , y_pos);
		API_VERTEX2F(posX + (width - 5) , y_pos+y_length);

		API_VERTEX2F(posX + (width - 4) , y_pos);
		API_VERTEX2F(posX + (width - 4) , y_pos+y_length);

		API_END();

		//API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	}

	return selfRefresh;
}

void cSongPlayer::drawBlocks()
{
	API_COLOR(0xFFFFFF);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);

	uint8_t maxPatternsVisible = (controlData->songLength > MAX_PATTERNS_VISIBLE) ? MAX_PATTERNS_VISIBLE : controlData->songLength;

	for(size_t pattern = 0; pattern < maxPatternsVisible; pattern++)
	{
		for(size_t track = 0; track < MAX_TRACKS_PER_PATTERN; track++)
		{
			uint16_t localX, localY;
			localX = posX + track*(BLOCK_WIDTH + SPACING_X);
			localY = posY + pattern*(BLOCK_HEIGHT + SPACING_Y);

			API_VERTEX2F(localX, localY);
			API_VERTEX2F((localX + BLOCK_WIDTH), (localY + BLOCK_HEIGHT));
		}
	}

	API_END();
}

void cSongPlayer::fillBlocks()
{
	API_BLEND_FUNC(SRC_ALPHA, ZERO);
	API_COLOR(0x000000);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);

	uint8_t maxPatternsVisible = (controlData->songLength > MAX_PATTERNS_VISIBLE) ? MAX_PATTERNS_VISIBLE : controlData->songLength;

	selectionCalcInit = 0;

	for(size_t pattern = 0; pattern < maxPatternsVisible; pattern++)
	{
		uint8_t data = controlData->patternsBitmask[controlData->songData[textListPos + pattern] - 1];

		for(size_t track = 0; track < MAX_TRACKS_PER_PATTERN; track++)
		{
			uint16_t localX, localY;

			localX = posX + track*(BLOCK_WIDTH + SPACING_X);
			localY = posY + pattern*(BLOCK_HEIGHT + SPACING_Y);

			if(!(data & (1 << track)))
			{
				API_VERTEX2F((localX + 1), (localY + 1));
				API_VERTEX2F((localX + BLOCK_WIDTH - 1), (localY + BLOCK_HEIGHT - 1));
			}

			calculateSelection(pattern, track, localX, localY, localX + BLOCK_WIDTH, localY + BLOCK_HEIGHT);
		}
	}

	drawSelection();

	API_END();
	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

void cSongPlayer::drawProgressLine()
{
	if(controlData->progress.isPlaying)
	{
		API_BLEND_FUNC(SRC_ALPHA, ZERO);
		API_COLOR(0xFF0000);
		API_LINE_WIDTH(16);
		API_BEGIN(LINES);

		int16_t patternPosition = controlData->progress.currentSongPosition - textListPos;

		if((patternPosition < MAX_PATTERNS_VISIBLE) && (patternPosition >= 0))
		{
			float positionInPattern = controlData->progress.positionInPattern * (BLOCK_HEIGHT / (float)controlData->progress.patternLength) + 0.5f; // 0.5 for rounding

			uint16_t localX, localY;
			localX = posX;
			localY = posY + patternPosition*(BLOCK_HEIGHT + SPACING_Y);

			API_VERTEX2F(localX - 5, localY + positionInPattern);
			API_VERTEX2F(localX + 480, localY + positionInPattern);

			API_END();
			API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		}
	}
}

void cSongPlayer::calculateSelection(uint8_t pattern, uint8_t track, uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end)
{
	if(selectionCalcInit == 0)
	{
		xSelectionStart = UINT16_MAX;
		ySelectionStart = UINT16_MAX;
		xSelectionEnd = 0;
		ySelectionEnd = 0;

		selectionCalcInit = 1;
	}

	if(controlData->selection.isActive)
	{
		if((textListPos + pattern) >= controlData->selection.startPattern)
		{
			uint8_t calculatedPattern = textListPos + pattern;
			uint8_t isSelected = 1;

			if(!(calculatedPattern >= controlData->selection.startPattern)) isSelected = 0;
			if(!(calculatedPattern < (controlData->selection.startPattern + controlData->selection.patternSelectionLength))) isSelected = 0;
			if(!(track >= controlData->selection.startTrack)) isSelected = 0;
			if(!(track < (controlData->selection.startTrack + controlData->selection.trackSelectionLength))) isSelected = 0;

			if(isSelected)
			{
				if(x < xSelectionStart)
				{
					xSelectionStart = x;
				}

				if(y < ySelectionStart)
				{
					ySelectionStart = y;
				}

				if(x_end > xSelectionEnd)
				{
					xSelectionEnd = x_end;
				}

				if(y_end > ySelectionEnd)
				{
					ySelectionEnd = y_end;
				}
			}
		}
	}

}

void cSongPlayer::drawSelection()
{
	if(xSelectionStart != UINT16_MAX && ySelectionStart != UINT16_MAX && xSelectionEnd != 0 && ySelectionEnd != 0)
	{
		API_COLOR(0xFF0000);
		API_LINE_WIDTH(16);
		API_BEGIN(LINE_STRIP);

		API_VERTEX2F(xSelectionStart, ySelectionStart);
		API_VERTEX2F(xSelectionEnd, ySelectionStart);
		API_VERTEX2F(xSelectionEnd, ySelectionEnd);
		API_VERTEX2F(xSelectionStart, ySelectionEnd);
		API_VERTEX2F(xSelectionStart, ySelectionStart);

		API_END();
	}
}

