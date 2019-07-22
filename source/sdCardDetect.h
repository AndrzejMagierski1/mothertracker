#ifndef SOURCE_SDCARDDETECT_H_
#define SOURCE_SDCARDDETECT_H_

#include "Arduino.h"
#include "SD.h"

constexpr uint8_t SD_DETECT_PIN = 2;

class SDCardDetector
{
public:

	void begin();
	void update();
	void setOnDetectFunction(void (*funct)(void));

private:
	void (*onDetectSD)(void) = nullptr;
	uint8_t currentState = 0;
	uint8_t lastState = 0;

};

extern SDCardDetector sdCardDetector;

#endif /* SOURCE_SDCARDDETECT_H_ */
