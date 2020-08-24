


#include "core/interface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"

#include "Snooze.h" // tymczasowo

#include "mtSequencer.h"
#include "chatBot.h"
#include "mtPadsBacklight.h"
#include "mtSleep.h"
#include "mtExporterWAV.h"
//#include "mtFileManager.h"
#include "fileManager.h"
#include "mtSliceDetector.h"
#include "mtMidi.h"

#include "mtConfig.h"
#include "mtRecorder.h"



//extern Sequencer seq[2];
extern void checkPowerState();
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
	checkPowerState();

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

//	Serial.printf( "unique ID: 0x%08X 0x%08X 0x%08X 0x%08X\n", SIM_UIDH, SIM_UIDMH, SIM_UIDML, SIM_UIDL );
}

//=======================================================================
void loop()
{
	updateHardware();

	if(lowPower.getLowPowerState() == shutdownStateSleep) return;

	sequencer.handle();
	handle_chatBot();

	padsBacklight.update();

//	exporter.update();
	exporter.updateSave();
	sliceDetector.update();

	//fileManager.update();
	newFileManager.update();
	recorder.updateSave();
	mtInterface.update();

}
