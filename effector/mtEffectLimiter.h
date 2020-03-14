#ifndef EFFECTOR_MTEFFECTLIMITER_H_
#define EFFECTOR_MTEFFECTLIMITER_H_

#include "mtEffect.h"


class mtEffectLimiter : public mtEffect
{
public:
//***********VIRTUAL SET PARAMS
	void setParamiter(void * ptr, uint8_t n) override;
//***********
//***********VIRTUAL PROCESSING
	bool startProcess() override;
	int32_t updateProcess() override;
	bool getProcessState() override;
	uint32_t getExpectedProcessLength() override;
//***********
private:

	uint16_t threshold;
	float attack;
	float release;


	int16_t buffer[5];
	float g=1.0;
	uint16_t xpeak;

	uint16_t delay = 5;
	uint16_t a;
	float coeff;
	float f;


	uint32_t length;

	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;
	bool state = false;
};

extern mtEffectLimiter sampleEditorLimiter;




#endif /* EFFECTOR_MTEFFECTLIMITER_H_ */
