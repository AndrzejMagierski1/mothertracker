#include "mtEffectAmplifier.h"

mtEffectAmplifier sampleEditorAmplifier;

//***********VIRTUAL SET PARAMS
void mtEffectAmplifier::setParamiter(void * ptr, uint8_t n)
{
	if(n > 0) return;

	amp = *((float *)ptr);
	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectAmplifier::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;

	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectAmplifier::updateProcess()
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
bool mtEffectAmplifier::getProcessState()
{
	return state;
}
uint32_t mtEffectAmplifier::getExpectedProcessLength()
{
	return confirmed.selection.length;
}

