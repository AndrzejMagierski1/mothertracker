#ifndef pitch_shift_h_
#define pitch_shift_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"

#define MAX_PITCH 4096


class PitchShift : public AudioStream
{
public:
	PitchShift(): AudioStream(1, inputQueueArray) { }
	virtual void update(void);
	void setDiv(double value);
	volatile double Mult=0;
	volatile double Div=0;
	volatile uint16_t licznik=0;
	volatile uint8_t outfull=0;
	volatile uint8_t onlyPitchUpdate=0;
private:
	audio_block_t *inputQueueArray[1];
	audio_block_t *out=NULL;
	audio_block_t *in=NULL;
	volatile int16_t *pin;
	volatile uint8_t w=1;
	volatile int16_t *pout=NULL;
	volatile uint16_t CountInSampels=0;
	volatile uint8_t DataRest=0;
	volatile double indicator=0;
	volatile uint16_t i=0;
	volatile uint8_t outMustSend=0;
	volatile uint8_t stillRest=0;

};

#endif
