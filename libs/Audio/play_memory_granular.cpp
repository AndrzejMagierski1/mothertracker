#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"

void AudioPlayMemory::playGranular(uint8_t instrIdx, int8_t note)
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



