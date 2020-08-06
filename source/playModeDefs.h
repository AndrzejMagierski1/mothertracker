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

				4, 3, 2, 1 };

struct strPlayOrderWithEffects
{
	uint8_t stepNumberToPlay = 0;

	uint8_t fxOption = 0;

};

enum playOrderFxOptions
{
	playModeFx_none,
	playModeFx_rollFlat_1_4_P30,
	playModeFx_rollFlat_1_4_P60,
	playModeFx_rollFlat_1_4_P100,

	playModeFx_rollIncVelo_1_4_P30,
	playModeFx_rollIncVelo_1_4_P60,
	playModeFx_rollIncVelo_1_4_P100,

	playModeFx_rollDecVelo_1_4_P30,
	playModeFx_rollDecVelo_1_4_P60,
	playModeFx_rollDecVelo_1_4_P100,

	playModeFx_rollIncVelo_1_2_P30,
	playModeFx_rollIncVelo_1_2_P60,
	playModeFx_rollIncVelo_1_2_P100,

	playModeFx_rollIncVelo_1_1_P30,
	playModeFx_rollIncVelo_1_1_P60,
	playModeFx_rollIncVelo_1_1_P100

};
const uint16_t playOrderWithFxLength = 128;

const strPlayOrderWithEffects playOrderWithFx[playOrderWithFxLength] =
		{
				{ 1, playModeFx_rollFlat_1_4_P60 },
				{ 2, playModeFx_none },
				{ 3, playModeFx_none },
				{ 4, playModeFx_none },
				{ 4, playModeFx_none },
				{ 3, playModeFx_none },
				{ 2, playModeFx_none },
				{ 1, playModeFx_none },
				{ 9, playModeFx_none },
				{ 10, playModeFx_none },
				{ 11, playModeFx_none },
				{ 12, playModeFx_none },
				{ 12, playModeFx_none },
				{ 11, playModeFx_none },
				{ 10, playModeFx_none },
				{ 9, playModeFx_none },
				{ 17, playModeFx_none },
				{ 18, playModeFx_none },
				{ 19, playModeFx_none },
				{ 20, playModeFx_none },
				{ 20, playModeFx_none },
				{ 19, playModeFx_none },
				{ 2, playModeFx_rollFlat_1_4_P60 },
				{ 17, playModeFx_none },
				{ 25, playModeFx_none },
				{ 26, playModeFx_none },
				{ 27, playModeFx_none },
				{ 28, playModeFx_none },
				{ 28, playModeFx_none },
				{ 2, playModeFx_none },
				{ 26, playModeFx_none },
				{ 25, playModeFx_none },
				{ 33, playModeFx_none },
				{ 34, playModeFx_none },
				{ 35, playModeFx_none },
				{ 36, playModeFx_none },
				{ 36, playModeFx_none },
				{ 35, playModeFx_none },
				{ 34, playModeFx_none },
				{ 33, playModeFx_none },
				{ 41, playModeFx_none },
				{ 1, playModeFx_rollIncVelo_1_4_P100 },
				{ 43, playModeFx_none },
				{ 44, playModeFx_none },
				{ 45, playModeFx_none },
				{ 46, playModeFx_none },
				{ 47, playModeFx_none },
				{ 48, playModeFx_none },
				{ 49, playModeFx_none },
				{ 50, playModeFx_none },
				{ 51, playModeFx_none },
				{ 52, playModeFx_none },
				{ 53, playModeFx_none },
				{ 54, playModeFx_none },
				{ 55, playModeFx_none },
				{ 56, playModeFx_none },
				{ 57, playModeFx_none },
				{ 58, playModeFx_none },
				{ 59, playModeFx_none },
				{ 60, playModeFx_none },
				{ 61, playModeFx_none },
				{ 62, playModeFx_none },
				{ 63, playModeFx_none },
				{ 64, playModeFx_none },
				{ 65, playModeFx_none },
				{ 66, playModeFx_none },
				{ 67, playModeFx_none },
				{ 68, playModeFx_none },
				{ 2, playModeFx_none },
				{ 70, playModeFx_none },
				{ 71, playModeFx_none },
				{ 72, playModeFx_none },
				{ 73, playModeFx_none },
				{ 74, playModeFx_none },
				{ 75, playModeFx_none },
				{ 76, playModeFx_none },
				{ 77, playModeFx_none },
				{ 78, playModeFx_none },
				{ 79, playModeFx_none },
				{ 80, playModeFx_none },
				{ 81, playModeFx_none },
				{ 2, playModeFx_rollIncVelo_1_2_P30 },
				{ 83, playModeFx_none },
				{ 84, playModeFx_none },
				{ 85, playModeFx_none },
				{ 86, playModeFx_none },
				{ 87, playModeFx_none },
				{ 88, playModeFx_none },
				{ 89, playModeFx_none },
				{ 90, playModeFx_none },
				{ 91, playModeFx_none },
				{ 92, playModeFx_none },
				{ 93, playModeFx_none },
				{ 94, playModeFx_none },
				{ 95, playModeFx_none },
				{ 96, playModeFx_none },
				{ 97, playModeFx_none },
				{ 98, playModeFx_none },
				{ 99, playModeFx_none },
				{ 2, playModeFx_none },
				{ 101, playModeFx_none },
				{ 102, playModeFx_none },
				{ 103, playModeFx_none },
				{ 104, playModeFx_none },
				{ 105, playModeFx_none },
				{ 2, playModeFx_none },
				{ 107, playModeFx_none },
				{ 108, playModeFx_none },
				{ 109, playModeFx_none },
				{ 1, playModeFx_none },
				{ 111, playModeFx_none },
				{ 112, playModeFx_none },
				{ 113, playModeFx_none },
				{ 1, playModeFx_none },
				{ 115, playModeFx_none },
				{ 116, playModeFx_none },
				{ 117, playModeFx_none },
				{ 1, playModeFx_rollIncVelo_1_1_P60 },
				{ 119, playModeFx_none },
				{ 120, playModeFx_none },
				{ 121, playModeFx_none },
				{ 1, playModeFx_none },
				{ 1, playModeFx_none },
				{ 1, playModeFx_none },
				{ 125, playModeFx_none },
				{ 126, playModeFx_none },
				{ 1, playModeFx_rollDecVelo_1_4_P100 },
				{ 128, playModeFx_none },

		};

#endif
