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
	if (state == STATE_IDLE || state == STATE_DELAY || release_forced_count == 0)
	{
		startVol = 0.0f;

		count = delay_count;

		if (count > 0)
		{
			state = STATE_DELAY;
			incVol = 0.0f;
		}
		else
		{
			state = STATE_ATTACK;
			count = attack_count;
			incVol = count ? 1.0f / count : 0;
		}
	}
	else if (state != STATE_FORCED)
	{
		state = STATE_FORCED;
		count = release_forced_count;
		incVol = count ? (-startVol) / count : 0;
	}
	__enable_irq();
}

void AudioEffectEnvelope::noteOff(void)
{
	__disable_irq();

	pressedFlag = 0;

	if (state != STATE_IDLE && state != STATE_FORCED)
	{
		if(loopFlag)
		{
			state = STATE_IDLE; //todo: envelope jako lfo - tracker edit
			__enable_irq();
			return;
		}
		state = STATE_RELEASE;
		count = release_count;
		incVol = (-startVol) / count;
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
	int16_t *p, *end;
	elapsedMicros apoloniuszTajmer = 0;

	block = receiveWritable();
	if (!block) return;

	if (state == STATE_IDLE)
	{
		AudioStream::release(block);
		return;
	}

	if(passFlag)
	{
		transmit(block);
		AudioStream::release(block);
		return;
	}

	p = block->data;
	end = p + AUDIO_BLOCK_SAMPLES;

	while (p < end)
	{
		// we only care about the state when completing a region
		if (count == 0)
		{
			if (state == STATE_ATTACK)
			{
				count = hold_count;
				state = STATE_HOLD;
				startVol = 1.0f;
				incVol = 0.0f;
				continue;
			}
			else if (state == STATE_HOLD)
			{
				state = STATE_DECAY;
				count = decay_count;
				incVol = count ? (sustain_vol - 1.0) / count : 0 ;
				continue;
			}
			else if (state == STATE_DECAY)
			{
				if(loopFlag && pressedFlag)
				{
					count = release_count;
					state = STATE_RELEASE;
					incVol = count ? (-startVol) / count : 0;
					continue;
				}
				else
				{
					state = STATE_SUSTAIN;
					count = 0xFFFFFFFF;
					startVol = sustain_vol;
					incVol = 0;
				}
			}
			else if (state == STATE_SUSTAIN)
			{
				count = 0xFFFFFFFF;
			}
			else if (state == STATE_RELEASE)
			{
				if(loopFlag && pressedFlag)
				{
					//powinienem dać delay state, ale nie jest uzywany w trackerze, a moze tworzyc dodatkowe problemy
					count = attack_count;
					state = STATE_ATTACK;
					incVol = count ? 1.0 / count : 0;
					continue;
				}
				else
				{
					state = STATE_IDLE;
					endReleaseFlag=1;
					while (p < end)
					{
						*p++ = 0;
					}
					break;
				}
			}
			else if (state == STATE_FORCED)
			{
				startVol = 0;
				count = delay_count;
				state = STATE_DELAY;
				incVol = 0;
				continue;

			}
			else if (state == STATE_DELAY)
			{
				state = STATE_ATTACK;
				count = attack_count;
				startVol = count ?  1.0f / count : 0;
				continue;
			}
		}

		*(p++) *= startVol;

		startVol += incVol;
		count--;
	}

	Serial.print("AT: "); Serial.println(apoloniuszTajmer);
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
