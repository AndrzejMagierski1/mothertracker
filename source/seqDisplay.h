#ifndef SOURCE_SEQDISPLAY_H_
#define SOURCE_SEQDISPLAY_H_
#include "elapsedMillis.h"

#include "mtLED.h"
#include "mtSequencer.h"

const uint8_t TRACKER_LINE = 			10;
const unsigned long REFRESH_TIME = 		50;

class SeqDisplay
{
public:

void init(Sequencer::strBank * seq);
void update();
void setMode(uint8_t s);

private:

void startPlayMode();
void updatePlayMode();
void updateStopMode();
int8_t getMaxTrackLen();

Sequencer::strBank * sequencerPtr;
uint8_t state;

elapsedMillis refreshTimer;
enum seqState
{
	seqPlay = 0,
	seqStop
};

};


extern SeqDisplay seqDisplay;

#endif /* SOURCE_SEQDISPLAY_H_ */
