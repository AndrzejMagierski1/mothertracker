#include "mtWavetableSmoother.h"

mtEffectWavetableSmoother sampleEditorWavetableSmoother;


//***********VIRTUAL SET PARAMS
void mtEffectWavetableSmoother::setParamiter(void * ptr, uint8_t n)
{
	switch(n)
	{
		case 0 :
			effectWavetableSmootherParams.windowsNumber = *((int*)ptr);
			break;
		case 1 :
			effectWavetableSmootherParams.windowSize = *((int*)ptr);
			break;
		default: break;
	}
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectWavetableSmoother::startProcess()
{
	if(effectWavetableSmootherParams.windowsNumber== 0 || effectWavetableSmootherParams.windowSize == 0)
	{
		return false;
	}

	sourceNumberOfWindows = confirmed.selection.length/effectWavetableSmootherParams.windowSize;
	if(sourceNumberOfWindows > (uint32_t)effectWavetableSmootherParams.windowsNumber)
	{
		sourceNumberOfWindows = effectWavetableSmootherParams.windowsNumber;
	}
	smoothWindowStep = sourceNumberOfWindows/(float)effectWavetableSmootherParams.windowsNumber;
	currentWindowNumber = 0;

	dstAddr = processed.selection.addr;
	srcAddr = confirmed.selection.addr;

	processedSamples = 0;
	length = effectWavetableSmootherParams.windowsNumber * effectWavetableSmootherParams.windowSize;

	state = true;
	return true;

}
int32_t mtEffectWavetableSmoother::updateProcess()
{
	if(!state) return -1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	if(sourceNumberOfWindows == (uint32_t)effectWavetableSmootherParams.windowsNumber )
	{
		for(uint32_t i = 0; i < processedBlockLength; i++)
		{
			dstAddr[processedSamples] = srcAddr[processedSamples];
			processedSamples++;
		}
	}
	else
	{
		for(uint32_t i = 0; i < processedBlockLength; i++)
		{
			uint32_t currentSampleInWindow = processedSamples % effectWavetableSmootherParams.windowSize;

			dstAddr[processedSamples] = srcAddr[(int)( ((int)currentWindowNumber * (effectWavetableSmootherParams.windowSize - 1))  + currentSampleInWindow)];


			if(!(processedSamples % (effectWavetableSmootherParams.windowSize -1))) currentWindowNumber += smoothWindowStep;
			processedSamples++;

		}
	}

	if(processedBlockLength < 8192)
	{
		state = false;
		processed.selection.length = processedSamples; //redundancy
		processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
	}

	return processedBlockLength;
}
bool mtEffectWavetableSmoother::getProcessState()
{
	return state;
}
uint32_t mtEffectWavetableSmoother::getExpectedProcessLength()
{
	return  effectWavetableSmootherParams.windowsNumber * effectWavetableSmootherParams.windowSize;
}

