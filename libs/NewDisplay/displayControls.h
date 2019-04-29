
#ifndef LIBS_NEWDISPLAY_DISPALYCONTROLS_H_
#define LIBS_NEWDISPLAY_DISPALYCONTROLS_H_

#include <stdint.h>


enum controlsTypes
{
	displayControlLabel,
	displayControlEdit,
	displayControlBox,

	displayControlTracker



};


enum controlsState
{
	controlHide,
	controlShow,


};

// CZCIONKI
#define MT_GPU_RAM_FONT1_ADRESS	1000
#define MT_GPU_RAM_FONT1_HANDLE	13
#define MT_GPU_RAM_FONT2_ADRESS	11000
#define MT_GPU_RAM_FONT2_HANDLE	14
// handle nie moze byc wikesze niz 14

void Number2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, int16_t number);
void Char2Bitmaps(int16_t x, int16_t y, uint8_t font_x, uint8_t font_y, char* string, uint8_t length);

extern const uint8_t Roboto_Mono_10_L4[];
extern const uint8_t Roboto_Mono_14_L4[];


class cDisplay;

//--------------------------------------------------------------------


class cDisplayControl
{
	friend cDisplay;

public:

	virtual uint8_t update() = 0;
	virtual uint8_t append(uint32_t address) = 0;
	virtual uint8_t memCpy(uint32_t address) = 0;

	void setColors(uint32_t colorsTable[])
	{
		for(uint8_t i = 0; i < colorsCount;i++) if(colorsTable+i == nullptr || colorsTable[i] > 0xFFFFFF) return;
		colors = colorsTable;
	}
	static uint8_t colorsCount;
	static uint32_t defaultColors[];
	uint32_t* colors = defaultColors;

	cDisplayControl()
	{
		ramMapPosition = 0;
		ramSize = 0;
		cState = 0;
		posX = 0;
		posY = 0;
	}
	virtual ~cDisplayControl() {}

	uint8_t cState;
	char* cText;

	uint16_t posX;
	uint16_t posY;

	uint16_t cWidth;
	uint16_t cHeight;


private:


	uint8_t ramMapPosition;
	uint32_t ramSize;



};



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class cLabel: public cDisplayControl
{
public:
	cLabel(char text[] = nullptr, uint8_t state = 0, uint16_t x = 0, uint16_t y = 0, uint16_t w = 0, uint16_t h = 0);
	virtual ~cLabel();

	virtual uint8_t update();
	virtual uint8_t memCpy(uint32_t address);
	virtual uint8_t append(uint32_t address);

	//void setColors(uint32_t colorsTable[]);
	static uint8_t colorsCount;
	static uint32_t defaultColors[];
	//uint32_t* colors = defaultColors;
};

//--------------------------------------------------------------------
class cEdit: public cDisplayControl
{
public:

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

};

//--------------------------------------------------------------------
class cTracker: public cDisplayControl
{
public:

	cTracker(char text[] = nullptr, uint8_t state = 0, uint16_t x = 0, uint16_t y = 0, uint16_t w = 0, uint16_t h = 0);
	virtual ~cTracker();


	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

//	void setColors(uint32_t colorsTable[]);
	static uint8_t colorsCount;
	static uint32_t defaultColors[];
	uint32_t* colors = defaultColors;

	uint16_t ramPartSize[5] = {0};
	uint8_t refreshStep;
	void refresh1();
	void refresh2();
	void refresh3();
	void refresh4();
	void refresh5();


};

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------









#endif
