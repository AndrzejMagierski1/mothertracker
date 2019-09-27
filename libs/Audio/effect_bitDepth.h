#ifndef LIBS_AUDIO_EFFECT_BITDEPTH_H_
#define LIBS_AUDIO_EFFECT_BITDEPTH_H_
#include "Arduino.h"
#include "AudioStream.h"

class AudioBitDepth : public AudioStream
{
public:
	AudioBitDepth(void) : AudioStream(1, inputQueueArray) {}
	virtual void update(void);
	void setBitDepth(uint8_t val);
private:
	uint8_t bitDepth = 16;
	uint16_t bitMask = 0;
	audio_block_t *inputQueueArray[1];
};



#endif /* LIBS_AUDIO_EFFECT_BITDEPTH_H_ */
