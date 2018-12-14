
#include "mtInterface.h"
#include "Arduino.h"

#include "mtSequencer.h"


//=======================================================================
void setup()
{
	Serial.begin(9600);


	mtInterface.begin();
}






//=======================================================================
void loop()
{


	mtInterface.update();
}
