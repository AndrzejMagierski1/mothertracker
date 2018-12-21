

#include <stdint.h>
#include <Arduino.h>

void onPowerButtonChange(uint8_t value);

//-----------------------------------------------------------------
void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t f)
{
	Serial.print("pad press: ");
	Serial.println(n);

}

//-----------------------------------------------------------------
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f)
{
	Serial.print("pad change: ");
	Serial.println(n);
}

//-----------------------------------------------------------------
void onPadRelease(uint8_t n)
{
	Serial.print("pad release: ");
	Serial.println(n);
}

//-----------------------------------------------------------------
void onPotChange(uint8_t n, int16_t value)
{
	Serial.print("pot change: ");
	Serial.print(n);
	Serial.print(" value: ");
	Serial.println(value);
}

//-----------------------------------------------------------------
// 0 - puszczenie
// 1 - wcisniecie
// 2 - hold
// 3 - double click
void onButtonChange(uint8_t n, uint8_t value)
{
	if(n == 5)
	{
		onPowerButtonChange(value);
		return;
	}

	Serial.print("button: ");
	Serial.print(n);
	Serial.print(" value: ");
	Serial.println(value);

}

void onPowerButtonChange(uint8_t value)
{

	Serial.print("power button value: ");
	Serial.println(value);
}



