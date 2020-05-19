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

void AudioPlayMemory::updateLoopForward()
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

			if ( dataIsEnable )
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
							if ((iPitchCounter <= constrainsInSamples.loopPoint1))
							{
								iPitchCounter = constrainsInSamples.loopPoint2;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= constrainsInSamples.loopPoint2))
							{
								iPitchCounter = constrainsInSamples.loopPoint1;
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

//*************************************************************************************** poczatek przetwarzania pitchCountera
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

//*************************************************************************************** koniec przetwarzania pitchCountera
				if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
//*************************************************************************************** warunki pointow

				if(reverseDirectionFlag)
				{
					if ((iPitchCounter <= constrainsInSamples.loopPoint1))
					{
						iPitchCounter = constrainsInSamples.loopPoint2;
						fPitchCounter = 0;
					}
				}
				else
				{
					if ((iPitchCounter >= constrainsInSamples.loopPoint2))
					{
						iPitchCounter = constrainsInSamples.loopPoint1;
						fPitchCounter = 0;
					}
				}

			//*************************************************************************************** koniec warunków pointow
			//*************************************************************************************** warunki pętli
				if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)) && !reverseDirectionFlag)
				{
					iPitchCounter = length;
				}
				else if (((iPitchCounter - castPitchControl) <= 0)  && (reverseDirectionFlag))
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

