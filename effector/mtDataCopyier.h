#ifndef EFFECTOR_MTDATACOPYIER_H_
#define EFFECTOR_MTDATACOPYIER_H_
#include "stdint.h"

class mtDataCopyier
{
public:
	mtDataCopyier(uint32_t blockSize);
	void start(int16_t * src, int16_t * dst, uint32_t len);
	int32_t update();
private:
	int16_t * currentSrc = nullptr;
	int16_t * currentDst = nullptr;
	uint32_t  length;
	uint32_t  currentLength;
	uint32_t  blockSizeInSamples;
	bool state;
};


#endif /* EFFECTOR_MTDATACOPYIER_H_ */
