#include "mtSamplesCopyier.h"
#include "mtWaveLoader.h"

uint8_t SamplesCopyier::start(char* destPatch, char* sourcePatch)
{

	if(!SD.exists(sourcePatch)) return 0;
	uint32_t localFileSize = 2 * externalWaveLoader.getInfoAboutWave(sourcePatch);
	if(localFileSize == 0) return 0;


	if(SD.exists(destPatch)) SD.remove(destPatch);

	sourceSample = SD.open(sourcePatch);
	if(!sourceSample)
	{
		sourceSample.close();
		return 0;
	}
	strWavFileHeader importFileHeader;
	readHeader(&importFileHeader,&sourceSample);

	copyFileSize = importFileHeader.subchunk2Size;
	sourceSample.close();

	sourceSample = SD.open(sourcePatch);

	destSample= SD.open(destPatch,FILE_WRITE);
	currentCopyingSize = 0;
	importSampleState =copyingSampleInProgress;

	return 1;
}

void SamplesCopyier::update()
{
	uint16_t lengthData=0;
	uint8_t currentBuffor[1024];

	if(importSampleState == copyingSampleEnded) return;
	if(sourceSample.available())
	{
		lengthData=sourceSample.read(currentBuffor,1024);
		destSample.write(currentBuffor,(size_t)lengthData);
		currentCopyingSize+=lengthData;
	}
	else
	{
		stop();
	}
}

void SamplesCopyier::stop()
{
	importSampleState = copyingSampleEnded;
	sourceSample.close();
	destSample.close();
}

uint8_t SamplesCopyier::getState()
{
	return importSampleState;
}

uint8_t SamplesCopyier::getProgress()
{
	return (currentCopyingSize * 100) / copyFileSize;
}
uint8_t SamplesCopyier::getCurrentStartIndex()
{
	return currentStartIndex;
}

