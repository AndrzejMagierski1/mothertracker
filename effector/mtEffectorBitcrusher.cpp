#include "mtEffectorBitcrusher.h"


mtBitcrusher effectorBitcrusher;

int32_t mtBitcrusher::makeBitcrusher(uint8_t cBits, uint16_t sRate)
{
	uint32_t localLength = effector.getLength();
	int32_t returnLength = (int32_t) localLength;

	int16_t * localAddress = effector.getAddress();
	int16_t * destAddress = sdram_effectsBank;

	int n = (AUDIO_SAMPLE_RATE_EXACT / sRate) + 0.5;
	if (n < 1) n = 1;
	else if (n > 64) n = 64;
	sampleStep = n;

	if (cBits > 16) cBits = 16;
	else if (cBits == 0) cBits = 1;
	crushBits = cBits;


	while ( localLength )
	{
/*		todo: jakby byly jakies problemy mozna na 0 ustawic wartosci wykraczajace poza bufor w ostatnim buforze
		if(localLength<AUDIO_BLOCK_SAMPLES) memset(localAddress+localLength,0,AUDIO_BLOCK_SAMPLES-localLength);*/
		calculate(localAddress,destAddress);
		localAddress += AUDIO_BLOCK_SAMPLES;
		destAddress += AUDIO_BLOCK_SAMPLES;
		if(localLength> AUDIO_BLOCK_SAMPLES) localLength -= AUDIO_BLOCK_SAMPLES;
		else localLength=0;
	}
	effector.affterEffectLength=returnLength/2;
	return 1;
}

void mtBitcrusher::calculate(int16_t * src, int16_t *dst)
{
	uint32_t i;
	uint32_t sampleSquidge, sampleSqueeze;

	if (crushBits == 16 && sampleStep <= 1)
	{
		for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			*dst++ = 0;
		}
		return;
	}

	if (sampleStep <= 1)
	{
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			sampleSquidge = *src >> (16 - crushBits);

			*dst = sampleSquidge << (16 - crushBits);
			src++;
			dst++;
		}
	}
	else if (crushBits == 16)
	{
		i = 0;
		while (i < AUDIO_BLOCK_SAMPLES)
		{
			sampleSqueeze = *src++;
			for (int j = 0; j < sampleStep && i < AUDIO_BLOCK_SAMPLES; j++)
			{
				*dst++ = sampleSqueeze;
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (i < AUDIO_BLOCK_SAMPLES)
		{

			sampleSqueeze = *src++;
			for (int j = 0; j < sampleStep && i < AUDIO_BLOCK_SAMPLES; j++)
			{
				sampleSquidge = sampleSqueeze >> (16 - crushBits);
				*dst = sampleSquidge << (16 - crushBits);
				i++;
			}
		}
	}
}
