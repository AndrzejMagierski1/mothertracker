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

#include "play_memory.h"
#include "utility/dspinst.h"



//void AudioPlayMemory::play(int16_t *data, uint32_t len,uint32_t startPoint,uint32_t endPoint, uint32_t loopPoint1, uint32_t loopPoint2)
void AudioPlayMemory::play(strStep * step)
{
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;

	playing = 0;
	prior = 0;

	stopLoop=0;
	int16_t * data = mtProject.sampleBank.sample[mtProject.instrument[step->instrumentIndex].sampleIndex].address;


	startBuf=(uint32_t)*data;



	startPoint=mtProject.instrument[step->instrumentIndex].startPoint;
	endPoint=mtProject.instrument[step->instrumentIndex].endPoint;
	loopPoint1=mtProject.instrument[step->instrumentIndex].loopPoint1;
	loopPoint2=mtProject.instrument[step->instrumentIndex].loopPoint2;

	if ( (startPoint >= endPoint) || (startPoint > loopPoint1) || (startPoint > loopPoint2) ) return; //startpoint
	if ((loopPoint1 > loopPoint2) || (loopPoint1 > endPoint)) return; //looppoint1
	if (loopPoint2 > endPoint) return; // looppoint2



	samplePoints.start= (uint32_t)(startPoint*44.1);
	samplePoints.end= (uint32_t)(endPoint*44.1);
	samplePoints.loop1= (uint32_t)(loopPoint1*44.1);
	samplePoints.loop2= (uint32_t)(loopPoint2*44.1);


	if((samplePoints.start >= startBuf) || (samplePoints.loop1>startBuf) || (samplePoints.loop2>startBuf) || (samplePoints.end>startBuf)) return; // wskazniki za plikiem


	sampleConstrains.loopPoint1=startBuf-samplePoints.loop1;
	sampleConstrains.loopPoint2=startBuf-samplePoints.loop2;
	sampleConstrains.endPoint= startBuf-samplePoints.end;
	sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;


	data+=2;

	next = data+42+samplePoints.start;
	beginning = data+samplePoints.start;
	length =startBuf-samplePoints.start;//format & 0xFFFFFF;

	playing = 0x81;//format >> 24;

}

void AudioPlayMemory::stop(void)
{
	playing = 0;
}

extern "C" {
extern const int16_t ulaw_decode_table[256];
};

void AudioPlayMemory::update(void)
{
	audio_block_t *block;
	int16_t *in;
	int16_t *out;
	uint32_t tmp32, consumed;
	int16_t s0, s1, s2, s3, s4;
	int i;

	if (!playing) return;
	block = allocate();
	if (block == NULL) return;

	//Serial.write('.');

	out = block->data;
	in = next;
	s0 = prior;

	switch (playing) {
	  case 0x01: // u-law encoded, 44100 Hz
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i += 4) {
			tmp32 = *in++;
			*out++ = ulaw_decode_table[(tmp32 >> 0) & 255];
			*out++ = ulaw_decode_table[(tmp32 >> 8) & 255];
			*out++ = ulaw_decode_table[(tmp32 >> 16) & 255];
			*out++ = ulaw_decode_table[(tmp32 >> 24) & 255];
		}
		consumed = AUDIO_BLOCK_SAMPLES;
		break;

	  case 0x81: // 16 bit PCM, 44100 Hz

		for (i=0; i < AUDIO_BLOCK_SAMPLES; i ++)
		{
			//tmp32 = *in++;
			//*out++ = (int16_t)(tmp32 & 65535);
			//*out++ = (int16_t)(tmp32 >> 16);
			*out++ = *in++;
			if (length > 0)
			{
				length --;
				if((length <= sampleConstrains.loopPoint2) && (!stopLoop) ) {length = sampleConstrains.loopPoint1;	 in -= sampleConstrains.loopLength;}
				if(length <= sampleConstrains.endPoint) length=0;
			}
			else
			{
				playing = 0;
			}
		}
		//consumed = AUDIO_BLOCK_SAMPLES;
		break;

	  case 0x02: // u-law encoded, 22050 Hz 
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i += 8) {
			tmp32 = *in++;
			s1 = ulaw_decode_table[(tmp32 >> 0) & 255];
			s2 = ulaw_decode_table[(tmp32 >> 8) & 255];
			s3 = ulaw_decode_table[(tmp32 >> 16) & 255];
			s4 = ulaw_decode_table[(tmp32 >> 24) & 255];
			*out++ = (s0 + s1) >> 1;
			*out++ = s1;
			*out++ = (s1 + s2) >> 1;
			*out++ = s2;
			*out++ = (s2 + s3) >> 1;
			*out++ = s3;
			*out++ = (s3 + s4) >> 1;
			*out++ = s4;
			s0 = s4;
		}
		consumed = AUDIO_BLOCK_SAMPLES/2;
		break;

	  case 0x82: // 16 bits PCM, 22050 Hz
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i += 4) {
			tmp32 = *in++;
			s1 = (int16_t)(tmp32 & 65535);
			s2 = (int16_t)(tmp32 >> 16);
			*out++ = (s0 + s1) >> 1;
			*out++ = s1;
			*out++ = (s1 + s2) >> 1;
			*out++ = s2;
			s0 = s2;
		}
		consumed = AUDIO_BLOCK_SAMPLES/2;
		break;

	  case 0x03: // u-law encoded, 11025 Hz
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i += 16) {
			tmp32 = *in++;
			s1 = ulaw_decode_table[(tmp32 >> 0) & 255];
			s2 = ulaw_decode_table[(tmp32 >> 8) & 255];
			s3 = ulaw_decode_table[(tmp32 >> 16) & 255];
			s4 = ulaw_decode_table[(tmp32 >> 24) & 255];
			*out++ = (s0 * 3 + s1) >> 2;
			*out++ = (s0 + s1)     >> 1;
			*out++ = (s0 + s1 * 3) >> 2;
			*out++ = s1;
			*out++ = (s1 * 3 + s2) >> 2;
			*out++ = (s1 + s2)     >> 1;
			*out++ = (s1 + s2 * 3) >> 2;
			*out++ = s2;
			*out++ = (s2 * 3 + s3) >> 2;
			*out++ = (s2 + s3)     >> 1;
			*out++ = (s2 + s3 * 3) >> 2;
			*out++ = s3;
			*out++ = (s3 * 3 + s4) >> 2;
			*out++ = (s3 + s4)     >> 1;
			*out++ = (s3 + s4 * 3) >> 2;
			*out++ = s4;
			s0 = s4;
		}
		consumed = AUDIO_BLOCK_SAMPLES/4;
		break;

	  case 0x83: // 16 bit PCM, 11025 Hz
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i += 8) {
			tmp32 = *in++;
			s1 = (int16_t)(tmp32 & 65535);
			s2 = (int16_t)(tmp32 >> 16);
			*out++ = (s0 * 3 + s1) >> 2;
			*out++ = (s0 + s1)     >> 1;
			*out++ = (s0 + s1 * 3) >> 2;
			*out++ = s1;
			*out++ = (s1 * 3 + s2) >> 2;
			*out++ = (s1 + s2)     >> 1;
			*out++ = (s1 + s2 * 3) >> 2;
			*out++ = s2;
			s0 = s2;
		}
		consumed = AUDIO_BLOCK_SAMPLES/4;
		break;

	  default:
		release(block);
		playing = 0;
		return;
	}
	prior = s0;
	next = in;
	transmit(block);
	release(block);
}


#define B2M_88200 (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0)
#define B2M_44100 (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT) // 97352592
#define B2M_22050 (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT * 2.0)
#define B2M_11025 (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT * 4.0)


uint32_t AudioPlayMemory::positionMillis(void)
{
	uint8_t p;
	const uint8_t *n, *b;
	uint32_t b2m;

	__disable_irq();
	p = playing;
	n = (const uint8_t *)next;
	b = (const uint8_t *)beginning;
	__enable_irq();
	switch (p) {
	  case 0x81: // 16 bit PCM, 44100 Hz
		b2m = B2M_88200;  break;
	  case 0x01: // u-law encoded, 44100 Hz
	  case 0x82: // 16 bits PCM, 22050 Hz
		b2m = B2M_44100;  break;
	  case 0x02: // u-law encoded, 22050 Hz
	  case 0x83: // 16 bit PCM, 11025 Hz
		b2m = B2M_22050;  break;
	  case 0x03: // u-law encoded, 11025 Hz
		b2m = B2M_11025;  break;
	  default:
		return 0;
	}
	if (p == 0) return 0;
	return ((uint64_t)(n - b) * b2m) >> 32;
}

uint32_t AudioPlayMemory::lengthMillis(void)
{
	uint8_t p;
	const uint32_t *b;
	uint32_t b2m;

	__disable_irq();
	p = playing;
	b = (const uint32_t *)beginning;
	__enable_irq();
	switch (p) {
	  case 0x81: // 16 bit PCM, 44100 Hz
	  case 0x01: // u-law encoded, 44100 Hz
		b2m = B2M_44100;  break;
	  case 0x82: // 16 bits PCM, 22050 Hz
	  case 0x02: // u-law encoded, 22050 Hz
		b2m = B2M_22050;  break;
	  case 0x83: // 16 bit PCM, 11025 Hz
	  case 0x03: // u-law encoded, 11025 Hz
		b2m = B2M_11025;  break;
	  default:
		return 0;
	}
	return ((uint64_t)(*(b - 1) & 0xFFFFFF) * b2m) >> 32;
}
void AudioPlayMemory::stopLoopMode(void)
{
	stopLoop=1;
}

void AudioPlayMemory::setStartPoint(uint32_t sp)
{
	samplePoints.start= (uint32_t)(sp*44.1);

}
void AudioPlayMemory::setEndPoint(uint32_t ep)
{
	samplePoints.end= (uint32_t)(ep*44.1);
	sampleConstrains.endPoint= startBuf-samplePoints.end;

}
void AudioPlayMemory::setLoopPoint1(uint32_t lp1)
{
	samplePoints.loop1= (uint32_t)(lp1*44.1);
	sampleConstrains.loopPoint1=startBuf-samplePoints.loop1;
	sampleConstrains.loopLength=sampleConstrains.loopPoint2-sampleConstrains.loopPoint1;

}
void AudioPlayMemory::setLoopPoint2(uint32_t lp2)
{
	samplePoints.loop2= (uint32_t)(lp2*44.1);
	sampleConstrains.loopPoint2=startBuf-samplePoints.loop2;
	sampleConstrains.loopLength=sampleConstrains.loopPoint2-sampleConstrains.loopPoint1;
}


