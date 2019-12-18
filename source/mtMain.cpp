


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
#include "mtSliceDetector.h"
#include "mtMidi.h"





//extern Sequencer seq[2];
extern void initHardware();
extern void updateHardware();
extern AudioControlSGTL5000 audioShield;

//=======================================================================
IntervalTimer midiReceiveTimer;
void receiveAllMidi()
{
	midiUpdate();
}
void setup()
{
	// inicjalizacja hardware jako pierwsza
	initHardware();

	engine.init();

	sequencer.init();
	padsBacklight.init(500);
//	sequencer.loadDefaultSequence(); // ręcznie ustawiane stepy na potrzeby testów
	//sequencer.play();

	//seqDisplay.init((Sequencer::strPattern*)sequencer.getPatternToUI());
	exporter.begin();
	// inincjalizacja interfejsu na koncu
	mtInterface.begin();
	//sequencer.printNotes(1);		// printuje nuty w trakcie sekwencji

	midiReceiveTimer.begin(receiveAllMidi, 2000); //  w ten spsób 'midi clock in' działa najlepiej
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

		exporter.update();
		sliceDetector.update();
		engine.update();
		fileManager.update();
		recorder.updateSave();
		mtInterface.update();
	}
}
