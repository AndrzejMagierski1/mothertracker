
#ifndef LIBS_NEWDISPLAY_DISPLAY_H_
#define LIBS_NEWDISPLAY_DISPLAY_H_


#include <typeinfo>
#include <Arduino.h>
#include <stdint.h>

#include "SD.h"

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
#include "imgControl.h"
#include "multiLabelControl.h"
#include "sliceControl.h"
#include "multiLabelControl.h"
#include "wtProgress.h"
#include "processingPopout.h"
#include "simpleTextControl.h"
#include "lineIndicatorControl.h"
#include "songPlayerControl.h"
#include "textBoxControl.h"


#include "displayStructs.h"



//#########################################################################
//							KLASY
//#########################################################################

class cDisplay
{
	friend cDebugLog;

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
	uint8_t isIdle();
	void forceAppedStage();

	void setControlPosition(hControl handle, int16_t x, int16_t y);
	void setControlSize(hControl handle, int16_t w, int16_t h);
	void setControlStyle(hControl handle, uint32_t style);
	void setControlStyle2(hControl handle, uint32_t style);
	void setControlShow(hControl handle);
	void setControlHide(hControl handle);
	void setAddControlStyle(hControl handle, uint32_t style);
	void setRemoveControlStyle(hControl handle, uint32_t style);
	void setControlText(hControl handle, char* text);
	inline void setControlText(hControl handle,  char const* text) { setControlText(handle, (char*)text); }

	void setControlText2(hControl handle, char* text);
	inline void setControlText2(hControl handle,  char const* text) { setControlText2(handle, (char*)text); }

	void setControlValue(hControl handle, int value);
	void setControlColors(hControl handle, uint32_t colorsTable[]);
	void setControlDefaultColors(hControl handle, uint32_t colorsTable[]);
	void setControlData(hControl handle, void* data);

	// jpg/png
	uint8_t readImgFromSd(char* path, uint32_t address, int16_t max_w, int16_t max_h);
	void readImgFromMemory(uint8_t* data, uint32_t size);
	uint8_t isImgLoaded() { return img.status == 3; }
	uint8_t getImgLoadState() { return img.status; }


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
	uint8_t fontsCount = displayFontCount;
	int8_t controls_count = 0;

	uint8_t updateDisplaySettings = 0;

	uint8_t backlightBrightness;
	uint8_t lastBacklightBrightness;

	uint8_t rotateValue;
	uint8_t lastRotateValue = 1;

	//png/jpg
	uint8_t loadImage = 0;

	SdFile imgFile;

	struct imgLoader
	{
		uint8_t  status = 0; // 0-1-2-3
		uint8_t  loadProgress = 0;
		uint32_t progressMax = 0;
		uint8_t  type;
		uint8_t* data;
		uint32_t size;

		uint32_t address;

	} img;


};


extern cDisplay display;



#endif
