#ifndef EFFECTOR_MTWAVETABLESMOOTHER_H_
#define EFFECTOR_MTWAVETABLESMOOTHER_H_
#include "mtEffect.h"

struct strEffectWavetableSmootherParams
{
	int windowSize;
	int windowsNumber;
};


class mtEffectWavetableSmoother : public mtEffect
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

	strEffectWavetableSmootherParams effectWavetableSmootherParams;
	uint32_t processedSamples = 0;
	uint32_t length;

	uint32_t sourceNumberOfWindows;

	float smoothWindowStep;

	float currentWindowNumber;

	int16_t * dstAddr;
	int16_t * srcAddr;

	bool state = false;
};

extern mtEffectWavetableSmoother sampleEditorWavetableSmoother;




#endif /* EFFECTOR_MTWAVETABLESMOOTHER_H_ */
