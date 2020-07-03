#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"


void AudioPlayMemory::playWavetable(uint8_t instrIdx, int8_t note)
{
	__disable_irq();
	AudioNoInterrupts();

	if( instrIdx > INSTRUMENTS_MAX ) instrIdx = INSTRUMENTS_MAX;
	if( note > MAX_NOTE ) note = MAX_NOTE;

	lastInstrIdxInPlay = currentInstrIdxInPlay;
	currentInstrIdxInPlay = instrIdx;
	currentInstrIdx = instrIdx;

	switch(mtProject.instrument[instrIdx].sample.wavetable_window_size)
	{
		case 32: 		wt_notes = wt32Note; 		break;
		case 64: 		wt_notes = wt64Note; 		break;
		case 128:		wt_notes = wt128Note; 		break;
		case 256:		wt_notes = wt256Note; 		break;
		case 512: 		wt_notes = wt512Note; 		break;
		case 1024: 		wt_notes = wt1024Note; 		break;
		case 2048: 		wt_notes = wt2048Note; 		break;
		default: break;
	}

	refreshStartParamiters();

	constrainCurrentTune(note);

	setStartPitch(note);

	applyFinetuneOnPitch(note);

	calculateGlidePitch(note);

	lastNote = note;

	wavetableWindowSize = mtProject.instrument[instrIdx].sample.wavetable_window_size;
	currentWindow = wavetableWindowForceFlag ? forcedWavetableWindow : mtProject.instrument[instrIdx].wavetableCurrentWindow;

	constrainsInSamples.endPoint = wavetableWindowSize * mtProject.instrument[instrIdx].sample.wavetableWindowNumber;
	constrainsInSamples.loopPoint1 = 0;
	constrainsInSamples.loopPoint2 = wavetableWindowSize;
	constrainsInSamples.loopLength = wavetableWindowSize;

	pointsInSamples.start = 0;

	next = currentStartAddress;
	length = currentSampleLength;
	iPitchCounter = 0;


	playing = 1;
	__enable_irq();
	AudioInterrupts();
}

void AudioPlayMemory::updateWavetable()
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

		waveTablePosition = wavetableWindowSize * currentWindow;

		castPitchControl = (int32_t)pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;


		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (length > (iPitchCounter + waveTablePosition))
			{
				//*********************************** GLIDE HANDLE

				if (constrainsInSamples.glide)
				{
					if (glideCounter <= constrainsInSamples.glide)
					{
						pitchControl += glideControl;
						castPitchControl = (int32_t) pitchControl;
						pitchFraction = pitchControl - (int32_t)pitchControl;
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

						if (iPitchCounter >= wavetableWindowSize)
						{
							iPitchCounter -= wavetableWindowSize;
						}
					}
					if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)) && !reverseDirectionFlag)
					{
						iPitchCounter = length;
					}
					i = SMOOTHING_SIZE;

				}
				//***********************************************

//*************************************************************************************** poczatek przetwarzania pitchCountera

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

			//*************************************************************************************** koniec warunków pointow
			//*************************************************************************************** warunki pętli
				if ((iPitchCounter >= (constrainsInSamples.endPoint)) && (constrainsInSamples.endPoint != (constrainsInSamples.loopPoint2)) && !reverseDirectionFlag)
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

