

#include <Arduino.h>
#include "play_wav_from_memory.h"
#include "spi_interrupt.h"

#define STATE_PLAY		1
#define STATE_STOP	 	0

void AudioPlayWavFromMemory::begin(void)
{
	state = STATE_STOP;
	data_length = 0;
}


bool AudioPlayWavFromMemory::play(int16_t * buf, uint32_t bufLen)
{
	stop();

	if (!buf) return false;

	memoryBuf = buf;

	data_length = bufLen;
	dataCnt=0;

	state = STATE_PLAY;

	return true;
}

void AudioPlayWavFromMemory::stop(void)
{
	__disable_irq();
	if (state != STATE_STOP)
	{
		audio_block_t *b1 = block;
		block = NULL;

		state = STATE_STOP;

		__enable_irq();
		if (b1) release(b1);
	}
	else
	{
		__enable_irq();
	}
}


void AudioPlayWavFromMemory::update(void)
{

	// only update if we're playing
	if (state == STATE_STOP) return;

	// allocate the audio blocks to transmit



	if(dataCnt >= data_length )
		{
		state = STATE_STOP;
		return;
		}

	block = allocate();
	if (block == NULL) return;

	if (block)
	{
		for (uint32_t i=0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			block->data[i] = *memoryBuf;
			memoryBuf++;
		}
		dataCnt+=AUDIO_BLOCK_SAMPLES;
		transmit(block, 0);

		release(block);
		block = NULL;
	}
}





bool AudioPlayWavFromMemory::isPlaying(void)
{

	return state;
}

float AudioPlayWavFromMemory::positionMillis(void)
{
	float result=0;

	if(!state) return 0;

	result = dataCnt/44100.0;

	return result;
}


float AudioPlayWavFromMemory::lengthMillis(void)
{
	float result=0;

	if(!state) return 0;

	result=data_length/44100.0;

	return result;
}






