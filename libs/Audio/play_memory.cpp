/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

constexpr uint8_t SMOOTHING_SIZE = 100;

//**************************************************************************************UPDATE START

void AudioPlayMemory::update(void)
{
	audio_block_t *block=NULL;
	int16_t *in=NULL;
	int16_t *out=NULL;
	int i;
	int32_t castPitchControl;
	float pitchFraction;

	block = allocate();
	if (!block) return;

	if (!playing)
	{
		release(block);
		return;
	}
	else if (playing == 0x81)
	{

		out = block->data;
		in = next;

		if (sampleType == mtSampleTypeWavetable)
		{
			waveTablePosition = wavetableWindowSize * currentWindow;
		}
		castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
		pitchFraction = pitchControl - (int32_t)pitchControl;
//		//todo: monitorować czy przez tą linijke nie wyjezdza za bufor
//		length += castPitchControl; //maksymalnie moze wyjsc za length i nie wiecej niz pitch control
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (((length > iPitchCounter) && (sampleType == mtSampleTypeWaveFile)) || ((length > (iPitchCounter + waveTablePosition)) && (sampleType == mtSampleTypeWavetable)))
			{
				if (sampleConstrains.glide)
				{
					if (glideCounter <= sampleConstrains.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
						pitchFraction = pitchControl - (int32_t)pitchControl;
						glideCounter++;
					}

				}

				if (slideControl != 0.0f)
				{
					if (slideCounter <= sampleConstrains.slide)
					{
						pitchControl += slideControl;
						castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
						slideCounter++;
					}
					else
					{
						pitchControl -= (slideControl * slideCounter); // nie bac sie - to sie robi tylko raz
						castPitchControl = (int32_t) (reverseDirectionFlag ?  -pitchControl : pitchControl);
						slideControl = 0.0f;
						slideCounter = 0;
						sampleConstrains.slide = 0;
					}
					pitchFraction = pitchControl - (int32_t)pitchControl;
				}

				if(needSmoothingFlag && (i == 0))
				{
					needSmoothingFlag = 0;
					for(uint8_t j = 0; j < SMOOTHING_SIZE; j++ )
					{
						if(iPitchCounter != length)
						{
							*out++ = ((int32_t)(((int32_t)((*(in + iPitchCounter) )*j) + (int32_t)(lastSample * (SMOOTHING_SIZE - 1 - j)) ) )/(SMOOTHING_SIZE - 1));
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
						if(sampleType == mtSampleTypeWavetable)
						{
							if (iPitchCounter >= wavetableWindowSize)
							{
								iPitchCounter -= wavetableWindowSize;
							}
						}
						else
						{
							if ((iPitchCounter >= (sampleConstrains.endPoint)) && (sampleConstrains.endPoint != (sampleConstrains.loopPoint2)) && !reverseDirectionFlag)
							{
								iPitchCounter = length;
							}
							else if (((iPitchCounter - castPitchControl) <= 0)  && reverseDirectionFlag)
							{
								iPitchCounter = 0;
							}
						}
					}
					i = SMOOTHING_SIZE;
				}
				if (sampleType != mtSampleTypeWavetable)
				{
					switch (playMode)
					{
					case playModeGranular:
						{
							uint8_t volIndex = map(iPitchCounter,sampleConstrains.loopPoint1,sampleConstrains.loopPoint2,0,GRANULAR_TAB_SIZE - 1);

							switch(granularLoopType)
							{
								case granularLoopForward:
									*out++ = *(in + iPitchCounter) * granularEnvelopeTab[volIndex];
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
									if(iPitchCounter > sampleConstrains.loopPoint2 ) iPitchCounter = sampleConstrains.loopPoint2;
								break;

								case granularLoopBackward:
									*out++ = *(in + iPitchCounter) * granularEnvelopeTab[volIndex];
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
										if(iPitchCounter > sampleConstrains.loopPoint2 ) iPitchCounter = sampleConstrains.loopPoint2;
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
										if(iPitchCounter < sampleConstrains.loopPoint1 ) iPitchCounter = sampleConstrains.loopPoint1;
									}
								break;

								case granularLoopPingPong:

									*out++ = *(in + iPitchCounter) * granularEnvelopeTab[volIndex] ;
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
										if(iPitchCounter > sampleConstrains.loopPoint2 ) iPitchCounter = sampleConstrains.loopPoint2;
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
										if(iPitchCounter < sampleConstrains.loopPoint1 ) iPitchCounter = sampleConstrains.loopPoint1;
									}
								break;
								default: break;
							}
						}
						break;
					case playModeSlice:
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
						break;
					case singleShot:
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
						break;
					case loopForward:
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
						break;
					case loopBackward:
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
						break;
					case loopPingPong:
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
						break;
					default:
						break;
					}

//					castPitchControl = (int32_t) pitchControl;
					if ((int32_t) iPitchCounter < 0) iPitchCounter = 0;
					switch (playMode)
					{
					case playModeGranular:
					{
						switch(granularLoopType)
						{
							case granularLoopForward:
								if(reverseDirectionFlag)
								{
									if ((iPitchCounter <= sampleConstrains.loopPoint1))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint2;
										fPitchCounter = 0;
									}
								}
								else
								{
									if ((iPitchCounter >= sampleConstrains.loopPoint2))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint1;
										fPitchCounter = 0;
									}
								}
							break;

							case granularLoopBackward:
								if(reverseDirectionFlag)
								{
									if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint1;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint1;
										fPitchCounter = 0;
									}
								}
								else
								{
									if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint2;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint2;
										fPitchCounter = 0;
									}
								}
							break;

							case granularLoopPingPong:
								if(reverseDirectionFlag)
								{
									if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint1;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint2;
										loopBackwardFlag = 0;
										fPitchCounter = 0;
									}
								}
								else
								{
									if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint2;
										loopBackwardFlag = 1;
										fPitchCounter = 0;
									}
									if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
									{
										if(granularPositionRefreshFlag) refreshGranularPosition();
										iPitchCounter = sampleConstrains.loopPoint1;
										loopBackwardFlag = 0;
										fPitchCounter = 0;
									}
								}
							break;
							default: break;
						}
						break;
					}
					case loopForward:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= sampleConstrains.loopPoint1))
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= sampleConstrains.loopPoint2))
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								fPitchCounter = 0;
							}
						}

						break;
					case loopBackward:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								fPitchCounter = 0;
							}
						}
						break;
					case loopPingPong:
						if(reverseDirectionFlag)
						{
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}
						else
						{
							if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
							{
								iPitchCounter = sampleConstrains.loopPoint2;
								loopBackwardFlag = 1;
								fPitchCounter = 0;
							}
							if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
							{
								iPitchCounter = sampleConstrains.loopPoint1;
								loopBackwardFlag = 0;
								fPitchCounter = 0;
							}
						}

						break;
					default:
						break;
					}
				}
				else
				{
					*out++ = *(in + (uint32_t) iPitchCounter + waveTablePosition);
					if(i == (AUDIO_BLOCK_SAMPLES - 1)) lastSample = *(in + (uint32_t) iPitchCounter + waveTablePosition);
					iPitchCounter += castPitchControl;
					fPitchCounter += pitchFraction;
					if (fPitchCounter >= 1.0f)
					{
						fPitchCounter -= 1.0f;
						iPitchCounter++;
					}

					if (iPitchCounter >= wavetableWindowSize)
					{
						iPitchCounter -= wavetableWindowSize;
					}
				}
				if ((iPitchCounter >= (sampleConstrains.endPoint)) && (sampleConstrains.endPoint != (sampleConstrains.loopPoint2)) && !reverseDirectionFlag)
				{
					iPitchCounter = length;
				}
				else if (((iPitchCounter - castPitchControl) <= 0)  && reverseDirectionFlag)
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
//		next = in;
		next = startAddress + samplePoints.start;
		transmit(block);
//		length -= castPitchControl; //powrot do bazowej dlugosci
	}
	release(block);


}

//**************************************************************************************UPDATE END
void AudioPlayMemory::printLog(SdFile * log)
{
	log->println("*************Parametry Silnika ");
	log->printf("pitch counter: %d ", iPitchCounter);
	if(iPitchCounter > length) log->print("counter poza plikiem");
	log->println("");
	log->printf("float pitch counter: %05f \n", fPitchCounter);
	log->printf("addr: %x \n", (uint32_t)next);

}
//**************************************************************************************PLAY
uint8_t AudioPlayMemory::play(uint8_t instr_idx,int8_t note)
{
	__disable_irq();
	AudioNoInterrupts();

	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	uint16_t startSlice = 0, endSlice = 0;
	uint16_t startGranular = 0, loopPoint1Granular = 0, loopPoint2Granular = 0, endGranular = 0;
	uint16_t granularLength = ((uint32_t)((uint32_t)mtProject.instrument[instr_idx].granular.grainLength * (uint32_t)MAX_16BIT))/mtProject.instrument[instr_idx].sample.length;

	if(sliceForcedFlag)
	{
		startSlice = (mtProject.instrument[instr_idx].sliceNumber > 0) ? mtProject.instrument[instr_idx].slices[forcedSlice] : 0;
		endSlice =
			((mtProject.instrument[instr_idx].sliceNumber > 1 ) &&  ( (mtProject.instrument[instr_idx].sliceNumber - 1) != forcedSlice) ) ?
			mtProject.instrument[instr_idx].slices[forcedSlice + 1] : MAX_16BIT;
	}
	else
	{
		startSlice = (mtProject.instrument[instr_idx].sliceNumber > 0) ? mtProject.instrument[instr_idx].slices[mtProject.instrument[instr_idx].selectedSlice] : 0;
		endSlice =
			((mtProject.instrument[instr_idx].sliceNumber > 1 ) &&  ( (mtProject.instrument[instr_idx].sliceNumber - 1) != mtProject.instrument[instr_idx].selectedSlice) ) ?
			mtProject.instrument[instr_idx].slices[mtProject.instrument[instr_idx].selectedSlice + 1] : MAX_16BIT;
	}
//************* granular
	int32_t granularDownConstrain = 0;
	int32_t granularUpConstrain = 0;
	if(granularForcedFlag)
	{
		granularDownConstrain = forcedGranularPosition - (granularLength/2);
		granularUpConstrain = forcedGranularPosition + (granularLength/2);
	}
	else
	{
		granularDownConstrain = mtProject.instrument[instr_idx].granular.currentPosition - (granularLength/2);
		granularUpConstrain = mtProject.instrument[instr_idx].granular.currentPosition + (granularLength/2);
	}

	startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
	loopPoint1Granular = startGranular + 1;
	endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
	loopPoint2Granular = endGranular - 1;

	granularLoopType = mtProject.instrument[instr_idx].granular.type;
	switch(mtProject.instrument[instr_idx].granular.shape)
	{
		case granularShapeSquare: 		granularEnvelopeTab = squareTab; 	break;
		case granularShapeGauss: 		granularEnvelopeTab = gaussTab; 	break;
		case granularShapeTriangle: 	granularEnvelopeTab = triangleTab; 	break;
		default: break;
	}

//******************

	if(instr_idx > INSTRUMENTS_MAX) instr_idx = INSTRUMENTS_MAX;
	if(note > MAX_NOTE) note = MAX_NOTE;

	if(playing == 0x81) needSmoothingFlag = 1;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter = 0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;
	currentInstr_idx=instr_idx;

	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
	glide= glideForceFlag ? forcedGlide : mtProject.instrument[instr_idx].glide;

	currentTune = tuneForceFlag ? forcedTune : mtProject.instrument[instr_idx].tune;

	sampleType = mtProject.instrument[instr_idx].sample.type;

	if( (note + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>note) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-note;
	}
	if( (note + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<note)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-note;
	}

	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		switch(mtProject.instrument[instr_idx].sample.wavetable_window_size)
		{
			case 32: 	wt_notes = wt32Note; 	break;
			case 64: 	wt_notes = wt64Note; 	break;
			case 128:	wt_notes = wt128Note; 	break;
			case 256:	wt_notes = wt256Note; 	break;
			case 512: 	wt_notes = wt512Note; 	break;
			case 1024: 	wt_notes = wt1024Note; 	break;
			case 2048: 	wt_notes = wt2048Note; 	break;
			default: break;
		}
	}

	if(lastNote>=0 && glide != 0 ) pitchControl = (sampleType == mtSampleTypeWaveFile) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (sampleType == mtSampleTypeWaveFile) ? (float)notes[note+ currentTune] : (float)wt_notes[note + currentTune];

	int16_t * data = mtProject.instrument[instr_idx].sample.address;
	startAddress = mtProject.instrument[instr_idx].sample.address;

	playMode=mtProject.instrument[instr_idx].playMode;

	startLen=mtProject.instrument[instr_idx].sample.length;



	if(mtProject.instrument[instr_idx].sample.type != mtSampleTypeWavetable)
	{
		if(pointsForceFlag)
		{
			startPoint=forcedStartPoint;
			endPoint=forcedEndPoint;
			if(playMode != singleShot) //loopMode
			{
				loopPoint1=forcedLoopPoint1;
				loopPoint2=forcedLoopPoint2;
			}
		}
		else
		{
			startPoint=mtProject.instrument[instr_idx].startPoint;
			endPoint=mtProject.instrument[instr_idx].endPoint;
			if(playMode != singleShot) //loopMode
			{
				loopPoint1=mtProject.instrument[instr_idx].loopPoint1;
				loopPoint2=mtProject.instrument[instr_idx].loopPoint2;
			}
		}
	}
	else
	{

		wavetableWindowSize = mtProject.instrument[instr_idx].sample.wavetable_window_size;
		currentWindow = wavetableWindowForceFlag ? forcedWavetableWindow : mtProject.instrument[instr_idx].wavetableCurrentWindow;

		sampleConstrains.endPoint=wavetableWindowSize*mtProject.instrument[instr_idx].sample.wavetableWindowNumber; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
	if(mtProject.instrument[instr_idx].sample.type != mtSampleTypeWavetable)
	{
		if(playMode == singleShot)
		{
			if (startPoint >= endPoint) return badStartPoint;
		}
		else if((playMode != playModeSlice) && (playMode != playModeGranular))
		{
			if ( (startPoint >= endPoint) || (startPoint > loopPoint1) || (startPoint > loopPoint2) )
			{
				__enable_irq();
				AudioInterrupts();
				return badStartPoint;
			}
			if ((loopPoint1 > loopPoint2) || (loopPoint1 > endPoint))
			{
				__enable_irq();
				AudioInterrupts();
				return badLoopPoint1;
			}
			if (loopPoint2 > endPoint)
			{
				__enable_irq();
				AudioInterrupts();
				return badLoopPoint2;
			}
		}
	}

	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/
	if(mtProject.instrument[instr_idx].fineTune >= 0)
	{
		if(!fineTuneForceFlag) currentFineTune=mtProject.instrument[instr_idx].fineTune;
		else currentFineTune = forcedFineTune;

		if((note + mtProject.instrument[instr_idx].tune + 1) <= MAX_NOTE)
		{
			fineTuneControl= currentFineTune * (((float)notes[note + currentTune + 1] - (float)notes[note + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((note + mtProject.instrument[instr_idx].tune - 1) >= MIN_NOTE)
		{
			fineTuneControl= (0 - currentFineTune) * (((float)notes[note + currentTune - 1] - (float)notes[note + currentTune] )/MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}

	pitchControl+=fineTuneControl;

	if(glide)
	{
		sampleConstrains.glide=(uint32_t)((float)glide*44.1);
		if((lastNote>=0) && (lastNote != note)) glideControl=((float)notes[note + currentTune]-(float)notes[lastNote + currentTune] )/sampleConstrains.glide;
		else glideControl=0;
	}
	else
	{
		sampleConstrains.glide=0;
		glideControl=0;
	}


	lastNote=note;
	if(mtProject.instrument[instr_idx].sample.type != mtSampleTypeWavetable)
	{
		if(playMode == playModeGranular)
		{
			samplePoints.start = (uint32_t)((float)startGranular*((float)startLen/MAX_16BIT));
			samplePoints.end = (uint32_t)((float)endGranular*((float)startLen/MAX_16BIT));

			samplePoints.loop1 = (uint32_t)((float)loopPoint1Granular*((float)startLen/MAX_16BIT));
			samplePoints.loop2 = (uint32_t)((float)loopPoint2Granular*((float)startLen/MAX_16BIT));

			sampleConstrains.loopPoint1=samplePoints.loop1-samplePoints.start;
			sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
		}
		else if(playMode  == playModeSlice)
		{
			samplePoints.start= (uint32_t)((float)startSlice*((float)startLen/MAX_16BIT));
			samplePoints.end= (uint32_t)((float)endSlice*((float)startLen/MAX_16BIT));

			samplePoints.loop1 = 0;
			samplePoints.loop2 = 0;
		}
		else
		{
			samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
			samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));

			if(playMode != singleShot)
			{
				samplePoints.loop1= (uint32_t)((float)loopPoint1*((float)startLen/MAX_16BIT));
				samplePoints.loop2= (uint32_t)((float)loopPoint2*((float)startLen/MAX_16BIT));
			}
			else
			{
				samplePoints.loop1 = 0;
				samplePoints.loop2 = 0;
			}

			if((samplePoints.start >= startLen) || (samplePoints.loop1>startLen) || (samplePoints.loop2>startLen) || (samplePoints.end>startLen))
			{
				__enable_irq();
				AudioInterrupts();
				return pointsBeyondFile; // wskazniki za plikiem
			}

			if((playMode != singleShot) && (playMode != playModeSlice))
			{
				sampleConstrains.loopPoint1=samplePoints.loop1-samplePoints.start;
				sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
				sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
			}
		}


		sampleConstrains.endPoint=samplePoints.end- samplePoints.start;
	}



/*===========================================================================================================================*/
/*============================================PRZEKAZANIE PARAMETROW=========================================================*/

	if(sampleType != mtSampleTypeWavetable)
	{
		next = data+samplePoints.start;
		beginning = data+samplePoints.start;
		length =startLen-samplePoints.start;
		iPitchCounter = reverseDirectionFlag ? sampleConstrains.endPoint - 1 : 0;
	}
	else
	{
		next = data;
		beginning = data;
		length =startLen;
		iPitchCounter = 0;
	}
	playing = 0x81;
	__enable_irq();
	AudioInterrupts();
	return successInit;
}

uint8_t AudioPlayMemory::playForPrev(uint8_t instr_idx,int8_t n)
{
	__disable_irq();
	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	uint16_t startSlice = (mtProject.instrument[instr_idx].sliceNumber > 0) ? mtProject.instrument[instr_idx].slices[mtProject.instrument[instr_idx].selectedSlice] : 0;
	uint16_t endSlice =
		((mtProject.instrument[instr_idx].sliceNumber > 1 ) &&  ( (mtProject.instrument[instr_idx].sliceNumber - 1) != mtProject.instrument[instr_idx].selectedSlice) ) ?
		mtProject.instrument[instr_idx].slices[mtProject.instrument[instr_idx].selectedSlice + 1] : MAX_16BIT;
	uint16_t startGranular = 0, loopPoint1Granular = 0, loopPoint2Granular = 0, endGranular = 0;
	uint16_t granularLength = ((uint32_t)((uint32_t)mtProject.instrument[instr_idx].granular.grainLength * (uint32_t)MAX_16BIT))/mtProject.instrument[instr_idx].sample.length;

	//************* granular
	int32_t granularDownConstrain = mtProject.instrument[instr_idx].granular.currentPosition - (granularLength/2);;
	int32_t granularUpConstrain = mtProject.instrument[instr_idx].granular.currentPosition + (granularLength/2);

	startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
	loopPoint1Granular = startGranular + 1;
	endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
	loopPoint2Granular = endGranular - 1;

	granularLoopType = mtProject.instrument[instr_idx].granular.type;
	switch(mtProject.instrument[instr_idx].granular.shape)
	{
		case granularShapeSquare: 		granularEnvelopeTab = squareTab; 	break;
		case granularShapeGauss: 		granularEnvelopeTab = gaussTab; 	break;
		case granularShapeTriangle: 	granularEnvelopeTab = triangleTab; 	break;
		default: break;
	}
	//******************


	if(playing == 0x81) needSmoothingFlag = 1;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;
	currentInstr_idx=instr_idx;
	lastNote= - 1;

	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
	glide=mtProject.instrument[instr_idx].glide;
	currentTune=mtProject.instrument[instr_idx].tune;

	if( (n + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>n) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-n;
	}
	if( (n + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<n)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-n;
	}
	if(mtProject.instrument[instr_idx].sample.type == mtSampleTypeWavetable)
	{
		switch(mtProject.instrument[instr_idx].sample.wavetable_window_size)
		{
			case 32: 	wt_notes = wt32Note; 	break;
			case 64: 	wt_notes = wt64Note; 	break;
			case 128:	wt_notes = wt128Note; 	break;
			case 256:	wt_notes = wt256Note; 	break;
			case 512: 	wt_notes = wt512Note; 	break;
			case 1024: 	wt_notes = wt1024Note; 	break;
			case 2048: 	wt_notes = wt2048Note; 	break;
			default: break;
		}
	}

	sampleType = mtProject.instrument[instr_idx].sample.type;

	if(lastNote>=0 && glide != 0 ) pitchControl = (sampleType == mtSampleTypeWaveFile) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (sampleType == mtSampleTypeWaveFile) ? (float)notes[n+ currentTune] : (float)wt_notes[n + currentTune];

	int16_t * data = mtProject.instrument[instr_idx].sample.address;
	startAddress = mtProject.instrument[instr_idx].sample.address;

	playMode=mtProject.instrument[instr_idx].playMode;

	startLen=mtProject.instrument[instr_idx].sample.length;



	if(sampleType != mtSampleTypeWavetable)
	{
		startPoint=mtProject.instrument[instr_idx].startPoint;
		endPoint=mtProject.instrument[instr_idx].endPoint;
		if(playMode != singleShot) //loopMode
		{
			loopPoint1=mtProject.instrument[instr_idx].loopPoint1;
			loopPoint2=mtProject.instrument[instr_idx].loopPoint2;
		}

	}
	else
	{
		wavetableWindowSize = mtProject.instrument[instr_idx].sample.wavetable_window_size;
		currentWindow=mtProject.instrument[instr_idx].wavetableCurrentWindow;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;

	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
	if(sampleType != mtSampleTypeWavetable)
	{
		if(playMode == singleShot)
		{
			if (startPoint >= endPoint) return badStartPoint;
		}
		else if((playMode != playModeSlice) && (playMode != playModeGranular))
		{
			if ( (startPoint >= endPoint) || (startPoint > loopPoint1) || (startPoint > loopPoint2) ) return badStartPoint;
			if ((loopPoint1 > loopPoint2) || (loopPoint1 > endPoint)) return badLoopPoint1;
			if (loopPoint2 > endPoint) return badLoopPoint2;
		}
	}

	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/
	if(mtProject.instrument[instr_idx].fineTune >= 0)
	{
		currentFineTune=mtProject.instrument[instr_idx].fineTune;
		if((n + mtProject.instrument[instr_idx].tune + 1) <= MAX_NOTE)
		{
			fineTuneControl= mtProject.instrument[instr_idx].fineTune * (((float)notes[n + currentTune + 1] - (float)notes[n + currentTune]) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((n + mtProject.instrument[instr_idx].tune - 1) >= MIN_NOTE)
		{
			fineTuneControl= (0 - mtProject.instrument[instr_idx].fineTune) * (((float)notes[n + currentTune - 1] - (float)notes[n + currentTune] )/MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}

	pitchControl+=fineTuneControl;

	if(glide)
	{
		sampleConstrains.glide=(uint32_t)((float)glide*44.1);
		if((lastNote>=0) && (lastNote != n)) glideControl=((float)notes[n + currentTune]-(float)notes[lastNote + currentTune] )/sampleConstrains.glide;
		else glideControl=0;
	}
	else
	{
		sampleConstrains.glide=0;
		glideControl=0;
	}



	if(sampleType != mtSampleTypeWavetable)
	{
		if(playMode == playModeGranular)
		{
			samplePoints.start = (uint32_t)((float)startGranular*((float)startLen/MAX_16BIT));
			samplePoints.end = (uint32_t)((float)endGranular*((float)startLen/MAX_16BIT));

			samplePoints.loop1 = (uint32_t)((float)loopPoint1Granular*((float)startLen/MAX_16BIT));
			samplePoints.loop2 = (uint32_t)((float)loopPoint2Granular*((float)startLen/MAX_16BIT));

			sampleConstrains.loopPoint1=samplePoints.loop1-samplePoints.start;
			sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
		}
		else if(playMode == playModeSlice)
		{
			samplePoints.start= (uint32_t)((float)startSlice*((float)startLen/MAX_16BIT));
			samplePoints.end= (uint32_t)((float)endSlice*((float)startLen/MAX_16BIT));
			samplePoints.loop1 = 0;
			samplePoints.loop2 = 0;
		}
		else
		{
			samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
			samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
			if(playMode != singleShot)
			{
				samplePoints.loop1= (uint32_t)((float)loopPoint1*((float)startLen/MAX_16BIT));
				samplePoints.loop2= (uint32_t)((float)loopPoint2*((float)startLen/MAX_16BIT));
			}

			if((samplePoints.start >= startLen) || (samplePoints.loop1>startLen) || (samplePoints.loop2>startLen) || (samplePoints.end>startLen)) return pointsBeyondFile; // wskazniki za plikiem

			if((playMode != singleShot) && (playMode != playModeSlice))
			{
				sampleConstrains.loopPoint1=samplePoints.loop1-samplePoints.start;
				sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
				sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
			}
		}


		sampleConstrains.endPoint=samplePoints.end- samplePoints.start;
	}



/*===========================================================================================================================*/
/*============================================PRZEKAZANIE PARAMETROW=========================================================*/
	if(sampleType != mtSampleTypeWavetable)
	{
		next = data+samplePoints.start;
		beginning = data+samplePoints.start;
		length =startLen-samplePoints.start;
	}
	else
	{
		next = data;
		beginning = data;
		length =startLen;
	}

	playing = 0x81;

	return successInit;
	__enable_irq();

}



uint8_t AudioPlayMemory::playForPrev(int16_t * addr,uint32_t len, uint8_t type)
{
	uint32_t startPoint,endPoint;
	int8_t note=60;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;

	glide=0;
	currentTune=0;
	lastNote=-1;
	sampleType = type;
	if( (note + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>note) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-note;
	}
	if( (note + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<note)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-note;
	}

	if(type == mtSampleTypeWavetable)
	{
		switch(mtProject.instrument[currentInstr_idx].sample.wavetable_window_size)
		{
			case 32: 	wt_notes = wt32Note; 	break;
			case 64: 	wt_notes = wt64Note; 	break;
			case 128:	wt_notes = wt128Note; 	break;
			case 256:	wt_notes = wt256Note; 	break;
			case 512: 	wt_notes = wt512Note; 	break;
			case 1024: 	wt_notes = wt1024Note; 	break;
			case 2048: 	wt_notes = wt2048Note; 	break;
			default: break;
		}
	}
	if(lastNote >= 0) pitchControl = (sampleType == mtSampleTypeWaveFile) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (sampleType == mtSampleTypeWaveFile) ? (float)notes[note+ currentTune] : (float)wt_notes[note + currentTune];


	int16_t * data = addr;
	startAddress = addr;

	if(sampleType == mtSampleTypeWavetable)
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=0;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}


	playMode=singleShot;

	startLen=len;

	startPoint=0;
	endPoint=MAX_16BIT;
	currentFineTune=0;
	fineTuneControl=0;


	samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
	samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
	sampleConstrains.endPoint=samplePoints.end- samplePoints.start;

	if(sampleType != mtSampleTypeWavetable)
	{
		next = data+samplePoints.start;
		beginning = data+samplePoints.start;
		length =startLen-samplePoints.start;
	}
	else
	{
		next = data;
		beginning = data;
		length =startLen;
	}

	playing = 0x81;

	return successInit;

}



uint8_t AudioPlayMemory::playForPrev(int16_t * addr,uint32_t len, uint8_t n, uint8_t type)
{
	uint32_t startPoint,endPoint;
	int8_t note=n;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter=0;
	fPitchCounter=0;
	glideCounter=0;
	slideCounter=0;

	glide=0;
	currentTune=0;
	lastNote=-1;
	sampleType = type;
	if( (note + currentTune) > (MAX_NOTE-1))
	{
		if(lastNote>note) currentTune=(MAX_NOTE-1)-lastNote;
		else currentTune=(MAX_NOTE-1)-note;
	}
	if( (note + currentTune) < MIN_NOTE)
	{
		if((lastNote>=0) && (lastNote<note)) currentTune=MIN_NOTE-lastNote;
		else currentTune=MIN_NOTE-note;
	}

	if(type == mtSampleTypeWavetable)
	{
		switch(mtProject.instrument[currentInstr_idx].sample.wavetable_window_size)
		{
			case 32: 	wt_notes = wt32Note; 	break;
			case 64: 	wt_notes = wt64Note; 	break;
			case 128:	wt_notes = wt128Note; 	break;
			case 256:	wt_notes = wt256Note; 	break;
			case 512: 	wt_notes = wt512Note; 	break;
			case 1024: 	wt_notes = wt1024Note; 	break;
			case 2048: 	wt_notes = wt2048Note; 	break;
			default: break;
		}
	}

	if(lastNote>=0) pitchControl = (sampleType == mtSampleTypeWaveFile) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (sampleType == mtSampleTypeWaveFile) ? (float)notes[note+ currentTune] : (float)wt_notes[note + currentTune];


	int16_t * data = addr;
	startAddress = addr;

	playMode=singleShot;

	startLen=len;

	startPoint=0;
	endPoint=MAX_16BIT;
	currentFineTune=0;
	fineTuneControl=0;
	if(sampleType == mtSampleTypeWavetable)
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=0;;
		sampleConstrains.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		sampleConstrains.loopPoint1=0; //currentWindow*wavetableWindowSize;
		sampleConstrains.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		sampleConstrains.loopLength=wavetableWindowSize;
	}

	samplePoints.start= (uint32_t)((float)startPoint*((float)startLen/MAX_16BIT));
	samplePoints.end= (uint32_t)((float)endPoint*((float)startLen/MAX_16BIT));
	sampleConstrains.endPoint=samplePoints.end- samplePoints.start;

	if(sampleType != mtSampleTypeWavetable)
	{
		next = data+samplePoints.start;
		beginning = data+samplePoints.start;
		length =startLen-samplePoints.start;
	}
	else
	{
		next = data;
		beginning = data;
		length =startLen;
	}

	playing = 0x81;

	return successInit;

}

//*******************************************************************************************************************

void AudioPlayMemory::stop(void)
{
	__disable_irq();
	playing = 0;
	__enable_irq();
}





void AudioPlayMemory::setWavetableWindow(int16_t value)
{
	if(mtProject.instrument[currentInstr_idx].sample.type != mtSampleTypeWavetable) return;

	if((uint32_t) value >= mtProject.instrument[currentInstr_idx].sample.wavetableWindowNumber) currentWindow = mtProject.instrument[currentInstr_idx].sample.wavetableWindowNumber - 1;
	else if(value < 0) currentWindow = 0;
	else currentWindow=value;
}

void AudioPlayMemory::setPlayMode(uint8_t value)
{
	if(playMode>3) return;
	playMode=value;
}

void AudioPlayMemory::setLP1(uint16_t value) // w audio engine zadba zeby zapodac odpowiednia wartosc gdy force
{
	if(playMode != singleShot) samplePoints.loop1= (uint32_t)((float)value*((float)startLen/MAX_16BIT));
	if ((samplePoints.loop1 < samplePoints.start)||(samplePoints.loop1 > samplePoints.loop2) || (samplePoints.loop1 > samplePoints.end)) return;
	if(playMode != singleShot)
	{
			sampleConstrains.loopPoint1=samplePoints.loop1- samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}

//	next = mtProject.instrument[currentInstr_idx].sample.address + samplePoints.start;
}
void AudioPlayMemory::setLP2(uint16_t value) // w audio engine zadba zeby zapodac odpowiednia wartosc gdy force
{
	if(playMode != singleShot) samplePoints.loop2= (uint32_t)((float)value*((float)startLen/MAX_16BIT));
	if ((samplePoints.loop2 < samplePoints.start)||(samplePoints.loop2 < samplePoints.loop1) || (samplePoints.loop1 > samplePoints.end)) return;
	if(playMode != singleShot)
	{
			sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
			sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	}
//	int16_t * lastNext = next;
//	next = mtProject.instrument[currentInstr_idx].sample.address + samplePoints.start;
//	int32_t localShift = (int32_t)(lastNext - next);
//	iPitchCounter += localShift;

}

void AudioPlayMemory::setGlide(uint16_t value, int8_t currentNote, uint8_t instr_idx)
{
	uint32_t localSampleConstrainsGlide = (uint32_t)(value*44.1);
	float localGlideControl = ((float)notes[currentNote + mtProject.instrument[instr_idx].tune] - (float)notes[lastNote + mtProject.instrument[instr_idx].tune] )/localSampleConstrainsGlide;
	AudioNoInterrupts();
	sampleConstrains.glide = localSampleConstrainsGlide;
	if((lastNote>=0) && (lastNote != currentNote)) glideControl= localGlideControl;
	else glideControl=0;
	AudioInterrupts();
}

void AudioPlayMemory::setSlide(uint16_t value, int8_t currentNote, int8_t slideNote,uint8_t instr_idx)
{
	//todo: jezeli bedzie kiedys uzywane zoptymalizowac wspolprace z przerwaniami i ogólnie ogarnąć to co tu sie dzieje
	float localPitchControl = pitchControl;
	localPitchControl -= (slideControl * slideCounter);
	pitchControl = localPitchControl;
	slideCounter=0;
	sampleConstrains.slide =(uint32_t)(value*44.1);
	if((slideNote>=0) && (slideNote != currentNote)) slideControl=((float)notes[slideNote + mtProject.instrument[instr_idx].tune] - (float)notes[currentNote] )/sampleConstrains.slide;
	else slideControl=0;

}

void AudioPlayMemory::setPitch(float value)
{
	if(value) pitchControl+=value;
	if(pitchControl < MIN_PITCH) pitchControl=MIN_PITCH;
	if(pitchControl > MAX_PITCH ) pitchControl=MAX_PITCH;
}
void AudioPlayMemory::setFineTune(int8_t value, int8_t currentNote)
{
	float localPitchControl = pitchControl;
	float localFineTuneControl = fineTuneControl;
	localPitchControl-=localFineTuneControl;
	uint8_t localSampleType = mtProject.instrument[currentInstr_idx].sample.type;

	if(value >= 0)
	{
		if((currentNote + currentTune + 1) <= (MAX_NOTE-1))
		{
			float localPitch1 = (localSampleType == mtSampleTypeWaveFile) ? (float)notes[currentNote + currentTune + 1] : (float)wt_notes[currentNote + currentTune + 1];
			float localPitch2 = (localSampleType == mtSampleTypeWaveFile) ? (float)notes[currentNote + currentTune] : (float)wt_notes[currentNote + currentTune];

			localFineTuneControl= value * (( localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
		}
		else localFineTuneControl=0;
	}
	else
	{
		if((currentNote + currentTune - 1) >= MIN_NOTE)
		{
			float localPitch1 = (localSampleType == mtSampleTypeWaveFile) ? (float)notes[currentNote + currentTune - 1] : (float)wt_notes[currentNote + currentTune - 1];
			float localPitch2 = (localSampleType == mtSampleTypeWaveFile) ? (float)notes[currentNote + currentTune] : (float)wt_notes[currentNote + currentTune];

			localFineTuneControl= (0-value) * ((localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
		}
		else localFineTuneControl=0;
	}

	localPitchControl+=localFineTuneControl;
	AudioNoInterrupts();
	pitchControl=localPitchControl; // przypisanie nowej wartosci pitchControl w jednej instrukcji asm, żeby nie moglo w miedzyczasie wbic sie przerwanie
	fineTuneControl = localFineTuneControl;
	currentFineTune=value;
	AudioInterrupts();

}

void AudioPlayMemory::refreshGranularPosition()
{
	uint16_t granularLength = ((uint32_t)((uint32_t)mtProject.instrument[currentInstr_idx].granular.grainLength * (uint32_t)MAX_16BIT))/mtProject.instrument[currentInstr_idx].sample.length;
	int32_t granularDownConstrain = currentGranularPosition - (granularLength/2);
	int32_t granularUpConstrain = currentGranularPosition  + (granularLength/2);


	uint16_t startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
	uint16_t loopPoint1Granular = startGranular + 1;
	uint16_t endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
	uint16_t loopPoint2Granular = endGranular - 1;

	samplePoints.start = (uint32_t)((float)startGranular*((float)startLen/MAX_16BIT));
	samplePoints.end = (uint32_t)((float)endGranular*((float)startLen/MAX_16BIT));

	samplePoints.loop1 = (uint32_t)((float)loopPoint1Granular*((float)startLen/MAX_16BIT));
	samplePoints.loop2 = (uint32_t)((float)loopPoint2Granular*((float)startLen/MAX_16BIT));

	length =startLen-samplePoints.start;

	sampleConstrains.loopPoint1=samplePoints.loop1-samplePoints.start;
	sampleConstrains.loopPoint2=samplePoints.loop2-samplePoints.start;
	sampleConstrains.loopLength=samplePoints.loop2-samplePoints.loop1;
	sampleConstrains.endPoint=samplePoints.end- samplePoints.start;


	granularPositionRefreshFlag = 0;
}


void AudioPlayMemory::setGranularPosition(uint16_t val)
{
	if(mtProject.instrument[currentInstr_idx].playMode != playModeGranular) return;

	currentGranularPosition = val;

	granularPositionRefreshFlag = 1;
//	switch(granularLoopType)
//	{
//		case granularLoopForward:
//			if(reverseDirectionFlag)
//			{
//				if ((iPitchCounter <= sampleConstrains.loopPoint1))
//				{
//					iPitchCounter = sampleConstrains.loopPoint2;
//					fPitchCounter = 0;
//				}
//			}
//			else
//			{
//				if ((iPitchCounter >= sampleConstrains.loopPoint2))
//				{
//					iPitchCounter = sampleConstrains.loopPoint1;
//					fPitchCounter = 0;
//				}
//			}
//		break;
//
//		case granularLoopBackward:
//			if(reverseDirectionFlag)
//			{
//				if ((iPitchCounter <= sampleConstrains.loopPoint1) && (!loopBackwardFlag))
//				{
//					iPitchCounter = sampleConstrains.loopPoint1;
//					loopBackwardFlag = 1;
//					fPitchCounter = 0;
//				}
//				if ((iPitchCounter >= sampleConstrains.loopPoint2) && loopBackwardFlag)
//				{
//					iPitchCounter = sampleConstrains.loopPoint1;
//					fPitchCounter = 0;
//				}
//			}
//			else
//			{
//				if ((iPitchCounter >= sampleConstrains.loopPoint2) && (!loopBackwardFlag))
//				{
//					iPitchCounter = sampleConstrains.loopPoint2;
//					loopBackwardFlag = 1;
//					fPitchCounter = 0;
//				}
//				if ((iPitchCounter <= sampleConstrains.loopPoint1) && loopBackwardFlag)
//				{
//					iPitchCounter = sampleConstrains.loopPoint2;
//					fPitchCounter = 0;
//				}
//			}
//	}
}
void AudioPlayMemory::setGranularGrainLength()
{
	if(mtProject.instrument[currentInstr_idx].playMode != playModeGranular) return;

	granularPositionRefreshFlag = 1;
}
void AudioPlayMemory::setGranularWave(uint8_t type)
{
	if(mtProject.instrument[currentInstr_idx].playMode != playModeGranular) return;
	switch(type)
	{
		case granularShapeSquare: 		granularEnvelopeTab = squareTab; 	break;
		case granularShapeGauss: 		granularEnvelopeTab = gaussTab; 	break;
		case granularShapeTriangle: 	granularEnvelopeTab = triangleTab; 	break;
		default: break;
	}
}
void AudioPlayMemory::setGranularLoopMode(uint8_t type)
{
	if(mtProject.instrument[currentInstr_idx].playMode != playModeGranular) return;
	granularLoopType = type;
}

void AudioPlayMemory::setTune(int8_t value, int8_t currentNote)
{
	if( (currentNote + value) > (MAX_NOTE-1)) value=(MAX_NOTE-1)-currentNote;
	if( (currentNote + value) < MIN_NOTE) value=MIN_NOTE-currentNote;

	uint8_t localSampleType = mtProject.instrument[currentInstr_idx].sample.type;
	float localPitchControl = pitchControl;
	localPitchControl -= (localSampleType == mtSampleTypeWaveFile) ? (float)notes[currentNote+currentTune] : (float)wt_notes[currentNote+currentTune];
	localPitchControl += (localSampleType == mtSampleTypeWaveFile) ? (float)notes[currentNote+value] : (float)wt_notes[currentNote+value];
	AudioNoInterrupts();
	pitchControl = localPitchControl;
	currentTune=value;
	AudioInterrupts();
	setFineTune(currentFineTune,currentNote);
}

void AudioPlayMemory::setTuneForceFlag()
{
	tuneForceFlag = 1;
}
void AudioPlayMemory::clearTuneForceFlag()
{
	tuneForceFlag = 0;
}
void AudioPlayMemory::setForcedTune(int8_t value)
{
	forcedTune = value;
}

void AudioPlayMemory::setPointsForceFlag()
{
	pointsForceFlag = 1;
}
void AudioPlayMemory::clearPointsForceFlag()
{
	pointsForceFlag = 0;
}
void AudioPlayMemory::setGlideForceFlag()
{
	glideForceFlag = 1;
}
void AudioPlayMemory::clearGlideForceFlag()
{
	glideForceFlag = 0;
}

void AudioPlayMemory::setForcedGlide(uint16_t value)
{
	forcedGlide = value;
}

void AudioPlayMemory::setFineTuneForceFlag()
{
	fineTuneForceFlag = 1;
}
void AudioPlayMemory::clearFineTuneForceFlag()
{
	fineTuneForceFlag = 0;
}
void AudioPlayMemory::setForcedFineTune(int8_t value)
{
	forcedFineTune = value;
}

void AudioPlayMemory::setForcedPoints(int32_t sp, int32_t lp1, int32_t lp2, int32_t ep)
{
	if(sp != -1) forcedStartPoint = sp;
	else forcedStartPoint = mtProject.instrument[currentInstr_idx].startPoint;
	if(lp1 != -1) forcedLoopPoint1 = lp1;
	else forcedLoopPoint1 = mtProject.instrument[currentInstr_idx].loopPoint1;
	if(lp2 != -1) forcedLoopPoint2 = lp2;
	else forcedLoopPoint2 = mtProject.instrument[currentInstr_idx].loopPoint2;
	if(ep != -1) forcedEndPoint = ep;
	else forcedEndPoint = mtProject.instrument[currentInstr_idx].endPoint;
}

void AudioPlayMemory::setWavetableWindowFlag()
{
	wavetableWindowForceFlag = 1;
}
void AudioPlayMemory::clearWavetableWindowFlag()
{
	wavetableWindowForceFlag = 0;
}
void AudioPlayMemory::setForcedWavetableWindow(int16_t val)
{
	if((uint32_t)val >= mtProject.instrument[currentInstr_idx].sample.wavetableWindowNumber) forcedWavetableWindow = mtProject.instrument[currentInstr_idx].sample.wavetableWindowNumber -1;
	else if(val < 0) forcedWavetableWindow = 0;
	else forcedWavetableWindow=val;
}

void AudioPlayMemory::setGranularPosForceFlag()
{
	granularForcedFlag = 1;
}
void AudioPlayMemory::clearGranularPosForceFlag()
{
	granularForcedFlag = 0;
}
void AudioPlayMemory::setForcedGranularPos(uint16_t value)
{
	forcedGranularPosition = value;
}



void AudioPlayMemory::setReverse()
{
	reverseDirectionFlag = 1;
}
void AudioPlayMemory::clearReverse()
{
	reverseDirectionFlag = 0;
}

uint16_t AudioPlayMemory::getPosition()
{
	return  (uint16_t)(65535 * ((samplePoints.start + iPitchCounter)/(float)startLen));
}

void AudioPlayMemory::setForcedSlice(uint8_t value)
{
	if(mtProject.instrument[currentInstr_idx].sliceNumber == 0) forcedSlice = 0;
	else if(value > mtProject.instrument[currentInstr_idx].sliceNumber - 1) forcedSlice = mtProject.instrument[currentInstr_idx].sliceNumber - 1;
	else forcedSlice = value;
}
void AudioPlayMemory::setSliceForcedFlag()
{
	sliceForcedFlag = 1;
}
void AudioPlayMemory::clearSliceForcedFlag()
{
	sliceForcedFlag = 0;
}

void AudioPlayMemory::clean(void)
{
	if(!playing)
	{
		next=NULL;
		beginning=NULL;
		length=0;
		pitchControl = 1;
		iPitchCounter = 0;
		fPitchCounter = 0;
		playMode = 0;
		playing=0;
		loopBackwardFlag = 0;
		lastNote = -1;
		glide=0;
		glideCounter=0;
		slideCounter=0;
		glideControl=0;
		slideControl=0;
		fineTuneControl=0;
		currentTune=0;
		wavetableWindowSize=0;
		currentWindow=0;
		waveTablePosition=0;
//	    wavetableSync=0;
//	    wavetablePWM=0;
//	    wavetableFlip=0;
//	    wavetableQuantize=0;
		currentInstr_idx=0;
		currentFineTune=0;

		samplePoints.start=0;
		samplePoints.end=0;
		samplePoints.loop1=0;
		samplePoints.loop2=0;

		sampleConstrains.loopLength=0;
		sampleConstrains.loopPoint1=0;
		sampleConstrains.loopPoint2=0;
		sampleConstrains.endPoint=0;

		sampleConstrains.glide=0;
		sampleConstrains.slide=0;

		startLen=0;
	}

}

