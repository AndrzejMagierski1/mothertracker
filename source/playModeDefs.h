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
#endif
