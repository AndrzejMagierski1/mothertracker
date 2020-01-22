
#ifndef SOURCE_MTMIDI_H_
#define SOURCE_MTMIDI_H_



void midiInit();
void midiUpdate();
void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);

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

#endif /* SOURCE_MTMIDI_H_ */
