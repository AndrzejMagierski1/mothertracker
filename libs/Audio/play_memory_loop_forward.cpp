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



