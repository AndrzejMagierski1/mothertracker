#include "mtEffectFlanger.h"
#include "sdram.h"
#include "arm_math.h"

mtEffectFlanger sampleEditorFlanger;

__NOINIT(EXTERNAL_RAM) int16_t flangerDelayBuffer[FLANGER_BUFFER_LENGTH];

//***********VIRTUAL SET PARAMS
void mtEffectFlanger::setParamiter(void * ptr, uint8_t n)
{
	if(n > 3) return;

	switch(n)
	{
	case 0:		delay_length = *((int*)ptr)/2;	 								 break;
	case 1: 	offset_percent = *((int*)ptr);	 							 break;
	case 2: 	delay_depth	= *((int*)ptr);	 									 break;
	case 3: 	delay_rate_incr	= (*((float*)ptr) * 2147483648.0)/ 44117.64706;	 break;
	default: break;
	}


	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectFlanger::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;

	delay_offset_idx = (offset_percent * delay_length)/ 100.0;
	l_delay_rate_index = 0;
	l_circ_idx = 0;
	l_delayline = flangerDelayBuffer;

	if(delay_offset_idx < -1)
	{
	    delay_offset_idx = 0;
	}
	if(delay_offset_idx >= delay_length)
	{
	    delay_offset_idx = delay_length - 1;
	}

	memset(l_delayline,0,2 * delay_length);

	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectFlanger::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;
	uint32_t processingNumber = processedBlockLength / 128;

	for(int i = 0; i < (int)processingNumber; i++)
	{
		calculate(srcAddr, dstAddr);
		srcAddr+=128;
		dstAddr+=128;
	}

	if(processedBlockLength < 8192)
	{
		processedBlockLength = processingNumber * 128;
		state = 0;
	}

	processedSamples += processedBlockLength;

	if(!state)
	{
		processed.selection.length = processedSamples; //redundancy
		processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
	}

	return processedBlockLength;

}
bool mtEffectFlanger::getProcessState()
{
	return state;
}
uint32_t mtEffectFlanger::getExpectedProcessLength()
{
	return confirmed.selection.length - confirmed.selection.length%128;
}

void mtEffectFlanger::calculate(int16_t * src, int16_t* dst) //process 128 samples
{
	constexpr uint16_t PROCESSED_BLOCK_LENGTH = 128;
	constexpr int FLANGE_DELAY_PASSTHRU = 0;
	int idx;
	int16_t frac;
	int idx1;
	int16_t tempBuf[PROCESSED_BLOCK_LENGTH];

	memcpy(tempBuf, src, 2*PROCESSED_BLOCK_LENGTH);

	if(delay_offset_idx == FLANGE_DELAY_PASSTHRU)
	{
		for(int i = 0; i < PROCESSED_BLOCK_LENGTH; i++)
		{
			l_circ_idx++;
			if(l_circ_idx >= delay_length) l_circ_idx = 0;
			l_delayline[l_circ_idx] = *src;
			*dst++ = *src++;
		}
		return;
	 }
	 for(int i = 0; i < PROCESSED_BLOCK_LENGTH; i++)
	 {
	   l_circ_idx++;

	   if(l_circ_idx >= delay_length)  l_circ_idx = 0;

	   l_delayline[l_circ_idx] = *src++;
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
	   *dst++ = (l_delayline[l_circ_idx]+ l_delayline[idx] + frac)/2;

	   l_delay_rate_index += delay_rate_incr;
	   if(l_delay_rate_index & 0x80000000) l_delay_rate_index &= 0x7fffffff;
	 }
}



