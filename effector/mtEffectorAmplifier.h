#ifndef EFFECTOR_MTEFFECTORAMPLIFIER_H_
#define EFFECTOR_MTEFFECTORAMPLIFIER_H_

#include "mtEffector.h"

#define AMPLIFIER_AMP_MAX		100

class mtAmplifier
{
 public:
	int32_t makeAmplifier(float amp, int8_t forceEffect);
 private:

	float lastAmp;

};

extern mtAmplifier effectorAmplifier;
#endif /* EFFECTOR_MTEFFECTORAMPLIFIER_H_ */
