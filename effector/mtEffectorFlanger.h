

#ifndef flanger_to_SD_h_
#define flanger_to_SD_h_

#include "mtEffector.h"

#define FLANGE_BUF_SIZE 4096
#define FLANGE_DELAY_PASSTHRU 0

extern int16_t flangerBuf[FLANGE_BUF_SIZE];

#define FLANGER_OFFSET_MAX		255
#define FLANGER_DEPTH_MAX		255
#define FLANGER_DELAYRATE_MAX	100
#define FLANGER_LENGTH_MAX		FLANGE_BUF_SIZE

class mtFlanger
{
public:
	uint8_t makeFlanger(int d_length,int delay_offset,int d_depth,float delay_rate);
	void process();
	uint8_t getProgress();
	uint8_t requireProcess();
	void cancelProcessing();


private:

	int32_t localLength;
	int32_t	returnLength;
	int16_t *localAddress;
	int16_t *destAddress;

	uint8_t requireProcessing;
	uint8_t loadProgress;
	uint32_t startLength;

	short *l_delayline;
	int delay_length;
	short l_circ_idx;
	int delay_depth;
	int delay_offset_idx;
	int   delay_rate_incr;
	unsigned int l_delay_rate_index;

	void calculate(int16_t * sbuf, int16_t * dbuf);
};

extern mtFlanger effectorFlanger;

#endif
