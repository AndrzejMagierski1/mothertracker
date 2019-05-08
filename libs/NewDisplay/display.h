
#ifndef LIBS_NEWDISPLAY_DISPLAY_H_
#define LIBS_NEWDISPLAY_DISPLAY_H_


#include <stdint.h>

#include "displayControls.h"
#include "trackerControl.h"
#include "commonControls.h"


typedef cDisplayControl* hControl;


//#########################################################################
//							DEFINICJE
//#########################################################################

const uint8_t controlsCount = 50;
const uint32_t controlsRamStartAddress = 100000;
const uint32_t controlsRamAddressStep = 10000;

const uint8_t displayFontCount = 2;
const uint8_t displayBitmapsCount = 1;

// NIE-EDITABLE
const uint8_t controlsRefreshQueueSize = controlsCount+1;

#define DISP_RGB(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))


//#########################################################################
//							KLASY
//#########################################################################

class cDisplay
{
public:

	void begin();
	void update();

	template <typename controlClass>
	hControl createControl(strControlProperties* properties)
	{
		hControl* newControlSlot = &controlsTable[0];

		uint8_t i = 0;

		while(nullptr != *newControlSlot)
		{
			newControlSlot++;
			if(++i >= controlsCount) return nullptr; //zabezpieczenie przed przekroczeniem max ilosci mozliwych kontrolek
		}

		//operacje specjalne przed utworzeniem obiektu---------------

		//sprawdzenie porpawnosci czcionki
		if(((properties->style >> 8) & 15) > displayFontCount)
		{
			properties->style &= ~(15 << 8);
		}

		//------------------------------------------------------------

		hControl newControl = new controlClass(properties);

		for(uint8_t i = 0; i < controlsCount;i++)
		{
			if(memoryMap[i] == 0)
			{
				 memoryMap[i] = 1;
				 newControl->ramMapPosition = i;
				 break;
			}
		}

		*newControlSlot = newControl;

		return *newControlSlot;
	}

	void setControlStyle(hControl handle, uint16_t style);
	void setControlColor(hControl handle, uint32_t colorsTable[]);
	void destroyControl(hControl handle);
	void refreshControl(hControl handle);


private:

	hControl controlsTable[controlsCount] = { nullptr };
	hControl actualUpdating;
	uint8_t updateStep;


	hControl refreshQueue[controlsRefreshQueueSize]; // FIFO
	uint8_t refreshQueueTop; 	// pozycja na nowe dane
	uint8_t refreshQueueBott; 	// pozycja do nastepnego odczytu

	uint8_t memoryMap[controlsCount] = {0};

	//kosmetyka
	struct strConfig
	{
		uint32_t bgColor = 0x000000;
	} config;

	uint8_t fontsCount = displayFontCount;

};


extern cDisplay display;



#endif
