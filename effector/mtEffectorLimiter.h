#ifndef EFFECTOR_MTEFFECTORLIMITER_H_
#define EFFECTOR_MTEFFECTORLIMITER_H_

#include "mtEffector.h"

#define LIMITER_THRESHOLD_MAX		32767U

#define LIMITER_ATTACK_MAX			1.0f

#define LIMITER_RELEASE_MAX			0.1f

class mtLimiter
{
public:
	int8_t makeLimiter(uint16_t ts, uint16_t a, float r);
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
};

extern mtLimiter effectorLimiter;
#endif /* EFFECTOR_MTEFFECTORLIMITER_H_ */
