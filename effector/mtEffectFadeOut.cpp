#include "mtEffectFadeOut.h"

mtEffectFadeOut sampleEditorFadeOut;

//***********VIRTUAL SET PARAMS
void mtEffectFadeOut::setParamiter(void * ptr, uint8_t n)
{
	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectFadeOut::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;
	decreaseVolumePerSampel = 1.0f / length;
	volumeMultiplier = 1.0f;
	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectFadeOut::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	for(uint16_t i = 0; i <processedBlockLength; i++ )
	{
		*dstAddr = *srcAddr * volumeMultiplier;
		volumeMultiplier -= decreaseVolumePerSampel;
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
bool mtEffectFadeOut::getProcessState()
{
	return state;
}
uint32_t mtEffectFadeOut::getExpectedProcessLength()
{
	return confirmed.selection.length;
}

