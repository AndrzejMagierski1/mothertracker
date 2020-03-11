#ifndef EFFECTOR_MTEFFECTBITCRUSHER_H_
#define EFFECTOR_MTEFFECTBITCRUSHER_H_

#include "mtEffect.h"

struct strEffectBitcrusherParams
{
	int rate;
	int bits;
};

class mtEffectBitcrusher : public mtEffect
{
public:
//***********VIRTUAL SET PARAMS
	void setParamiter(void * ptr, uint8_t n) override;
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
