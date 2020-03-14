#ifndef EFFECTOR_MTEFFECTFLANGER_H_
#define EFFECTOR_MTEFFECTFLANGER_H_

#include "mtEffect.h"

constexpr int FLANGER_BUFFER_LENGTH = 4096;

class mtEffectFlanger : public mtEffect
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

	int delay_length;
	int offset_percent;
	int delay_depth;
	int delay_rate_incr;

	int delay_offset_idx;
	int16_t *l_delayline;
	int16_t l_circ_idx;
	uint32_t l_delay_rate_index;

	bool state = false;
};

extern mtEffectFlanger sampleEditorFlanger;



#endif /* EFFECTOR_MTEFFECTFLANGER_H_ */
