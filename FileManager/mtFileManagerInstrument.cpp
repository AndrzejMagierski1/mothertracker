#include "mtFileManager.h"

uint8_t FileManager::assignSampleToInstrument(char* filePatch, char* name,int8_t instrumentIndex, uint8_t type)
{
	uint8_t status =  samplesImporter.start(filePatch, name, currentProjectPatch, instrumentIndex, type);
	if(!status) return 0;

	if(mtProject.instrument[instrumentIndex].isActive == 0)
	{
		mtProject.mtProjectRemote.instrumentFile[instrumentIndex].isActive = 1;
		mtProject.instrument[instrumentIndex].isActive = 1;


		mtProject.instrument[instrumentIndex].startPoint=0;
		mtProject.instrument[instrumentIndex].loopPoint1=1;
		mtProject.instrument[instrumentIndex].loopPoint2=MAX_16BIT-1;
		mtProject.instrument[instrumentIndex].endPoint=MAX_16BIT;

		mtProject.instrument[instrumentIndex].wavetableCurrentWindow = 0;
		mtProject.instrument[instrumentIndex].playMode = 0;

		mtProject.instrument[instrumentIndex].envelope[envAmp].delay = 0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].attack = 0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].hold = 0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].decay = 0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].sustain = 1.0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].release = 1000;
		mtProject.instrument[instrumentIndex].envelope[envAmp].amount = 1.0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].enable = envelopeOn;

		mtProject.instrument[instrumentIndex].envelope[envFilter].delay = 0;
		mtProject.instrument[instrumentIndex].envelope[envFilter].attack = 3000;
		mtProject.instrument[instrumentIndex].envelope[envFilter].hold = 0;
		mtProject.instrument[instrumentIndex].envelope[envFilter].decay = 0;
		mtProject.instrument[instrumentIndex].envelope[envFilter].sustain = 1.0;
		mtProject.instrument[instrumentIndex].envelope[envFilter].release = 1000;
		mtProject.instrument[instrumentIndex].envelope[envFilter].amount = 1.0;
		mtProject.instrument[instrumentIndex].envelope[envFilter].enable = envelopeOff;

		mtProject.instrument[instrumentIndex].cutOff = 1.0;
		mtProject.instrument[instrumentIndex].filterEnable = filterOff;
		mtProject.instrument[instrumentIndex].filterType = lowPass;
		mtProject.instrument[instrumentIndex].resonance = 0;
		mtProject.instrument[instrumentIndex].panning = 50;
		mtProject.instrument[instrumentIndex].glide = 0;
		mtProject.instrument[instrumentIndex].volume = 100;
		mtProject.instrument[instrumentIndex].tune = 0;
		mtProject.instrument[instrumentIndex].fineTune = 0;

		mtProject.instrument[instrumentIndex].reverbSend = 0;

	}

	char currentPatch[PATCH_SIZE];

	if(instrumentIndex < 10 ) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,instrumentIndex);
	else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,instrumentIndex);
	writeInstrumentFile(currentPatch, &mtProject.instrument[instrumentIndex]);

	sprintf(currentPatch,"%s/project.bin", currentProjectPatch );
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

	return 1;
}

void FileManager::importInstrumentToProject(char* projectPatch,char* name, int8_t index) //todo: to nie dziala ze wzgledu na to ze nie ma sensu robic(na ta chwile nie jest potrzebne, a potem sie wszystko zmieni)
{
	char currentPatch[PATCH_SIZE];
	FsFile file;
	char localName[15];

	if(mtProject.instrument[index].isActive)
	{
		if(index < 10 ) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,index);
		else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,index);

		if(SD.exists(currentPatch)) SD.remove(currentPatch);
	}

	if(projectPatch!= NULL)
	{
		sprintf(currentPatch, "%s/instruments/%s",projectPatch,name);
	}
	else strcpy(currentPatch,name);

	readInstrumentFile(currentPatch,&mtProject.instrument[index]);


	sprintf(mtProject.instrument[index].name,"%d",(index+1));

	mtProject.mtProjectRemote.instrumentFile[index].isActive=1;
	mtProject.instrument[index].isActive=1;

	if(index<10) sprintf(localName,"instr0%d.wav",index);
	else sprintf(localName,"instr%d.wav",index);

	sprintf(currentPatch,"%s/samples/",projectPatch);

	samplesImporter.start(currentPatch,localName,currentProjectPatch,index,mtProject.mtProjectRemote.instrumentFile[index].sampleType);

	if(index < 10 ) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,index);
	else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,index);
	writeInstrumentFile(currentPatch,&mtProject.instrument[index]);


	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::deleteInstrument(int8_t index)
{
	char currentPatch[PATCH_SIZE];
	if(mtProject.mtProjectRemote.instrumentFile[index].isActive == -1)
	{
		if(index < 10 ) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,index);
		else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,index);
		if(SD.exists(currentPatch)) SD.remove(currentPatch);
		deleteSample(index);
		return;
	}


	if(index < 10 ) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,index);
	else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,index);
	if(SD.exists(currentPatch)) SD.remove(currentPatch);


	mtProject.mtProjectRemote.instrumentFile[index].isActive=-1;
	memset(&mtProject.instrument[index],0,sizeof(mtProject.instrument[index]));

	deleteSample(index);
	mtProject.instruments_count--;

	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::deleteSample(int8_t index)
{
	char currentPatch[PATCH_SIZE];

	if(index<10) sprintf(currentPatch,"%s/samples/instr0%d.wav",currentProjectPatch,index);
	else sprintf(currentPatch,"%s/samples/instr%d.wav",currentProjectPatch,index);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	mtProject.mtProjectRemote.instrumentFile[index].sampleType=0;

	mtProject.instrument[index].sample.type=0;
	memset(mtProject.instrument[index].sample.file_name,0,SAMPLE_NAME_SIZE);

	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}
