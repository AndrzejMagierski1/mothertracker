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

	constrainsInSamples.endPoint = pointsInSamples.end- pointsInSamples.start;

	if(currentPlayMode != playModeWavetable)
	{
		next = currentStartAddress + pointsInSamples.start;
		length = currentSampleLength - pointsInSamples.start;
		iPitchCounter = reverseDirectionFlag ? constrainsInSamples.endPoint - 1 : 0;
	}

	playing = 1;
	__enable_irq();
	AudioInterrupts();
}



