#include "mtInterface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"

#include "mtSequencer.h"
#include "seqDisplay.h"
#include "chatBot.h"




extern void initHardware();
extern void updateHardware();



//=======================================================================
void setup()
{

	// inicjalizacja hardware jako pierwsza
	initHardware();
	engine.init();


	sequencer.init();
	sequencer.loadDefaultSequence(); // ręcznie ustawiane stepy na potrzeby testów
	//sequencer.play();

	seqDisplay.init((Sequencer::strBank*)sequencer.pattern);



	// inincjalizacja interfejsu na koncu
	mtInterface.begin();
	//sequencer.printNotes(1);		// printuje nuty w trakcie sekwencji
}

//=======================================================================
void loop()
{

	//sequencer.handle();
	//handle_chatBot();

	//seqDisplay.update();

	updateHardware();

	engine.update();
	mtInterface.update();


}
