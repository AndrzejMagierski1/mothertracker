#include "mtEffectorBitcrusher.h"


mtBitcrusher effectorBitcrusher;

int32_t mtBitcrusher::makeBitcrusher(uint8_t cBits, uint16_t sRate, int8_t forceEffect)
{
	if(forceEffect == 0)
	{
		if(cBits == last_cBits && sRate == last_sRate)
		{
			return 0;
		}
	}

	localLength = effector.getLength();
	returnLength = (int32_t) localLength;

	localAddress = effector.getAddress();
	destAddress = effector.previewBuffer;

	int n = (AUDIO_SAMPLE_RATE_EXACT / sRate) + 0.5;
	if (n < 1) n = 1;
	else if (n > 64) n = 64;
	sampleStep = n;

	if (cBits > 16) cBits = 16;
	else if (cBits == 0) cBits = 1;
	crushBits = cBits;

	requireProcessing = 1;
	startLength = localLength;
	loadProgress = 0;

	last_cBits =cBits;
	last_sRate = sRate;

	return 1;
}

void mtBitcrusher::process()
{
	if(requireProcessing == 1 && localLength != 0)
	{
		/*		todo: jakby byly jakies problemy mozna na 0 ustawic wartosci wykraczajace poza bufor w ostatnim buforze
				if(localLength<AUDIO_BLOCK_SAMPLES) memset(localAddress+localLength,0,AUDIO_BLOCK_SAMPLES-localLength);*/
		calculate(localAddress,destAddress);
		localAddress += AUDIO_BLOCK_SAMPLES;
		destAddress += AUDIO_BLOCK_SAMPLES;
		if(localLength> AUDIO_BLOCK_SAMPLES) localLength -= AUDIO_BLOCK_SAMPLES;
		else localLength=0;

		loadProgress = ((startLength-localLength) * 100) / startLength;

		if(localLength == 0)
		{
			requireProcessing = 0;
			effector.affterEffectLength = returnLength / 2;
		}
	}
}

uint8_t mtBitcrusher::getProgress()
{
	return loadProgress;
}

uint8_t mtBitcrusher::requireProcess()
{
	return requireProcessing;
}

void mtBitcrusher::calculate(int16_t * src, int16_t *dst)
{
	uint32_t i;
	uint32_t sampleSquidge, sampleSqueeze;
	int16_t localBuf[AUDIO_BLOCK_SAMPLES];
	int16_t	* localAddress = localBuf;



	if (crushBits == 16 && sampleStep <= 1)
	{
		for(uint8_t i=0; i<AUDIO_BLOCK_SAMPLES; i++)
		{
			*dst++=*src++;
		}
		return;
	}

	memcpy(localBuf,src,2*AUDIO_BLOCK_SAMPLES);

	if (sampleStep <= 1)
	{
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			sampleSquidge = *localAddress >> (16 - crushBits);

			*localAddress++ = sampleSquidge << (16 - crushBits);
		}
	}
	else if (crushBits == 16)
	{
		i = 0;
		while (i < AUDIO_BLOCK_SAMPLES)
		{
			sampleSqueeze = *localAddress;
			for (int j = 0; j < sampleStep && i < AUDIO_BLOCK_SAMPLES; j++)
			{
				*localAddress++ = sampleSqueeze;
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (i < AUDIO_BLOCK_SAMPLES)
		{
			sampleSqueeze = *localAddress;
			for (int j = 0; j < sampleStep && i < AUDIO_BLOCK_SAMPLES; j++)
			{
				sampleSquidge = sampleSqueeze >> (16 - crushBits);
				*localAddress++ = sampleSquidge << (16 - crushBits);
				i++;
			}
		}
	}
	localAddress=localBuf;
	memcpy(dst,localAddress,2*AUDIO_BLOCK_SAMPLES);
}
