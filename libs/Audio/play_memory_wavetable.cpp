#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"


void AudioPlayMemory::playWavetable(uint8_t instrIdx, int8_t note)
{
	__disable_irq();
	AudioNoInterrupts();

	if( instrIdx > INSTRUMENTS_MAX ) instrIdx = INSTRUMENTS_MAX;
	if( note > MAX_NOTE ) note = MAX_NOTE;

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

