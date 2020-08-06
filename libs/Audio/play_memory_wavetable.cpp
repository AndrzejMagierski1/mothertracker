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

audio_block_t * AudioPlayMemory::updateWavetable()
{
	audio_block_t *block= nullptr;
	int16_t *in = nullptr;
	int16_t *out = nullptr;
	int32_t castPitchControl;
	float pitchFraction;

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

		waveTablePosition = wavetableWindowSize * currentWindow;

		castPitchControl = (int32_t)pitchControl;
		pitchFraction = pitchControl - (int32_t)pitchControl;

		int32_t currentFractionPitchCounter = fPitchCounter * MAX_16BIT;
		int32_t currentFractionPitchControl = pitchFraction * MAX_16BIT;

		interpolationCondition = ((pitchControl  < 1.0f) && (( (iPitchCounter + waveTablePosition + 128 * pitchControl) < length))) ? 0: 1;
		int16_t * in_interpolation = in+1;

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
//*************************************************************************************** poczatek przetwarzania pitchCountera
			currentSampelValue = *(in + iPitchCounter + waveTablePosition);

			if(interpolationCondition) interpolationDif = 0;
			else interpolationDif = (*(in_interpolation + iPitchCounter) - currentSampelValue);

			*out++ = currentSampelValue + (int32_t)(( (currentFractionPitchCounter >> 2) * interpolationDif) >> 14);

			iPitchCounter += castPitchControl;
			currentFractionPitchCounter += currentFractionPitchControl;
			if (currentFractionPitchCounter >= MAX_16BIT)
			{
				currentFractionPitchCounter -= MAX_16BIT;
				iPitchCounter++;
			}

			if (iPitchCounter >= wavetableWindowSize)
			{
				iPitchCounter = 0;
			}

		}
		next = currentStartAddress + pointsInSamples.start;
		fPitchCounter = (float)currentFractionPitchCounter/MAX_16BIT;

		return block;
	}
}

