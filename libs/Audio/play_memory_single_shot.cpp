#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playSingleShot(uint8_t instrIdx, int8_t note)
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
		checkFirstUpdate();

		out = block->data;
		in = (int16_t*)next;

		castPitchControl = (int32_t) ((reverseDirectionFlag) ?  -pitchControl : pitchControl);
		pitchFraction = ((reverseDirectionFlag) ?  - (pitchControl - (int32_t)pitchControl) : (pitchControl - (int32_t)pitchControl));

		interpolationCondition = 	   (!((iPitchCounter  < 1.0f) ||
										   (( (iPitchCounter + 128 * pitchControl) < length) && (!reverseDirectionFlag)) ||
										   (((int)(iPitchCounter - 128 * pitchControl) > 0) && (reverseDirectionFlag)) )) ? 1: 0;
		int16_t * in_interpolation = reverseDirectionFlag ? in-1: in+1;

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

				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				*out++ = currentSampelValue + (int32_t)(fPitchCounter * interpolationDif);
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



