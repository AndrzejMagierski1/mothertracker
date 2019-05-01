
#ifndef LIBS_NEWDISPLAY_DISPLAY_H_
#define LIBS_NEWDISPLAY_DISPLAY_H_


#include <stdint.h>

#include "displayControls.h"




typedef cDisplayControl* hControl;


//#########################################################################
//							DEFINICJE
//#########################################################################

const uint8_t controlsCount = 50;
const uint32_t controlsRamStartAddress = 100000;


// NIE-EDITABLE
const uint8_t controlsRefreshQueueSize = controlsCount+1;



//#########################################################################
//							KLASY
//#########################################################################

class cDisplay
{
public:

	void begin();
	void update();

	template <typename controlClass>
	hControl createControl(char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
	{
		hControl* newControlSlot = &controlsTable[0];

		uint8_t i = 0;

		while(nullptr != *newControlSlot)
		{
			newControlSlot++;
			if(++i >= controlsCount) return nullptr; //zabezpieczenie przed przekroczeniem max ilosci mozliwych kontrolek
		}

		hControl newControl = new controlClass(text, state, x, y, w, h);


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

	void setControlState(hControl handle, uint8_t state);
	void destroyControl(hControl handle);
	void refreshControl(hControl handle);


private:

	hControl controlsTable[controlsCount] = { nullptr };
	hControl actualUpdating;
	uint8_t updateStep;


	hControl refreshQueue[controlsRefreshQueueSize]; // FIFO
	uint8_t refreshQueueTop; // pozycja na nowe dane
	uint8_t refreshQueueBott; // pozycja do nastepnego odczytu

	uint8_t memoryMap[controlsCount] = {0};

	//kosmetyka
	struct strConfig
	{
		uint32_t bgColor = 0x000000;
	} config;

};


extern cDisplay display;



#endif
