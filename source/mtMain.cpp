#include "mtInterface.h"
#include "Arduino.h"

#include "mtSequencer.h"
#include "chatBot.h"
Sequencer sequencer;

//=======================================================================
void setup()
{
	Serial.begin(9600);
//	mtInterface.begin();
	sequencer.init();
	sequencer.loadDefaultSequence(); // ręcznie ustawiane stepy na potrzeby testów
	sequencer.play();
	sequencer.printNotes(1);		// printuje nuty w trakcie sekwencji
}

//=======================================================================
void loop()
{

//	mtInterface.update();
//
	sequencer.handle();
	handle_chatBot();

}
