
#include "sdCardDetect.h"
#include "SD.h"


SDCardDetector sdCardDetector;

void SDCardDetector::begin()
{
	pinMode(SD_DETECT_PIN,INPUT_PULLUP);
	currentState = digitalRead(SD_DETECT_PIN);
	lastState = currentState;
}


void SDCardDetector::update()
{
	if(initCardControl == 0 && measureTimer > 100)
	{
		measureTimer = 0;
		currentState = digitalRead(SD_DETECT_PIN);
		if((currentState == 0) && (lastState == 1))
		{
			initCardControl = 1;
		}
		else if((currentState == 1) && (lastState == 0))
		{
			SD.end();
			if(onChangeSD != nullptr) onChangeSD(0);
		}
		lastState = currentState;
	}
	else if(initCardControl == 1 && measureTimer > 100)
	{
		initCardControl = 2;
		measureTimer = 0;
		if(digitalRead(SD_DETECT_PIN) == 0) SD.begin(SdioConfig(DMA_SDIO));
		else initCardControl = 0;
	}
	else if(initCardControl == 2 && measureTimer > 100)
	{
		initCardControl = 0;
		measureTimer = 0;
		if(onChangeSD != nullptr) onChangeSD(1);
	}

}


void SDCardDetector::setOnDetectFunction(void (*funct)(uint8_t))
{
	onChangeSD = funct;
}

uint8_t SDCardDetector::isCardInserted()
{
	return !currentState;
}

