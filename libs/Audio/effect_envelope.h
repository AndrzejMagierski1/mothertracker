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

#ifndef effect_envelope_h_
#define effect_envelope_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"

#define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT/1000.0)


class AudioEffectEnvelope : public AudioStream
{
public:
	AudioEffectEnvelope() : AudioStream(1, inputQueueArray) {
		state = 0;
		delay(0.0f);  // default values...
		attack(10.5f);
		hold(2.5f);
		decay(35.0f);
		sustain(0.5f);
		release(300.0f);
		releaseNoteOn(0.0f);
	}
	void noteOn();
	void noteOff();
	void setIdle();

	void delay(float milliseconds);
	void attack(float milliseconds);
	void hold(float milliseconds);
	void decay(float milliseconds);
	void sustain(float level);
	void release(float milliseconds);
	void setSustain()
	{
		state = 5;
		count = 0xFFFF;
		mult_hires = sustain_mult;
		inc_hires = 0;
	}

	void releaseNoteOn(float milliseconds) {
		release_forced_count = milliseconds2count(milliseconds);
		if (release_forced_count == 0) release_forced_count = 1;
//		release_forced_count = 16;
	}
	void setLoop(uint8_t state)
	{
		loopFlag = state;
	}

	void setPassFlag(uint8_t state)
	{
		passFlag = state;
	}

	virtual void update(void);
	uint8_t endRelease();
	void clearEndReleaseFlag();

	uint8_t getEndReleaseKill();
	void clearEndReleaseKill();
	uint8_t getState();

	void syncTrackerSeq(uint32_t val, float seqSpeed);
	void setSyncStartStep(uint16_t n);
	void setPhaseNumbers(int8_t n1, int8_t n2);
	void setSyncRate(float sync);
	enum enEnvelopePhase
	{
		envelopePhaseIdle,
		envelopePhaseDelay,
		envelopePhaseAttack,
		envelopePhaseHold,
		envelopePhaseDecay,
		envelopePhaseSustain,
		envelopePhaseRelease,
		envelopePhaseForced
	};
private:
	void switchPhase(uint8_t nextPhase);
	uint8_t endReleaseFlag = 0;
	uint8_t endKillReleaseFlag = 0;
	uint16_t milliseconds2count(float milliseconds) {
		if (milliseconds < 0.0) milliseconds = 0.0;
		uint32_t c = ((uint32_t)(milliseconds*SAMPLES_PER_MSEC)+7)>>3;
		if (c > 65535) c = 65535; // allow up to 11.88 seconds
		return c;
	}
	audio_block_t *inputQueueArray[1];
	// state
	uint8_t  state;      // idle, delay, attack, hold, decay, sustain, release, forced
	uint16_t count;      // how much time remains in this state, in 8 sample units
	int32_t  mult_hires; // attenuation, 0=off, 0x40000000=unity gain
	int32_t  inc_hires;  // amount to change mult_hires every 8 samples

	// settings
	uint16_t delay_count;
	uint16_t attack_count;
	uint16_t hold_count;
	uint16_t decay_count;
	int32_t  sustain_mult;
	uint16_t release_count;
	uint16_t release_forced_count;
	uint8_t loopFlag = 0;
	uint8_t pressedFlag = 0;
	uint8_t passFlag = 0;
	uint32_t sample12, sample34, sample56, sample78;
	uint32_t tmp1, tmp2;

	uint16_t startStep = 0;
	int8_t phaseNumber[2] = {-1,-1};
	float syncRate = 1;
	uint16_t periodTime = 0;



};

#undef SAMPLES_PER_MSEC
#endif
