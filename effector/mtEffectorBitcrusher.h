#ifndef EFFECTOR_MTEFFECTORBITCRUSHER_H_
#define EFFECTOR_MTEFFECTORBITCRUSHER_H_

#include "mtEffector.h"

#define BITCRUSHER_BITS_MAX			16U
#define BITCRUSHER_RATE_MAX			44100U

class mtBitcrusher
{
	public:
	int32_t makeBitcrusher(uint8_t cBits, uint16_t sRate);
	void process();
	uint8_t getProgress();
	uint8_t requireProcess();

	private:
	void calculate(int16_t * src, int16_t *dst);
	uint8_t crushBits;
	uint16_t sampleStep;


	uint32_t localLength;
	int32_t returnLength;

	int16_t * localAddress;
	int16_t * destAddress;


	uint8_t requireProcessing;
	uint8_t loadProgress;
	uint32_t startLength;

};

extern mtBitcrusher effectorBitcrusher;

#endif /* EFFECTOR_MTEFFECTORBITCRUSHER_H_ */
