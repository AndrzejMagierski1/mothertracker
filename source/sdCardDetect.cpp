
#include "sdCardDetect.h"
#include "SD.h"


SDCardDetector sdCardDetector;

void SDCardDetector::begin()
{
	pinMode(SD_DETECT_PIN,INPUT_PULLUP);
	currentState = digitalRead(SD_DETECT_PIN);
	lastState = 1;
	isInitialized = 0;

	update(1);
}

void SDCardDetector::update(uint8_t forcedUpdate)
{
	if(initCardControl == 0 && (measureTimer > 100 || forcedUpdate))
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
			isInitialized = 0;
			if(onChangeSD != nullptr) onChangeSD(0);
		}
		lastState = currentState;
	}
	else if((initCardControl == 1) && (measureTimer > 100 || forcedUpdate))
	{
		initCardControl = 2;
		measureTimer = 0;
		if(digitalRead(SD_DETECT_PIN) == 0)
		{
			if(SD.begin(SdioConfig(DMA_SDIO)))
			{
				isInitialized = 1;
			}
			else
			{
				lastState = 1;
				initCardControl = 0;
			}
		}
		else initCardControl = 0;
	}
	else if(initCardControl == 2 && (measureTimer > 100 || forcedUpdate))
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

uint8_t SDCardDetector::isCardInitialized()
{
	return isInitialized;
}

