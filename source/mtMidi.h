
#ifndef SOURCE_MTMIDI_H_
#define SOURCE_MTMIDI_H_

enum mtConfigSpecific
{
	configDefaultGeneral,
	configDefaultFirmware,
	configDefaultMIDI,
	configDefaultSD,
	configDefaultHelp,
	configDefaultCredits,

	mtConfigGroupsCount
};

enum mtConfigMIDI
{
	configMIDIClockIn,
	configMIDIClockOut,
	configMIDITransportIn,
	configMIDITansportOut,
	configMIDICcout,
	configMIDINotesFrom_mode,
	configMIDINotesFrom_channel,
	configMIDINotesOut_mode,
	configMIDINotesOut_channel,

	configMIDI_count
};

enum mtConfigGeneral
{
	configGeneralPatternDiv,
	configGeneralRadioRegion,
	configGeneralBrightness,

};

enum enClockOut
{
	clockOut_Off,
	clockOut_usb,
	clockOut_jack,
	clockOut_usb_and_jack,
	clockOut_count
};

enum enNotesIn
{
	notesIn_None,
	notesIn_usb,
	notesIn_jack,
	notesIn_usb_and_jack,
};

enum enNotesOut
{
	notesOut_mode_None,
	notesOut_mode_usb,
	notesOut_mode_jack,
	notesOut_mode_usb_and_jack,
};

enum enClockIn
{
	clockIn_Internal,
	clockIn_Usb,
	clockIn_Jack,

};



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
