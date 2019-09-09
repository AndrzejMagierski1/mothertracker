#include "mtSamplesImporter.h"
#include "mtWaveLoader.h"

uint8_t SamplesImporter::start(char* filePatch, char* name, char* projectPatch, int8_t instrumentIndex, uint8_t type)
{
	char currentPatch[PATCH_SIZE];
	char localName[15];
	currentStartIndex = instrumentIndex;
	strcpy(currentProjectPatch,projectPatch);

	if(filePatch !=  NULL)
	{
		sprintf(currentPatch,"%s/%s",filePatch,name);
	}
	else
	{
		strcpy(currentPatch,name);
	}
	if(!SD.exists(currentPatch)) return 0;
	uint32_t localFileSize = 2 * externalWaveLoader.getInfoAboutWave(currentPatch);
	uint32_t locaUsedMemory;
	if(localFileSize == 0) return 0;

	if(mtProject.instrument[instrumentIndex].sample.loaded)
	{
		locaUsedMemory=mtProject.used_memory - 2* mtProject.instrument[instrumentIndex].sample.length;
	}
	else
	{
		locaUsedMemory=mtProject.used_memory;
	}
	if( (locaUsedMemory + localFileSize) > mtProject.max_memory) return 0;



/*********************************************************SAMPLE****************************************************************/
	strcpy(mtProject.mtProjectRemote.instrumentFile[instrumentIndex].sample.name,name);

	if(instrumentIndex < 10) sprintf(localName,"instr0%d.wav",instrumentIndex);
	else sprintf(localName,"instr%d.wav",instrumentIndex);


	mtProject.mtProjectRemote.instrumentFile[instrumentIndex].sample.type = type;


	sprintf(currentPatch,"%s/samples/%s",currentProjectPatch,localName);
	if(SD.exists(currentPatch)) SD.remove(currentPatch);


	strcpy(mtProject.instrument[instrumentIndex].sample.file_name,name);
	mtProject.instrument[instrumentIndex].sample.type = type;

	if(filePatch!= NULL)
	{
		sprintf(currentPatch,"%s/%s",filePatch,name);
	}
	else
	{
		strcpy(currentPatch,name);
	}



	fileImportSample = SD.open(currentPatch);
	if(!fileImportSample)
	{
		fileImportSample.close();
		return 0;
	}
	strWavFileHeader importFileHeader;
	readHeader(&importFileHeader,&fileImportSample);

	copyFileSize = importFileHeader.subchunk2Size;
	fileImportSample.close();

	fileImportSample = SD.open(currentPatch);

	sprintf(currentPatch,"%s/samples/%s",currentProjectPatch,localName);

	copyImportSample= SD.open(currentPatch,FILE_WRITE);
	currentCopyingSize = 0;
	importSampleState =importingSampleInProgress;

	return 1;
}

void SamplesImporter::update()
{
	uint16_t lengthData=0;
	uint8_t currentBuffor[1024];

	if(importSampleState == importingSampleEnded) return;
	if(fileImportSample.available())
	{
		lengthData=fileImportSample.read(currentBuffor,1024);
		copyImportSample.write(currentBuffor,(size_t)lengthData);
		currentCopyingSize+=lengthData;
	}
	else
	{
		stop();
	}
}

void SamplesImporter::stop()
{
	importSampleState = importingSampleEnded;
	fileImportSample.close();
	copyImportSample.close();
}

uint8_t SamplesImporter::getState()
{
	return importSampleState;
}

uint8_t SamplesImporter::getProgress()
{
	return (currentCopyingSize * 100) / copyFileSize;
}
uint8_t SamplesImporter::getCurrentStartIndex()
{
	return currentStartIndex;
}

