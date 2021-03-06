
#include "mtAudioEngine.h"

#include "mtStructs.h"


#include "mtPadBoard.h"


#include "mtSequencer.h"


cMtPadBoard mtPadBoard;



const uint8_t padBoard[48] =
{
	36,37,38,39,40,41,42,43,44,45,46,47,
	24,25,26,27,28,29,30,31,32,33,34,35,
	12,13,14,15,16,17,18,19,20,21,22,23,
	0,1,2,3,4,5,6,7,8,9,10,11,
};



uint8_t cMtPadBoard::convertPadToNote(uint8_t pad)
{
	if(pad  > 48) pad = 48;
	return padNotes[padBoard[pad]];
}

void cMtPadBoard::configureInstrumentPlayer(uint8_t maxVoices)
{
	if(maxVoices > 8) maxVoices = 8;
	voicesCount = maxVoices;
}

void cMtPadBoard::startInstrument(uint8_t pad, uint8_t index, int8_t velocity)
{
	int8_t voiceToTake = getEmptyVoice();
	if(voiceToTake < 0) return;

	voices[voiceToTake] = pad;

	//mtPrint("start: ");
	//mtPrintln(voiceToTake);
	instrumentPlayer[voiceToTake].noteOff();
	// >= 48 oznacza slice
	if(pad < 48) instrumentPlayer[voiceToTake].noteOnforPrev(index, convertPadToNote(pad),velocity);
	else instrumentPlayer[voiceToTake].noteOnforPrev(index, convertPadToNote(12), velocity);
}

void cMtPadBoard::startInstrument(uint8_t note, int16_t * addres, uint32_t length, int8_t velocity, int16_t source)
{
	int8_t voiceToTake = getEmptyVoice();
	if(voiceToTake < 0) return;

	voices[voiceToTake] = source;

	instrumentPlayer[voiceToTake].noteOnforPrev(addres, length, note, velocity, mtSampleTypeWaveFile);
}

int8_t cMtPadBoard::getEmptyVoice()
{
	for(uint8_t i = 0; i < voicesCount; i++)
	{
		if(voices[i] == -1) return i;
	}
	return -1;
}

int8_t cMtPadBoard::getVoiceTakenByPad(int16_t source)
{
	for(uint8_t i = 0; i < voicesCount; i++)
	{
		if(voices[i] == source) return i;
	}
	return -1;
}



void cMtPadBoard::stopInstrument(int16_t source)
{
	int8_t voiceToClear = getVoiceTakenByPad(source);
	if(voiceToClear < 0) return;
	voices[voiceToClear] = -1;
	//mtPrint("stop: ");
	//mtPrintln(voiceToClear);
	instrumentPlayer[voiceToClear].noteOff(-3); // zmienione na cut ze wzgledu na to ze i tak nie jest uzywane nigdzie oprocz miejsc gdzie nie ma envelopow
}

void cMtPadBoard::cutInstrument(uint8_t pad)
{
	int8_t voiceToClear = getVoiceTakenByPad(pad);
	if(voiceToClear < 0) return;
	voices[voiceToClear] = -1;
	//mtPrint("stop: ");
	//mtPrintln(voiceToClear);
	instrumentPlayer[voiceToClear].noteOff(-3);
}

void cMtPadBoard::clearVoice(uint8_t voice)
{
	if(voice < 0) return;
	voices[voice] = -1;
}

// wylacza WSZYSTKIE INSTRUMENTY
void cMtPadBoard::releaseAllInstruments()
{
	// todo: poniżej jest raczej bez sensu
	if(sequencer.getSeqState() == 1) return;

	for(uint8_t i = 0; i<8; i++)
	{
		voices[i] = -1;
		instrumentPlayer[i].noteOff();
	}
}

void cMtPadBoard::releaseInstrumentsTakenByPads()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (voices[i] >= 0)
		{
			voices[i] = -1;
			instrumentPlayer[i].noteOff();
		}
	}
}

void cMtPadBoard::cutAllInstrument()
{
	if(sequencer.getSeqState() == 1) return;

	for(uint8_t i = 0; i<8; i++)
	{
		voices[i] = -1;
		instrumentPlayer[i].noteOff(-3);
	}
}







void cMtPadBoard::setPadNotes(uint8_t scale, uint8_t noteOffset, uint8_t rootNote)
{
	if (lastScale		!= scale
	 || lastNoteOffset	!= noteOffset
	 || lastRootNote	!= rootNote)
	{

		if(noteOffset > MAX_NOTE_OFFSET) noteOffset = MAX_NOTE_OFFSET;
		//noteOffset = constrain(noteOffset, MIN_NOTE_OFFSET, MAX_NOTE_OFFSET);

		uint16_t tempNote = rootNote; // nuta do podstawiania
		for (int8_t a = 0; a < 48; a++)
		{
			if ((a > 11) && (a % 12 == 0))
			{
				if(noteOffset < 12) tempNote = padNotes[a - (12 - noteOffset)]; // delta wiersza
				else if(noteOffset == 12) tempNote = padNotes[a - 1] + 1; // delta wiersza
				else if(noteOffset == MAX_NOTE_OFFSET)
				{
					//if(a == 48)	tempNote = padNotes[a - (8 - 4)]; // delta 4
					//else
					tempNote = padNotes[a - (12 - 5)]; // delta 5
				}
			}

			// czy jest w skali
			while (!isInScale(tempNote, rootNote, scale))
			{
				tempNote++;
				//Serial.println("tempnote++");
			}
			padNotes[a] = constrain(tempNote, MIN_NOTE, MAX_NOTE);
			if (tempNote < MAX_NOTE) tempNote++;
		}
		lastNoteOffset 	= noteOffset;
		lastScale 		= scale;
		lastRootNote 	= rootNote;
	}
}



uint8_t cMtPadBoard::getPadsWithNote(int8_t note, uint8_t* pads)
{
	uint8_t count = 0;

	for(uint8_t i = 0; i < 48; i++)
	{
		if(padNotes[padBoard[i]] == note)
		{
			pads[i] = 1;
			count++;
		}
		else pads[i] = 0;
	}

	return count;
}

uint8_t cMtPadBoard::getNoteFromPad(int8_t pad)
{
	if(pad == INTERFACE_BUTTON_PREVIEW) return 60;
	return padNotes[padBoard[pad]];
}





