#ifndef EFFECTOR_MTEFFECTORCOMPRESSOR_H_
#define EFFECTOR_MTEFFECTORCOMPRESSOR_H_
#include "mtEffector.h"

class mtCompressor
{
public:

int8_t makeCompressor(uint16_t cT, int16_t cR, uint16_t eT, int16_t eR, uint16_t at, uint16_t rt);

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


};

extern mtCompressor effectorCompressor;

#endif /* EFFECTOR_MTEFFECTORCOMPRESSOR_H_ */
