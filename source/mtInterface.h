

#ifndef MTINTERFACE_H
#define MTINTERFACE_H

#include "keyScanner.h"
#include "mtLED.h"
#include "Arduino.h"
#include "Wire.h"
#include "mtHardware.h"







class cMtInterface
{
public:
	void begin();
	void update();

private:



};










extern cMtInterface mtInterface;



#endif
