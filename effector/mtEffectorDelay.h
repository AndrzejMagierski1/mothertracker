#ifndef EFFECTOR_MTEFFECTORDELAY_H_
#define EFFECTOR_MTEFFECTORDELAY_H_

#include "mtEffector.h"
#include "arm_math.h"

class mtDelay
{

 public:

	int32_t makeDelay(float feedback,uint16_t time);

};

extern mtDelay effectorDelay;
#endif /* EFFECTOR_MTEFFECTORDELAY_H_ */
