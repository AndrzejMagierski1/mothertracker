#ifndef SOURCE_SDCARDDETECT_H_
#define SOURCE_SDCARDDETECT_H_


#include "elapsedMillis.h"

constexpr uint8_t SD_DETECT_PIN = 2;

class SDCardDetector
{
public:

	void begin();
	void update();
	void setOnDetectFunction(void (*funct)(uint8_t));
	uint8_t isCardInserted();

private:
	void (*onChangeSD)(uint8_t) = nullptr;
	uint8_t currentState = 1;
	uint8_t lastState = 1;
	elapsedMillis measureTimer;
	uint8_t initCardControl = 0;

};

extern SDCardDetector sdCardDetector;

#endif /* SOURCE_SDCARDDETECT_H_ */
