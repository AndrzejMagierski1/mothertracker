

#include "FT812.h"

#include "display.h"

#include "debugLog.h"

#include "mtStructs.h"



cDebugLog debugLog;

//--------------------------------------------------------------------------------
//-------------------------------  DEBUG LOG  ------------------------------------
//--------------------------------------------------------------------------------

void cDebugLog::forceRefresh()
{
	if(!mtConfig.debug.debugLogState) return;

	//uint32_t forceTime = micros();


	API_LIB_AwaitCoProEmpty();

	API_LIB_BeginCoProListNoCheck();
	API_CMD_DLSTART();

	API_CLEAR_COLOR(0x000000);
	API_CLEAR(1,1,1);

	API_VERTEX_FORMAT(0);

	// wczytaj elementy w odwrotnej kolejnosci niz w jakiej byly tworzone
	for(int8_t  i = controlsCount-1; i >= 0; i--)
	{
		hControl p = display.controlsTable[i];
		if(p == nullptr) continue;
		if(p->style & controlStyleShow)
		{
			uint32_t ramAddress = controlsRamStartAddress+(p->ramMapPosition*controlsRamAddressStep);
			p->append(ramAddress);
		}
	}


//--------------------------------------------
	processLog();
//--------------------------------------------


	API_DISPLAY();
	API_CMD_SWAP();

	API_LIB_EndCoProList();
	//API_LIB_AwaitCoProEmpty();

	//Serial.println(micros()-forceTime);

	display.updateStep = 0;


}


void cDebugLog::addLine(char text[])
{
	if(!mtConfig.debug.debugLogState) return;

	if(logBott == logTop)		logLinesCount = 0;
	else if(logBott > logTop) 	logLinesCount = (logLinesMax-logBott)+logTop;
	else						logLinesCount = logTop-logBott;

	if(logLinesCount >= logLinesMax-1)
	{
		removeBottLine();

		if(logBott == logTop)		logLinesCount = 0;
		else if(logBott > logTop) 	logLinesCount = (logLinesMax-logBott)+logTop;
		else						logLinesCount = logTop-logBott;

		if(logLinesCount >= logLinesMax-1) return;
	}

	uint16_t strLength = strlen(text) +1;

	logLine[logTop].text = new char[strLength];
	if(logLine[logTop].text == nullptr) return;

	memcpy(logLine[logTop].text, text, strLength);

	uint32_t actualMillis = millis();
	logLine[logTop].time = actualMillis;

	logTop++;
	if(logTop >= logLinesMax) logTop = 0;

	if(display.isIdle()) display.forceAppedStage();
}

void cDebugLog::addText(char text[])
{
	if(!mtConfig.debug.debugLogState) return;

	if(logBott == logTop)return;

	uint8_t addIndex = 0;
	if(logTop == 0) addIndex = logLinesMax-1;
	else addIndex = logTop-1;

	uint16_t addStrLength = strlen(text) +1;
	uint16_t oldStrLength = strlen(logLine[addIndex].text);

	char* newNext = new char[addStrLength+oldStrLength];
	if(newNext == nullptr) return;

	memcpy(newNext, logLine[addIndex].text, oldStrLength);
	memcpy(newNext+oldStrLength, text, addStrLength);

	delete[] logLine[addIndex].text;

	logLine[addIndex].text = newNext;

	if(display.isIdle()) display.forceAppedStage();
}

void cDebugLog::addValue(int value)
{
	if(!mtConfig.debug.debugLogState) return;
	if(logBott == logTop)return;


	char buf[34];
/*
	uint8_t digit, i;
	uint8_t sign = 0;

	if (value < 0)
	{
		sign = '-';
		value = -value;
	}

	if (value == 0)
	{
		buf[sizeof(buf) - 1] = '0';
		i = sizeof(buf) - 1;
	}
	else
	{
		i = sizeof(buf) - 1;
		while (1)
		{
			digit = value % 10;
			buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
			value /= 10;
			if (value == 0) break;
			i--;
		}
	}
	if (sign) {
		i--;
		buf[i] = '-';
	}

	//return write(buf + i, sizeof(buf) - i);
*/

	sprintf(buf,"%d",value);
	addText(buf);
}

//--------------------------------------------------------------------------------
void cDebugLog::processLog()
{
	if(logBott == logTop) return;

	if(logBott > logTop) 	logLinesCount = (logLinesMax-logBott)+logTop;
	else					logLinesCount = logTop-logBott;


	// no transparency background
	API_BLEND_FUNC(SRC_ALPHA, ZERO);
	API_COLOR(0x000000);
	API_LINE_WIDTH(16);
	API_BEGIN(RECTS);
	API_VERTEX2F(0 , 0);
	API_VERTEX2F(799, logLinesCount*(logFont->height+5)+5);
	API_END();
	API_BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);


	API_COLOR(0xFFFFFF);
	uint8_t actualLine = logBott;

	for(uint8_t i = 0; i<logLinesCount; i++)
	{
		API_CMD_TEXT(5, 5+i*(logFont->height+5), logFont->handle, 0, logLine[actualLine].text);
		actualLine++;
		if(actualLine >= logLinesMax) actualLine = 0;
	}

}


void cDebugLog::update()
{
	if(logBott == logTop) return;

	if(logBott > logTop) 	logLinesCount = (logLinesMax-logBott)+logTop;
	else					logLinesCount = logTop-logBott;


	for(uint8_t i = 0; i<logLinesCount; i++)
	{
		uint32_t actualMillis = millis();

		if(logLine[logBott].time + logLineTimeMax < actualMillis)
		{
			removeBottLine();

			if(display.isIdle()) display.forceAppedStage();
		}

		if(logBott == logTop) return;
	}
}


uint8_t cDebugLog::toggleState()
{
	return mtConfig.debug.debugLogState = !mtConfig.debug.debugLogState;
}


void cDebugLog::removeBottLine()
{
	if(logBott == logTop) return;

	delete[] logLine[logBott].text;

	logBott++;
	if(logBott >= logLinesMax) logBott = 0;
}





//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
