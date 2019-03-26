#ifndef SOURCE_SEQDISPLAY_H_
#define SOURCE_SEQDISPLAY_H_
#include "elapsedMillis.h"

#include "mtLED.h"
#include "mtSequencer.h"

const uint8_t TRACKER_LINE = 			10;
const unsigned long REFRESH_TIME = 		20;

class SeqDisplay
{
public:

void init(Sequencer::strPattern * seq);
void update();
void setMode(uint8_t s);
void setScroll(int8_t sc);
uint8_t getStep(uint8_t x, uint8_t y);
void incScroll();
void decScroll();
private:
void drawCurrentPosition();
void startPlayMode();
void updatePlayMode();
void updateStopMode();
int8_t getMaxTrackLen();

Sequencer::strPattern * sequencerPtr;
uint8_t state;
int16_t scrollShift;

enum seqState
{
	seqPlay = 0,
	seqStop
};

};


extern SeqDisplay seqDisplay;

#endif /* SOURCE_SEQDISPLAY_H_ */
