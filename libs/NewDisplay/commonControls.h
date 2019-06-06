#ifndef LIBS_NEWDISPLAY_COMMONCONTROLS_H_
#define LIBS_NEWDISPLAY_COMMONCONTROLS_H_


struct strLabelData
{
	uint16_t xValue;
	uint16_t yValue;

	uint16_t styleValue;

};

struct strFrameData
{
	uint8_t startPlace;
	uint16_t placesCount;
	uint16_t* places[8];
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
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	uint16_t textStyle;
	int16_t textFont;

	strLabelData* data;

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
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	uint16_t textStyle;
	int16_t textFont;
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
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

	uint16_t textStyle;
	int16_t textFont;

	strFrameData* data;
};


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------










#endif
