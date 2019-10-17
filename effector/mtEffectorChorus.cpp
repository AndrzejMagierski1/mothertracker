
#include "mtEffectorChorus.h"


int16_t chorusBuffer[CHORUS_BUF_SIZE];
mtChorus effectorChorus;

uint8_t mtChorus::makeChorus(int d_length, int n_chorus, uint8_t forceEffect)
{
	if(forceEffect == 0)
	{
		if(d_length == last_d_length && n_chorus == last_n_chorus)
		{
			return 0;
		}
	}

	localLength = effector.getLength();
	returnLength = (int32_t) localLength;

	localAddress = effector.getAddress();
	destAddress = effector.previewBuffer;

	l_delayline = NULL;
	delay_length = 0;
	l_circ_idx = 0;

	if( (d_length%AUDIO_BLOCK_SAMPLES) || (d_length > CHORUS_BUF_SIZE) ) return 0;
	if(n_chorus < 1) return 0;

	l_delayline = chorusBuffer;
	delay_length = d_length/2;
	num_chorus = n_chorus;

	memset(chorusBuffer,0,d_length);

	requireProcessing = 1;
	startLength = localLength;
	loadProgress = 0;
	last_d_length = d_length;
	last_n_chorus = n_chorus;

	return 1;
}


void mtChorus::process()
{
	if(requireProcessing == 1 && localLength != 0)
	{
		calculate(localAddress, destAddress);
		localAddress += AUDIO_BLOCK_SAMPLES;
		destAddress += AUDIO_BLOCK_SAMPLES;
		if(localLength > AUDIO_BLOCK_SAMPLES) localLength -= AUDIO_BLOCK_SAMPLES;
		else localLength = 0;

		loadProgress = ((startLength-localLength) * 100) / startLength;

		if(localLength == 0)
		{
			requireProcessing = 0;
			effector.affterEffectLength = returnLength / 2;
		}
	}
}

uint8_t mtChorus::getProgress()
{
	return loadProgress;
}

uint8_t mtChorus::requireProcess()
{
	return requireProcessing;
}

void mtChorus::cancelProcessing()
{
	requireProcessing = 0;
	loadProgress = 0;
}

void mtChorus::calculate(int16_t * sbuf, int16_t * dbuf)
{
  short *bp;
  int sum;
  int c_idx;
  int16_t tempBuf[AUDIO_BLOCK_SAMPLES];

  memcpy(tempBuf,sbuf,2*AUDIO_BLOCK_SAMPLES);

  if(l_delayline == NULL) return;

  if(num_chorus <= 1)
  {

      bp = sbuf;
      for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++)
      {
        l_circ_idx++;
        if(l_circ_idx >= delay_length)
        {
          l_circ_idx = 0;
        }
        l_delayline[l_circ_idx] = *bp++;
      }

      bp-=AUDIO_BLOCK_SAMPLES;
      for(int i=0; i<AUDIO_BLOCK_SAMPLES;i++)
      {
    	  *dbuf++ = *bp++;
      }

      memcpy(sbuf,tempBuf,AUDIO_BLOCK_SAMPLES);
      return;
  }

	  bp = sbuf;
	  uint32_t tmp = delay_length/(num_chorus - 1) - 1;

	  for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++)
	  {
		  l_circ_idx++;

		  if(l_circ_idx >= delay_length)
		  {
			  l_circ_idx = 0;
		  }

		  l_delayline[l_circ_idx] = *bp;
		  sum = 0;
		  c_idx = l_circ_idx;

		  for(int k = 0; k < num_chorus; k++)
		  {
			  sum += l_delayline[c_idx];
			  if(num_chorus > 1) c_idx -= tmp;

			  if(c_idx < 0) c_idx += delay_length;
		  }

		  *bp++ = sum/num_chorus;
	  }
	  bp-=AUDIO_BLOCK_SAMPLES;
      for(int i=0; i<AUDIO_BLOCK_SAMPLES;i++)
      {
    	  *dbuf++ = *bp++;
      }
      //todo: gdyby dzialal wolno mozna optymalizowac

      memcpy(sbuf,tempBuf,2*AUDIO_BLOCK_SAMPLES);
}


