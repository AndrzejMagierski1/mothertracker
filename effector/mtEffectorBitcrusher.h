#ifndef EFFECTOR_MTEFFECTORBITCRUSHER_H_
#define EFFECTOR_MTEFFECTORBITCRUSHER_H_

#include "mtEffector.h"

class mtBitcrusher
{
	public:
	int32_t makeBitcrusher(uint8_t cBits, uint16_t sRate);

	private:
	void calculate(int16_t * src, int16_t *dst);
	uint8_t crushBits;
	uint16_t sampleStep;

};

extern mtBitcrusher effectorBitcrusher;

#endif /* EFFECTOR_MTEFFECTORBITCRUSHER_H_ */
