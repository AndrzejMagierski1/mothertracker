#include "mtAudioEngine.h"

void playerEngine:: update()
{
//**************HANDLE UPDATE END RELEASE
	//obsluga tego zdarzenia jako pierwsza zeby zapobiec zawieszaniu nut
	handleUpdateEndReleaseAction();
//***********************
//**************HANDLE UPDATE END PLAY DETECT
	handleUpdateEndPlayDetect();
//***********************
//**************HANDLE UPDATE ENVELOPE
	//stablicowanie warunkow odswiezenia envelopeow
	bool envelopeCondition[ACTIVE_ENVELOPES];
	envelopeCondition[envAmp] = ((mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable == 1) ||
				(trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[envAmp]]) 	||
				(trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[envAmp]]) 	||
				(trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[envAmp]]));
	envelopeCondition[envCutoff] = (mtProject.instrument[currentInstrument_idx].filterEnable == 1);
	envelopeCondition[envPan] = true;
	envelopeCondition[envWtPos] = (mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable);
	envelopeCondition[envGranPos] = ((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) &&
			(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular));
	//*******
	for(uint8_t i = envAmp; i< ACTIVE_ENVELOPES; i++)
	{
		handleUpdateEnvelope(i, envelopeCondition[i]);
	}
//***********************
//**************HANDLE UPDATE REFRESH PARAMS
	if(statusBytes)
	{
		// Jako pierwsza obsluga parametrow sterowanych z envelope
		handleUpdateRefreshCutoff();
		handleUpdateRefreshPanning();
		handleUpdateRefreshGranPos();
		handleUpdateRefreshWtPos();
		//**********
		handleUpdateRefreshLP1();
		handleUpdateRefreshLP2();
		handleUpdateRefreshFinetune();
		handleUpdateRefreshTune();
		handleUpdateRefreshVolume();
		handleUpdateRefreshResonance();
		handleUpdateRefreshReverb();
		handleUpdateRefreshGranLen();
		handleUpdateRefreshGranWave();
		handleUpdateRefreshGranLoop();
		handleUpdateRefreshAmpLFO();
		handleUpdateRefreshCutoffLFO();
		handleUpdateRefreshWtPosLFO();
		handleUpdateRefreshGranPosLFO();
		handleUpdateRefreshPanningLFO();
	}
//***********************
}
void playerEngine::handleUpdateEndPlayDetect()
{
	currentPlayState = playMemPtr->isPlaying();
	if(currentPlayState == 0 && lastPlayState == 1)
	{
		envelopeAmpPtr->noteOff();
		interfacePlayingEndFlag = 1;
	}
	lastPlayState = currentPlayState;
}

void playerEngine::handleUpdateEnvelope(uint8_t type, bool enableCondition)
{
	if(enableCondition)
	{
		if(type == envAmp)
		{
			if( (mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop) ||
			  ( trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp] ) ||
			  ( trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp] ) ||
			  ( trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] ) )
			{
				if(envelopeAmpPtr->getState() != 0 )
				{
					if(sequencer.getSeqState())
					{
						currentSeqTempo = sequencer.getActualTempo();

						if(currentSeqTempo != lastSeqTempo)
						{
							uint8_t mostSignificantLFORate = mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed;
							if( trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
							{
								mostSignificantLFORate = currentPerformanceValues.lfoAmpRate;
							}
							else if (( trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp] ) ||
							( trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp] ) )
							{
								mostSignificantLFORate = currentSeqModValues.lfoAmpRate;
							}
							calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], mostSignificantLFORate);
							initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);
						}

						lastSeqTempo = currentSeqTempo;
					}
				}
			}
		}
		else
		{
			if((mtProject.instrument[currentInstrument_idx].envelope[type].enable)||
			  (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[type]]) ||
			  (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[type]]) ||
			  (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[type]]))
			{
				bool isActiveEnvelope = ((envelopePtr[type]->isKeyPressed() == 1) || (envelopePtr[type]->getPhase() != 0));

				if((mtProject.instrument[currentInstrument_idx].envelope[type].loop) ||
				  (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[type]]) ||
				  (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[type]]) ||
				  (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[type]]))
				{
					if(isActiveEnvelope)
					{
						if(sequencer.getSeqState())
						{
							envelopePtr[type]->syncTrackerSeq(sequencer.getSeqTimer(), sequencer.getActualTempo());
							envelopePtr[type]->setSync(1);
						}
						else
						{
							envelopePtr[type]->setSync(0);
						}
					}
				}
				else
				{
					envelopePtr[type]->setSync(0);
				}
				if(isActiveEnvelope)
				{
					currentEnvelopeModification[type] =  envelopePtr[type]->getOut();
					statusBytes |= envTargetRefreshMask[type];
				}
			}
		}
	}
}

void playerEngine::handleUpdateEndReleaseAction()
{
	if(envelopeAmpPtr->endRelease())
	{
		envelopeAmpPtr->clearEndReleaseFlag();
		interfaceEndReleaseFlag = 1;
		playMemPtr->stop();

		for ( uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++ )
		{
			if((mtProject.instrument[currentInstrument_idx].envelope[i].enable && mtProject.instrument[currentInstrument_idx].envelope[i].loop)||
				  (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[i]]) ||
				  (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[i]]) ||
				  (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[i]]))
				{
					currentEnvelopeModification[i] = 0;
					envelopePtr[i]->stop();
				}
		}
	}
}

void playerEngine::handleUpdateRefreshLP1()
{
	if(statusBytes & LP1_MASK)
	{
		statusBytes &= (~LP1_MASK);
		modLP1(mtProject.instrument[currentInstrument_idx].loopPoint1);
	}
}
void playerEngine::handleUpdateRefreshLP2()
{
	if(statusBytes & LP2_MASK)
	{
		statusBytes &= (~LP2_MASK);
		modLP2(mtProject.instrument[currentInstrument_idx].loopPoint2);
	}
}
void playerEngine::handleUpdateRefreshFinetune()
{
	if(statusBytes & FINETUNE_MASK)
	{
		statusBytes &= (~FINETUNE_MASK);

		int8_t fineTune;
		if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::fineTune] ||
		   trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::fineTune] ) fineTune = currentSeqModValues.fineTune;
		else fineTune = mtProject.instrument[currentInstrument_idx].fineTune;


		if(fineTune > MAX_INSTRUMENT_FINETUNE)
		{
			playMemPtr->setForcedFineTune(MAX_INSTRUMENT_FINETUNE);
			modFineTune(MAX_INSTRUMENT_FINETUNE);
		}
		else if(fineTune < MIN_INSTRUMENT_FINETUNE)
		{
			playMemPtr->setForcedFineTune(MIN_INSTRUMENT_FINETUNE);
			modFineTune(MIN_INSTRUMENT_FINETUNE);
		}
		else
		{
			playMemPtr->setForcedFineTune(fineTune);
			modFineTune(fineTune);
		}
	}
}
void playerEngine::handleUpdateRefreshTune()
{
	if(statusBytes & TUNE_MASK)
	{
		statusBytes &= (~TUNE_MASK);
		modTune(mtProject.instrument[currentInstrument_idx].tune);
	}
}
void playerEngine::handleUpdateRefreshVolume()
{
	if(statusBytes & VOLUME_MASK)
	{
		statusBytes &= (~VOLUME_MASK);

		if(muteState == 0)
		{
			uint8_t volume = getMostSignificantVolume();
			float localAmount = getMostSignificantAmount();
			ampPtr->gain( ampLogValues[volume] * localAmount );
		}
	}
}
void playerEngine::handleUpdateRefreshPanning()
{
	if(statusBytes & PANNING_MASK)
	{
		statusBytes &= (~PANNING_MASK);
		if((!mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
		&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning])
	    &&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
		&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
		{
			currentEnvelopeModification[envPan] = 0;
		}

		int32_t localPanningMod = currentEnvelopeModification[envPan] * PANNING_MAX;

		int16_t localPanning = 0;

		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
		{
			localPanning = currentPerformanceValues.panning;
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning])
		{
			localPanning = currentSeqModValues.panning;
		}
		else
		{
			localPanning = mtProject.instrument[currentInstrument_idx].panning;
		}

		if((int)(localPanning + localPanningMod) < PANNING_MIN) localPanning = PANNING_MIN;
		else if((int)(localPanning+ localPanningMod) >= PANNING_MAX) localPanning = PANNING_MAX;
		else localPanning += localPanningMod;

		modPanning(localPanning);
	}
}
void playerEngine::handleUpdateRefreshCutoff()
{
	if(statusBytes & CUTOFF_MASK)
	{
		statusBytes &= (~CUTOFF_MASK);
		if((!mtProject.instrument[currentInstrument_idx].envelope[envCutoff].enable)
		&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff])
		&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
		&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
		{
			currentEnvelopeModification[envCutoff] = 0;
		}

		float localCutoff = 0.0f;

		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
		{
			localCutoff = currentPerformanceValues.filterCutoff;
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff])
		{
			localCutoff = currentSeqModValues.filterCutoff;
		}
		else
		{
			localCutoff = mtProject.instrument[currentInstrument_idx].cutOff;
		}

		if(localCutoff + currentEnvelopeModification[envCutoff] < 0.0f) localCutoff = 0.0f;
		else if(localCutoff + currentEnvelopeModification[envCutoff] > 1.0f) localCutoff = 1.0f;
		else localCutoff += currentEnvelopeModification[envCutoff];

		modCutoff(localCutoff);
	}
}
void playerEngine::handleUpdateRefreshResonance()
{
	if(statusBytes & RESONANCE_MASK)
	{
		statusBytes &= (~RESONANCE_MASK);
		modResonance(mtProject.instrument[currentInstrument_idx].resonance);
	}
}
void playerEngine::handleUpdateRefreshReverb()
{
	if(statusBytes & REVERB_SEND_MASK)
	{
		statusBytes &= (~REVERB_SEND_MASK);

		uint8_t localReverbSend = 0;

		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
		{
			localReverbSend = currentPerformanceValues.reverbSend;
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend])
		{
			localReverbSend = currentSeqModValues.reverbSend;
		}
		else
		{
			localReverbSend = mtProject.instrument[currentInstrument_idx].reverbSend;
		}

		if(((muteState == 0) && (onlyReverbMuteState == 0)) || (engine.forceSend == 1))
		{
			modReverbSend(localReverbSend);
		}
	}
}
void playerEngine::handleUpdateRefreshWtPos()
{
	if(statusBytes & WT_POS_SEND_MASK)
	{
		statusBytes &= (~WT_POS_SEND_MASK);
		if((!mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
		&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition])
		&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
		&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
		{
			currentEnvelopeModification[envWtPos] = 0;
		}

		int32_t localWtPosisionMod = currentEnvelopeModification[envWtPos] * mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber;
		uint32_t localWtPos = 0;

		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
		{
			localWtPos = currentPerformanceValues.wavetablePosition;
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::wavetablePosition] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::wavetablePosition])
		{
			localWtPos = currentSeqModValues.wavetablePosition;
		}
		else
		{
			localWtPos = mtProject.instrument[currentInstrument_idx].wavetableCurrentWindow;
		}

		if( (int)(localWtPos + localWtPosisionMod) < 0)
		{
			localWtPos = 0;
		}
		else if( (int)(localWtPos+ localWtPosisionMod) > (int)(mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1))
		{
			localWtPos = mtProject.instrument[currentInstrument_idx].sample.wavetableWindowNumber - 1;
		}
		else localWtPos += localWtPosisionMod;

		playMemPtr->setWavetableWindow(localWtPos);
	}
}
void playerEngine::handleUpdateRefreshGranPos()
{
	if(statusBytes & GRANULAR_POS_SEND_MASK)
	{
		statusBytes &= (~GRANULAR_POS_SEND_MASK);
		if((!mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
		&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition])
		&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
		&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition]))
		{
			currentEnvelopeModification[envGranPos] = 0;
		}

		int32_t localGranPosisionMod = currentEnvelopeModification[envGranPos] * MAX_16BIT;

		uint16_t localGranPosition = 0;
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::granularPosition])
		{
			localGranPosition = currentPerformanceValues.granularPosition;
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::granularPosition] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::granularPosition])
		{
			localGranPosition = currentSeqModValues.granularPosition;
		}
		else
		{
			localGranPosition = mtProject.instrument[currentInstrument_idx].granular.currentPosition;
		}

		if((int)(localGranPosition + localGranPosisionMod) < 0) localGranPosition = 0;
		else if((int)(localGranPosition + localGranPosisionMod) >= MAX_16BIT) localGranPosition = MAX_16BIT;
		else localGranPosition += localGranPosisionMod;

		modGranularPosition(localGranPosition);
	}
}
void playerEngine::handleUpdateRefreshGranLen()
{
	if(statusBytes & GRANULAR_LEN_SEND_MASK)
	{
		statusBytes &= (~GRANULAR_LEN_SEND_MASK);
		modGranularGrainLength();
	}
}
void playerEngine::handleUpdateRefreshGranWave()
{
	if(statusBytes & GRANULAR_WAVE_SEND_MASK)
	{
		statusBytes &= (~GRANULAR_WAVE_SEND_MASK);
		playMemPtr->setGranularWave(mtProject.instrument[currentInstrument_idx].granular.shape);
	}
}
void playerEngine::handleUpdateRefreshGranLoop()
{
	if(statusBytes & GRANULAR_LOOP_SEND_MASK)
	{
		statusBytes &= (~GRANULAR_LOOP_SEND_MASK);
		playMemPtr->setGranularLoopMode(mtProject.instrument[currentInstrument_idx].granular.type);
	}
}
void playerEngine::handleUpdateRefreshAmpLFO()
{
	if(statusBytes & LFO_AMP_SEND_MASK)
	{
		statusBytes &= (~LFO_AMP_SEND_MASK);

		if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
		&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp])
		&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp])
		&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp]))
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp],mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed );
				initEnvelopesParamiters(envAmp, &lfoBasedEnvelope[envAmp]);
			}
		}
	}
}
void playerEngine::handleUpdateRefreshCutoffLFO()
{
	if(statusBytes & LFO_FILTER_SEND_MASK)
	{
		statusBytes &= (~LFO_FILTER_SEND_MASK);

		if(mtProject.instrument[currentInstrument_idx].filterEnable)
		{
			if((mtProject.instrument[currentInstrument_idx].envelope[envCutoff].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envCutoff].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envCutoff], &mtProject.instrument[currentInstrument_idx].lfo[envCutoff],mtProject.instrument[currentInstrument_idx].lfo[envCutoff].speed);
					initEnvelopesParamiters(envCutoff, &lfoBasedEnvelope[envCutoff]);
					setSyncParamsLFO(envCutoff);
				}
			}
		}

	}
}
void playerEngine::handleUpdateRefreshWtPosLFO()
{
	if(statusBytes & LFO_WT_POS_SEND_MASK)
	{
		statusBytes &= (~LFO_WT_POS_SEND_MASK);

		if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
		{
			if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envWtPos], &mtProject.instrument[currentInstrument_idx].lfo[envWtPos],mtProject.instrument[currentInstrument_idx].lfo[envWtPos].speed);
					initEnvelopesParamiters(envWtPos, &lfoBasedEnvelope[envWtPos]);
					setSyncParamsLFO(envWtPos);
				}
			}
		}
	}
}
void playerEngine::handleUpdateRefreshGranPosLFO()
{
	if(statusBytes & LFO_GRAN_POS_SEND_MASK)
	{
		statusBytes &= (~LFO_GRAN_POS_SEND_MASK);
		if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
		{
			if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable)
			&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition])
			&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
			&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
			{
				if(mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
				{
					calcLfoBasedEnvelope(&lfoBasedEnvelope[envGranPos], &mtProject.instrument[currentInstrument_idx].lfo[envGranPos],mtProject.instrument[currentInstrument_idx].lfo[envGranPos].speed);
					initEnvelopesParamiters(envGranPos, &lfoBasedEnvelope[envGranPos]);
					setSyncParamsLFO(envGranPos);
				}
			}

		}
	}
}
void playerEngine::handleUpdateRefreshPanningLFO()
{
	if(statusBytes & LFO_PANNING_SEND_MASK)
	{
		statusBytes &= (~LFO_PANNING_SEND_MASK);

		if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable)
		&&(!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning])
		&&(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
		&&(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[envPan], &mtProject.instrument[currentInstrument_idx].lfo[envPan],mtProject.instrument[currentInstrument_idx].lfo[envPan].speed);
				initEnvelopesParamiters(envPan, &lfoBasedEnvelope[envPan]);
				setSyncParamsLFO(envPan);
			}
		}

	}
}

