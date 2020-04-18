
#ifndef INTERFACE_PERFORMANCEMODE_PERFORMANCEFX_H_
#define INTERFACE_PERFORMANCEMODE_PERFORMANCEFX_H_

#include <stdint.h>





struct fxSetup
{
	int16_t min;
	int16_t max;

	const char* alternativeValuesLabels;
	uint8_t alternativeValuesLabelsLength;

	// id fxow zerowanych przez ten fx
	uint8_t blockedFx1 = 0;
	uint8_t blockedFx2 = 0;
};



class cPerformFx
{
public:
	cPerformFx(uint8_t fxId, const char* fxName, void (*fxSetFunct)(uint8_t,int16_t), void (*fxClearFunct)(uint8_t), const fxSetup* fxSetup);
	~cPerformFx() {  }


	uint8_t id;
	const char* name;
	void (*setFunct)(uint8_t,int16_t);
	void (*clearFunct)(uint8_t);
	const fxSetup* setup;

};

























#endif /* INTERFACE_PERFORMANCEMODE_PERFORMANCEFX_H_ */
