
#ifndef MT_STRUCTS_H
#define MT_STRUCTS_H


//=====================================================================
#define SAMPLE_MEMORY_MAX					8*1024*1024
#define INSTRUMENTS_MAX					32
#define SAMPLES_MAX						32
#define SAMPLES_FILENAME_LENGTH_MAX		32



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
#endif
