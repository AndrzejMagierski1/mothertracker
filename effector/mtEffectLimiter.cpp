#include "mtEffectLimiter.h"

//***********VIRTUAL SET PARAMS
void mtEffectLimiter::setParamiter(void * ptr, uint8_t n)
{
	switch(n)
	{
		case 0: 		threshold = *((int*)ptr);			break;
		case 1: 		attack = *((float*)ptr)/1000.0;		break;
		case 2: 		release = *((float*)ptr)/1000.0;	break;
		default: break;
	}
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectLimiter::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;


	processedSamples = 0;

	state = true;

	return true;

}
int32_t mtEffectLimiter::updateProcess()
{
	if(!state) return -1;


	int32_t dif = length - processedSamples;
	int32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	if(processedBlockLength < 8192) state = false;

	for(int n = 0; n < processedBlockLength; n++)
	{
		a = (*srcAddr<0) ? -*srcAddr : *srcAddr;

		coeff = (a > xpeak) ? attack : release;

		xpeak = (1-coeff) * xpeak + coeff * a;

		f = threshold/(float)xpeak;
		f = (f < 1.0f) ? f : 1.0f;

		coeff = (f<g) ? attack : release;

		g = (1-coeff) *g + coeff * f;

		*dstAddr++ = g * buffer[delay-1];

		for(uint8_t i = delay-1; i>=1 ; i--)
		{
			buffer[i]=buffer[i-1];
		}
		buffer[0]=*srcAddr++;
	}

	processedSamples += processedBlockLength;

	if(!state)
	{
		processed.selection.length = processedSamples; //redundancy
		processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
	}

	return processedBlockLength;
}
bool mtEffectLimiter::getProcessState()
{
	return state;
}
uint32_t mtEffectLimiter::getExpectedProcessLength()
{
	return  confirmed.selection.length;
}

mtEffectLimiter sampleEditorLimiter;
//***********


