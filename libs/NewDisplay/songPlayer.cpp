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
	//calculateGrid();
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

	textFont = FONT_INDEX_FROM_STYLE;
	textFont = (textFont>=0) ? textFont : 0;
	textFont =  fonts[textFont].handle;
}

void cSongPlayer::setText(char* text)
{
	this->text = text;
}

void cSongPlayer::setValue(int value)
{
	this->value = value;
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
	controlData = (block_data_t*)data;
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
    case 1: refresh2(); break;
    //case 2: refresh3(); break;
    //case 3: refresh4(); break;
    //case 4: refresh5(); break;
    default: refreshStep = 0; break;
    }

    API_LIB_EndCoProList();

	return 0;
}

uint8_t cSongPlayer::memCpy(uint32_t address)
{
	uint32_t dlOffset = EVE_MemRead32(REG_CMD_DL);

	API_LIB_BeginCoProListNoCheck();


	switch(refreshStep)
	{
	case 0: API_CMD_MEMCPY(address, 		 		RAM_DL, dlOffset); break;
	case 1: API_CMD_MEMCPY(address+ramPartSize[0], 	RAM_DL, dlOffset); break;
	//case 2: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1], 	RAM_DL, dlOffset); break;
	//case 3: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2], 	RAM_DL, dlOffset); break;
	//case 4: API_CMD_MEMCPY(address+ramPartSize[0]+ramPartSize[1]+ramPartSize[2]+ramPartSize[3], 	RAM_DL, dlOffset); break;
	default: break;
	}

	ramPartSize[refreshStep] = dlOffset;

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

    refreshStep++;
    if(refreshStep > 1)
    {
    	refreshStep = 0;
    	return 0;
    }

	return 1;
}


uint8_t cSongPlayer::append(uint32_t address)
{
	API_CMD_APPEND(address, ramPartSize[0]);
	API_CMD_APPEND(address+ ramPartSize[0], ramPartSize[1]);
	//API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1], ramPartSize[2]);
	//API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1] +ramPartSize[2], ramPartSize[3]);
	//API_CMD_APPEND(address+ ramPartSize[0] +ramPartSize[1] +ramPartSize[2] +ramPartSize[3], ramPartSize[4]);

	return 0;
}

void cSongPlayer::refresh1()
{
	drawBlocks();

}

void cSongPlayer::refresh2()
{
	if(controlData->data != NULL)
	{
		fillBlocks();
		drawProgressLine();

		//drawSelection();
	}
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
		uint8_t data = controlData->data[pattern];

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

		int16_t patternPosition = controlData->progress.currentSongPosition - controlData->firstVisiblePattern;

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
		if((controlData->firstVisiblePattern + pattern) >= controlData->selection.startPattern)
		{
			uint8_t calculatedPattern = controlData->firstVisiblePattern + pattern;
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

