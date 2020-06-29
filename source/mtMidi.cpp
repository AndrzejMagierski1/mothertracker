#include <Arduino.h>
#include "MIDI.h"

#include  "mtMidi.h"
#include "mtSequencer.h"

#include "configEditor/configEditor.h"
#include "patternEditor/patternEditor.h"
#include "samplePlayback/samplePlayback.h"
#include "sampleRecorder/sampleRecorder.h"
#include "sampleEditor/sampleEditor.h"

static cSamplePlayback *SP = &samplePlayback;
static cSampleRecorder *SR = &sampleRecorder;
static cSampleEditor *SE = &sampleEditor;

void midiInit()
{
	// jack
	MIDI.begin(0);
	MIDI.setHandleNoteOn(handleJackNoteOn);
	MIDI.setHandleNoteOff(handleJackNoteOff);
	MIDI.setHandleClock(receiveJackClock);
	MIDI.setHandleStart(receiveJackStart);
	MIDI.setHandleStop(receiveJackStop);

	// usb
	usbMIDI.setHandleRealTimeSystem(receiveUsbRealtime);

	usbMIDI.setHandleNoteOn(handleUsbNoteOn);
	usbMIDI.setHandleNoteOff(handleUsbNoteOff);
	usbMIDI.setHandleSongPosition(handleUsbSongPosition);
}

void midiUpdate()
{
	MIDI.read();
	usbMIDI.read();
}

uint8_t isIncomingChannelDesired(uint8_t chan)
{
	return (mtConfig.midi.notesInChannel == chan ||
			mtConfig.midi.notesInChannel == 0);
}

void handleJackNoteOn(byte channel, byte pitch, byte velocity)
{
//	Serial.printf("ON\tch: %d, pitch: %d, velo: %d \n", channel, pitch,
//					velocity);
//	MIDI.sendNoteOn(pitch, velocity, channel);

	if (velocity == 0)
	{
		handleJackNoteOff(channel, pitch, 0);
		return;
	}

	if (mtConfig.midi.notesInMode == notesIn_jack ||
			mtConfig.midi.notesInMode == notesIn_usb_and_jack)
	{

		if (isIncomingChannelDesired(channel))
		{
			if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_sequencer)
			{
				sendSelection();

				sequencer.handleNoteOn(channel, pitch, velocity, -2);
				if (isEditMode())
				{
					patternEditor.moveCursorByStep();
				}

				patternEditor.refreshPattern();
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SamplePlayback)
			{
				SP->noteOnHandle(channel, pitch, velocity, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleEditor)
			{
				SE->noteOnHandle(channel, pitch, velocity, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleRecorder)
			{
				SR->noteOnHandle(channel, pitch, velocity, pitch + 100);
			}
		}
	}

}
void handleUsbNoteOn(byte channel, byte pitch, byte velocity)
{
//	Serial.printf("ON\tch: %d, pitch: %d, velo: %d \n", channel, pitch,
//					velocity);
//	MIDI.sendNoteOn(pitch, velocity, channel);

	if (velocity == 0)
	{
		handleUsbNoteOff(channel, pitch, 0);
		return;
	}

	if (mtConfig.midi.notesInMode == notesIn_usb ||
			mtConfig.midi.notesInMode == notesIn_usb_and_jack)
	{
		if (isIncomingChannelDesired(channel))
		{
			if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_sequencer)
			{
				sendSelection();

				sequencer.handleNoteOn(channel, pitch, velocity, -2);
				if (isEditMode())
				{
					patternEditor.moveCursorByStep();
				}

				patternEditor.refreshPattern();
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SamplePlayback)
			{
				SP->noteOnHandle(channel, pitch, velocity, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleEditor)
			{
				SE->noteOnHandle(channel, pitch, velocity, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleRecorder)
			{
				SR->noteOnHandle(channel, pitch, velocity, pitch + 100);
			}
		}
	}

}

void handleJackNoteOff(byte channel, byte pitch, byte velocity)
{

//	Serial.printf("OFF\tch: %d, pitch: %d, velo: %d\n", channel, pitch,
//					velocity);

	if (mtConfig.midi.notesInMode == notesIn_jack ||
			mtConfig.midi.notesInMode == notesIn_usb_and_jack)
	{
		if (isIncomingChannelDesired(channel))
		{
			if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_sequencer)
			{
				sendSelection();
				sequencer.handleNoteOff(channel, pitch, 0, -2);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SamplePlayback)
			{
				SP->noteOffHandle(channel, pitch, 0, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleEditor)
			{
				SE->noteOffHandle(channel, pitch, 0, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleRecorder)
			{
				SR->noteOffHandle(channel, pitch, 0, pitch + 100);
			}
		}
	}
}

void handleUsbNoteOff(byte channel, byte pitch, byte velocity)
{

//	Serial.printf("OFF\tch: %d, pitch: %d, velo: %d\n", channel, pitch,
//					velocity);

	if (mtConfig.midi.notesInMode == notesIn_usb ||
			mtConfig.midi.notesInMode == notesIn_usb_and_jack)
	{
		if (isIncomingChannelDesired(channel))
		{
			if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_sequencer)
			{
				sendSelection();
				sequencer.handleNoteOff(channel, pitch, 0, -2);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SamplePlayback)
			{
				SP->noteOffHandle(channel, pitch, 0, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleEditor)
			{
				SE->noteOffHandle(channel, pitch, 0, pitch + 100);
			}
			else if (sequencer.getMidiInVoiceMode() == sequencer.midiInVoiceMode_SampleRecorder)
			{
				SR->noteOffHandle(channel, pitch, 0, pitch + 100);
			}
		}
	}
}

void sendMidiStart()
{
	switch (mtConfig.midi.transportOut)
	{
	case clockOut_usb:
		usbMIDI.sendRealTime(Start);
		break;

	case clockOut_usb_and_jack:
		usbMIDI.sendRealTime(Start);
		MIDI.sendRealTime(Start);
		break;

	case clockOut_jack:
		MIDI.sendRealTime(Start);
		break;

	default:
		break;
	}
}
void sendMidiStop()
{
	switch (mtConfig.midi.transportOut)
	{
	case clockOut_usb:
		usbMIDI.sendRealTime(Stop);
		break;

	case clockOut_usb_and_jack:
		usbMIDI.sendRealTime(Stop);
		MIDI.sendRealTime(Stop);
		break;

	case clockOut_jack:
		MIDI.sendRealTime(Stop);
		break;

	default:
		break;
	}
}

void sendMidiClock()
{
	switch (mtConfig.midi.clkOut)
	{
	case clockOut_usb:
		usbMIDI.sendRealTime(Clock);
		break;

	case clockOut_usb_and_jack:
		usbMIDI.sendRealTime(Clock);
		MIDI.sendRealTime(Clock);
		break;

	case clockOut_jack:
		MIDI.sendRealTime(Clock);
		break;

	default:
		break;
	}
}

void receiveUsbRealtime(uint8_t type)
{
	switch (type)
	{
	case Clock:
		if (mtConfig.midi.clkIn == clockIn_Usb)
		{
			receiveClock();
		}
		break;

	case Start:
		if (mtConfig.midi.transportIn == clockIn_Usb)
		{
			Serial.printf("usb start\n");
			Serial.send_now();
			receiveStart();
		}
		break;

	case Continue:
		if (mtConfig.midi.transportIn == clockIn_Usb)
		{
			Serial.printf("usb continue\n");
			Serial.send_now();
			receiveContinue();
		}
		break;

	case Stop:
		if (mtConfig.midi.transportIn == clockIn_Usb)
		{
			Serial.printf("usb stop\n");
			Serial.send_now();
			receiveStop();
		}
		break;

	default:
		break;
	}
}

void receiveJackStart()
{
	if (mtConfig.midi.transportIn == clockIn_Jack)
	{
		receiveStart();
	}
}
void receiveJackStop()
{
	if (mtConfig.midi.transportIn == clockIn_Jack)
	{
		receiveStop();
	}
}
void receiveJackClock()
{
	if (mtConfig.midi.clkIn == clockIn_Jack)
	{
		receiveClock();
	}
}

#define MAX_CLOCKS_TO_CALC 	6			//	max liczba cykli do przeliczenia tempa
elapsedMicros timer = 0;
elapsedMicros timeout = 0;

uint8_t count = 0; 			// tu liczę numer clokcka od startu
uint8_t clockStep = 0;
uint16_t lastSongPosition = 0;
uint16_t startFromPosition = 0;

uint8_t externalClockRunning = 0;

long clock_total = 0;

bool next_step = 0;
bool isFirstClock = 0;

void receiveClock()
{
	if (!externalClockRunning) return;

	if (!sequencer.isInternalClock())
	{
//		Serial.printf("cl cnt %d\n",count);
//		Serial.send_now();
		if (count == 1)
		{
			timer = 0;
			midiForceStep();
		}

		if ((count >= MAX_CLOCKS_TO_CALC))
		{
			float temp_o = constrain(12500000.0 / float(timer),
										10.0,
										(float )300.0);

			sequencer.feedExternalTempo(temp_o);

			// Serial.println("externalTempo");
			// Serial.println(player.externalTempo, 1);
			// Serial.println();

			count = 0;
			timer = 0;

		}
		count++;
	}
}
void receiveStart()
{
	startFromPosition = 0;

	externalClockRunning = 1;
	count = 1;
	clockStep = 0;

	isFirstClock = 1;
}
void receiveContinue()
{

	startFromPosition = (lastSongPosition) % (sequencer.getActualPattern()->track[0].length + 1);

	Serial.printf("startFromPosition: %d\n\n", startFromPosition);

//	receiveStart();

	externalClockRunning = 1;
	count = 1;
	clockStep = 0;

	isFirstClock = 1;

}
void receiveStop()
{
	externalClockRunning = 0;
	sequencer.stop();
}

void handleUsbSongPosition(uint16_t beats)
{
	Serial.printf("song pos: %d beats\n", beats);
	sequencer.forcePosition(beats);

	lastSongPosition = beats;
}

void midiForceStep()
{
	if (isFirstClock)
	{
		isFirstClock = 0;

		sequencer.stop();
		externalClockRunning = 1;
		sequencer.play(startFromPosition);
	}
	else
	{
		sequencer.handle_nanoStep(1);
	}

}

void sendCC(uint8_t internalControl, uint8_t value)
{
	if (mtConfig.midi.notesOutMode == notesOut_mode_usb ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		usbMIDI.sendControlChange(mtConfig.midi.ccOut[internalControl],
									value,
									mtConfig.midi.notesOutChannel + 1);
	}

	if (mtConfig.midi.notesOutMode == notesOut_mode_jack ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		MIDI.sendControlChange(mtConfig.midi.ccOut[internalControl],
								value,
								mtConfig.midi.notesOutChannel + 1);
	}
}

void sendProgramChange(uint8_t value)
{
	if (mtConfig.midi.notesOutMode == notesOut_mode_usb ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		usbMIDI.sendProgramChange(value,
									mtConfig.midi.notesOutChannel + 1);
	}

	if (mtConfig.midi.notesOutMode == notesOut_mode_jack ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		MIDI.sendProgramChange(value,
								mtConfig.midi.notesOutChannel + 1);
	}
}

void sendMidiNoteOn(uint8_t note, uint8_t velo, uint8_t channel)
{
	if (mtConfig.midi.notesOutMode == notesOut_mode_usb ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		usbMIDI.sendNoteOn(note, velo, channel);
	}

	if (mtConfig.midi.notesOutMode == notesOut_mode_jack ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		MIDI.sendNoteOn(note, velo, channel);
	}
}
void sendMidiNoteOff(uint8_t note, uint8_t velo, uint8_t channel)
{
	if (mtConfig.midi.notesOutMode == notesOut_mode_usb ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		usbMIDI.sendNoteOff(note, 0, channel);
	}

	if (mtConfig.midi.notesOutMode == notesOut_mode_jack ||
			mtConfig.midi.notesOutMode == notesOut_mode_usb_and_jack)
	{
		MIDI.sendNoteOff(note, 0, channel);
	}
}

