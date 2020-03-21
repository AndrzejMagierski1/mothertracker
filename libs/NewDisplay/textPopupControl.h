#ifndef LIBS_NEWDISPLAY_TEXTPOPUPCONTROL_H_
#define LIBS_NEWDISPLAY_TEXTPOPUPCONTROL_H_


struct strTextPopupData
{
	uint8_t textLinesCount = 1;

	uint32_t** multiLineColors = nullptr;
	uint32_t** multiLineStyle = nullptr;
	char** multiLineText = nullptr;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cTextPopup: public cDisplayControl
{
public:

	cTextPopup(strControlProperties* properties = nullptr);
	virtual ~cTextPopup();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	void string2Bitmaps(int16_t x, int16_t y, char* string, int8_t length);

	uint16_t textStyle;
	//int16_t textFont;
	//uint8_t fontWidth;
	//uint8_t fontHeight;
	const strFont* font;

	strTextPopupData* data;

};

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------




#endif
