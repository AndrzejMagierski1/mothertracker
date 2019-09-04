#include "play_sd_wav_float.h"

/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "play_sd_wav.h"
#include "spi_interrupt.h"

constexpr uint8_t STATE_STOP = 0;
constexpr uint8_t STATE_PLAY = 1;

void AudioPlaySdWavFloat::begin(void)
{
	state = STATE_STOP;
	if (block)
	{
		release(block);
		block = NULL;
	}
}

bool AudioPlaySdWavFloat::play(const char *filename)
{
	stop();
#if defined(HAS_KINETIS_SDHC)
	if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStartUsingSPI();
#else
	AudioStartUsingSPI();
#endif
	__disable_irq()
	;
	currentPosition = 0;
	wavfile = SD.open(filename);
	__enable_irq()
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

	if (wavHeader.chunkId == 0x46464952 && wavHeader.format == 0x45564157 && wavHeader.subchunk1Id == 0x20746D66 && wavHeader.AudioFormat == 3)
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

void AudioPlaySdWavFloat::stop(void)
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

void AudioPlaySdWavFloat::update(void)
{

	if (state == STATE_STOP) return;

	block = allocate();
	if (block == NULL) return;

	if (wavfile.available() && (currentPosition < wavHeader.subchunk2Size) )
	{
		if (wavHeader.numChannels == 1)
		{
			uint32_t localLength = wavfile.read(buffer, 512);
			buffer_length = localLength / 4;
			currentPosition += localLength;

			if( wavHeader.subchunk2Size < currentPosition  ) buffer_length -= (  currentPosition - wavHeader.subchunk2Size) /4;

//			if( (wavHeader.subchunk2Size - currentPosition) < 512 ) buffer_length = (wavHeader.subchunk2Size - currentPosition) /4;
		}
		else if (wavHeader.numChannels == 2)
		{
			uint32_t localLength = wavfile.read(buffer, 1024);
			buffer_length = localLength / 8;
			currentPosition += localLength;
			if( wavHeader.subchunk2Size < currentPosition  ) buffer_length -= (  currentPosition - wavHeader.subchunk2Size) /8;
//			if( (wavHeader.subchunk2Size - currentPosition) < 1024 ) buffer_length = (wavHeader.subchunk2Size - currentPosition) /8;
		}
	}
	else
	{
		stop();
		return;
	}

	float * p = buffer;

	for (uint8_t i = 0; i < 128; i++)
	{
		if(i >= buffer_length) block->data[i] = 0;
		else
		{
			block->data[i] = (((*(p++) + 1.0) * 65535.0) / 2.0) - 32768.0;
			if (wavHeader.numChannels == 2) p++;
		}
	}
	if (block)
	{
		transmit(block, 0);
		release(block);
		block = NULL;
	}

}





















