#include "mtEffectFadeIn.h"

mtEffectFadeIn sampleEditorFadeIn;

//***********VIRTUAL SET PARAMS
void mtEffectFadeIn::setParamiter(void * ptr, uint8_t n)
{
	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectFadeIn::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;
	increaseVolumePerSampel = 1.0f / length;
	volumeMultiplier = 0.0f;
	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectFadeIn::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	for(uint16_t i = 0; i <processedBlockLength; i++ )
	{
		*dstAddr = *srcAddr * volumeMultiplier;
		volumeMultiplier += increaseVolumePerSampel;
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
bool mtEffectFadeIn::getProcessState()
{
	return state;
}
uint32_t mtEffectFadeIn::getExpectedProcessLength()
{
	return confirmed.selection.length;
}

