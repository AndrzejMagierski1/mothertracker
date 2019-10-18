#include "mtEffectorAmplifier.h"

mtAmplifier effectorAmplifier;


int32_t mtAmplifier::makeAmplifier(float amp, int8_t forceEffect)
{
	if(forceEffect == 0)
	{
		if(lastAmp == amp)
		{
			return 0;
		}
	}

	uint32_t srcLength = effector.getLength();
	int32_t returnLength = (int32_t) srcLength;

	int16_t * srcAddress = effector.getAddress();
	int16_t * destAddress = effector.previewBuffer;
	int32_t currentValue;

	for(uint32_t i = 0; i< srcLength; i++)
	{
		currentValue = amp * (float)(*srcAddress) ;
		if(currentValue > MAX_SIGNED_16BIT) currentValue = MAX_SIGNED_16BIT;
		if(currentValue < MIN_SIGNED_16BIT) currentValue = MIN_SIGNED_16BIT;
		*destAddress=(int16_t)currentValue;
		srcAddress++;
		destAddress++;
	}

	lastAmp = amp;
	effector.affterEffectLength=returnLength/2;
	return 1;
}

