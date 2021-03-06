#ifndef MTRECORDER_H_
#define MTRECORDER_H_

#include "mtAudioEngine.h"
#include "wavHeaderReader.h"

class Recorder
{
public:
	void startRecording(int16_t * addr);
	void stopRecording();
	void play(uint16_t start, uint16_t stop);
	uint8_t update();
	void stop();
	void trim(uint16_t a, uint16_t b);
	uint8_t startSave(char * name);
	void updateSave();
	void stopSave();
	uint8_t getSaveProgress();
	uint8_t getSaveState();
	void undo(int16_t * address, uint32_t length);
	int16_t * getAddress();
	int16_t * getStartAddress();
	uint32_t getLength();
	uint8_t mode = recorderModeStop;
private:
	void writeOutHeader();
	char currentName[32];
	uint32_t saveLength;
	int16_t * currentAddress;
	int16_t * startAddress;
	uint8_t saveInProgressFlag = 0;
	strWavFileHeader header;

	uint32_t recByteSaved = 0;

	SdFile rec;
};

extern Recorder recorder;
extern int16_t* sdram_ptrSampleBank;
extern int16_t* sdram_ptrEffectsBank;


#endif /* SOURCE_MTRECORDER_H_ */
