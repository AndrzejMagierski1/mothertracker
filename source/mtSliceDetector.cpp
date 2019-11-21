#include "mtSliceDetector.h"
#include "sdram.h"

__NOINIT(EXTERNAL_RAM) uint32_t measureBuffer[INSTANT_BUFFER_SIZE];

mtSliceDetector sliceDetector;

void mtSliceDetector::start(int16_t * addr, uint32_t len, int16_t ** sliceTab)
{
	if(len < LOCAL_BUFFER_SIZE) return;
	state = detectState::inProgress;
	position = 0;
	averagePositionHead = 0;
	currentSlice = 0;
	address = addr;
	slice = sliceTab;
	length = len;
}
void mtSliceDetector::update()
{
	if(state == detectState::ended) return;

	int16_t * localAddr = address + position;
	uint32_t dif = length - position;

	uint16_t constrainSize = (dif < INSTANT_BUFFER_SIZE) ? (uint16_t)dif : INSTANT_BUFFER_SIZE;

	for(uint16_t i = 0; i <constrainSize ; i++ )
	{
		measureBuff[i] = localAddr[i] *  localAddr[i];
	}

	instantAverageBuffer[averagePositionHead] = calcInstantAverage(measureBuff);
	if(position > LOCAL_BUFFER_SIZE)
	{
		localAverage = calcLocalAverage(instantAverageBuffer);
		localVariance = calcLocalVariance(instantAverageBuffer, localAverage);
		sensCoeff = ((-0.0000000001564) * localVariance) + 1.5142857;
		Serial.print("sc: :"); Serial.println(sensCoeff);
		if(instantAverageBuffer[averagePositionHead] > sensCoeff * localAverage)
		{
			if(currentSlice != MAX_SLICE_NUMBER)
			{
				slice[currentSlice++] = (int16_t *)(address + position);
				Serial.printf("SLICE: %d, POS: %d\n",currentSlice-1, position);
			}
		}
	}

	averagePositionHead++;
	if(averagePositionHead == LOCAL_BUFFER_SIZE/INSTANT_BUFFER_SIZE ) averagePositionHead = 0;
	position += constrainSize;
	if(position == length) state = detectState::ended;

}
uint8_t mtSliceDetector::getState()
{
	return (uint8_t) state;
}
uint8_t mtSliceDetector::getProgress()
{
	return (position * 100) / length;
}

uint32_t mtSliceDetector::calcInstantAverage(uint32_t * buf)
{
	uint64_t sum = 0;
	for(uint16_t i = 0; i < INSTANT_BUFFER_SIZE; i++)
	{
		sum += buf[i];
	}
	return (sum/INSTANT_BUFFER_SIZE);
}

uint32_t mtSliceDetector::calcLocalAverage(uint32_t * buf)
{
	uint64_t sum = 0;
	for(uint16_t i = 0; i < LOCAL_BUFFER_SIZE/INSTANT_BUFFER_SIZE; i++)
	{
		sum += buf[i];
	}

	return (sum/ (LOCAL_BUFFER_SIZE/INSTANT_BUFFER_SIZE));
}
uint32_t mtSliceDetector::calcLocalVariance(uint32_t * buf, uint32_t average)
{
	uint64_t sum = 0;
	for(uint16_t i = 0; i < LOCAL_BUFFER_SIZE/INSTANT_BUFFER_SIZE; i++)
	{
		int32_t locDif = buf[i] - average;
		sum += (locDif * locDif);
	}
	return (sum/(LOCAL_BUFFER_SIZE/INSTANT_BUFFER_SIZE));
}




