#ifndef _PLAYMODEDEFS_H_
#define _PLAYMODEDEFS_H_

#include <stdint.h>

const uint16_t playOrder1Length = 32;
uint8_t playOrder1[playOrder1Length] =
			  { 1, 2, 3, 4,
				4, 3, 2, 1,

				1, 2, 3, 4,
				4, 3, 2, 1,

				1, 2, 3, 4,
				4, 3, 2, 1,

				1, 2, 3, 4,
				4, 3, 2, 1, };

const uint16_t playOrder2Length = 12;
uint8_t playOrder2[playOrder1Length] =
			  { 1, 1, 3, 4,
				1, 1, 3, 1,

				4, 3, 2, 1};





struct strPlayOrderWithEffects
{
	uint8_t stepNumberToPlay = 0;
	uint8_t fx = sequencer.fx.FX_TYPE_NONE;
	uint8_t fxValue = 50;

	uint8_t fxProbability = 50;
};


const uint8_t playOrderWithFxLength = 4;

const strPlayOrderWithEffects playOrderWithFx[playOrderWithFxLength] =
		{
				{
						0,
						sequencer.fx.FX_TYPE_NONE,
						50,
						50
				},

				{
						1,
						sequencer.fx.FX_TYPE_NONE,
						50,
						50
				},

				{
						2,
						sequencer.fx.FX_TYPE_REVERSE_PLAYBACK,
						1,
						50
				},
				{
						2,
						sequencer.fx.FX_TYPE_ROLL,
						sequencer.fx.ROLL_PERIOD_1_4,
						50
				},

		};









#endif
