#ifndef SOURCE_SEQDISPLAY_H_
#define SOURCE_SEQDISPLAY_H_
#include "elapsedMillis.h"

#include "mtLED.h"
#include "mtSequencer.h"

const uint8_t TRACKER_LINE = 			10;
const unsigned long REFRESH_TIME = 		20;
const uint8_t TRACKER_LINE_LIGHT_OFF =	10;
const uint8_t TRACKER_LINE_LIGHT_ON =	21;
const uint16_t REFRESH_BLINK_TIME_MS = 	500;
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
void setBlink(uint8_t x, uint8_t y);
void clearAllBlink();
void setMultiBlink(uint8_t x,uint8_t y);
void clearLast();
uint8_t getBlink(uint8_t track, uint8_t step);
private:
void drawCurrentPosition();
void startPlayMode();
void updatePlayMode();
void updateStopMode();
int8_t getMaxTrackLen();

Sequencer::strPattern * sequencerPtr;
uint8_t state;
int16_t scrollShift;
uint8_t cleared = 1;
uint8_t toggler=1;
struct strLastPoint
{
	int8_t x = -1;
	int8_t y = -1;
} lastPoint;

enum seqState
{
	seqPlay = 0,
	seqStop
};

};


extern SeqDisplay seqDisplay;
extern uint8_t blinkTab[8][20];

#endif /* SOURCE_SEQDISPLAY_H_ */
