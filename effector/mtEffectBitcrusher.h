#ifndef EFFECTOR_MTEFFECTBITCRUSHER_H_
#define EFFECTOR_MTEFFECTBITCRUSHER_H_

#include "mtEffect.h"

struct strEffectBitcrusherParams
{
	uint16_t rate;
	uint8_t bits;
};

class mtEffectBitcrusher : public mtEffect
{
public:
//***********VIRTUAL SET PARAMS
	void setParams(void * ptr) override;
//***********
//***********VIRTUAL PROCESSING
	bool startProcess() override;
	int32_t updateProcess() override;
	bool getProcessState() override;
	uint32_t getExpectedProcessLength(uint32_t selectLen) override;
//***********
private:
	strEffectBitcrusherParams effectBitcrusherParams;

	uint8_t crushBits;
	uint16_t sampleStep;

	uint32_t length;

	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;
	bool state = false;
};

extern mtEffectBitcrusher sampleEditorBitcrusher;




#endif /* EFFECTOR_MTEFFECTBITCRUSHER_H_ */
