
#ifndef SOURCE_MTMIDI_H_
#define SOURCE_MTMIDI_H_
#include <Arduino.h>
#include "MIDI.h"
#include "patternEditor/patternEditor.h"
#include "configEditor/configEditor.h"



void midiInit();
void midiUpdate();
void handleJackNoteOn(byte channel, byte pitch, byte velocity);
void handleJackNoteOff(byte channel, byte pitch, byte velocity);

void handleUsbNoteOn(byte channel, byte pitch, byte velocity);
void handleUsbNoteOff(byte channel, byte pitch, byte velocity);

// hocki clocki
void sendMidiStart();
void sendMidiClock();
void sendMidiStop();

void receiveUsbRealtime(uint8_t type);
//void receiveJackRealtime(uint8_t type);
void receiveUsbStart();
void receiveUsbStop();
void receiveJackStart();
void receiveJackStop();
void receiveJackClock();


void receiveClock();
void receiveStart();
void receiveStop();

void midiForceStep();

void sendCC(uint8_t, uint8_t);
void sendMidiNoteOn(uint8_t note, uint8_t velo, uint8_t channel);
void sendMidiNoteOff(uint8_t note, uint8_t velo, uint8_t channel);

#endif /* SOURCE_MTMIDI_H_ */
