
#ifndef LIBS_DEBUGLOG_H_
#define LIBS_DEBUGLOG_H_

#include <stdint.h>

#include "displayStructs.h"



extern cDisplay display;
typedef cDisplayControl* hControl;


//#########################################################################
//							DEFINICJE
//#########################################################################

enum enDebugLogState
{
	debugLogDisabled,
	debugLogRefresh




};

const uint8_t  fifoSize = 10; //max 21-22
const uint8_t  logLinesMaxStart = 1; //max 21-22
const uint16_t  logLineTimeMax = 5000;

//#########################################################################
//							KLASY
//#########################################################################

class cDebugLog
{
	friend cDisplay;

public:
	//  usable
	void addLine(char text[]);
	void addLine(const char text[]) { addLine((char*)text); }
	void addText(char text[]);
	void addText(const char text[]) { addText((char*)text); }
	void addValue(int value);

	void forceRefresh();

	void setMaxLineCount(uint8_t count);

	// internal
	void update();
	uint8_t toggleState();
	void processLog();

private:
	//void processLog();
	void removeBottLine();


// filo
	uint8_t logTop = 0;
	uint8_t logBott = 0;

	uint8_t logLinesMax = logLinesMaxStart;
	uint8_t logLinesCount;
//

	struct strLogLine
	{
		uint32_t time = 0;
		char* text;
	} logLine[fifoSize];

	strFont* logFont = (strFont*)&fonts[0];


};


extern cDebugLog debugLog;



#endif
