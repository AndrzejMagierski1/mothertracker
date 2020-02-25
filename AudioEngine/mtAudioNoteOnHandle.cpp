#include "mtAudioEngine.h"
#include "mtSequencer.h"


//********************************** NOTE_ON
uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity)
{
	if(mtProject.instrument[instr_idx].isActive != 1) return 0;

	engine.clearReverb();

	endFx(lastSeqFx[0],0);
	endFx(lastSeqFx[1],1);

	__disable_irq();
	AudioNoInterrupts();

	uint8_t status;

	currentInstrument_idx=instr_idx;
	currentNote=note;

	handleInitNoteOnAllEnvelopes(); // kill + init

	handleNoteOnFilter();

	handleNoteOnGain();

	handleNoteOnPanning();

	handleNoteOnReverbSend();


	status = playMemPtr->play(instr_idx,note);
	envelopePtr[envAmp]->start();

	for(uint8_t i = 0; i < ENVELOPES_WITHOUT_AMP_MAX; i++)
	{
		if(mtProject.instrument[instr_idx].envelope[envelopesWithoutAmpIdx[i]].enable) envelopePtr[envelopesWithoutAmpIdx[i]]->start();
	}

	__enable_irq();
	AudioInterrupts();
	return status;
}
//**********************************
//********************************** NOTE_ON FX
uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity, uint8_t fx1_id, uint8_t fx1_val,
								uint8_t fx2_id, uint8_t fx2_val)
{
	if(mtProject.instrument[instr_idx].isActive != 1) return 0;

	__disable_irq();
	AudioNoInterrupts();
	uint8_t status;

	currentInstrument_idx=instr_idx;
	currentNote=note;

	handleInitFxNoteOnAllEnvelopes();

	handleFxNoteOnFilter();

	handleFxNoteOnGain();

	handleFxNoteOnPanning();

	handleFxNoteOnReverbSend();

//********* obsluga performance parametrow obslugiwanych w play_memory
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
	{
		changeEndPointPerformanceMode(performanceMod.endPoint);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])
	{
		changeStartPointPerformanceMode(performanceMod.startPoint);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::wavetablePosition])
	{
		changeWavetableWindowPerformanceMode(performanceMod.wavetablePosition);
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::tune])
	{
		changeTunePerformanceMode(performanceMod.tune);
	}
//******** seqFx
	if(velocity >= 0)
	{
		if (isFxVelocity(fx1_id))
		{
			fx1_id = fx_t::FX_TYPE_VELOCITY;
			fx1_val = velocity;
		}

		if (isFxVelocity(fx2_id))
		{
			fx2_id = fx_t::FX_TYPE_VELOCITY;
			fx2_val = velocity;
		}
	}

	seqFx(fx1_id,fx1_val,0);
	seqFx(fx2_id,fx2_val,1);
//*******
	status = playMemPtr->play(instr_idx,note);
//******* start env
	envelopePtr[envAmp]->start();

	if((mtProject.instrument[instr_idx].envelope[envAmp].enable)
	|| (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp])
	|| (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp])
	|| (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp]) )
	{
		setSyncParamsAmpLFO();
	}

	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX; i++)
	{
		if((mtProject.instrument[instr_idx].envelope[envelopesWithoutAmpIdx[i]].enable)
		|| (trackControlParameter[(int)controlType::sequencerMode][envelopesWithoutAmpControlValue[i]])
		|| (trackControlParameter[(int)controlType::sequencerMode2][envelopesWithoutAmpControlValue[i]])
		|| (trackControlParameter[(int)controlType::performanceMode][envelopesWithoutAmpControlValue[i]]))
		{
			envelopePtr[envelopesWithoutAmpIdx[i]]->start();
			setSyncParamsLFO(i);
		}
	}
//*******
	__enable_irq();
	AudioInterrupts();
	return status;
}
//********************************** NOTE OFF
void playerEngine::noteOff(int8_t option)
{
	switch (option)
	{
		case Sequencer::STEP_NOTE_FADE:		noteOffFade();		 break;
		case Sequencer::STEP_NOTE_CUT:		noteOffCut();		 break;
		case Sequencer::STEP_NOTE_OFF:		noteOffOrdinary();	 break;
	}
}
//************************************* handle noteOff
void playerEngine::noteOffFade()
{
	__disable_irq();
	AudioNoInterrupts();
	envelopePtr[envAmp]->init((envelopeGenerator::strEnv *)&fadeOutEnvelope);
	envelopePtr[envAmp]->stop();
	//lfo ma dzialac do konca fade

	for(uint8_t i = 0; i < ENVELOPES_WITHOUT_AMP_MAX; i++ )
	{
		if((!mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[i]].loop)
		 && (!trackControlParameter[(int)controlType::sequencerMode][envelopesWithoutAmpControlValue[i]])
		 && (!trackControlParameter[(int)controlType::sequencerMode2][envelopesWithoutAmpControlValue[i]])
		 && (!trackControlParameter[(int)controlType::performanceMode][envelopesWithoutAmpControlValue[i]]))
		{
			envelopePtr[envelopesWithoutAmpIdx[i]]->stop();
		}
	}

	AudioInterrupts();
	__enable_irq();
}
void playerEngine::noteOffCut()
{
	__disable_irq();
	AudioNoInterrupts();
	// caly voice i automatyka zabijane
	for(uint8_t i = 0; i < ACTIVE_ENVELOPES_MAX ; i++)
	{
		envelopePtr[i]->stop();
		envelopePtr[i]->killToZero();
	}
	playMemPtr->stop();

	AudioInterrupts();
	__enable_irq();
}
void playerEngine::noteOffOrdinary()
{
	__disable_irq();
	AudioNoInterrupts();
	envelopePtr[envAmp]->stop();

	if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
	 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp])
	 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp])
	 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp]))
	{
		//amp loop nie ma release wiec wszystko zabijamy od razu
		for(uint8_t i = envAmp + 1; i < ACTIVE_ENVELOPES_MAX ; i++)
		{
			envelopePtr[i]->stop();
		}
	}
	else
	{
		//env ampa nie zabija od razu loopow - maja grac do konca env

		for(uint8_t i = envAmp + 1; i < ENVELOPES_WITHOUT_AMP_MAX; i++ )
		{
			if((!mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[i]].loop)
			 && (!trackControlParameter[(int)controlType::sequencerMode][envelopesWithoutAmpControlValue[i]])
			 && (!trackControlParameter[(int)controlType::sequencerMode2][envelopesWithoutAmpControlValue[i]])
			 && (!trackControlParameter[(int)controlType::performanceMode][envelopesWithoutAmpControlValue[i]]))
			{
				envelopePtr[envelopesWithoutAmpIdx[i]]->stop();
			}
		}
	}


	if(!mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
	{
		playMemPtr->stop();
		if((mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable && mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)
		 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff])
		 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
		 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
		{
			currentEnvelopeModification[envFilter] = 0;
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
		 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition])
		 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
		 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
		{
			currentEnvelopeModification[envWtPos] = 0;
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
		 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition])
		 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
		 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
		{
			currentEnvelopeModification[envGranPos] = 0;
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable && mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
		 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning])
		 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
		 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
		{
			currentEnvelopeModification[envPan] = 0;
		}
	}
	else
	{
		if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].release == 0.0f) || (envelopePassFlag) || (mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop) )
		{
			playMemPtr->stop();
			if((mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable && mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)
			 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff])
			 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff])
			 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
			{
				currentEnvelopeModification[envFilter] = 0;
			}
			if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)
			 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition])
			 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition])
			 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
			{
				currentEnvelopeModification[envWtPos] = 0;
			}
			if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)
			 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition])
			 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition])
			 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
			{
				currentEnvelopeModification[envGranPos] = 0;
			}
			if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable && mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)
			 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning])
			 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning])
			 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
			{
				currentEnvelopeModification[envPan] = 0;
			}
		}
	}
	AudioInterrupts();
	__enable_irq();
}

//************************************* handle noteOn
void playerEngine::handleInitNoteOnAllEnvelopes()
{
	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX ; i++)
	{
		envelopePtr[envelopesWithoutAmpIdx[i]]->kill();
	}

	handleInitNoteOnAmpEnvelope();

	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		handleInitNoteOnEnvelope((uint8_t)envWithoutAmp::filter);
	}

	if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
	{
		handleInitNoteOnEnvelope((uint8_t)envWithoutAmp::wtPos);
	}

	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
	{
		handleInitNoteOnEnvelope((uint8_t)envWithoutAmp::granPos);
	}
	handleInitNoteOnEnvelope((uint8_t)envWithoutAmp::panning);
}

void playerEngine::handleNoteOnFilter()
{
	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterConnect();
		filterPtr->resonance(mtProject.instrument[currentInstrument_idx].resonance + RESONANCE_OFFSET);
		filterPtr->setCutoff(mtProject.instrument[currentInstrument_idx].cutOff);

		changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
	}
	else if(!mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterDisconnect();
	}
}

void playerEngine::handleNoteOnGain()
{
	if(muteState == MUTE_DISABLE)
	{
		ampPtr->gain(currentEnvelopeModification[envAmp] * ampLogValues[mtProject.instrument[currentInstrument_idx].volume]);
	}
	else
	{
		ampPtr->gain(AMP_MUTED);
	}
}

void playerEngine::handleNoteOnPanning()
{
	float gainL = 0, gainR = 0;

	if(mtProject.instrument[currentInstrument_idx].panning < 50)
	{
		gainR = (mtProject.instrument[currentInstrument_idx].panning) / 50.0;
		gainL = 1.0;
	}
	else if(mtProject.instrument[currentInstrument_idx].panning > 50)
	{
		gainR = 1.0;
		gainL = (100 - mtProject.instrument[currentInstrument_idx].panning)/50.0;
	}
	else if(mtProject.instrument[currentInstrument_idx].panning == 50)
	{
		gainL = 1.0; gainR = 1.0;
	}

	mixerL.gain(nChannel,gainL);
	mixerR.gain(nChannel,gainR);

}

void playerEngine::handleNoteOnReverbSend()
{
	if(((muteState == MUTE_DISABLE) && (onlyReverbMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
	{
		mixerReverb.gain(nChannel,mtProject.instrument[currentInstrument_idx].reverbSend/100.0);
	}
	else
	{
		mixerReverb.gain(nChannel,AMP_MUTED);
	}
}

void playerEngine::handleInitNoteOnAmpEnvelope()
{
	if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed );

			envelopePtr[envAmp]->init(&lfoBasedEnvelope[envAmp]);
		}
		else
		{
			envelopePtr[envAmp]->init(&mtProject.instrument[currentInstrument_idx].envelope[envAmp]);
		}
	}
	else
	{
		envelopePtr[envAmp]->init((envelopeGenerator::strEnv *)&passEnvelope);
	}
}

void playerEngine::handleInitNoteOnEnvelope(uint8_t n)
{
	currentEnvelopeModification[envelopesWithoutAmpIdx[n]] = 0;
	if(mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[n]].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[n]].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envelopesWithoutAmpIdx[n]], &mtProject.instrument[currentInstrument_idx].lfo[envelopesWithoutAmpIdx[n]],mtProject.instrument[currentInstrument_idx].lfo[envelopesWithoutAmpIdx[n]].speed);
			envelopePtr[envelopesWithoutAmpIdx[n]]->init(&lfoBasedEnvelope[envelopesWithoutAmpIdx[n]]);
		}
		else
		{
			envelopePtr[envelopesWithoutAmpIdx[n]]->init(&mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[n]]);
		}
	}
}
//*************************************** FX NOTE ON
void playerEngine::handleInitFxNoteOnEnvelope(uint8_t n)
{
	if((!trackControlParameter[(int)controlType::sequencerMode][envelopesWithoutAmpControlValue[n]]) &&
	(!trackControlParameter[(int)controlType::sequencerMode2][envelopesWithoutAmpControlValue[n]]) &&
	(!trackControlParameter[(int)controlType::performanceMode][envelopesWithoutAmpControlValue[n]]))
	{
		handleInitNoteOnEnvelope(n);
	}

	if(mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[n]].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envelopesWithoutAmpIdx[n]].loop)
		{
			if(trackControlParameter[(int)controlType::performanceMode][envelopesWithoutAmpControlValue[n]])
			{
				switch(n)
				{
				case (int)envWithoutAmp::filter:

					changeCutoffLfoRatePerformanceMode(performanceMod.lfoCutoffRate);
					break;
				case (int)envWithoutAmp::granPos:

					changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);
					break;
				case (int)envWithoutAmp::wtPos:

					changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);
					break;
				case (int)envWithoutAmp::panning:

					changePositionLfoRatePerformanceMode(performanceMod.lfoPanningRate);
					break;
				}
			}
		}
	}
}
void playerEngine::handleInitFxNoteOnAmpEnvelope()
{
	if((!trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp]) &&
	(!trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp]) &&
	(!trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp]))
	{
		handleInitNoteOnAmpEnvelope();
	}
	if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
		{
			if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp])
			{
				changeCutoffLfoRatePerformanceMode(performanceMod.lfoAmpRate);
			}
		}
	}

}
void playerEngine::handleInitFxNoteOnAllEnvelopes()
{
	for(uint8_t i = 0 ; i < ENVELOPES_WITHOUT_AMP_MAX ; i++)
	{
		envelopePtr[envelopesWithoutAmpIdx[i]]->kill();
	}

	handleInitFxNoteOnAmpEnvelope();

	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		handleInitFxNoteOnEnvelope((uint8_t)envWithoutAmp::filter);
	}

	if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
	{
		handleInitFxNoteOnEnvelope((uint8_t)envWithoutAmp::wtPos);
	}

	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
	{
		handleInitFxNoteOnEnvelope((uint8_t)envWithoutAmp::granPos);
	}
	handleInitFxNoteOnEnvelope((uint8_t)envWithoutAmp::panning);
}

void playerEngine::handleFxNoteOnFilter()
{
//***** resonance
	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterPtr->resonance(mtProject.instrument[currentInstrument_idx].resonance + RESONANCE_OFFSET);
	}
//*****
//***** cutoff + enable
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterCutoff])
	{
		changeCutoffPerformanceMode(performanceMod.cutoff); // włączenie filtra jest w środku
	}
	else if((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterCutoff])
		|| (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterCutoff]))
	{
		filterConnect();
		filterPtr->setCutoff(currentSeqModValues.filterCutoff);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].filterEnable)
		{
			filterConnect();
			filterPtr->setCutoff(mtProject.instrument[currentInstrument_idx].cutOff);
		}
		else if(!mtProject.instrument[currentInstrument_idx].filterEnable) filterDisconnect();
	}
//*****
//***** filter type
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::filterType])
	{
		changeFilterTypePerformanceMode(performanceMod.filterType);
	}
	else if((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::filterType])
		|| (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::filterType]))
	{
		changeFilterType(currentSeqModValues.filterType);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].filterEnable)
		{
			changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
		}
	}
//*****
}

void playerEngine::handleFxNoteOnGain()
{
	if(muteState == MUTE_DISABLE)
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
		{
			changeVolumePerformanceMode(performanceMod.volume);
		}
		else if(trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::volume] ||
				trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::volume])
		{
			ampPtr->gain(ampLogValues[currentSeqModValues.volume] * currentEnvelopeModification[envAmp]);
		}
		else
		{
			ampPtr->gain(ampLogValues[mtProject.instrument[currentInstrument_idx].volume] * currentEnvelopeModification[envAmp]);
		}
	}
	else
	{
		ampPtr->gain(AMP_MUTED);
	}
}

void playerEngine::handleFxNoteOnPanning()
{
	float gainL = 0, gainR = 0;

	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::panning])
	{
		changePanningPerformanceMode(performanceMod.panning);
	}
	else if((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::panning]) ||
		(trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::panning]))
	{
		if(currentSeqModValues.panning < 50)
		{
			gainR = (currentSeqModValues.panning ) / 50.0;
			gainL = 1.0;
		}
		else if(currentSeqModValues.panning > 50)
		{
			gainR = 1.0;
			gainL = ( 100 - currentSeqModValues.panning) / 50.0;
		}
		else if(currentSeqModValues.panning == 50)
		{
			gainL = 1.0; gainR = 1.0;
		}

		mixerL.gain(nChannel,gainL);
		mixerR.gain(nChannel,gainR);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].panning < 50)
		{
			gainR = (mtProject.instrument[currentInstrument_idx].panning) / 50.0;
			gainL = 1.0;
		}
		else if(mtProject.instrument[currentInstrument_idx].panning > 50)
		{
			gainR = 1.0;
			gainL = (100 - mtProject.instrument[currentInstrument_idx].panning)/50.0;
		}
		else if(mtProject.instrument[currentInstrument_idx].panning == 50)
		{
			gainL = 1.0; gainR = 1.0;
		}

		mixerL.gain(nChannel,gainL);
		mixerR.gain(nChannel,gainR);
	}
}

void playerEngine::handleFxNoteOnReverbSend()
{
	if(((muteState == MUTE_DISABLE) && (onlyReverbMuteState == MUTE_DISABLE)) || (engine.forceSend == 1))
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::reverbSend])
		{
			changeReverbSendPerformanceMode(performanceMod.reverbSend);
		}
		else if ((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::reverbSend])
			||(trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::reverbSend]))
		{
			mixerReverb.gain(nChannel,currentSeqModValues.reverbSend/100.0);
		}
		else
		{
			mixerReverb.gain(nChannel,mtProject.instrument[currentInstrument_idx].reverbSend/100.0);
		}
	}
	else
	{
		mixerReverb.gain(nChannel,AMP_MUTED);
	}
}

