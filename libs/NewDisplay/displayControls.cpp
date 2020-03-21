

#include "FT812.h"
#include <string.h>

#include "displayControls.h"



uint16_t getTextWidth(strFont* font, const char* text, uint16_t length)
{
	uint16_t width = 0;

	for(uint16_t i = 0; i < length; i++)
	{
		width += font->data[(uint8_t)text[i]];
	}


	return width;
}

