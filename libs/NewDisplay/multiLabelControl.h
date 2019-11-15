#ifndef LIBS_NEWDISPLAY_MULTILABELCONTROL_H_
#define LIBS_NEWDISPLAY_MULTILABELCONTROL_H_


struct strMultiLabelData
{
	uint16_t xOffset = 0;
	uint16_t yOffset = 0;

	uint8_t textLinesCount = 1;

	uint8_t selected = 0;
	uint32_t style[4] = {0};
	char* text[4] = {nullptr};

};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cMultiLabel: public cDisplayControl
{
public:

	cMultiLabel(strControlProperties* properties = nullptr);
	virtual ~cMultiLabel();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	void string2Bitmaps(int16_t x, int16_t y, char* string, int8_t length);

	uint16_t textStyle;
	int16_t textFont;
	uint8_t fontWidth;
	uint8_t fontHeight;

	strMultiLabelData* data;

};








#endif
