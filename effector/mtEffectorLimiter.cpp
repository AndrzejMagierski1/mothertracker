#include "mtEffectorLimiter.h"

mtLimiter effectorLimiter;

int8_t mtLimiter::makeLimiter(uint16_t ts, uint16_t a, uint16_t r )
{
	threshold = ts;
	attack = a/1000.0;
	release = r/1000.0;
	int32_t localLength = effector.getLength();
	int32_t	returnLength = localLength;
	int16_t * localAddress = effector.getAddress();
	int16_t * destAddress = sdram_effectsBank;

	memset(buffer,0,10);

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


