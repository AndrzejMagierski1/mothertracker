#include "Snooze.h" // tymczasowo

#include "mtInterface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"
#include "mtProjectEditor.h"

#include "mtSequencer.h"
#include "seqDisplay.h"
#include "chatBot.h"
#include "mtPadsBacklight.h"
#include "mtSleep.h"
#include "mtExporterWAV.h"



extern Sequencer seq[2];
extern void initHardware();
extern void updateHardware();




//=======================================================================
void setup()
{
	// inicjalizacja hardware jako pierwsza
	initHardware();
	engine.init();

	sequencer.init();
	padsBacklight.init(500);
	sequencer.loadDefaultSequence(); // ręcznie ustawiane stepy na potrzeby testów
	//sequencer.play();

	seqDisplay.init((Sequencer::strPattern*)sequencer.getPatternToUI());

	// inincjalizacja interfejsu na koncu
	mtInterface.begin();
	//sequencer.printNotes(1);		// printuje nuty w trakcie sekwencji
}

//=======================================================================
void loop()
{
	powerModeUpdate();
	sequencer.handle();
	//handle_chatBot();

	seqDisplay.update();
	padsBacklight.update();
	updateExport();
	updateHardware();

	engine.update();
	mtInterface.update();
}
