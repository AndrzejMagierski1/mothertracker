/* Audio Library for Teensy 3.X
 * Copyright (c) 2017, Paul Stoffregen, paul@pjrc.com
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

#include "effect_envelope.h"

#define STATE_IDLE	0
#define STATE_DELAY	1
#define STATE_ATTACK	2
#define STATE_HOLD	3
#define STATE_DECAY	4
#define STATE_SUSTAIN	5
#define STATE_RELEASE	6
#define STATE_FORCED	7

void AudioEffectEnvelope::noteOn(void)
{
	__disable_irq();
	pressedFlag = 1;
	if (state == STATE_IDLE || state == STATE_DELAY || release_forced_count == 0) {
		mult_hires = 0;
		count = delay_count;
		if (count > 0) {
			state = STATE_DELAY;
			inc_hires = 0;
		} else {
			state = STATE_ATTACK;
			count = attack_count;
			inc_hires = 0x40000000 / (int32_t)count;
		}
	} else if (state != STATE_FORCED) {
		state = STATE_FORCED;
		count = release_forced_count;
		inc_hires = (-mult_hires) / (int32_t)count;
	}
	__enable_irq();
}

void AudioEffectEnvelope::noteOff(void)
{
	pressedFlag = 0;
	__disable_irq();
	if (state != STATE_IDLE && state != STATE_FORCED) {
		if(loopFlag)
		{
			state = STATE_IDLE; //todo: envelope jako lfo - tracker edit
			__enable_irq();
			return;
		}
		state = STATE_RELEASE;
		count = release_count;
		inc_hires = (-mult_hires) / (int32_t)count;
	}
	__enable_irq();
}

void AudioEffectEnvelope::setIdle(void)
{
	__disable_irq();
	pressedFlag = 0;
	state = STATE_IDLE;
	__enable_irq();
}

void AudioEffectEnvelope::update(void)
{
	audio_block_t *block;
	uint32_t *p, *end;
	block = receiveWritable();
	if (!block) return;

	if (state == STATE_IDLE) {
		AudioStream::release(block);
		return;
	}

	if(passFlag)
	{
		transmit(block);
		AudioStream::release(block);
		return;
	}

	p = (uint32_t *)(block->data);
	end = p + AUDIO_BLOCK_SAMPLES/2;

	while (p < end)
	{
		// we only care about the state when completing a region
		if (count == 0)
		{
			if (state == STATE_ATTACK)
			{
				count = hold_count;
				state = STATE_HOLD;
				mult_hires = 0x40000000;
				inc_hires = 0;
				continue;
			}
			else if (state == STATE_HOLD)
			{
				state = STATE_DECAY;
				count = decay_count;
				inc_hires = count ? (sustain_mult - 0x40000000) / (int32_t)count : 0 ;
				continue;
			}
			else if (state == STATE_DECAY)
			{
				if(loopFlag && pressedFlag)
				{
					count = release_count;
					state = STATE_RELEASE;
					inc_hires = count ? (-mult_hires) / (int32_t)count : 0;
					continue;
				}
				else
				{
					state = STATE_SUSTAIN;
					count = 0xFFFF;
					mult_hires = sustain_mult;
					inc_hires = 0;
				}
			}
			else if (state == STATE_SUSTAIN)
			{
				count = 0xFFFF;
			}
			else if (state == STATE_RELEASE)
			{
				if(loopFlag && pressedFlag)
				{
					//powinienem dać delay state, ale nie jest uzywany w trackerze, a moze tworzyc dodatkowe problemy
					count = attack_count;
					state = STATE_ATTACK;
					inc_hires = count ? 0x40000000 / (int32_t)count : 0;
					continue;
				}
				else
				{
					state = STATE_IDLE;
					endReleaseFlag=1;
					while (p < end)
					{

						*p++ = 0;
						*p++ = 0;
						*p++ = 0;
						*p++ = 0;
					}
					break;
				}
			}
			else if (state == STATE_FORCED)
			{
				mult_hires = 0;
				count = delay_count;
				state = STATE_DELAY;
				inc_hires = 0;
				continue;

			}
			else if (state == STATE_DELAY)
			{
				state = STATE_ATTACK;
				count = attack_count;
				inc_hires = count ?  0x40000000 / count : 0;
				continue;
			}
		}

		int32_t mult = mult_hires >> 14; // przeskalowanie na 16 bitow bo 0x40000000 zajmuje 30 bitow
		int32_t inc = inc_hires >> 17; // podejrzewam ze przeskalowanie na 13 bitow aby suma takich osmiu (3bity) obslugiwala 8 probek (inc_hires wyliczone dla 8 bitowego cyklu)
//		sample12 = *p++;
//		sample34 = *p++;
//		sample56 = *p++;
//		sample78 = *p++;
//		p -= 4;
//		mult += inc;
//		tmp1 = signed_multiply_32x16b(mult, sample12);
//		mult += inc;
//		tmp2 = signed_multiply_32x16t(mult, sample12);
//		sample12 = pack_16b_16b(tmp2, tmp1);
//		mult += inc;
//		tmp1 = signed_multiply_32x16b(mult, sample34);
//		mult += inc;
//		tmp2 = signed_multiply_32x16t(mult, sample34);
//		sample34 = pack_16b_16b(tmp2, tmp1);
//		mult += inc;
//		tmp1 = signed_multiply_32x16b(mult, sample56);
//		mult += inc;
//		tmp2 = signed_multiply_32x16t(mult, sample56);
//		sample56 = pack_16b_16b(tmp2, tmp1);
//		mult += inc;
//		tmp1 = signed_multiply_32x16b(mult, sample78);
//		mult += inc;
//		tmp2 = signed_multiply_32x16t(mult, sample78);
//		sample78 = pack_16b_16b(tmp2, tmp1);

		sample12 = *p;
		sample34 = *(p+1);
		sample56 = *(p+2);
		sample78 = *(p+3);

		mult+=inc;
		tmp1 = mult * (*sample1);
		mult+=inc;
		tmp2 = mult * (*sample2);

		*sample1 = *tmp1Shifted;
		*sample2 = *tmp2Shifted;

		mult+=inc;
		tmp1 = mult * (*sample3);
		mult+=inc;
		tmp2 = mult * (*sample4);

		*sample3 = *tmp1Shifted;
		*sample4 = *tmp2Shifted;

		mult+=inc;
		tmp1 = mult * (*sample5);
		mult+=inc;
		tmp2 = mult * (*sample6);

		*sample5 = *tmp1Shifted;
		*sample6 = *tmp2Shifted;

		mult+=inc;
		tmp1 = mult * (*sample7);
		mult+=inc;
		tmp2 = mult * (*sample8);

		*sample7 = *tmp1Shifted;
		*sample8 = *tmp2Shifted;

		*p++ = sample12;
		*p++ = sample34;
		*p++ = sample56;
		*p++ = sample78;


		// adjust the long-term gain using 30 bit resolution (fix #102)
		// https://github.com/PaulStoffregen/Audio/issues/102
		mult_hires += inc_hires;
		count--;
	}
	transmit(block);
	AudioStream::release(block);
}

uint8_t AudioEffectEnvelope::endRelease()
{
	if(endReleaseFlag==1) return 1;
	else return 0;
	}

void AudioEffectEnvelope::clearEndReleaseFlag()
{
	endReleaseFlag = 0;
}

uint8_t AudioEffectEnvelope:: getState()
{
	return state;
}
