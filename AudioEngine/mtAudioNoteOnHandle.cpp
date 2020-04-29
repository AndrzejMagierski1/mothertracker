#include "mtAudioEngine.h"
#include "mtSequencer.h"


//********************************** NOTE_ON
uint8_t playerEngine :: noteOn (uint8_t instr_idx,int8_t note, int8_t velocity)
{
	if(mtProject.instrument[instr_idx].isActive != 1) return 0;

//	engine.clearDelay();

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

	handleNoteOnDelaySend();


	status = playMemPtr->play(instr_idx,note);
	if(isTrackDisplayed) onEndDisplay = true;
	envelopeAmpPtr->noteOn();

	for(uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++)
	{
		if(mtProject.instrument[instr_idx].envelope[i].enable) envelopePtr[i]->start();
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

	handleFxNoteOnDelaySend();

//********* obsluga performance parametrow obslugiwanych w play_memory
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::endPoint])
	{
		changeEndPointPerformanceMode(map(performanceMod.endPoint,-MAX_16BIT,MAX_16BIT,-100,100));//zalatanie na szybko
	}
	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::startPoint])//tu nie ma mapowania bo posredniczy change position
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

	envelopeAmpPtr->noteOn(); // zawsze odpalamy nawet jak nie aktywny

	for(uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++)
	{
		if(isActiveEnvelope(i))
		{
			envelopePtr[i]->start();
			setSyncParamsLFO(i);
		}
	}
//*******

//*******
	status = playMemPtr->play(instr_idx,note);
	if(isTrackDisplayed) onEndDisplay = true;
//******* start env
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
	envelopeAmpPtr->release(300);
	envelopeAmpPtr->noteOff();
	//lfo ma dzialac do konca fade

	for(uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++ )
	{
		if((!mtProject.instrument[currentInstrument_idx].envelope[i].loop)
		 && (!trackControlParameter[(int)controlType::sequencerMode][i])
		 && (!trackControlParameter[(int)controlType::sequencerMode2][i])
		 && (!trackControlParameter[(int)controlType::performanceMode][i]))
		{
			envelopePtr[i]->stop();
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
	envelopeAmpPtr->noteOff();
	envelopeAmpPtr->setIdle();
	for(uint8_t i = envPan; i < ACTIVE_ENVELOPES ; i++)
	{
		envelopePtr[i]->stop();
		envelopePtr[i]->killToZero();
	}
	playMemPtr->stop();

	if(isTrackDisplayed) onEndDisplay = true;

	AudioInterrupts();
	__enable_irq();
}
void playerEngine::noteOffOrdinary()
{
	__disable_irq();
	AudioNoInterrupts();
	envelopeAmpPtr->noteOff();

	if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
	 || (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoAmp])
	 || (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoAmp])
	 || (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoAmp]))
	{
		//amp loop nie ma release wiec wszystko zabijamy od razu
		for(uint8_t i = envPan; i < ACTIVE_ENVELOPES ; i++)
		{
			envelopePtr[i]->stop();
		}
	}
	else
	{
		//env ampa nie zabija od razu loopow - maja grac do konca env

		for(uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++ )
		{
			if((!mtProject.instrument[currentInstrument_idx].envelope[i].loop)
			 && (!trackControlParameter[(int)controlType::sequencerMode][i])
			 && (!trackControlParameter[(int)controlType::sequencerMode2][i])
			 && (!trackControlParameter[(int)controlType::performanceMode][i]))
			{
				envelopePtr[i]->stop();
			}
		}
	}


	if(!mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
	{
		playMemPtr->stop();
		if(isTrackDisplayed) onEndDisplay = true;
		for ( uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++ )
		{
			if((mtProject.instrument[currentInstrument_idx].envelope[i].enable && mtProject.instrument[currentInstrument_idx].envelope[i].loop)
			 || (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[i]])
			 || (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[i]])
			 || (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[i]]))
			{
				currentEnvelopeModification[i] = 0;
			}
		}
	}
	else
	{
		if((mtProject.instrument[currentInstrument_idx].envelope[envAmp].release == 0.0f) || (envelopePassFlag) || (mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop) )
		{
			playMemPtr->stop();
			if(isTrackDisplayed) onEndDisplay = true;
			for ( uint8_t i = envPan; i < ACTIVE_ENVELOPES; i++ )
			{
				if((mtProject.instrument[currentInstrument_idx].envelope[i].enable && mtProject.instrument[currentInstrument_idx].envelope[i].loop)
				 || (trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[i]])
				 || (trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[i]])
				 || (trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[i]]))
				{
					currentEnvelopeModification[i] = 0;
				}
			}
		}
	}
	AudioInterrupts();
	__enable_irq();
}

//************************************* handle noteOn
void playerEngine::handleInitNoteOnAllEnvelopes()
{
	//envAmp killuje sie wewnetrznie
	for(uint8_t i = envPan ; i < ACTIVE_ENVELOPES ; i++)
	{
		envelopePtr[i]->kill();
	}

	handleInitNoteOnEnvelope(envAmp);

	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		handleInitNoteOnEnvelope(envCutoff);
	}

	if(mtProject.instrument[currentInstrument_idx].playMode == playModeWavetable)
	{
		handleInitNoteOnEnvelope(envWtPos);
	}

	if(mtProject.instrument[currentInstrument_idx].playMode == playModeGranular)
	{
		handleInitNoteOnEnvelope(envGranPos);
	}
	handleInitNoteOnEnvelope(envPan);
	handleInitNoteOnEnvelope(envFinetune);
}

void playerEngine::handleNoteOnFilter()
{
	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterConnect();
		filterPtr->resonance(mtProject.instrument[currentInstrument_idx].resonance + RESONANCE_OFFSET);
		if(!isActiveEnvelope(envCutoff)) modCutoff(mtProject.instrument[currentInstrument_idx].cutOff);

		changeFilterType(mtProject.instrument[currentInstrument_idx].filterType);
	}
	else if(!mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		filterDisconnect();
	}
}

void playerEngine::handleNoteOnGain()
{
	float localAmount = getMostSignificantAmount();
	uint8_t localVolume = getMostSignificantVolume();

	ampPtr->gain(localAmount * ampLogValues[localVolume]);
}

void playerEngine::handleNoteOnPanning()
{

	if(!isActiveEnvelope(envPan)) modPanning(mtProject.instrument[currentInstrument_idx].panning);
}

void playerEngine::handleNoteOnDelaySend()
{
	if(((muteState == MUTE_DISABLE) && (onlyDelayMuteState == MUTE_DISABLE)) || ((engine.forceSend == 1) && !mtProject.values.trackMute[nChannel]))
	{
		modDelaySend(mtProject.instrument[currentInstrument_idx].delaySend);
	}
	else
	{
		modDelaySend(AMP_MUTED);
	}
}

void playerEngine::handleInitNoteOnAmpEnvelope()
{
	if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].enable)
	{
		if(mtProject.instrument[currentInstrument_idx].envelope[envAmp].loop)
		{
			calcLfoBasedEnvelope(&lfoBasedEnvelope[envAmp], &mtProject.instrument[currentInstrument_idx].lfo[envAmp], mtProject.instrument[currentInstrument_idx].lfo[envAmp].speed,1);

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

void playerEngine::handleInitNoteOnEnvelope(uint8_t n)
{
	if(n == envAmp)
	{
		handleInitNoteOnAmpEnvelope();
	}
	else
	{
		currentEnvelopeModification[n] = 0;
		if(mtProject.instrument[currentInstrument_idx].envelope[n].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[n].loop)
			{
				calcLfoBasedEnvelope(&lfoBasedEnvelope[n], &mtProject.instrument[currentInstrument_idx].lfo[n],mtProject.instrument[currentInstrument_idx].lfo[n].speed,0);
				initEnvelopesParamiters(n, &lfoBasedEnvelope[n]);
			}
			else
			{
				initEnvelopesParamiters(n, &mtProject.instrument[currentInstrument_idx].envelope[n]);
			}
		}
	}


}
//*************************************** FX NOTE ON
void playerEngine::handleInitFxNoteOnEnvelope(uint8_t n)
{
	if(n == envAmp)
	{
		handleInitFxNoteOnAmpEnvelope();
	}
	else
	{
		if((!trackControlParameter[(int)controlType::sequencerMode][envelopesControlValue[n]]) &&
		  (!trackControlParameter[(int)controlType::sequencerMode2][envelopesControlValue[n]]) &&
		  (!trackControlParameter[(int)controlType::performanceMode][envelopesControlValue[n]]))
		{
			handleInitNoteOnEnvelope(n);
		}

		if(mtProject.instrument[currentInstrument_idx].envelope[n].enable)
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[n].loop)
			{
				if(trackControlParameter[(int)controlType::performanceMode][n])
				{
					switch(n)
					{
						case envCutoff:		changeCutoffLfoRatePerformanceMode(performanceMod.lfoCutoffRate);		break;
						case envGranPos: 	changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);	break;
						case envWtPos:		changePositionLfoRatePerformanceMode(performanceMod.lfoPositionRate);	break;
						case envPan:		changePositionLfoRatePerformanceMode(performanceMod.lfoPanningRate);	break;
						case envFinetune: 	changePositionLfoRatePerformanceMode(performanceMod.lfoFinetuneRate);	break;
					}
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
	for(uint8_t i = envPan ; i < ACTIVE_ENVELOPES ; i++)
	{
		envelopePtr[i]->kill();
	}

	handleInitFxNoteOnEnvelope(envAmp);

	if(mtProject.instrument[currentInstrument_idx].filterEnable)
	{
		handleInitFxNoteOnEnvelope(envCutoff);
	}

	if(mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWavetable)
	{
		handleInitFxNoteOnEnvelope(envWtPos);
	}

	if((mtProject.instrument[currentInstrument_idx].sample.type == mtSampleTypeWaveFile) && (mtProject.instrument[currentInstrument_idx].playMode == playModeGranular))
	{
		handleInitFxNoteOnEnvelope(envGranPos);
	}
	handleInitFxNoteOnEnvelope(envPan);
	handleInitFxNoteOnEnvelope(envFinetune);
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
		if(!isActiveEnvelope(envCutoff)) modCutoff(currentSeqModValues.filterCutoff);
	}
	else
	{
		if(mtProject.instrument[currentInstrument_idx].filterEnable)
		{
			filterConnect();
			if(!isActiveEnvelope(envCutoff)) modCutoff(mtProject.instrument[currentInstrument_idx].cutOff);
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
	float localAmount = getMostSignificantAmount();


	if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::volume])
	{
		changeVolumePerformanceMode(performanceMod.volume);
	}
	else
	{

		ampPtr->gain( ampLogValues[getMostSignificantVolume()] * localAmount);
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
		if(!isActiveEnvelope(envPan)) modPanning(currentSeqModValues.panning);
	}
	else
	{
		if(!isActiveEnvelope(envPan)) modPanning(mtProject.instrument[currentInstrument_idx].panning);
	}
}

void playerEngine::handleFxNoteOnDelaySend()
{
	if(((muteState == MUTE_DISABLE) && (onlyDelayMuteState == MUTE_DISABLE)) || ((engine.forceSend == 1) && !mtProject.values.trackMute[nChannel]))
	{
		if(trackControlParameter[(int)controlType::performanceMode][(int)parameterList::delaySend])
		{
			changeDelaySendPerformanceMode(performanceMod.reverbSend);
		}
		else if ((trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::delaySend])
			||(trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::delaySend]))
		{
			modDelaySend(currentSeqModValues.delaySend);
		}
		else
		{
			modDelaySend(mtProject.instrument[currentInstrument_idx].delaySend);
		}
	}
	else
	{
		modDelaySend(AMP_MUTED);
	}
}

