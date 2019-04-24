
#ifndef LIBS_NEWDISPLAY_DISPLAY_H_
#define LIBS_NEWDISPLAY_DISPLAY_H_


#include <stdint.h>

#include "displayControls.h"



typedef cDisplayControl* hControl;


//#########################################################################
//							DEFINICJE
//#########################################################################
const uint8_t controlsCount = 100;




//#########################################################################
//							KLASY
//#########################################################################
class cDisplay
{
public:

	void begin();
	void update();


	hControl createControl(uint16_t type, char text[], uint8_t state, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	void setControlState(hControl handle, uint8_t state);
	void destroyControl(hControl handle);
	void refreshControl(hControl handle);


private:

	cDisplayControl* controlsTable[controlsCount];

	cDisplayControl* refreshQueue[controlsCount];
	uint8_t refreshCount = 0;;

};


extern cDisplay display;



#endif
