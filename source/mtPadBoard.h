#ifndef _MT_PAD_BOARD_H
#define _MT_PAD_BOARD_H

#include <stdint.h>

#include "scales.h"

constexpr uint8_t INTERFACE_BUTTON_PREVIEW = 96;

class cMtPadBoard
{
public:
	uint8_t convertPadToNote(uint8_t pad);
	void setPadNotes(uint8_t scale, uint8_t noteOffset, uint8_t rootNote);
	void configureInstrumentPlayer(uint8_t maxVoices);
	void startInstrument(uint8_t pad, uint8_t index, int8_t velocity);
	void startInstrument(uint8_t note,int16_t * addres, uint32_t length, int16_t source);
	void stopInstrument(int16_t pad);
	void cutInstrument(uint8_t pad);
	int8_t getVoiceTakenByPad(int16_t pad);
	void clearVoice(uint8_t voice);

	void releaseAllInstruments();
	void releaseInstrumentsTakenByPads();
	void cutAllInstrument();


	uint8_t getPadsWithNote(int8_t note, uint8_t* pads);
	uint8_t getNoteFromPad(int8_t pad);
	int8_t getEmptyVoice();

private:

//	uint8_t isInScale(uint8_t note, uint8_t root, uint8_t scale);
//	int8_t getVoiceTakenByPad(uint8_t pad);


	uint8_t lastScale = 255;
	uint8_t lastNoteOffset = 255;
	uint8_t lastRootNote = 255;

	uint8_t voicesCount;

	uint8_t padNotes[48];

	int16_t voices[8] = {-1,-1,-1,-1,-1,-1,-1,-1};



};







extern cMtPadBoard mtPadBoard;




#endif
