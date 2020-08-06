#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

//Play dla wszystkich trybow loop jest wspolny stad tylko tu pelna implementacja a w innych wywolanie
void AudioPlayMemory::playLoopForward(uint8_t instrIdx, int8_t note)
{
	__disable_irq();
	AudioNoInterrupts();

	if( instrIdx > INSTRUMENTS_MAX ) instrIdx = INSTRUMENTS_MAX;
	if( note > MAX_NOTE ) note = MAX_NOTE;

	lastInstrIdxInPlay = currentInstrIdxInPlay;
	currentInstrIdxInPlay = instrIdx;
	currentInstrIdx = instrIdx;

	refreshStartParamiters();

	constrainCurrentTune(note);

	setStartPitch(note);

	applyFinetuneOnPitch(note);

	calculateGlidePitch(note);

	lastNote = note;

	uint16_t startPoint, endPoint, loopPoint1, loopPoint2;

	if(pointsForceFlag)
	{
		startPoint=forcedStartPoint;
		endPoint=forcedEndPoint;
		loopPoint1=forcedLoopPoint1;
		loopPoint2=forcedLoopPoint2;
	}
	else
	{
		startPoint=mtProject.instrument[instrIdx].startPoint;
		endPoint=mtProject.instrument[instrIdx].endPoint;
		loopPoint1=mtProject.instrument[instrIdx].loopPoint1;
		loopPoint2=mtProject.instrument[instrIdx].loopPoint2;
	}


	if ( (startPoint >= endPoint)  || (startPoint > loopPoint1) ||  (startPoint > loopPoint2) ||
		 (loopPoint1 > loopPoint2) || (loopPoint1 > endPoint)   ||  (loopPoint2 > endPoint))
	{
		__enable_irq();
		AudioInterrupts();
		return;
	}


	pointsInSamples.start= (uint32_t)( (float)startPoint * ((float)currentSampleLength / MAX_16BIT));
	pointsInSamples.end= (uint32_t)( (float)endPoint * ((float)currentSampleLength / MAX_16BIT));
	pointsInSamples.loop1= (uint32_t)( (float)loopPoint1 * ((float)currentSampleLength / MAX_16BIT));
	pointsInSamples.loop2= (uint32_t)( (float)loopPoint2 * ((float)currentSampleLength / MAX_16BIT));


	if((pointsInSamples.start >= currentSampleLength) || (pointsInSamples.loop1 > currentSampleLength) ||
	   (pointsInSamples.loop2 > currentSampleLength)  || (pointsInSamples.end > currentSampleLength))
	{
		__enable_irq();
		AudioInterrupts();
		return;
	}


	constrainsInSamples.loopPoint1 = pointsInSamples.loop1 - pointsInSamples.start;
	constrainsInSamples.loopPoint2 = pointsInSamples.loop2 - pointsInSamples.start;
	constrainsInSamples.loopLength = pointsInSamples.loop2 - pointsInSamples.loop1;
	constrainsInSamples.endPoint = pointsInSamples.end - pointsInSamples.start;

	next = currentStartAddress + pointsInSamples.start;
	length = currentSampleLength - pointsInSamples.start;
	iPitchCounter = reverseDirectionFlag ? constrainsInSamples.endPoint - 1 : 0;


	playing = 1;
	__enable_irq();
	AudioInterrupts();
}

audio_block_t * AudioPlayMemory::updateLoopForward()
{
	if(reverseDirectionFlag) return updateLoopForwardReverse();
	else return updateLoopForwardNormal();
}

audio_block_t * AudioPlayMemory::updateLoopForwardNormal()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;
	int32_t loopEndPoint = min((int32_t)constrainsInSamples.loopPoint2, (int32_t)min(length, constrainsInSamples.endPoint));

	block = allocate();
	if (!block) return nullptr;

	if (!playing)
	{
		release(block);
		return nullptr;
	}
	else if (playing == 1)
	{
		out = block->data;
		in = (int16_t*)next;

		castPitchControl =  pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		interpolationCondition = ((pitchControl  < 1.0f) && (( (iPitchCounter + 128 * pitchControl) < length))) ? 0: 1;
		int16_t * in_interpolation = in+1;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if ((int32_t)length > iPitchCounter)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) pitchControl;
						pitchFraction = pitchControl - (int32_t)pitchControl;

						currentFractionPitchControl = pitchFraction * MAX_16BIT;

						glideCounter++;
					}
				}
				//***********************************************
//*************************************************************************************** poczatek przetwarzania pitchCountera
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = currentSampelValue + (int32_t)(( (currentFractionPitchCounter >> 2) * interpolationDif) >> 14);
				iPitchCounter += castPitchControl;
				currentFractionPitchCounter += currentFractionPitchControl;
				if (currentFractionPitchCounter >= MAX_16BIT)
				{
					currentFractionPitchCounter -= MAX_16BIT;
					iPitchCounter++;
				}
//*************************************************************************************** koniec przetwarzania pitchCountera
//*************************************************************************************** warunki pointow
				if (iPitchCounter >= loopEndPoint)
				{
					iPitchCounter = constrainsInSamples.loopPoint1;
					currentFractionPitchCounter = 0;
				}
			//*************************************************************************************** warunki pętli
			}
			else
			{
				*out++ = 0;
				playing = 0;
			}
		}
		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}
audio_block_t * AudioPlayMemory::updateLoopForwardReverse()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;
	int32_t loopStartPoint = max((int32_t)constrainsInSamples.loopPoint1, 0);

	block = allocate();
	if (!block) return nullptr;

	if (!playing)
	{
		release(block);
		return nullptr;
	}
	else if (playing == 1)
	{
		out = block->data;
		in = (int16_t*)next;

		castPitchControl = (int32_t) -pitchControl;
		pitchFraction = - (pitchControl - (int32_t)pitchControl);

		interpolationCondition = ((pitchControl  < 1.0f) && ((int)(iPitchCounter - 128 * pitchControl) > 0)) ? 0: 1;
		int16_t * in_interpolation = in-1;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if ( iPitchCounter > 0 )
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t)-pitchControl;
						pitchFraction =  - (pitchControl - (int32_t)pitchControl);
						currentFractionPitchControl = pitchFraction * MAX_16BIT;

						glideCounter++;
					}
				}
				//***********************************************
//*************************************************************************************** poczatek przetwarzania pitchCountera
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = (int32_t)currentSampelValue + (int32_t)((((-currentFractionPitchCounter) >> 2) * interpolationDif) >> 14);
				iPitchCounter += castPitchControl;
				currentFractionPitchCounter += currentFractionPitchControl;

				if(currentFractionPitchCounter <= -MAX_16BIT)
				{
					currentFractionPitchCounter += MAX_16BIT;
					iPitchCounter--;
				}

//*************************************************************************************** koniec przetwarzania pitchCountera
				if (iPitchCounter <= loopStartPoint)
				{
					iPitchCounter = constrainsInSamples.loopPoint2;
					currentFractionPitchCounter = 0;
				}
			//*************************************************************************************** warunki pętli
			}
			else
			{
				*out++ = 0;
				playing = 0;
			}
		}
		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}

