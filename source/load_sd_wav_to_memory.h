
#ifndef load_sd_wav__to_memory_h_
#define load_sd_wav__to_memory_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"

class AudioLoadSdWav
{
public:
	uint32_t loadSdWavToMemory(const char *filename, int16_t * buf);
};

#endif
