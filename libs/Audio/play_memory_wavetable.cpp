#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"


void AudioPlayMemory::playWavetable(uint8_t instrIdx, int8_t note)
{
	__disable_irq();
	AudioNoInterrupts();

	/*========================================================INIT=============================================================*/
	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
	uint16_t startSlice = 0, endSlice = 0;
	uint16_t startGranular = 0, loopPoint1Granular = 0, loopPoint2Granular = 0, endGranular = 0;
	uint16_t granularLength = ((uint32_t)((uint32_t)mtProject.instrument[instrIdx].granular.grainLength * (uint32_t)MAX_16BIT))/mtProject.instrument[instrIdx].sample.length;


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
		granularDownConstrain = mtProject.instrument[instrIdx].granular.currentPosition - (granularLength/2);
		granularUpConstrain = mtProject.instrument[instrIdx].granular.currentPosition + (granularLength/2);
	}

	startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
	loopPoint1Granular = startGranular + 1;
	endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
	loopPoint2Granular = endGranular - 1;

	granularLoopType = mtProject.instrument[instrIdx].granular.type;
	currentGranularPosition = granularForcedFlag ? forcedGranularPosition : mtProject.instrument[instrIdx].granular.currentPosition;
	switch(mtProject.instrument[instrIdx].granular.shape)
	{
		case granularShapeSquare: 		granularEnvelopeTab = squareTab; 	break;
		case granularShapeGauss: 		granularEnvelopeTab = gaussTab; 	break;
		case granularShapeTriangle: 	granularEnvelopeTab = triangleTab; 	break;
		default: break;
	}

//******************

	if(instrIdx > INSTRUMENTS_MAX) instrIdx = INSTRUMENTS_MAX;
	if(note > MAX_NOTE) note = MAX_NOTE;

	if(playing == 1) needSmoothingFlag = 1;
	playing = 0;
	loopBackwardFlag=0;
	iPitchCounter = 0;
	fPitchCounter=0;
	glideCounter=0;
	currentInstrIdx=instrIdx;

	/*=========================================================================================================================*/
	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
	currentGlide= glideForceFlag ? forcedGlide : mtProject.instrument[instrIdx].glide;

	currentTune = tuneForceFlag ? forcedTune : mtProject.instrument[instrIdx].tune;

	currentFineTune = fineTuneForceFlag ? forcedFineTune : mtProject.instrument[instrIdx].fineTune;

	currentStartAddress = mtProject.instrument[instrIdx].sample.address;

	currentPlayMode = mtProject.instrument[instrIdx].playMode;

	currentSampleLength = mtProject.instrument[instrIdx].sample.length;



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

	if(currentPlayMode == playModeWavetable)
	{
		switch(mtProject.instrument[instrIdx].sample.wavetable_window_size)
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

	if(lastNote>=0 && currentGlide != 0 ) pitchControl = (currentPlayMode != playModeWavetable) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (currentPlayMode != playModeWavetable) ? (float)notes[note+ currentTune] : (float)wt_notes[note + currentTune];

	if(currentPlayMode != playModeWavetable)
	{
		if(pointsForceFlag)
		{
			startPoint=forcedStartPoint;
			endPoint=forcedEndPoint;
			if(currentPlayMode != playModeSingleShot) //loopMode
			{
				loopPoint1=forcedLoopPoint1;
				loopPoint2=forcedLoopPoint2;
			}
		}
		else
		{
			startPoint=mtProject.instrument[instrIdx].startPoint;
			endPoint=mtProject.instrument[instrIdx].endPoint;
			if(currentPlayMode != playModeSingleShot) //loopMode
			{
				loopPoint1=mtProject.instrument[instrIdx].loopPoint1;
				loopPoint2=mtProject.instrument[instrIdx].loopPoint2;
			}
		}
	}
	else
	{

		wavetableWindowSize = mtProject.instrument[instrIdx].sample.wavetable_window_size;
		currentWindow = wavetableWindowForceFlag ? forcedWavetableWindow : mtProject.instrument[instrIdx].wavetableCurrentWindow;

		constrainsInSamples.endPoint=wavetableWindowSize*mtProject.instrument[instrIdx].sample.wavetableWindowNumber; // nie ma znaczenia
		constrainsInSamples.loopPoint1=0; //currentWindow*wavetableWindowSize;
		constrainsInSamples.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		constrainsInSamples.loopLength=wavetableWindowSize;

		pointsInSamples.start = 0;
	}
	/*=========================================================================================================================*/
	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
	if(currentPlayMode != playModeWavetable)
	{
		if(currentPlayMode == playModeSingleShot)
		{
			if (startPoint >= endPoint);
		}
		else if((currentPlayMode != playModeSlice) && (currentPlayMode != playModeGranular))
		{
			if ( (startPoint >= endPoint) || (startPoint > loopPoint1) || (startPoint > loopPoint2) )
			{
				__enable_irq();
				AudioInterrupts();
				return;
			}
			if ((loopPoint1 > loopPoint2) || (loopPoint1 > endPoint))
			{
				__enable_irq();
				AudioInterrupts();
				return;
			}
			if (loopPoint2 > endPoint)
			{
				__enable_irq();
				AudioInterrupts();
				return;
			}
		}
	}

	/*=========================================================================================================================*/
	/*====================================================PRZELICZENIA=========================================================*/

	if(currentFineTune >= 0)
	{
		if((note + currentTune + 1) <= (MAX_NOTE-1))
		{
			float localPitch1 = (currentPlayMode != playModeWavetable) ? (float)notes[note + currentTune + 1] : (float)wt_notes[note + currentTune + 1];
			float localPitch2 = (currentPlayMode != playModeWavetable) ? (float)notes[note + currentTune] : (float)wt_notes[note + currentTune];

			fineTuneControl= currentFineTune * (( localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}
	else
	{
		if((note + currentTune - 1) >= MIN_NOTE)
		{
			float localPitch1 = (currentPlayMode != playModeWavetable) ? (float)notes[note + currentTune - 1] : (float)wt_notes[note + currentTune - 1];
			float localPitch2 = (currentPlayMode != playModeWavetable) ? (float)notes[note + currentTune] : (float)wt_notes[note + currentTune];

			fineTuneControl= (0-currentFineTune) * ((localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
		}
		else fineTuneControl=0;
	}

	pitchControl+=fineTuneControl;

	if(currentGlide)
	{
		constrainsInSamples.glide=(uint32_t)((float)currentGlide*44.1);

		float localPitch1 = (currentPlayMode != playModeWavetable) ? (float)notes[note + currentTune] : (float)wt_notes[note + currentTune];
		float localPitch2 = (currentPlayMode != playModeWavetable) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
		if((lastNote>=0) && (lastNote != note)) glideControl=(localPitch1 - localPitch2 )/constrainsInSamples.glide;
		else glideControl=0;
	}
	else
	{
		constrainsInSamples.glide=0;
		glideControl=0;
	}


	lastNote=note;
	if(currentPlayMode != playModeWavetable)
	{
		if(currentPlayMode == playModeGranular)
		{
			pointsInSamples.start = (uint32_t)((float)startGranular*((float)currentSampleLength/MAX_16BIT));
			pointsInSamples.end = (uint32_t)((float)endGranular*((float)currentSampleLength/MAX_16BIT));

			pointsInSamples.loop1 = (uint32_t)((float)loopPoint1Granular*((float)currentSampleLength/MAX_16BIT));
			pointsInSamples.loop2 = (uint32_t)((float)loopPoint2Granular*((float)currentSampleLength/MAX_16BIT));

			constrainsInSamples.loopPoint1=pointsInSamples.loop1-pointsInSamples.start;
			constrainsInSamples.loopPoint2=pointsInSamples.loop2-pointsInSamples.start;
			constrainsInSamples.loopLength=pointsInSamples.loop2-pointsInSamples.loop1;
		}
		else if(currentPlayMode  == playModeSlice)
		{
			pointsInSamples.start= (uint32_t)((float)startSlice*((float)currentSampleLength/MAX_16BIT));
			pointsInSamples.end= (uint32_t)((float)endSlice*((float)currentSampleLength/MAX_16BIT));

			pointsInSamples.loop1 = 0;
			pointsInSamples.loop2 = 0;
		}
		else
		{
			pointsInSamples.start= (uint32_t)((float)startPoint*((float)currentSampleLength/MAX_16BIT));
			pointsInSamples.end= (uint32_t)((float)endPoint*((float)currentSampleLength/MAX_16BIT));

			if(currentPlayMode != playModeSingleShot)
			{
				pointsInSamples.loop1= (uint32_t)((float)loopPoint1*((float)currentSampleLength/MAX_16BIT));
				pointsInSamples.loop2= (uint32_t)((float)loopPoint2*((float)currentSampleLength/MAX_16BIT));
			}
			else
			{
				pointsInSamples.loop1 = 0;
				pointsInSamples.loop2 = 0;
			}

			if((pointsInSamples.start >= currentSampleLength) || (pointsInSamples.loop1>currentSampleLength) || (pointsInSamples.loop2>currentSampleLength) || (pointsInSamples.end>currentSampleLength))
			{
				__enable_irq();
				AudioInterrupts();
				return;
			}

			if((currentPlayMode != playModeSingleShot) && (currentPlayMode != playModeSlice))
			{
				constrainsInSamples.loopPoint1=pointsInSamples.loop1-pointsInSamples.start;
				constrainsInSamples.loopPoint2=pointsInSamples.loop2-pointsInSamples.start;
				constrainsInSamples.loopLength=pointsInSamples.loop2-pointsInSamples.loop1;
			}
		}


		constrainsInSamples.endPoint=pointsInSamples.end- pointsInSamples.start;
	}



/*===========================================================================================================================*/
/*============================================PRZEKAZANIE PARAMETROW=========================================================*/

	if(currentPlayMode != playModeWavetable)
	{
		next = currentStartAddress + pointsInSamples.start;
		length =currentSampleLength-pointsInSamples.start;
		iPitchCounter = reverseDirectionFlag ? constrainsInSamples.endPoint - 1 : 0;
	}
	else
	{
		next = currentStartAddress;
		length =currentSampleLength;
		iPitchCounter = 0;
	}

	playing = 1;
	__enable_irq();
	AudioInterrupts();
	return;
}

