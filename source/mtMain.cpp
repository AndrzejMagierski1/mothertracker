
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
	sequencer.loadDefaultSequence(); // ręcznie ustawiane stepy na potrzeby testów
	sequencer.play();

//	Serial.println(sequencer.fxConsts.FX_TYPE_LP1);





}






//=======================================================================
void loop()
{

//	mtInterface.update();
//
	sequencer.handle();

}
