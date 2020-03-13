#ifndef EFFECTOR_MTEFFECTAMPLIFIER_H_
#define EFFECTOR_MTEFFECTAMPLIFIER_H_

#include "mtEffect.h"

class mtEffectAmplifier : public mtEffect
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
	uint32_t length;

	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;
	float amp;
	bool state = false;
};

extern mtEffectAmplifier sampleEditorAmplifier;



#endif /* EFFECTOR_MTEFFECTAMPLIFIER_H_ */
