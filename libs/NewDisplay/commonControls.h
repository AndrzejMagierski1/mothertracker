#ifndef LIBS_NEWDISPLAY_COMMONCONTROLS_H_
#define LIBS_NEWDISPLAY_COMMONCONTROLS_H_




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

	virtual void setStyle(uint16_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setData(void* data);

	uint16_t textStyle;
	int16_t textFont;
};

//--------------------------------------------------------------------
class cEdit: public cDisplayControl
{
public:

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint16_t style);
	virtual void setText(char* text);
};

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------










#endif
