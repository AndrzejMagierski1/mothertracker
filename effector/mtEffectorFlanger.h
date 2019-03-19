

#ifndef flanger_to_SD_h_
#define flanger_to_SD_h_

#include "mtEffector.h"

#define FLANGE_BUF_SIZE 4096
#define FLANGE_DELAY_PASSTHRU 0

extern int16_t flangerBuf[FLANGE_BUF_SIZE];

class mtFlanger
{
public:
  int32_t makeFlanger(int d_length,int delay_offset,int d_depth,float delay_rate);

private:
  short *l_delayline;
  int delay_length;
  short l_circ_idx;
  int delay_depth;
  int delay_offset_idx;
  int   delay_rate_incr;
  unsigned int l_delay_rate_index;

  void calculate(int16_t * sbuf, int16_t * dbuf);
};



#endif
