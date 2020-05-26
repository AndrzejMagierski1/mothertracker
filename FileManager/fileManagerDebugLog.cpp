
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
		if(!SD.exists("trackerLog")) SD.mkdir(0, "trackerLog");

		uint8_t fileNum = 0;
		do
		{
			if(fileNum > 200)
			{
				logFile.close();
				debugLog.clearSdBuffer();
				return;
			}

			fileNum++;
			logFile.close();
			char fileName[50];
			sprintf(fileName, "trackerLog/log%d.ptl", fileNum);
			logFile.open(fileName, FA_READ | FA_WRITE | FA_OPEN_APPEND);

			size = logFile.size();
		}
		while(size > 1000000);



		debugLog.saveLogToFileNow(&logFile);

		logFile.close();

	}


}
