#include "mtSliceManager.h"
#include "mtSliceDetector.h"

mtSliceManager sliceManager;

void mtSliceManager::addSlice(strInstrument * instr)
{
	if(instr->sliceNumber == MAX_SLICE_NUMBER) return;
	if(instr->sliceNumber == 0)
	{
		instr->sliceNumber++;
		instr->selectedSlice = 0;
		instr->slices[0] = 0;
		return;
	}
	if(instr->selectedSlice == (instr->sliceNumber - 1) )
	{
		instr->selectedSlice++;
		instr->sliceNumber++;
		instr->slices[instr->selectedSlice] = (MAX_16BIT + instr->slices[instr->selectedSlice - 1]) / 2;
		return;
	}
	for(int8_t i = instr->sliceNumber; i > (instr->selectedSlice+1); i--)
	{
		instr->slices[i] = instr->slices[i-1];
	}
	instr->selectedSlice++;
	instr->slices[instr->selectedSlice] = ((uint32_t)(instr->slices[instr->selectedSlice - 1] + instr->slices[instr->selectedSlice + 1]))/2;


	instr->sliceNumber++;
}
void mtSliceManager::removeSlice(strInstrument * instr)
{
	if((instr->sliceNumber == 0) || (instr->sliceNumber <= instr->selectedSlice)) return;
	if((instr->sliceNumber - 1) == instr->selectedSlice )
	{
		instr->selectedSlice = instr->sliceNumber == 1 ? 0 : instr->selectedSlice - 1 ;
		instr->sliceNumber--;
		return;
	}

	for(uint8_t i = instr->selectedSlice ; i < instr->sliceNumber - 1; i++)
	{
		instr->slices[i] = instr->slices[i+1];
	}
	instr->sliceNumber--;
	if(instr->selectedSlice != 0) instr->selectedSlice--;
	else instr->selectedSlice = 0;
}
void mtSliceManager::adjustSlice(strInstrument * instr, int32_t val)
{
	if((instr->sliceNumber == 0) || (instr->selectedSlice >=  instr->sliceNumber)) return;

	if ((instr->selectedSlice != (instr->sliceNumber - 1)) && (instr->selectedSlice != 0))
	{
		if(instr->slices[instr->selectedSlice] + val >= instr->slices[instr->selectedSlice+1]) instr->slices[instr->selectedSlice] = instr->slices[instr->selectedSlice+1] - 1;
		else if(instr->slices[instr->selectedSlice] + val <= instr->slices[instr->selectedSlice-1]) instr->slices[instr->selectedSlice] = instr->slices[instr->selectedSlice-1] + 1;
		else instr->slices[instr->selectedSlice] += val;
	}
	else if( (instr->selectedSlice == 0) &&  ( instr->selectedSlice == (instr->sliceNumber - 1)) )
	{
		if(instr->slices[instr->selectedSlice] + val > MAX_16BIT) instr->slices[instr->selectedSlice] = MAX_16BIT;
		else if(instr->slices[instr->selectedSlice] + val < 0) instr->slices[instr->selectedSlice] = 0;
		else instr->slices[instr->selectedSlice] += val;
	}
	else if( instr->selectedSlice == instr->sliceNumber - 1)
	{
		if(instr->slices[instr->selectedSlice] + val > MAX_16BIT) instr->slices[instr->selectedSlice] = MAX_16BIT;
		else if(instr->slices[instr->selectedSlice] + val <= instr->slices[instr->selectedSlice-1]) instr->slices[instr->selectedSlice] = instr->slices[instr->selectedSlice-1] + 1;
		else instr->slices[instr->selectedSlice] += val;
	}
	else if( instr->selectedSlice == 0)
	{
		if(instr->slices[instr->selectedSlice] + val >= instr->slices[instr->selectedSlice+1]) instr->slices[instr->selectedSlice] = instr->slices[instr->selectedSlice+1] - 1;
		else if(instr->slices[instr->selectedSlice] + val < 0 ) instr->slices[instr->selectedSlice] = 0;
		else instr->slices[instr->selectedSlice] += val;
	}
}
void mtSliceManager::autoSlice(strInstrument * instr)
{
	memset(instr->slices,0,MAX_SLICE_NUMBER * 2);
	instr->selectedSlice = 0;
	sliceDetector.start(instr->sample.address,instr->sample.length,instr->slices, &instr->sliceNumber);
}

uint8_t mtSliceManager::getAutoSliceProgress()
{
	return sliceDetector.getProgress();
}
uint8_t  mtSliceManager::getAutoSliceState()
{
	return sliceDetector.getState();
}
