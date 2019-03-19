#include "mtEffectorFlanger.h"

#include "arm_math.h"

int16_t flangerBuf[FLANGE_BUF_SIZE];

int32_t mtFlanger::makeFlanger(int d_length,int delay_offset,int d_depth,float delay_rate)
{
	int32_t localLength = effector.getLength();
	int32_t	returnLength = localLength;
	int16_t * localAddress = effector.getAddress();
	int16_t * destAddress = sdram_effectsBank;

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


	while ( localLength )
	{
		calculate(localAddress,destAddress);

/*		todo: jakby byly jakies problemy mozna na 0 ustawic wartosci wykraczajace poza bufor w ostatnim buforze
		if(localLength<AUDIO_BLOCK_SAMPLES) memset(localAddress+localLength,0,AUDIO_BLOCK_SAMPLES-localLength);*/
		calculate(localAddress,destAddress);
		localAddress += AUDIO_BLOCK_SAMPLES;
		destAddress += AUDIO_BLOCK_SAMPLES;
		if(localLength> AUDIO_BLOCK_SAMPLES) localLength -= AUDIO_BLOCK_SAMPLES;
		else localLength=0;
	}


	return returnLength;

}





void mtFlanger::calculate(int16_t * sbuf, int16_t * dbuf)
{
	int idx;
	short *bp;
	short frac;
	int idx1;

	if(l_delayline == NULL) return;

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

     for(int i=0; i<AUDIO_BLOCK_SAMPLES;i++)
     {
   	  *dbuf++ = *sbuf++;
     }

}
