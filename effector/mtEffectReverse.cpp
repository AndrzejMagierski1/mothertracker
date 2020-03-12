#include "mtEffectReverse.h"

mtEffectReverse sampleEditorReverse;

//***********VIRTUAL SET PARAMS
void mtEffectReverse::setParamiter(void * ptr, uint8_t n)
{
	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectReverse::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr ;
	dstAddr = processed.selection.addr + confirmed.selection.length;

	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectReverse::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	for(uint16_t i = 0; i <processedBlockLength; i++ )
	{
		*dstAddr = *srcAddr;
		dstAddr--;
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
bool mtEffectReverse::getProcessState()
{
	return state;
}
uint32_t mtEffectReverse::getExpectedProcessLength(uint32_t selectLen)
{
	return selectLen;
}

