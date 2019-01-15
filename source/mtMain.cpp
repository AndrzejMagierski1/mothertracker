
#include "mtInterface.h"
#include "Arduino.h"

#include "mtSequencer.h"
Sequencer sequencer;




//=======================================================================
void setup()
{
	Serial.begin(9600);


//	mtInterface.begin();
	sequencer.init();
}






//=======================================================================
void loop()
{

//	mtInterface.update();
//
//	sequencer.handle();

}
