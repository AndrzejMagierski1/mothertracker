#ifndef EFFECTOR_MTEFFECTORLIMITER_H_
#define EFFECTOR_MTEFFECTORLIMITER_H_

#include "mtEffector.h"

#define LIMITER_THRESHOLD_MAX		10000
#define LIMITER_ATTACK_MAX			10000
#define LIMITER_RELEASE_MAX			10000

class mtLimiter
{
public:
	int8_t makeLimiter(uint16_t ts, uint16_t a, uint16_t r, int8_t forceEffect);
	void process();
	uint8_t getProgress();
	uint8_t requireProcess();
private:
	int32_t localLength;
	int32_t	returnLength;
	int16_t * localAddress;
	int16_t * destAddress;

	int16_t buffer[5];
	uint16_t threshold;
	float g=1.0;
	uint16_t xpeak;
	float attack;
	float release;
	uint16_t delay = 5;
	uint16_t a;
	float coeff;
	float f;
	void calculate(int16_t *sbuf, int16_t *dbuf);

	uint8_t requireProcessing;
	uint8_t loadProgress;
	uint32_t startLength;

	uint16_t last_ts;
	uint16_t last_a;
	uint16_t last_r;
};

extern mtLimiter effectorLimiter;
#endif /* EFFECTOR_MTEFFECTORLIMITER_H_ */
