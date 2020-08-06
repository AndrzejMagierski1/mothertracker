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
				{ 1, playModeFx_rollFlat_1_4_P60 }, //step 1
				{ 2, playModeFx_none }, //step 2
				{ 3, playModeFx_none }, //step 3
				{ 4, playModeFx_none }, //step 4
				{ 4, playModeFx_none }, //step 5
				{ 3, playModeFx_none }, //step 6
				{ 2, playModeFx_none }, //step 7
				{ 1, playModeFx_none }, //step 8
				{ 9, playModeFx_none }, //step 9
				{ 10, playModeFx_none }, //step 10
				{ 11, playModeFx_none }, //step 11
				{ 12, playModeFx_none }, //step 12
				{ 12, playModeFx_none }, //step 13
				{ 11, playModeFx_none }, //step 14
				{ 10, playModeFx_none }, //step 15
				{ 9, playModeFx_none }, //step 16
				{ 17, playModeFx_none }, //step 17
				{ 18, playModeFx_none }, //step 18
				{ 19, playModeFx_none }, //step 19
				{ 20, playModeFx_none }, //step 20
				{ 20, playModeFx_none }, //step 21
				{ 19, playModeFx_none }, //step 22
				{ 2, playModeFx_rollFlat_1_4_P60 }, //step 23
				{ 17, playModeFx_none }, //step 24
				{ 25, playModeFx_none }, //step 25
				{ 26, playModeFx_none }, //step 26
				{ 27, playModeFx_none }, //step 27
				{ 28, playModeFx_none }, //step 28
				{ 28, playModeFx_none }, //step 29
				{ 2, playModeFx_none }, //step 30
				{ 26, playModeFx_none }, //step 31
				{ 25, playModeFx_none }, //step 32
				{ 33, playModeFx_none }, //step 33
				{ 34, playModeFx_none }, //step 34
				{ 35, playModeFx_none }, //step 35
				{ 36, playModeFx_none }, //step 36
				{ 36, playModeFx_none }, //step 37
				{ 35, playModeFx_none }, //step 38
				{ 34, playModeFx_none }, //step 39
				{ 33, playModeFx_none }, //step 40
				{ 41, playModeFx_none }, //step 41
				{ 1, playModeFx_rollIncVelo_1_4_P100 }, //step 42
				{ 43, playModeFx_none }, //step 43
				{ 44, playModeFx_none }, //step 44
				{ 45, playModeFx_none }, //step 45
				{ 46, playModeFx_none }, //step 46
				{ 47, playModeFx_none }, //step 47
				{ 48, playModeFx_none }, //step 48
				{ 49, playModeFx_none }, //step 49
				{ 50, playModeFx_none }, //step 50
				{ 51, playModeFx_none }, //step 51
				{ 52, playModeFx_none }, //step 52
				{ 53, playModeFx_none }, //step 53
				{ 54, playModeFx_none }, //step 54
				{ 55, playModeFx_none }, //step 55
				{ 56, playModeFx_none }, //step 56
				{ 57, playModeFx_none }, //step 57
				{ 58, playModeFx_none }, //step 58
				{ 59, playModeFx_none }, //step 59
				{ 60, playModeFx_none }, //step 60
				{ 61, playModeFx_none }, //step 61
				{ 62, playModeFx_none }, //step 62
				{ 63, playModeFx_none }, //step 63
				{ 64, playModeFx_none }, //step 64
				{ 65, playModeFx_none }, //step 65
				{ 66, playModeFx_none }, //step 66
				{ 67, playModeFx_none }, //step 67
				{ 68, playModeFx_none }, //step 68
				{ 2, playModeFx_none }, //step 69
				{ 70, playModeFx_none }, //step 70
				{ 71, playModeFx_none }, //step 71
				{ 72, playModeFx_none }, //step 72
				{ 73, playModeFx_none }, //step 73
				{ 74, playModeFx_none }, //step 74
				{ 75, playModeFx_none }, //step 75
				{ 76, playModeFx_none }, //step 76
				{ 77, playModeFx_none }, //step 77
				{ 78, playModeFx_none }, //step 78
				{ 79, playModeFx_none }, //step 79
				{ 80, playModeFx_none }, //step 80
				{ 81, playModeFx_none }, //step 81
				{ 2, playModeFx_rollIncVelo_1_2_P30 }, //step 82
				{ 83, playModeFx_none }, //step 83
				{ 84, playModeFx_none }, //step 84
				{ 85, playModeFx_none }, //step 85
				{ 86, playModeFx_none }, //step 86
				{ 87, playModeFx_none }, //step 87
				{ 88, playModeFx_none }, //step 88
				{ 89, playModeFx_none }, //step 89
				{ 90, playModeFx_none }, //step 90
				{ 91, playModeFx_none }, //step 91
				{ 92, playModeFx_none }, //step 92
				{ 93, playModeFx_none }, //step 93
				{ 94, playModeFx_none }, //step 94
				{ 95, playModeFx_none }, //step 95
				{ 96, playModeFx_none }, //step 96
				{ 97, playModeFx_none }, //step 97
				{ 98, playModeFx_none }, //step 98
				{ 99, playModeFx_none }, //step 99
				{ 2, playModeFx_none }, //step 100
				{ 101, playModeFx_none }, //step 101
				{ 102, playModeFx_none }, //step 102
				{ 103, playModeFx_none }, //step 103
				{ 104, playModeFx_none }, //step 104
				{ 105, playModeFx_none }, //step 105
				{ 2, playModeFx_none }, //step 106
				{ 107, playModeFx_none }, //step 107
				{ 108, playModeFx_none }, //step 108
				{ 109, playModeFx_none }, //step 109
				{ 1, playModeFx_none }, //step 110
				{ 111, playModeFx_none }, //step 111
				{ 112, playModeFx_none }, //step 112
				{ 113, playModeFx_none }, //step 113
				{ 1, playModeFx_none }, //step 114
				{ 115, playModeFx_none }, //step 115
				{ 116, playModeFx_none }, //step 116
				{ 117, playModeFx_none }, //step 117
				{ 1, playModeFx_rollIncVelo_1_1_P60 }, //step 118
				{ 119, playModeFx_none }, //step 119
				{ 120, playModeFx_none }, //step 120
				{ 121, playModeFx_none }, //step 121
				{ 1, playModeFx_none }, //step 122
				{ 1, playModeFx_none }, //step 123
				{ 1, playModeFx_none }, //step 124
				{ 125, playModeFx_none }, //step 125
				{ 126, playModeFx_none }, //step 126
				{ 1, playModeFx_rollDecVelo_1_4_P100 }, //step 127
				{ 128, playModeFx_none }, //step 128

		};

#endif
