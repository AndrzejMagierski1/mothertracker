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

void AudioPlayMemory::updateGranular()
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

		interpolationCondition = 	   (!((iPitchCounter  < 1.0f) ||
										   (( (iPitchCounter + 128 * pitchControl) < length) && (!reverseDirectionFlag)) ||
										   (((int)(iPitchCounter - 128 * pitchControl) > 0) && (reverseDirectionFlag)) )) ? 1: 0;
		int16_t * in_interpolation = reverseDirectionFlag ? in-1: in+1;

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
						castPitchControl = (int32_t) ( (reverseDirectionFlag) ?  -pitchControl : pitchControl);
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
					uint8_t volIndex = 0;
					for(uint8_t j = 0; j < SMOOTHING_SIZE; j++ )
					{
						volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);
						if(iPitchCounter <= length)
						{
							//srednia wazona miedzy ostatnia probka z poprzedniego pliku a nowymi probkami
							float granMult = granularEnvelopeTab[volIndex];
							*out++ = ( (int32_t)( ( (int32_t)( (*(in + iPitchCounter)) * j * granMult ) + (int32_t)(lastSample * (SMOOTHING_SIZE - 1 - j) ) ) )/(SMOOTHING_SIZE - 1));
						}
						else
						{
							*out++ = 0;
						}

						switch(granularLoopType)
						{
							case granularLoopForward:
								//**************************************************************************
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
								//**************************************************************************
							break;
							case granularLoopBackward:
								//**************************************************************************
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
								//**************************************************************************
							break;
							case granularLoopPingPong:
								//**************************************************************************
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
								//**************************************************************************
							break;
							default: break;
						}

						if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;

						switch(granularLoopType)
						{
							case granularLoopForward:
								if(reverseDirectionFlag)
								{
									if ((iPitchCounter <= constrainsInSamples.loopPoint1))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint2;
										fPitchCounter = 0;
									}
								}
								else
								{
									if ((iPitchCounter >= constrainsInSamples.loopPoint2))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint1;
										fPitchCounter = 0;
									}
								}
							break;

							case granularLoopBackward:
								if(reverseDirectionFlag)
								{
									if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
										fPitchCounter = 0;
									}
								}
								else
								{
									if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint2;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint2;
										fPitchCounter = 0;
									}
								}
							break;

							case granularLoopPingPong:
								if(reverseDirectionFlag)
								{
									if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint2;
										loopBackwardFlag = 0;
										fPitchCounter = 0;
									}
								}
								else
								{
									if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint2;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = constrainsInSamples.loopPoint1;
										loopBackwardFlag = 0;
										fPitchCounter = 0;
									}
								}
							break;
							default: break;
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
				uint8_t volIndex = map(iPitchCounter,constrainsInSamples.loopPoint1,constrainsInSamples.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

				currentSampelValue = *(in + iPitchCounter);

				if(interpolationCondition) interpolationDif = 0;
				else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

				switch(granularLoopType)
				{
					case granularLoopForward:
						//**************************************************************************
						*out++ = (currentSampelValue + (int32_t)(fPitchCounter * interpolationDif)) * granularEnvelopeTab[volIndex];
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(out - 1);
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
						//**************************************************************************
					break;
					case granularLoopBackward:
						//**************************************************************************
						*out++ = (currentSampelValue + (int32_t)(fPitchCounter * interpolationDif)) * granularEnvelopeTab[volIndex];
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(out - 1);
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
						//**************************************************************************
					break;
					case granularLoopPingPong:
						//**************************************************************************
						*out++ = (currentSampelValue + (int32_t)(fPitchCounter * interpolationDif)) * granularEnvelopeTab[volIndex];
						if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(out - 1);
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
						//**************************************************************************
					break;
					default: break;
				}
//*************************************************************************************** koniec przetwarzania pitchCountera
				if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
//*************************************************************************************** warunki pointow

				switch(granularLoopType)
				{
					case granularLoopForward:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= constrainsInSamples.loopPoint1))
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint2;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= constrainsInSamples.loopPoint2))
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint1;
								fPitchCounter = 0;
							}
						}
					break;

					case granularLoopBackward:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint2;
								fPitchCounter = 0;
							}
						}
					break;

					case granularLoopPingPong:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= constrainsInSamples.loopPoint1) && (!loopBackwardFlag))
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint1 ? constrainsInSamples.loopPoint1 : 1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= constrainsInSamples.loopPoint2) && loopBackwardFlag)
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint2;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= constrainsInSamples.loopPoint2) && (!loopBackwardFlag))
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= constrainsInSamples.loopPoint1) && loopBackwardFlag)
							{
								if(granularPositionRefreshFlag) refreshGranularPosition();
								iPitchCounter = constrainsInSamples.loopPoint1;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}
					break;
					default: break;
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


