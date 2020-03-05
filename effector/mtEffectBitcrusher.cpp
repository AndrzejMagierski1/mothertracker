#include "mtEffectBitcrusher.h"
#include "mtStructs.h"


//***********VIRTUAL SET PARAMS
void mtEffectBitcrusher::setParams(void * ptr)
{
	effectBitcrusherParams = *((strEffectBitcrusherParams*)ptr);
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectBitcrusher::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;

	int n = (AUDIO_SAMPLE_RATE_EXACT / effectBitcrusherParams.rate) + 0.5;
	if (n < 1) n = 1;
	else if (n > 64) n = 64;
	sampleStep = n;

	if (effectBitcrusherParams.bits > 16) effectBitcrusherParams.bits = 16;
	else if (effectBitcrusherParams.bits == 0) effectBitcrusherParams.bits = 1;
	crushBits = effectBitcrusherParams.bits;

	return true;

}
int32_t mtEffectBitcrusher::updateProcess()
{
	if(!state) return -1;

	int32_t processedBlockLength = 0;

	uint32_t sampleSquidge, sampleSqueeze;
	int16_t	* localAddress = srcAddr;

	for(uint32_t i = 0; i < 8192; i++)
	{
		if( (processedSamples <= length ) && (processedSamples <= SAMPLE_EFFECTOR_LENGTH_MAX) )
		{
			if (crushBits == 16 && sampleStep <= 1) //todo: zmienic zeby robil oddzielna petle a nie za kazdym razem sprawdzal
			{
					*dstAddr++=*srcAddr++;
					processedBlockLength++;
			}
			else
			{
				//todo: na razie nie dokonczone po prostu na potrzeby projektu
			}
		}
		else
		{
			state = false;
			processed.selection.length = processedSamples; //redundancy
			processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
		}
	}

	processedSamples += processedBlockLength;

	return processedBlockLength;
}
bool mtEffectBitcrusher::getProcessState()
{
	return state;
}
uint32_t mtEffectBitcrusher::getExpectedProcessLength(uint32_t selectLen)
{
	return  selectLen;
}

mtEffectBitcrusher sampleEditorBitcrusher;
//***********



