#ifndef play_sd_wav_h_
#define play_sd_wav_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"
#include "wavHeaderReader.h"

class AudioPlaySdWav: public AudioStream
{
public:
	AudioPlaySdWav(void) :
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
	SdFile wavfile;
	audio_block_t *block;
	uint8_t buffer[512];		// buffer one block of data
	uint16_t buffer_length;
	uint8_t state;
};

#endif
