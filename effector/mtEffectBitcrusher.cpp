#include "mtEffectBitcrusher.h"
#include "mtStructs.h"


//***********VIRTUAL SET PARAMS
void mtEffectBitcrusher::setParamiter(void * ptr, uint8_t n)
{
	switch(n)
	{
		case 0: 		effectBitcrusherParams.bits = *((uint8_t*)ptr); 		break;
		case 1: 		effectBitcrusherParams.rate = *((uint16_t*)ptr);		break;
		default: break;
	}
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectBitcrusher::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;

	int n = (44117.64706 / effectBitcrusherParams.rate) + 0.5;
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


	if(state)
	{
		if (crushBits == 16 && sampleStep <= 1)
		{
			for(uint16_t i = 0; i < 8192; i++)
			{
				*dstAddr++ = *srcAddr++;
				processedBlockLength++;

				if(processedBlockLength + processedSamples > length)
				{
					state = 0;
					break;
				}
			}
		}
		else
		{
			if (sampleStep <= 1)
			{
				for (uint16_t i = 0; i < 8192; i++)
				{
					//szift w prawo i lewo czysci bity lsb
					sampleSquidge = *srcAddr++ >> (16 - crushBits);

					*dstAddr++ = sampleSquidge << (16 - crushBits);
					processedBlockLength++;

					if(processedBlockLength + processedSamples > length)
					{
						state = 0;
						break;
					}
				}
			}
			else if (crushBits == 16)
			{
				uint32_t i = 0;
				while (i < 8192)
				{
					sampleSqueeze = *srcAddr;
					for (int j = 0; j < sampleStep && i < 8192; j++)
					{
						*dstAddr++ = sampleSqueeze;
						srcAddr++;
						i++;
						processedBlockLength++;

						if(processedBlockLength + processedSamples > length)
						{
							state = 0;
							break;
						}
					}
				}
			}
			else
			{
				uint32_t i = 0;
				while (i < 8192)
				{
					sampleSqueeze = *srcAddr;
					for (int j = 0; j < sampleStep && i < 8192; j++)
					{
						sampleSquidge = sampleSqueeze >> (16 - crushBits);
						*dstAddr++ = sampleSquidge << (16 - crushBits);
						srcAddr++;
						i++;
						processedBlockLength++;

						if(processedBlockLength + processedSamples > length)
						{
							state = 0;
							break;
						}
					}
				}
			}
		}
	}

	processedSamples += processedBlockLength;

	if(!state)
	{
		processed.selection.length = processedSamples; //redundancy
		processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
	}

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



