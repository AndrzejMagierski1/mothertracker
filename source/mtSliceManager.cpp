#include "mtSliceManager.h"
#include "mtSliceDetector.h"

mtSliceManager sliceManager;

void mtSliceManager::addSlice(strInstrument * instr)
{
	if(instr->sliceNumber == MAX_SLICE_NUMBER) return;
	if(instr->sliceNumber == 0)
	{
		instr->slices[0] = 0;
		instr->sliceNumber++;
	}
	for(int8_t i = instr->sliceNumber; i > 0; i--)
	{
		instr->slices[i] = instr->slices[i-1];
	}
	instr->slices[0] = 0;
	instr->sliceNumber++;
}
void mtSliceManager::removeSlice(strInstrument * instr, uint8_t n)
{
	if((instr->sliceNumber == 0) || (instr->sliceNumber <= n)) return;
	if((instr->sliceNumber - 1) == n )
	{
		instr->sliceNumber--;
		return;
	}

	for(uint8_t i = n ; i < instr->sliceNumber; i++)
	{
		instr->slices[i] = instr->slices[i+1];
	}
	instr->sliceNumber--;
}
void mtSliceManager::adjustSlice(strInstrument * instr, uint8_t n, int32_t val)
{
	if((instr->sliceNumber == 0) || (n >=  instr->sliceNumber)) return;

	if ((n != (instr->sliceNumber - 1)) && (n != 0))
	{
		if(instr->slices[n] + val >= instr->slices[n+1]) instr->slices[n] = instr->slices[n+1] - 1;
		else if(instr->slices[n] + val <= instr->slices[n-1]) instr->slices[n] = instr->slices[n-1] + 1;
		else instr->slices[n] += val;
	}
	else if( (n == 0) &&  ( n = instr->sliceNumber - 1) )
	{
		if(instr->slices[n] + val > MAX_16BIT) instr->slices[n] = MAX_16BIT;
		else if(instr->slices[n] + val < 0) instr->slices[n] = 0;
		else instr->slices[n] += val;
	}
	else if( n == instr->sliceNumber - 1)
	{
		if(instr->slices[n] + val > MAX_16BIT) instr->slices[n] = MAX_16BIT;
		else if(instr->slices[n] + val <= instr->slices[n-1]) instr->slices[n] = instr->slices[n-1] + 1;
		else instr->slices[n] += val;
	}
	else if( n == 0)
	{
		if(instr->slices[n] + val > MAX_16BIT) instr->slices[n] = MAX_16BIT;
		else if(instr->slices[n] + val <= instr->slices[n-1]) instr->slices[n] = instr->slices[n-1] + 1;
		else instr->slices[n] += val;
	}


}
void mtSliceManager::autoSlice(strInstrument * instr)
{
	sliceDetector.start(instr->sample.address,instr->sample.length,instr->slices);
}

void mtSliceManager::getAutoSliceProgress()
{
	sliceDetector.getProgress();
}
void mtSliceManager::getAutoSliceState()
{
	sliceDetector.getState();
}
