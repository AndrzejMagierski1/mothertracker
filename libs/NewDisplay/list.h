

#ifndef LIBS_NEWDISPLAY_LIST_H_
#define LIBS_NEWDISPLAY_LIST_H_

#include "displayControls.h"


#define LIST_LENGTH_MAX 50
#define BITMAP_ICON_OFFSET 5

typedef struct
{
	uint8_t useSpecialIcons = 0;
	int8_t iconPositionInList;
	uint8_t iconNum;

}special_icon_t;


struct strList
{
	int16_t start;
	uint16_t length;
	uint16_t linesCount;

	uint8_t *selectTab;
	uint8_t selectionActive;

	special_icon_t icon;


	uint8_t lineLengthMax = 0;


	char** data;


};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cList: public cDisplayControl
{
public:

	cList(strControlProperties* properties = nullptr);
	virtual ~cList();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t* colors);
	virtual void setData(void* data);

	uint16_t textStyle;
	//int16_t textFont;
	const strFont* font;

	strList* list;

	int16_t listPosition;

	uint16_t barPos;
	uint16_t textListPos;

	uint8_t disableBar;

	int8_t   listAnimationStep = 0;

	uint8_t selfRefresh;


};








#endif
