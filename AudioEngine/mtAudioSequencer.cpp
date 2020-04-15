#include "mtAudioEngine.h"


//Najpierw przepisywane są wartosci seqa nawet gdy aktywny jest performanceMode gdyz i tak trzeba zaktualizowac performanceMode bo bedzie wyliczany na podstawie nowych zmienionych wartosci w sequ
void playerEngine::seqFx(uint8_t fx_id, uint8_t fx_val, uint8_t fx_n)
{
	AudioNoInterrupts();
	__disable_irq();

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	endFx(lastSeqFx[fx_n],fx_n);
	switch(fx_id)
	{
		case 0: break; // na 0 mial wywolywac endFx ale wywoluje go zawsze i tak
		case fx_t::FX_TYPE_FILTER_BANDPASS :		fxFilter(fx_val, fx_n, bandPass);		break;
		case fx_t::FX_TYPE_FILTER_HIGHPASS :		fxFilter(fx_val, fx_n, highPass);		break;
		case fx_t::FX_TYPE_FILTER_LOWPASS :			fxFilter(fx_val, fx_n, lowPass);		break;
		case fx_t::FX_TYPE_GLIDE :					fxGlide(fx_val, fx_n);					break;
		case fx_t::FX_TYPE_MICROTUNING :			fxFinetune(fx_val, fx_n);				break;
		case fx_t::FX_TYPE_PANNING :				fxPanning(fx_val, fx_n);				break;
		case fx_t::FX_TYPE_DELAY_SEND :			fxReverbSend(fx_val, fx_n);				break;
		case fx_t::FX_TYPE_REVERSE_PLAYBACK :		fxReversePlayback(fx_val, fx_n);		break;
		case fx_t::FX_TYPE_R1 : break;
		case fx_t::FX_TYPE_R7 : break;
		case fx_t::FX_TYPE_R8 : break;
		case fx_t::FX_TYPE_R9 : break;
		case fx_t::FX_TYPE_R10 : break;
		case fx_t::FX_TYPE_R30 : break;
		case fx_t::FX_TYPE_R31 : break;
		case fx_t::FX_TYPE_POSITION :			    fxPosition(fx_val, fx_n);				break;
		case fx_t::FX_TYPE_VELOCITY:				fxVolume(fx_val, fx_n);					break;
		case fx_t::FX_TYPE_RANDOM_VELOCITY:			fxRandomVolume(fx_val, fx_n);			break;
		case fx_t::FX_TYPE_SAMPLE_SLICE:			fxSampleSlice(fx_val, fx_n);			break;
		case fx_t::FX_TYPE_VOLUME_LFO:				fxVolumeLFO(fx_val, fx_n);				break;
		case fx_t::FX_TYPE_FILTER_LFO:				fxCutoffLFO(fx_val, fx_n);				break;
		case fx_t::FX_TYPE_POSITION_LFO:			fxPositionLFO(fx_val, fx_n);			break;
		case fx_t::FX_TYPE_PANNING_LFO:				fxPanningLFO(fx_val, fx_n);				break;
		default: break;
	}

	lastSeqFx[fx_n] = fx_id;
	lastSeqVal[fx_n] = fx_val;

	AudioInterrupts();
	__enable_irq();
}
//*** Jeżeli kończy się mniej znaczący efekt mimo, że aktywny jest bardziej znaczący efekt to nie wykonujemy żadnej akcji poza wyzerowaniem flag ***/
//*** Jeżeli kończy się bardziej znaczący efekt i aktywny jest mniej znaczący efekt to mniej znaczący efekt przejmuje kontrolę lub performanceMode wykonuje
// swoje operacje na podstawie wartości mniej znaczącego efektu ***/
void playerEngine::endFx(uint8_t fx_id, uint8_t fx_n)
{
	if(isFxVelocity(fx_id)) fx_id = fx_t::FX_TYPE_VELOCITY;

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	switch(fx_id)
	{
		case 0: break;
		case fx_t::FX_TYPE_FILTER_BANDPASS : 			endFxFilter(fx_n);			break;
		case fx_t::FX_TYPE_FILTER_HIGHPASS :			endFxFilter(fx_n);			break;
		case fx_t::FX_TYPE_FILTER_LOWPASS :				endFxFilter(fx_n);			break;
		case fx_t::FX_TYPE_GLIDE:						endFxGlide(fx_n);			break;
		case fx_t::FX_TYPE_MICROTUNING :				endFxFinetune(fx_n);		break;
		case fx_t::FX_TYPE_PANNING :					endFxPanning(fx_n);			break;
		case fx_t::FX_TYPE_DELAY_SEND :				endFxReverbSend(fx_n);		break;
		case fx_t::FX_TYPE_REVERSE_PLAYBACK :			endFxReversePlayback(fx_n);	break;
		case fx_t::FX_TYPE_R1 : break;
		case fx_t::FX_TYPE_R7 : break;
		case fx_t::FX_TYPE_R8 : break;
		case fx_t::FX_TYPE_R9 : break;
		case fx_t::FX_TYPE_R10 : break;
		case fx_t::FX_TYPE_R30 : break;
		case fx_t::FX_TYPE_R31 : break;
		case fx_t::FX_TYPE_POSITION:					endFxPosition(fx_n);		break;
		case fx_t::FX_TYPE_RANDOM_VELOCITY:				endFxRandomVolume(fx_n);	break; // dla porządku - wyzej i tak go nadpisze na zwykle velocity
		case fx_t::FX_TYPE_VELOCITY:					endFxVolume(fx_n);			break;
		case fx_t::FX_TYPE_SAMPLE_SLICE:				endFxSlice(fx_n);			break;
		case fx_t::FX_TYPE_VOLUME_LFO:					endFxVolumeLFO(fx_n);		break;
		case fx_t::FX_TYPE_FILTER_LFO:					endFxCutoffLFO(fx_n);		break;
		case fx_t::FX_TYPE_POSITION_LFO:				endFxPositionLFO(fx_n);		break;
		case fx_t::FX_TYPE_PANNING_LFO:					endFxPanningLFO(fx_n);		break;
		default: break;
	}
}

//************************************SEQ FX HANDLE

void playerEngine::fxFilter(uint8_t fx_val, uint8_t fx_n, uint8_t type)
{
	uint8_t otherFx_n = !fx_n;
	//moglbym offsetowac od najmniejszego fx_t + type ale zakladam ze predzej cos znajdzie sie pomiedzy typami niz beda dla roznych filtrow rozne zakresy stąd fx_t::FX_TYPE_FILTER_BANDPASS
	uint8_t maxFilterFx = sequencer.getFxMax(fx_t::FX_TYPE_FILTER_BANDPASS);
	uint8_t minFilterFx = sequencer.getFxMin(fx_t::FX_TYPE_FILTER_BANDPASS);

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 1;
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 1;
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 1;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.filterCutoff = (fx_val - minFilterFx)/(float)(maxFilterFx - minFilterFx);
		currentSeqModValues.filterType = type;
		currentSeqModValues.filterEnable = 1;
	}
	else if( fx_n == LEAST_SIGNIFICANT_FX)
	{
		if( !trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
			!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
			!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
		{
			currentSeqModValues.filterCutoff = (fx_val - minFilterFx)/(float)(maxFilterFx - minFilterFx);
			currentSeqModValues.filterType = type;
			currentSeqModValues.filterEnable = 1;
		}
	}

	setFxCutoff();
	setFxFilterType();
}

void playerEngine::fxGlide(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxGlide = sequencer.getFxMax(fx_t::FX_TYPE_GLIDE);
	uint8_t minFxGlide = sequencer.getFxMin(fx_t::FX_TYPE_GLIDE);
	uint8_t halfFxGlide = maxFxGlide/2;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.glide = (fx_val > halfFxGlide) ? map(fx_val,halfFxGlide,maxFxGlide,1000,GLIDE_MAX) : map(fx_val,minFxGlide,halfFxGlide,GLIDE_MIN,1000);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
		{
			currentSeqModValues.glide = (fx_val > halfFxGlide) ? map(fx_val,halfFxGlide,maxFxGlide,1000,GLIDE_MAX) : map(fx_val,minFxGlide,halfFxGlide,GLIDE_MIN,1000);
		}
	}
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::glide] = 1;

	setFxGlide();
}

void playerEngine::fxFinetune(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;
	uint8_t maxFxFinetune = sequencer.getFxMax(fx_t::FX_TYPE_MICROTUNING);
	uint8_t minFxFinetune = sequencer.getFxMin(fx_t::FX_TYPE_MICROTUNING);

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.fineTune = map(fx_val,minFxFinetune,maxFxFinetune,MIN_INSTRUMENT_FINETUNE,MAX_INSTRUMENT_FINETUNE);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune])
		{
			currentSeqModValues.fineTune = map(fx_val,minFxFinetune,maxFxFinetune,MIN_INSTRUMENT_FINETUNE,MAX_INSTRUMENT_FINETUNE);
		}
	}
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::fineTune] = 1;

	setFxFinetune();
}

void playerEngine::fxPanning(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxPanning = sequencer.getFxMax(fx_t::FX_TYPE_PANNING);
	uint8_t minFxPanning = sequencer.getFxMin(fx_t::FX_TYPE_PANNING);

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.panning = map(fx_val,minFxPanning,maxFxPanning,PANNING_MIN,PANNING_MAX);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
		{
			currentSeqModValues.panning = map(fx_val,minFxPanning,maxFxPanning,PANNING_MIN,PANNING_MAX);
		}
	}

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::panning] = 1;

	setFxPanning();
}

void playerEngine::fxReverbSend(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxReverbSend = sequencer.getFxMax(fx_t::FX_TYPE_DELAY_SEND);
	uint8_t minFxReverbSend = sequencer.getFxMin(fx_t::FX_TYPE_DELAY_SEND);


	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.delaySend = map(fx_val,minFxReverbSend,maxFxReverbSend,REVERB_SEND_MIN,REVERB_SEND_MAX);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
		{
			currentSeqModValues.delaySend = map(fx_val,minFxReverbSend,maxFxReverbSend,REVERB_SEND_MIN,REVERB_SEND_MAX);
		}
	}
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::reverbSend] = 1;

	setFxReverbSend();
}

void playerEngine::fxReversePlayback(uint8_t fx_val, uint8_t fx_n)
{
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::samplePlaybeckDirection] = 1;

	uint8_t otherFx_n = !fx_n;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.reversePlayback = fx_val;
		setFxReverse();
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection])
		{
			currentSeqModValues.reversePlayback = fx_val;
			setFxReverse();
		}
	}
}
//******* position
void playerEngine::fxPosition(uint8_t fx_val, uint8_t fx_n)
{
	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable) && (mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable))
	{
		fxPositionWavetable(fx_val, fx_n);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		fxPositionGranular(fx_val, fx_n);
	}
	else if((mtProject.instrument[currentInstrument_idx].playMode != playModeSlice) &&
			(mtProject.instrument[currentInstrument_idx].playMode != playModeBeatSlice))
	{
		fxPositionStartPoint(fx_val, fx_n);
	}
}
void playerEngine::fxPositionStartPoint(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxPosition = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
	uint8_t minFxPosition = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::startPoint] = 1;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.startPoint = map(fx_val,minFxPosition,maxFxPosition,0,MAX_16BIT);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
		{
			currentSeqModValues.startPoint = map(fx_val,minFxPosition,maxFxPosition,0,MAX_16BIT);
		}
	}

	setFxStartPoint();
}
void playerEngine::fxPositionWavetable(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxPosition = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
	uint8_t minFxPosition = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.wavetablePosition = map(fx_val,minFxPosition,maxFxPosition,0,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::wavetablePosition])
		{
			currentSeqModValues.wavetablePosition = map(fx_val,minFxPosition,maxFxPosition,0,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
		}
	}

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::wavetablePosition] = 1;

	setFxPositionWavetable();
}
void playerEngine::fxPositionGranular(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxPosition = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
	uint8_t minFxPosition = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.granularPosition = map(fx_val,minFxPosition,maxFxPosition,0,MAX_16BIT);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::granularPosition])
		{
			currentSeqModValues.granularPosition = map(fx_val,minFxPosition,maxFxPosition,0,MAX_16BIT);
		}
	}

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::granularPosition] = 1;

	setFxPositionGranular();
}
//*******
void playerEngine::fxRandomVolume(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t localVolume = 0;
	uint8_t otherFx_n = !fx_n;

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume]) localVolume = currentSeqModValues.volume;
	else localVolume = mtProject.instrument[currentInstrument_idx].volume;

	int8_t maxFxVol = sequencer.getFxMax(fx_t::FX_TYPE_VELOCITY);
	int8_t minFxVol = sequencer.getFxMin(fx_t::FX_TYPE_VELOCITY);
	uint8_t minRand = 0 , maxRand = 0;

	if(localVolume + fx_val > maxFxVol) maxRand = maxFxVol;
	else maxRand = localVolume + fx_val;

	if(localVolume - fx_val < minFxVol) minRand = minFxVol;
	else minRand = localVolume - fx_val;

	localVolume = random(minRand,maxRand);

	fxVolume(localVolume, fx_n);
}
void playerEngine::fxVolume(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxVolume = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
	uint8_t minFxVolume = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::volume] = 1;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.volume = map(fx_val,minFxVolume,maxFxVolume,0,MAX_INSTRUMENT_VOLUME);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
		{
			currentSeqModValues.volume = map(fx_val,minFxVolume,maxFxVolume,0,MAX_INSTRUMENT_VOLUME);
		}
	}

	setFxVolume();
}

void playerEngine::fxSampleSlice(uint8_t fx_val, uint8_t fx_n)
{
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::slice] = 1;

	uint8_t otherFx_n = !fx_n;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.slice = fx_val % (mtProject.instrument[currentInstrument_idx].sliceNumber);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::slice])
		{
			currentSeqModValues.slice = fx_val % (mtProject.instrument[currentInstrument_idx].sliceNumber);
		}
	}

	setFxSlice();
}

void playerEngine::fxVolumeLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 1;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoAmpEnable = 1;
		currentSeqModValues.lfoAmpRate = fx_val;
		setSyncParamsLFO(envAmp);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
		{
			currentSeqModValues.lfoAmpEnable = 1;
			currentSeqModValues.lfoAmpRate = fx_val;
			setSyncParamsLFO(envAmp);
		}
	}
	setFxAmpRateLFO();
}

void playerEngine::fxCutoffLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoCutoff] = 1;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoCutoffEnable = 1;
		currentSeqModValues.lfoCutoffRate = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoCutoff])
		{
			currentSeqModValues.lfoCutoffEnable = 1;
			currentSeqModValues.lfoCutoffRate = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff])
	{
		changeCutoffLfoRatePerformanceMode(performanceMod.lfoCutoffRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff],currentSeqModValues.lfoCutoffRate,0);
		initEnvelopesParamiters(envCutoff, &lfoBasedEnvelope[envCutoff]);
		if((envelopePtr[envCutoff]->isKeyPressed() != 1) && (envelopePtr[envCutoff]->getPhase() == 0))
		{
			envelopePtr[envCutoff]->start();
		}
		else if( (!mtProject.instrument[currentInstrument_idx].envelope[envCutoff].loop) &&
				 (!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoCutoff]))
		{
			envelopePtr[envCutoff]->kill();
			envelopePtr[envCutoff]->start();
		}
	}

	setSyncParamsLFO(envCutoff);
}
//******* position LFO
void playerEngine::fxPositionLFO(uint8_t fx_val, uint8_t fx_n)
{
	if(mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable)
	{
		fxPositionWavetableLFO(fx_val, fx_n);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		fxPositionGranularLFO(fx_val, fx_n);
	}
}
void playerEngine::fxPositionGranularLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoGranularPosition] = 1;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoGranularPositionEnable = 1;
		currentSeqModValues.lfoGranularPositionRate = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoGranularPosition])
		{
			currentSeqModValues.lfoGranularPositionEnable = 1;
			currentSeqModValues.lfoGranularPositionRate = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition])
	{
		changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],currentSeqModValues.lfoGranularPositionRate,0);
		initEnvelopesParamiters(envGranPos, &lfoBasedEnvelope[envGranPos]);
		if((envelopePtr[envGranPos]->isKeyPressed() != 1) && (envelopePtr[envGranPos]->getPhase() == 0))
		{
			envelopePtr[envGranPos]->start();
		}
	}

	setSyncParamsLFO(envGranPos);
}
void playerEngine::fxPositionWavetableLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoWavetablePosition] = 1;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoWavetablePositionEnable = 1;
		currentSeqModValues.lfoWavetablePositionRate = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoWavetablePosition])
		{
			currentSeqModValues.lfoWavetablePositionEnable = 1;
			currentSeqModValues.lfoWavetablePositionRate = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition])
	{
		changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos],currentSeqModValues.lfoWavetablePositionRate,0);
		initEnvelopesParamiters(envWtPos, &lfoBasedEnvelope[envWtPos]);
		if((envelopePtr[envWtPos]->isKeyPressed() != 1) && (envelopePtr[envWtPos]->getPhase() == 0))
		{
			envelopePtr[envWtPos]->start();
		}
	}

	setSyncParamsLFO(envWtPos);
}
//*********
void playerEngine::fxPanningLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoPanning] = 1;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoPanningEnable = 1;
		currentSeqModValues.lfoPanningRate = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoPanning])
		{
			currentSeqModValues.lfoPanningEnable = 1;
			currentSeqModValues.lfoPanningRate = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning])
	{
		changePanningLfoRatePerformanceMode(performanceMod.lfoPanningRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan],currentSeqModValues.lfoPanningRate,0);
		initEnvelopesParamiters(envPan, &lfoBasedEnvelope[envPan]);
		if((envelopePtr[envPan]->isKeyPressed() != 1) && (envelopePtr[envPan]->getPhase() == 0))
		{
			envelopePtr[envPan]->start();
		}
	}

	setSyncParamsLFO(envPan);
}
//************************************************************
void playerEngine::fxFinetuneLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFinetune] = 1;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoFinetuneEnable = 1;
		currentSeqModValues.lfoFinetuneRate = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFinetune])
		{
			currentSeqModValues.lfoFinetuneEnable = 1;
			currentSeqModValues.lfoFinetuneRate = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoFinetune])
	{
		changeFinetuneLfoRatePerformanceMode(performanceMod.lfoFinetuneRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[currentInstrument_idx].lfo[envFinetune],currentSeqModValues.lfoFinetuneRate,0);
		initEnvelopesParamiters(envFinetune, &lfoBasedEnvelope[envFinetune]);
		if((envelopePtr[envFinetune]->isKeyPressed() != 1) && (envelopePtr[envFinetune]->getPhase() == 0))
		{
			envelopePtr[envFinetune]->start();
		}
	}

	setSyncParamsLFO(envFinetune);
}


//*********************************** END SEQ FX

void playerEngine::endFxFilter(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterCutoff] = 0;
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterType] = 0;
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::filterEnable] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
			trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable] &&
			trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
		{
			uint8_t maxFilterFx = sequencer.getFxMax(fx_t::FX_TYPE_FILTER_BANDPASS);
			uint8_t minFilterFx = sequencer.getFxMin(fx_t::FX_TYPE_FILTER_BANDPASS);

			currentSeqModValues.filterCutoff = (lastSeqVal[otherFx_n] - minFilterFx)/(float)(maxFilterFx - minFilterFx);

			currentSeqModValues.filterType = (lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_BANDPASS) ?  bandPass
					: ((lastSeqFx[otherFx_n] == fx_t::FX_TYPE_FILTER_HIGHPASS) ? highPass : lowPass);

			currentSeqModValues.filterEnable = 1;
		}

	}

	if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
		trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable])
	{
		setFxCutoff();
	}
	else
	{
		clearFxCutoff();
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
	{
		setFxFilterType();
	}
	else
	{
		clearFxFilterType();
	}
}

void playerEngine::endFxGlide(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::glide] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
		{
			uint8_t maxFxGlide = sequencer.getFxMax(fx_t::FX_TYPE_GLIDE);
			uint8_t minFxGlide = sequencer.getFxMin(fx_t::FX_TYPE_GLIDE);
			uint8_t halfFxGlide = maxFxGlide/2;

			currentSeqModValues.glide = (lastSeqVal[otherFx_n] > halfFxGlide) ? map(lastSeqVal[otherFx_n],halfFxGlide,maxFxGlide,1000,GLIDE_MAX) : map(lastSeqVal[otherFx_n],minFxGlide,halfFxGlide,GLIDE_MIN,1000);

			setFxGlide();
		}
		else
		{
			clearFxGlide();
		}
	}
	else
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
		{
			clearFxGlide();
		}
	}
}
void playerEngine::endFxFinetune(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::fineTune] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune])
		{
			uint8_t maxFxFinetune = sequencer.getFxMax(fx_t::FX_TYPE_MICROTUNING);
			uint8_t minFxFinetune = sequencer.getFxMin(fx_t::FX_TYPE_MICROTUNING);

			currentSeqModValues.fineTune = map(lastSeqVal[otherFx_n], minFxFinetune, maxFxFinetune, MIN_INSTRUMENT_FINETUNE, MAX_INSTRUMENT_FINETUNE);

			setFxFinetune();
		}
		else
		{
			clearFxFinetune();
		}
	}
	else
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune])
		{
			clearFxFinetune();
		}
	}
}

void playerEngine::endFxPanning(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::panning] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX) // Trzeba zaktualizowac wartosci sequencera
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
		{
			uint8_t maxFxPanning = sequencer.getFxMax(fx_t::FX_TYPE_PANNING);
			uint8_t minFxPanning = sequencer.getFxMin(fx_t::FX_TYPE_PANNING);

			currentSeqModValues.panning = map(lastSeqVal[otherFx_n],minFxPanning,maxFxPanning,PANNING_MIN,PANNING_MAX);
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
	{
		setFxPanning();
	}
	else
	{
		clearFxPanning();
	}
}

void playerEngine::endFxReverbSend(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::reverbSend] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
		{
			uint8_t maxFxReverbSend = sequencer.getFxMax(fx_t::FX_TYPE_DELAY_SEND);
			uint8_t minFxReverbSend = sequencer.getFxMin(fx_t::FX_TYPE_DELAY_SEND);

			currentSeqModValues.delaySend = map(lastSeqVal[otherFx_n],minFxReverbSend,maxFxReverbSend,REVERB_SEND_MIN,REVERB_SEND_MAX);
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
	{
		setFxReverbSend();
	}
	else
	{
		clearFxReverbSend();
	}
}

void playerEngine::endFxReversePlayback(uint8_t fx_n)
{
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::samplePlaybeckDirection] = 0;

	uint8_t otherFx_n = !fx_n;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection])
		{
			currentSeqModValues.reversePlayback = lastSeqVal[otherFx_n];
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection])
	{
		setFxReverse();
	}
	else
	{
		clearFxReverse();
	}
}
//******** position
void playerEngine::endFxPosition(uint8_t fx_n)
{
	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable) && (mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable))
	{
		endFxPositionWavetable(fx_n);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		endFxPositionGranular(fx_n);
	}
	else if((mtProject.instrument[currentInstrument_idx].playMode != playModeSlice) &&
			(mtProject.instrument[currentInstrument_idx].playMode != playModeBeatSlice))
	{
		endFxPositionStartPoint(fx_n);
	}
}
void playerEngine::endFxPositionStartPoint(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::startPoint] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
		{
			uint8_t maxFxPosition = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
			uint8_t minFxPosition = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

			currentSeqModValues.startPoint = map(lastSeqVal[otherFx_n],minFxPosition,maxFxPosition,0,MAX_16BIT);
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
	{
		setFxStartPoint();
	}
	else
	{
		clearFxStartPoint();
	}
}
void playerEngine::endFxPositionGranular(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::granularPosition] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::granularPosition])
		{
			uint8_t maxFxPosition = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
			uint8_t minFxPosition = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

			currentSeqModValues.granularPosition = map(lastSeqVal[otherFx_n],minFxPosition,maxFxPosition,0,MAX_16BIT);
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::granularPosition])
	{
		setFxPositionGranular();
	}
	else
	{
		clearFxPositionGranular();
	}

}
void playerEngine::endFxPositionWavetable(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::wavetablePosition] = 0;
	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::wavetablePosition])
		{
			uint8_t maxFxPosition = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
			uint8_t minFxPosition = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

			currentSeqModValues.wavetablePosition = map(lastSeqVal[otherFx_n],minFxPosition,maxFxPosition,0,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber);
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::wavetablePosition])
	{
		setFxPositionWavetable();
	}
	else
	{
		clearFxPositionWavetable();
	}
}
//*******
void playerEngine::endFxRandomVolume(uint8_t fx_n)
{
	endFxVolume(fx_n);
}

void playerEngine::endFxVolume(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::volume] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX) // Trzeba zaktualizowac wartosci sequencera
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
		{
			uint8_t maxFxVolume = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
			uint8_t minFxVolume = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

			currentSeqModValues.volume = map(lastSeqVal[otherFx_n],minFxVolume,maxFxVolume,0,MAX_INSTRUMENT_VOLUME);
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
	{
		setFxVolume();
	}
	else
	{
		clearFxVolume();
	}


}

void playerEngine::endFxSlice(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::slice] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::slice])
		{
			currentSeqModValues.slice = lastSeqVal[otherFx_n] % mtProject.instrument[currentInstrument_idx].sliceNumber;
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::slice])
	{
		setFxSlice();
	}
	else
	{
		clearFxSlice();
	}
}

void playerEngine::endFxVolumeLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
		{
			syncFxAmpLFO();
			currentSeqModValues.lfoAmpRate = lastSeqVal[otherFx_n];
			currentSeqModValues.lfoAmpEnable = 1;
		}
	}

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
	{
		setFxAmpRateLFO();
	}
	else
	{
		clearFxAmpRateLFO();
	}

	setSyncParamsLFO(envAmp);
}

void playerEngine::endFxCutoffLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoCutoff] = 0;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff])
	{
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoCutoff])
			{
				currentSeqModValues.lfoCutoffRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoCutoffEnable = 1;

				changeCutoffLfoRatePerformanceMode(performanceMod.lfoCutoffRate);
			}
		}
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoCutoff])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.lfoCutoffRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoCutoffEnable = 1;

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff],currentSeqModValues.lfoCutoffRate,0);
				initEnvelopesParamiters(envCutoff, &lfoBasedEnvelope[envCutoff]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff], mtProject.instrument[currentInstrument_idx].lfo[envCutoff].speed,0);
					initEnvelopesParamiters(envCutoff, &lfoBasedEnvelope[envCutoff]);
				}
				else
				{
					initEnvelopesParamiters(envCutoff, &mtProject.instrument[currentInstrument_idx].envelope[envCutoff]);
				}

			}

		}
	}

	setSyncParamsLFO(envCutoff);
}
//***** position lfo
void playerEngine::endFxPositionLFO(uint8_t fx_n)
{
	if(trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoWavetablePosition])
	{
		endFxPositionWavetableLFO(fx_n);
	}
	else if(trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoGranularPosition])
	{
		endFxPositionGranularLFO(fx_n);
	}
}
void playerEngine::endFxPositionGranularLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoGranularPosition] = 0;


	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition])
	{
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoGranularPosition])
			{
				currentSeqModValues.lfoGranularPositionRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoGranularPositionEnable = 1;

				changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);
			}
		}
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoGranularPosition])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.lfoGranularPositionRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoGranularPositionEnable = 1;

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],currentSeqModValues.lfoGranularPositionRate,0);
				initEnvelopesParamiters(envGranPos, &lfoBasedEnvelope[envGranPos]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],mtProject.instrument[currentInstrument_idx].lfo[envGranPos].speed,0);
					initEnvelopesParamiters(envGranPos, &lfoBasedEnvelope[envGranPos]);
				}
				else
				{
					initEnvelopesParamiters(envGranPos, &mtProject.instrument[currentInstrument_idx].envelope[envGranPos]);
				}
			}
		}
	}

	setSyncParamsLFO(envGranPos);
}
void playerEngine::endFxPositionWavetableLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoWavetablePosition] = 0;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition])
	{
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoWavetablePosition])
			{
				currentSeqModValues.lfoWavetablePositionRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoWavetablePositionEnable = 1;

				changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);
			}
		}
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoWavetablePosition])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.lfoWavetablePositionRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoWavetablePositionEnable = 1;

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos],currentSeqModValues.lfoWavetablePositionRate,0);
				initEnvelopesParamiters(envWtPos, &lfoBasedEnvelope[envWtPos]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos],mtProject.instrument[currentInstrument_idx].lfo[envWtPos].speed,0);
					initEnvelopesParamiters(envWtPos, &lfoBasedEnvelope[envWtPos]);
				}
				else
				{
					initEnvelopesParamiters(envWtPos, &mtProject.instrument[currentInstrument_idx].envelope[envWtPos]);
				}

			}
		}
	}

	setSyncParamsLFO(envWtPos);
}
//********
void playerEngine::endFxPanningLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoPanning] = 0;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning])
	{
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoPanning])
			{
				currentSeqModValues.lfoPanningRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoPanningEnable = 1;
			}
		}
		changePanningLfoRatePerformanceMode(performanceMod.lfoPanningRate);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoPanning])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.lfoPanningRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoPanningEnable = 1;

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan],currentSeqModValues.lfoPanningRate, 0);
				initEnvelopesParamiters(envPan, &lfoBasedEnvelope[envPan]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan],mtProject.instrument[currentInstrument_idx].lfo[envPan].speed, 0);
					initEnvelopesParamiters(envPan, &lfoBasedEnvelope[envPan]);
				}
				else
				{
					initEnvelopesParamiters(envPan, &mtProject.instrument[currentInstrument_idx].envelope[envPan]);
				}

			}
		}
	}

	setSyncParamsLFO(envPan);
}

void playerEngine::endFxFinetuneLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoFinetune] = 0;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoFinetune])
	{
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFinetune])
			{
				currentSeqModValues.lfoFinetuneRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoFinetuneEnable = 1;
			}
		}
		changeFinetuneLfoRatePerformanceMode(performanceMod.lfoFinetuneRate);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoFinetune])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.lfoFinetuneRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoFinetuneEnable = 1;

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[currentInstrument_idx].lfo[envFinetune],currentSeqModValues.lfoFinetuneRate, 0);
				initEnvelopesParamiters(envFinetune, &lfoBasedEnvelope[envFinetune]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envFinetune].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envFinetune].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[currentInstrument_idx].lfo[envFinetune],mtProject.instrument[currentInstrument_idx].lfo[envFinetune].speed, 0);
					initEnvelopesParamiters(envFinetune, &lfoBasedEnvelope[envFinetune]);
				}
				else
				{
					initEnvelopesParamiters(envFinetune, &mtProject.instrument[currentInstrument_idx].envelope[envFinetune]);
				}

			}
		}
	}

	setSyncParamsLFO(envFinetune);
}

uint8_t playerEngine::isFxVelocity(uint8_t fx_id)
{
	return (fx_id == fx_t::FX_TYPE_VELOCITY) ||
			(fx_id == fx_t::FX_TYPE_ROLL) ||
			(fx_id == fx_t::FX_TYPE_NONE); // jeśli kontynuacja rolki to brak fxow w komunikacie
}
float playerEngine::getMostSignificantAmount()
{
	float localAmount = 0.0f;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp]  ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp] ||
	   trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] )
	{
		localAmount = mtProject.instrument[currentInstrument_idx].lfo[envAmp].amount;
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
			{
				localAmount = mtProject.instrument[currentInstrument_idx].lfo[envAmp].amount;
			}
			else
			{
				localAmount = mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount;
			}
		}
		else
		{
			localAmount = 1.0f;
		}

	}

	return localAmount;
}

uint8_t playerEngine::getMostSignificantVolume()
{
	uint8_t localVol = 0;
	if(muteState == MUTE_DISABLE)
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
		{
			localVol = currentPerformanceValues.volume;
		}
		else if((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume]) ||
			(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume]))
		{
			localVol = currentSeqModValues.volume;
		}
		else
		{
			localVol = mtProject.instrument[currentInstrument_idx].volume;
		}
	}

	return localVol;
}

void playerEngine::initEnvelopesParamiters(uint8_t n, envelopeGenerator::strEnv * env)
{
	if(n == envAmp)
	{
		envelopeAmpPtr->delay(env->delay);
		envelopeAmpPtr->attack(env->attack);
		envelopeAmpPtr->hold(env->hold);
		envelopeAmpPtr->decay(env->decay);
		envelopeAmpPtr->sustain(env->sustain);
		envelopeAmpPtr->release(env->release);
		envelopeAmpPtr->setLoop(env->loop);

		uint8_t localVol = getMostSignificantVolume();

		ampPtr->gain( ampLogValues[localVol] * env->amount);

	}
	else
	{
		envelopePtr[n]->init(env);
	}
}

bool playerEngine::isActiveEnvelope(uint8_t type)
{
	return ((mtProject.instrument[currentInstrument_idx].envelope[type].enable) ||
			  (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[type]]) ||
			  (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[type]]) ||
			  (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[type]]));
}

void playerEngine::setSyncParamsLFO(uint8_t type)
{
	if(type == envAmp)
	{
		setSyncParamsAmpLFO();
	}
	else
	{

		if((mtProject.instrument[currentInstrument_idx].envelope[type].loop) ||
		  (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[type]]) ||
		  (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[type]]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[type]]))
		{

			uint8_t localRate = mtProject.instrument[currentInstrument_idx].lfo[type].speed;

			if(trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[type]])
			{
				switch(type)
				{
					case envCutoff: 	localRate 	= currentPerformanceValues.lfoCutoffRate;					break;
					case envGranPos: 	localRate 	= currentPerformanceValues.lfoGranularPositionRate;			break;
					case envPan: 		localRate	= currentPerformanceValues.lfoPanningRate;					break;
					case envWtPos: 		localRate	= currentPerformanceValues.lfoWavetablePositionRate;		break;
				}
			}
			else if ( (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[type]]) ||
					  (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[type]]) )
			{
				switch(type)
				{
					case envCutoff: 	localRate 	= currentSeqModValues.lfoCutoffRate;					break;
					case envGranPos: 	localRate 	= currentSeqModValues.lfoGranularPositionRate;			break;
					case envPan: 		localRate	= currentSeqModValues.lfoPanningRate;					break;
					case envWtPos: 		localRate	= currentSeqModValues.lfoWavetablePositionRate;			break;
				}
			}


			envelopePtr[type]->setSyncRate(tempoSyncRatesOthers[localRate]);
			envelopePtr[type]->setSyncStartStep(sequencer.getActualPos());

/*			Magiczne liczby:
			2 - attack,
			3 - hold,
			4 - decay,
			6 - release
*/

			switch(mtProject.instrument[currentInstrument_idx].lfo[type].shape)
			{
			case (int)lfoShapeType::lfoShapeSaw: envelopePtr[type]->setPhaseNumbers(2, -1);	 			break;
			case (int)lfoShapeType::lfoShapeReverseSaw: envelopePtr[type]->setPhaseNumbers(4, -1);		break;
			case (int)lfoShapeType::lfoShapeTriangle: envelopePtr[type]->setPhaseNumbers(2, 4);			break;
			case (int)lfoShapeType::lfoShapeSquare: envelopePtr[type]->setPhaseNumbers(3, 6);			break;
			default:	break;
			}
		}
	}



}

void playerEngine::setSyncParamsAmpLFO()
{
	if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop) ||
	  (trackControlParameter[(int)controlType::performanceMode][envAmp]) ||
	  (trackControlParameter[(int)controlType::sequencerMode][envAmp]) ||
	  (trackControlParameter[(int)controlType::sequencerMode2][envAmp]))
	{

		uint8_t localRate = mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed;

		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
		{
			localRate = currentPerformanceValues.lfoAmpRate;
		}
		else if ( (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp]) ||
				  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp]) )
		{
			localRate = currentSeqModValues.lfoAmpRate;
		}


		envelopeAmpPtr->setSyncRate(tempoSyncRatesAmp[localRate]);
		envelopeAmpPtr->setSyncStartStep(sequencer.getActualPos());

/*			Magiczne liczby:
			2 - attack,
			3 - hold,
			4 - decay,
			6 - release
*/
		switch(mtProject.instrument[currentInstrument_idx].lfo[envAmp].shape)
		{
		case (int)lfoShapeType::lfoShapeSaw: envelopeAmpPtr->setPhaseNumbers(2, -1);	 			break;
		case (int)lfoShapeType::lfoShapeReverseSaw: envelopeAmpPtr->setPhaseNumbers(4, -1);			break;
		case (int)lfoShapeType::lfoShapeTriangle: envelopeAmpPtr->setPhaseNumbers(2, 4);			break;
		case (int)lfoShapeType::lfoShapeSquare: envelopeAmpPtr->setPhaseNumbers(3, 6);				break;
		default:	break;
		}
	}
}
//****************** FUNKCJE NARZUCAJĄCE WARTOSCI W SILNIKU
//Przed wywolaniem tych funkcji nalezy przygotowac wartosci:
// instrumentu, i bieżące z sequencera
//VELOCITY
void playerEngine::setFxVolume()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
	{
		changeVolumePerformanceMode(performanceMod.volume);
	}
	else
	{
		float localAmount = getMostSignificantAmount();

		ampPtr->gain( ampLogValues[getMostSignificantVolume()] * localAmount);
	}
}
void playerEngine::clearFxVolume()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
	{
		changeVolumePerformanceMode(performanceMod.volume);
	}
	else
	{
		float localAmount = getMostSignificantAmount();
		uint8_t localVolume = getMostSignificantVolume();

		ampPtr->gain( ampLogValues[localVolume] * localAmount);
	}
}
//PANNING
void playerEngine::setFxPanning()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else
	{
		modPanning(currentSeqModValues.panning);
	}
}
void playerEngine::clearFxPanning()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else
	{

		modPanning(mtProject.instrument[currentInstrument_idx].panning);
	}
}
//FINETUNE
void playerEngine::setFxFinetune()
{
	playMemPtr->setFineTuneForceFlag();
	playMemPtr->setForcedFineTune(currentSeqModValues.fineTune);
	playMemPtr->setFineTune(currentSeqModValues.fineTune, currentNote);
}
void playerEngine::clearFxFinetune()
{
	playMemPtr->clearFineTuneForceFlag();
	playMemPtr->setFineTune(mtProject.instrument[currentInstrument_idx].fineTune, currentNote);
}
//GLIDE
void playerEngine::setFxGlide()
{
	playMemPtr->setGlideForceFlag();
	playMemPtr->setForcedGlide(currentSeqModValues.glide);
}
void playerEngine::clearFxGlide()
{
	playMemPtr->clearGlideForceFlag();
	playMemPtr->setForcedGlide(0);
}
//REVERSE
void playerEngine::setFxReverse()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection])
	{
		changeSamplePlaybackPerformanceMode(performanceMod.reversePlayback);
	}
	else
	{
		if(currentSeqModValues.reversePlayback) playMemPtr->setReverse();
		else playMemPtr->clearReverse();
	}
}
void playerEngine::clearFxReverse()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection])
	{
		changeSamplePlaybackPerformanceMode(performanceMod.reversePlayback);
	}
	else
	{
		playMemPtr->clearReverse();
	}
}
//POSITION GRANULAR
void playerEngine::setFxPositionGranular()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
	{
		changeGranularPositionPerformanceMode(performanceMod.granularPosition);
	}
	else
	{
		playMemPtr->setGranularPosForceFlag();
		playMemPtr->setForcedGranularPos(currentSeqModValues.granularPosition);
		playMemPtr->setGranularPosition(currentSeqModValues.granularPosition);
	}
}
void playerEngine::clearFxPositionGranular()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.granularPosition);
	}
	else
	{
		uint32_t localGranPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;

		int32_t localGranMod = currentEnvelopeModification[envGranPos] * MAX_16BIT;

		if(localGranPos + localGranMod > MAX_16BIT ) localGranPos = MAX_16BIT;
		else if(localGranPos + localGranMod < 0 ) localGranPos = 0;
		else localGranPos += localGranMod;

		playMemPtr->clearGranularPosForceFlag();
		playMemPtr->setGranularPosition(localGranPos);
	}
}
//POSITION WAVETABLE
void playerEngine::setFxPositionWavetable()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
	}
	else
	{
		playMemPtr->setWavetableWindowFlag();
		playMemPtr->setForcedWavetableWindow(currentSeqModValues.wavetablePosition);
		playMemPtr->setWavetableWindow(currentSeqModValues.wavetablePosition);
	}
}
void playerEngine::clearFxPositionWavetable()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
	}
	else
	{
		uint32_t localWtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
		int32_t localWtMod = currentEnvelopeModification[envWtPos] * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;

		if(localWtPos + localWtMod > mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1 ) localWtPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
		else if(localWtPos + localWtMod < 0 ) localWtPos = 0;
		else localWtPos += localWtMod;

		playMemPtr->clearWavetableWindowFlag();
		playMemPtr->setWavetableWindow(localWtPos);
	}
}
//START POINT
void playerEngine::setFxStartPoint()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
	else
	{
//		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint]) modSeqPoints(currentSeqModValues.startPoint, currentSeqModValues.endPoint);
//		else modSeqPoints(currentSeqModValues.startPoint , NOT_MOD_POINTS);
		modSeqPoints(currentSeqModValues.startPoint , NOT_MOD_POINTS);
	}
}
void playerEngine::clearFxStartPoint()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
	else
	{
		playMemPtr->clearPointsForceFlag();
		playMemPtr->setForcedPoints(-1, -1, -1, -1);
	}

//	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint])
//	{
//		modSeqPoints(NOT_MOD_POINTS,currentSeqModValues.endPoint);
//	}
}
//SLICE
void playerEngine::setFxSlice()
{
	playMemPtr->setSliceForcedFlag();
	playMemPtr->setForcedSlice(currentSeqModValues.slice);
}
void playerEngine::clearFxSlice()
{
	playMemPtr->clearSliceForcedFlag();
}
//REVERB SEND
void playerEngine::setFxReverbSend()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
	{
		changeReverbSendPerformanceMode(performanceMod.reverbSend);
	}
	else
	{
		if(((muteState == MUTE_DISABLE) && (onlyDelayMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
		{
			modDelaySend(currentSeqModValues.delaySend);
		}
		else modDelaySend(AMP_MUTED);
	}
}
void playerEngine::clearFxReverbSend()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
	{
		changeReverbSendPerformanceMode(performanceMod.reverbSend);
	}
	else
	{
		if(((muteState == MUTE_DISABLE) && (onlyDelayMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
		{
			modDelaySend(mtProject.instrument[currentInstrument_idx].delaySend);
		}
		else modDelaySend(AMP_MUTED);
	}
}
//FILTER
void playerEngine::setFxCutoff()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff);
	}
	else
	{
		if(!isActiveEnvelope(envCutoff)) modCutoff(currentSeqModValues.filterCutoff);
		filterConnect();
	}
}
void playerEngine::clearFxCutoff()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff);
	}
	else
	{
		float localCutoff = mtProject.instrument[currentInstrument_idx].cutOff;

		if(currentEnvelopeModification[envCutoff] != 0.0f)
		{
			if(localCutoff + currentEnvelopeModification[envCutoff] > 1.0f) localCutoff = 1.0;
			else if(localCutoff + currentEnvelopeModification[envCutoff] < 0.0f)  localCutoff = 0.0;
			else localCutoff += currentEnvelopeModification[envCutoff];
		}

		if(!isActiveEnvelope(envCutoff))  modCutoff(localCutoff);

		if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
		else filterConnect();
	}
}

void playerEngine::setFxFilterType()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
	{
		changeFilterTypePerformanceMode(performanceMod.filterType);
	}
	else
	{
		changeFilterType(currentSeqModValues.filterType);
	}
}
void playerEngine::clearFxFilterType()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
	{
		changeFilterTypePerformanceMode(performanceMod.filterType);
	}
	else
	{
		changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
	}
}

void playerEngine::setFxAmpRateLFO()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
	{
		changeAmpLfoRatePerformanceMode(performanceMod.lfoAmpRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp],currentSeqModValues.lfoAmpRate, 1);
		initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);
	}

}

void playerEngine::clearFxAmpRateLFO()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
	{
		changeAmpLfoRatePerformanceMode(performanceMod.lfoAmpRate);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed,1);

				initEnvelopesParamiters(envAmp,&lfoBasedEnvelope[envAmp]);

			}
			else
			{
				initEnvelopesParamiters(envAmp,&mtProject.instrument[currentInstrument_idx].envelope[envAmp]);
				if(envelopeAmpPtr->getState() != 0 ) envelopeAmpPtr->setSustain();
			}
		}
		else
		{
			initEnvelopesParamiters(envAmp,(envelopeGenerator::strEnv *)&passEnvelope);
			if(envelopeAmpPtr->getState() != 0 ) envelopeAmpPtr->setSustain();
		}
	}

}

void playerEngine::syncFxAmpLFO()
{
	envelopeAmpPtr->noteOff();
	envelopeAmpPtr->setIdle();
	envelopeAmpPtr->noteOn();
}
