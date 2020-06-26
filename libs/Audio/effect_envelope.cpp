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
#include "Audio.h"
#include "debugLog.h"
#include "mtSequencer.h"

void AudioEffectEnvelope::delay(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastDelayCount = delay_count;
	delay_count = milliseconds2count(milliseconds);
	if (delay_count == 0) delay_count = 1;

	if(delay_count == lastDelayCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(state == envelopePhaseDelay)
	{
		int32_t dif = delay_count - lastDelayCount;

		if((int)(count + dif) < 1 ) count = 1;
		else if ((int)(count + dif) > 65535) count = 65535;
		else count += dif;
	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioEffectEnvelope::attack(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();

	uint16_t lastAttackCount = attack_count;

	attack_count = milliseconds2count(milliseconds);
	if (attack_count == 0) attack_count = 1;

	if(attack_count == lastAttackCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(state == envelopePhaseAttack)
	{
		int32_t dif = attack_count - lastAttackCount;

		if((int)(count + dif) < 1 ) count = 1;
		else if ((int)(count + dif) > 65535) count = 65535;
		else count += dif;

		int32_t y = 0x3FFFFFFF - mult_hires;
		int32_t x = count;

		if(x < 0) x = 0;

		inc_hires = x ?  y /x : y;

	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioEffectEnvelope::hold(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastHoldCount = hold_count;
	hold_count = milliseconds2count(milliseconds);
	if (hold_count == 0) hold_count = 1;
	if(hold_count == lastHoldCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}

	if(state == envelopePhaseHold)
	{
		int32_t dif = hold_count - lastHoldCount;

		if((int)(count + dif) < 1 ) count = 1;
		else if ((int)(count + dif) > 65535) count = 65535;
		else count += dif;

	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioEffectEnvelope::decay(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastDecayCount = decay_count;
	decay_count = milliseconds2count(milliseconds);
	if (decay_count == 0) decay_count = 1;

	if(decay_count == lastDecayCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(state == envelopePhaseDecay)
	{
		int32_t dif = decay_count - lastDecayCount;

		if((int)(count + dif) < 1 ) count = 1;
		else if ((int)(count + dif) > 65535) count = 65535;
		else count += dif;

		inc_hires = count ? (sustain_mult - mult_hires) / count : (sustain_mult - mult_hires) ;

	}
	AudioInterrupts();
	__enable_irq();
}
void  AudioEffectEnvelope::sustain(float level)
{
	__disable_irq();
	AudioNoInterrupts();
	if (level < 0.0) level = 0;
	else if (level > 1.0) level = 1.0;
	sustain_mult = level * 1073741823.0;
	if(state == envelopePhaseSustain) mult_hires = sustain_mult;
	AudioInterrupts();
	__enable_irq();
}
void  AudioEffectEnvelope::release(float milliseconds)
{
	__disable_irq();
	AudioNoInterrupts();
	uint16_t lastReleaseCount = release_count;
	release_count = milliseconds2count(milliseconds);
	if (release_count == 0) release_count = 1;

	if(release_count == lastReleaseCount)
	{
		AudioInterrupts();
		__enable_irq();
		return;
	}


	if(state == envelopePhaseRelease)
	{
		int32_t dif = release_count - lastReleaseCount;

		if( (int)(count + dif) < 1 ) count = 1;
		else if ((int)(count + dif) > 65535) count = 65535;
		else count += dif;

		inc_hires = count ? (-mult_hires) / count : -mult_hires;

	}
	AudioInterrupts();
	__enable_irq();
}


void AudioEffectEnvelope::noteOn(void)
{
	__disable_irq();

	pressedFlag = 1;

	if(passFlag)
	{
		switchPhase(envelopePhaseIdle);
	}
	else if (state == envelopePhaseIdle || state == envelopePhaseDelay)
	{
		switchPhase(envelopePhaseDelay);
	}
	else if (state != envelopePhaseForced)
	{
		switchPhase(envelopePhaseForced);
	}
	else endKillReleaseFlag = 1;
	// nie ma dla force bo i tak wyliczy taka sama prosta wygaszania jak byla
	__enable_irq();
}

void AudioEffectEnvelope::noteOff(void)
{
	pressedFlag = 0;
	if (state != envelopePhaseIdle)
	{
		if(loopFlag) switchPhase(envelopePhaseIdle);
		else
		{
			switchPhase(envelopePhaseRelease);
		}

	}
	__enable_irq();
}

void AudioEffectEnvelope::setIdle(void)
{
	__disable_irq();
	pressedFlag = 0;
	switchPhase(envelopePhaseIdle);
	__enable_irq();
}

void AudioEffectEnvelope::update(void)
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

	if(passFlag)
	{
		transmit(block);
		AudioStream::release(block);
		return;
	}
	if (state == envelopePhaseIdle)
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
		if (state == envelopePhaseSustain)
		{
			if(loopFlag && pressedFlag)
			{
				switchPhase(envelopePhaseRelease);
			}
		}

		if (count == 0)
		{
			if (state == envelopePhaseAttack)
			{
				switchPhase(envelopePhaseHold);
				if(isRandom)
				{
					randomSeed(micros());
					mult_hires = random(0,0x3FFFFFFF);
				}
				continue;
			}
			else if (state == envelopePhaseHold)
			{
				if(isRandom)
				{
					switchPhase(envelopePhaseHold);
					randomSeed(micros());
					mult_hires = random(0,0x3FFFFFFF);
				}
				else switchPhase(envelopePhaseDecay);
				continue;
			}
			else if (state == envelopePhaseDecay)
			{
				if(loopFlag && pressedFlag)
				{
					switchPhase(envelopePhaseRelease);
					continue;
				}
				else
				{
					switchPhase(envelopePhaseSustain);
				}
			}
			else if (state == envelopePhaseSustain)
			{
				switchPhase(envelopePhaseSustain);
			}
			else if (state == envelopePhaseRelease)
			{
				if(loopFlag && pressedFlag)
				{
					switchPhase(envelopePhaseDelay);
					continue;
				}
				else
				{
					switchPhase(envelopePhaseIdle);
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
			else if (state == envelopePhaseForced)
			{
				switchPhase(envelopePhaseDelay);
				endKillReleaseFlag = 1;
				continue;

			}
			else if (state == envelopePhaseDelay)
			{
				switchPhase(envelopePhaseAttack);
				continue;
			}
		}

//		if( (mult_hires + inc_hires) > 0x3FFFFFFF)
//		{
//			if(mult_hires >= 0x3FFFFFFF)
//			{
//				mult_hires= 0x3FFFFFFF;
//				inc_hires = 0;
//			}
//			else
//			{
//				mult_hires -= inc_hires;
//			}
////			count = 1;
//		}
//		else if( (mult_hires + inc_hires) < 0)
//		{
//			if(mult_hires <= 0)
//			{
//				mult_hires = 0;
//				inc_hires = 0;
//			}
//			else
//			{
//				mult_hires -= inc_hires;
//			}
////			count = 1;
//		}
//		mult_hires = 0x3FFFFFFF;
//		inc_hires = 0;

		int32_t mult = mult_hires >> 14; // przeskalowanie na 16 bitow bo 0x3FFFFFFF zajmuje 30 bitow


		int32_t inc = inc_hires >> 17; // podejrzewam ze przeskalowanie na 13 bitow aby suma takich osmiu (3bity) obslugiwala 8 probek (inc_hires wyliczone dla 8 bitowego cyklu)



		sample12 = *p;
		sample34 = *(p+1);
		sample56 = *(p+2);
		sample78 = *(p+3);
//		sample12 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//		sample34 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//		sample56 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//		sample78 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));

//		p -= 4;
		mult += inc;
		tmp1 = signed_multiply_32x16b(mult, sample12);
		mult += inc;
		tmp2 = signed_multiply_32x16t(mult, sample12);
		sample12 = pack_16b_16b(tmp2, tmp1);
		mult += inc;
		tmp1 = signed_multiply_32x16b(mult, sample34);
		mult += inc;
		tmp2 = signed_multiply_32x16t(mult, sample34);
		sample34 = pack_16b_16b(tmp2, tmp1);
		mult += inc;
		tmp1 = signed_multiply_32x16b(mult, sample56);
		mult += inc;
		tmp2 = signed_multiply_32x16t(mult, sample56);
		sample56 = pack_16b_16b(tmp2, tmp1);
		mult += inc;
		tmp1 = signed_multiply_32x16b(mult, sample78);
		mult += inc;
		tmp2 = signed_multiply_32x16t(mult, sample78);
		sample78 = pack_16b_16b(tmp2, tmp1);

//		mult = 0;
//		inc = 0;

//		sample12 = *p;
//		sample34 = *(p+1);
//		sample56 = *(p+2);
//		sample78 = *(p+3);
//
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp1 = mult * (*sample1);
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp2 = mult * (*sample2);
//
//		*sample1 = *tmp1Shifted;
//		*sample2 = *tmp2Shifted;
//
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp1 = mult * (*sample3);
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp2 = mult * (*sample4);
//
//		*sample3 = *tmp1Shifted;
//		*sample4 = *tmp2Shifted;
//
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp1 = mult * (*sample5);
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp2 = mult * (*sample6);
//
//		*sample5 = *tmp1Shifted;
//		*sample6 = *tmp2Shifted;
//
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp1 = mult * (*sample7);
//		mult+=inc;
////		if( mult >= 0x10000)
////		{
////			mult = 0xFFFF;
////			inc = 0;
////		}
////		else if( mult < 0)
////		{
////			mult = 0;
////			inc = 0;
////		}
//		tmp2 = mult * (*sample8);
//
//
//
//		*sample7 = *tmp1Shifted;
//		*sample8 = *tmp2Shifted;
//
//		if(state == envelopePhaseForced)
//		{
//			sample12 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//			sample34 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//			sample56 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//			sample78 = (MAX_SIGNED_16BIT| (MAX_SIGNED_16BIT<<16));
//		}

		*p++ = sample12;
		*p++ = sample34;
		*p++ = sample56;
		*p++ = sample78;



		// adjust the long-term gain using 30 bit resolution (fix #102)
		// https://github.com/PaulStoffregen/Audio/issues/102
		mult_hires += inc_hires;
		if(mult_hires < 0 )
		{
			mult_hires = 0;
			count = 1;
			debugLog.setMaxLineCount(9);
			debugLog.addLine("MH<0 ");
			debugLog.addValue(state);
			debugLog.addText(" ");
			debugLog.addValue(count);
		}
		else if(mult_hires > 0x3FFFFFFF)
		{
			mult_hires = 0x3FFFFFFF;
			count = 1;
			debugLog.setMaxLineCount(9);
			debugLog.addLine("MH>MAX ");
			debugLog.addValue(state);
			debugLog.addText(" ");
			debugLog.addValue(count);
		}

		if(count > 0 )
		{
			count--;
		}
		else
		{
			debugLog.setMaxLineCount(10);
			debugLog.addLine("count = 0 ");
			debugLog.addValue(state);
		}
	}
	transmit(block);
	AudioStream::release(block);
}

uint8_t AudioEffectEnvelope::endRelease()
{
	return  endReleaseFlag;
}
void AudioEffectEnvelope::clearEndReleaseFlag()
{
	endReleaseFlag = 0;
}

uint8_t AudioEffectEnvelope::getEndReleaseKill()
{
	return endKillReleaseFlag;
}
void AudioEffectEnvelope::clearEndReleaseKill()
{
	endKillReleaseFlag = 0;
}

uint8_t AudioEffectEnvelope:: getState()
{
	return state;
}

void AudioEffectEnvelope::syncTrackerSeq(uint32_t val, float seqSpeed)
{
	if(state == envelopePhaseForced) return;

	AudioNoInterrupts();
	uint16_t ticksOnPeriod = (6912/12) * syncRate;

	uint16_t stepShift = (startStep % 36) * (6912/12);

	val += stepShift;

	uint16_t currentPointInPhase = val % ticksOnPeriod;

	float lfoFrequency = (seqSpeed/15.0);
	float periodTime = (1000 / lfoFrequency) * syncRate;

	if(phaseNumber[0] != -1 && phaseNumber[1] != -1)
	{
		float fullPhaseTime = periodTime/2.0f;
		uint16_t fullPhaseCount = milliseconds2count(fullPhaseTime);

		if(currentPointInPhase > ticksOnPeriod/2)
		{
			state = phaseNumber[1];
			count = milliseconds2count((fullPhaseTime) * (float)( (float)(currentPointInPhase-ticksOnPeriod/2.0f) /(ticksOnPeriod/2.0f)));
			if(count == 0) count = 1;

			if(phaseNumber[1] == envelopePhaseDecay)
			{
				decay_count = fullPhaseCount;
				if (decay_count == 0) decay_count = 1;
				inc_hires = fullPhaseCount ? -(0x3FFFFFFF/fullPhaseCount) : 0;
				mult_hires = 0x3FFFFFFF + (inc_hires * (fullPhaseCount - count));
			}
			else if(phaseNumber[1] == envelopePhaseRelease)
			{
				release_count = fullPhaseCount;
				if (release_count == 0) release_count = 1;
				inc_hires = 0;
				mult_hires = 0;
				// w normalnym przypadku powinno byc od ostatniego mult_hires - ale  w lfo stan poczatkowy release = 1.0
			}


		}
		else
		{
			if(phaseNumber[0] == envelopePhaseAttack)
			{
				attack_count = fullPhaseCount;
				if (attack_count == 0) attack_count = 1;
				inc_hires = fullPhaseCount ? 0x3FFFFFFF/fullPhaseCount : 0;
			}
			else if(phaseNumber[0] == envelopePhaseHold)
			{
				hold_count = fullPhaseCount;
				if (hold_count == 0) hold_count = 1;
			}

			state = phaseNumber[0];
			count = milliseconds2count((fullPhaseTime) * (float)((float)(currentPointInPhase) /(ticksOnPeriod/2.0f)));
			if(count == 0) count = 1;
			if(phaseNumber[0] == envelopePhaseAttack)
			{
				mult_hires = inc_hires * (fullPhaseCount - count);
			}
			else if(phaseNumber[0] == envelopePhaseHold)
			{
				mult_hires = 0x3FFFFFFF;
				inc_hires = 0;
			}
		}
	}
	else if(phaseNumber[0] != -1)
	{
		uint16_t fullPhaseCount = milliseconds2count(periodTime);

		state = phaseNumber[0];
		count = milliseconds2count((periodTime) * (float)((float)currentPointInPhase/ticksOnPeriod));
		if(count == 0) count = 1;
		if(phaseNumber[0] == envelopePhaseAttack)
		{
			attack_count = fullPhaseCount;
			if (attack_count == 0) attack_count = 1;
			inc_hires = fullPhaseCount ? 0x3FFFFFFF/fullPhaseCount : 0;
			mult_hires = inc_hires * (fullPhaseCount - count);
		}
		else if(phaseNumber[0] == envelopePhaseDecay)
		{
			decay_count = fullPhaseCount;
			if (decay_count == 0) decay_count = 1;
			inc_hires = fullPhaseCount ? -(0x3FFFFFFF/fullPhaseCount) : 0;
			mult_hires = 0x3FFFFFFF + (inc_hires * (fullPhaseCount - count));
		}
	}
	AudioInterrupts();
}
void AudioEffectEnvelope::setSyncStartStep(uint16_t n)
{
	startStep = n;
}
void AudioEffectEnvelope::setPhaseNumbers(int8_t n1, int8_t n2)
{
	phaseNumber[0] = n1;
	phaseNumber[1] = n2;
}
void AudioEffectEnvelope::setSyncRate(float sync)
{
	syncRate = sync;
}

void AudioEffectEnvelope::switchPhase(uint8_t nextPhase)
{
	switch(nextPhase)
	{
		case envelopePhaseIdle:
			state = envelopePhaseIdle;
			mult_hires = 0;
			inc_hires = 0;
			count = 0xFFFF;
		break;
		case envelopePhaseDelay:
			state = envelopePhaseDelay;
			count = delay_count;
			mult_hires = 0;
			inc_hires = 0;
		break;

		case envelopePhaseAttack:
			state = envelopePhaseAttack;
			count = attack_count;
			inc_hires = count? 0x3FFFFFFF / count : 0x3FFFFFFF;
			mult_hires = 0;
		break;
		case envelopePhaseHold:
			state = envelopePhaseHold;
			count = hold_count;
			inc_hires = 0;
			mult_hires = 0x3FFFFFFF;
		break;
		case envelopePhaseDecay:
			state = envelopePhaseDecay;
			count = decay_count;
			inc_hires = count ? (sustain_mult - 0x3FFFFFFF) / count : (sustain_mult - 0x3FFFFFFF);
			mult_hires = 0x3FFFFFFF;
		break;
		case envelopePhaseSustain:
			state = envelopePhaseSustain;
			count = 0xFFFF;
			inc_hires = 0;
			mult_hires = sustain_mult;
		break;
		case envelopePhaseRelease:
			state = envelopePhaseRelease;
			count = release_count;
			inc_hires = count ? (-mult_hires) / count : (-mult_hires);
			//mult bez zmian
		break;
		case envelopePhaseForced:
			state = envelopePhaseForced;
			count = release_forced_count;
			inc_hires = count ? (-mult_hires) / count : (-mult_hires);
			//mult bez zmian
		break;
		default:
		break;
	}
}
