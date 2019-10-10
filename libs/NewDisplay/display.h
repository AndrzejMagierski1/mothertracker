
#ifndef LIBS_NEWDISPLAY_DISPLAY_H_
#define LIBS_NEWDISPLAY_DISPLAY_H_


#include <typeinfo>
#include <Arduino.h>

#include <stdint.h>


#include "displayControls.h"
#include "trackerControl.h"
#include "commonControls.h"
#include "spectrumControl.h"
#include "pointsControl.h"
#include "list.h"
#include "barControl.h"
#include "horizontalBarControl.h"
#include "keyboardControl.h"
#include "patternPopupControl.h"
#include "selectionWindowControl.h"
#include "progressCursorControl.h"
#include "startScreenControl.h"
#include "notePopoutControl.h"
#include "textPopupControl.h"



typedef cDisplayControl* hControl;


//#########################################################################
//							DEFINICJE
//#########################################################################

const uint8_t controlsCount = 55;
const uint32_t controlsRamStartAddress = 100000;
const uint32_t controlsRamAddressStep = 10000;

const uint8_t displayFontCount = 2;
const uint8_t displayBitmapsCount = 5;

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

		uint8_t limit = 0;

		while(nullptr != *newControlSlot)
		{
			newControlSlot++;
			if(++limit >= controlsCount) return nullptr; //zabezpieczenie przed przekroczeniem max ilosci mozliwych kontrolek
		}

		//operacje specjalne przed utworzeniem obiektu---------------

		//sprawdzenie porpawnosci czcionki
		if(((properties->style >> 8) & 15) > displayFontCount)
		{
			properties->style &= ~(15 << 8);
		}

		//------------------------------------------------------------


		hControl newControl = new controlClass(properties);
		//int8_t memory_slot = -1;

		for(uint8_t i = 0; i < controlsCount;i++)
		{
			if(memoryMap[i] == 0)
			{
				 memoryMap[i] = 1;
				 newControl->ramMapPosition = i;
				 //memory_slot = i;
				 break;
			}
		}

		*newControlSlot = newControl;

		return *newControlSlot;
	}

	void destroyControl(hControl handle);
	void refreshControl(hControl handle);
	void synchronizeRefresh();
	void resetControlQueue();

	void setControlPosition(hControl handle, int16_t x, int16_t y);
	void setControlSize(hControl handle, int16_t w, int16_t h);
	void setControlStyle(hControl handle, uint32_t style);
	void setControlShow(hControl handle);
	void setControlHide(hControl handle);
	void setAddControlStyle(hControl handle, uint32_t style);
	void setRemoveControlStyle(hControl handle, uint32_t style);
	void setControlText(hControl handle, char* text);
	inline void setControlText(hControl handle,  char const* text) { setControlText(handle, (char*)text); }
	void setControlValue(hControl handle, int value);
	void setControlColors(hControl handle, uint32_t colorsTable[]);
	void setControlDefaultColors(hControl handle, uint32_t colorsTable[]);
	void setControlData(hControl handle, void* data);

	/// grupowe
	void hideAllControls();


	// hardware
	void setBacklightBrightness(uint8_t value);
	void setRotate(uint8_t value);


	void disable() {state = 0;}
	void enable()  {state = 1;}
	void clear();

private:

	uint8_t state = 1;

	hControl controlsTable[controlsCount] = { nullptr };
	hControl actualUpdating;
	uint8_t updateStep;


	uint8_t stopAppend;
	uint8_t synchQueuePosition;
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

	int8_t controls_count = 0;


	uint8_t updateDisplaySettings = 0;

	uint8_t backlightBrightness;
	uint8_t lastBacklightBrightness;

	uint8_t rotateValue;
	uint8_t lastRotateValue = 1;
};


extern cDisplay display;



#endif
