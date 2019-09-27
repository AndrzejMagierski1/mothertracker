#include "effect_bitDepth.h"

void AudioBitDepth::update(void)
{
	audio_block_t *in;

	in = receiveWritable(0);

	if(!in) return;

	if(bitDepth != 16)
	{
		for(uint8_t i = 0 ; i <AUDIO_BLOCK_SAMPLES; i++)
		{
			in->data[i] &= bitMask;
		}
	}

	transmit(in);
	release(in);
}

void AudioBitDepth::setBitDepth(uint8_t val)
{
	if(val < 4) bitDepth = 4;
	else if(val > 16) bitDepth = 16;
	else bitDepth = val;
	bitMask = 0;
	uint8_t maskSize = 16-val;
	uint16_t maskownica = 0; // :)
	for(uint8_t i = 0; i<maskSize; i++)
	{
		maskownica = 1<<i;
		bitMask |= maskownica;
	}
	bitMask = (~bitMask);

}
