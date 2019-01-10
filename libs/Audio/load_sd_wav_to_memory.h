
#ifndef load_sd_wav__to_memory_h_
#define load_sd_wav__to_memory_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"

	//parametry przetwarzanego pliku
struct strWavFileHeader
{
	uint32_t chunkId;			//0
	uint32_t chunkSize;
	uint32_t format;

	uint32_t subchunk1Id;		//12
	uint32_t subchunk1Size;
	uint16_t AudioFormat;		//20
	uint16_t numChannels;		//22
	uint32_t sampleRate;		//24
	uint32_t byteRate;			//28
	uint16_t blockAlign;		//32
	uint16_t bitsPerSample;		//34

	uint32_t subchunk2Id;		//36
	uint32_t subchunk2Size;		//40

};



int32_t loadSdWavToMemory(const char *filename, uint16_t * buf);

#endif
