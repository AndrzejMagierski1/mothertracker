
#include "Arduino.h"

#include "debugLog.h"

extern "C" void cserial_println(const char* text);
extern "C" void cserial_print(const char* text);
extern "C" void cserial_print_val(int n);

extern "C" void clog_print(const char* text);
extern "C" void clog_println(const char* text);
extern "C" void clog_print_val(int n);



void cserial_print(const char* text)
{
	Serial.print(text);
}


void cserial_println(const char* text)
{
	Serial.println(text);
}


void cserial_print_val(int n)
{
	Serial.print(n);
}




void clog_print(const char* text)
{
	debugLog.addText(text);
}

void clog_println(const char* text)
{
	debugLog.addLine(text);
}

void clog_print_val(int n)
{
	debugLog.addValue(n);
}
