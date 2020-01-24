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

constexpr uint32_t SAMPLES_OF_20S = ((uint32_t)( 20000 *SAMPLES_PER_MSEC));

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
	void delay(float milliseconds)
	{
		delay_count = milliseconds2count(milliseconds);
	}
	void attack(float milliseconds)
	{
		attack_count = milliseconds2count(milliseconds);
	}
	void hold(float milliseconds)
	{
		hold_count = milliseconds2count(milliseconds);
	}
	void decay(float milliseconds)
	{
		decay_count = milliseconds2count(milliseconds);
	}
	void sustain(float level)
	{
		if (level < 0.0) sustain_vol = 0;
		else if (level > 1.0) sustain_vol = 1.0;
		else sustain_vol = level;
	}
	void release(float milliseconds)
	{
		release_count = milliseconds2count(milliseconds);
	}
	void releaseNoteOn(float milliseconds)
	{
		release_forced_count = milliseconds2count(milliseconds);
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
	uint8_t getState();
private:

	uint8_t endReleaseFlag=0;
	uint32_t milliseconds2count(float milliseconds)
	{
		if (milliseconds < 0.0) milliseconds = 0.0;

		uint32_t c = ((uint32_t)(milliseconds*SAMPLES_PER_MSEC));
		if (c > SAMPLES_OF_20S) c = SAMPLES_OF_20S; // allow up to 11.88 seconds
		return c;
	}
	audio_block_t *inputQueueArray[1];
	// state
	uint8_t  state;      // idle, delay, attack, hold, decay, sustain, release, forced
	uint32_t count;      // how much time remains in this state, in 8 sample units
	float startVol;
	float incVol;

	// settings
	uint32_t delay_count;
	uint32_t attack_count;
	uint32_t hold_count;
	uint32_t decay_count;
	float   sustain_vol;
	uint32_t release_count;
	uint32_t release_forced_count;
	uint8_t loopFlag = 0;
	uint8_t pressedFlag = 0;
	uint8_t passFlag = 0;

};

#undef SAMPLES_PER_MSEC
#endif
