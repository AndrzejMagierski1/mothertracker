#include "mtEffectorDelay.h"

mtDelay effectorDelay;

int32_t mtDelay::makeDelay(float feedback,uint16_t time)
{
	uint32_t localLength = effector.getLength();

	int16_t * localAddress = effector.getAddress();
	int16_t * destAddress = sdram_effectsBank;
	uint32_t samplesTime = (uint32_t)((float)time*44.1);
	int16_t * sPointer[99];
	float feedbackTab[99];
	uint32_t lastIdxSmpTime[99];
	int16_t * compareAddr = localAddress + localLength/2;
	uint8_t compareIdx;
	int32_t currentSum;
	uint8_t firstIndex=0;
	uint8_t lastIndex=0;
	uint32_t sampleCnt=0;



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

	while((sPointer[compareIdx] < compareAddr ) || (sampleCnt >= SAMPLE_MEMORY_MAX) )
	{
		currentSum=0;

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
	}

	effector.affterEffectLength = sampleCnt;
	return 1;

}


