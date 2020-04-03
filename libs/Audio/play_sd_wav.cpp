#include "play_sd_wav.h"
#include "spi_interrupt.h"

constexpr uint8_t STATE_STOP = 0;
constexpr uint8_t STATE_PLAY = 1;

void AudioPlaySdWav::begin(void)
{
	state = STATE_STOP;
	if (block)
	{
		release(block);
		block = NULL;
	}
}

bool AudioPlaySdWav::play(const char *filename)
{
	stop();
#if defined(HAS_KINETIS_SDHC)
	if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStartUsingSPI();
#else
	AudioStartUsingSPI();
#endif
	//__disable_irq()
	;
	currentPosition = 0;
	wavfile = SD.open(filename);
	//__enable_irq()
	;
	if (!wavfile)
	{
#if defined(HAS_KINETIS_SDHC)
		if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStopUsingSPI();
#else
		AudioStopUsingSPI();
#endif
		return false;
	}
	readHeader(&wavHeader, &wavfile);

	if (wavHeader.chunkId == 0x46464952 && wavHeader.format == 0x45564157 && wavHeader.subchunk1Id == 0x20746D66 && wavHeader.AudioFormat == 1 && wavHeader.bitsPerSample == 16)
	{
		state = STATE_PLAY;
		return true;
	}
	else
	{
		wavfile.close();
		return false;
	}
}

void AudioPlaySdWav::stop(void)
{
	__disable_irq()
	;
	if (state == STATE_PLAY)
	{
		if(block)
		{
			release(block);
			block = NULL;
		}
		state = STATE_STOP;
		wavfile.close();
		__enable_irq()
		;

#if defined(HAS_KINETIS_SDHC)
		if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStopUsingSPI();
#else
		AudioStopUsingSPI();
#endif
	}
	else
	{
		__enable_irq()
		;
	}
}

void AudioPlaySdWav::update(void)
{

	if (state == STATE_STOP) return;

	block = allocate();
	if (block == NULL) return;

	if (wavfile.available() && (currentPosition < wavHeader.subchunk2Size) )
	{
		if (wavHeader.numChannels == 1)
		{
			uint32_t localLength = wavfile.read(buffer, 256);
			buffer_length = localLength / 2;
			currentPosition += localLength;

			if( wavHeader.subchunk2Size < currentPosition  ) buffer_length -= (  currentPosition - wavHeader.subchunk2Size) /2;

		}
		else if (wavHeader.numChannels == 2)
		{
			uint32_t localLength = wavfile.read(buffer, 512);
			buffer_length = localLength / 4;
			currentPosition += localLength;
			if( wavHeader.subchunk2Size < currentPosition ) buffer_length -= (currentPosition - wavHeader.subchunk2Size) /4;

		}
	}
	else
	{
		stop();
		return;
	}

	uint8_t * p = buffer;

	for (uint8_t i = 0; i < 128; i++)
	{
		if(i >= buffer_length)
		{
			block->data[i] = 0;
		}
		else
		{
			block->data[i] = *((int16_t *) p);
			if(i != buffer_length-1) p+= 2*wavHeader.numChannels;
		}
	}
	if (block)
	{
		transmit(block, 0);
		release(block);
		block = NULL;
	}

}






