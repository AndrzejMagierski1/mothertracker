#include "mtAudioEngine.h"

//*******************************************change
void playerEngine ::changeVolumePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] != 1) && (value == 0)) return;

	uint8_t volume;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume]) volume = currentSeqModValues.volume;
	else volume = mtProject.instrument[currentInstrument_idx].volume;

	performanceMod.volume = value;
	if(volume + value > MAX_INSTRUMENT_VOLUME) currentPerformanceValues.volume = MAX_INSTRUMENT_VOLUME;
	else if(volume + value < MIN_INSTRUMENT_VOLUME) currentPerformanceValues.volume = MIN_INSTRUMENT_VOLUME;
	else currentPerformanceValues.volume = volume + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] = 1;

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

	if(muteState == MUTE_DISABLE)
	{
		ampPtr->gain(ampLogValues[currentPerformanceValues.volume] * localAmount);
	}

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

	float gainL=0,gainR=0;
	if(currentPerformanceValues.panning < 50)
	{
		gainR=(0 + currentPerformanceValues.panning)/50.0;
		gainL=1.0;
	}
	else if(currentPerformanceValues.panning > 50)
	{
		gainR=1.0;
		gainL=(100 - currentPerformanceValues.panning)/50.0;
	}
	else if(currentPerformanceValues.panning == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(nChannel,gainL);
	mixerR.gain(nChannel,gainR);
}
void playerEngine ::changeTunePerformanceMode(int8_t value)
{
	if((trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] != 1) && (value == 0)) return;
	performanceMod.tune = value;
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
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]	) reverbSend = currentSeqModValues.reverbSend;
	else reverbSend = mtProject.instrument[currentInstrument_idx].reverbSend;

	if(reverbSend + value > REVERB_SEND_MAX) currentPerformanceValues.reverbSend = REVERB_SEND_MAX;
	else if(reverbSend + value < REVERB_SEND_MIN) currentPerformanceValues.reverbSend = REVERB_SEND_MIN;
	else currentPerformanceValues.reverbSend = reverbSend + value;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] = 1;

	if(((muteState == 0) && (onlyReverbMuteState == 0)) || (engine.forceSend == 1))
	{
		mixerReverb.gain(nChannel,currentPerformanceValues.reverbSend/100.0);
	}

}

void playerEngine::changeStartPointPerformanceMode(int32_t value)
{
	changePointsPerformanceMode(value, trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint] ? performanceMod.endPoint : 0);
}
void playerEngine::changeEndPointPerformanceMode(int32_t value)
{
	changePointsPerformanceMode(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint] ? performanceMod.startPoint : 0, value );
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


	if(mtProject.instrument[currentInstrument_idx].playMode != singleShot)
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

	filterPtr->setCutoff(currentPerformanceValues.filterCutoff);

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

	uint32_t wavetableWindow;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::wavetablePosition]) wavetableWindow = currentSeqModValues.wavetablePosition;
	else wavetableWindow = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;

	int32_t valueMap = map(value,-255,255,- mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber,mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber) ;

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

	uint32_t granularPosition;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::granularPosition]) granularPosition = currentSeqModValues.granularPosition;
	else granularPosition = mtProject.instrument[currentInstrument_idx].granular.currentPosition;

	int32_t valueMap = map(value,-255,255,- MAX_16BIT,MAX_16BIT) ;

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
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
	{
		changeGranularPositionPerformanceMode(map(value,-100,100,-MAX_16BIT,MAX_16BIT));
	}
	else if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(map(value,-100,100,-MAX_WAVETABLE_WINDOW,MAX_WAVETABLE_WINDOW));
	}
	else if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
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

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], currentPerformanceValues.lfoAmpRate);
	envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
	envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
	envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
	envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
	envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
	envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
	envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);

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
		localCutoffRate = mtProject.instrument[currentInstrument_idx].lfo[envFilter].speed;
	}

	if(localCutoffRate + value > 19) currentPerformanceValues.lfoCutoffRate = 19;
	else if(localCutoffRate + value < 0) currentPerformanceValues.lfoCutoffRate = 0;
	else currentPerformanceValues.lfoCutoffRate = localCutoffRate + value;

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[currentInstrument_idx].lfo[envFilter], currentPerformanceValues.lfoCutoffRate);

	envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);

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

		calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos], currentPerformanceValues.lfoGranularPositionRate);

		envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);

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

		calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos], currentPerformanceValues.lfoWavetablePositionRate);

		envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);

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

	calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan], currentPerformanceValues.lfoPanningRate);

	envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning] = 1;
}


//*******************************************end
void playerEngine::endVolumePerformanceMode()
{

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume] = 0;

	if(muteState == MUTE_DISABLE)
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

		ampPtr->gain(ampLogValues[mtProject.instrument[currentInstrument_idx].volume] * localAmount);
	}
}
void playerEngine::endPanningPerformanceMode()
{
	int16_t panning;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning]	) panning = currentSeqModValues.panning;
	else panning = mtProject.instrument[currentInstrument_idx].panning;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning] = 0;


	float gainL=0,gainR=0;
	if(panning < 50)
	{
		gainR=(0 + panning)/50.0;
		gainL=1.0;
	}
	else if(panning> 50)
	{
		gainR=1.0;
		gainL=(100 - panning)/50.0;
	}
	else if(panning == 50)
	{
		gainL=1.0; gainR=1.0;
	}

	mixerL.gain(nChannel,gainL);
	mixerR.gain(nChannel,gainR);
}
void playerEngine::endTunePerformanceMode()
{
	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune] = 0;
	playMemPtr->clearTuneForceFlag(); //blokuje zmiane tuna w playMemory


	playMemPtr->setTune(mtProject.instrument[currentInstrument_idx].tune,currentNote);
}
void playerEngine::endReverbSendPerformanceMode()
{
	uint8_t reverbSend;

	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]) reverbSend = currentSeqModValues.reverbSend;
	else reverbSend = mtProject.instrument[currentInstrument_idx].reverbSend;

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend] = 0;

	if(((muteState == 0) && (onlyReverbMuteState == 0)) || (engine.forceSend == 1)) mixerReverb.gain(nChannel,reverbSend/100.0);
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
		filterPtr->setCutoff(mtProject.instrument[currentInstrument_idx].cutOff);
	}
	else
	{
		filterPtr->setCutoff(currentSeqModValues.filterCutoff);
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
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], currentSeqModValues.lfoAmpRate);
		envelopeAmpPtr->delay(lfoBasedEnvelope[envAmp].delay);
		envelopeAmpPtr->attack(lfoBasedEnvelope[envAmp].attack);
		envelopeAmpPtr->hold(lfoBasedEnvelope[envAmp].hold);
		envelopeAmpPtr->decay(lfoBasedEnvelope[envAmp].decay);
		envelopeAmpPtr->sustain(lfoBasedEnvelope[envAmp].sustain);
		envelopeAmpPtr->release(lfoBasedEnvelope[envAmp].release);
		envelopeAmpPtr->setLoop(lfoBasedEnvelope[envAmp].loop);
	}
	else
	{
		uint8_t localVol = 0;

		if((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume]))
		{
			localVol = currentSeqModValues.volume;
		}
		else localVol = mtProject.instrument[currentInstrument_idx].volume;

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

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] = 0;

}
void playerEngine::endCutoffLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envFilter], &mtProject.instrument[currentInstrument_idx].lfo[envFilter], currentSeqModValues.lfoCutoffRate);
		envelopePtr[envFilter]->init(&lfoBasedEnvelope[envFilter]);
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

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff] = 0;
}
void playerEngine::endPositionLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition])
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos], currentSeqModValues.lfoGranularPositionRate);
			envelopePtr[envGranPos]->init(&lfoBasedEnvelope[envGranPos]);
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

		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition] = 0;
	}
	else if (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition])
	{
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos], currentSeqModValues.lfoPanningRate);
			envelopePtr[envWtPos]->init(&lfoBasedEnvelope[envWtPos]);
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

		trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition] = 0;
	}
}
void playerEngine::endPanningLfoRatePerformanceMode()
{
	if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning] ||
	   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
	{
		calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan], currentSeqModValues.lfoPanningRate);
		envelopePtr[envPan]->init(&lfoBasedEnvelope[envPan]);
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

	trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning] = 0;
}
