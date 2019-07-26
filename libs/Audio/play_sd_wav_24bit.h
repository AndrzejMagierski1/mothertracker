#ifndef LIBS_AUDIO_PLAY_SD_WAV_24BIT_H_
#define LIBS_AUDIO_PLAY_SD_WAV_24BIT_H_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"
#include "wavHeaderReader.h"

class AudioPlaySdWav24bit: public AudioStream
{
public:
	AudioPlaySdWav24bit(void) :
			AudioStream(0, NULL)
	{
		begin();
	}
	void begin(void);
	bool play(const char *filename);
	void stop(void);
	virtual void update(void);

private:
	strWavFileHeader wavHeader;

	uint32_t currentPosition = 0;
	FsFile wavfile;
	audio_block_t *block;
	uint8_t buffer[768];		// buffer one block of data
	uint16_t buffer_length;
	uint8_t state;
};

#endif /* LIBS_AUDIO_PLAY_SD_WAV_FLOAT_H_ */
