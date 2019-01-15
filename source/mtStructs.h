
#ifndef MT_STRUCTS_H
#define MT_STRUCTS_H

#include <stdint.h>

//=====================================================================
const uint16_t MAX_16BIT =              		65535;
const uint32_t SAMPLE_MEMORY_MAX =      		8*1024*1024;
const uint8_t  INSTRUMENTS_MAX =        		32;
const uint8_t  SAMPLES_MAX =            		32;
const uint8_t  SAMPLES_FILENAME_LENGTH_MAX =    32;

const uint8_t SAMPLE_POINT_POS_MIN =            0;
const uint16_t SAMPLE_POINT_POS_MAX =           MAX_16BIT;

//=====================================================================

//=====================================================================
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


struct strSampleBank
{
	struct strSampleBankSlot
	{
		uint8_t loaded;
		int16_t *address;
		char *file_name;
		uint32_t length;

	} sample[SAMPLES_MAX];

	uint32_t max_memory = SAMPLE_MEMORY_MAX;
	uint32_t used_memory;
	uint8_t samples_count;

};


struct strInstrument
{
	uint8_t sampleIndex;

	uint8_t  playMode;
	uint16_t startPoint;
	uint16_t loopPoint1;
	uint16_t loopPoint2;
	uint16_t endPoint;

	uint16_t ampDelay;
	uint16_t ampAttack;
	uint16_t ampHold;
	uint16_t ampDecay;
	uint16_t ampSustain;
	uint16_t ampRelease;

	int16_t panning;
};



struct strStep
{
	uint8_t instrumentIndex;
	uint8_t volume;
	int8_t note;
	float pitchCtrl;
	//TODO: FX

};

struct strTrack
{
	strStep step[32];
	uint8_t volume;
	uint8_t enabled;

};
struct strPatern
{
	strTrack track[8];
};

struct strMtProject
{
	strSampleBank sampleBank;
	strInstrument instrument[INSTRUMENTS_MAX];

	uint8_t instruments_count;

};

extern strMtProject mtProject;
extern strPatern mtPatern;

enum memoryPlayStatus
{
	successInit=0,
	badStartPoint,
	badLoopPoint1,
	badLoopPoint2,
	pointsBeyondFile
};

#endif
