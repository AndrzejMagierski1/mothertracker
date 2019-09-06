#include "mtFileManager.h"


uint8_t FileManager::assignSampleToInstrument(char* filePatch, char* name,int8_t instrumentIndex, uint8_t type)
{
	uint8_t status =  samplesImporter.start(filePatch, name, currentProjectPatch, instrumentIndex, type);
	if(!status) return 0;

	if(mtProject.instrument[instrumentIndex].isActive == 0)
	{
		mtProject.mtProjectRemote.instrumentFile[instrumentIndex].index = instrumentIndex;

		if(instrumentIndex < 10) sprintf(mtProject.mtProjectRemote.instrumentFile[instrumentIndex].name,"instrument_0%d.mti", instrumentIndex);
		else sprintf(mtProject.mtProjectRemote.instrumentFile[instrumentIndex].name,"instrument_%d.mti", instrumentIndex);

		mtProject.instrument[instrumentIndex].isActive=1;
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

		mtProject.instruments_count++;
	}

	char currentPatch[PATCH_SIZE];

	sprintf(currentPatch,"%s/instruments/%s",currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[instrumentIndex].name);
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
		sprintf(currentPatch,"%s/instruments/%s",currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[index].name);
		if(SD.exists(currentPatch)) SD.remove(currentPatch);
	}

	mtProject.mtProjectRemote.instrumentFile[index].index=index;

	if(index < 10) 	sprintf(mtProject.mtProjectRemote.instrumentFile[index].name, "instrument_0%d.mti",index);
	else sprintf(mtProject.mtProjectRemote.instrumentFile[index].name, "instrument_%d.mti",index);

	if(projectPatch!= NULL)
	{
		sprintf(currentPatch, "%s/instruments/%s",projectPatch,name);
	}
	else strcpy(currentPatch,name);

	readInstrumentFile(currentPatch,&mtProject.instrument[index]);


	sprintf(mtProject.instrument[index].name,"%d",(index+1));

	mtProject.instrument[index].isActive=1;
	mtProject.instruments_count++;

	if(index<10) sprintf(localName,"instr0%d.wav",index);
	else sprintf(localName,"instr%d.wav",index);

	sprintf(currentPatch,"%s/samples/",projectPatch);

	samplesImporter.start(currentPatch,localName,currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[index].index ,mtProject.mtProjectRemote.instrumentFile[index].sample.type);


	sprintf(currentPatch,"%s/instruments/%s",currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[index].name);
	writeInstrumentFile(currentPatch,&mtProject.instrument[index]);


	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::deleteInstrument(int8_t index)
{
	char currentPatch[PATCH_SIZE];


//	uint8_t cnt=0;
//
//	while((mtProject.mtProjectRemote.instrumentFile[cnt].index != index) && (cnt < INSTRUMENTS_COUNT) )
//	{
//			cnt++;
//	}
//	if((cnt == INSTRUMENTS_COUNT) && (!mtProject.instrument[index].isActive)) return;
//
//	strcpy(currentPatch,currentProjectPatch);
//	strcat(currentPatch,"/instruments/");
//	strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[cnt].name);
//
//	if(SD.exists(currentPatch)) SD.remove(currentPatch);
//
//
//	mtProject.mtProjectRemote.instrumentFile[cnt].index=-1;
//	memset(mtProject.mtProjectRemote.instrumentFile[cnt].name,0,INSTRUMENT_NAME_SIZE);
//	memset(&mtProject.instrument[index],0,sizeof(mtProject.instrument[index]));
//
//	deleteSample(index);
//	mtProject.instruments_count--;
//
//	memset(currentPatch,0,PATCH_SIZE);
//	strcpy(currentPatch,currentProjectPatch);
//	strcat(currentPatch,"/project.bin");
//
//	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

//********************* tam bylo zalozenie ze w slotach instrumentow moga byc rozne indeksy ***********//

	sprintf(currentPatch,"%s/instruments/%s",currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[index].name);
	if(SD.exists(currentPatch)) SD.remove(currentPatch);


	mtProject.mtProjectRemote.instrumentFile[index].index=-1;
	memset(mtProject.mtProjectRemote.instrumentFile[index].name,0,INSTRUMENT_NAME_SIZE);
	memset(&mtProject.instrument[index],0,sizeof(mtProject.instrument[index]));

	deleteSample(index);
	mtProject.instruments_count--;

	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::deleteSample(int8_t index)
{
	char currentPatch[PATCH_SIZE];
	char number[3];

	number[0] = ((index-index%10)/10) + 48;
	number[1] = index%10 + 48;
	number[2] = 0;

	if(mtProject.instrument[index].sample.loaded) mtProject.samples_count--;

	if(index<10) sprintf(currentPatch,"%s/samples/instr0%d.wav",currentProjectPatch,index);
	else sprintf(currentPatch,"%s/samples/instr%d.wav",currentProjectPatch,index);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	mtProject.mtProjectRemote.instrumentFile[index].sample.type=0;
	memset(mtProject.mtProjectRemote.instrumentFile[index].sample.name,0,SAMPLE_NAME_SIZE);

	mtProject.instrument[index].sample.type=0;
	mtProject.instrument[index].sample.loaded=0;
	memset(mtProject.instrument[index].sample.file_name,0,SAMPLE_NAME_SIZE);

	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}
