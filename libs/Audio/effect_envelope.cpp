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

#include "Audio.h"
#include "debugLog.h"
#include "mtSequencer.h"
#include "play_memory.h"


#define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT/1000.0)

void AudioPlayMemory::envelopeSetSustain()
{
	envelope.state = 5;
	envelope.count = 0xFFFF;
	envelope.mult_hires = envelope.sustain_mult;
	envelope.inc_hires = 0;
}

void AudioPlayMemory::envelopeReleaseNoteOn(float milliseconds)
{
	envelope.release_forced_count = envelopeMilliseconds2count(milliseconds);
	if (envelope.release_forced_count == 0) envelope.release_forced_count = 1;
//		release_forced_count = 16;
}
void AudioPlayMemory::envelopeSetLoop(uint8_t state)
{
	envelope.loopFlag = state;
}

void AudioPlayMemory::envelopeSetPassFlag(uint8_t state)
{
	envelope.passFlag = state;
}

void AudioPlayMemory::envelopeSetIsRandom(bool value)
{
	envelope.isRandom = value;
}


void AudioPlayMemory::envelopeDelay(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastDelayCount = envelope.delay_count;
	envelope.delay_count = envelopeMilliseconds2count(milliseconds);
	if (envelope.delay_count == 0) envelope.delay_count = 1;

	if(envelope.delay_count == lastDelayCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(envelope.state == envelopePhaseDelay)
	{
		int32_t dif = envelope.delay_count - lastDelayCount;

		if((int)(envelope.count + dif) < 1 ) envelope.count = 1;
		else if ((int)(envelope.count + dif) > 65535) envelope.count = 65535;
		else envelope.count += dif;
	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioPlayMemory::envelopeAttack(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();

	uint16_t lastAttackCount = envelope.attack_count;

	envelope.attack_count = envelopeMilliseconds2count(milliseconds);
	if (envelope.attack_count == 0) envelope.attack_count = 1;

	if(envelope.attack_count == lastAttackCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(envelope.state == envelopePhaseAttack)
	{
		int32_t dif = envelope.attack_count - lastAttackCount;

		if((int)(envelope.count + dif) < 1 ) envelope.count = 1;
		else if ((int)(envelope.count + dif) > 65535) envelope.count = 65535;
		else envelope.count += dif;

		int32_t y = 0x3FFFFFFF - envelope.mult_hires;
		int32_t x = envelope.count;

		if(x < 0) x = 0;

		envelope.inc_hires = x ?  y /x : y;

	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioPlayMemory::envelopeHold(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastHoldCount = envelope.hold_count;
	envelope.hold_count = envelopeMilliseconds2count(milliseconds);
	if (envelope.hold_count == 0) envelope.hold_count = 1;
	if(envelope.hold_count == lastHoldCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}

	if(envelope.state == envelopePhaseHold)
	{
		int32_t dif = envelope.hold_count - lastHoldCount;

		if((int)(envelope.count + dif) < 1 ) envelope.count = 1;
		else if ((int)(envelope.count + dif) > 65535) envelope.count = 65535;
		else envelope.count += dif;

	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioPlayMemory::envelopeDecay(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastDecayCount = envelope.decay_count;
	envelope.decay_count = envelopeMilliseconds2count(milliseconds);
	if (envelope.decay_count == 0) envelope.decay_count = 1;

	if(envelope.decay_count == lastDecayCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(envelope.state == envelopePhaseDecay)
	{
		int32_t dif = envelope.decay_count - lastDecayCount;

		if((int)(envelope.count + dif) < 1 ) envelope.count = 1;
		else if ((int)(envelope.count + dif) > 65535) envelope.count = 65535;
		else envelope.count += dif;

		envelope.inc_hires = envelope.count ? (envelope.sustain_mult - envelope.mult_hires) / envelope.count : (envelope.sustain_mult - envelope.mult_hires) ;

	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioPlayMemory::envelopeSustain(float level)
{
	__disable_irq();
	AudioNoInterrupts();
	if (level < 0.0) level = 0;
	else if (level > 1.0) level = 1.0;
	envelope.sustain_mult = level * 1073741823.0;
	if(envelope.state == envelopePhaseSustain) envelope.mult_hires = envelope.sustain_mult;
	AudioInterrupts();
	__enable_irq();
}
void  AudioPlayMemory::envelopeRelease(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastReleaseCount = envelope.release_count;
	envelope.release_count = envelopeMilliseconds2count(milliseconds);
	if (envelope.release_count == 0) envelope.release_count = 1;

	if(envelope.release_count == lastReleaseCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(envelope.state == envelopePhaseRelease)
	{
		int32_t dif = envelope.release_count - lastReleaseCount;

		if( (int)(envelope.count + dif) < 1 ) envelope.count = 1;
		else if ((int)(envelope.count + dif) > 65535) envelope.count = 65535;
		else envelope.count += dif;

		envelope.inc_hires = envelope.count ? (-envelope.mult_hires) / envelope.count : -envelope.mult_hires;

	}
	AudioInterrupts();
	__enable_irq();
}


void AudioPlayMemory::envelopeNoteOn(void)
{
	__disable_irq();

	envelope.pressedFlag = 1;

	if(envelope.passFlag)
	{
		envelopeSwitchPhase(envelopePhaseIdle);
	}
	else if (envelope.state == envelopePhaseIdle || envelope.state == envelopePhaseDelay)
	{
		envelopeSwitchPhase(envelopePhaseDelay);
	}
	else if (envelope.state != envelopePhaseForced)
	{
		envelopeSwitchPhase(envelopePhaseForced);
	}
	else envelope.endKillReleaseFlag = 1;
	// nie ma dla force bo i tak wyliczy taka sama prosta wygaszania jak byla
	__enable_irq();
}

void AudioPlayMemory::envelopeNoteOff(void)
{
	envelope.pressedFlag = 0;
	if (envelope.state != envelopePhaseIdle)
	{
		if(envelope.loopFlag) envelopeSwitchPhase(envelopePhaseIdle);
		else
		{
			envelopeSwitchPhase(envelopePhaseRelease);
		}

	}
	__enable_irq();
}

void AudioPlayMemory::envelopeSetIdle(void)
{
	__disable_irq();
	envelope.pressedFlag = 0;
	envelopeSwitchPhase(envelopePhaseIdle);
	__enable_irq();
}

void AudioPlayMemory::envelopeUpdate(void)
{
	__disable_irq();
	audio_block_t *block;
	uint32_t *p, *end;

	block = receiveWritable();
	if (!block)
	{
		block = allocate();
		if(!block) return;
		else memset(block->data,0,AUDIO_BLOCK_SAMPLES * 2);
	}

	if(envelope.passFlag)
	{
		transmit(block);
		AudioStream::release(block);
		return;
	}
	if (envelope.state == envelopePhaseIdle)
	{
		memset(block->data,0,AUDIO_BLOCK_SAMPLES * 2);
		transmit(block);
		AudioStream::release(block);
		return;
	}
	p = (uint32_t *)(block->data);
	end = p + AUDIO_BLOCK_SAMPLES/2;



	while (p < end)
	{
		// we only care about the state when completing a region
		if (envelope.state == envelopePhaseSustain)
		{
			if(envelope.loopFlag && envelope.pressedFlag)
			{
				envelopeSwitchPhase(envelopePhaseRelease);
			}
		}

		if (envelope.count == 0)
		{
			if (envelope.state == envelopePhaseAttack)
			{
				envelopeSwitchPhase(envelopePhaseHold);
				if(envelope.isRandom)
				{
					randomSeed(micros());
					envelope.mult_hires = random(0,0x3FFFFFFF);
				}
				continue;
			}
			else if (envelope.state == envelopePhaseHold)
			{
				if(envelope.isRandom)
				{
					envelopeSwitchPhase(envelopePhaseHold);
					randomSeed(micros());
					envelope.mult_hires = random(0,0x3FFFFFFF);
				}
				else envelopeSwitchPhase(envelopePhaseDecay);
				continue;
			}
			else if (envelope.state == envelopePhaseDecay)
			{
				if(envelope.loopFlag && envelope.pressedFlag)
				{
					envelopeSwitchPhase(envelopePhaseRelease);
					continue;
				}
				else
				{
					envelopeSwitchPhase(envelopePhaseSustain);
				}
			}
			else if (envelope.state == envelopePhaseSustain)
			{
				envelopeSwitchPhase(envelopePhaseSustain);
			}
			else if (envelope.state == envelopePhaseRelease)
			{
				if(envelope.loopFlag && envelope.pressedFlag)
				{
					envelopeSwitchPhase(envelopePhaseDelay);
					continue;
				}
				else
				{
					envelopeSwitchPhase(envelopePhaseIdle);
					envelope.endReleaseFlag=1;
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
			else if (envelope.state == envelopePhaseForced)
			{
				envelopeSwitchPhase(envelopePhaseDelay);
				envelope.endKillReleaseFlag = 1;
				continue;

			}
			else if (envelope.state == envelopePhaseDelay)
			{
				envelopeSwitchPhase(envelopePhaseAttack);
				continue;
			}
		}

		int32_t mult = envelope.mult_hires >> 14; // przeskalowanie na 16 bitow bo 0x3FFFFFFF zajmuje 30 bitow
		int32_t inc = envelope.inc_hires >> 17; // podejrzewam ze przeskalowanie na 13 bitow aby suma takich osmiu (3bity) obslugiwala 8 probek (inc_hires wyliczone dla 8 bitowego cyklu)

		envelope.sample12 = *p;
		envelope.sample34 = *(p+1);
		envelope.sample56 = *(p+2);
		envelope.sample78 = *(p+3);
//		p -= 4;
		mult += inc;
		envelope.tmp1 = signed_multiply_32x16b(mult, envelope.sample12);
		mult += inc;
		envelope.tmp2 = signed_multiply_32x16t(mult, envelope.sample12);
		envelope.sample12 = pack_16b_16b(envelope.tmp2, envelope.tmp1);
		mult += inc;
		envelope.tmp1 = signed_multiply_32x16b(mult, envelope.sample34);
		mult += inc;
		envelope.tmp2 = signed_multiply_32x16t(mult, envelope.sample34);
		envelope.sample34 = pack_16b_16b(envelope.tmp2, envelope.tmp1);
		mult += inc;
		envelope.tmp1 = signed_multiply_32x16b(mult, envelope.sample56);
		mult += inc;
		envelope.tmp2 = signed_multiply_32x16t(mult, envelope.sample56);
		envelope.sample56 = pack_16b_16b(envelope.tmp2, envelope.tmp1);
		mult += inc;
		envelope.tmp1 = signed_multiply_32x16b(mult, envelope.sample78);
		mult += inc;
		envelope.tmp2 = signed_multiply_32x16t(mult, envelope.sample78);
		envelope.sample78 = pack_16b_16b(envelope.tmp2, envelope.tmp1);

		*p++ = envelope.sample12;
		*p++ = envelope.sample34;
		*p++ = envelope.sample56;
		*p++ = envelope.sample78;

		// adjust the long-term gain using 30 bit resolution (fix #102)
		// https://github.com/PaulStoffregen/Audio/issues/102
		envelope.mult_hires += envelope.inc_hires;
		if(envelope.mult_hires < 0 )
		{
			envelope.mult_hires = 0;
			envelope.count = 1;
			debugLog.setMaxLineCount(9);
			debugLog.addLine("MH<0 ");
			debugLog.addValue(envelope.state);
			debugLog.addText(" ");
			debugLog.addValue(envelope.count);
		}
		else if(envelope.mult_hires > 0x3FFFFFFF)
		{
			envelope.mult_hires = 0x3FFFFFFF;
			envelope.count = 1;
			debugLog.setMaxLineCount(9);
			debugLog.addLine("MH>MAX ");
			debugLog.addValue(envelope.state);
			debugLog.addText(" ");
			debugLog.addValue(envelope.count);
		}

		if(envelope.count > 0 )
		{
			envelope.count--;
		}
		else
		{
			debugLog.setMaxLineCount(10);
			debugLog.addLine("count = 0 ");
			debugLog.addValue(envelope.state);
		}
	}
	transmit(block);
	AudioStream::release(block);
}

uint8_t AudioPlayMemory::envelopeEndRelease()
{
	return  envelope.endReleaseFlag;
}
void AudioPlayMemory::envelopeClearEndReleaseFlag()
{
	envelope.endReleaseFlag = 0;
}

uint8_t AudioPlayMemory::envelopeGetEndReleaseKill()
{
	return envelope.endKillReleaseFlag;
}
void AudioPlayMemory::envelopeClearEndReleaseKill()
{
	envelope.endKillReleaseFlag = 0;
}

uint8_t AudioPlayMemory:: envelopeGetState()
{
	return envelope.state;
}

void AudioPlayMemory::envelopeSyncTrackerSeq(uint32_t val, float seqSpeed)
{
	if(envelope.state == envelopePhaseForced) return;

	AudioNoInterrupts();
	uint16_t ticksOnPeriod = (6912/12) * envelope.syncRate;

	uint16_t stepShift = (envelope.startStep % 36) * (6912/12);

	val += stepShift;

	uint16_t currentPointInPhase = val % ticksOnPeriod;

	float lfoFrequency = (seqSpeed/15.0);
	float periodTime = (1000 / lfoFrequency) * envelope.syncRate;

	if(envelope.phaseNumber[0] != -1 && envelope.phaseNumber[1] != -1)
	{
		float fullPhaseTime = periodTime/2.0f;
		uint16_t fullPhaseCount = envelopeMilliseconds2count(fullPhaseTime);

		if(currentPointInPhase > ticksOnPeriod/2)
		{
			envelope.state = envelope.phaseNumber[1];
			envelope.count = envelopeMilliseconds2count((fullPhaseTime) * (float)( (float)(currentPointInPhase-ticksOnPeriod/2.0f) /(ticksOnPeriod/2.0f)));
			if(envelope.count == 0) envelope.count = 1;

			if(envelope.phaseNumber[1] == envelopePhaseDecay)
			{
				envelope.decay_count = fullPhaseCount;
				if (envelope.decay_count == 0) envelope.decay_count = 1;
				envelope.inc_hires = fullPhaseCount ? -(0x3FFFFFFF/fullPhaseCount) : 0;
				envelope.mult_hires = 0x3FFFFFFF + (envelope.inc_hires * (fullPhaseCount - envelope.count));
			}
			else if(envelope.phaseNumber[1] == envelopePhaseRelease)
			{
				envelope.release_count = fullPhaseCount;
				if (envelope.release_count == 0) envelope.release_count = 1;
				envelope.inc_hires = 0;
				envelope.mult_hires = 0;
				// w normalnym przypadku powinno byc od ostatniego mult_hires - ale  w lfo stan poczatkowy release = 1.0
			}


		}
		else
		{
			if(envelope.phaseNumber[0] == envelopePhaseAttack)
			{
				envelope.attack_count = fullPhaseCount;
				if (envelope.attack_count == 0) envelope.attack_count = 1;
				envelope.inc_hires = fullPhaseCount ? 0x3FFFFFFF/fullPhaseCount : 0;
			}
			else if(envelope.phaseNumber[0] == envelopePhaseHold)
			{
				envelope.hold_count = fullPhaseCount;
				if (envelope.hold_count == 0) envelope.hold_count = 1;
			}

			envelope.state = envelope.phaseNumber[0];
			envelope.count = envelopeMilliseconds2count((fullPhaseTime) * (float)((float)(currentPointInPhase) /(ticksOnPeriod/2.0f)));
			if(envelope.count == 0) envelope.count = 1;
			if(envelope.phaseNumber[0] == envelopePhaseAttack)
			{
				envelope.mult_hires = envelope.inc_hires * (fullPhaseCount - envelope.count);
			}
			else if(envelope.phaseNumber[0] == envelopePhaseHold)
			{
				envelope.mult_hires = 0x3FFFFFFF;
				envelope.inc_hires = 0;
			}
		}
	}
	else if(envelope.phaseNumber[0] != -1)
	{
		uint16_t fullPhaseCount = envelopeMilliseconds2count(periodTime);

		envelope.state = envelope.phaseNumber[0];
		envelope.count = envelopeMilliseconds2count((periodTime) * (float)((float)currentPointInPhase/ticksOnPeriod));
		if(envelope.count == 0) envelope.count = 1;
		if(envelope.phaseNumber[0] == envelopePhaseAttack)
		{
			envelope.attack_count = fullPhaseCount;
			if (envelope.attack_count == 0) envelope.attack_count = 1;
			envelope.inc_hires = fullPhaseCount ? 0x3FFFFFFF/fullPhaseCount : 0;
			envelope.mult_hires = envelope.inc_hires * (fullPhaseCount - envelope.count);
		}
		else if(envelope.phaseNumber[0] == envelopePhaseDecay)
		{
			envelope.decay_count = fullPhaseCount;
			if (envelope.decay_count == 0) envelope.decay_count = 1;
			envelope.inc_hires = fullPhaseCount ? -(0x3FFFFFFF/fullPhaseCount) : 0;
			envelope.mult_hires = 0x3FFFFFFF + (envelope.inc_hires * (fullPhaseCount - envelope.count));
		}
	}
	AudioInterrupts();
}
void AudioPlayMemory::envelopeSetSyncStartStep(uint16_t n)
{
	envelope.startStep = n;
}
void AudioPlayMemory::envelopeSetPhaseNumbers(int8_t n1, int8_t n2)
{
	envelope.phaseNumber[0] = n1;
	envelope.phaseNumber[1] = n2;
}
void AudioPlayMemory::envelopeSetSyncRate(float sync)
{
	envelope.syncRate = sync;
}

void AudioPlayMemory::envelopeSwitchPhase(uint8_t nextPhase)
{
	switch(nextPhase)
	{
		case envelopePhaseIdle:
			envelope.state = envelopePhaseIdle;
			envelope.mult_hires = 0;
			envelope.inc_hires = 0;
			envelope.count = 0xFFFF;
		break;
		case envelopePhaseDelay:
			envelope.state = envelopePhaseDelay;
			envelope.count = envelope.delay_count;
			envelope.mult_hires = 0;
			envelope.inc_hires = 0;
		break;

		case envelopePhaseAttack:
			envelope.state = envelopePhaseAttack;
			envelope.count = envelope.attack_count;
			envelope.inc_hires = envelope.count? 0x3FFFFFFF / envelope.count : 0x3FFFFFFF;
			envelope.mult_hires = 0;
		break;
		case envelopePhaseHold:
			envelope.state = envelopePhaseHold;
			envelope.count = envelope.hold_count;
			envelope.inc_hires = 0;
			envelope.mult_hires = 0x3FFFFFFF;
		break;
		case envelopePhaseDecay:
			envelope.state = envelopePhaseDecay;
			envelope.count = envelope.decay_count;
			envelope.inc_hires = envelope.count ? (envelope.sustain_mult - 0x3FFFFFFF) / envelope.count : (envelope.sustain_mult - 0x3FFFFFFF);
			envelope.mult_hires = 0x3FFFFFFF;
		break;
		case envelopePhaseSustain:
			envelope.state = envelopePhaseSustain;
			envelope.count = 0xFFFF;
			envelope.inc_hires = 0;
			envelope.mult_hires = envelope.sustain_mult;
		break;
		case envelopePhaseRelease:
			envelope.state = envelopePhaseRelease;
			envelope.count = envelope.release_count;
			envelope.inc_hires = envelope.count ? (-envelope.mult_hires) / envelope.count : (-envelope.mult_hires);
			//mult bez zmian
		break;
		case envelopePhaseForced:
			envelope.state = envelopePhaseForced;
			envelope.count = envelope.release_forced_count;
			envelope.inc_hires = envelope.count ? (-envelope.mult_hires) / envelope.count : (-envelope.mult_hires);
			//mult bez zmian
		break;
		default:
		break;
	}
}

uint16_t AudioPlayMemory::envelopeMilliseconds2count(float milliseconds)
{
	if (milliseconds < 0.0) milliseconds = 0.0;
	uint32_t c = ((uint32_t)(milliseconds*SAMPLES_PER_MSEC)+7)>>3;
	if (c > 65535) c = 65535; // allow up to 11.88 seconds
	return c;
}

#undef SAMPLES_PER_MSEC
