#ifndef EFFECTOR_MTEFFECTORCOMPRESSOR_H_
#define EFFECTOR_MTEFFECTORCOMPRESSOR_H_
#include "mtEffector.h"

#define CMPSR_THRESHOLD_MAX			32767U
#define	CMPSR_RATIO_MAX				10
#define CMPSR_ATTACK_MAX_MS			1.0f
#define CMPSR_RELEASE_MAX_MS		0.1f


class mtCompressor
{
public:

int8_t makeCompressor(uint16_t cT, int16_t cR, uint16_t at, uint16_t rt);

void process();
uint8_t getProgress();
uint8_t requireProcess();

private:
void calculate(int16_t * sbuf, int16_t * dbuf);

float compressorThreshold;
float expanderThreshold;
float compressorRatio;
float expanderRatio;
float attackTime;
float releaseTime;
float tav;
uint8_t delay;
float xrms;
float g;
int16_t buffer[150];
float X;
float G;
float f;
float coeff;

uint8_t requireProcessing;
int32_t localLength;
int32_t	returnLength;
int16_t * localAddress;
int16_t * destAddress;


uint32_t startLength;
uint8_t loadProgress;

};

extern mtCompressor effectorCompressor;

#endif /* EFFECTOR_MTEFFECTORCOMPRESSOR_H_ */
