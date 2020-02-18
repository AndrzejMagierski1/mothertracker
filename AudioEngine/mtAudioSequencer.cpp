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
		case fx_t::FX_TYPE_REVERB_SEND :			fxReverbSend(fx_val, fx_n);				break;
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
	bool isVeloFx = (fx_id == fx_t::FX_TYPE_RANDOM_VELOCITY) || (fx_id == fx_t::FX_TYPE_VELOCITY) || (fx_id == fx_t::FX_TYPE_ROLL);

	if(isVeloFx) fx_id = fx_t::FX_TYPE_VELOCITY;

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
		case fx_t::FX_TYPE_REVERB_SEND :				endFxReverbSend(fx_n);		break;
		case fx_t::FX_TYPE_REVERSE_PLAYBACK :			endFxReversePlayback(fx_n);	break;
		case fx_t::FX_TYPE_R1 : break;
		case fx_t::FX_TYPE_R7 : break;
		case fx_t::FX_TYPE_R8 : break;
		case fx_t::FX_TYPE_R9 : break;
		case fx_t::FX_TYPE_R10 : break;
		case fx_t::FX_TYPE_R30 : break;
		case fx_t::FX_TYPE_R31 : break;
		case fx_t::FX_TYPE_POSITION:					endFxPosition(fx_n);		break;
		case fx_t::FX_TYPE_RANDOM_VELOCITY:				endFxVolume(fx_n);			break; // dla porządku - wyzej i tak go nadpisze na zwykle velocity
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

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff); // nie trzeba zmieniac typu filtra bo jest ustawiony
	}
	else
	{
		filterPtr->setCutoff(currentSeqModValues.filterCutoff);
		changeFilterType(currentSeqModValues.filterType);
		filterConnect();
	}
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

	playMemPtr->setGlideForceFlag();
	playMemPtr->setForcedGlide(currentSeqModValues.glide);
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

	playMemPtr->setFineTuneForceFlag();
	playMemPtr->setForcedFineTune(currentSeqModValues.fineTune);
	playMemPtr->setFineTune(currentSeqModValues.fineTune, currentNote);
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

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else
	{
		modPanning(currentSeqModValues.panning);
	}
}

void playerEngine::fxReverbSend(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	uint8_t maxFxReverbSend = sequencer.getFxMax(fx_t::FX_TYPE_REVERB_SEND);
	uint8_t minFxReverbSend = sequencer.getFxMin(fx_t::FX_TYPE_REVERB_SEND);


	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.reverbSend = map(fx_val,minFxReverbSend,maxFxReverbSend,REVERB_SEND_MIN,REVERB_SEND_MAX);
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
		{
			currentSeqModValues.reverbSend = map(fx_val,minFxReverbSend,maxFxReverbSend,REVERB_SEND_MIN,REVERB_SEND_MAX);
		}
	}
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::reverbSend] = 1;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
	{
		changeReverbSendPerformanceMode(performanceMod.reverbSend);
	}
	else
	{
		if(((muteState == MUTE_DISABLE) && (onlyReverbMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
		{
			modReverbSend(currentSeqModValues.reverbSend);
		}
		else modReverbSend(AMP_MUTED);
	}
}

void playerEngine::fxReversePlayback(uint8_t fx_val, uint8_t fx_n)
{
	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::samplePlaybeckDirection] = 1;

	uint8_t otherFx_n = !fx_n;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(fx_val) playMemPtr->setReverse();
		else playMemPtr->clearReverse();

		currentSeqModValues.reversePlayback = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection])
		{
			if(fx_val) playMemPtr->setReverse();
			else playMemPtr->clearReverse();

			currentSeqModValues.reversePlayback = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection]) changeSamplePlaybackPerformanceMode(performanceMod.reversePlayback);
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
	else if(mtProject.instrument[currentInstrument_idx].playMode != playModeSlice )
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

	if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint]) modSeqPoints(currentSeqModValues.startPoint, currentSeqModValues.endPoint);
	else modSeqPoints(currentSeqModValues.startPoint , NOT_MOD_POINTS);

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
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
	else minFxVol = localVolume - fx_val;

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

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
	{
		changeVolumePerformanceMode(performanceMod.volume);
	}
	else
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
		if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[currentSeqModValues.volume] * localAmount);
		else ampPtr->gain(AMP_MUTED);
	}
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

	playMemPtr->setSliceForcedFlag();
	playMemPtr->setForcedSlice(currentSeqModValues.slice);
}

void playerEngine::fxVolumeLFO(uint8_t fx_val, uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 1;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		currentSeqModValues.lfoAmpEnable = 1;
		currentSeqModValues.lfoAmpRate = fx_val;
	}
	else if(fx_n == LEAST_SIGNIFICANT_FX)
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
		{
			currentSeqModValues.lfoAmpEnable = 1;
			currentSeqModValues.lfoAmpRate = fx_val;
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
	{
		changeAmpLfoRatePerformanceMode(performanceMod.lfoAmpRate);
	}
	else
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp],currentSeqModValues.lfoAmpRate);
		envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
		envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
		envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
		envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
		envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
		envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
		envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);
	}

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
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[currentInstrument_idx].lfo[envFilter],currentSeqModValues.lfoCutoffRate);
		envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);
	}
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
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],currentSeqModValues.lfoGranularPositionRate);
		envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);
	}
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
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos],currentSeqModValues.lfoWavetablePositionRate);
		envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);
	}
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
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan],currentSeqModValues.lfoPanningRate);
		envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);
	}
}
//************************************************************
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

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff);
	}
	else
	{
		if( trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterCutoff] &&
			trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterEnable])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				filterPtr->setCutoff(currentSeqModValues.filterCutoff);
			}
		}
		else
		{
			float localCutoff = mtProject.instrument[currentInstrument_idx].cutOff;

			if(currentEnvelopeModification[envFilter] != 0.0f)
			{
				if(localCutoff + currentEnvelopeModification[envFilter] > 1.0f) localCutoff = 1.0;
				else if(localCutoff + currentEnvelopeModification[envFilter] < 0.0f)  localCutoff = 0.0;
				else localCutoff += currentEnvelopeModification[envFilter];
			}


			filterPtr->setCutoff(localCutoff);
			if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
			else filterConnect();
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
	{
		changeFilterTypePerformanceMode(performanceMod.filterType);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::filterType])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				changeFilterType(currentSeqModValues.filterType);
			}
		}
		else changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
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
			playMemPtr->setForcedGlide(currentSeqModValues.glide);
		}
		else
		{
			playMemPtr->clearGlideForceFlag();
			playMemPtr->setForcedGlide(0);
		}
	}
	else
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::glide])
		{
			playMemPtr->clearGlideForceFlag();
			playMemPtr->setForcedGlide(0);
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

			currentSeqModValues.fineTune = map(lastSeqVal[otherFx_n], minFxFinetune, maxFxFinetune, MIN_INSTRUMENT_FINETUNE,MAX_INSTRUMENT_FINETUNE);
			playMemPtr->setFineTuneForceFlag();
			playMemPtr->setForcedFineTune(currentSeqModValues.fineTune);
		}
		else
		{
			playMemPtr->clearFineTuneForceFlag();
			modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
		}
	}
	else
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::fineTune])
		{
			playMemPtr->clearFineTuneForceFlag();
			modFineTune(mtProject.instrument[currentInstrument_idx].fineTune);
		}
	}
}

void playerEngine::endFxPanning(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::panning] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
		{
			uint8_t maxFxPanning = sequencer.getFxMax(fx_t::FX_TYPE_PANNING);
			uint8_t minFxPanning = sequencer.getFxMin(fx_t::FX_TYPE_PANNING);

			currentSeqModValues.panning = map(lastSeqVal[otherFx_n],minFxPanning,maxFxPanning,PANNING_MIN,PANNING_MAX);
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::panning])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				modPanning(currentSeqModValues.panning);
			}
		}
		else
		{
			modPanning(mtProject.instrument[currentInstrument_idx].panning);
		}
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
			uint8_t maxFxReverbSend = sequencer.getFxMax(fx_t::FX_TYPE_REVERB_SEND);
			uint8_t minFxReverbSend = sequencer.getFxMin(fx_t::FX_TYPE_REVERB_SEND);

			currentSeqModValues.reverbSend = map(lastSeqVal[otherFx_n],minFxReverbSend,maxFxReverbSend,REVERB_SEND_MIN,REVERB_SEND_MAX);
		}
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
	{
		changeReverbSendPerformanceMode(performanceMod.reverbSend);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::reverbSend])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				if(((muteState == MUTE_DISABLE) && (onlyReverbMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
				{
					modReverbSend(currentSeqModValues.reverbSend);
				}
				else modReverbSend(AMP_MUTED);
			}
		}
		else
		{
			if(((muteState == MUTE_DISABLE) && (onlyReverbMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
			{
				modReverbSend(mtProject.instrument[currentInstrument_idx].reverbSend);
			}
			else modReverbSend(AMP_MUTED);
		}
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

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection])
	{
		changeSamplePlaybackPerformanceMode(performanceMod.reversePlayback);
	}
	else
	{
		if(!trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::samplePlaybeckDirection]) playMemPtr->clearReverse();
		else
		{
			if(currentSeqModValues.reversePlayback) playMemPtr->setReverse();
			else playMemPtr->clearReverse();
		}
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
	else if(mtProject.instrument[currentInstrument_idx].playMode != playModeSlice )
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
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::endPoint])
		{
			modSeqPoints(NOT_MOD_POINTS,currentSeqModValues.endPoint);
		}
		else if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::startPoint])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				modSeqPoints(currentSeqModValues.startPoint, NOT_MOD_POINTS);
			}
		}
		else
		{
			playMemPtr->clearPointsForceFlag();
			playMemPtr->setForcedPoints(-1, -1, -1, -1);
		}
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

			playMemPtr->setGranularPosForceFlag();
			playMemPtr->setForcedGranularPos(currentSeqModValues.granularPosition);
			playMemPtr->setGranularPosition(currentSeqModValues.granularPosition);
		}
		else
		{
			playMemPtr->clearGranularPosForceFlag();
			uint32_t localGranPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;

			int32_t localGranMod = currentEnvelopeModification[envGranPos] * MAX_16BIT;

			if(localGranPos + localGranMod > MAX_16BIT ) localGranPos = MAX_16BIT;
			else if(localGranPos + localGranMod < 0 ) localGranPos = 0;
			else localGranPos += localGranMod;
			playMemPtr->setGranularPosition(localGranPos);
		}
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.granularPosition);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::granularPosition])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				playMemPtr->setGranularPosForceFlag();
				playMemPtr->setForcedGranularPos(currentSeqModValues.granularPosition);
			}
		}
		else
		{
			playMemPtr->clearGranularPosForceFlag();
			uint32_t localGranPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;

			int32_t localGranMod = currentEnvelopeModification[envGranPos] * MAX_16BIT;

			if(localGranPos + localGranMod > MAX_16BIT ) localGranPos = MAX_16BIT;
			else if(localGranPos + localGranMod < 0 ) localGranPos = 0;
			else localGranPos += localGranMod;
			playMemPtr->setGranularPosition(localGranPos);
		}
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

			playMemPtr->setWavetableWindowFlag();
			playMemPtr->setForcedWavetableWindow(currentSeqModValues.wavetablePosition);
		}
		else
		{
			playMemPtr->clearWavetableWindowFlag();
			uint32_t localWtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;

			int32_t localWtMod = currentEnvelopeModification[envWtPos] * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;

			if(localWtPos + localWtMod > mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1 ) localWtPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
			else if(localWtPos + localWtMod < 0 ) localWtPos = 0;
			else localWtPos += localWtMod;
			playMemPtr->setWavetableWindow(localWtPos);
		}
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::wavetablePosition])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				playMemPtr->setWavetableWindowFlag();
				playMemPtr->setForcedWavetableWindow(currentSeqModValues.wavetablePosition);
			}
		}
		else
		{
			playMemPtr->clearWavetableWindowFlag();
			uint32_t localWtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;

			int32_t localWtMod = currentEnvelopeModification[envWtPos] * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;

			if(localWtPos + localWtMod > mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1 ) localWtPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
			else if(localWtPos + localWtMod < 0 ) localWtPos = 0;
			else localWtPos += localWtMod;
			playMemPtr->setWavetableWindow(localWtPos);
		}
	}
}
//*******
void playerEngine::endFxVolume(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::volume] = 0;

	if(fx_n == MOST_SIGNIFICANT_FX)
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
		{
			uint8_t maxFxVolume = sequencer.getFxMax(fx_t::FX_TYPE_POSITION);
			uint8_t minFxVolume = sequencer.getFxMin(fx_t::FX_TYPE_POSITION);

			currentSeqModValues.volume = map(lastSeqVal[otherFx_n],minFxVolume,maxFxVolume,0,MAX_INSTRUMENT_VOLUME);
		}
	}

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
	{
		changeVolumePerformanceMode(performanceMod.volume);
	}
	else
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

		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[currentSeqModValues.volume] * localAmount);
			}
		}
		else
		{
			if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[mtProject.instrument[currentInstrument_idx].volume] * localAmount);
		}
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
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			playMemPtr->setSliceForcedFlag();
			playMemPtr->setForcedSlice(currentSeqModValues.slice);
		}
	}
	else
	{
		playMemPtr->clearSliceForcedFlag();
	}
}

void playerEngine::endFxVolumeLFO(uint8_t fx_n)
{
	uint8_t otherFx_n = !fx_n;

	trackControlParameter[(int)controlType::sequencerMode + fx_n][(int)parameterList::lfoAmp] = 0;

	uint8_t localVol = 0;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume]) localVol = currentPerformanceValues.volume;
	else if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::volume]) localVol = currentSeqModValues.volume;
	else localVol = mtProject.instrument[currentInstrument_idx].volume;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
	{
		if(fx_n == MOST_SIGNIFICANT_FX)
		{
			if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
			{
				currentSeqModValues.lfoAmpRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoAmpEnable = 1;
			}
		}

		changeAmpLfoRatePerformanceMode(performanceMod.lfoAmpRate);
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode + otherFx_n][(int)parameterList::lfoAmp])
		{
			if(fx_n == MOST_SIGNIFICANT_FX)
			{
				currentSeqModValues.lfoAmpRate = lastSeqVal[otherFx_n];
				currentSeqModValues.lfoAmpEnable = 1;

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp],currentSeqModValues.lfoAmpRate);

				envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
				envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
				envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
				envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
				envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
				envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
				envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);

				if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[localVol] * lfoBasedEnvelope[envAmp].amount);
				else ampPtr->gain(AMP_MUTED);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp]);

					envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
					envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
					envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
					envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
					envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
					envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
					envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);

					if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[localVol] * mtProject.instrument[currentInstrument_idx].lfo[envAmp].amount);
					else ampPtr->gain(AMP_MUTED);
				}
				else
				{
					envelopeAmpPtr->delay(mtProject.instrument[currentInstrument_idx].envelope[envAmp].delay);
					envelopeAmpPtr->attack(mtProject.instrument[currentInstrument_idx].envelope[envAmp].attack);
					envelopeAmpPtr->hold(mtProject.instrument[currentInstrument_idx].envelope[envAmp].hold);
					envelopeAmpPtr->decay(mtProject.instrument[currentInstrument_idx].envelope[envAmp].decay);
					envelopeAmpPtr->sustain(mtProject.instrument[currentInstrument_idx].envelope[envAmp].sustain);
					envelopeAmpPtr->release(mtProject.instrument[currentInstrument_idx].envelope[envAmp].release);
					envelopeAmpPtr->setLoop(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop);

					if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[localVol] * mtProject.instrument[currentInstrument_idx].envelope[envAmp].amount);
					else ampPtr->gain(AMP_MUTED);
				}

			}
			else
			{
				envelopeAmpPtr->delay(0);
				envelopeAmpPtr->attack(0);
				envelopeAmpPtr->hold(0);
				envelopeAmpPtr->decay(0);
				envelopeAmpPtr->sustain(1.0);
				envelopeAmpPtr->release(0.0f);
				envelopeAmpPtr->setLoop(0);

				if(muteState == MUTE_DISABLE ) ampPtr->gain( ampLogValues[localVol]); //amount = 1;
				else ampPtr->gain(AMP_MUTED);
			}
		}
	}
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

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[currentInstrument_idx].lfo[envFilter],currentSeqModValues.lfoCutoffRate);

				envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[currentInstrument_idx].lfo[envFilter]);
					envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);
				}
				else
				{
					envelopePtr[envFilter]->init(&mtProject.instrument[currentInstrument_idx].envelope[envFilter]);
				}

			}
		}
	}
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

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],currentSeqModValues.lfoGranularPositionRate);

				envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos]);
					envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);
				}
				else
				{
					envelopePtr[envGranPos]->init(&mtProject.instrument[currentInstrument_idx].envelope[envGranPos]);
				}
			}
		}
	}
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

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos],currentSeqModValues.lfoWavetablePositionRate);

				envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos]);
					envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);
				}
				else
				{
					envelopePtr[envWtPos]->init(&mtProject.instrument[currentInstrument_idx].envelope[envWtPos]);
				}

			}
		}
	}
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

				calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan],currentSeqModValues.lfoPanningRate);

				envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan]);
					envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);
				}
				else
				{
					envelopePtr[envPan]->init(&mtProject.instrument[currentInstrument_idx].envelope[envPan]);
				}

			}
		}
	}
}

