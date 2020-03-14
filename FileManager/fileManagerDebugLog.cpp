
#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"

#include "debugLog.h"


#include <stdio.h>

#include "fileManager.h"

SdFile logFile;
uint32_t size = 0;

void cFileManager::saveDebugLogToSd()
{

	if(debugLog.isSdBufferChanged())
	{
		if(!SD.exists("debugLog")) SD.mkdir(0, "debugLog");

		uint8_t fileNum = 0;
		do
		{
			fileNum++;
			logFile.close();
			char fileName[50];
			sprintf(fileName, "debugLog/log%d.txt", fileNum);
			logFile.open(fileName, FA_READ | FA_WRITE | FA_OPEN_APPEND);

			size = logFile.size();
		}
		while(size > 1000000);


		//logFile.seek(size);

		debugLog.saveLogToFileNow(&logFile);

		logFile.close();

	}


}
