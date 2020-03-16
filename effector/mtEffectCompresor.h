#ifndef EFFECTOR_MTEFFECTCOMPRESOR_H_
#define EFFECTOR_MTEFFECTCOMPRESOR_H_

#include "mtEffect.h"

class mtEffectCompresor : public mtEffect
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

	struct strCompresorParams
	{
		int threshold;
		int ratio;
		int attack;
		int release;
	} interfaceParams;

	float compressorThreshold;
	float compressorRatio;
	float attackTime;
	float releaseTime;

	float expanderThreshold;
	float expanderRatio;



	float tav;
	int delay;
	float xrms;
	float g;
	int16_t buffer[150];
	float X;
	float G;
	float f;
	float coeff;



	uint32_t length;

	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;
	bool state = false;
};

extern mtEffectCompresor sampleEditorCompresor;




#endif /* EFFECTOR_MTEFFECTCOMPRESOR_H_ */
