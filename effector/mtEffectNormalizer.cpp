#include "mtEffectNormalizer.h"
mtEffectNormalizer sampleEditorNormalizer;

//***********VIRTUAL SET PARAMS
void mtEffectNormalizer::setParamiter(void * ptr, uint8_t n)
{
	if(n > 0) return;

	norm_level = *((int *)ptr);
	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectNormalizer::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;

	processedSamples = 0;
	state = true;

	max_peak = find_max(srcAddr,length);
	int norm_peak = map(norm_level,0,100,0,MAX_SIGNED_16BIT);

	amp = (float)norm_peak/max_peak;

	return true;
}
int32_t mtEffectNormalizer::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	for(uint16_t i = 0; i <processedBlockLength; i++ )
	{
		int32_t processVal = amp * (*srcAddr);

		if(processVal > MAX_SIGNED_16BIT) processVal = MAX_SIGNED_16BIT;
		if(processVal < MIN_SIGNED_16BIT ) processVal = MIN_SIGNED_16BIT;

		*dstAddr = (int16_t)processVal;
		dstAddr++;
		srcAddr++;
	}

	if(processedBlockLength < 8192)
	{
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
bool mtEffectNormalizer::getProcessState()
{
	return state;
}
uint32_t mtEffectNormalizer::getExpectedProcessLength()
{
	return confirmed.selection.length;
}

int mtEffectNormalizer::find_max(int16_t * ptr, int len)
{
	int max = 0;
	if((ptr == nullptr) || (len == 0)) return 0;

	while(len--)
	{
		int abs = ( *ptr > 0 ) ? *ptr : - (*ptr);
		if(abs > max) max = abs;

		ptr++;
	}

	return max;
}


