#ifndef EFFECTOR_MTEFFECTORLIMITER_H_
#define EFFECTOR_MTEFFECTORLIMITER_H_

#include "mtEffector.h"

class mtLimiter
{
public:
	int8_t makeLimiter(uint16_t ts, uint16_t a, uint16_t r );
private:
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
};

extern mtLimiter effectorLimiter;
#endif /* EFFECTOR_MTEFFECTORLIMITER_H_ */
