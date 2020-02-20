#include "mtSamplesCopyier.h"
#include "mtWaveLoader.h"
#include "mtCommonBuffer.h"

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

uint16_t SamplesCopyier::update()
{
	uint16_t lengthData=0;
	uint8_t * buffer = getWriteLoadBufferPointer();

	if(importSampleState == copyingSampleEnded) return 0;
	if(sourceSample.available())
	{
		lengthData=sourceSample.read(buffer,COMMON_BUFFER_SIZE);
		destSample.write(buffer,(size_t)lengthData);
		currentCopyingSize+=lengthData;
		return lengthData;
	}
	else
	{
		stop();
		return 0;
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

