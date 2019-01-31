
#include "mtInterface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"

#include "mtSequencer.h"




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





	// inincjalizacja interfejsu na koncu
	mtInterface.begin();


}

//=======================================================================
void loop()
{

	sequencer.handle();


	updateHardware();

	engine.update();
	mtInterface.update();


}
