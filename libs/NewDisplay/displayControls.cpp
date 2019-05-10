

#include "FT812.h"
#include <string.h>

#include "displayControls.h"



uint16_t getTextWidth(uint8_t font, char* text)
{
	uint16_t length = 0;

	for(uint8_t i = 0; i < strlen(text); i++)
	{
		length += fonts[font].data[(uint8_t)text[i]-32];
	}


	return length;
}

