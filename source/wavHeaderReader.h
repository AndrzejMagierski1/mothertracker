
#ifndef SOURCE_WAVHEADERREADER_H_
#define SOURCE_WAVHEADERREADER_H_

#include "Arduino.h"
#include "SD.h"

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

void readHeader(strWavFileHeader* header, SdFile * wavfile);
void readHeader(strWavFileHeader* header, ExFatFile * wavfile);
void readHeader(strWavFileHeader* header, FatFile * wavfile);
#endif /* SOURCE_WAVHEADERREADER_H_ */
