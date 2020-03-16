#include "mtEffectCompresor.h"

//***********VIRTUAL SET PARAMS
void mtEffectCompresor::setParamiter(void * ptr, uint8_t n)
{
	switch(n)
	{
		case 0: 		interfaceParams.threshold = *((int*)ptr);			break;
		case 1: 		interfaceParams.ratio = *((int*)ptr);				break;
		case 2: 		interfaceParams.attack = *((int*)ptr);			break;
		case 3: 		interfaceParams.release	= *((int*)ptr);				break;
		default: break;
	}
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectCompresor::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;


	compressorThreshold=10*log10f(pow(interfaceParams.threshold,2));
	expanderThreshold= 10*log10f(pow(1,2));
	compressorRatio= interfaceParams.ratio;
	expanderRatio = 1.0/1000.0;
	attackTime= interfaceParams.attack/1000.0;
	releaseTime= interfaceParams.release/1000.0;
	tav = 0.01;
	delay = 150;
	xrms = 0;
	g = 1;

	memset(buffer,0,delay*2);


	processedSamples = 0;

	state = true;

	return true;

}
int32_t mtEffectCompresor::updateProcess()
{
	if(!state) return -1;

	float temp1;
	float temp2;

	int32_t dif = length - processedSamples;
	int32_t processedBlockLength = dif > 8192 ? 8192 : dif;

	if(processedBlockLength < 8192) state = false;

	for(int n = 0; n < processedBlockLength; n++)
	{
		xrms = (1-tav) * xrms + tav * powf(*srcAddr,2);
		X=10*log10f(xrms);

		temp1=compressorRatio*(compressorThreshold - X);
		temp2=expanderRatio*(expanderThreshold - X );
		if(temp1 > temp2) temp1 = temp2;
		if(temp1 < 0) G = temp1;
		else G = 0;

		f= powf(10, G/20.0);

		if(f<g) coeff = attackTime;
		else coeff = releaseTime;
		g = (1-coeff) * g + coeff*f;

		if((processedSamples == 0) && (processedBlockLength < delay)) *dstAddr++ = *srcAddr;
		else *dstAddr++ = g*buffer[delay-1];

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
bool mtEffectCompresor::getProcessState()
{
	return state;
}
uint32_t mtEffectCompresor::getExpectedProcessLength()
{
	return  confirmed.selection.length;
}

mtEffectCompresor sampleEditorCompresor;
//***********




