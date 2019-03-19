
#ifndef chorus_to_SD_h_
#define chorus_to_SD_h_

#include "mtEffector.h"

#define CHORUS_BUF_SIZE 4096



class mtChorus
{
public:
	int32_t makeChorus(int16_t *delayline,int d_length,int n_chorus);

private:
	short *l_delayline;
	short l_circ_idx;
	int num_chorus;
	int delay_length;

	void calculate(int16_t * sbuf, int16_t * dbuf);

};

extern int16_t chorusBuffer[CHORUS_BUF_SIZE];
extern mtChorus effectorChorus;

#endif
