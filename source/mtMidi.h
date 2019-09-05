
#ifndef SOURCE_MTMIDI_H_
#define SOURCE_MTMIDI_H_
#include <Arduino.h>
#include "MIDI.h"
#include "patternEditor.h"


void midiInit();
void midiUpdate();
void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);



#endif /* SOURCE_MTMIDI_H_ */