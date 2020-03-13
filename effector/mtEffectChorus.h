#ifndef EFFECTOR_MTEFFECTCHORUS_H_
#define EFFECTOR_MTEFFECTCHORUS_H_

#include "mtEffect.h"

constexpr int CHORUS_BUFFER_LENGTH = 4096;

class mtEffectChorus : public mtEffect
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
	void calculate(int16_t * src, int16_t* dst); //process 128 samples

	uint32_t length;

	int16_t * srcAddr;
	int16_t * dstAddr;

	uint32_t processedSamples = 0;

	int16_t * l_delayline;
	int16_t l_circ_idx;
	int num_chorus;
	int delay_length;

	bool state = false;
};

extern mtEffectChorus sampleEditorChorus;



#endif /* EFFECTOR_MTEFFECTCHORUS_H_ */
