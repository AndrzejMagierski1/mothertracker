#ifndef LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_
#define LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"

class AudioPlaySdWavFloat : public AudioStream
{
public:
	AudioPlaySdWavFloat(void) : AudioStream(0, NULL) { begin(); }
	void begin(void);
	bool play(const char *filename);
	void stop(void);
	virtual void update(void);
private:
	FsFile wavfile;
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

	} wavHeader;
	audio_block_t *block;
	float buffer[256];		// buffer one block of data
	uint16_t buffer_length = 128;		// how much data is in "buffer" (512 until last read)
	uint8_t state;
};



#endif /* LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_ */
