#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playLoopPingpong(uint8_t instrIdx, int8_t note)
{
	playLoopForward(instrIdx, note);
}


