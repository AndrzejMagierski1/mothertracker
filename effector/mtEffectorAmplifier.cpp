#include "mtEffectorAmplifier.h"

mtAmplifier effectorAmplifier;


int32_t mtAmplifier::makeAmplifier(float amp)
{
	uint32_t srcLength = effector.getLength();
	int32_t returnLength = (int32_t) srcLength;

	int16_t * srcAddress = effector.getAddress();
	int16_t * destAddress = sdram_effectsBank;
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

	effector.affterEffectLength=returnLength/2;
	return 1;
}

