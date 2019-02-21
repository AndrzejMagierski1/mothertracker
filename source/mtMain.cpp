#include "mtInterface.h"
#include "mtHardware.h"
#include "mtAudioEngine.h"
#include "mtProjectEditor.h"

#include "mtSequencer.h"
#include "seqDisplay.h"
#include "chatBot.h"



extern Sequencer seq[2];
extern void initHardware();
extern void updateHardware();



//=======================================================================
void setup()
{
	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		mtProject.mtProjectRemote.instrumentFile[i].index = -1;
	}
	for(uint8_t i=0; i < SAMPLES_COUNT; i++)
	{
		mtProject.mtProjectRemote.sampleFile[i].index = -1;
	}
	for(uint8_t i=0; i < PATTERNS_COUNT; i++)
	{
		mtProject.mtProjectRemote.patternFile[i].index = -1;
	}
	// inicjalizacja hardware jako pierwsza
	initHardware();
	engine.init();


	sequencer.init();
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
	sequencer.handle();
	//handle_chatBot();

	seqDisplay.update();

	updateHardware();

	engine.update();
	mtInterface.update();
}
