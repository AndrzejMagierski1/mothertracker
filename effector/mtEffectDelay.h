#ifndef EFFECTOR_MTEFFECTDELAY_H_
#define EFFECTOR_MTEFFECTDELAY_H_
#include "mtEffect.h"

struct strEffectDelayParams
{
	float feedback;
	int time;
};

constexpr uint8_t MAX_DELAY_VOICE_NUMBER = 99;

class mtEffectDelay : public mtEffect
{
public:
//***********VIRTUAL SET PARAMS
	void setParamiter(void * ptr, uint8_t n);
//***********
//***********VIRTUAL PROCESSING
	bool startProcess() override;
	int32_t updateProcess() override;
	bool getProcessState() override;
	uint32_t getExpectedProcessLength() override;
//***********
private:
	void calcLastVoiceIdx();
	strEffectDelayParams effectDelayParams;

	int16_t * currentDstAddr;

	int16_t * sourceVoicePointer[MAX_DELAY_VOICE_NUMBER];
	float feedbackVoiceMult[MAX_DELAY_VOICE_NUMBER];
	uint32_t lastIndexInVoice[MAX_DELAY_VOICE_NUMBER];
	int16_t * lastSrcAddr;
	uint8_t lastVoiceIdx;

	uint8_t firstActiveVoiceIdx = 0;
	uint8_t lastActiveVoiceIdx = 0;
	uint32_t processedSamples = 0;

	bool state = false;
};

extern mtEffectDelay sampleEditorDelay;


#endif /* EFFECTOR_MTEFFECTDELAY_H_ */
