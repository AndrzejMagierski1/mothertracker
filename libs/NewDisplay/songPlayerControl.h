

#ifndef LIBS_NEWDISPLAY_SONGPLAYER_H_
#define LIBS_NEWDISPLAY_SONGPLAYER_H_

#include "displayControls.h"

#define MAX_BLOCKS_VISIBLE		32
#define MAX_PATTERNS_VISIBLE	14
#define MAX_TRACKS_PER_PATTERN	8

typedef struct
{
	uint8_t useSpecialIcons = 0;
	int8_t 	iconPositionInList;
	uint8_t iconNum;

}text_icon_t;

typedef struct
{
	int16_t  start;
	uint16_t length;
	uint16_t linesCount;

	char**   data;

	text_icon_t   icon;

}localList_t;

typedef struct
{
	uint8_t isActive;
	uint8_t startPattern;
	uint8_t patternSelectionLength;
	uint8_t startTrack;
	uint8_t trackSelectionLength;

}song_selection_t;

typedef struct
{
	uint8_t isPlaying;
	uint8_t currentSongPosition;
	uint8_t patternLength;
	uint8_t positionInPattern;

}progress_control_t;

typedef struct
{
	uint8_t songLength;

	uint8_t *patternsBitmask;
	uint8_t *songData;

	song_selection_t selection;
	progress_control_t progress;
	localList_t *list;

}player_data_t;



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cSongPlayer: public cDisplayControl
{
public:

	cSongPlayer(strControlProperties* properties = nullptr);
	virtual ~cSongPlayer();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	uint16_t textStyle;
	int16_t textFont;
	uint8_t keyboardShift = 0;

	uint16_t textListPos = 0;

private:

	uint8_t refresh1();
	uint8_t refresh2();
	uint8_t refresh3();

	uint8_t showList();
	void drawBlocks();
	void fillBlocks();
	void drawProgressLine();
	void calculateSelection(uint8_t pattern, uint8_t track, uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end);
	void drawSelection();

	player_data_t *controlData;

	uint16_t ramPartSize[5] = {0};
	uint8_t refreshStep;


	uint8_t selectionCalcInit = 0;
	uint16_t xSelectionStart;
	uint16_t ySelectionStart;
	uint16_t xSelectionEnd;
	uint16_t ySelectionEnd;



	//lokalna lista playera
	const strFont* font;
	localList_t* list;
	int16_t listPosition;

	uint16_t barPos;


	uint8_t disableBar;

	int8_t   listAnimationStep = 0;

	uint8_t selfRefresh;
};








#endif
