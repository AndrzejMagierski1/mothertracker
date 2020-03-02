#ifndef FILEMANAGER_MTSAMPLESLOADER_H_
#define FILEMANAGER_MTSAMPLESLOADER_H_

#include "mtWaveLoader.h"
#include "mtWavetableLoader.h"
#include "SD.h"
#include "mtStructs.h"


class SamplesLoader
{
public:
//	uint8_t loadSamplesMemory();

	void update();
	void start(uint8_t startIndex, char * currentProjectPatch, uint8_t firstLoad = 0);
	uint8_t getMemoryUsageChangeFlag();
	void clearMemoryUsageChangeFlag();
	uint8_t getLoadChangeFlag();
	void clearLoadChangeFlag();
	uint8_t getCurrentProgress();
	uint8_t getStateFlag();
	uint8_t getFirstLoadFlag();
	void setFilesToLoad(uint8_t filesNum);
	WaveLoader waveLoader;
	WavetableLoader wavetableLoader;
private:
	uint32_t calcSamplesFolderSize();
	uint8_t currentIndex;
	uint8_t state;
	uint8_t firstLoadFlag;
	uint8_t lastIndex;
	int32_t currentSize;
	uint8_t memoryUsageChange = 0;
	uint8_t loadedFlagChange = 0;
	uint32_t sizeAllFiles;
	uint32_t currentLoadSize = 0;
	uint32_t currentStepLoadSize = 0;
	uint8_t filesToLoad = 0;
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];

};


#endif /* FILEMANAGER_MTSAMPLESLOADER_H_ */
