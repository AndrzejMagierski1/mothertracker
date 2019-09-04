#include "scales.h"
#include <stdint.h>

// zwraca 1 jeśli jest w skali
uint8_t isInScale(uint8_t note,		// nuta do sprawdzenia ,
		uint8_t root,		// root note
		uint8_t scale)	// numer skali
{

	if (scale > MAX_SCALE) scale = MAX_SCALE;

	uint8_t noteName = note % 12;
	uint8_t rootName = root % 12;

	if (noteName == rootName)
	{
		return 1;
	}
	else if (noteName > rootName)
	{
		if (scaleDef[scale] & (1 << (noteName - rootName)))
		{
			return 1;
		}
	}
	else
	{
		if (scaleDef[scale] & (1 << ((12 - rootName) + noteName)))
		{
			return 1;
		}
	}
	return 0;
}

