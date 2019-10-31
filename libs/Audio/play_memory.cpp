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

	if(instr_idx > INSTRUMENTS_MAX) instr_idx = INSTRUMENTS_MAX;
	if(note > MAX_NOTE) note = MAX_NOTE;

	if(playing == 0x81) needSmoothingFlag = 1;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter = 0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;
	currentInstr_idx=instr_idx;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
	if(!glideForceFlag) glide=mtProject.instrument[instr_idx].glide;
	else glide = forcedGlide;

	if(!tuneForceFlag) currentTune=mtProject.instrument[instr_idx].tune;
	else currentTune = forcedTune;

	sampleType = mtProject.instrument[instr_idx].sample.type;

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
		if(pointsForceFlag)
		{
			startPoint=forcedStartPoint;
			endPoint=forcedEndPoint;
			if(playMode != singleShot) //loopMode
			{
				loopPoint1=forcedLoopPoint1;
				loopPoint2=forcedLoopPoint2;
			}
		}
		else
		{
			startPoint=mtProject.instrument[instr_idx].startPoint;
			endPoint=mtProject.instrument[instr_idx].endPoint;
			if(playMode != singleShot) //loopMode
			{
				loopPoint1=mtProject.instrument[instr_idx].loopPoint1;
				loopPoint2=mtProject.instrument[instr_idx].loopPoint2;
			}
		}
	}
	else
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=mtProject.instrument[instr_idx].wavetableCurrentWindow;
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
		if(!fineTuneForceFlag) currentFineTune=mtProject.instrument[instr_idx].fineTune;
		else currentFineTune = forcedFineTune;

		if((note + mtProject.instrument[instr_idx].tune + 1) <= MAX_NOTE)
		{
			fineTuneControl= currentFineTune * ((notes[note + currentTune + 1] - notes[note + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((note + mtProject.instrument[instr_idx].tune - 1) >= MIN_NOTE)
		{
			fineTuneControl= (0 - currentFineTune) * ((notes[note + currentTune - 1] - notes[note + currentTune] )/MAX_INSTRUMENT_FINETUNE);
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
		else
		{
			samplePoints.loop1 = 0;
			samplePoints.loop2 = 0;
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
	iPitchCounter = reverseDirectionFlag ? sampleConstrains.endPoint - 1 : 0;
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
	int i;
	int32_t castPitchControl;
	float pitchFraction;

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

		if (sampleType == mtSampleTypeWavetable)
		{
			waveTablePosition = wavetableWindowSize * currentWindow;
		}
		castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
		pitchFraction = pitchControl - (int32_t)pitchControl;
//		//todo: monitorować czy przez tą linijke nie wyjezdza za bufor
//		length += castPitchControl; //maksymalnie moze wyjsc za length i nie wiecej niz pitch control
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (length > iPitchCounter)
			{
				if (sampleConstrains.glide)
				{
					if (glideCounter <= sampleConstrains.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
						pitchFraction = pitchControl - (int32_t)pitchControl;
						glideCounter++;
					}

				}

				if (slideControl != 0.0f)
				{
					if (slideCounter <= sampleConstrains.slide)
					{
						pitchControl += slideControl;
						castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
						slideCounter++;
					}
					else
					{
						pitchControl -= (slideControl * slideCounter); // nie bac sie - to sie robi tylko raz
						castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
						slideControl = 0.0f;
						slideCounter = 0;
						sampleConstrains.slide = 0;
					}
					pitchFraction = pitchControl - (int32_t)pitchControl;
				}

				if(needSmoothingFlag && (i == 0))
				{
					needSmoothingFlag = 0;
					for(uint8_t j = 0; j < SMOOTHING_SIZE; j++ )
					{
						*out++ = ((int32_t)(((int32_t)((*(in + iPitchCounter) )*j) + (int32_t)(lastSample * (SMOOTHING_SIZE - 1 - j)) ) )/(SMOOTHING_SIZE - 1));
						iPitchCounter += castPitchControl;
						fPitchCounter += pitchFraction;
						if (fPitchCounter >= 1.0f)
						{
							fPitchCounter -= 1.0f;
							iPitchCounter++;
						}
					}
					i = SMOOTHING_SIZE;
				}
				if (sampleType != mtSampleTypeWavetable)
				{
					switch (playMode)
					{
					case singleShot:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						iPitchCounter += castPitchControl;
						fPitchCounter += pitchFraction;
						if (fPitchCounter >= 1.0f)
						{
							fPitchCounter -= 1.0f;
							iPitchCounter++;
						}
						else if(fPitchCounter <= -1.0f)
						{
							fPitchCounter += 1.0f;
							iPitchCounter--;
						}
						break;
					case loopForward:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						iPitchCounter += castPitchControl;
						fPitchCounter += pitchFraction;
						if (fPitchCounter >= 1.0f)
						{
							fPitchCounter -= 1.0f;
							iPitchCounter++;
						}
						else if(fPitchCounter <= -1.0f)
						{
							fPitchCounter += 1.0f;
							iPitchCounter--;
						}
						break;
					case loopBackward:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						if (!loopBackwardFlag)
						{
							iPitchCounter += castPitchControl;
							fPitchCounter += pitchFraction;
							if (fPitchCounter >= 1.0f)
							{
								fPitchCounter -= 1.0f;
								iPitchCounter++;
							}
							else if(fPitchCounter <= -1.0f)
							{
								fPitchCounter += 1.0f;
								iPitchCounter--;
							}
						}
						else
						{
							iPitchCounter -= castPitchControl;

							fPitchCounter -= pitchFraction;
							if (fPitchCounter <= -1.0f)
							{
								fPitchCounter += 1.0f;
								iPitchCounter--;
							}
							else if (fPitchCounter >= 1.0f)
							{
								fPitchCounter -= 1.0f;
								iPitchCounter++;
							}
						}
						break;
					case loopPingPong:
						*out++ = *(in + iPitchCounter);
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + iPitchCounter);
						if (!loopBackwardFlag)
						{
							iPitchCounter += castPitchControl;
							fPitchCounter += pitchFraction;
							if (fPitchCounter >= 1.0f)
							{
								fPitchCounter -= 1.0f;
								iPitchCounter++;
							}
							else if(fPitchCounter <= -1.0f)
							{
								fPitchCounter += 1.0f;
								iPitchCounter--;
							}
						}
						else
						{
							iPitchCounter -= castPitchControl;

							fPitchCounter -= pitchFraction;
							if (fPitchCounter <= -1.0f)
							{
								fPitchCounter += 1.0f;
								iPitchCounter--;
							}
							else if (fPitchCounter >= 1.0f)
							{
								fPitchCounter -= 1.0f;
								iPitchCounter++;
							}
						}
						break;
					default:
						break;
					}

//					castPitchControl = (int32_t) pitchControl;
					if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
					switch (playMode)
					{
					case loopForward:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= sampleConstrains.loopPoint1))
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= sampleConstrains.loopPoint2))
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								fPitchCounter = 0;
							}
						}

						break;
					case loopBackward:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								fPitchCounter = 0;
							}
						}

						break;
					case loopPingPong:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
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
					fPitchCounter += pitchFraction;
					if (fPitchCounter >= 1.0f)
					{
						fPitchCounter -= 1.0f;
						iPitchCounter++;
					}

					if (iPitchCounter >= wavetableWindowSize)
					{
						iPitchCounter -= wavetableWindowSize;
					}
				}
				if ((iPitchCounter >= (sampleConstrains.endPoint)) && (sampleConstrains.endPoint != (sampleConstrains.loopPoint2)) && !reverseDirectionFlag)
				{
					iPitchCounter = length;
				}
				else if (((iPitchCounter - castPitchControl) <= 0)  && reverseDirectionFlag)
				{
					iPitchCounter = 0;
				}
			}
			else
			{
				*out++ = 0;
				playing = 0;
			}

		}
		next = in;
		transmit(block);
//		length -= castPitchControl; //powrot do bazowej dlugosci
	}
	release(block);

}


void AudioPlayMemory::setWavetableWindow(int16_t value)
{
	if(mtProject.instrument[currentInstr_idx].sample.type != mtSampleTypeWavetable) return;
	if(value > MAX_WAVETABLE_WINDOW) currentWindow = MAX_WAVETABLE_WINDOW;
	else if(value < 0) currentWindow = 0;
	else currentWindow=value;
}

void AudioPlayMemory::setPlayMode(uint8_t value)
{
	if(playMode>3) return;
	playMode=value;
}

void AudioPlayMemory::setLP1(uint16_t value) // w audio engine zadba zeby zapodac odpowiednia wartosc gdy force
{
	if(playMode != singleShot) samplePoints.loop1= (uint32_t)((float)value*((float)startLen/MAX_16BIT));
	if ((samplePoints.loop1 < samplePoints.start)||(samplePoints.loop1 > samplePoints.loop2) || (samplePoints.loop1 > samplePoints.end)) return;
	if(playMode != singleShot)
	{
			sampleConstrains.loopPoint1=samplePoints.loop1- samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}
}
void AudioPlayMemory::setLP2(uint16_t value) // w audio engine zadba zeby zapodac odpowiednia wartosc gdy force
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

void AudioPlayMemory::setTuneForceFlag()
{
	tuneForceFlag = 1;
}
void AudioPlayMemory::clearTuneForceFlag()
{
	tuneForceFlag = 0;
}
void AudioPlayMemory::setForcedTune(int8_t value)
{
	forcedTune = value;
}

void AudioPlayMemory::setPointsForceFlag()
{
	pointsForceFlag = 1;
}
void AudioPlayMemory::clearPointsForceFlag()
{
	pointsForceFlag = 0;
}
void AudioPlayMemory::setGlideForceFlag()
{
	glideForceFlag = 1;
}
void AudioPlayMemory::clearGlideForceFlag()
{
	glideForceFlag = 0;
}

void AudioPlayMemory::setForcedGlide(uint16_t value)
{
	forcedGlide = value;
}

void AudioPlayMemory::setFineTuneForceFlag()
{
	fineTuneForceFlag = 1;
}
void AudioPlayMemory::clearFineTuneForceFlag()
{
	fineTuneForceFlag = 0;
}
void AudioPlayMemory::setForcedFineTune(int8_t value)
{
	forcedFineTune = value;
}

void AudioPlayMemory::setForcedPoints(int32_t sp, int32_t lp1, int32_t lp2, int32_t ep)
{
	if(sp != -1) forcedStartPoint = sp;
	else forcedStartPoint = mtProject.instrument[currentInstr_idx].startPoint;
	if(lp1 != -1) forcedLoopPoint1 = lp1;
	else forcedLoopPoint1 = mtProject.instrument[currentInstr_idx].loopPoint1;
	if(lp2 != -1) forcedLoopPoint2 = lp2;
	else forcedLoopPoint2 = mtProject.instrument[currentInstr_idx].loopPoint2;
	if(ep != -1) forcedEndPoint = ep;
	else forcedEndPoint = mtProject.instrument[currentInstr_idx].endPoint;
}

void AudioPlayMemory::setWavetableWindowFlag()
{
	wavetableWindowForceFlag = 1;
}
void AudioPlayMemory::clearWavetableWindowFlag()
{
	wavetableWindowForceFlag = 0;
}
void AudioPlayMemory::setForcedWavetableWindow(int16_t val)
{
	if(val > MAX_WAVETABLE_WINDOW) forcedWavetableWindow = MAX_WAVETABLE_WINDOW;
	else if(val < 0) forcedWavetableWindow = 0;
	else forcedWavetableWindow=val;
}

void AudioPlayMemory::setReverse()
{
	reverseDirectionFlag = 1;
}
void AudioPlayMemory::clearReverse()
{
	reverseDirectionFlag = 0;
}

uint16_t AudioPlayMemory::getPosition()
{
	return  (uint16_t)(65535 * ((samplePoints.start + iPitchCounter)/(float)startLen));
}

void AudioPlayMemory::clean(void)
{
	if(!playing)
	{
		next=NULL;
		beginning=NULL;
		length=0;
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
	}

}
uint8_t AudioPlayMemory::playForPrev(uint8_t instr_idx,int8_t n)
{
	__disable_irq();
	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	if(playing == 0x81) needSmoothingFlag = 1;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;
	currentInstr_idx=instr_idx;
	lastNote= - 1;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
	glide=mtProject.instrument[instr_idx].glide;
	currentTune=mtProject.instrument[instr_idx].tune;

	if( (n + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>n) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-n;
	}
	if( (n + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<n)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-n;
	}

	if(lastNote>=0 && glide != 0 ) pitchControl=notes[lastNote + currentTune];
	else pitchControl=notes[n+ currentTune];

	int16_t * data = mtProject.instrument[instr_idx].sample.address;

	playMode=mtProject.instrument[instr_idx].playMode;

	startLen=mtProject.instrument[instr_idx].sample.length;
	sampleType = mtProject.instrument[instr_idx].sample.type;


	if(sampleType != mtSampleTypeWavetable)
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
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		currentWindow=mtProject.instrument[instr_idx].wavetableCurrentWindow;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
	if(sampleType != mtSampleTypeWavetable)
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
		if((n + mtProject.instrument[instr_idx].tune + 1) <= MAX_NOTE)
		{
			fineTuneControl= mtProject.instrument[instr_idx].fineTune * ((notes[n + currentTune + 1] - notes[n + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((n + mtProject.instrument[instr_idx].tune - 1) >= MIN_NOTE)
		{
			fineTuneControl= (0 - mtProject.instrument[instr_idx].fineTune) * ((notes[n + currentTune - 1] - notes[n + currentTune] )/MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}

	pitchControl+=fineTuneControl;

	if(glide)
	{
		sampleConstrains.glide=(uint32_t)((float)glide*44.1);
		if((lastNote>=0) && (lastNote != n)) glideControl=(notes[n + currentTune]-notes[lastNote + currentTune] )/sampleConstrains.glide;
		else glideControl=0;
	}
	else
	{
		sampleConstrains.glide=0;
		glideControl=0;
	}



	if(sampleType != mtSampleTypeWavetable)
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



uint8_t AudioPlayMemory::playForPrev(int16_t * addr,uint32_t len, uint8_t type)
{
	uint32_t startPoint,endPoint;
	int8_t note=60;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;

	glide=0;
	currentTune=0;
	lastNote=-1;
	sampleType = type;
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

	if(sampleType == mtSampleTypeWavetable)
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=0;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}


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



uint8_t AudioPlayMemory::playForPrev(int16_t * addr,uint32_t len, uint8_t n, uint8_t type)
{
	uint32_t startPoint,endPoint;
	int8_t note=n;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;

	glide=0;
	currentTune=0;
	lastNote=-1;
	sampleType = type;
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
	if(sampleType == mtSampleTypeWavetable)
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=0;;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}

	samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
	samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
	sampleConstrains.endPoint=samplePoints.end- samplePoints.start;

	next = data+samplePoints.start;
	beginning = data+samplePoints.start;
	length =startLen-samplePoints.start;
	playing = 0x81;

	return successInit;

}
