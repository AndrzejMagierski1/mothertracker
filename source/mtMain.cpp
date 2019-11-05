


#include <interface.h>
#include "mtHardware.h"
#include "mtAudioEngine.h"

#include "Snooze.h" // tymczasowo

#include "mtSequencer.h"
#include "chatBot.h"
#include "mtPadsBacklight.h"
#include "mtSleep.h"
#include "mtExporterWAV.h"
#include "mtFileManager.h"






//extern Sequencer seq[2];
extern void initHardware();
extern void updateHardware();
extern AudioControlSGTL5000 audioShield;


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

	//seqDisplay.init((Sequencer::strPattern*)sequencer.getPatternToUI());

	// inincjalizacja interfejsu na koncu
	mtInterface.begin();
	//sequencer.printNotes(1);		// printuje nuty w trakcie sekwencji
}

//=======================================================================
void loop()
{
	updateHardware();

	if(!lowPower.isLowPower())
	{
		sequencer.handle();
		handle_chatBot();

		padsBacklight.update();

		updateExport();

		engine.update();
		fileManager.update();
		recorder.updateSave();
		mtInterface.update();
	}
}
