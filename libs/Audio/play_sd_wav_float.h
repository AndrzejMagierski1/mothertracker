#ifndef LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_
#define LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"
#include "wavHeaderReader.h"


class AudioPlaySdWavFloat : public AudioStream
{
public:
	AudioPlaySdWavFloat(void) : AudioStream(0, NULL) { begin(); }
	void begin(void);
	bool play(const char *filename);
	void stop(void);
	virtual void update(void);

private:
	strWavFileHeader wavHeader;

	FsFile wavfile;
	audio_block_t *block;
	float buffer[256];		// buffer one block of data
	uint16_t buffer_length = 128;		// how much data is in "buffer" (512 until last read)
	uint8_t state;
};



#endif /* LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_ */
