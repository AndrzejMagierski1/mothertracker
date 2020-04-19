#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playLoopPingpong(uint8_t instrIdx, int8_t note)
{
	playLoopForward(instrIdx, note);
}

void AudioPlayMemory::updateLoopPingpong()
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

		castPitchControl = (int32_t) ((reverseDirectionFlag) ?  -pitchControl : pitchControl);
		pitchFraction = ((reverseDirectionFlag) ?  - (pitchControl - (int32_t)pitchControl) : (pitchControl - (int32_t)pitchControl));

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			bool dataIsEnable = reverseDirectionFlag ? (iPitchCounter > 0) : (length > iPitchCounter);

			if (dataIsEnable)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) ((reverseDirectionFlag) ?  -pitchControl : pitchControl);
						pitchFraction = ((reverseDirectionFlag) ?  - (pitchControl - (int32_t)pitchControl) : (pitchControl - (int32_t)pitchControl));
						glideCounter++;
					}

				}
				//***********************************************
				//*********************************** SMOOTHING HANDLE
				// needSmoothingFlag ustawiana jest na play gdy jest aktywne odtwarzanie, więc iPitchCounter się zeruje, ale na wysokim pitchu i bardzo krotkich loopach
				// moga zostac przekroczone wartosci graniczne - trzeba je obsluzyc
				if(needSmoothingFlag && (i == 0))
				{
					needSmoothingFlag = 0;
					for(uint8_t j = 0; j < SMOOTHING_SIZE; j++ )
					{
						if(iPitchCounter <= length)
						{
							//srednia wazona miedzy ostatnia probka z poprzedniego pliku a nowymi probkami
							*out++ = ( (int32_t)( ( (int32_t)( (*(in + iPitchCounter)) * j) + (int32_t)(lastSample * (SMOOTHING_SIZE - 1 - j) ) ) )/(SMOOTHING_SIZE - 1));
						}
						else
						{
							*out++ = 0;
						}

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

						if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;

						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
							{
								iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
							{
								iPitchCounter = constrainsInSamples.loopPoint2;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
							{
								iPitchCounter = constrainsInSamples.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
							{
								iPitchCounter = constrainsInSamples.loopPoint1;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}
					}
					if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)) && !reverseDirectionFlag)
					{
						iPitchCounter = length;
					}
					else if (((iPitchCounter - castPitchControl) <= 0)  && (reverseDirectionFlag))
					{
						iPitchCounter = 0;
					}
					i = SMOOTHING_SIZE;
				}
				//***********************************************
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

				if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
//*************************************************************************************** warunki pointow
				if(reverseDirectionFlag)
				{
					if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
					{
						iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
						loopBackwardFlag = 1;
						fPitchCounter = 0;
					}
					if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
					{
						iPitchCounter = constrainsInSamples.loopPoint2;
						loopBackwardFlag = 0;
						fPitchCounter = 0;
					}
				}
				else
				{
					if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
					{
						iPitchCounter = constrainsInSamples.loopPoint2;
						loopBackwardFlag = 1;
						fPitchCounter = 0;
					}
					if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
					{
						iPitchCounter = constrainsInSamples.loopPoint1;
						loopBackwardFlag = 0;
						fPitchCounter = 0;
					}
				}
//*******************************************************************
				if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)) && !reverseDirectionFlag)
				{
					iPitchCounter = length;
				}
				else if (((iPitchCounter - castPitchControl) <= 0)  && (reverseDirectionFlag))
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

		transmit(block);
	}
	release(block);
}

