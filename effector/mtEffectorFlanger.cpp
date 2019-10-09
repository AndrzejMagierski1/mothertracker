#include "mtEffectorFlanger.h"

#include "arm_math.h"

int16_t flangerBuf[FLANGE_BUF_SIZE];
mtFlanger effectorFlanger;

uint8_t mtFlanger::makeFlanger(int d_length,int delay_offset,int d_depth,float delay_rate, uint8_t forceEffect)
{
	if(forceEffect == 0)
	{
		if((delay_offset == last_delay_offset) && (d_depth == last_d_depth) && (delay_rate == last_delay_rate))
		{
			return 0;
		}
	}

	if(d_length == 0 || delay_offset == 0 || d_depth == 0 || delay_rate == 0)
	{
		return 0;
	}

	localLength = effector.getLength();
	returnLength = localLength;
	localAddress = effector.getAddress();
	destAddress = sdram_effectsBank;

	delay_length = d_length/2;
	l_delayline = flangerBuf;

	delay_depth = d_depth;

	l_delay_rate_index = 0;
	l_circ_idx = 0;
	delay_rate_incr =(delay_rate * 2147483648.0)/ AUDIO_SAMPLE_RATE_EXACT;

	delay_offset_idx = delay_offset;

	if ((d_length%AUDIO_BLOCK_SAMPLES) || (d_length>FLANGE_BUF_SIZE)) return -1;
	if(delay_offset_idx < -1)
	{
	    delay_offset_idx = 0;
	    return -1;
	}
	if(delay_offset_idx >= delay_length)
	{
	    delay_offset_idx = delay_length - 1;
	    return -1;
	}

	memset(flangerBuf,0,d_length);

	requireProcessing = 1;
	loadProgress = 0;
	startLength = localLength;
	last_delay_offset= delay_offset;
	last_d_depth = d_depth;
	last_delay_rate = delay_rate;

	return 1;
}

void mtFlanger::process()
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

uint8_t mtFlanger::getProgress()
{
	return loadProgress;
}

uint8_t mtFlanger::requireProcess()
{
	return requireProcessing;
}

void mtFlanger::cancelProcessing()
{
	requireProcessing = 0;
	loadProgress = 0;
}

void mtFlanger::calculate(int16_t * sbuf, int16_t * dbuf)
{
	int idx;
	short *bp;
	short frac;
	int idx1;
	int16_t tempBuf[AUDIO_BLOCK_SAMPLES];

	if(l_delayline == NULL) return;

	memcpy(tempBuf,sbuf,2*AUDIO_BLOCK_SAMPLES);
	if(delay_offset_idx == FLANGE_DELAY_PASSTHRU)
	{
		  bp = sbuf;
	      for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++)
	      {
	        l_circ_idx++;
	        if(l_circ_idx >= delay_length)
	        {
	          l_circ_idx = 0;
	        }
	        l_delayline[l_circ_idx] = *bp++;
	      }
	    return;
	 }
	 bp = sbuf;
	 for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++)
	 {

	   l_circ_idx++;

	   if(l_circ_idx >= delay_length)  l_circ_idx = 0;

	   l_delayline[l_circ_idx] = *bp;
	   frac = arm_sin_q15( (q15_t)((l_delay_rate_index >> 16) & 0x7fff));
	   idx = (frac * delay_depth) >> 15;
	   idx = l_circ_idx - (delay_offset_idx + idx);
	   if(idx < 0) idx += delay_length;
	   if(idx >= delay_length) idx -= delay_length;

	   if(frac < 0) idx1 = idx - 1;
	   else	idx1 = idx + 1;

	   if(idx1 < 0) idx1 += delay_length;
	   if(idx1 >= delay_length) idx1 -= delay_length;

	   frac = (l_delay_rate_index >> 1) &0x7fff;
	   frac = (( (int)(l_delayline[idx1] - l_delayline[idx])*frac) >> 15);
	   *bp++ = (l_delayline[l_circ_idx]+ l_delayline[idx] + frac)/2;

	   l_delay_rate_index += delay_rate_incr;
	   if(l_delay_rate_index & 0x80000000) l_delay_rate_index &= 0x7fffffff;
	 }
	 bp = sbuf;
     for(int i=0; i<AUDIO_BLOCK_SAMPLES;i++)
     {
   	  *dbuf++ = *bp++;
     }

     memcpy(sbuf,tempBuf,2*AUDIO_BLOCK_SAMPLES);

}
