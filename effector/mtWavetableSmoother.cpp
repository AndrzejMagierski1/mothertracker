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

	sourceWindowMax = (confirmed.selection.length/effectWavetableSmootherParams.windowSize) - 1;
	if(sourceWindowMax > ((uint32_t)effectWavetableSmootherParams.windowsNumber - 1 ))
	{
		sourceWindowMax = effectWavetableSmootherParams.windowsNumber - 1;
	}
	smoothWindowStep = sourceWindowMax/(float)effectWavetableSmootherParams.windowsNumber;
	currentWindowNumber = 0;
	iCurrentWindowNumber = 0;
	iLastWindowNumber = 0;

	dstAddr = processed.selection.addr;
	srcAddr = confirmed.selection.addr;

	processedSamples = 0;
	length = effectWavetableSmootherParams.windowsNumber * effectWavetableSmootherParams.windowSize;

	currentCrossfadeCoef =  1.0;
	crossfadeStep = 1.0 / getSmoothStepNumber();

	state = true;
	return true;

}
int32_t mtEffectWavetableSmoother::updateProcess()
{
	if(!state) return -1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	if(sourceWindowMax == (uint32_t)effectWavetableSmootherParams.windowsNumber )
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

			dstAddr[processedSamples] = (currentCrossfadeCoef * srcAddr[(int)( ((int)currentWindowNumber * (effectWavetableSmootherParams.windowSize - 1))  + currentSampleInWindow)])
			+ (((uint32_t)currentWindowNumber == sourceWindowMax) ? 0 : (1.0 - currentCrossfadeCoef) * srcAddr[(int)( (((int)currentWindowNumber+1) * (effectWavetableSmootherParams.windowSize - 1))  + currentSampleInWindow)]);


			if(!(processedSamples % (effectWavetableSmootherParams.windowSize -1)))
			{

				currentWindowNumber += smoothWindowStep;
				currentCrossfadeCoef -= crossfadeStep;
				if(detectChangeSourceWindow())
				{
					currentCrossfadeCoef =  1.0;
					crossfadeStep = 1.0 / getSmoothStepNumber();
				}
			}
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

bool  mtEffectWavetableSmoother::detectChangeSourceWindow()
{
	bool result = false;

	iCurrentWindowNumber = (int)currentWindowNumber;

	if(iCurrentWindowNumber != iLastWindowNumber )
	{
		result = true;
	}

	iLastWindowNumber = iCurrentWindowNumber;

	return result;
}

uint32_t mtEffectWavetableSmoother::getSmoothStepNumber()
{
	float localCurrentWindow = currentWindowNumber;

	int iLocalCurrentWindow = (int) localCurrentWindow;
	int iLocalLastWindow = iLocalCurrentWindow;

	uint32_t result = 0;

	while(1)
	{
		result++;
		localCurrentWindow += smoothWindowStep;

		iLocalCurrentWindow = (int) localCurrentWindow;

		if(iLocalCurrentWindow != iLocalLastWindow)
		{
			return result;
		}

		iLocalLastWindow = iLocalCurrentWindow;
	}
}

