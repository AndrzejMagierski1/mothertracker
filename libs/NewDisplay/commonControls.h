#ifndef LIBS_NEWDISPLAY_COMMONCONTROLS_H_
#define LIBS_NEWDISPLAY_COMMONCONTROLS_H_

const uint8_t labelBitmapsCount = 2;

struct strLabelData
{
	struct bitmap
	{
		uint16_t xValue;
		uint16_t yValue;
		uint8_t bitmapIndex = 0;
	} bitmaps[labelBitmapsCount];

	uint16_t styleValue;


};

typedef struct
{
	uint8_t isActive;
	uint8_t frameNum;

}multisel_control_t;



struct strEditData
{
	uint8_t isFrame;
	uint8_t selectionStart;
	uint8_t selectionWidth;
};

struct strFrameData
{
	uint8_t startPlace;
	uint16_t placesCount;
	uint16_t* places[12];

	uint8_t multiSelActiveNum;
	multisel_control_t multisel[8];
};


struct strBgLabelData
{
	uint8_t activDivLine = 255;

};
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cLabel: public cDisplayControl
{
public:

	cLabel(strControlProperties* properties = nullptr);
	virtual ~cLabel();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	void string2Bitmaps(int16_t x, int16_t y, const char* string, int8_t length);



	void setText2(const char* text);
	void setStyle2(uint32_t style);

	uint16_t textStyle1;
	uint16_t textStyle2;
	//int16_t textFont;
	//uint8_t fontWidth;
	//uint8_t fontHeight;

	const strFont* font1;
	const strFont* font2;


	strLabelData* data = nullptr;

	const char* text2 = nullptr;
	uint32_t style2 = 0;

};

//--------------------------------------------------------------------
class cEdit: public cDisplayControl
{
public:
	cEdit(strControlProperties* properties = nullptr);
	virtual ~cEdit();

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	uint16_t textStyle;
	const strFont* font;
private:
	strEditData* data;
};

//--------------------------------------------------------------------
class cFrame: public cDisplayControl
{
public:
	cFrame(strControlProperties* properties = nullptr);
	virtual ~cFrame();

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(const char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);


	strFrameData* data;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cBgLabel: public cDisplayControl
{
public:

	cBgLabel(strControlProperties* properties = nullptr);
	virtual ~cBgLabel();

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


	strBgLabelData* data;


};

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------










#endif
