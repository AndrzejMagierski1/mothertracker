#include "mtEffectDelay.h"
#include "mtStructs.h"

//***********VIRTUAL SET PARAMS
void mtEffectDelay::setParamiter(void * ptr, uint8_t n)
{
	switch(n)
	{
		case 0 : 	effectDelayParams.feedback = *((float*)ptr);		break;
		case 1 : 	effectDelayParams.time = *((uint16_t*)ptr);			break;
		default: break;
	}
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectDelay::startProcess()
{
	if(effectDelayParams.feedback == 0 || effectDelayParams.time == 0)
	{
		return false;
	}

	constexpr float MILISECOND_TIME = 44.1f;

	uint32_t timeInSamples = (uint32_t)( (float)effectDelayParams.time * MILISECOND_TIME );

	currentDstAddr = processed.selection.addr;
	lastSrcAddr = confirmed.selection.addr + confirmed.selection.length;

	firstActiveVoiceIdx = 0;
	lastActiveVoiceIdx = 0;
	processedSamples = 0;

	for(uint8_t i = 0 ; i< MAX_DELAY_VOICE_NUMBER; i++)
	{
		sourceVoicePointer[i] = confirmed.selection.addr;
		feedbackVoiceMult[i] = pow(effectDelayParams.feedback,i);
		lastIndexInVoice[i] = i*timeInSamples;
	}

	uint8_t maxVoiceNumber = 0;
	float feedbackGain = 1.0;

	while(feedbackGain > 0.01f)
	{
		feedbackGain *= effectDelayParams.feedback;
		maxVoiceNumber++;
		if(maxVoiceNumber == MAX_DELAY_VOICE_NUMBER) break;
	}

	lastVoiceIdx = maxVoiceNumber - 1;

	return true;

}
int32_t mtEffectDelay::updateProcess()
{
	if(!state) return -1;

	int32_t processedBlockLength = 0;

	for(uint32_t i = 0; i < 8192; i++)
	{
		if( ( sourceVoicePointer[lastVoiceIdx] < lastSrcAddr ) && (processedSamples <= SAMPLE_EFFECTOR_LENGTH_MAX))
		{
			int32_t currentSum = 0;

			for(uint8_t i = firstActiveVoiceIdx ; i <= lastActiveVoiceIdx ; i ++ )
			{
				currentSum+= *sourceVoicePointer[i] * feedbackVoiceMult[i];
				sourceVoicePointer[i]++;
			}

			if(currentSum > MAX_SIGNED_16BIT) currentSum = MAX_SIGNED_16BIT;
			else if(currentSum < MIN_SIGNED_16BIT) currentSum = MIN_SIGNED_16BIT;

			*currentDstAddr++ = (int16_t)currentSum;
			processedBlockLength++;

			if((processedSamples > lastIndexInVoice[lastActiveVoiceIdx])) lastActiveVoiceIdx ++;
			if(sourceVoicePointer[firstActiveVoiceIdx] >= lastSrcAddr) firstActiveVoiceIdx ++;


			if(lastActiveVoiceIdx > MAX_DELAY_VOICE_NUMBER - 1) lastActiveVoiceIdx = MAX_DELAY_VOICE_NUMBER - 1;
			if(firstActiveVoiceIdx > MAX_DELAY_VOICE_NUMBER - 1) firstActiveVoiceIdx = MAX_DELAY_VOICE_NUMBER - 1;
		}
		else
		{
			state = false;
			processed.selection.length = processedSamples; //redundancy
			processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
		}
	}

	processedSamples += processedBlockLength;

	return processedBlockLength;
}
bool mtEffectDelay::getProcessState()
{
	return state;
}
uint32_t mtEffectDelay::getExpectedProcessLength(uint32_t selectLen)
{
	constexpr float MILISECOND_TIME = 44.1f;
	uint32_t timeInSamples = (uint32_t)( (float)effectDelayParams.time * MILISECOND_TIME );

	return  selectLen + lastVoiceIdx * timeInSamples;
}

mtEffectDelay sampleEditorDelay;
//***********

