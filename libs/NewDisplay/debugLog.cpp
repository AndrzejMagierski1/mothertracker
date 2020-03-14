

#include "FT812.h"

#include "display.h"

#include "debugLog.h"

#include "mtStructs.h"

#include "SD.h"

#include <cr_section_macros.h>
__NOINIT(EXTERNAL_RAM) char debugLogSdBuffer[10000] {0};
uint16_t debugLogSdBufferUsage;
uint16_t lastDebugLogSdBufferUsage;

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


void cDebugLog::addLine(const char text[])
{
	if(!mtConfig.debug.debugLogState) return;

	if(logBott == logTop)		logLinesCount = 0;
	else if(logBott > logTop) 	logLinesCount = (fifoSize-logBott)+logTop;
	else						logLinesCount = logTop-logBott;

	if(logLinesCount >= logLinesMax)
	{
		removeBottLine();

		if(logBott == logTop)		logLinesCount = 0;
		else if(logBott > logTop) 	logLinesCount = (fifoSize-logBott)+logTop;
		else						logLinesCount = logTop-logBott;

		if(logLinesCount >= logLinesMax) return;
	}

	//uint16_t strLength = strlen(text);
	strncpy(logLine[logTop].text, text, logLineLengthMax-1);
	addLineToSdBuffer(text, strlen(text));

	logLine[logTop].displayed = false;
	logLine[logTop].time = millis();

	logTop++;
	if(logTop >= fifoSize) logTop = 0;

	if(display.isIdle()) display.forceAppedStage();
}

void cDebugLog::addText(const char text[])
{
	if(!mtConfig.debug.debugLogState) return;

	if(logBott == logTop) return;

	uint8_t addIndex = 0;
	if(logTop == 0) addIndex = logLinesMax-1;
	else addIndex = logTop-1;

	uint16_t oldStrLength = strlen(logLine[addIndex].text);
	int16_t addStrLength = logLineLengthMax-oldStrLength;

	if(addStrLength > 0)
	{
		strncat(logLine[addIndex].text, text, addStrLength-1);
		addTextToSdBuffer(text, addStrLength-1);
	}

	logLine[addIndex].displayed = false;

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

	if(logBott > logTop) 	logLinesCount = (fifoSize-logBott)+logTop;
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
		logLine[actualLine].displayed = true;
		API_CMD_TEXT(5, 5+i*(logFont->height+5), logFont->handle, 0, logLine[actualLine].text);
		actualLine++;
		if(actualLine >= logLinesMax) actualLine = 0;
	}

}


void cDebugLog::update()
{
	if(logBott == logTop) return;

	if(logBott > logTop) 	logLinesCount = (fifoSize-logBott)+logTop;
	else					logLinesCount = logTop-logBott;


	uint32_t actualMillis = millis();

	uint8_t last_line = logTop>0 ? logTop-1 : fifoSize-1;

	if(!logLine[last_line].displayed)
	{
		if(display.isIdle()) display.forceAppedStage();
	}

	//for(uint8_t i = 0; i<logLinesCount; i++)
	//{
		if(logLine[logBott].time + logLineTimeMax < actualMillis)
		{
			removeBottLine();

			if(display.isIdle()) display.forceAppedStage();
		}
	//}
		//if(logBott == logTop) return;

}


uint8_t cDebugLog::toggleState()
{
	return mtConfig.debug.debugLogState = !mtConfig.debug.debugLogState;
}


void cDebugLog::removeBottLine()
{
	if(logBott == logTop) return;

	logBott++;
	if(logBott >= fifoSize) logBott = 0;
}



void cDebugLog::setMaxLineCount(uint8_t count)
{
	if(logLinesMax == count) return;
	if(count < 1 || count > fifoSize) return;

	while(1)
	{
		if(logBott > logTop) 	logLinesCount = (fifoSize-logBott)+logTop;
		else					logLinesCount = logTop-logBott;

		if(logLinesCount > count-1) removeBottLine();
		else break;
	}

	logLinesMax = count;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void cDebugLog::addLineToSdBuffer(const char* text, uint8_t length)
{
	strcpy(&debugLogSdBuffer[debugLogSdBufferUsage], text);
	debugLogSdBufferUsage += length;

	debugLogSdBuffer[debugLogSdBufferUsage] = '\n';

	debugLogSdBufferUsage += 1;
	debugLogSdBuffer[debugLogSdBufferUsage] = 0;
}

void cDebugLog::addTextToSdBuffer(const char* text, uint8_t length)
{
	if(debugLogSdBufferUsage > 1)
	{
		strcpy(&debugLogSdBuffer[debugLogSdBufferUsage-2], text);
		debugLogSdBufferUsage += length-1;

		debugLogSdBuffer[debugLogSdBufferUsage] = '\n';

		debugLogSdBufferUsage += 1;
		debugLogSdBuffer[debugLogSdBufferUsage] = 0;
		//debugLogSdBufferUsage += 1;
	}
}



bool cDebugLog::isSdBufferChanged()
{
	if(lastDebugLogSdBufferUsage != debugLogSdBufferUsage)
	{
		//lastDebugLogSdBufferUsage = debugLogSdBufferUsage;
		return true;
	}
	return false;
}

void cDebugLog::saveLogToFileNow(SdFile* file)
{
	file->write(debugLogSdBuffer,debugLogSdBufferUsage);
	clearSdBuffer();
}


void cDebugLog::clearSdBuffer()
{
	debugLogSdBufferUsage = 0;
}
