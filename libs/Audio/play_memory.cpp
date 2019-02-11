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



uint8_t AudioPlayMemory::play(uint8_t instr_idx,int8_t note)
{

	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	playing = 0;
	prior = 0;
	stopLoop=0;
	loopBackwardFlag=0;
	pitchCounter=0;
	glideCounter=0;
	slideCounter=0;
	currentInstr_idx=instr_idx;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI STEP========================================================*/
	glide=mtProject.instrument[instr_idx].glide;
	currentTune=mtProject.instrument[instr_idx].tune;

	if(lastNote>=0) pitchControl=notes[lastNote + mtProject.instrument[instr_idx].tune];
	else pitchControl=notes[note+ mtProject.instrument[instr_idx].tune];

	int16_t * data = mtProject.sampleBank.sample[mtProject.instrument[instr_idx].sampleIndex].address;

	playMode=mtProject.instrument[instr_idx].playMode;

	startLen=mtProject.sampleBank.sample[mtProject.instrument[instr_idx].sampleIndex].length;



	if(mtProject.sampleBank.sample[mtProject.instrument[instr_idx].sampleIndex].type != mtSampleTypeWavetable)
	{
		startPoint=mtProject.instrument[instr_idx].startPoint;
		endPoint=mtProject.instrument[instr_idx].endPoint;
		if(playMode != singleShot) //loopMode
		{
			loopPoint1=mtProject.instrument[instr_idx].loopPoint1;
			loopPoint2=mtProject.instrument[instr_idx].loopPoint2;
		}
	}
	else
	{
		wavetableWindowSize = mtProject.sampleBank.sample[mtProject.instrument[instr_idx].sampleIndex].wavetable_window_size;
		currentWindow=mtProject.instrument[instr_idx].wavetableCurrentWindow;
		samplePoints.start=currentWindow*wavetableWindowSize;;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=(currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
	if(mtProject.sampleBank.sample[mtProject.instrument[instr_idx].sampleIndex].type != mtSampleTypeWavetable)
	{
		if(playMode == singleShot)
		{
			if (startPoint >= endPoint) return badStartPoint;
		}
		else
		{
			if ( (startPoint >= endPoint) || (startPoint > loopPoint1) || (startPoint > loopPoint2) ) return badStartPoint;
			if ((loopPoint1 > loopPoint2) || (loopPoint1 > endPoint)) return badLoopPoint1;
			if (loopPoint2 > endPoint) return badLoopPoint2;
		}
	}

	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/
	if(mtProject.instrument[instr_idx].fineTune >= 0)
	{
		if((note + mtProject.instrument[instr_idx].tune + 1) <= MAX_NOTE)
		{
			fineTuneControl= mtProject.instrument[instr_idx].fineTune * ((notes[note + currentTune + 1] - notes[note + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((note + mtProject.instrument[instr_idx].tune - 1) >= MIN_NOTE)
		{
			fineTuneControl= (0 - mtProject.instrument[instr_idx].fineTune) * ((notes[note + currentTune - 1] - notes[note + currentTune] )/MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}

	pitchControl+=fineTuneControl;


	sampleConstrains.glide=(uint32_t)(glide*44.1);
	if((lastNote>=0) && (lastNote != note)) glideControl=(notes[note + currentTune]-notes[lastNote + currentTune] )/sampleConstrains.glide;
	else glideControl=0;

	lastNote=note;
	if(mtProject.sampleBank.sample[mtProject.instrument[instr_idx].sampleIndex].type != mtSampleTypeWavetable)
	{
		samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
		samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
		if(playMode != singleShot)
		{
			samplePoints.loop1= (uint32_t)((float)loopPoint1*((float)startLen/MAX_16BIT));
			samplePoints.loop2= (uint32_t)((float)loopPoint2*((float)startLen/MAX_16BIT));
		}

		if((samplePoints.start >= startLen) || (samplePoints.loop1>startLen) || (samplePoints.loop2>startLen) || (samplePoints.end>startLen)) return pointsBeyondFile; // wskazniki za plikiem

		if(playMode != singleShot)
		{
			sampleConstrains.loopPoint1=samplePoints.loop1- samplePoints.start;
			sampleConstrains.loopPoint2=samplePoints.loop2- samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
		}

		sampleConstrains.endPoint=samplePoints.end- samplePoints.start;
	}



/*===========================================================================================================================*/
/*============================================PRZEKAZANIE PARAMETROW=========================================================*/
	next = data+samplePoints.start;
	beginning = data+samplePoints.start;
	length =startLen-samplePoints.start;

	playing = 0x81;

	return successInit;

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
	int16_t s0;
	int i;

	if (!playing) return;
	block = allocate();
	if (block == NULL) return;

	//Serial.write('.');

	out = block->data;
	in = next;
	s0 = prior;

	switch (playing)
	{

	  case 0x81: // 16 bit PCM, 44100 Hz

		if(mtProject.sampleBank.sample[mtProject.instrument[currentInstr_idx].sampleIndex].type == mtSampleTypeWavetable)
		{
			pitchCounter-=waveTablePosition;

			waveTablePosition=wavetableWindowSize * currentWindow;

			pitchCounter+=waveTablePosition;

			sampleConstrains.loopPoint2=waveTablePosition + wavetableWindowSize;

			sampleConstrains.loopPoint1=waveTablePosition;
		}
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i ++)
		{

			if (length > (uint32_t)pitchCounter) //if (length > 0)
			{

				if(glideCounter<=sampleConstrains.glide) pitchControl+=glideControl;
				glideCounter++;
				if(slideControl != 0.0f)
				{
					if(slideCounter<=sampleConstrains.slide)
					{
						pitchControl+=slideControl;
						slideCounter++;
					}
					else
					{
						pitchControl -= (slideControl * slideCounter);
						slideControl=0.0f;
						slideCounter=0;
						sampleConstrains.glide=0;
					}

				}

				if(((playMode == singleShot) ||(playMode == loopForward)) && (mtProject.sampleBank.sample[mtProject.instrument[currentInstr_idx].sampleIndex].type != mtSampleTypeWavetable))
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					pitchCounter+=pitchControl;

					if(playMode == loopForward)
					{
						if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) ) pitchCounter = sampleConstrains.loopPoint1 ;
					}
				}
				else if((playMode == loopBackward) && (mtProject.sampleBank.sample[mtProject.instrument[currentInstr_idx].sampleIndex].type != mtSampleTypeWavetable) )
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					if(!loopBackwardFlag) pitchCounter+=pitchControl;
					else pitchCounter-=pitchControl;

					if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) && (!loopBackwardFlag) ) loopBackwardFlag=1;
					if(( (uint32_t)pitchCounter  <= sampleConstrains.loopPoint1) && (!stopLoop) && loopBackwardFlag ) pitchCounter = sampleConstrains.loopPoint2 ;

				}
				else if((playMode == loopPingPong) && (mtProject.sampleBank.sample[mtProject.instrument[currentInstr_idx].sampleIndex].type != mtSampleTypeWavetable))
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					if(!loopBackwardFlag) pitchCounter+=pitchControl;
					else pitchCounter-=pitchControl;


					if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) && (!loopBackwardFlag) ) loopBackwardFlag=1;
					if(( (uint32_t)pitchCounter  <= sampleConstrains.loopPoint1) && (!stopLoop) && loopBackwardFlag ) loopBackwardFlag=0;
				}

				if(mtProject.sampleBank.sample[mtProject.instrument[currentInstr_idx].sampleIndex].type == mtSampleTypeWavetable)
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					pitchCounter+=pitchControl;

					if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) ) pitchCounter = sampleConstrains.loopPoint1;
				}


				if( (uint32_t)pitchCounter >= sampleConstrains.endPoint) pitchCounter=length;
			}
			else
			{
				*out++=0;
				playing = 0;
			}
		}
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
	switch (p)
	{
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

void AudioPlayMemory::setWavetableWindow(uint16_t value)
{
	if(mtProject.sampleBank.sample[mtProject.instrument[currentInstr_idx].sampleIndex].type != mtSampleTypeWavetable) return;
	if(value > MAX_WAVETABLE_WINDOW) return;
	currentWindow=value;
}

void AudioPlayMemory::setPlayMode(uint8_t value)
{
	if(playMode>3) return;
	playMode=value;
}

void AudioPlayMemory::setLP1(uint16_t value)
{
	if(playMode != singleShot) samplePoints.loop1= (uint32_t)((float)value*((float)startLen/MAX_16BIT));
	if ((samplePoints.loop1 < samplePoints.start)||(samplePoints.loop1 > samplePoints.loop2) || (samplePoints.loop1 > samplePoints.end)) return;
	if(playMode != singleShot)
	{
			sampleConstrains.loopPoint1=samplePoints.loop1- samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}
}
void AudioPlayMemory::setLP2(uint16_t value)
{
	if(playMode != singleShot) samplePoints.loop2= (uint32_t)((float)value*((float)startLen/MAX_16BIT));
	if ((samplePoints.loop2 < samplePoints.start)||(samplePoints.loop2 < samplePoints.loop1) || (samplePoints.loop1 > samplePoints.end)) return;
	if(playMode != singleShot)
	{
			sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}
}

void AudioPlayMemory::setGlide(uint16_t value, int8_t currentNote, uint8_t instr_idx)
{
	sampleConstrains.glide=(uint32_t)(value*44.1);
	if((lastNote>=0) && (lastNote != currentNote)) glideControl=(notes[currentNote + mtProject.instrument[instr_idx].tune] - notes[lastNote + mtProject.instrument[instr_idx].tune] )/sampleConstrains.glide;
	else glideControl=0;

}

void AudioPlayMemory::setSlide(uint16_t value, int8_t currentNote, int8_t slideNote,uint8_t instr_idx)
{
	pitchControl-=(slideControl * slideCounter);
	slideCounter=0;
	sampleConstrains.slide =(uint32_t)(value*44.1);
	if((slideNote>=0) && (slideNote != currentNote)) slideControl=(notes[slideNote + mtProject.instrument[instr_idx].tune] - notes[currentNote] )/sampleConstrains.slide;
	else slideControl=0;

}

void AudioPlayMemory::setPitch(float value)
{
	if(value) pitchControl+=value;
	if(pitchControl < MIN_PITCH) pitchControl=MIN_PITCH;
	if(pitchControl > MAX_PITCH ) pitchControl=MAX_PITCH;
}

void AudioPlayMemory::setFineTune(int8_t value, int8_t currentNote)
{
	pitchControl-=fineTuneControl;
	if(value >= 0)
	{
		if((currentNote + currentTune + 1) <= MAX_NOTE)
		{
			fineTuneControl= value * ((notes[currentNote + currentTune + 1] - notes[currentNote + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((currentNote + currentTune - 1) >= MIN_NOTE)
		{
			fineTuneControl= (0-value) * ((notes[currentNote + currentTune - 1] - notes[currentNote + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}

	pitchControl+=fineTuneControl;
}




