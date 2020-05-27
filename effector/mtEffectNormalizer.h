#ifndef EFFECTOR_MTEFFECTNORMALIZER_H_
#define EFFECTOR_MTEFFECTNORMALIZER_H_

#include "mtEffect.h"

class mtEffectNormalizer : public mtEffect
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
	int find_max(int16_t * ptr, int len);

	uint32_t length;

	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;
	float amp;
	int norm_level;
	int max_peak;
	bool state = false;
};

extern mtEffectNormalizer sampleEditorNormalizer;



#endif /* EFFECTOR_MTEFFECTNORMALIZER_H_ */
