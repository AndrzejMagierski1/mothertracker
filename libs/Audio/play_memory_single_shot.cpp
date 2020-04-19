#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playSingleShot(uint8_t instrIdx, int8_t note)
{
	__disable_irq();
	AudioNoInterrupts();

	if( instrIdx > INSTRUMENTS_MAX ) instrIdx = INSTRUMENTS_MAX;
	if( note > MAX_NOTE ) note = MAX_NOTE;

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
		__enable_irq();
		AudioInterrupts();
		return;
	}

	pointsInSamples.start= (uint32_t)( (float)startPoint * ( (float)currentSampleLength / MAX_16BIT) );
	pointsInSamples.end= (uint32_t)((float)endPoint * ( (float)currentSampleLength / MAX_16BIT) );
	pointsInSamples.loop1 = 0;
	pointsInSamples.loop2 = 0;

	if((pointsInSamples.start >= currentSampleLength) || (pointsInSamples.end > currentSampleLength))
	{
		__enable_irq();
		AudioInterrupts();
		return;
	}

	constrainsInSamples.endPoint = pointsInSamples.end - pointsInSamples.start;


	next = currentStartAddress + pointsInSamples.start;
	length = currentSampleLength - pointsInSamples.start;
	iPitchCounter = reverseDirectionFlag ? constrainsInSamples.endPoint - 1 : 0;


	playing = 1;
	__enable_irq();
	AudioInterrupts();
}

void AudioPlayMemory::updateSingleShot()
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
			bool dataIsEnable = reverseDirectionFlag ? ((int32_t)iPitchCounter > 0) : (length > iPitchCounter);

			if(dataIsEnable)
			{
				//*********************************** GLIDE HANDLE
				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) ((reverseDirectionFlag) ?  -pitchControl : pitchControl);
						pitchFraction = (float) ((reverseDirectionFlag) ?  - (pitchControl - (int32_t)pitchControl) : (pitchControl - (int32_t)pitchControl));
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
							*out++ = ( (int32_t)( ( (int32_t)( (*(in + iPitchCounter)) * j) + (int32_t)(lastSample * (SMOOTHING_SIZE - 1 - j) ))) / (SMOOTHING_SIZE - 1));
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

				if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)) && !reverseDirectionFlag)
				{
					iPitchCounter = length;
				}
				else if (((int32_t)(iPitchCounter - castPitchControl) <= 0)  && (reverseDirectionFlag))
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



