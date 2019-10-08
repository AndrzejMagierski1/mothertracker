#ifndef EFFECTOR_MTEFFECTORDELAY_H_
#define EFFECTOR_MTEFFECTORDELAY_H_

#include "mtEffector.h"
#include "arm_math.h"


#define DELAY_FEEDBACK_MAX 		0.6f
#define DELAY_TIME_MAX			500

#define DELAY_FEEDBACK_STEP		0.01f
#define DELAY_TIME_STEP			10

class mtDelay
{

 public:

	uint8_t makeDelay(float feedback, uint16_t time, uint8_t forceEffect);

	void process();
	uint8_t getProgress();
	uint8_t requireProcess();
	void cancelProcessing();

 private:
	int16_t * destAddress;

	int16_t * sPointer[99];
	float feedbackTab[99];
	uint32_t lastIdxSmpTime[99];
	int16_t * compareAddr;
	uint8_t compareIdx;

	uint8_t firstIndex=0;
	uint8_t lastIndex=0;
	uint32_t sampleCnt=0;

	uint32_t maxLoad;


	float last_feedback;
	uint16_t last_time;

	uint8_t requireProcessing;
	uint8_t loadProgress;

};

extern mtDelay effectorDelay;
#endif /* EFFECTOR_MTEFFECTORDELAY_H_ */
