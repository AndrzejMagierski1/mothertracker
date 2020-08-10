#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playLoopPingpong(uint8_t instrIdx, int8_t note)
{
	playLoopForward(instrIdx, note);
}

audio_block_t * AudioPlayMemory::updateLoopPingpong()
{
	if(reverseDirectionFlag) return updateLoopPingPongReverse();
	else return updateLoopPingPongNormal();
}

audio_block_t * AudioPlayMemory::updateLoopPingPongNormal()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;

	int32_t loopStartPoint = max((int32_t)constrainsInSamples.loopPoint1, 0);
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

		castPitchControl = (int32_t)pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		interpolationCondition = ((pitchControl  < 1.0f) && (( (iPitchCounter + 128 * pitchControl) < length))) ? 0: 1;
		int16_t * in_interpolation = loopBackwardFlag ? in-1: in+1;
		int32_t currentInterpolationFraction;

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

				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				currentInterpolationFraction =  (currentFractionPitchCounter < 0) ? -currentFractionPitchCounter : currentFractionPitchCounter;

				*out++ = (int32_t)currentSampelValue + (int32_t) (((currentInterpolationFraction >> 2) * interpolationDif) >> 14 );

				if (!loopBackwardFlag)
				{
					iPitchCounter += castPitchControl;
					currentFractionPitchCounter += currentFractionPitchControl;
					if (currentFractionPitchCounter >= MAX_16BIT)
					{
						currentFractionPitchCounter -= MAX_16BIT;
						iPitchCounter++;
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
				}
//*************************************************************************************** warunki pointow
				if ((iPitchCounter <= loopStartPoint) && loopBackwardFlag)
				{
					iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 + 1 : 1;
					loopBackwardFlag = 0;
					in_interpolation = in+1;
					currentFractionPitchCounter = 0;
				}
				else if (iPitchCounter >= loopEndPoint)
				{
					iPitchCounter = constrainsInSamples.loopPoint2 - 1;
					loopBackwardFlag = 1;
					in_interpolation = in-1;
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
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}
audio_block_t * AudioPlayMemory::updateLoopPingPongReverse()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;
	int32_t loopStartPoint = max((int32_t)constrainsInSamples.loopPoint1, 0);
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

		castPitchControl = (int32_t) -pitchControl;
		pitchFraction = - (pitchControl - (int32_t)pitchControl);

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		interpolationCondition = ((pitchControl  < 1.0f) && (((int)(iPitchCounter - 128 * pitchControl) > 0)) ) ? 0: 1;

		int16_t * in_interpolation = loopBackwardFlag ? in+1: in-1;
		int32_t currentInterpolationFraction;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (iPitchCounter > 0)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t)-pitchControl;
						pitchFraction = - (pitchControl - (int32_t)pitchControl);
						currentFractionPitchControl = pitchFraction * MAX_16BIT;
						glideCounter++;
					}

				}
				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				currentInterpolationFraction =  (currentFractionPitchCounter < 0) ? -currentFractionPitchCounter : currentFractionPitchCounter;

				*out++ = (int32_t)(currentSampelValue + (int32_t) (( (currentInterpolationFraction >> 2) * interpolationDif) >> 14 ) );

				if (!loopBackwardFlag)
				{
					iPitchCounter += castPitchControl;
					currentFractionPitchCounter += currentFractionPitchControl;
					if(currentFractionPitchCounter <= -MAX_16BIT)
					{
						currentFractionPitchCounter += MAX_16BIT;
						iPitchCounter--;
					}
				}
				else
				{
					iPitchCounter -= castPitchControl;
					currentFractionPitchCounter -= currentFractionPitchControl;
					if (currentFractionPitchCounter >= MAX_16BIT)
					{
						currentFractionPitchCounter -= MAX_16BIT;
						iPitchCounter++;
					}
				}
//*************************************************************************************** warunki pointow

				if ((iPitchCounter >= loopEndPoint) && loopBackwardFlag)
				{
					iPitchCounter = constrainsInSamples.loopPoint2 - 1;
					loopBackwardFlag = 0;
					currentFractionPitchCounter = 0;
				}
				if (iPitchCounter <= loopStartPoint)
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
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}

