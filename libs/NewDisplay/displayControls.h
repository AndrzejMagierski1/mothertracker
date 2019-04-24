
#ifndef LIBS_NEWDISPLAY_DISPALYCONTROLS_H_
#define LIBS_NEWDISPLAY_DISPALYCONTROLS_H_

#include <stdint.h>

//--------------------------------------------------------------------
class cDisplayControl
{
public:

	virtual uint8_t update() = 0;
	virtual void setColors(uint32_t colorsTable[]) = 0;

	inline uint32_t getRamAddress(){ return ramAddress; }
	inline uint32_t getRamSize() { return ramSize; }
	void setRamAddress(uint32_t address){ ramAddress = address; }
	void setRamSize(uint32_t size){ ramSize = size; }


private:

	uint32_t ramAddress;
	uint32_t ramSize;


	uint16_t posX;
	uint8_t posY;

};


//--------------------------------------------------------------------
class cLabel: public cDisplayControl
{
public:

	virtual uint8_t update();
	void create();
	void setColors(uint32_t colorsTable[]);

};

//--------------------------------------------------------------------
class cEdit: public cDisplayControl
{
	virtual uint8_t update();
	void create();
	void setColors(uint32_t colorsTable[]);
};

//--------------------------------------------------------------------
class cTracker: public cDisplayControl
{
	virtual uint8_t update();
	void create();
	void setColors(uint32_t colorsTable[]);
};

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------









#endif
