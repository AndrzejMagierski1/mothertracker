#include "effect_limiter.h"


void AudioEffectLimiter::begin(uint16_t ts, uint16_t a, uint16_t r)
{
	threshold = ts;
	attack = a/1000.0;
	releaseTime = r/1000.0;

	memset(buffer,0,10);
}

void AudioEffectLimiter::setThreshold(uint16_t ts)
{
	threshold = ts;
}
void AudioEffectLimiter::setAttack(uint16_t a)
{
	attack = a/1000.0;
}
void AudioEffectLimiter::setRelease(uint16_t r)
{
	releaseTime = r/1000.0;
}

void AudioEffectLimiter::update(void)
{
	audio_block_t *sblock;
    int16_t *sbuf;
    int16_t *dbuf;
    int16_t buf[AUDIO_BLOCK_SAMPLES];



    sblock = receiveWritable(0);
    if(sblock) sbuf = sblock->data;
    else
    {
    	release(sblock);
    	return;
    }

    dbuf=buf;

	for(uint8_t n=0; n < AUDIO_BLOCK_SAMPLES; n++)
	{
		if(*sbuf<0) a=0-*sbuf;
		else a=*sbuf;

		if(a > xpeak) 		coeff = attack;
		else 				coeff = releaseTime;
		xpeak = (1-coeff) * xpeak + coeff * a;

		if ((threshold/(float)xpeak) < 1) f=(threshold/(float)xpeak);
		else f=1.0;
		if(f<g) coeff=attack;
		else coeff= releaseTime;
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
	memcpy(sblock->data,buf,2*AUDIO_BLOCK_SAMPLES);

    transmit(sblock,0);
    release(sblock);
}
