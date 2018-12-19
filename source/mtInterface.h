

#ifndef MTINTERFACE_H
#define MTINTERFACE_H

#include "keyScanner.h"
#include "Arduino.h"
#include "Wire.h"
#include "mtHardware.h"

//keyScanner
void onButtonPush			(uint8_t x, uint8_t y);
void onButtonRelease		(uint8_t x, uint8_t y);
void onButtonHold			(uint8_t x, uint8_t y);
void onButtonDouble			(uint8_t x, uint8_t y);





class cMtInterface
{
public:
	void begin();
	void update();

private:



};










extern cMtInterface mtInterface;



#endif
