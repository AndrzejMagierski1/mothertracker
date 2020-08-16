#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playGranular(uint8_t instrIdx, int8_t note)
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

	uint16_t granularLength = ( (uint32_t)( (uint32_t)mtProject.instrument[instrIdx].granular.grainLength * (uint32_t)MAX_16BIT ) ) / mtProject.instrument[instrIdx].sample.length;

	int32_t granularDownConstrain;
	int32_t granularUpConstrain;

	if(granularForcedFlag)
	{
		granularDownConstrain = forcedGranularPosition - (granularLength/2);
		granularUpConstrain = forcedGranularPosition + (granularLength/2);
	}
	else
	{
		granularDownConstrain = mtProject.instrument[instrIdx].granular.currentPosition - (granularLength/2);
		granularUpConstrain = mtProject.instrument[instrIdx].granular.currentPosition + (granularLength/2);
	}

	uint16_t startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
	uint16_t loopPoint1Granular = startGranular + 1;
	uint16_t endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
	uint16_t loopPoint2Granular = endGranular - 1;

	granularLoopType = mtProject.instrument[instrIdx].granular.type;
	currentGranularPosition = granularForcedFlag ? forcedGranularPosition : mtProject.instrument[instrIdx].granular.currentPosition;
	switch(mtProject.instrument[instrIdx].granular.shape)
	{
		case granularShapeSquare: 		granularEnvelopeTab = squareTab; 		break;
		case granularShapeGauss: 		granularEnvelopeTab = gaussTab; 		break;
		case granularShapeTriangle: 	granularEnvelopeTab = triangleTab; 		break;
		default: break;
	}

	pointsInSamples.start = (uint32_t)( (float)startGranular * ( (float)currentSampleLength / MAX_16BIT));
	pointsInSamples.end = 	(uint32_t)( (float)endGranular * ( (float)currentSampleLength / MAX_16BIT));

	pointsInSamples.loop1 = (uint32_t)( (float)loopPoint1Granular * ( (float)currentSampleLength / MAX_16BIT));
	pointsInSamples.loop2 = (uint32_t)( (float)loopPoint2Granular * ( (float)currentSampleLength / MAX_16BIT));

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

audio_block_t * AudioPlayMemory::updateGranular()
{
	switch(granularLoopType)
	{
	case granularLoopForward: 		return updateGranularLoopForward();		break;
	case granularLoopBackward: 		return updateGranularLoopBackward();	break;
	case granularLoopPingPong: 		return updateGranularLoopPingPong();	break;
	default:	break;
	}
}

audio_block_t * AudioPlayMemory::updateGranularLoopForward()
{
	if(reverseDirectionFlag) return updateGranularLoopForwardReverse();
	else return updateGranularLoopForwardNormal();
}
audio_block_t * AudioPlayMemory::updateGranularLoopBackward()
{
	loopBackwardFlag = 1;
	if(reverseDirectionFlag) return updateGranularLoopBackwardReverse();
	else return updateGranularLoopBackwardNormal();
}
audio_block_t * AudioPlayMemory::updateGranularLoopPingPong()
{
	if(reverseDirectionFlag) return updateGranularLoopPingPongReverse();
	else return updateGranularLoopPingPongNormal();
}

audio_block_t * AudioPlayMemory::updateGranularLoopForwardNormal()
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

		castPitchControl = (int32_t)pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;

		if(enableInterpolation) interpolationCondition = ( (pitchControl  < 1.0f) && ((iPitchCounter + 128 * pitchControl) < length))  ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation = in+1;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
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
			uint16_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

			currentSampelValue = *(in + iPitchCounter);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);


			//**************************************************************************
			*out++ = ((int32_t)(currentSampelValue + (int32_t) (( (currentFractionPitchCounter >> 2) * interpolationDif) >> 14 ) ) * granularEnvelopeTab[volIndex]) >> 16;

			iPitchCounter += castPitchControl;

			currentFractionPitchCounter += currentFractionPitchControl;
			if (currentFractionPitchCounter >= MAX_16BIT)
			{
				currentFractionPitchCounter -= MAX_16BIT;
				iPitchCounter++;
			}
			//**************************************************************************

//*************************************************************************************** koniec przetwarzania pitchCountera

			//*************************************************************************************** warunki pętli
			if (iPitchCounter >= loopEndPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint1;
				currentFractionPitchCounter = 0;
			}
			//*************************************************************************************** warunki pętli

		}
		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}
audio_block_t * AudioPlayMemory::updateGranularLoopForwardReverse()
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

		if(enableInterpolation) interpolationCondition = ((pitchControl  < 1.0f) && ((int)(iPitchCounter - 128 * pitchControl) > 0)) ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation = in-1;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			//*********************************** GLIDE HANDLE

			if (constrainsInSamples.glide)
			{
				if (glideCounter <= constrainsInSamples.glide)
				{
					pitchControl += glideControl;
					castPitchControl = (int32_t)  -pitchControl;
					pitchFraction =  - (pitchControl - (int32_t)pitchControl);

					currentFractionPitchControl = pitchFraction * MAX_16BIT;

					glideCounter++;
				}
			}
			//***********************************************

//*************************************************************************************** poczatek przetwarzania pitchCountera
			uint16_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

			currentSampelValue = *(in + iPitchCounter);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);


			//**************************************************************************
			*out++ = ((int32_t)(currentSampelValue + (int32_t) (( ( (-currentFractionPitchCounter) >> 2) * interpolationDif) >> 14 ) ) * granularEnvelopeTab[volIndex]) >> 16;

			iPitchCounter += castPitchControl;
			currentFractionPitchCounter += currentFractionPitchControl;

			if(currentFractionPitchCounter <= -MAX_16BIT)
			{
				currentFractionPitchCounter += MAX_16BIT;
				iPitchCounter--;
			}
			//**************************************************************************

//*************************************************************************************** koniec przetwarzania pitchCountera

			//*************************************************************************************** warunki pętli
			if (iPitchCounter <= loopStartPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint2;
				currentFractionPitchCounter = 0;
			}
			//*************************************************************************************** warunki pętli
		}

		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}

}
audio_block_t * AudioPlayMemory::updateGranularLoopBackwardNormal()
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

		castPitchControl = (int32_t) pitchControl;
		pitchFraction =  pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		if(enableInterpolation) interpolationCondition = ((pitchControl  < 1.0f) && (( (iPitchCounter + 128 * pitchControl) < length))) ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation = loopBackwardFlag ? in-1 : in+1;
		int32_t currentInterpolationFraction;
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
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
			//**********************************************
//*************************************************************************************** poczatek przetwarzania pitchCountera
			uint16_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

			currentSampelValue = *(in + iPitchCounter);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

			//**************************************************************************
			currentInterpolationFraction =  (currentFractionPitchCounter < 0) ? -currentFractionPitchCounter : currentFractionPitchCounter;
			*out++ = ((int32_t)(currentSampelValue + (int32_t) (( (currentInterpolationFraction >> 2) * interpolationDif) >> 14 ) ) * granularEnvelopeTab[volIndex]) >> 16;

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
			//**************************************************************************

//*************************************************************************************** koniec przetwarzania pitchCountera

			//*************************************************************************************** warunki pętli
			if (iPitchCounter <= loopStartPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint2;
				currentFractionPitchCounter = 0;
			} else if (iPitchCounter >= loopEndPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint2 - 1;
				loopBackwardFlag = 1;
				in_interpolation = in+1;
				currentFractionPitchCounter = 0;
			}
			//*************************************************************************************** warunki pętli

		}

		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}
audio_block_t * AudioPlayMemory::updateGranularLoopBackwardReverse()
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

		castPitchControl = (int32_t)-pitchControl;
		pitchFraction =  - (pitchControl - (int32_t)pitchControl);

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		if(enableInterpolation) interpolationCondition = ((pitchControl  < 1.0f) && (((int)(iPitchCounter - 128 * pitchControl) > 0)) ) ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation = loopBackwardFlag ? in+1: in-1;
		int32_t currentInterpolationFraction;
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			//*********************************** GLIDE HANDLE

			if (constrainsInSamples.glide)
			{
				if (glideCounter <= constrainsInSamples.glide)
				{
					pitchControl += glideControl;
					castPitchControl = (int32_t)   -pitchControl;
					pitchFraction =  -(pitchControl - (int32_t)pitchControl);
					currentFractionPitchControl = pitchFraction * MAX_16BIT;

					glideCounter++;
				}
			}
			//**********************************************
//*************************************************************************************** poczatek przetwarzania pitchCountera
			uint16_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

			currentSampelValue = *(in + iPitchCounter);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

			//**************************************************************************

			currentInterpolationFraction =  (currentFractionPitchCounter < 0) ? -currentFractionPitchCounter : currentFractionPitchCounter;
			*out++ = ((int32_t)(currentSampelValue + (int32_t) (( (currentInterpolationFraction >> 2) * interpolationDif) >> 14 ) ) * granularEnvelopeTab[volIndex]) >> 16;

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
			//**************************************************************************

//*************************************************************************************** koniec przetwarzania pitchCountera
			if (iPitchCounter >= loopEndPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
				currentFractionPitchCounter = 0;
			} else if (iPitchCounter <= loopStartPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 + 1 : 1;
				loopBackwardFlag = 1;
				in_interpolation = in+1;
				currentFractionPitchCounter = 0;
			}
		}

		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}
audio_block_t * AudioPlayMemory::updateGranularLoopPingPongNormal()
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

		castPitchControl = (int32_t) pitchControl;
		pitchFraction =  pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		if(enableInterpolation) interpolationCondition = ((pitchControl  < 1.0f) && (( (iPitchCounter + 128 * pitchControl) < length))) ? 0: 1;
		else interpolationCondition = 1;

		int16_t * in_interpolation =  loopBackwardFlag ? in-1 : in+1;
		int32_t currentInterpolationFraction;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
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
			uint16_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

			currentSampelValue = *(in + iPitchCounter);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

			currentInterpolationFraction =  (currentFractionPitchCounter < 0) ? -currentFractionPitchCounter : currentFractionPitchCounter;
			//**************************************************************************
			*out++ = ((int32_t)(currentSampelValue + (int32_t) (( (currentInterpolationFraction >> 2) * interpolationDif) >> 14 ) ) * granularEnvelopeTab[volIndex]) >> 16;

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
			//**************************************************************************

//*************************************************************************************** koniec przetwarzania pitchCountera
			if (iPitchCounter <= loopStartPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 + 1 : 1;
				loopBackwardFlag = 0;
				in_interpolation = in+1;
				currentFractionPitchCounter = 0;
			}
			else if (iPitchCounter >= loopEndPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint2 - 1;
				loopBackwardFlag = 1;
				in_interpolation = in-1;
				currentFractionPitchCounter = 0;
			}

		}

		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}
audio_block_t * AudioPlayMemory::updateGranularLoopPingPongReverse()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;
	int32_t loopStartPoint = max((int32_t)constrainsInSamples.loopPoint1, 0);
	int32_t loopEndPoint = min((int32_t)constrainsInSamples.loopPoint2, (int32_t)min(length,constrainsInSamples.endPoint));

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

		if(enableInterpolation) interpolationCondition = ((pitchControl  < 1.0f) && (((int)(iPitchCounter - 128 * pitchControl) > 0)) ) ? 0: 1;
		else interpolationCondition = 1;


		int16_t * in_interpolation = loopBackwardFlag ? in+1 : in-1;
		int32_t currentInterpolationFraction;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			//*********************************** GLIDE HANDLE

			if (constrainsInSamples.glide)
			{
				if (glideCounter <= constrainsInSamples.glide)
				{
					pitchControl += glideControl;
					castPitchControl = (int32_t) -pitchControl;
					pitchFraction = - (pitchControl - (int32_t)pitchControl) ;
					currentFractionPitchControl = pitchFraction * MAX_16BIT;
					glideCounter++;
				}
			}
			//***********************************************
//*************************************************************************************** poczatek przetwarzania pitchCountera
			uint16_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

			currentSampelValue = *(in + iPitchCounter);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);


			//**************************************************************************

			currentInterpolationFraction =  (currentFractionPitchCounter < 0) ? -currentFractionPitchCounter : currentFractionPitchCounter;
			*out++ = ((int32_t)(currentSampelValue + (int32_t) (( (currentInterpolationFraction >> 2) * interpolationDif) >> 14 ) ) * granularEnvelopeTab[volIndex]) >> 16;
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
			//**************************************************************************

//*************************************************************************************** koniec przetwarzania pitchCountera

//*************************************************************************************** warunki pointow

			if (iPitchCounter >= loopEndPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint2 - 1;
				loopBackwardFlag = 0;
				in_interpolation = in-1;
				currentFractionPitchCounter = 0;
			}
			if (iPitchCounter <= loopStartPoint)
			{
				if(granularPositionRefreshFlag) refreshGranularPosition();
				iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 + 1 : 1;
				loopBackwardFlag = 1;
				in_interpolation = in+1;
				currentFractionPitchCounter = 0;
			}

		}

		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}

