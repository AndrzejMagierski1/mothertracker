#include "mtEffectorLimiter.h"

mtLimiter effectorLimiter;

int8_t mtLimiter::makeLimiter(uint16_t ts, uint16_t a, uint16_t r)
{
	threshold = ts;
	attack = a/1000.0;
	release = r/1000.0;

	//set defaults?
	g=1.0;
	xpeak = 0;
	delay = 5;
	coeff = 0;
	f = 0;

	localLength = effector.getLength();
	returnLength = localLength;
	localAddress = effector.getAddress();
	destAddress = effector.previewBuffer;

	memset(buffer,0,10);

	startLength = localLength;
	requireProcessing = 1;
	loadProgress = 0;

	return 1;
}

void mtLimiter::process()
{
	if(requireProcessing == 1 && localLength != 0)
	{
		calculate(localAddress, destAddress);
		localAddress += AUDIO_BLOCK_SAMPLES;
		destAddress += AUDIO_BLOCK_SAMPLES;
		if(localLength > AUDIO_BLOCK_SAMPLES) localLength -= AUDIO_BLOCK_SAMPLES;
		else localLength = 0;

		loadProgress = ((startLength-localLength) * 100) / startLength;

		if(localLength == 0)
		{
			requireProcessing = 0;
			effector.affterEffectLength = returnLength / 2;
		}
	}
}

uint8_t mtLimiter::getProgress()
{
	return loadProgress;
}

uint8_t mtLimiter::requireProcess()
{
	return requireProcessing;
}

void mtLimiter::calculate(int16_t *sbuf, int16_t *dbuf)
{
	for(uint8_t n=0; n < AUDIO_BLOCK_SAMPLES; n++)
	{
		if(*sbuf<0) a=0-*sbuf;
		else a=*sbuf;

		if(a > xpeak) 		coeff = attack;
		else 				coeff = release;
		xpeak = (1-coeff) * xpeak + coeff * a;

		if ((threshold/(float)xpeak) < 1) f=(threshold/(float)xpeak);
		else f=1.0;
		if(f<g) coeff=attack;
		else coeff= release;
		g = (1-coeff) *g + coeff * f;
		if(buffer[delay-1] == 0) *dbuf= *sbuf; //todo: czy to ma sens?
		else *dbuf = g * buffer[delay-1];

		for(uint8_t i = delay-1; i>=1 ; i--)
		{
			buffer[i]=buffer[i-1];
		}
		buffer[0]=*sbuf;
		sbuf++;
		dbuf++;
	}

}


