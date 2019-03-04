#ifndef MTRECORDER_H_
#define MTRECORDER_H_

#include "mtAudioEngine.h"

class Recorder
{
public:
	void startRecording(char * name, int16_t * addr);
	void stopRecording();
	void play(uint16_t start, uint16_t stop);
	void update();
	void stop();
	void trim(uint16_t a, uint16_t b);
	void save(char * patch, char * name);
	int16_t * getAddress();
	uint32_t getLength();
	uint8_t mode = recorderModeStop;
private:
	void writeOutHeader();

	char currentName[SAMPLE_NAME_SIZE];
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



#endif /* SOURCE_MTRECORDER_H_ */
