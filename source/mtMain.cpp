
#include "mtInterface.h"
#include "mtHardware.h"




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
