

#ifndef LIBS_NEWDISPLAY_SONGPLAYER_H_
#define LIBS_NEWDISPLAY_SONGPLAYER_H_

#include "displayControls.h"

#define MAX_BLOCKS_VISIBLE		32
#define MAX_PATTERNS_VISIBLE	14
#define MAX_TRACKS_PER_PATTERN	8

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
	uint8_t firstVisiblePattern;
	uint8_t *data;

	song_selection_t selection;
	progress_control_t progress;

}block_data_t;

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

private:

	void refresh1();
	void refresh2();

	void drawBlocks();
	void fillBlocks();
	void drawProgressLine();
	void calculateSelection(uint8_t pattern, uint8_t track, uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end);
	void drawSelection();

	block_data_t *controlData;

	uint16_t ramPartSize[5] = {0};
	uint8_t refreshStep;


	uint8_t selectionCalcInit = 0;
	uint16_t xSelectionStart;
	uint16_t ySelectionStart;
	uint16_t xSelectionEnd;
	uint16_t ySelectionEnd;
};








#endif
