#include "mtEffectorCompressor.h"

mtCompressor effectorCompressor;

int8_t mtCompressor::makeCompressor(uint16_t cT, int16_t cR, uint16_t eT, int16_t eR, uint16_t at, uint16_t rt)
{
	int32_t localLength = effector.getLength();
	int32_t	returnLength = localLength;
	int16_t * localAddress = effector.getAddress();
	int16_t * destAddress = effector.previewBuffer;

	compressorThreshold=10*log10f(pow(cT,2));
	expanderThreshold= 10*log10f(pow(eT,2));
	compressorRatio= cR/1000.0;
	expanderRatio= eR/1000.0;
	attackTime= at/1000.0;
	releaseTime= rt/1000.0;
	tav = 0.01;
	delay = 150;
	xrms = 0;
	g = 1;

	memset(buffer,0,delay*2);

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

void mtCompressor::calculate(int16_t * sbuf, int16_t * dbuf)
{
	float temp1;
	float temp2;
	for(uint8_t i=0;i<AUDIO_BLOCK_SAMPLES;i++)
	{
		xrms = (1-tav) * xrms + tav * powf(*sbuf,2);
		X=10*log10f(xrms);

		temp1=compressorRatio*(compressorThreshold - X);
		temp2=expanderRatio*(expanderThreshold - X );
		if(temp1 > temp2) temp1 = temp2;
		if(temp1 < 0) G = temp1;
		else G = 0;

		f= powf(10, G/20.0);

		if(f<g) coeff = attackTime;
		else coeff = releaseTime;
		g = (1-coeff) * g + coeff*f;

		if(buffer[delay-1] == 0) *dbuf=*sbuf;
		else *dbuf = g*buffer[delay-1];

		for(uint8_t i = delay-1; i>=1 ; i--)
		{
			buffer[i]=buffer[i-1];
		}
		buffer[0]=*sbuf;
		sbuf++;
		dbuf++;
	}


}



