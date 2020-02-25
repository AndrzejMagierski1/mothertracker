#include "mtAudioEngine.h"

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
					envelopeAmpPtr->syncTrackerSeq(sequencer.getSeqTimer(), sequencer.getActualTempo());
				}
			}
		}
		else
		{
			if(mtProject.instrument[currentInstrument_idx].envelope[type].enable)
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

void playerEngine::handleEndReleaseAction()
{
	if(envelopeAmpPtr->endRelease())
	{
		envelopeAmpPtr->clearEndReleaseFlag();
		interfaceEndReleaseFlag = 1;
		playMemPtr->stop();

		if((mtProject.instrument[currentInstrument_idx].envelope[envFilter].enable && mtProject.instrument[currentInstrument_idx].envelope[envFilter].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoCutoff]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoCutoff]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoCutoff]))
		{
			currentEnvelopeModification[envFilter] = 0;
			envelopePtr[envFilter]->stop();
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envWtPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envWtPos].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoWavetablePosition]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoWavetablePosition]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoWavetablePosition]))
		{
			currentEnvelopeModification[envWtPos] = 0;
			envelopePtr[envWtPos]->stop();
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envGranPos].enable && mtProject.instrument[currentInstrument_idx].envelope[envGranPos].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoGranularPosition]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoGranularPosition]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoGranularPosition]))
		{
			currentEnvelopeModification[envGranPos] = 0;
			envelopePtr[envGranPos]->stop();
		}
		if((mtProject.instrument[currentInstrument_idx].envelope[envPan].enable && mtProject.instrument[currentInstrument_idx].envelope[envPan].loop)||
		  (trackControlParameter[(int)controlType::sequencerMode][(int)parameterList::lfoPanning]) ||
		  (trackControlParameter[(int)controlType::sequencerMode2][(int)parameterList::lfoPanning]) ||
		  (trackControlParameter[(int)controlType::performanceMode][(int)parameterList::lfoPanning]))
		{
			currentEnvelopeModification[envPan] = 0;
			envelopePtr[envPan]->stop();
		}
	}
}





