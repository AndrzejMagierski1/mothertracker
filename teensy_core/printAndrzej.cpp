#include "Arduino.h"

extern "C" void printAndrzej2000(const char* text);

void printAndrzej2000(const char * c)
{
	Serial.println(c);
}
