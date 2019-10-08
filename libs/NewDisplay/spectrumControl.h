
#ifndef _SPECTRUM_CONTROL_H_
#define _SPECTRUM_CONTROL_H_


#include "displayControls.h"

struct strTrackerSpectrum
{
	uint8_t spectrumType = 0;
	int8_t upperData[600];
	int8_t lowerData[600];

	uint16_t loadProgress = 0;

	uint32_t width = 0;
};





//--------------------------------------------------------------------
class cSpectrum: public cDisplayControl
{
public:

	cSpectrum(strControlProperties* properties);
	virtual ~cSpectrum();

	virtual uint8_t update();
	virtual uint8_t append(uint32_t address);
	virtual uint8_t memCpy(uint32_t address);

	virtual void setStyle(uint32_t style);
	virtual void setText(char* text);
	virtual void setValue(int value);
	virtual void setColors(uint32_t* colors);
	virtual void setDefaultColors(uint32_t colors[]);
	virtual void setData(void* data);

private:

	strTrackerSpectrum* spectrum;

	uint16_t ramPartSize[5] = {0};
	uint8_t refreshStep;
	void refresh1();
	void refresh2();
	void refresh3();
	void refresh4();
	void refresh5();


};




#endif



