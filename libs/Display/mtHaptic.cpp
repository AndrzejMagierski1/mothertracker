

#include <stdint.h>

#include "mtDisplay.h"


cMtHaptic mtHaptic;

void cMtHaptic::enable()
{
	enabled = 1;
}


void cMtHaptic::disable()
{
	enabled = 0;
}



void cMtHaptic::set(uint16_t time, uint8_t amplitude, uint8_t effect, uint8_t pitch)
{
	params.amplitude = amplitude;
	params.amplitude = effect;
	params.amplitude = time;
	params.amplitude = pitch;
}

void cMtHaptic::start(uint16_t time)
{
	params.time = time;

	timer = 0;
	active = 1;
}

void cMtHaptic::start(uint16_t time, uint8_t amplitude)
{
	params.amplitude = amplitude;
	params.time = time;

	timer = 0;
	active = 1;
}

void cMtHaptic::start(uint16_t time, uint8_t amplitude, uint8_t effect, uint8_t pitch)
{
	params.amplitude = amplitude;
	params.effect = effect;
	params.time = time;
	params.pitch = pitch;

	timer = 0;
	active = 1;
}

void cMtHaptic::stop()
{
	active = 2;
}
