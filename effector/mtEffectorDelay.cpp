#include "mtEffectorDelay.h"

mtDelay effectorDelay;

uint8_t mtDelay::makeDelay(float feedback, uint16_t time, uint8_t forceEffect)
{
	if(forceEffect == 0)
	{
		if(feedback == last_feedback && time == last_time)
		{
			return 0;
		}
	}

	if(feedback == 0 || time == 0)
	{
		return 0;
	}

	uint32_t localLength = effector.getLength();
	int16_t * localAddress = effector.getAddress();
	uint32_t samplesTime = (uint32_t)((float)time*44.1);

	destAddress = sdram_effectsBank;
	compareAddr = localAddress + localLength/2;

	firstIndex=0;
	lastIndex=0;
	sampleCnt=0;

	for(uint8_t i=0 ; i< 99; i++)
	{
		sPointer[i] = localAddress;
		feedbackTab[i] = pow(feedback,i);
		lastIdxSmpTime[i] = i*samplesTime;
	}
	uint8_t voiceCnt=0;
	float g=feedback;

	while(g>0.01)
	{
		g*=feedback;
		voiceCnt++;
		if(voiceCnt==99) break;
	}

	compareIdx = voiceCnt - 1;

	maxLoad = (effector.getLength() / 2) + ((voiceCnt - 2) * samplesTime);


	requireProcessing = 1;
	loadProgress = 0;
	last_feedback = feedback;
	last_time = time;

	return 1;
}

void mtDelay::process()
{
	if(requireProcessing == 1)
	{
		if((sPointer[compareIdx] < compareAddr ) && (sampleCnt <= SAMPLE_MEMORY_MAX))
		{
			int32_t currentSum = 0;

			for(uint8_t i = firstIndex ; i<= lastIndex ; i ++ )
			{
				currentSum+= *sPointer[i] * feedbackTab[i];
				sPointer[i]++;
			}

			if(currentSum > MAX_SIGNED_16BIT) currentSum = MAX_SIGNED_16BIT;
			else if(currentSum < MIN_SIGNED_16BIT) currentSum = MIN_SIGNED_16BIT;

			*destAddress++ = (int16_t)currentSum;
			sampleCnt++;
			if((sampleCnt > lastIdxSmpTime[lastIndex])) lastIndex ++;
			if(sPointer[firstIndex] >= compareAddr) firstIndex ++;


			if(lastIndex > 98) lastIndex = 98;
			if(firstIndex > 98) firstIndex = 98;

			loadProgress = (sampleCnt * 100) / maxLoad;
		}
		else
		{
			effector.affterEffectLength = sampleCnt;
			loadProgress = 100;
			requireProcessing = 0;
		}
	}
}

uint8_t mtDelay::getProgress()
{
	return loadProgress;
}

uint8_t mtDelay::requireProcess()
{
	return requireProcessing;
}

void mtDelay::cancelProcessing()
{
	requireProcessing = 0;
	loadProgress = 0;
}


