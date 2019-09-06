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

constexpr uint8_t SMOOTHING_SIZE = 100;

uint8_t AudioPlayMemory::play(uint8_t instr_idx,int8_t note)
{
	__disable_irq();
	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	if(playing == 0x81) needSmoothingFlag = 1;
	playing = 0;
	prior = 0;
	stopLoop=0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;
	currentInstr_idx=instr_idx;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
	glide=mtProject.instrument[instr_idx].glide;
	currentTune=mtProject.instrument[instr_idx].tune;

	if( (note + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>note) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-note;
	}
	if( (note + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<note)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-note;
	}

	if(lastNote>=0 && glide != 0 ) pitchControl=notes[lastNote + currentTune];
	else pitchControl=notes[note+ currentTune];

	int16_t * data = mtProject.instrument[instr_idx].sample.address;

	playMode=mtProject.instrument[instr_idx].playMode;

	startLen=mtProject.instrument[instr_idx].sample.length;



	if(mtProject.instrument[instr_idx].sample.type != mtSampleTypeWavetable)
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
		wavetableWindowSize = mtProject.instrument[instr_idx].sample.wavetable_window_size;
		currentWindow=mtProject.instrument[instr_idx].wavetableCurrentWindow;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
	if(mtProject.instrument[instr_idx].sample.type != mtSampleTypeWavetable)
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
		currentFineTune=mtProject.instrument[instr_idx].fineTune;
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

	if(glide)
	{
		sampleConstrains.glide=(uint32_t)((float)glide*44.1);
		if((lastNote>=0) && (lastNote != note)) glideControl=(notes[note + currentTune]-notes[lastNote + currentTune] )/sampleConstrains.glide;
		else glideControl=0;
	}
	else
	{
		sampleConstrains.glide=0;
		glideControl=0;
	}


	lastNote=note;
	if(mtProject.instrument[instr_idx].sample.type != mtSampleTypeWavetable)
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
			sampleConstrains.loopPoint1=samplePoints.loop1-samplePoints.start;
			sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
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
	__enable_irq();
}


void AudioPlayMemory::stop(void)
{
	__disable_irq();
	playing = 0;
	__enable_irq();
}


void AudioPlayMemory::update(void)
{
	audio_block_t *block=NULL;
	int16_t *in=NULL;
	int16_t *out=NULL;
	int16_t s0=0;
	int i;
	uint32_t castPitchControl;
	uint8_t localType = mtProject.instrument[currentInstr_idx].sample.type ;

	block = allocate();
	if (!block) return;

	if (!playing)
	{
		release(block);
		return;
	}
	else if (playing == 0x81)
	{

		out = block->data;
		in = next;
		s0 = prior;

		if (localType == mtSampleTypeWavetable)
		{
			waveTablePosition = wavetableWindowSize * currentWindow;
		}
		castPitchControl = (uint32_t) pitchControl;
		//todo: monitorować czy przez tą linijke nie wyjezdza za bufor
		length += castPitchControl; //maksymalnie moze wyjsc za length i nie wiecej niz pitch control
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (length > iPitchCounter)
			{
				if (sampleConstrains.glide)
				{
					if (glideCounter <= sampleConstrains.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (uint32_t) pitchControl;
					}
					glideCounter++;
				}

				if (slideControl != 0.0f)
				{
					if (slideCounter <= sampleConstrains.slide)
					{
						pitchControl += slideControl;
						castPitchControl = (uint32_t) pitchControl;
						slideCounter++;
					}
					else
					{
						pitchControl -= (slideControl * slideCounter); // nie bac sie - to sie robi tylko raz
						castPitchControl = (uint32_t) pitchControl;
						slideControl = 0.0f;
						slideCounter = 0;
						sampleConstrains.slide = 0;
					}

				}
				if(needSmoothingFlag && i == 0)
				{
					needSmoothingFlag = 0;
					for(uint8_t j = 0; j < SMOOTHING_SIZE; j++ )
					{
						*out++ = ((int32_t)(((int32_t)((*(in + iPitchCounter) )*j) + (int32_t)(lastSample * (SMOOTHING_SIZE - 1 - j)) ) )/(SMOOTHING_SIZE - 1));
						iPitchCounter += castPitchControl;
						fPitchCounter += pitchControl - castPitchControl;
						if (fPitchCounter >= 1.0f)
						{
							fPitchCounter -= 1.0f;
							iPitchCounter++;
						}
					}
					i = SMOOTHING_SIZE;
				}
				if (localType != mtSampleTypeWavetable)
				{
					switch (playMode)
					{
					case singleShot:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						iPitchCounter += castPitchControl;
						fPitchCounter += pitchControl - castPitchControl;
						if (fPitchCounter >= 1.0f)
						{
							fPitchCounter -= 1.0f;
							iPitchCounter++;
						}
						break;
					case loopForward:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						iPitchCounter += castPitchControl;
						fPitchCounter += pitchControl - castPitchControl;
						if (fPitchCounter >= 1.0f)
						{
							fPitchCounter -= 1.0f;
							iPitchCounter++;
						}
						break;
					case loopBackward:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						if (!loopBackwardFlag)
						{
							iPitchCounter += castPitchControl;
							fPitchCounter += pitchControl - castPitchControl;
							if (fPitchCounter >= 1.0f)
							{
								fPitchCounter -= 1.0f;
								iPitchCounter++;
							}
						}
						else
						{
							iPitchCounter -= castPitchControl;
							if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;

							fPitchCounter -= pitchControl - castPitchControl;
							if (fPitchCounter <= -1.0f)
							{
								fPitchCounter += 1.0f;
								iPitchCounter--;
							}
						}
						break;
					case loopPingPong:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						if (!loopBackwardFlag)
						{
							iPitchCounter += castPitchControl;
							fPitchCounter += pitchControl - castPitchControl;
							if (fPitchCounter >= 1.0f)
							{
								fPitchCounter -= 1.0f;
								iPitchCounter++;
							}
						}
						else
						{
							iPitchCounter -= castPitchControl;
							if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;

							fPitchCounter -= pitchControl - castPitchControl;
							if (fPitchCounter <= -1.0f)
							{
								fPitchCounter += 1.0f;
								iPitchCounter--;
							}
						}
						break;
					default:
						break;
					}

					castPitchControl = (uint32_t) pitchControl;

					switch (playMode)
					{
					case loopForward:
						if ((iPitchCounter >= sampleConstrains.loopPoint2))
						{
							iPitchCounter = sampleConstrains.loopPoint1;
							fPitchCounter = 0;
						}
						break;
					case loopBackward:
						if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
						{
							loopBackwardFlag = 1;
							fPitchCounter = 0;

						}
						if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
						{
							iPitchCounter = sampleConstrains.loopPoint2;
							fPitchCounter = 0;
						}
						break;
					case loopPingPong:
						if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
						{
							loopBackwardFlag = 1;
							fPitchCounter = 0;
						}
						if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
						{
							loopBackwardFlag = 0;
							fPitchCounter = 0;
						}
						break;
					default:
						break;
					}
				}
				else
				{
					*out++ = *(in + (uint32_t) iPitchCounter + waveTablePosition);
					if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + (uint32_t) iPitchCounter + waveTablePosition);
					iPitchCounter += castPitchControl;
					fPitchCounter += pitchControl - castPitchControl;
					if ((iPitchCounter >= wavetableWindowSize))
					{
						iPitchCounter = 0;
						fPitchCounter = 0;
					}
				}
				if ((iPitchCounter >= sampleConstrains.endPoint) && (sampleConstrains.endPoint != sampleConstrains.loopPoint2))
					iPitchCounter = length;
			}
			else
			{
				*out++ = 0;
				playing = 0;
			}

		}
		prior = s0;
		next = in;
		transmit(block);
		length -= castPitchControl; //powrot do bazowej dlugosci
	}
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
	if(mtProject.instrument[currentInstr_idx].sample.type != mtSampleTypeWavetable) return;
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
		if((currentNote + currentTune + 1) <= (MAX_NOTE-1))
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
	currentFineTune=value;
}

void AudioPlayMemory::setTune(int8_t value, int8_t currentNote)
{
	if( (currentNote + value) > (MAX_NOTE-1)) value=(MAX_NOTE-1)-currentNote;
	if( (currentNote + value) < MIN_NOTE) value=MIN_NOTE-currentNote;


	pitchControl-=notes[currentNote+currentTune];
	pitchControl+=notes[currentNote+value];
	currentTune=value;
	setFineTune(currentFineTune,currentNote);
}

void AudioPlayMemory::clean(void)
{
	if(!playing)
	{
		next=NULL;
		beginning=NULL;
		length=0;
		prior=0;
		pitchControl = 1;
		iPitchCounter = 0;
		fPitchCounter = 0;
		playMode = 0;
		playing=0;
		loopBackwardFlag = 0;
		lastNote = -1;
		glide=0;
		glideCounter=0;
		slideCounter=0;
		glideControl=0;
		slideControl=0;
		fineTuneControl=0;
		currentTune=0;
		wavetableWindowSize=0;
		currentWindow=0;
		waveTablePosition=0;
//	    wavetableSync=0;
//	    wavetablePWM=0;
//	    wavetableFlip=0;
//	    wavetableQuantize=0;
		currentInstr_idx=0;
		currentFineTune=0;

		samplePoints.start=0;
		samplePoints.end=0;
		samplePoints.loop1=0;
		samplePoints.loop2=0;

		sampleConstrains.loopLength=0;
		sampleConstrains.loopPoint1=0;
		sampleConstrains.loopPoint2=0;
		sampleConstrains.endPoint=0;

		sampleConstrains.glide=0;
		sampleConstrains.slide=0;

		startLen=0;
		stopLoop=0;
	}

}

uint8_t AudioPlayMemory::playForPrev(int16_t * addr,uint32_t len)
{
	uint32_t startPoint,endPoint;
	int8_t note=60;
	playing = 0;
	prior = 0;
	stopLoop=0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;

	glide=0;
	currentTune=0;
	lastNote=-1;

	if( (note + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>note) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-note;
	}
	if( (note + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<note)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-note;
	}

	if(lastNote>=0) pitchControl=notes[lastNote + currentTune];
	else pitchControl=notes[note+ currentTune];

	int16_t * data = addr;

	playMode=singleShot;

	startLen=len;

	startPoint=0;
	endPoint=MAX_16BIT;
	currentFineTune=0;
	fineTuneControl=0;


	samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
	samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
	sampleConstrains.endPoint=samplePoints.end- samplePoints.start;

	next = data+samplePoints.start;
	beginning = data+samplePoints.start;
	length =startLen-samplePoints.start;

	playing = 0x81;

	return successInit;

}

uint8_t AudioPlayMemory::playForPrev(int16_t * addr,uint32_t len, uint8_t n)
{
	uint32_t startPoint,endPoint;
	int8_t note=n;
	playing = 0;
	prior = 0;
	stopLoop=0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;

	glide=0;
	currentTune=0;
	lastNote=-1;

	if( (note + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>note) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-note;
	}
	if( (note + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<note)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-note;
	}

	if(lastNote>=0) pitchControl=notes[lastNote + currentTune];
	else pitchControl=notes[note+ currentTune];

	int16_t * data = addr;

	playMode=singleShot;

	startLen=len;

	startPoint=0;
	endPoint=MAX_16BIT;
	currentFineTune=0;
	fineTuneControl=0;


	samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
	samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
	sampleConstrains.endPoint=samplePoints.end- samplePoints.start;

	next = data+samplePoints.start;
	beginning = data+samplePoints.start;
	length =startLen-samplePoints.start;

	playing = 0x81;

	return successInit;

}
