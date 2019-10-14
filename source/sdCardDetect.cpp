
#include "sdCardDetect.h"

SDCardDetector sdCardDetector;

void SDCardDetector::begin()
{
	pinMode(SD_DETECT_PIN,INPUT_PULLUP);
	currentState = digitalRead(SD_DETECT_PIN);
	lastState = currentState;
}
void SDCardDetector::update()
{
	if(measureTimer > 100)
	{
		measureTimer = 0;
		currentState = digitalRead(SD_DETECT_PIN);
		if((currentState == 0) && (lastState == 1))
		{
			SD.begin(SdioConfig(DMA_SDIO));

			if(onChangeSD != nullptr) onChangeSD(1);

		}
		else if((currentState == 1) && (lastState == 0))
		{
			if(onChangeSD != nullptr) onChangeSD(0);
		}
		lastState = currentState;
	}

}
void SDCardDetector::setOnDetectFunction(void (*funct)(uint8_t))
{
	onChangeSD = funct;
}
