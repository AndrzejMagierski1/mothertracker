
#include "AnalogInputs.h"
#include "mtDisplay.h"


void onPadPress(uint8_t n, int8_t x, int8_t y, uint8_t velo);
void onPadChange(uint8_t n, int8_t x, int8_t y, uint8_t f);
void onPadRelease(uint8_t n);
void onPotChange(uint8_t n, int16_t value);
void onButtonChange(uint8_t n, uint8_t value);



void initHardware()
{
	Serial.begin(9600);
//....................................................
	AnalogInputs.setPadPressFunc(onPadPress);
	AnalogInputs.setPadChangeFunc(onPadChange);
	AnalogInputs.setPadReleaseFunc(onPadRelease);
	AnalogInputs.setPotChangeFunc(onPotChange);
	AnalogInputs.setButtonChangeFunc(onButtonChange);

	AnalogInputs.testMode(0); // (1 = on; 0 = off) test mode
	AnalogInputs.setPadxMode(0);
	AnalogInputs.setPadyMode(0);
	AnalogInputs.setPotDeathZone(4);

/*
	AnalogInputs.setPotResolution(0, 100);
	AnalogInputs.setPotResolution(1, 100);
	AnalogInputs.setPotResolution(2, 100);
	AnalogInputs.setPotResolution(3, 100);
	AnalogInputs.setPotResolution(4, 100);
*/
	AnalogInputs.begin();

//....................................................
	mtDisplay.begin(mtDisplayModePolyLogo);


}



void updateHardware()
{
	AnalogInputs.update();

	mtDisplay.updateDisplay();

}
