
#include "AnalogInputs.h"
#include "keyScanner.h"
#include "mtLED.h"

void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo);
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f);
void onPadRelease(uint8_t n);
void onPotChange(uint8_t n, int16_t value);
void onButtonChange(uint8_t n, uint8_t value);

//keyScanner
void onButtonPush			(uint8_t x, uint8_t y);
void onButtonRelease		(uint8_t x, uint8_t y);
void onButtonHold			(uint8_t x, uint8_t y);
void onButtonDouble			(uint8_t x, uint8_t y);

keyScanner seqButtonsA,seqButtonsB,seqButtonsC;
mtLEDs leds;

void IO7326_INT_FUNCT_A() { seqButtonsA.intAction(); }
void IO7326_INT_FUNCT_B() { seqButtonsB.intAction(); }
void IO7326_INT_FUNCT_C() { seqButtonsC.intAction(); }

void initHardware()
{
	Serial.begin(9600);

	AnalogInputs.setPadPressFunc(onPadPress);
	AnalogInputs.setPadChangeFunc(onPadChange);
	AnalogInputs.setPadReleaseFunc(onPadRelease);
	AnalogInputs.setPotChangeFunc(onPotChange);
	AnalogInputs.setButtonChangeFunc(onButtonChange);

	AnalogInputs.testMode(0); // (1 = on; 0 = off) test mode
	AnalogInputs.setPadxMode(0);
	AnalogInputs.setPadyMode(0);
	AnalogInputs.setPotDeathZone(3);

	AnalogInputs.begin();

	////////////////// IO7326 A
	seqButtonsA.setButtonPushFunc(onButtonPush);
	seqButtonsA.setButtonReleaseFunc(onButtonRelease);
	seqButtonsA.setButtonHoldFunc(onButtonHold);
	seqButtonsA.setButtonDoubleFunc(onButtonDouble);
	seqButtonsA.setHoldTime(200);
	seqButtonsA.setDoubleTime(300);
	////////////////// IO7326 B
	seqButtonsB.setButtonPushFunc(onButtonPush);
	seqButtonsB.setButtonReleaseFunc(onButtonRelease);
	seqButtonsB.setButtonHoldFunc(onButtonHold);
	seqButtonsB.setButtonDoubleFunc(onButtonDouble);
	seqButtonsB.setHoldTime(200);
	seqButtonsB.setDoubleTime(300);
	////////////////// IO7326 C
	seqButtonsC.setButtonPushFunc(onButtonPush);
	seqButtonsC.setButtonReleaseFunc(onButtonRelease);
	seqButtonsC.setButtonHoldFunc(onButtonHold);
	seqButtonsC.setButtonDoubleFunc(onButtonDouble);
	seqButtonsC.setHoldTime(200);
	seqButtonsC.setDoubleTime(300);

	////////////////// IO7326 A
	seqButtonsA.begin(IO7326_ADDR1,I2C_SDA,I2C_SCL,GRID_A,IO7326_INT_FUNCT_A);
	////////////////// IO7326 B
	seqButtonsB.begin(IO7326_ADDR3,I2C_SDA,I2C_SCL,GRID_B,IO7326_INT_FUNCT_B);
	////////////////// IO7326 C
	seqButtonsC.begin(IO7326_ADDR2,I2C_SDA,I2C_SCL,GRID_C,IO7326_INT_FUNCT_C);

	leds.begin();
	leds.setAllLEDPWM(leds.ledPWMseq,leds.ledPWMgrid, 0);


}



void updateHardware()
{
	AnalogInputs.update();

	if(Wire2.done())
	{
			// odczyt przyciskow
			if(seqButtonsA.update())
			{
				return;
			}
			if(seqButtonsB.update())
			{
				return;
			}
			if(seqButtonsC.update())
			{
				return;
			}
			leds.updateSeq();
			leds.updateGrid();

	}

}
