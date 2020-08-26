#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playSingleShot(uint8_t instrIdx, int8_t note)
{
//	__disable_irq();
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

	uint16_t startPoint = pointsForceFlag ? forcedStartPoint : mtProject.instrument[currentInstrIdx].startPoint;
	uint16_t endPoint = pointsForceFlag ? forcedEndPoint : mtProject.instrument[currentInstrIdx].endPoint;

	if(startPoint >= endPoint)
	{
//		__enable_irq();
		AudioInterrupts();
		return;
	}

	pointsInSamples.start= (uint32_t)( (float)startPoint * ( (float)currentSampleLength / MAX_16BIT) );
	pointsInSamples.end= (uint32_t)((float)endPoint * ( (float)currentSampleLength / MAX_16BIT) );
	pointsInSamples.loop1 = 0;
	pointsInSamples.loop2 = 0;

	if((pointsInSamples.start >= currentSampleLength) || (pointsInSamples.end > currentSampleLength))
	{
//		__enable_irq();
		AudioInterrupts();
		return;
	}

	constrainsInSamples.endPoint = pointsInSamples.end - pointsInSamples.start;


	next = currentStartAddress + pointsInSamples.start;
	length = currentSampleLength - pointsInSamples.start;
	iPitchCounter = reverseDirectionFlag ? constrainsInSamples.endPoint - 1 : 0;


	playing = 1;

//	__enable_irq();
	AudioInterrupts();
}

audio_block_t * AudioPlayMemory::updateSingleShot()
{
	if(reverseDirectionFlag) return updateSingleShotReverse();
	else return updateSingleShotNormal();
}

audio_block_t * AudioPlayMemory::updateSingleShotNormal()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;

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

		castPitchControl = (int32_t) pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		if(enableInterpolation) interpolationCondition = ((iPitchCounter + 128 * pitchControl) < length) ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation = in+1;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if((int32_t)length > iPitchCounter)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t)pitchControl;
						pitchFraction =  pitchControl - (int32_t)pitchControl;

						currentFractionPitchControl = pitchFraction * MAX_16BIT;

						glideCounter++;
					}
				}
				//***********************************************
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

				if (iPitchCounter >= (int32_t)(constrainsInSamples.endPoint))
				{
					iPitchCounter = length;
				}
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
audio_block_t * AudioPlayMemory::updateSingleShotReverse()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;

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
		pitchFraction =  - (pitchControl - (int32_t)pitchControl);

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		if(enableInterpolation) interpolationCondition = (((int)(iPitchCounter - 128 * pitchControl) > 0)) ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation = in-1;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if((int32_t)iPitchCounter > 0)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t)-pitchControl;
						pitchFraction = -(pitchControl - (int32_t)pitchControl);

						currentFractionPitchControl = pitchFraction * MAX_16BIT;

						glideCounter++;
					}
				}
				//***********************************************
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = currentSampelValue + (int32_t)(( ( (-currentFractionPitchCounter) >> 2) * interpolationDif) >> 14);
				iPitchCounter += castPitchControl;
				currentFractionPitchCounter += currentFractionPitchControl;

				if(currentFractionPitchCounter <= -MAX_16BIT)
				{
					currentFractionPitchCounter += MAX_16BIT;
					iPitchCounter--;
				}

				if ((int32_t)(iPitchCounter - castPitchControl) <= 0)
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
		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;
		return block;
	}
}


