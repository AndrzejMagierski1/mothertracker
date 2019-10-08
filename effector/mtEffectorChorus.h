
#ifndef chorus_to_SD_h_
#define chorus_to_SD_h_

#include "mtEffector.h"

#define CHORUS_STRENGTH_MAX		8
#define CHORUS_STRENGTH_MIN		0

#define CHORUS_BUF_SIZE 		4096

class mtChorus
{
public:
	uint8_t makeChorus(int d_length, int n_chorus, uint8_t forceEffect);

	uint8_t getProgress();
	uint8_t requireProcess();
	void cancelProcessing();
	void process();

private:
	int last_d_length;
	int last_n_chorus;

	short *l_delayline;
	short l_circ_idx;
	int num_chorus;
	int delay_length;

	uint8_t requireProcessing;
	uint8_t loadProgress;
	uint32_t startLength;

	uint32_t localLength;
	int32_t returnLength;

	int16_t *localAddress;
	int16_t *destAddress;

	void calculate(int16_t * sbuf, int16_t * dbuf);

};

extern int16_t chorusBuffer[CHORUS_BUF_SIZE];
extern mtChorus effectorChorus;

#endif
