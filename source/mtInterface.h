

#ifndef MTINTERFACE_H
#define MTINTERFACE_H


#include "keyScanner.h"
#include "mtLED.h"
#include "Arduino.h"
#include "Wire.h"
#include "mtHardware.h"





#include <stdint.h>
#include "elapsedMillis.h"




class cMtInterface
{
public:
	void begin();
	void update();
	void setOperatingMode(uint8_t mode);

private:

	void processOperatingMode();

	uint8_t operatingMode;
	elapsedMillis startupTimer;



};










extern cMtInterface mtInterface;



#endif
