#ifndef MTRECORDER_H_
#define MTRECORDER_H_

#include "mtAudioEngine.h"

class Recorder
{
public:
	void startRecording(int16_t * addr);
	void stopRecording();
	void play(uint16_t start, uint16_t stop);
	void update();
	void stop();
	void trim(uint16_t a, uint16_t b);
	uint8_t startSave(char * name, uint8_t type = 0);
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

	uint32_t saveLength;
	int16_t * currentAddress;
	int16_t * startAddress;
	uint32_t ChunkSize = 0L;
	uint32_t Subchunk1Size = 16;
	uint32_t AudioFormat = 1;
	uint32_t numChannels = 1;
	uint32_t sampleRate = 44100;
	uint32_t bitsPerSample = 16;
	uint32_t byteRate = sampleRate*numChannels*(bitsPerSample/8);
	uint32_t blockAlign = numChannels*bitsPerSample/8;
	uint32_t Subchunk2Size = 0;
	uint32_t recByteSaved = 0;
	uint32_t NumSamples = 0;
	uint8_t byte1, byte2, byte3, byte4;

	FsFile rec;
};

extern Recorder recorder;
extern int16_t sdram_effectsBank[4*1024*1024];
extern int16_t sdram_sampleBank[4*1024*1024];


#endif /* SOURCE_MTRECORDER_H_ */
