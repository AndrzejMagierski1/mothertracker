

#ifndef LIBS_NEWDISPLAY_PATTERNPOPUPCONTROL_H_
#define LIBS_NEWDISPLAY_PATTERNPOPUPCONTROL_H_

#include "displayControls.h"


enum patternPopupColumnOptions
{

	columnOptionDoubleWide 	= (1<<16),

};


struct strPatternPopup
{
	uint8_t columnsCount;

	struct strColumn
	{
		uint8_t type = 0;
		char* title = nullptr;
		char* text = nullptr;
		int value = 0;
		int max = 0;
		void* data = nullptr;
		uint32_t options = 0;

	} column[5];

};



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cPatternPopup: public cDisplayControl
{
public:

	cPatternPopup(strControlProperties* properties = nullptr);
	virtual ~cPatternPopup();

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

	strPatternPopup* popup;

	uint16_t ramPartSize[6] = {0};
	uint8_t refreshStep = 0;
	uint8_t selfRefresh = 0;


	void refresh1();
	void refresh2();
	void refresh3();
	void refresh4();
	void refresh5();
	void refresh6();

	uint16_t x;
	uint16_t y;


	cDisplayControl* column[5] = {nullptr};

	uint32_t popupColumnLabelColors[3]=
	{
		0xFFFFFF, // tekst
		0xFFFFFF, // tło
		0xFF0000, // ramka
	};


};








#endif
