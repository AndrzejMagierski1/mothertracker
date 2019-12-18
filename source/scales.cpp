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



const char* ptrScaleNames[] =
{
		&scaleNames[0][0],
		&scaleNames[1][0],
		&scaleNames[2][0],
		&scaleNames[3][0],
		&scaleNames[4][0],
		&scaleNames[5][0],
		&scaleNames[6][0],
		&scaleNames[7][0],
		&scaleNames[8][0],
		&scaleNames[9][0],
		&scaleNames[10][0],
		&scaleNames[11][0],
		&scaleNames[12][0],
		&scaleNames[13][0],
		&scaleNames[14][0],
		&scaleNames[15][0],
		&scaleNames[16][0],
		&scaleNames[17][0],
		&scaleNames[18][0],
		&scaleNames[19][0],
		&scaleNames[20][0],
		&scaleNames[21][0],
		&scaleNames[22][0],
		&scaleNames[23][0],
		&scaleNames[24][0],
		&scaleNames[25][0],
		&scaleNames[26][0],
		&scaleNames[27][0],
		&scaleNames[28][0],
		&scaleNames[29][0],
		&scaleNames[30][0],
		&scaleNames[31][0],
		&scaleNames[32][0],
		&scaleNames[33][0],
		&scaleNames[34][0],
		&scaleNames[35][0],
		&scaleNames[36][0],
		&scaleNames[37][0],
		&scaleNames[38][0],
		&scaleNames[39][0],
		&scaleNames[40][0],
		&scaleNames[41][0],
		&scaleNames[42][0],
		&scaleNames[43][0],
		&scaleNames[44][0],
		&scaleNames[45][0],
		&scaleNames[46][0],
		&scaleNames[47][0],
		&scaleNames[48][0],
		&scaleNames[49][0],
		&scaleNames[50][0],
		&scaleNames[51][0],
		&scaleNames[52][0],
		&scaleNames[53][0],
		&scaleNames[54][0],
		&scaleNames[55][0],
		&scaleNames[56][0],
		&scaleNames[57][0],

};

