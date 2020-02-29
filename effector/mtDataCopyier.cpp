#include "mtDataCopyier.h"
#include "WProgram.h"

mtDataCopyier::mtDataCopyier(uint32_t blockSize) : blockSizeInSamples(blockSize){};

void mtDataCopyier::start(int16_t * src, int16_t * dst, uint32_t len)
{
	currentSrc = src;
	currentDst = dst;
	length = len;
	currentLength = 0;
	state = true;
}
int32_t mtDataCopyier::update()
{
	if(!state) return -1;

	uint32_t dif = length - currentLength;
	uint32_t blockSize = (dif > blockSize) ? blockSize : dif;

	memcpy(currentDst,currentSrc,2*blockSize);

	return blockSize;

}
