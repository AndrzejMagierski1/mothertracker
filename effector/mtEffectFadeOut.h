#ifndef EFFECTOR_MTEFFECTFADEOUT_H_
#define EFFECTOR_MTEFFECTFADEOUT_H_

#include "mtEffect.h"

class mtEffectFadeOut : public mtEffect
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
	float decreaseVolumePerSampel;
	float volumeMultiplier;
	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;
	bool state = false;
};

extern mtEffectFadeOut sampleEditorFadeOut;




#endif /* EFFECTOR_MTEFFECTREVERSE_H_ */
