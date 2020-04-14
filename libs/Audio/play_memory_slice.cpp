#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playSlice(uint8_t instrIdx, int8_t note)
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

	uint16_t startSlice, endSlice;


	if(sliceForcedFlag)
	{
		startSlice = (mtProject.instrument[instrIdx].sliceNumber > 0) ? mtProject.instrument[instrIdx].slices[forcedSlice] : 0;
		endSlice =
			 ((mtProject.instrument[instrIdx].sliceNumber > 1 ) &&  ( (mtProject.instrument[instrIdx].sliceNumber - 1) != forcedSlice) ) ?
			mtProject.instrument[instrIdx].slices[forcedSlice + 1] : MAX_16BIT;
	}
	else
	{
		startSlice = (mtProject.instrument[instrIdx].sliceNumber > 0) ? mtProject.instrument[instrIdx].slices[mtProject.instrument[instrIdx].selectedSlice] : 0;
		endSlice =
			((mtProject.instrument[instrIdx].sliceNumber > 1 ) &&  ( (mtProject.instrument[instrIdx].sliceNumber - 1) != mtProject.instrument[instrIdx].selectedSlice) ) ?
			mtProject.instrument[instrIdx].slices[mtProject.instrument[instrIdx].selectedSlice + 1] : MAX_16BIT;
	}

	pointsInSamples.start = (uint32_t)( (float)startSlice * ((float)currentSampleLength / MAX_16BIT));
	pointsInSamples.end = (uint32_t)( (float)endSlice * ((float)currentSampleLength / MAX_16BIT));

	pointsInSamples.loop1 = 0;
	pointsInSamples.loop2 = 0;

	constrainsInSamples.endPoint = pointsInSamples.end - pointsInSamples.start;

	next = currentStartAddress + pointsInSamples.start;
	length = currentSampleLength - pointsInSamples.start;
	iPitchCounter = reverseDirectionFlag ? constrainsInSamples.endPoint - 1 : 0;

	playing = 1;
	__enable_irq();
	AudioInterrupts();
}

void AudioPlayMemory::updateSlice()
{
	updateSingleShot();
}

