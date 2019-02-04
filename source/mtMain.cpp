
#include "mtInterface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"

#include "mtSequencer.h"
#include "seqDisplay.h"



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
	sequencer.play();

	seqDisplay.init(&sequencer.seq[0]);



	// inincjalizacja interfejsu na koncu
	mtInterface.begin();


}

//=======================================================================
void loop()
{

	sequencer.handle();
	//seqDisplay.update();

	updateHardware();

	engine.update();
	mtInterface.update();


}
