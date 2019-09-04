#include  "mtMidi.h"
#include <Arduino.h>
#include "mtSequencer.h"

void midiInit()
{
	MIDI.begin(0);
	MIDI.setHandleNoteOn(handleNoteOn);
	MIDI.setHandleNoteOff(handleNoteOff);

}

void midiUpdate()
{
	MIDI.read();
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
	Serial.printf("ON\tch: %d, pitch: %d, velo: %d \n", channel, pitch,
					velocity);
	MIDI.sendNoteOn(pitch, velocity, channel);

	if (patternEditor.editMode == 1)
	{
		sendSelection();
		if (!isMultiSelection())
		{
			sequencer.handleNote(channel, pitch, velocity);
			patternEditor.moveCursorByStep();
		}
	}
	patternEditor.refreshPattern();

}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{

	Serial.printf("OFF\tch: %d, pitch: %d, velo: %d\n", channel, pitch,
					velocity);
	MIDI.sendNoteOff(pitch, velocity, channel);

	if (patternEditor.editMode == 1)
	{
		sendSelection();
		sequencer.handleNote(channel, pitch, 0);
	}
}
