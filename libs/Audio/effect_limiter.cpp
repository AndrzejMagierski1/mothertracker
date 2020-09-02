#include "effect_limiter.h"
#include "mtStructs.h"

void AudioEffectLimiter::begin(uint16_t ts, uint16_t a, uint16_t r)
{
	threshold = ts;
	attackTime = a/1000.0;
	releaseTime = r/1000.0;

	memset(buffer,0,10);
}

void AudioEffectLimiter::setThreshold(uint16_t ts)
{
	threshold = ts;
}
void AudioEffectLimiter::setAttack(uint16_t a)
{
	if(mtConfig.general.limiterMode == limiterModeSubtle) attackTime = 1 - expf( -2.2f *  (1000.0f/(44100.0f * a)) );
	else if(mtConfig.general.limiterMode == limiterModeHard) attackTime = a/1000.0f;
}
void AudioEffectLimiter::setRelease(float r)
{
	if(mtConfig.general.limiterMode == limiterModeSubtle) releaseTime = 1 - expf(-2.2f *  (1000.0f/(44100.0f * r)));
	else if(mtConfig.general.limiterMode == limiterModeHard) releaseTime = r/1000.0f;
}

void AudioEffectLimiter::update(void)
{
	audio_block_t *sblock;
    int16_t *sbuf;
    int16_t temp;
    float localf;

    sblock = receiveWritable(0);
    if(sblock) sbuf = sblock->data;
    else
    {
    	return;
    }

	for(uint8_t n=0; n < AUDIO_BLOCK_SAMPLES; n++)
	{
		a = (*sbuf<0) ? -*sbuf : *sbuf;

		coeff = (a > xpeak) ? attackTime : releaseTime;

		xpeak = (1-coeff) * xpeak + coeff * a;

		localf = threshold/(float)xpeak;
		f = (localf < 1.0f) ? localf : 1.0f;

		coeff = (f<g) ? attackTime : releaseTime;

		g = (1-coeff) *g + coeff * f;

		temp=*sbuf;

		*sbuf = g * buffer[LIMITER_DELAY-1];

		for(uint8_t i = LIMITER_DELAY-1; i>=1 ; i--)
		{
			buffer[i]=buffer[i-1];
		}
		buffer[0]=temp;
		sbuf++;
	}

    transmit(sblock,0);
    release(sblock);
}
