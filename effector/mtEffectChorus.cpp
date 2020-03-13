#include "mtEffectChorus.h"
#include "sdram.h"

mtEffectChorus sampleEditorChorus;

__NOINIT(EXTERNAL_RAM) int16_t chorusDelayBuffer[CHORUS_BUFFER_LENGTH];

//***********VIRTUAL SET PARAMS
void mtEffectChorus::setParamiter(void * ptr, uint8_t n)
{
	if(n > 1) return;

	switch(n)
	{
	case 0:		delay_length = *((int*)ptr)/2;		 break;
	case 1: 	num_chorus = *((int*)ptr);   		 break;
	default: break;
	}


	return;
}
//***********
//***********VIRTUAL PROCESSING
bool mtEffectChorus::startProcess()
{
	length = confirmed.selection.length;

	srcAddr = confirmed.selection.addr;
	dstAddr = processed.selection.addr;

	l_delayline = chorusDelayBuffer;
	l_circ_idx = 0;

	memset(l_delayline,0,delay_length);

	processedSamples = 0;
	state = true;
	return true;
}
int32_t mtEffectChorus::updateProcess()
{
	if(!state) return - 1;

	uint32_t dif = length - processedSamples;
	uint32_t processedBlockLength = dif > 8192 ? 8192 : dif;
	uint32_t processingNumber = processedBlockLength / 128;

	for(int i = 0; i < processingNumber; i++)
	{
		calculate(srcAddr, dstAddr);
		srcAddr+=128;
		dstAddr+=128;
	}

	if(processedBlockLength < 8192)
	{
		processedBlockLength = processingNumber * 128;
		state = 0;
	}

	processedSamples += processedBlockLength;

	if(!state)
	{
		processed.selection.length = processedSamples; //redundancy
		processed.area.length = confirmed.area.length + (processed.selection.length - confirmed.selection.length); //redundancy
	}

	return processedBlockLength;

}
bool mtEffectChorus::getProcessState()
{
	return state;
}
uint32_t mtEffectChorus::getExpectedProcessLength()
{
	return confirmed.selection.length - confirmed.selection.length%128;
}

void mtEffectChorus::calculate(int16_t * src, int16_t* dst) //process 128 samples
{
  constexpr uint16_t PROCESSED_BLOCK_LENGTH = 128;
  int sum;
  int c_idx;
  int16_t localBuf[PROCESSED_BLOCK_LENGTH];

  if(l_delayline == nullptr) return;

  if(num_chorus <= 1)
  {

	  for(int i = 0; i < PROCESSED_BLOCK_LENGTH; i++)
	  {
		l_circ_idx++;
		if(l_circ_idx >= delay_length)
		{
		  l_circ_idx = 0;
		}
		l_delayline[l_circ_idx] = *src++;
	  }

	  src -= PROCESSED_BLOCK_LENGTH;

	  for(int i = 0; i < PROCESSED_BLOCK_LENGTH; i++)
	  {
		  *dst++ = *src++;
	  }

	  return;
  }

  uint32_t tmp = delay_length/(num_chorus - 1) - 1;

  for(int i = 0; i < PROCESSED_BLOCK_LENGTH; i++)
  {
	  l_circ_idx++;

	  if(l_circ_idx >= delay_length)
	  {
		  l_circ_idx = 0;
	  }

	  l_delayline[l_circ_idx] = *src++;
	  sum = 0;
	  c_idx = l_circ_idx;

	  for(int k = 0; k < num_chorus; k++)
	  {
		  sum += l_delayline[c_idx];
		  if(num_chorus > 1) c_idx -= tmp;

		  if(c_idx < 0) c_idx += delay_length;
	  }

	  *(localBuf + i) = sum/num_chorus;
  }

  for(int i=0; i<PROCESSED_BLOCK_LENGTH;i++)
  {
	  *dst++ = *(localBuf+i);
  }
}
