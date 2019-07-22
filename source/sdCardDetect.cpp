#include "sdCardDetect.h"

SDCardDetector sdCardDetector;

void SDCardDetector::begin()
{
	pinMode(SD_DETECT_PIN,INPUT_PULLUP);
}
void SDCardDetector::update()
{
	currentState = digitalRead(SD_DETECT_PIN);
	if((currentState == 0) && (currentState != lastState))
	{
		SD.begin(SdioConfig(DMA_SDIO));
		if(onDetectSD != nullptr) onDetectSD();
	}
	lastState = currentState;
}
void SDCardDetector::setOnDetectFunction(void (*funct)(void))
{
	onDetectSD = funct;
}
