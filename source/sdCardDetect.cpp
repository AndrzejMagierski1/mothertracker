
#include "mtHardware.h"

#include "sdCardDetect.h"
#include "SD.h"

#include "debugLog.h"


SDCardDetector sdCardDetector;


enum cardState
{
	cardEjected,
	cardInserted,
};


void SDCardDetector::begin()
{
	pinMode(SD_DETECT, INPUT_PULLUP);
	lastState = digitalRead(SD_DETECT);
	isInitialized = 0;
	sdCheckTimer = 0;

	update(1);
}

void SDCardDetector::update(uint8_t forcedUpdate)
{
	if(sdCheckTimer < 100 && !forcedUpdate) return;
	sdCheckTimer = 0;

	uint8_t currentState = !digitalRead(SD_DETECT);

	if(lastState == currentState)
	{
		if(isInitialized == 0 && currentState == cardInserted)
		{
			if(SD.init())
			{
				isInitialized = 1;
				if(onChangeSD != nullptr) onChangeSD(1);
			}
			else
			{
				debugLog.addLine("blad inicjalizacji karty sd");
			}
		}
		return;
	}
	else if(currentState == cardEjected)
	{
		lastState = cardEjected;
		isInitialized = 0;
		if(onChangeSD != nullptr) onChangeSD(0);
	}
	else if(currentState == cardInserted)
	{
		lastState = cardInserted;
		isInitialized = 0;
	}
}

void SDCardDetector::setStateFunction(void (*funct)(uint8_t))
{
	onChangeSD = funct;
}

uint8_t SDCardDetector::isCardInserted()
{
	return lastState;
}

uint8_t SDCardDetector::isCardInitialized()
{
	return isInitialized;
}

