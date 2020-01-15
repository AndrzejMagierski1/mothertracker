#ifndef SOURCE_SDCARDDETECT_H_
#define SOURCE_SDCARDDETECT_H_


#include "elapsedMillis.h"

class SDCardDetector
{
public:
	void begin();
	void update(uint8_t forcedUpdate = 0);
	void setStateFunction(void (*funct)(uint8_t));
	uint8_t isCardInserted();
	uint8_t isCardInitialized();

private:
	void (*onChangeSD)(uint8_t) = nullptr;
	uint8_t lastState = 0;
	uint8_t isInitialized = 0;
	elapsedMillis sdCheckTimer;

};

extern SDCardDetector sdCardDetector;

#endif /* SOURCE_SDCARDDETECT_H_ */
