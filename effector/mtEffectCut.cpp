#include"mtEffectCut.h"

mtEffectCut sampleEditorCut;

//***********VIRTUAL SET PARAMS
void mtEffectCut::setParamiter(void * ptr, uint8_t n)
{
	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectCut::startProcess()
{
	processingWithoutCopying = true;

	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr ;
	dstAddr = processed.area.addr;

	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectCut::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;


	memcpy(dstAddr,srcAddr, 2 * processedBlockLength);
	dstAddr+=processedBlockLength;
	srcAddr+=processedBlockLength;

	if(processedBlockLength < 8192)
	{
		state = 0;
	}

	processedSamples += processedBlockLength;

	if(!state)
	{
		processed.selection.length = processedSamples; //redundancy
		processed.area.length = processedSamples;

		processed.selection.addr = processed.area.addr;
	}

	return processedBlockLength;

}
bool mtEffectCut::getProcessState()
{
	return state;
}
uint32_t mtEffectCut::getExpectedProcessLength()
{
	return confirmed.selection.length;
}
