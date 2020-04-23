#include "mtAudioEngine.h"

//*******************************************change
void playerEngine ::changeVolumePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] != 1) && (value == 0)) return;

	uint8_t volume = getMostSignificantVolume();
	float localAmount = getMostSignificantAmount();

	ampPtr->gain(ampLogValues[volume] * localAmount);


}
void playerEngine ::changePanningPerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] != 1) && (value == 0)) return;
	performanceMod.panning = value;

	int16_t panning;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning]	) panning = currentSeqModValues.panning;
	else panning = mtProject.instrument[currentInstrument_idx].panning;


	if(panning + value > PANNING_MAX) currentPerformanceValues.panning = PANNING_MAX;
	else if(panning + value < PANNING_MIN) currentPerformanceValues.panning = PANNING_MIN;
	else currentPerformanceValues.panning = panning + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] = 1;

	if(!isActiveEnvelope(envPan)) modPanning(currentPerformanceValues.panning);

}
void playerEngine ::changeTunePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] != 1) && (value == 0)) return;

	performanceMod.tune = value;

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji

	if(mtProject.instrument[currentInstrument_idx].tune + value > MAX_INSTRUMENT_TUNE) currentPerformanceValues.tune = MAX_INSTRUMENT_TUNE;
	else if(mtProject.instrument[currentInstrument_idx].tune + value < MIN_INSTRUMENT_TUNE) currentPerformanceValues.tune = MIN_INSTRUMENT_TUNE;
	else currentPerformanceValues.tune = mtProject.instrument[currentInstrument_idx].tune + value;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] = 1;
	playMemPtr->setTuneForceFlag();
	playMemPtr->setForcedTune(currentPerformanceValues.tune);


	playMemPtr->setTune(currentPerformanceValues.tune,currentNote);
}
void playerEngine ::changeReverbSendPerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] != 1) && (value == 0)) return;
	performanceMod.reverbSend = value;

	uint8_t reverbSend;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]	) reverbSend = currentSeqModValues.delaySend;
	else reverbSend = mtProject.instrument[currentInstrument_idx].delaySend;

	if(reverbSend + value > REVERB_SEND_MAX) currentPerformanceValues.delaySend = REVERB_SEND_MAX;
	else if(reverbSend + value < REVERB_SEND_MIN) currentPerformanceValues.delaySend = REVERB_SEND_MIN;
	else currentPerformanceValues.delaySend = reverbSend + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] = 1;

	if(((muteState == 0) && (onlyDelayMuteState == 0)) || ((engine.forceSend == 1) && !mtProject.values.trackMute[nChannel]))
	{
		modDelaySend(currentPerformanceValues.delaySend);
	}

}

void playerEngine::changeStartPointPerformanceMode(int32_t value)
{
	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	changePointsPerformanceMode(value, trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] ? performanceMod.endPoint : 0);
}
void playerEngine::changeEndPointPerformanceMode(int32_t value)
{
	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	int32_t localEndPointMod = map(value,-100,100,-MAX_16BIT,MAX_16BIT);
	changePointsPerformanceMode(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] ? performanceMod.startPoint : 0, localEndPointMod );
}

void playerEngine::changePointsPerformanceMode(int32_t spValue, int32_t epValue)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] != 1) && (epValue == 0) &&
	  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] != 1) && (spValue == 0)) return;

	performanceMod.endPoint = epValue;
	performanceMod.startPoint = spValue;

	int32_t startPoint,loopPoint1,loopPoint2,endPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint] ||
	 trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::startPoint]) startPoint = currentSeqModValues.startPoint;
	else startPoint = mtProject.instrument[currentInstrument_idx].startPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint1])	loopPoint1 = currentSeqModValues.loopPoint1;
	else loopPoint1 = mtProject.instrument[currentInstrument_idx].loopPoint1;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]) loopPoint2 = currentSeqModValues.loopPoint2;
	else loopPoint2 = mtProject.instrument[currentInstrument_idx].loopPoint2;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint]) endPoint = currentSeqModValues.endPoint;
	else endPoint = mtProject.instrument[currentInstrument_idx].endPoint;

	if(epValue != 0 ) trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] = 1;
	if(spValue != 0 ) 	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] = 1;


	if(mtProject.instrument[currentInstrument_idx].playMode != playModeSingleShot)
	{
		startPoint+=spValue;
		if(startPoint < SAMPLE_POINT_POS_MIN) startPoint = SAMPLE_POINT_POS_MIN;
		else if(startPoint > SAMPLE_POINT_POS_MAX) startPoint = SAMPLE_POINT_POS_MAX;

		endPoint+=epValue;
		if(endPoint < SAMPLE_POINT_POS_MIN) endPoint = SAMPLE_POINT_POS_MIN;
		else if(endPoint > SAMPLE_POINT_POS_MAX) endPoint = SAMPLE_POINT_POS_MAX;

		uint32_t loopSize = loopPoint2 - loopPoint1;

		if(endPoint > loopPoint2)
		{
			if(startPoint > loopPoint1)
			{
				if((int32_t)(startPoint + 1 + loopSize) > (int32_t)endPoint)
				{
					loopPoint2 = endPoint - 1;
					loopPoint1 = loopPoint2 - loopSize;
					startPoint = loopPoint1 > 0 ? loopPoint1 - 1: 0;
				}
				else
				{
					loopPoint1 = startPoint + 1;
					loopPoint2 = loopPoint1 + loopSize;
				}

			}
			// else endPoint > od loopPoint2 , startPoint < od loopPoint1  - nic nie trzeba ruszac
		}
		else
		{
			if(startPoint > loopPoint1)
			{
				startPoint = loopPoint1 > 0 ? loopPoint1 - 1: 0;
				endPoint = loopPoint2 < SAMPLE_POINT_POS_MAX ? loopPoint2 + 1: SAMPLE_POINT_POS_MAX;
			}
			else
			{
				if((int32_t)(endPoint - 1 - loopSize) < (int32_t)startPoint)
				{
					loopPoint1 = startPoint +1;
					loopPoint2 = startPoint + loopSize;
					endPoint = loopPoint2 < SAMPLE_POINT_POS_MAX ? loopPoint2 + 1: SAMPLE_POINT_POS_MAX;
				}
				else
				{
					loopPoint2 = endPoint - 1;
					loopPoint1 = loopPoint2 - loopSize;
				}
			}
		}
	}
	else
	{
		startPoint+=spValue;
		if(startPoint < SAMPLE_POINT_POS_MIN) startPoint = SAMPLE_POINT_POS_MIN;
		else if(startPoint > SAMPLE_POINT_POS_MAX) startPoint = SAMPLE_POINT_POS_MAX;

		endPoint+=epValue;
		if(endPoint < SAMPLE_POINT_POS_MIN) endPoint = SAMPLE_POINT_POS_MIN;
		else if(endPoint > SAMPLE_POINT_POS_MAX) endPoint = SAMPLE_POINT_POS_MAX;
	}

	playMemPtr->setForcedPoints(startPoint,loopPoint1,loopPoint2,endPoint);
	playMemPtr->setPointsForceFlag();
}


void playerEngine ::changeCutoffPerformanceMode(int8_t value) // przed ta funkcja musi byc wowołana funkcja changeFilterTypePerformanceMode
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff] != 1) && (value == 0)) return;
	performanceMod.cutoff = value;

	float cutoff;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff] )
	{
		cutoff = currentSeqModValues.filterCutoff;
	}
	else
	{
		cutoff = mtProject.instrument[currentInstrument_idx].cutOff;
	}

	if(cutoff*100 + value > 100) currentPerformanceValues.filterCutoff = 1.0;
	else if(cutoff*100 + value < 0) currentPerformanceValues.filterCutoff = 0.0;
	else currentPerformanceValues.filterCutoff = cutoff + (value/100.0);


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff] = 1;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterEnable] = 1;
	currentPerformanceValues.filterEnable = 1;
	filterConnect();

	if(!isActiveEnvelope(envCutoff))  modCutoff(currentPerformanceValues.filterCutoff);

}
void playerEngine ::changeFilterTypePerformanceMode(uint8_t mode)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType] != 1) && (mode == 0)) return;
	if((mode < 1) || (mode > 3)) return;

	performanceMod.filterType = mode;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType] = 1;
	currentPerformanceValues.filterType = mode -1;
	changeFilterType(currentPerformanceValues.filterType);
}

void playerEngine ::changeSamplePlaybackPerformanceMode(uint8_t value)
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection] = 1;

	performanceMod.reversePlayback = value;

	if(value)
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::samplePlaybeckDirection])
		{
			if(currentSeqModValues.reversePlayback) playMemPtr->clearReverse();
			else playMemPtr->setReverse();
		}
		else
		{
			playMemPtr->setReverse();
		}
	}
	else
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::samplePlaybeckDirection])
		{
			if(currentSeqModValues.reversePlayback) playMemPtr->setReverse();
			else playMemPtr->clearReverse();
		}
		else
		{
			playMemPtr->clearReverse();
		}
	}
}

void playerEngine::changeWavetableWindowPerformanceMode(int16_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition] != 1) && (value == 0)) return;
	performanceMod.wavetablePosition = value;

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji

	uint32_t wavetableWindow;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::wavetablePosition]) wavetableWindow = currentSeqModValues.wavetablePosition;
	else wavetableWindow = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;

	int32_t valueMap = map(value,-100,100,- mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber) ;


	if(wavetableWindow + valueMap >= mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber) currentPerformanceValues.wavetablePosition = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
	else if(wavetableWindow + valueMap < 0) currentPerformanceValues.wavetablePosition = 0;
	else currentPerformanceValues.wavetablePosition = wavetableWindow + valueMap;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition] = 1;

	playMemPtr->setWavetableWindowFlag();
	playMemPtr->setForcedWavetableWindow(currentPerformanceValues.wavetablePosition);



	playMemPtr->setWavetableWindow(currentPerformanceValues.wavetablePosition);
}

void playerEngine::changeGranularPositionPerformanceMode(int16_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition] != 1) && (value == 0)) return;
	performanceMod.granularPosition = value;

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji

	uint32_t granularPosition;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::granularPosition]) granularPosition = currentSeqModValues.granularPosition;
	else granularPosition = mtProject.instrument[currentInstrument_idx].granular.currentPosition;

	int32_t valueMap = map(value,-100,100,- MAX_16BIT,MAX_16BIT) ;

	if(granularPosition + valueMap >= MAX_16BIT) currentPerformanceValues.granularPosition = MAX_16BIT;
	else if(granularPosition + valueMap < 0) currentPerformanceValues.granularPosition = 0;
	else currentPerformanceValues.granularPosition = granularPosition + valueMap;


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition] = 1;

	playMemPtr->setGranularPosForceFlag();
	playMemPtr->setForcedGranularPos(currentPerformanceValues.granularPosition);

	playMemPtr->setGranularPosition(currentPerformanceValues.granularPosition);
}

void playerEngine::changePositionPerformanceMode(int16_t value)
{
	if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		changeGranularPositionPerformanceMode(value);
	}
	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable)
	{
		changeWavetableWindowPerformanceMode(value);
	}
	else if((mtProject.instrument[currentInstrument_idx].playMode != playModeSlice) &&
			(mtProject.instrument[currentInstrument_idx].playMode != playModeBeatSlice))
	{
		changeStartPointPerformanceMode(map(value,-100,100,-MAX_16BIT,MAX_16BIT));
	}
}

void playerEngine::changeAmpLfoRatePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] != 1) && (value == 0)) return;
	performanceMod.lfoAmpRate = value;

	uint8_t localAmpRate = 0;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp] )
	{
		localAmpRate = currentSeqModValues.lfoAmpRate;
	}
	else
	{
		localAmpRate = mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed;
	}

	if(localAmpRate + value >= 19) currentPerformanceValues.lfoAmpRate = 19;
	else if(localAmpRate + value < 0) currentPerformanceValues.lfoAmpRate = 0;
	else currentPerformanceValues.lfoAmpRate = localAmpRate + value;

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], currentPerformanceValues.lfoAmpRate,1);
	initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);

	setSyncParamsLFO(envAmp);
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] = 1;

}
void playerEngine::changeCutoffLfoRatePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff] != 1) && (value == 0)) return;
	performanceMod.lfoCutoffRate = value;

	uint8_t localCutoffRate = 0;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff] )
	{
		localCutoffRate = currentSeqModValues.lfoCutoffRate;
	}
	else
	{
		localCutoffRate = mtProject.instrument[currentInstrument_idx].lfo[envCutoff].speed;
	}

	if(localCutoffRate + value > 19) currentPerformanceValues.lfoCutoffRate = 19;
	else if(localCutoffRate + value < 0) currentPerformanceValues.lfoCutoffRate = 0;
	else currentPerformanceValues.lfoCutoffRate = localCutoffRate + value;

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff], currentPerformanceValues.lfoCutoffRate,0);
	initEnvelopesParamiters(envCutoff, &lfoBasedEnvelope[envCutoff]);
	setSyncParamsLFO(envCutoff);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff] = 1;
}
void playerEngine::changePositionLfoRatePerformanceMode(int8_t value)
{
	performanceMod.lfoPositionRate = value;

	if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition] != 1) && (value == 0)) return;

		uint8_t localGranularRate = 0;

		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition] )
		{
			localGranularRate = currentSeqModValues.lfoGranularPositionRate ;
		}
		else
		{
			localGranularRate = mtProject.instrument[currentInstrument_idx].lfo[envGranPos].speed;
		}

		if(localGranularRate + value > 19) currentPerformanceValues.lfoGranularPositionRate = 19;
		else if(localGranularRate + value < 0) currentPerformanceValues.lfoGranularPositionRate = 0;
		else currentPerformanceValues.lfoGranularPositionRate = localGranularRate + value;

		calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos], currentPerformanceValues.lfoGranularPositionRate,0);

		initEnvelopesParamiters(envGranPos, &lfoBasedEnvelope[envGranPos]);
		setSyncParamsLFO(envGranPos);

		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition] = 1;
	}

	else if(mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable)
	{
		if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition] != 1) && (value == 0)) return;

		uint8_t localWavetablePosRate = 0;

		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition] )
		{
			localWavetablePosRate = currentSeqModValues.lfoWavetablePositionRate ;
		}
		else
		{
			localWavetablePosRate = mtProject.instrument[currentInstrument_idx].lfo[envWtPos].speed;
		}

		if(localWavetablePosRate + value > 19) currentPerformanceValues.lfoWavetablePositionRate = 19;
		else if(localWavetablePosRate + value < 0) currentPerformanceValues.lfoWavetablePositionRate = 0;
		else currentPerformanceValues.lfoWavetablePositionRate = localWavetablePosRate + value;

		calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos], currentPerformanceValues.lfoWavetablePositionRate,0);

		initEnvelopesParamiters(envWtPos, &lfoBasedEnvelope[envWtPos]);
		setSyncParamsLFO(envWtPos);

		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition] = 1;
	}

}
void playerEngine::changePanningLfoRatePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning] != 1) && (value == 0)) return;
	performanceMod.lfoPanningRate = value;

	uint8_t localPanningRate = 0;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning] )
	{
		localPanningRate = currentSeqModValues.lfoPanningRate ;
	}
	else
	{
		localPanningRate = mtProject.instrument[currentInstrument_idx].lfo[envPan].speed;
	}

	if(localPanningRate + value > 19) currentPerformanceValues.lfoPanningRate = 19;
	else if(localPanningRate + value < 0) currentPerformanceValues.lfoPanningRate = 0;
	else currentPerformanceValues.lfoPanningRate = localPanningRate + value;

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan], currentPerformanceValues.lfoPanningRate,0);
	initEnvelopesParamiters(envPan, &lfoBasedEnvelope[envPan]);
	setSyncParamsLFO(envPan);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning] = 1;
}

void playerEngine::changeFinetuneLfoRatePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoFinetune] != 1) && (value == 0)) return;
	performanceMod.lfoFinetuneRate = value;

	uint8_t localFinetuneRate = 0;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoFinetune] || trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoFinetune] )
	{
		localFinetuneRate = currentSeqModValues.lfoFinetuneRate ;
	}
	else
	{
		localFinetuneRate = mtProject.instrument[currentInstrument_idx].lfo[envFinetune].speed;
	}

	if(localFinetuneRate + value > 19) currentPerformanceValues.lfoFinetuneRate = 19;
	else if(localFinetuneRate + value < 0) currentPerformanceValues.lfoFinetuneRate = 0;
	else currentPerformanceValues.lfoFinetuneRate = localFinetuneRate + value;

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[currentInstrument_idx].lfo[envFinetune], currentPerformanceValues.lfoFinetuneRate,0);
	initEnvelopesParamiters(envFinetune, &lfoBasedEnvelope[envFinetune]);
	setSyncParamsLFO(envFinetune);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoFinetune] = 1;
}


//*******************************************end
void playerEngine::endVolumePerformanceMode()
{

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] = 0;

	float localAmount = getMostSignificantAmount();
	uint8_t localVolume = getMostSignificantVolume();
	ampPtr->gain(ampLogValues[localVolume] * localAmount);

}
void playerEngine::endPanningPerformanceMode()
{
	int16_t panning;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning]	) panning = currentSeqModValues.panning;
	else panning = mtProject.instrument[currentInstrument_idx].panning;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] = 0;

	if(!isActiveEnvelope(envPan))	modPanning(panning);

}
void playerEngine::endTunePerformanceMode()
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] = 0;
	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	playMemPtr->clearTuneForceFlag(); //blokuje zmiane tuna w playMemory


	playMemPtr->setTune(mtProject.instrument[currentInstrument_idx].tune,currentNote);
}
void playerEngine::endReverbSendPerformanceMode()
{
	uint8_t delaySend;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]) delaySend = currentSeqModValues.delaySend;
	else delaySend = mtProject.instrument[currentInstrument_idx].delaySend;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] = 0;

	if(((muteState == 0) && (onlyDelayMuteState == 0)) || ((engine.forceSend == 1) && !mtProject.values.trackMute[nChannel])) modDelaySend(delaySend);
}

void playerEngine::endPointsPerformanceMode()
{
	uint16_t startPoint,loopPoint1,loopPoint2,endPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::startPoint]	) startPoint = currentSeqModValues.startPoint;
	else startPoint = mtProject.instrument[currentInstrument_idx].startPoint;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint1]	) loopPoint1 = currentSeqModValues.loopPoint1;
	else loopPoint1 = mtProject.instrument[currentInstrument_idx].loopPoint1;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]	) loopPoint2 = currentSeqModValues.loopPoint2;
	else loopPoint2 = mtProject.instrument[currentInstrument_idx].loopPoint2;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]	) endPoint = currentSeqModValues.endPoint;
	else endPoint = mtProject.instrument[currentInstrument_idx].endPoint;


	if((!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::startPoint]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint1]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::loopPoint2]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::endPoint]) 		&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::startPoint]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint1]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::loopPoint2]) 	&&
	  (!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::endPoint]))
	{
		playMemPtr->clearPointsForceFlag();
	}
	else
	{
		playMemPtr->setForcedPoints(startPoint, loopPoint1, loopPoint2, endPoint);
		playMemPtr->setPointsForceFlag();
	}


	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::loopPoint1] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::loopPoint2] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] = 0;
}
void playerEngine::endStartPointPerformanceMode()
{
	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
	{
		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] = 0;
		changePointsPerformanceMode(0 , performanceMod.endPoint);
		return;
	}

	endPointsPerformanceMode();
}


void playerEngine::endCutoffPerformanceMode()
{
	if(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] &&
	   !trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff])
	{
		if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
		if(!isActiveEnvelope(envCutoff)) modCutoff(mtProject.instrument[currentInstrument_idx].cutOff);
	}
	else
	{
		if(!isActiveEnvelope(envCutoff)) modCutoff(currentSeqModValues.filterCutoff);
	}

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff] = 0;
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterEnable] = 0;
	currentPerformanceValues.filterEnable = 0;

}
void playerEngine::endFilterTypePerformanceMode()
{

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType] = 0;
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterType] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterType]	) changeFilterType(currentSeqModValues.filterType);
	else changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);

}
void playerEngine ::endSamplePlaybackPerformanceMode()
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::samplePlaybeckDirection] = 0;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::samplePlaybeckDirection] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::samplePlaybeckDirection] )
	{
		if(currentSeqModValues.reversePlayback) playMemPtr->setReverse();
		else playMemPtr->clearReverse();
	}
	else playMemPtr->clearReverse();
}
void playerEngine ::endEndPointPerformanceMode()
{
	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] = 0;
		changePointsPerformanceMode(performanceMod.startPoint, 0);
		return;
	}

	endPointsPerformanceMode();
}
void playerEngine::endWavetableWindowPerformanceMode()
{
	uint32_t wavetableWindow;

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition])
	{
		wavetableWindow = currentSeqModValues.wavetablePosition;
		playMemPtr->setForcedWavetableWindow(wavetableWindow);
	}
	else
	{
		wavetableWindow = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
		playMemPtr->clearWavetableWindowFlag();

	}
	playMemPtr->setWavetableWindow(wavetableWindow);
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition] = 0;
}

void playerEngine::endGranularPositionPerformanceMode()
{
	uint32_t granularPos;

	playMemPtr->setCurrentInstrIdx(currentInstrument_idx); //play mem dopiero aktualizuje index na play, a czasem korzysta sie wczesniej z funkcji

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
	   trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition])
	{
		granularPos = currentSeqModValues.granularPosition;
		playMemPtr->setForcedGranularPos(granularPos);
	}
	else
	{
		granularPos = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
		playMemPtr->clearGranularPosForceFlag();

	}
	playMemPtr->setGranularPosition(granularPos);
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition] = 0;
}

void playerEngine::endPositionPerformanceMode()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
	{
		endGranularPositionPerformanceMode();
	}
	else if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		endWavetableWindowPerformanceMode();
	}
	else if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		endStartPointPerformanceMode();
	}
}

void playerEngine::endAmpLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp])
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], currentSeqModValues.lfoAmpRate,1);
		initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp],mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed,1);
				initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);
			}
			else
			{
				initEnvelopesParamiters(envAmp, &mtProject.instrument[currentInstrument_idx].envelope[envAmp]);
			}
		}
		else
		{
			initEnvelopesParamiters(envAmp, (envelopeGenerator::strEnv *)&passEnvelope);
		}

	}
	setSyncParamsLFO(envAmp);
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] = 0;

}
void playerEngine::endCutoffLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff], currentSeqModValues.lfoCutoffRate,0);
		initEnvelopesParamiters(envCutoff,&lfoBasedEnvelope[envCutoff]);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff],mtProject.instrument[currentInstrument_idx].lfo[envCutoff].speed,0);
				initEnvelopesParamiters(envCutoff,&lfoBasedEnvelope[envCutoff]);
			}
			else
			{
				initEnvelopesParamiters(envCutoff,&mtProject.instrument[currentInstrument_idx].envelope[envCutoff]);
			}

		}
	}

	setSyncParamsLFO(envCutoff);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff] = 0;
}
void playerEngine::endPositionLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition])
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos], currentSeqModValues.lfoGranularPositionRate,0);
			initEnvelopesParamiters(envGranPos,&lfoBasedEnvelope[envGranPos]);
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],mtProject.instrument[currentInstrument_idx].lfo[envGranPos].speed,0);
					initEnvelopesParamiters(envGranPos,&lfoBasedEnvelope[envGranPos]);
				}
				else
				{
					initEnvelopesParamiters(envGranPos,&mtProject.instrument[currentInstrument_idx].envelope[envGranPos]);
				}

			}
		}

		setSyncParamsLFO(envGranPos);

		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition] = 0;
	}
	else if (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition])
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos], currentSeqModValues.lfoWavetablePositionRate,0);
			initEnvelopesParamiters(envWtPos,&lfoBasedEnvelope[envWtPos]);
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos], mtProject.instrument[currentInstrument_idx].lfo[envWtPos].speed,0);
					initEnvelopesParamiters(envWtPos,&lfoBasedEnvelope[envWtPos]);
				}
				else
				{
					initEnvelopesParamiters(envWtPos,&mtProject.instrument[currentInstrument_idx].envelope[envWtPos]);
				}

			}
		}

		setSyncParamsLFO(envWtPos);
		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition] = 0;
	}
}
void playerEngine::endPanningLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan], currentSeqModValues.lfoPanningRate,0);
		initEnvelopesParamiters(envPan,&lfoBasedEnvelope[envPan]);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan], mtProject.instrument[currentInstrument_idx].lfo[envPan].speed,0);
				initEnvelopesParamiters(envPan,&lfoBasedEnvelope[envPan]);
			}
			else
			{
				initEnvelopesParamiters(envPan,&mtProject.instrument[currentInstrument_idx].envelope[envPan]);
			}
		}
	}

	setSyncParamsLFO(envPan);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning] = 0;
}
void playerEngine::endFinetuneLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoFinetune] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoFinetune])
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[currentInstrument_idx].lfo[envFinetune], currentSeqModValues.lfoFinetuneRate,0);
		initEnvelopesParamiters(envFinetune,&lfoBasedEnvelope[envFinetune]);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envFinetune].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envFinetune].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envFinetune], &mtProject.instrument[currentInstrument_idx].lfo[envFinetune], mtProject.instrument[currentInstrument_idx].lfo[envFinetune].speed,0);
				initEnvelopesParamiters(envFinetune,&lfoBasedEnvelope[envFinetune]);
			}
			else
			{
				initEnvelopesParamiters(envFinetune,&mtProject.instrument[currentInstrument_idx].envelope[envFinetune]);
			}
		}
	}

	setSyncParamsLFO(envFinetune);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoFinetune] = 0;
}
