#ifndef SOURCE_MTEFFECTOR_H_
#define SOURCE_MTEFFECTOR_H_

#include "mtFileManager.h"
#include "mtAudioEngine.h"
#include "mtStructs.h"

class Effector
{
public:

	void loadSample(const char *patch, int16_t * buf);
	void play(uint16_t start, uint16_t stop);
	void playPrev(uint8_t effect);
	void stop();
	void trim(uint16_t a, uint16_t b);
	void save(const char *patch);
	void setEffects();


	int16_t * getAddress();
	uint32_t getLength();

private:
	void writeOutHeader();
	int16_t * currentAddress;
	int16_t * startAddress;

	int16_t * startAddressEffect;
	uint32_t affterEffectLength; // w bajtach

	uint32_t ChunkSize = 0L;
	uint32_t Subchunk1Size = 16;
	uint32_t AudioFormat = 1;
	uint32_t numChannels = 1;
	uint32_t sampleRate = 44100;
	uint32_t bitsPerSample = 16;
	uint32_t byteRate = sampleRate*numChannels*(bitsPerSample/8);
	uint32_t blockAlign = numChannels*bitsPerSample/8;
	uint32_t Subchunk2Size = 0;
	uint32_t fileByteSaved = 0; // w bajtach
	uint32_t NumSamples = 0;
	uint8_t byte1, byte2, byte3, byte4;

	FsFile file;
};



#endif /* SOURCE_MTEFFECTOR_H_ */
