

#ifndef play_wav_from_memory_h_
#define play_wav_from_memory_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"

class AudioPlayWavFromMemory : public AudioStream
{
public:
	AudioPlayWavFromMemory(void) : AudioStream(0, NULL) { begin(); }
	void begin(void);
	bool play(int16_t * buf, uint32_t bufLen);
	void stop(void);
	bool isPlaying(void);
	float positionMillis(void);
	float lengthMillis(void);
	virtual void update(void);
private:

	uint32_t data_length;		// number of bytes remaining in current section
	audio_block_t *block;
	int16_t * memoryBuf;
	uint8_t state;
	uint32_t dataCnt=0;
};

#endif
