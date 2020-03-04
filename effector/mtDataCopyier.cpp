#include "mtDataCopyier.h"
#include "WProgram.h"

mtDataCopyier::mtDataCopyier(uint32_t blockSize) : blockSizeInSamples(blockSize){};

constexpr uint32_t LOAD_BLOCK_SIZE_IN_SAMPLES = 8192;

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

	int32_t dif = length - currentLength;
	int32_t blockSize = blockSizeInSamples;
	if(dif < (int32_t)blockSizeInSamples)
	{
		state = false;
		blockSize = dif;
	}

	memcpy(currentDst,currentSrc,2*blockSize);
	currentDst+=blockSize;
	currentSrc+=blockSize;
	currentLength += blockSize;

	return blockSize;
}

bool mtDataCopyier::getState()
{
	return state;
}

mtDataCopyier globalDataCopyier(LOAD_BLOCK_SIZE_IN_SAMPLES);
