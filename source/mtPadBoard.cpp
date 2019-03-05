
#include "mtAudioEngine.h"

#include "mtStructs.h"


#include "mtPadBoard.h"





cMtPadBoard mtPadBoard;



const uint8_t padBoard[32] =
{
	24,25,26,27,28,29,30,31,
	16,17,18,19,20,21,22,23,
	8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,
};



uint8_t cMtPadBoard::convertPadToNote(uint8_t pad)
{
	if(pad  > 31) pad = 31;
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

	instrumentPlayer[voiceToTake].noteOn(index, convertPadToNote(pad), velocity);
}

int8_t cMtPadBoard::getEmptyVoice()
{
	for(uint8_t i = 0; i < voicesCount; i++)
	{
		if(voices[i] == -1) return i;
	}
	return -1;
}

int8_t cMtPadBoard::getVoiceTakenByPad(uint8_t pad)
{
	for(uint8_t i = 0; i < voicesCount; i++)
	{
		if(voices[i] == pad) return i;
	}
	return -1;
}



void cMtPadBoard::stopInstrument(uint8_t pad)
{
	int8_t voiceToClear = getVoiceTakenByPad(pad);
	if(voiceToClear < 0) return;
	voices[voiceToClear] = -1;
	//mtPrint("stop: ");
	//mtPrintln(voiceToClear);
	instrumentPlayer[voiceToClear].noteOff();
}


// zwraca 1 jeśli jest w skali
uint8_t cMtPadBoard::isInScale(uint8_t note,		// nuta do sprawdzenia ,
                  uint8_t root,		// root note
                  uint8_t scale)	// numer skali
{
	//note = constrain(note, 0, 255);
	//root = constrain(root, 0, 127);
	//scale = constrain(scale, MIN_SCALE, MAX_SCALE);

	//if(note > MAX_NOTE_OFFSET) note = MAX_NOTE_OFFSET;
	//if(root > 127) root = 127;
	if(scale > MAX_SCALE) scale = MAX_SCALE;

	uint8_t noteName = note % 12;
	uint8_t rootName = root % 12;


	if(noteName == rootName)
	{
		//Serial.println(" - yes");
		return 1;
	}
	else if(noteName > rootName)
	{
		if(scaleDef[scale] & (1 << (noteName - rootName)))
		{
			//Serial.println(" - yes");
			return 1;
		}
	}
	else
	{
		if(scaleDef[scale] & (1 << ((12-rootName)+noteName)))
		{
			//Serial.println(" - yes");
			return 1;
		}
	}

	//Serial.println(" - no");
	return 0;
}



void cMtPadBoard::setPadNotes(uint8_t scale, uint8_t noteOffset, uint8_t rootNote)
{
	if (lastScale		!= scale
	 || lastNoteOffset	!= noteOffset
	 || lastRootNote	!= rootNote)
	{
		// ptrPreset->scale = 3;
		// Serial.print("scale: ");
		// Serial.println(ptrPreset->scale);
		// Serial.print("rootNote: ");
		// Serial.println(ptrPreset->rootNote);

		if(noteOffset > MAX_NOTE_OFFSET) noteOffset = MAX_NOTE_OFFSET;
		//noteOffset = constrain(noteOffset, MIN_NOTE_OFFSET, MAX_NOTE_OFFSET);
		// synth.lastNoteOffset = constrain(synth.lastNoteOffset, 1, 7);

		uint16_t tempNote = rootNote; // nuta do podstawiania
		for (int8_t a = 0; a < 32; a++)
		{
			if ((a > 7) && (a % 8 == 0))
			{
				if(noteOffset < 8) tempNote = padNotes[a - (8 - noteOffset)]; // delta wiersza
				else if(noteOffset == MAX_NOTE_OFFSET)
				{
					if(a == 48)	tempNote = padNotes[a - (8 - 4)]; // delta 4
					else		tempNote = padNotes[a - (8 - 5)]; // delta 5
				}
			}

			// czy jest w skali
			while (!isInScale(tempNote, rootNote, scale))
			{
				tempNote++;
				//Serial.println("tempnote++");
			}
			padNotes[a] = tempNote;
			tempNote++;

			//if(tempNote > MAX_MEDUSA_NOTE) synth.padNotes[a] = -1;
			// Serial.println(tempNote);

			// Serial.print("a: ");
			// Serial.print(a);
			// Serial.print(" tempNote: ");
			// Serial.println(tempNote);
		}
		lastNoteOffset 	= noteOffset;
		lastScale 		= scale;
		lastRootNote 	= rootNote;
	}
}








