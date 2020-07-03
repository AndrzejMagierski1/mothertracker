#include "effect_declick.h"



void AudioEffectDeclick::update(void)
{
	audio_block_t *sblock;
    int16_t *sbuf;

    sblock = receiveReadOnly(0);
    if(sblock) sbuf = sblock->data;
    else
    {
    	lastSample = 0;
    	return;
    }
    if(needCrossfade)
    {
    	audio_block_t *dblock;
        int16_t *dbuf;

        dblock = allocate();
        if(dblock) dbuf = dblock->data;
        else
        {
        	release(sblock);
        	return;
        }

       	needCrossfade = false;

        uint8_t j = AUDIO_BLOCK_SAMPLES;

    	for(uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    	{
    		dbuf[i] = ((int32_t)( (int32_t)(lastSample * j) + (int32_t)(sbuf[i] * i))) / AUDIO_BLOCK_SAMPLES ;
    		j--;
    	}

    	lastSample = sbuf[127];

//    	dbuf[0] = 32000;
//    	dbuf[127] = 32000;

    	transmit(dblock, 0);
    	release(sblock);
    	release(dblock);
    }
    else
    {

    	lastSample = sbuf[127];
        transmit(sblock, 0);
        release(sblock);
    }
}
