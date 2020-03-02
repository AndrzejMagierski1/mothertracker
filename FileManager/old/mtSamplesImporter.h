#ifndef FILEMANAGER_MTSAMPLESIMPORTER_H_
#define FILEMANAGER_MTSAMPLESIMPORTER_H_
#include "SD.h"
#include "mtStructs.h"
#include "wavHeaderReader.h"

class SamplesImporter
{

public:
	uint8_t start(char* filePatch, char* name,char* projectPatch,int8_t instrumentIndex, uint8_t type = mtSampleTypeWaveFile);
	void stop();
	void update();
	uint8_t getState();
	uint8_t getProgress();
	uint8_t getCurrentStartIndex();
	void setStartIndex(uint8_t startIndex);
private:
	SdFile fileImportSample;
	SdFile copyImportSample;
	uint32_t currentCopyingSize = 0;
	uint32_t copyFileSize;
	uint8_t importSampleState = importingSampleEnded;
	int8_t currentStartIndex = -1;
	enum importSampleStateType
	{
		importingSampleEnded,
		importingSampleInProgress,
	};
	char currentProjectPatch[PATCH_SIZE-PROJECT_NAME_SIZE];

};


#endif /* FILEMANAGER_MTSAMPLESIMPORTER_H_ */
