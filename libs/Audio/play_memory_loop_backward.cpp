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
						glideCounter++;
					}

				}
//*************************************************************************************** poczatek przetwarzania pitchCountera
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = currentSampelValue + (int32_t)(fPitchCounter * interpolationDif);

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
//*************************************************************************************** koniec przetwarzania pitchCountera
				if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
//*************************************************************************************** warunki pointow
				if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
				{
					iPitchCounter = constrainsInSamples.loopPoint2;
					loopBackwardFlag = 1;
					fPitchCounter = 0;
				}
				if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
				{
					iPitchCounter = constrainsInSamples.loopPoint2;
					fPitchCounter = 0;
				}
			//*************************************************************************************** koniec warunków pointow
			//*************************************************************************************** warunki pętli
				if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)))
				{
					iPitchCounter = length;
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
//*************************************************************************************** koniec przetwarzania pitchCountera
				if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
//*************************************************************************************** warunki pointow
				if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
				{
					iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
					loopBackwardFlag = 1;
					fPitchCounter = 0;
				}
				if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
				{
					iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
					fPitchCounter = 0;
				}
			//*************************************************************************************** koniec warunków pointow
			//*************************************************************************************** warunki pętli
				if((iPitchCounter - castPitchControl) <= 0)
				{
					iPitchCounter = 0;
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
