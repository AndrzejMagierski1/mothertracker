#ifndef FILEMANAGER_MTSAMPLESCOPYIER_H_
#define FILEMANAGER_MTSAMPLESCOPYIER_H_
#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"

class SamplesCopyier
{

public:
	uint8_t start(char* destPatch, char* sourcePatch);
	void stop();
	void update();
	uint8_t getState();
	uint8_t getProgress();
	uint8_t getCurrentStartIndex();
private:
	FsFile sourceSample;
	FsFile destSample;
	uint32_t currentCopyingSize = 0;
	uint32_t copyFileSize;
	uint8_t importSampleState = copyingSampleEnded;
	int8_t currentStartIndex = -1;
	enum importSampleStateType
	{
		copyingSampleEnded,
		copyingSampleInProgress,
	};
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];

};


#endif /* FILEMANAGER_MTSAMPLESCOPYIER_H_ */
