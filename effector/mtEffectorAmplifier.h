#ifndef EFFECTOR_MTEFFECTORAMPLIFIER_H_
#define EFFECTOR_MTEFFECTORAMPLIFIER_H_

#include "mtEffector.h"

#define AMPLIFIER_AMP_MAX		100

class mtAmplifier
{
 public:
	int32_t makeAmplifier(float amp);
 private:

};

extern mtAmplifier effectorAmplifier;
#endif /* EFFECTOR_MTEFFECTORAMPLIFIER_H_ */
