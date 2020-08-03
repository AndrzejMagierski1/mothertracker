#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playLoopBackward(uint8_t instrIdx, int8_t note)
{
	playLoopForward(instrIdx, note);
}

void AudioPlayMemory::updateLoopBackward()
{
	if(reverseDirectionFlag) updateLoopBackwardReverse();
	else updateLoopBackwardNormal();
}

void AudioPlayMemory::updateLoopBackwardNormal()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;

	uint32_t loopStartPoint = max(constrainsInSamples.loopPoint1, 0U);
	uint32_t loopEndPoint = min(constrainsInSamples.loopPoint2, min(length, constrainsInSamples.endPoint));

	block = allocate();
	if (!block) return;

	if (!playing)
	{
		release(block);
		return;
	}
	else if (playing == 1)
	{
		out = block->data;
		in = (int16_t*)next;

		castPitchControl = (int32_t)pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		interpolationCondition = ((pitchControl  < 1.0f) || (( (iPitchCounter + 128 * pitchControl) < length))) ? 0: 1;
		int16_t * in_interpolation = in+1;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (length > iPitchCounter)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t)pitchControl;
						pitchFraction = pitchControl - (int32_t)pitchControl;

						currentFractionPitchControl = pitchFraction * MAX_16BIT;

						glideCounter++;
					}

				}
//*************************************************************************************** poczatek przetwarzania pitchCountera
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = (int32_t)(currentSampelValue + (int32_t) (( (currentFractionPitchCounter >> 2) * interpolationDif) >> 14 ) );

				if (!loopBackwardFlag)
				{
					iPitchCounter += castPitchControl;
					currentFractionPitchCounter += currentFractionPitchControl;
					if (currentFractionPitchCounter >= MAX_16BIT)
					{
						currentFractionPitchCounter -= MAX_16BIT;
						iPitchCounter++;
					}
					else if(currentFractionPitchCounter <= -MAX_16BIT)
					{
						currentFractionPitchCounter += MAX_16BIT;
						iPitchCounter--;
					}
				}
				else
				{
					iPitchCounter -= castPitchControl;

					currentFractionPitchCounter -= currentFractionPitchControl;
					if (currentFractionPitchCounter <= -MAX_16BIT)
					{
						currentFractionPitchCounter += MAX_16BIT;
						iPitchCounter--;
					}
					else if (currentFractionPitchCounter >= MAX_16BIT)
					{
						currentFractionPitchCounter -= MAX_16BIT;
						iPitchCounter++;
					}
				}
//*************************************************************************************** koniec przetwarzania pitchCountera
//*************************************************************************************** warunki pointow
				if ((iPitchCounter <= loopStartPoint) && loopBackwardFlag)
				{
					iPitchCounter = constrainsInSamples.loopPoint2;
					currentFractionPitchCounter = 0;
				}
				else if (iPitchCounter >= loopEndPoint)
				{
					iPitchCounter = constrainsInSamples.loopPoint2 - 1;
					loopBackwardFlag = 1;
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
		transmit(block);
	}
	release(block);
}
void AudioPlayMemory::updateLoopBackwardReverse()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;
	uint32_t loopStartPoint = max(constrainsInSamples.loopPoint1, 0);
	uint32_t loopEndPoint = min(constrainsInSamples.loopPoint2, min(length, constrainsInSamples.endPoint));


	block = allocate();
	if (!block) return;

	if (!playing)
	{
		release(block);
		return;
	}
	else if (playing == 1)
	{
		out = block->data;
		in = (int16_t*)next;

		castPitchControl = (int32_t) -pitchControl;
		pitchFraction = -(pitchControl - (int32_t)pitchControl);

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;


		interpolationCondition = ((pitchControl  < 1.0f) || (((int)(iPitchCounter - 128 * pitchControl) > 0)) ) ? 0: 1;
		int16_t * in_interpolation = in-1;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if ((int32_t)iPitchCounter > 0)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) -pitchControl;
						pitchFraction = - (pitchControl - (int32_t)pitchControl);

						currentFractionPitchControl = pitchFraction * MAX_16BIT;
						glideCounter++;
					}
				}
				//***********************************************
//*************************************************************************************** poczatek przetwarzania pitchCountera
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = currentSampelValue + (int32_t)(fPitchCounter * interpolationDif);
				if (!loopBackwardFlag)
				{
					iPitchCounter += castPitchControl;
					currentFractionPitchCounter += currentFractionPitchControl;
					if (currentFractionPitchCounter >= MAX_16BIT)
					{
						currentFractionPitchCounter -= MAX_16BIT;
						iPitchCounter++;
					}
					else if(currentFractionPitchCounter <= -MAX_16BIT)
					{
						currentFractionPitchCounter += MAX_16BIT;
						iPitchCounter--;
					}
				}
				else
				{
					iPitchCounter -= castPitchControl;

					currentFractionPitchCounter -= currentFractionPitchControl;
					if (currentFractionPitchCounter <= -MAX_16BIT)
					{
						currentFractionPitchCounter += MAX_16BIT;
						iPitchCounter--;
					}
					else if (currentFractionPitchCounter >= MAX_16BIT)
					{
						currentFractionPitchCounter -= MAX_16BIT;
						iPitchCounter++;
					}
				}
//*************************************************************************************** koniec przetwarzania pitchCountera
//*************************************************************************************** warunki pointow
				if ((iPitchCounter >= loopEndPoint) && loopBackwardFlag)
				{
					iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
					currentFractionPitchCounter = 0;
				}
				else if (iPitchCounter <= loopStartPoint)
				{
					iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 + 1 : 1;
					loopBackwardFlag = 1;
					currentFractionPitchCounter = 0;
				}
			}
			else
			{
				*out++ = 0;
				playing = 0;
			}

		}
		next = currentStartAddress + pointsInSamples.start;
		transmit(block);
	}
	release(block);
}
