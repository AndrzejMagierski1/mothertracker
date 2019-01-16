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

#ifndef play_memory_h_
#define play_memory_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "mtStructs.h"

class AudioPlayMemory : public AudioStream
{
public:
	AudioPlayMemory(void) : AudioStream(0, NULL), playing(0) { }
	uint8_t play(strStep * step,strMtModAudioEngine * mod);
	uint8_t play(strInstrument *instr,strMtModAudioEngine * mod, uint8_t vol );
	void stop(void);
	bool isPlaying(void) { return playing; }
	uint32_t positionMillis(void);
	uint32_t lengthMillis(void);
	virtual void update(void);
	void stopLoopMode(void);
	uint8_t setMod(strStep * step, strMtModAudioEngine * mod);
	uint8_t setMod(strInstrument * instr, strMtModAudioEngine * mod);

private:
	int16_t *next;
	int16_t *beginning;
	uint32_t length;
	int16_t prior;
	float pitchControl=1;
	float pitchCounter=0;
	uint8_t playMode=0;
	volatile uint8_t playing;

	struct strSamplePoints
	{
		uint32_t start=0;
		uint32_t end=0;
		uint32_t loop1=0;
		uint32_t loop2=0;
	} samplePoints;

	struct strSampleConstrains
	{
		uint32_t loopLength=0;
		uint32_t loopPoint1=0;
		uint32_t loopPoint2=0;
		uint32_t endPoint=0;

	} sampleConstrains;

	uint32_t startLen=0;
	uint8_t  stopLoop=0;
};

#endif
