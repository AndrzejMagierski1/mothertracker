#include "play_memory.h"
#include "Audio.h"
#include "utility/dspinst.h"
#include "mtAudioEngine.h"


//**************************************************************************************UPDATE START

void AudioPlayMemory::update(void)
{
	if(isCurrentLoadInstrument[currentInstrIdx]) return;

	switch(currentPlayMode)
	{
		case playModeSingleShot: 		updateSingleShot(); 		return;
		case playModeLoopForward:		updateLoopForward(); 		return;
		case playModeLoopBackward:		updateLoopBackward(); 		return;
		case playModePingpong:			updateLoopPingpong(); 		return;
		case playModeSlice:				updateSlice();				return;
		case playModeBeatSlice:			updateBeatSlice();			return;
		case playModeGranular:			updateGranular();			return;
		case playModeWavetable:			updateWavetable();			return;
		default: break;
	}
}

//**************************************************************************************UPDATE END
//**************************************************************************************PLAY
uint8_t AudioPlayMemory::play(uint8_t instrIdx,int8_t note)
{
	isFirstUpdateAfterNoteOn = true;
	needWaitOneBuffer = true;
	switch(mtProject.instrument[instrIdx].playMode)
	{
		case playModeSingleShot: 		playSingleShot(instrIdx, note); 		return 1;
		case playModeLoopForward:		playLoopForward(instrIdx, note); 		return 1;
		case playModeLoopBackward:		playLoopBackward(instrIdx, note); 		return 1;
		case playModePingpong:			playLoopPingpong(instrIdx, note); 		return 1;
		case playModeSlice:				playSlice(instrIdx,note);				return 1;
		case playModeBeatSlice:			playBeatSlice(instrIdx,note);			return 1;
		case playModeGranular:			playGranular(instrIdx, note);			return 1;
		case playModeWavetable:			playWavetable(instrIdx, note);			return 1;
		default: break;
	}
}

uint8_t AudioPlayMemory::playForPrev(uint8_t instr_idx,int8_t n)
{
//	__disable_irq();
//
//	uint8_t localSlice;
//	if(n < mtProject.values.padBoardRootNote) localSlice = 0;
//	else if(n > (mtProject.values.padBoardRootNote + mtProject.instrument[instr_idx].sliceNumber - 1 ) ) localSlice = mtProject.instrument[instr_idx].sliceNumber - 1;
//	else localSlice = n - mtProject.values.padBoardRootNote;
//
//	n = 60; // zerowy pitch
//
//	/*========================================================INIT=============================================================*/
//	uint16_t startPoint=0,endPoint=0,loopPoint1=0,loopPoint2=0;
//	uint16_t startSlice = (mtProject.instrument[instr_idx].sliceNumber > 0) ? mtProject.instrument[instr_idx].slices[mtProject.instrument[instr_idx].selectedSlice] : 0;
//	uint16_t endSlice =
//		((mtProject.instrument[instr_idx].sliceNumber > 1 ) &&  ( (mtProject.instrument[instr_idx].sliceNumber - 1) != mtProject.instrument[instr_idx].selectedSlice) ) ?
//		mtProject.instrument[instr_idx].slices[mtProject.instrument[instr_idx].selectedSlice + 1] : MAX_16BIT;
//	uint16_t startGranular = 0, loopPoint1Granular = 0, loopPoint2Granular = 0, endGranular = 0;
//	uint16_t granularLength = ((uint32_t)((uint32_t)mtProject.instrument[instr_idx].granular.grainLength * (uint32_t)MAX_16BIT))/mtProject.instrument[instr_idx].sample.length;
//
//	//************* granular
//	int32_t granularDownConstrain = mtProject.instrument[instr_idx].granular.currentPosition - (granularLength/2);;
//	int32_t granularUpConstrain = mtProject.instrument[instr_idx].granular.currentPosition + (granularLength/2);
//
//	startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
//	loopPoint1Granular = startGranular + 1;
//	endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
//	loopPoint2Granular = endGranular - 1;
//
//	granularLoopType = mtProject.instrument[instr_idx].granular.type;
//	currentGranularPosition = mtProject.instrument[instr_idx].granular.currentPosition;
//	switch(mtProject.instrument[instr_idx].granular.shape)
//	{
//		case granularShapeSquare: 		granularEnvelopeTab = squareTab; 	break;
//		case granularShapeGauss: 		granularEnvelopeTab = gaussTab; 	break;
//		case granularShapeTriangle: 	granularEnvelopeTab = triangleTab; 	break;
//		default: break;
//	}
//	//******************
//
//
//	if(playing == 1) needSmoothingFlag = 1;
//	playing = 0;
//	loopBackwardFlag=0;
//	iPitchCounter=0;
//	fPitchCounter=0;
//	glideCounter=0;
//	currentInstrIdx=instr_idx;
//	lastNote= - 1;
//
//	/*=========================================================================================================================*/
//	/*========================================PRZEPISANIE WARTOSCI ============================================================*/
//	currentGlide=mtProject.instrument[instr_idx].glide;
//	currentTune=mtProject.instrument[instr_idx].tune;
//	currentStartAddress = mtProject.instrument[instr_idx].sample.address;
//
//	currentPlayMode=mtProject.instrument[instr_idx].playMode;
//
//	currentSampleLength=mtProject.instrument[instr_idx].sample.length;
//
//	if( (n + currentTune) > (MAX_NOTE-1))
//	{
//		if(lastNote>n) currentTune=(MAX_NOTE-1)-lastNote;
//		else currentTune=(MAX_NOTE-1)-n;
//	}
//	if( (n + currentTune) < MIN_NOTE)
//	{
//		if((lastNote>=0) && (lastNote<n)) currentTune=MIN_NOTE-lastNote;
//		else currentTune=MIN_NOTE-n;
//	}
//	if(currentPlayMode == playModeWavetable)
//	{
//		switch(mtProject.instrument[instr_idx].sample.wavetable_window_size)
//		{
//			case 32: 	wt_notes = wt32Note; 	break;
//			case 64: 	wt_notes = wt64Note; 	break;
//			case 128:	wt_notes = wt128Note; 	break;
//			case 256:	wt_notes = wt256Note; 	break;
//			case 512: 	wt_notes = wt512Note; 	break;
//			case 1024: 	wt_notes = wt1024Note; 	break;
//			case 2048: 	wt_notes = wt2048Note; 	break;
//			default: break;
//		}
//	}
//
//
//	if(lastNote>=0 && currentGlide != 0 ) pitchControl = (currentPlayMode != playModeWavetable) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
//	else pitchControl =  (currentPlayMode != playModeWavetable) ? (float)notes[n+ currentTune] : (float)wt_notes[n + currentTune];
//
//	int16_t * data = mtProject.instrument[instr_idx].sample.address;
//
//
//
//
//	if(currentPlayMode != playModeWavetable)
//	{
//		startPoint=mtProject.instrument[instr_idx].startPoint;
//		endPoint=mtProject.instrument[instr_idx].endPoint;
//		if(currentPlayMode != playModeSingleShot) //loopMode
//		{
//			loopPoint1=mtProject.instrument[instr_idx].loopPoint1;
//			loopPoint2=mtProject.instrument[instr_idx].loopPoint2;
//		}
//
//	}
//	else
//	{
//		wavetableWindowSize = mtProject.instrument[instr_idx].sample.wavetable_window_size;
//		currentWindow=mtProject.instrument[instr_idx].wavetableCurrentWindow;
//		constrainsInSamples.endPoint=wavetableWindowSize*256; // nie ma znaczenia
//		constrainsInSamples.loopPoint1=0; //currentWindow*wavetableWindowSize;
//		constrainsInSamples.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
//		constrainsInSamples.loopLength=wavetableWindowSize;
//		pointsInSamples.start = 0;
//
//	}
//	/*=========================================================================================================================*/
//	/*========================================WARUNKI LOOPPOINTOW==============================================================*/
//	if(currentPlayMode != playModeWavetable)
//	{
//		if(currentPlayMode == playModeSingleShot)
//		{
//			if (startPoint >= endPoint) return badStartPoint;
//		}
//		else if((currentPlayMode != playModeSlice) && (currentPlayMode != playModeGranular))
//		{
//			if ( (startPoint >= endPoint) || (startPoint > loopPoint1) || (startPoint > loopPoint2) ) return badStartPoint;
//			if ((loopPoint1 > loopPoint2) || (loopPoint1 > endPoint)) return badLoopPoint1;
//			if (loopPoint2 > endPoint) return badLoopPoint2;
//		}
//	}
//
//	/*=========================================================================================================================*/
//	/*====================================================PRZELICZENIA=========================================================*/
//	currentFineTune=mtProject.instrument[instr_idx].fineTune;
//
//
//	if(currentFineTune >= 0)
//	{
//		if((n + currentTune + 1) <= (MAX_NOTE-1))
//		{
//			float localPitch1 = (currentPlayMode != playModeWavetable) ? (float)notes[n + currentTune + 1] : (float)wt_notes[n + currentTune + 1];
//			float localPitch2 = (currentPlayMode != playModeWavetable) ? (float)notes[n + currentTune] : (float)wt_notes[n + currentTune];
//
//			fineTuneControl= currentFineTune * (( localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
//		}
//		else fineTuneControl=0;
//	}
//	else
//	{
//		if((n + currentTune - 1) >= MIN_NOTE)
//		{
//			float localPitch1 = (currentPlayMode != playModeWavetable) ? (float)notes[n + currentTune - 1] : (float)wt_notes[n + currentTune - 1];
//			float localPitch2 = (currentPlayMode != playModeWavetable) ? (float)notes[n + currentTune] : (float)wt_notes[n + currentTune];
//
//			fineTuneControl= (0-currentFineTune) * ((localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
//		}
//		else fineTuneControl=0;
//	}
//
//	pitchControl+=fineTuneControl;
//
//	if(currentGlide)
//	{
//		constrainsInSamples.glide=(uint32_t)((float)currentGlide*44.1);
//
//		float localPitch1 = (currentPlayMode != playModeWavetable) ? (float)notes[n + currentTune] : (float)wt_notes[n + currentTune];
//		float localPitch2 = (currentPlayMode != playModeWavetable) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
//		if((lastNote>=0) && (lastNote != n)) glideControl=(localPitch1 - localPitch2 )/constrainsInSamples.glide;
//		else glideControl=0;
//	}
//	else
//	{
//		constrainsInSamples.glide=0;
//		glideControl=0;
//	}
//
//
//
//
//	if(currentPlayMode != playModeWavetable)
//	{
//		if(currentPlayMode == playModeGranular)
//		{
//			pointsInSamples.start = (uint32_t)((float)startGranular*((float)currentSampleLength/MAX_16BIT));
//			pointsInSamples.end = (uint32_t)((float)endGranular*((float)currentSampleLength/MAX_16BIT));
//
//			pointsInSamples.loop1 = (uint32_t)((float)loopPoint1Granular*((float)currentSampleLength/MAX_16BIT));
//			pointsInSamples.loop2 = (uint32_t)((float)loopPoint2Granular*((float)currentSampleLength/MAX_16BIT));
//
//			constrainsInSamples.loopPoint1=pointsInSamples.loop1-pointsInSamples.start;
//			constrainsInSamples.loopPoint2=pointsInSamples.loop2-pointsInSamples.start;
//			constrainsInSamples.loopLength=pointsInSamples.loop2-pointsInSamples.loop1;
//		}
//		else if(currentPlayMode == playModeSlice)
//		{
//			pointsInSamples.start= (uint32_t)((float)startSlice*((float)currentSampleLength/MAX_16BIT));
//			pointsInSamples.end= (uint32_t)((float)endSlice*((float)currentSampleLength/MAX_16BIT));
//			pointsInSamples.loop1 = 0;
//			pointsInSamples.loop2 = 0;
//		}
//		else
//		{
//			pointsInSamples.start= (uint32_t)((float)startPoint*((float)currentSampleLength/MAX_16BIT));
//			pointsInSamples.end= (uint32_t)((float)endPoint*((float)currentSampleLength/MAX_16BIT));
//			if(currentPlayMode != playModeSingleShot)
//			{
//				pointsInSamples.loop1= (uint32_t)((float)loopPoint1*((float)currentSampleLength/MAX_16BIT));
//				pointsInSamples.loop2= (uint32_t)((float)loopPoint2*((float)currentSampleLength/MAX_16BIT));
//			}
//
//			if((pointsInSamples.start >= currentSampleLength) || (pointsInSamples.loop1>currentSampleLength) || (pointsInSamples.loop2>currentSampleLength) || (pointsInSamples.end>currentSampleLength)) return pointsBeyondFile; // wskazniki za plikiem
//
//			if((currentPlayMode != playModeSingleShot) && (currentPlayMode != playModeSlice))
//			{
//				constrainsInSamples.loopPoint1=pointsInSamples.loop1-pointsInSamples.start;
//				constrainsInSamples.loopPoint2=pointsInSamples.loop2-pointsInSamples.start;
//				constrainsInSamples.loopLength=pointsInSamples.loop2-pointsInSamples.loop1;
//			}
//		}
//
//
//		constrainsInSamples.endPoint=pointsInSamples.end- pointsInSamples.start;
//	}
//
//
//
///*===========================================================================================================================*/
///*============================================PRZEKAZANIE PARAMETROW=========================================================*/
//	if(currentPlayMode != playModeWavetable)
//	{
//		next = data+pointsInSamples.start;
//		length =currentSampleLength-pointsInSamples.start;
//	}
//	else
//	{
//		next = data;
//		length =currentSampleLength;
//	}
//
//	playing = 1;
//
//	return successInit;
//	__enable_irq();
	//todo: clear forced flag dla wszystkich flag?

	play(instr_idx,n);

	lastNote = -1;
	return 1;

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

	currentGlide=0;
	currentTune=0;
	lastNote=-1;

	currentPlayMode=playModeSingleShot;

	currentSampleLength=len;
	currentStartAddress = addr;
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
		switch(mtProject.instrument[currentInstrIdx].sample.wavetable_window_size)
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
	if(lastNote >= 0) pitchControl = (currentPlayMode != playModeWavetable) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (currentPlayMode != playModeWavetable) ? (float)notes[note+ currentTune] : (float)wt_notes[note + currentTune];


	int16_t * data = addr;

	if(currentPlayMode == playModeWavetable)
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=0;
		constrainsInSamples.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		constrainsInSamples.loopPoint1=0; //currentWindow*wavetableWindowSize;
		constrainsInSamples.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		constrainsInSamples.loopLength=wavetableWindowSize;
		pointsInSamples.start = 0;
	}




	startPoint=0;
	endPoint=MAX_16BIT;
	currentFineTune=0;
	fineTuneControl=0;


	pointsInSamples.start= (uint32_t)((float)startPoint*((float)currentSampleLength/MAX_16BIT));
	pointsInSamples.end= (uint32_t)((float)endPoint*((float)currentSampleLength/MAX_16BIT));
	constrainsInSamples.endPoint=pointsInSamples.end- pointsInSamples.start;

	if(currentPlayMode != playModeWavetable)
	{
		next = data+pointsInSamples.start;
		length =currentSampleLength-pointsInSamples.start;
	}
	else
	{
		next = data;
		length =currentSampleLength;
	}

	playing = 1;
	isFirstUpdateAfterNoteOn = true;
	needWaitOneBuffer = true;
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

	currentGlide=0;
	currentTune=0;
	lastNote=-1;
	currentStartAddress = addr;
	currentPlayMode=playModeSingleShot;
	currentSampleLength=len;

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
		switch(mtProject.instrument[currentInstrIdx].sample.wavetable_window_size)
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

	if(lastNote>=0) pitchControl = (currentPlayMode != playModeWavetable) ? (float)notes[lastNote + currentTune] : (float)wt_notes[lastNote + currentTune];
	else pitchControl =  (currentPlayMode != playModeWavetable) ? (float)notes[note+ currentTune] : (float)wt_notes[note + currentTune];


	int16_t * data = addr;


	startPoint=0;
	endPoint=MAX_16BIT;
	currentFineTune=0;
	fineTuneControl=0;
	if(currentPlayMode == playModeWavetable)
	{
		wavetableWindowSize = SERUM_WAVETABLE_WINDOW_LEN;
		if(wavetableWindowForceFlag) currentWindow = forcedWavetableWindow;
		else currentWindow=0;
		constrainsInSamples.endPoint=wavetableWindowSize*256; // nie ma znaczenia
		constrainsInSamples.loopPoint1=0; //currentWindow*wavetableWindowSize;
		constrainsInSamples.loopPoint2=wavetableWindowSize; // (currentWindow+1)*wavetableWindowSize;
		constrainsInSamples.loopLength=wavetableWindowSize;
		pointsInSamples.start = 0;
	}

	pointsInSamples.start= (uint32_t)((float)startPoint*((float)currentSampleLength/MAX_16BIT));
	pointsInSamples.end= (uint32_t)((float)endPoint*((float)currentSampleLength/MAX_16BIT));
	constrainsInSamples.endPoint=pointsInSamples.end- pointsInSamples.start;

	if(currentPlayMode != playModeWavetable)
	{
		next = data+pointsInSamples.start;
		length =currentSampleLength-pointsInSamples.start;
	}
	else
	{
		next = data;
		length =currentSampleLength;
	}

	playing = 1;
	isFirstUpdateAfterNoteOn = true;
	needWaitOneBuffer = true;
	return successInit;

}

//*******************************************************************************************************************

void AudioPlayMemory::stop(void)
{
	__disable_irq();
	playing = 0;
	__enable_irq();
}

void AudioPlayMemory::checkFirstUpdate()
{
	if(isFirstUpdateAfterNoteOn)
	{
		if(needWaitOneBuffer)
		{
			needWaitOneBuffer = false;
			trackEnvelope->noteOn();
			return;
		}
		trackDeclicker->setNextBufferCrossfade();
		isFirstUpdateAfterNoteOn = false;
	}
}




void AudioPlayMemory::setWavetableWindow(int16_t value)
{
	if(mtProject.instrument[currentInstrIdx].playMode != playModeWavetable) return;

	if((uint32_t) value >= mtProject.instrument[currentInstrIdx].sample.wavetableWindowNumber) currentWindow = mtProject.instrument[currentInstrIdx].sample.wavetableWindowNumber - 1;
	else if(value < 0) currentWindow = 0;
	else currentWindow=value;
}


void AudioPlayMemory::setLP1(uint16_t value) // w audio engine zadba zeby zapodac odpowiednia wartosc gdy force
{
	if (!((currentPlayMode == playModeLoopForward) || (currentPlayMode == playModeLoopBackward) || (currentPlayMode == playModePingpong))) return;

	uint32_t lastSamplePointsLoop1 = pointsInSamples.loop1;
	pointsInSamples.loop1 = (uint32_t)((float)value*((float)currentSampleLength/MAX_16BIT));

	if ((pointsInSamples.loop1 < pointsInSamples.start)||(pointsInSamples.loop1 > pointsInSamples.loop2) || (pointsInSamples.loop1 > pointsInSamples.end))
	{
		pointsInSamples.loop1 = lastSamplePointsLoop1;
		return;
	}

	constrainsInSamples.loopPoint1 = pointsInSamples.loop1 - pointsInSamples.start;
	constrainsInSamples.loopLength = pointsInSamples.loop2 - pointsInSamples.loop1;


//	next = mtProject.instrument[currentInstr_idx].sample.address + samplePoints.start;
}
void AudioPlayMemory::setLP2(uint16_t value) // w audio engine zadba zeby zapodac odpowiednia wartosc gdy force
{
	if (!((currentPlayMode == playModeLoopForward) || (currentPlayMode == playModeLoopBackward) || (currentPlayMode == playModePingpong))) return;

	uint32_t lastSamplePointsLoop2 = pointsInSamples.loop2;

	pointsInSamples.loop2 = (uint32_t)((float)value*((float)currentSampleLength/MAX_16BIT));

	if ((pointsInSamples.loop2 < pointsInSamples.start)||(pointsInSamples.loop2 < pointsInSamples.loop1) || (pointsInSamples.loop1 > pointsInSamples.end))
	{
		pointsInSamples.loop2 = lastSamplePointsLoop2;
		return;
	}

	constrainsInSamples.loopPoint2=pointsInSamples.loop2-pointsInSamples.start;
	constrainsInSamples.loopLength=pointsInSamples.loop2-pointsInSamples.loop1;

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
	constrainsInSamples.glide = localSampleConstrainsGlide;
	if((lastNote>=0) && (lastNote != currentNote)) glideControl= localGlideControl;
	else glideControl=0;
	AudioInterrupts();
}

void AudioPlayMemory::setFineTune(int8_t value, int8_t currentNote)
{
	float localPitchControl = pitchControl;
	float localFineTuneControl = fineTuneControl;
	localPitchControl-=localFineTuneControl;
	uint8_t localPlayMode = mtProject.instrument[currentInstrIdx].playMode;

	if(value >= 0)
	{
		if((currentNote + currentTune + 1) <= (MAX_NOTE-1))
		{
			float localPitch1 = (localPlayMode != playModeWavetable) ? (float)notes[currentNote + currentTune + 1] : (float)wt_notes[currentNote + currentTune + 1];
			float localPitch2 = (localPlayMode != playModeWavetable) ? (float)notes[currentNote + currentTune] : (float)wt_notes[currentNote + currentTune];

			localFineTuneControl= value * (( localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
		}
		else localFineTuneControl=0;
	}
	else
	{
		if((currentNote + currentTune - 1) >= MIN_NOTE)
		{
			float localPitch1 = (localPlayMode != playModeWavetable) ? (float)notes[currentNote + currentTune - 1] : (float)wt_notes[currentNote + currentTune - 1];
			float localPitch2 = (localPlayMode != playModeWavetable) ? (float)notes[currentNote + currentTune] : (float)wt_notes[currentNote + currentTune];

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

	if(mtProject.instrument[currentInstrIdx].playMode != playModeGranular)
	{
		granularPositionRefreshFlag = 0;
		return;
	}

	uint16_t granularLength = ((uint32_t)((uint32_t)mtProject.instrument[currentInstrIdx].granular.grainLength * (uint32_t)MAX_16BIT))/mtProject.instrument[currentInstrIdx].sample.length;
	int32_t granularDownConstrain = currentGranularPosition - (granularLength/2);
	int32_t granularUpConstrain = currentGranularPosition  + (granularLength/2);


	uint16_t startGranular = (granularDownConstrain > 0) ? (uint16_t) granularDownConstrain : 0;
	uint16_t loopPoint1Granular = startGranular + 1;
	uint16_t endGranular = (granularUpConstrain < MAX_16BIT) ? (uint16_t)granularUpConstrain : MAX_16BIT;
	uint16_t loopPoint2Granular = endGranular - 1;

	pointsInSamples.start = (uint32_t)((float)startGranular*((float)currentSampleLength/MAX_16BIT));
	pointsInSamples.end = (uint32_t)((float)endGranular*((float)currentSampleLength/MAX_16BIT));

	pointsInSamples.loop1 = (uint32_t)((float)loopPoint1Granular*((float)currentSampleLength/MAX_16BIT));
	pointsInSamples.loop2 = (uint32_t)((float)loopPoint2Granular*((float)currentSampleLength/MAX_16BIT));

	length =currentSampleLength-pointsInSamples.start;

	constrainsInSamples.loopPoint1=pointsInSamples.loop1-pointsInSamples.start;
	constrainsInSamples.loopPoint2=pointsInSamples.loop2-pointsInSamples.start;
	constrainsInSamples.loopLength=pointsInSamples.loop2-pointsInSamples.loop1;
	constrainsInSamples.endPoint=pointsInSamples.end- pointsInSamples.start;


	granularPositionRefreshFlag = 0;
}


void AudioPlayMemory::setGranularPosition(uint16_t val)
{
	if(mtProject.instrument[currentInstrIdx].playMode != playModeGranular) return;

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
	if(mtProject.instrument[currentInstrIdx].playMode != playModeGranular) return;

	granularPositionRefreshFlag = 1;
}
void AudioPlayMemory::setGranularWave(uint8_t type)
{
	if(mtProject.instrument[currentInstrIdx].playMode != playModeGranular) return;
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
	if(mtProject.instrument[currentInstrIdx].playMode != playModeGranular) return;
	granularLoopType = type;
}

void AudioPlayMemory::setTune(int8_t value, int8_t currentNote)
{
	if( (currentNote + value) > (MAX_NOTE-1)) value=(MAX_NOTE-1)-currentNote;
	if( (currentNote + value) < MIN_NOTE) value=MIN_NOTE-currentNote;

	uint8_t localPlaymode = mtProject.instrument[currentInstrIdx].playMode;

	float localPitchControl = pitchControl;
	localPitchControl -= (localPlaymode != playModeWavetable) ? (float)notes[currentNote+currentTune] : (float)wt_notes[currentNote+currentTune];
	localPitchControl += (localPlaymode != playModeWavetable) ? (float)notes[currentNote+value] : (float)wt_notes[currentNote+value];
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
	else forcedStartPoint = mtProject.instrument[currentInstrIdx].startPoint;
	if(lp1 != -1) forcedLoopPoint1 = lp1;
	else forcedLoopPoint1 = mtProject.instrument[currentInstrIdx].loopPoint1;
	if(lp2 != -1) forcedLoopPoint2 = lp2;
	else forcedLoopPoint2 = mtProject.instrument[currentInstrIdx].loopPoint2;
	if(ep != -1) forcedEndPoint = ep;
	else forcedEndPoint = mtProject.instrument[currentInstrIdx].endPoint;
}

void AudioPlayMemory::setWavetableWindowFlag()
{
	wavetableWindowForceFlag = 1;
}
void AudioPlayMemory::clearWavetableWindowFlag()
{
	wavetableWindowForceFlag = 0;
}
void AudioPlayMemory::setForcedWavetableWindow(uint32_t val)
{
	if(val >= mtProject.instrument[currentInstrIdx].sample.wavetableWindowNumber) forcedWavetableWindow = mtProject.instrument[currentInstrIdx].sample.wavetableWindowNumber -1;
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
	return  (uint16_t)(65535 * ((pointsInSamples.start + iPitchCounter)/(float)currentSampleLength));
}


void AudioPlayMemory::setForcedSlice(uint8_t value)
{
	if(mtProject.instrument[currentInstrIdx].sliceNumber == 0) forcedSlice = 0;
	else if(value > mtProject.instrument[currentInstrIdx].sliceNumber - 1) forcedSlice = mtProject.instrument[currentInstrIdx].sliceNumber - 1;
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
void AudioPlayMemory::setCurrentInstrIdx(uint8_t n)
{
	currentInstrIdx = n;
}

void AudioPlayMemory::assignDeclicker(AudioEffectDeclick * ptr)
{
	trackDeclicker = ptr;
}
void AudioPlayMemory::assignEnvelope(AudioEffectEnvelope * ptr)
{
	trackEnvelope = ptr;
}
bool AudioPlayMemory::getWaitOnEnvNoteOnState()
{
	return isFirstUpdateAfterNoteOn;
}

void AudioPlayMemory::clean(void)
{
	if(!playing)
	{
		next=NULL;
		length=0;
		pitchControl = 1;
		iPitchCounter = 0;
		fPitchCounter = 0;
		currentPlayMode = 0;
		playing=0;
		loopBackwardFlag = 0;
		lastNote = -1;
		currentGlide=0;
		glideCounter=0;
		glideControl=0;
		fineTuneControl=0;
		currentTune=0;
		wavetableWindowSize=0;
		currentWindow=0;
		waveTablePosition=0;
//	    wavetableSync=0;
//	    wavetablePWM=0;
//	    wavetableFlip=0;
//	    wavetableQuantize=0;
		currentInstrIdx=0;
		currentFineTune=0;

		pointsInSamples.start=0;
		pointsInSamples.end=0;
		pointsInSamples.loop1=0;
		pointsInSamples.loop2=0;

		constrainsInSamples.loopLength=0;
		constrainsInSamples.loopPoint1=0;
		constrainsInSamples.loopPoint2=0;
		constrainsInSamples.endPoint=0;

		constrainsInSamples.glide=0;

		currentSampleLength=0;
	}

}
//PLAY OBSÓGA OGÓLNA
void AudioPlayMemory::refreshStartParamiters()
{
	if(playing == 1) needSmoothingFlag = 1;
	playing = 0;
	loopBackwardFlag = 0;
	iPitchCounter = 0;
	fPitchCounter = 0;
	glideCounter = 0;

	currentGlide = glideForceFlag ? forcedGlide : mtProject.instrument[currentInstrIdx].glide;

	currentTune = tuneForceFlag ? forcedTune : mtProject.instrument[currentInstrIdx].tune;

	currentFineTune = fineTuneForceFlag ? forcedFineTune : mtProject.instrument[currentInstrIdx].fineTune;

	currentStartAddress = mtProject.instrument[currentInstrIdx].sample.address;

	currentPlayMode = mtProject.instrument[currentInstrIdx].playMode;

	currentSampleLength = mtProject.instrument[currentInstrIdx].sample.length;
}

void AudioPlayMemory::constrainCurrentTune(uint8_t note)
{
	if((note + currentTune) > (MAX_NOTE - 1))
	{
		if(lastNote > note) currentTune = (MAX_NOTE - 1) - lastNote;
		else currentTune = (MAX_NOTE - 1) - note;
	}
	else if((note + currentTune) < MIN_NOTE)
	{
		if((lastNote >= 0) && (lastNote < note)) currentTune = MIN_NOTE - lastNote;
		else currentTune = MIN_NOTE - note;
	}
}

//note + tune
void AudioPlayMemory::setStartPitch(uint8_t note)
{
	if(currentPlayMode == playModeWavetable)
	{
		if((lastNote >= 0) && (currentGlide != 0)) pitchControl = (float)wt_notes[lastNote + currentTune];
		else pitchControl = (float)wt_notes[note+ currentTune];
	}
	else
	{
		if((lastNote >= 0) && (currentGlide != 0)) pitchControl = (float)notes[lastNote + currentTune];
		else pitchControl = (float)notes[note+ currentTune];
	}

}
void AudioPlayMemory::applyFinetuneOnPitch(uint8_t note)
{
	if(currentPlayMode == playModeWavetable)
	{
		if(currentFineTune >= 0)
		{
			if( (note + currentTune + 1) <= (MAX_NOTE-1))
			{
				float localPitch1 = (float)wt_notes[note + currentTune + 1];
				float localPitch2 = (float)wt_notes[note + currentTune];

				fineTuneControl = currentFineTune * ((localPitch1 - localPitch2) / MAX_INSTRUMENT_FINETUNE );
			}
			else fineTuneControl = 0;
		}
		else
		{
			if((note + currentTune - 1) >= MIN_NOTE)
			{
				float localPitch1 = (float)wt_notes[note + currentTune - 1];
				float localPitch2 = (float)wt_notes[note + currentTune];

				fineTuneControl = (-currentFineTune) * ((localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
			}
			else fineTuneControl = 0;
		}

		pitchControl += fineTuneControl;
	}
	else
	{
		if(currentFineTune >= 0)
		{
			if( (note + currentTune + 1) <= (MAX_NOTE-1))
			{
				float localPitch1 = (float)notes[note + currentTune + 1];
				float localPitch2 = (float)notes[note + currentTune];

				fineTuneControl = currentFineTune * ((localPitch1 - localPitch2) / MAX_INSTRUMENT_FINETUNE );
			}
			else fineTuneControl = 0;
		}
		else
		{
			if((note + currentTune - 1) >= MIN_NOTE)
			{
				float localPitch1 = (float)notes[note + currentTune - 1];
				float localPitch2 = (float)notes[note + currentTune];

				fineTuneControl = (-currentFineTune) * ((localPitch1 - localPitch2) /MAX_INSTRUMENT_FINETUNE);
			}
			else fineTuneControl = 0;
		}

		pitchControl += fineTuneControl;
	}
}
void AudioPlayMemory::calculateGlidePitch(uint8_t note)
{
	if(currentPlayMode == playModeWavetable)
	{
		if(currentGlide)
		{
			constrainsInSamples.glide = (uint32_t)((float)currentGlide * 44.1);

			float localPitch1 = (float)wt_notes[note + currentTune];
			float localPitch2 = (float)wt_notes[lastNote + currentTune];
			if((lastNote >= 0) && (lastNote != note)) glideControl = (localPitch1 - localPitch2 ) / constrainsInSamples.glide;
			else glideControl = 0;
		}
		else
		{
			constrainsInSamples.glide = 0;
			glideControl = 0;
		}
	}
	else
	{
		if(currentGlide)
		{
			constrainsInSamples.glide = (uint32_t)((float)currentGlide * 44.1);

			float localPitch1 = (float)notes[note + currentTune];
			float localPitch2 = (float)notes[lastNote + currentTune];
			if((lastNote >= 0) && (lastNote != note)) glideControl = (localPitch1 - localPitch2 ) / constrainsInSamples.glide;
			else glideControl = 0;
		}
		else
		{
			constrainsInSamples.glide = 0;
			glideControl=0;
		}
	}
}
// w
float AudioPlayMemory::getChangeVolumeCoef()
{
	if(lastInstrIdxInPlay == -1) return 1.0f;

	float currentVol = ampLogValues[mtProject.instrument[currentInstrIdxInPlay].volume];
	float lastVol = ampLogValues[mtProject.instrument[lastInstrIdxInPlay].volume];
	float volCoef = lastVol/currentVol;

	return volCoef;
}

