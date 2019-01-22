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



uint8_t AudioPlayMemory::play(strStep * step,strMtModAudioEngine * mod)
{

	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	playing = 0;
	prior = 0;
	stopLoop=0;
	loopBackwardFlag=0;
	pitchCounter=0;
	glideCounter=0;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI STEP========================================================*/
	glide=mtProject.instrument[step->instrumentIndex].glide;

	if(lastNote>=0) pitchControl=notes[lastNote];
	else pitchControl=notes[step->note];

	int16_t * data = mtProject.sampleBank.sample[mtProject.instrument[step->instrumentIndex].sampleIndex].address;

	playMode=mtProject.instrument[step->instrumentIndex].playMode;

	startLen=mtProject.sampleBank.sample[mtProject.instrument[step->instrumentIndex].sampleIndex].length;

	startPoint=mtProject.instrument[step->instrumentIndex].startPoint;
	endPoint=mtProject.instrument[step->instrumentIndex].endPoint;

	if(playMode != singleShot) //loopMode
	{
		loopPoint1=mtProject.instrument[step->instrumentIndex].loopPoint1;
		loopPoint2=mtProject.instrument[step->instrumentIndex].loopPoint2;
	}
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI MOD=========================================================*/
	if((mod->startPointMod == relativeMod) && (mod->startPoint)) startPoint += mod->startPoint;
	else if(mod->startPointMod == globalMod) startPoint = mod->startPoint;

	if((mod->endPointMod == relativeMod) && (mod->endPoint)) endPoint += mod->endPoint;
	else if(mod->endPointMod == globalMod) endPoint=mod->endPoint;

	if(playMode != singleShot)
	{
		if((mod->loopPoint1Mod == relativeMod) && (mod->loopPoint1)) loopPoint1 += mod->loopPoint1;
		else if(mod->loopPoint1Mod == globalMod) loopPoint1 = mod->loopPoint1;

		if((mod->loopPoint2Mod == relativeMod) && (mod->loopPoint2)) loopPoint2 += mod->loopPoint2;
		else if(mod->loopPoint2Mod == globalMod) loopPoint2 = mod->loopPoint2;
	}

	if((mod->glideMod == relativeMod) && (mod->glide)) glide+=mod->glide;
	else if(mod->glideMod == globalMod) glide=mod->glideMod;

	if(mod->pitchCtrl) pitchControl += mod->pitchCtrl;
	if(pitchControl < MIN_PITCH) pitchControl = MIN_PITCH;
	if(pitchControl > MAX_PITCH) pitchControl = MAX_PITCH;
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/

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
	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/
	sampleConstrains.glide=(uint32_t)(glide*44.1);
	if((lastNote>=0) && (lastNote != step->note)) glideControl=(notes[step->note]-notes[lastNote] )/sampleConstrains.glide;
	else glideControl=0;

	lastNote=step->note;

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

/*===========================================================================================================================*/
/*============================================PRZEKAZANIE PARAMETROW=========================================================*/
	next = data+samplePoints.start;
	beginning = data+samplePoints.start;
	length =startLen-samplePoints.start;

	playing = 0x81;

	return successInit;

}

uint8_t AudioPlayMemory:: play(strInstrument *instr,strMtModAudioEngine * mod, uint8_t vol, int8_t note )
{
	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;

	playing = 0;
	prior = 0;
	stopLoop=0;
	pitchCounter=0;
	glideCounter=0;
	loopBackwardFlag=0;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI INSTR=======================================================*/
	int16_t * data = mtProject.sampleBank.sample[instr->sampleIndex].address;

	playMode=instr->playMode;

	startLen=mtProject.sampleBank.sample[instr->sampleIndex].length;

	if(lastNote>=0) pitchControl=notes[lastNote];
	else pitchControl=notes[note];

	glide=instr->glide;

	startPoint=instr->startPoint;
	endPoint=instr->endPoint;

	if(playMode != singleShot) //loopMode
	{
		loopPoint1=instr->loopPoint1;
		loopPoint2=instr->loopPoint2;
	}
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI MOD=========================================================*/
	if((mod->startPointMod == relativeMod) && (mod->startPoint)) startPoint += mod->startPoint;
	else if(mod->startPointMod == globalMod) startPoint = mod->startPoint;

	if((mod->endPointMod == relativeMod) && (mod->endPoint)) endPoint += mod->endPoint;
	else if(mod->endPointMod == globalMod) endPoint = mod->endPoint;

	if(playMode != singleShot)
	{
		if((mod->loopPoint1Mod == relativeMod) && (mod->loopPoint1)) loopPoint1 += mod->loopPoint1;
		else if(mod->loopPoint1Mod == globalMod) loopPoint1 = mod->loopPoint1;

		if((mod->loopPoint2Mod == relativeMod) && (mod->loopPoint2)) loopPoint2 += mod->loopPoint2;
		else if(mod->loopPoint2Mod == globalMod) loopPoint2 = mod->loopPoint2;
	}

	if((mod->glideMod == relativeMod) && (mod->glide)) glide+=mod->glide;
	else if(mod->glideMod == globalMod) glide=mod->glideMod;
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
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
	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/

	sampleConstrains.glide=(uint32_t)(glide*44.1);
	if((lastNote>=0) && (lastNote != note)) glideControl=(notes[note]-notes[lastNote]  )/sampleConstrains.glide;
	else glideControl=0;

	lastNote=note;

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

		for (i=0; i < AUDIO_BLOCK_SAMPLES; i ++)
		{

			if (length > (uint32_t)pitchCounter) //if (length > 0)
			{

				if(glideCounter<=sampleConstrains.glide) pitchControl+=glideControl;
				glideCounter++;

				if((playMode == singleShot) ||(playMode == loopForward))
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					pitchCounter+=pitchControl;

					if(playMode == loopForward)
					{
						if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) ) pitchCounter = sampleConstrains.loopPoint1 ;
					}
				}
				else if(playMode == loopBackward)
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					if(!loopBackwardFlag) pitchCounter+=pitchControl;
					else pitchCounter-=pitchControl;

					if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) && (!loopBackwardFlag) ) loopBackwardFlag=1;
					if(( (uint32_t)pitchCounter  <= sampleConstrains.loopPoint1) && (!stopLoop) && loopBackwardFlag ) pitchCounter = sampleConstrains.loopPoint2 ;

				}
				else if(playMode == loopPingPong)
				{
					*out++ = *(in+(uint32_t)pitchCounter);
					if(!loopBackwardFlag) pitchCounter+=pitchControl;
					else pitchCounter-=pitchControl;


					if(( (uint32_t)pitchCounter  >= sampleConstrains.loopPoint2) && (!stopLoop) && (!loopBackwardFlag) ) loopBackwardFlag=1;
					if(( (uint32_t)pitchCounter  <= sampleConstrains.loopPoint1) && (!stopLoop) && loopBackwardFlag ) loopBackwardFlag=0;

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

uint8_t AudioPlayMemory::setMod(strStep * step,strMtModAudioEngine * mod)
{
	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI STEP========================================================*/
	startPoint=mtProject.instrument[step->instrumentIndex].startPoint;
	endPoint=mtProject.instrument[step->instrumentIndex].endPoint;

	if(playMode != singleShot) //loopMode
	{
		loopPoint1=mtProject.instrument[step->instrumentIndex].loopPoint1;
		loopPoint2=mtProject.instrument[step->instrumentIndex].loopPoint2;
	}

	glide=mtProject.instrument[step->instrumentIndex].glide;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI MOD=========================================================*/
	if((mod->startPointMod == relativeMod) && (mod->startPoint)) startPoint += mod->startPoint;
	else if(mod->startPointMod == globalMod) startPoint = mod->startPoint;

	if((mod->endPointMod == relativeMod) && (mod->endPoint)) endPoint += mod->endPoint;
	else if(mod->endPointMod == globalMod) endPoint = mod->endPoint;

	if(playMode != singleShot) //loopMode
	{
		if((mod->loopPoint1Mod == relativeMod) && (mod->loopPoint1)) loopPoint1 += mod->loopPoint1;
		else if(mod->loopPoint1Mod == globalMod) loopPoint1 = mod->loopPoint1;

		if((mod->loopPoint2Mod == relativeMod) && (mod->loopPoint2)) loopPoint2 += mod->loopPoint2;
		else if(mod->loopPoint2Mod == globalMod) loopPoint2 = mod->loopPoint2;
	}

	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
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
	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/
	sampleConstrains.glide=(uint32_t)(glide*44.1);
	if((lastNote>=0) && (lastNote != step->note)) glideControl=(notes[step->note] - notes[lastNote] )/sampleConstrains.glide;
	else glideControl=0;

	lastNote=step->note;

	if(mod->pitchCtrl) pitchControl+=mod->pitchCtrl;
	if(pitchControl < MIN_PITCH) pitchControl=MIN_PITCH;
	if(pitchControl > MAX_PITCH ) pitchControl=MAX_PITCH;

	sampleConstrains.glide=(uint32_t)(glide*44.1);
	if(lastNote>=0) glideControl=(notes[step->note] - notes[lastNote])/sampleConstrains.glide;

	samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
	samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
	if(playMode == 1)
	{
		samplePoints.loop1= (uint32_t)((float)loopPoint1*((float)startLen/MAX_16BIT));
		samplePoints.loop2= (uint32_t)((float)loopPoint2*((float)startLen/MAX_16BIT));
	}



	if((samplePoints.start >= startLen) || (samplePoints.loop1>startLen) || (samplePoints.loop2>startLen) || (samplePoints.end>startLen)) return pointsBeyondFile; // wskazniki za plikiem


	if(playMode != singleShot)
	{
		sampleConstrains.loopPoint1=samplePoints.loop1;
		sampleConstrains.loopPoint2=samplePoints.loop2;
		sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}

	sampleConstrains.endPoint= samplePoints.end;

	return successInit;
}

uint8_t AudioPlayMemory::setMod(strInstrument * instr,strMtModAudioEngine * mod, int8_t note)
{
	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI INSTR=======================================================*/
	startPoint=instr->startPoint;
	endPoint=instr->endPoint;

	if(playMode != singleShot) //loopMode
	{
		loopPoint1=instr->loopPoint1;
		loopPoint2=instr->loopPoint2;
	}
	playMode=instr->playMode;
	glide=instr->glide;
	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI MOD=========================================================*/
	if((mod->startPointMod == relativeMod) && (mod->startPoint)) startPoint += mod->startPoint;
	else if(mod->startPointMod == globalMod) startPoint = mod->startPoint;

	if((mod->endPointMod == relativeMod) && (mod->endPoint)) endPoint += mod->endPoint;
	else if(mod->endPointMod == globalMod) endPoint = mod->endPoint;

	if(playMode != singleShot) //loopMode
	{
		if((mod->loopPoint1Mod == relativeMod) && (mod->loopPoint1)) loopPoint1 += mod->loopPoint1;
		else if(mod->loopPoint1Mod == globalMod) loopPoint1 = mod->loopPoint1;

		if((mod->loopPoint2Mod == relativeMod) && (mod->loopPoint2)) loopPoint2 += mod->loopPoint2;
		else if(mod->loopPoint2Mod == globalMod) loopPoint2 = mod->loopPoint2;
	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
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
	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/
	if((mod->glideMod == relativeMod) && (mod->glide)) glide+=mod->glide;
	else if(mod->glideMod == globalMod) glide=mod->glideMod;

	pitchControl+=mod->pitchCtrl;
	if(pitchControl < MIN_PITCH) pitchControl=MIN_PITCH;
	if(pitchControl > MAX_PITCH ) pitchControl=MAX_PITCH;

	sampleConstrains.glide=(uint32_t)(glide*44.1);
	if((lastNote>=0) && (lastNote != note)) glideControl=(notes[note] - notes[lastNote])/sampleConstrains.glide;
	else glideControl=0;

	lastNote=note;

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
		sampleConstrains.loopPoint1=samplePoints.loop1;
		sampleConstrains.loopPoint2=samplePoints.loop2;
		sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}

	sampleConstrains.endPoint= samplePoints.end;

	return successInit;
}


