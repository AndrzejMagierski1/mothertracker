

#include "mtStructs.h"


#include <FastCRC.h>
#include <FastCRC_cpu.h>
#include <FastCRC_tables.h>

#include "mtFileManager.h"


FileManager fileManager;

void FileManager::writeInstrumentFile(char * name, strInstrument * instr)
{
	if(SD.exists(name)) SD.remove(name);

	FsFile file;
	FastCRC32 crcCalc;

	strInstrumentFile instrumentFile;


	instrumentFile.instrumentDataAndHeader.instrument = * instr;


	instrumentFile.instrumentDataAndHeader.instrHeader.id_file[0]='I';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_file[1]='D';
	instrumentFile.instrumentDataAndHeader.instrHeader.type = fileTypeInstrument;
	instrumentFile.instrumentDataAndHeader.instrHeader.version[0] = '0';
	instrumentFile.instrumentDataAndHeader.instrHeader.version[1] = '.';
	instrumentFile.instrumentDataAndHeader.instrHeader.version[2] = '0';
	instrumentFile.instrumentDataAndHeader.instrHeader.version[3] = '1';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[0] = 'D';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[1] = 'A';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[2] = 'T';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[3] = 'A';
	instrumentFile.instrumentDataAndHeader.instrHeader.size = sizeof(*instr);

	instrumentFile.crc = crcCalc.crc32((uint8_t *)&instrumentFile.instrumentDataAndHeader,sizeof(instrumentFile.instrumentDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&instrumentFile,sizeof(instrumentFile));
	file.close();


}

void FileManager::writePatternFile(char * name)
{
	if(SD.exists(name)) SD.remove(name); //todo: do ustalenia czy nadpisujemy czy nie

	FsFile file;
	FastCRC32 crcCalc;
	strPatternFile patternFile;
	Sequencer::strPattern * patt;
	patt=(Sequencer::strPattern *)sequencer.getPatternToSaveToFile();

	patternFile.patternDataAndHeader.pattern = * patt;


	patternFile.patternDataAndHeader.patternHeader.id_file[0]='I';
	patternFile.patternDataAndHeader.patternHeader.id_file[1]='D';
	patternFile.patternDataAndHeader.patternHeader.type = fileTypePattern;
	patternFile.patternDataAndHeader.patternHeader.version[0] = '0';
	patternFile.patternDataAndHeader.patternHeader.version[1] = '.';
	patternFile.patternDataAndHeader.patternHeader.version[2] = '0';
	patternFile.patternDataAndHeader.patternHeader.version[3] = '1';
	patternFile.patternDataAndHeader.patternHeader.id_data[0] = 'D';
	patternFile.patternDataAndHeader.patternHeader.id_data[1] = 'A';
	patternFile.patternDataAndHeader.patternHeader.id_data[2] = 'T';
	patternFile.patternDataAndHeader.patternHeader.id_data[3] = 'A';
	patternFile.patternDataAndHeader.patternHeader.size = sizeof(*patt);

	patternFile.crc = crcCalc.crc32((uint8_t *)&patternFile.patternDataAndHeader,sizeof(patternFile.patternDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&patternFile,sizeof(patternFile));
	file.close();

	sequencer.saveToFileDone();
}


void FileManager::writeProjectFile(char * name, strMtProjectRemote * proj)
{
	if(SD.exists(name)) SD.remove(name);

	FsFile file;
	FastCRC32 crcCalc;
	strProjectFile projectFile;

	projectFile.projectDataAndHeader.project = * proj;
	projectFile.projectDataAndHeader.project.values = mtProject.values;

	projectFile.projectDataAndHeader.projectHeader.id_file[0]='I';
	projectFile.projectDataAndHeader.projectHeader.id_file[1]='D';
	projectFile.projectDataAndHeader.projectHeader.type = fileTypeProject;
	projectFile.projectDataAndHeader.projectHeader.version[0] = '0';
	projectFile.projectDataAndHeader.projectHeader.version[1] = '.';
	projectFile.projectDataAndHeader.projectHeader.version[2] = '0';
	projectFile.projectDataAndHeader.projectHeader.version[3] = '1';
	projectFile.projectDataAndHeader.projectHeader.id_data[0] = 'D';
	projectFile.projectDataAndHeader.projectHeader.id_data[1] = 'A';
	projectFile.projectDataAndHeader.projectHeader.id_data[2] = 'T';
	projectFile.projectDataAndHeader.projectHeader.id_data[3] = 'A';
	projectFile.projectDataAndHeader.projectHeader.size = sizeof(*proj);

	projectFile.crc = crcCalc.crc32((uint8_t *)&projectFile.projectDataAndHeader,sizeof(projectFile.projectDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&projectFile,sizeof(projectFile));
	file.close();


}

uint8_t FileManager::readInstrumentFile(char * name, strInstrument * instr)
{
	if(!SD.exists(name)) return 0;
	FsFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strInstrumentFile instrumentFile;

	file=SD.open(name);
	file.read((uint8_t*)&instrumentFile, sizeof(instrumentFile));
	file.close();

	if(instrumentFile.instrumentDataAndHeader.instrHeader.type != fileTypeInstrument) return 0;

	checkCRC=crcCalc.crc32((uint8_t *)&instrumentFile.instrumentDataAndHeader,sizeof(instrumentFile.instrumentDataAndHeader));
	if(checkCRC == instrumentFile.crc)
	{
		*instr=instrumentFile.instrumentDataAndHeader.instrument;

		return 1;
	}
	else return 0;
}

uint8_t FileManager::readPatternFile(char * name)
{
	if(!SD.exists(name))
	{
		sequencer.loadFromFileERROR();
		return 0;
	}
	FsFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;


	strPatternFile patternFile;
	Sequencer::strPattern * patt;
	patt=(Sequencer::strPattern *)sequencer.getPatternToLoadFromFile();
	file=SD.open(name);
	file.read((uint8_t*)&patternFile, sizeof(patternFile));
	file.close();

	if(patternFile.patternDataAndHeader.patternHeader.type != fileTypePattern)
	{
		return 0;
		sequencer.loadFromFileERROR();
	}

	checkCRC=crcCalc.crc32((uint8_t *)&patternFile.patternDataAndHeader,sizeof(patternFile.patternDataAndHeader));
	if(checkCRC == patternFile.crc)
	{
		*patt=patternFile.patternDataAndHeader.pattern;
		sequencer.loadFromFileOK();
		return 1;
	}
	else
	{
		return 0;
		sequencer.loadFromFileERROR();
	}


}

uint8_t FileManager::readProjectFile(char * name, strMtProjectRemote * proj)
{
	if(!SD.exists(name)) return 0;
	FsFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strProjectFile projectFile;

	file=SD.open(name);
	file.read((uint8_t*)&projectFile, sizeof(projectFile));
	file.close();

	if(projectFile.projectDataAndHeader.projectHeader.type != fileTypeProject) return 0;

	checkCRC=crcCalc.crc32((uint8_t *)&projectFile.projectDataAndHeader,sizeof(projectFile.projectDataAndHeader));
	if(checkCRC == projectFile.crc)
	{
		*proj=projectFile.projectDataAndHeader.project;
		mtProject.values=projectFile.projectDataAndHeader.project.values;
		return 1;
	}
	else return 0;
}

uint8_t FileManager::openProject(char * name , uint8_t type)
{
	uint8_t status;
	char currentPatch[PATCH_SIZE];

	if(type == projectTypeExample)
	{
		strcpy(currentPatch,"Templates/");
		strcat(currentPatch,name);

		strcpy(currentProjectPatch,currentPatch);
	}
	else if( type == projectTypeUserMade)
	{
		strcpy(currentPatch,"Projects/");
		strcat(currentPatch,name);

		strcpy(currentProjectPatch,currentPatch);
	}

	strcat(currentPatch,"/project.bin");

	status = readProjectFile(currentPatch, &mtProject.mtProjectRemote);
	if(!status) return status;

	 mtProject.instruments_count=0;
	for(int i=0; i < INSTRUMENTS_COUNT; i++)
	{
		 if(mtProject.mtProjectRemote.instrumentFile[i].index != - 1)
		 {
			 memset(currentPatch,0,PATCH_SIZE);
			 strcpy(currentPatch,currentProjectPatch);
			 strcat(currentPatch,"/instruments/");
			 strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[i].name);

			 status=readInstrumentFile(currentPatch,&mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
			 if(!status) return status;
			 mtProject.instruments_count++;
		 }
	}

	for(int i=0; i< PATTERNS_COUNT;i++)
	{
		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
		{
			memset(currentPatch,0,PATCH_SIZE);
			strcpy(currentPatch,currentProjectPatch);
			strcat(currentPatch,"/patterns/");
			strcat(currentPatch,mtProject.mtProjectRemote.patternFile[i].name);
			status=readPatternFile(currentPatch);
			if(!status) return status;
			else break;
		}
	}

	for(int i=0; i< SAMPLES_COUNT ; i++)
	{
		if(mtProject.mtProjectRemote.sampleFile[i].index != - 1)
		{
			memcpy(mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].file_name,mtProject.mtProjectRemote.sampleFile[i].name, SAMPLE_NAME_SIZE);
			mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].type=mtProject.mtProjectRemote.sampleFile[i].type;
			mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].wavetable_window_size=mtProject.mtProjectRemote.sampleFile[i].wavetable_window_size;
		}
	}

	return status;
}

void FileManager::createNewProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	strcpy(currentProjectPatch,"Projects/");
	strcat(currentProjectPatch,name);

	strcpy(currentProjectName,name);

	if(!SD.exists("Projects")) SD.mkdir("Projects");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(currentProjectPatch);

	strcpy(patchFolder,"Projects/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/instruments");
	SD.mkdir(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Projects/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/patterns");
	SD.mkdir(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Projects/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/samples");
	SD.mkdir(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Projects/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/project.bin");

	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);
}

void FileManager::importSampleToProject(char* filePatch, char* name, char* newName, int8_t sampleIndex,int8_t instrumentIndex, uint8_t type)
{
	char currentPatch[PATCH_SIZE];
	FsFile file;
	FsFile copy;
	uint16_t lengthData=0;
	uint8_t currentBuffor[1024];
	uint8_t cnt=0;

	while( (mtProject.mtProjectRemote.sampleFile[cnt].index != sampleIndex) && (cnt < SAMPLES_COUNT) )
	{
		cnt++;
	}


	if(cnt != SAMPLES_COUNT )
	{
		strcpy(currentPatch,currentProjectPatch);
		strcat(currentPatch,"/samples/");
		strcat(currentPatch,mtProject.mtProjectRemote.sampleFile[cnt].name);
		if(SD.exists(currentPatch)) SD.remove(currentPatch);
		mtProject.mtProjectRemote.sampleFile[cnt].index = -1;
		memset(mtProject.mtProjectRemote.sampleFile[cnt].name,0,SAMPLE_NAME_SIZE);
	}

	cnt=0;


	while( (mtProject.mtProjectRemote.sampleFile[cnt].index != -1) && (cnt < SAMPLES_COUNT) )
	{
		cnt++;
	}
	if(cnt == SAMPLES_COUNT) return;

	mtProject.mtProjectRemote.sampleFile[cnt].index=sampleIndex;
	mtProject.mtProjectRemote.sampleFile[cnt].type=type;
	strcpy(mtProject.mtProjectRemote.sampleFile[cnt].name,name);

	mtProject.sampleBank.sample[sampleIndex].type=type;
	strcpy(mtProject.sampleBank.sample[sampleIndex].file_name,name);

	if(filePatch!= NULL)
	{
		memset(currentPatch,0,PATCH_SIZE);
		strcpy(currentPatch,filePatch);
		strcat(currentPatch,"/");
		strcat(currentPatch,name);
	}
	else
	{
		strcpy(currentPatch,name);
	}
	file = SD.open(currentPatch);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/samples/");
	strcat(currentPatch,newName);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		lengthData=file.read(currentBuffor,1024);
		copy.write(currentBuffor,(size_t)lengthData);
	}
	file.close();
	copy.close();


	if(instrumentIndex != -1)
	{
		if(mtProject.instrument[instrumentIndex].isActive)
		{
			// todo:wywolanie zapytania o nadpisanie?

			while((mtProject.mtProjectRemote.instrumentFile[cnt].index != instrumentIndex) && (cnt < INSTRUMENTS_COUNT) )
			{
				cnt++;
			}
			if(cnt != INSTRUMENTS_COUNT)
			{
				mtProject.mtProjectRemote.instrumentFile[cnt].index=-1;
				memset(mtProject.mtProjectRemote.instrumentFile[cnt].name,0,INSTRUMENT_NAME_SIZE);
			}
		}

		cnt=0;
		while( (mtProject.mtProjectRemote.instrumentFile[cnt].index != -1) && (cnt < INSTRUMENTS_COUNT) )
		{
			cnt++;
		}
		if(cnt == INSTRUMENTS_COUNT) return;



		mtProject.instrument[instrumentIndex].isActive = 1;
		mtProject.mtProjectRemote.instrumentFile[cnt].index = instrumentIndex;

		strcpy(mtProject.mtProjectRemote.instrumentFile[cnt].name,"instrument_00.mti");
		mtProject.mtProjectRemote.instrumentFile[cnt].name[11] = ((instrumentIndex-instrumentIndex%10)/10) + 48;
		mtProject.mtProjectRemote.instrumentFile[cnt].name[12] = instrumentIndex%10 + 48;

		mtProject.instrument[instrumentIndex].isActive=1;
		mtProject.instrument[instrumentIndex].startPoint=0;
		mtProject.instrument[instrumentIndex].loopPoint1=0;
		mtProject.instrument[instrumentIndex].loopPoint2=MAX_16BIT;
		mtProject.instrument[instrumentIndex].endPoint=MAX_16BIT;

		mtProject.instrument[instrumentIndex].wavetableCurrentWindow = 0;
		mtProject.instrument[instrumentIndex].playMode = 1;

		mtProject.instrument[instrumentIndex].envelope[envAmp].delay = 0;
		mtProject.instrument[instrumentIndex].envelope[envAmp].attack = 1000;
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
		mtProject.instrument[instrumentIndex].panning = 0;
		mtProject.instrument[instrumentIndex].glide = 0;
		mtProject.instrument[instrumentIndex].volume = 100;
		mtProject.instrument[instrumentIndex].tune = 0;
		mtProject.instrument[instrumentIndex].fineTune = 0;

		mtProject.instrument[instrumentIndex].reverbSend = 100;

		mtProject.instruments_count++;

		memset(currentPatch,0,PATCH_SIZE);
		strcpy(currentPatch,currentProjectPatch);
		strcat(currentPatch,"/instruments/");
		strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[cnt].name);
		writeInstrumentFile(currentPatch, &mtProject.instrument[instrumentIndex]);
	}



	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::importInstrumentToProject(char* filePatch,char* name, int8_t index)
{
	char currentPatch[PATCH_SIZE];
	FsFile file;
	uint8_t cnt=0;
	if(mtProject.instrument[index].isActive)
	{
		//todo: zapytanie o nadpisanie?
		while((mtProject.mtProjectRemote.instrumentFile[cnt].index != index) && (cnt < INSTRUMENTS_COUNT) )
		{
			cnt++;
		}
		if(cnt != INSTRUMENTS_COUNT)
		{
			mtProject.mtProjectRemote.instrumentFile[cnt].index=-1;
			memset(mtProject.mtProjectRemote.instrumentFile[cnt].name,0,INSTRUMENT_NAME_SIZE);
		}
		cnt=0;
	}

	while((mtProject.mtProjectRemote.instrumentFile[cnt].index != -1) && (cnt < INSTRUMENTS_COUNT) )
	{
		cnt++;
	}
	mtProject.mtProjectRemote.instrumentFile[cnt].index=index;
	strcpy(mtProject.mtProjectRemote.instrumentFile[cnt].name,"instrument_00.mti");
	mtProject.mtProjectRemote.instrumentFile[cnt].name[11] = ((index-index%10)/10) + 48;
	mtProject.mtProjectRemote.instrumentFile[cnt].name[12] = index%10 + 48;

	if(filePatch!= NULL)
	{
	strcpy(currentPatch,filePatch);
	strcat(currentPatch,"/");
	strcat(currentPatch,name);
	}
	else strcpy(currentPatch,name);

	readInstrumentFile(currentPatch,&mtProject.instrument[index]);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/instruments/");
	strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[cnt].name);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	if(index<9) mtProject.instrument[index].name[0] = (index+1)%10 + 48;
	else
	{
		mtProject.instrument[index].name[0] = (((index+1)-(index+1)%10)/10) + 48;
		mtProject.instrument[index].name[1] = (index+1)%10 + 48;
	}
	mtProject.instrument[index].isActive=1;
	mtProject.instruments_count++;
	writeInstrumentFile(currentPatch,&mtProject.instrument[index]);

	writeInstrumentFile(currentPatch,&mtProject.instrument[index]);


	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::importPatternToProject(char* filePatch, char* name, int8_t index)
{
	char currentPatch[PATCH_SIZE];
	FsFile file;
	FsFile copy;
	uint8_t cnt=0;
	uint8_t currentBuf[1024];
	uint16_t lengthData=0;


	while((mtProject.mtProjectRemote.patternFile[cnt].index != -1) && (cnt < PATTERNS_COUNT))
	{
		cnt++;
	}
	mtProject.mtProjectRemote.patternFile[cnt].index=index;
	strcpy(mtProject.mtProjectRemote.patternFile[cnt].name,"pattern_00.mtp");
	mtProject.mtProjectRemote.patternFile[cnt].name[11] = ((index-index%10)/10) + 48;
	mtProject.mtProjectRemote.patternFile[cnt].name[12] = index%10 + 48;

	if(filePatch!= NULL)
	{
		strcpy(currentPatch,filePatch);
		strcat(currentPatch,"/");
		strcat(currentPatch,name);
	}
	else strcpy(currentPatch,name);

	file = SD.open(currentPatch);

	memset(currentPatch,0,PATCH_SIZE);

	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/patterns/");
	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);
	if(SD.exists(currentPatch)) SD.remove(currentPatch);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		lengthData=file.read(currentBuf,1024);
		copy.write(currentBuf,(size_t)lengthData);
	}
	file.close();
	copy.close();

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::importProject(char* sourceProjectPatch,char* name, char* newName)
{
	char currentPatch [PATCH_SIZE];
	createNewProject(name);

	if(!SD.exists("Projects")) SD.mkdir("Projects");

	strcpy(currentProjectPatch,"Projects/");
	strcat(currentProjectPatch,newName);
	strcpy(currentProjectName,newName);

	strcpy(currentPatch,sourceProjectPatch);
	strcat(currentPatch,"/project.bin");
	readProjectFile(currentPatch,&mtProject.mtProjectRemote);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,sourceProjectPatch);
	strcat(currentPatch,"/samples");

	for(uint8_t i=0; i < SAMPLES_COUNT; i++)
	{
		if(mtProject.mtProjectRemote.sampleFile[i].index != -1)
		{
			importSampleToProject(currentPatch,mtProject.mtProjectRemote.sampleFile[i].name,mtProject.mtProjectRemote.sampleFile[i].name ,mtProject.mtProjectRemote.sampleFile[i].index,-1,mtProject.mtProjectRemote.sampleFile[i].type);
		}
	}
	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,sourceProjectPatch);
	strcat(currentPatch,"/instruments");
	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{
			mtProject.instruments_count++;
			importInstrumentToProject(currentPatch,mtProject.mtProjectRemote.instrumentFile[i].name,mtProject.mtProjectRemote.sampleFile[i].index);
		}
	}
	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,sourceProjectPatch);
	strcat(currentPatch,"/patterns");
	for(uint8_t i=0; i < PATTERNS_COUNT; i++)
	{
		if(mtProject.mtProjectRemote.patternFile[i].index != -1)
		{
			mtProject.patterns_count++;
			importInstrumentToProject(currentPatch,mtProject.mtProjectRemote.patternFile[i].name,mtProject.mtProjectRemote.patternFile[i].index);
		}
	}
}

void FileManager::saveAsProject(char* name)
{
	char currentPatch [PATCH_SIZE];
	createNewProject(name);

	if(!SD.exists("Projects")) SD.mkdir("Projects");

	for(uint8_t i=0;i<SAMPLES_COUNT;i++)
	{
		if(mtProject.mtProjectRemote.sampleFile[i].index != -1)
		{
			memcpy(mtProject.mtProjectRemote.sampleFile[i].name,mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].file_name, SAMPLE_NAME_SIZE);
			mtProject.mtProjectRemote.sampleFile[i].type=mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].type;
			mtProject.mtProjectRemote.sampleFile[i].wavetable_window_size=mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].wavetable_window_size;

			memset(currentPatch,0,PATCH_SIZE);
			strcpy(currentPatch,"Projects/");
			strcat(currentPatch,name);

			copySample(currentProjectPatch,mtProject.mtProjectRemote.sampleFile[i].name,currentPatch,mtProject.mtProjectRemote.sampleFile[i].name);
		}
	}
	for(uint8_t i=0;i<INSTRUMENTS_COUNT;i++)
	{

		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{
			memset(currentPatch,0,PATCH_SIZE);
			strcpy(currentPatch,"Projects/");
			strcat(currentPatch,name);
			strcat(currentPatch,"/instruments/");
			strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[i].name);

			writeInstrumentFile(currentPatch, &mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
		}
	}
	for(uint8_t i=0; i< PATTERNS_COUNT; i++)
	{

		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
		{
			memset(currentPatch,0,PATCH_SIZE);
			strcpy(currentPatch,"Projects/");
			strcat(currentPatch,name);
			if(mtProject.mtProjectRemote.patternFile[i].index == currentPattern)
			{
				strcat(currentPatch,"/patterns/");
				strcat(currentPatch,mtProject.mtProjectRemote.patternFile[i].name);
				writePatternFile(currentPatch);

				memset(currentPatch,0,PATCH_SIZE);
				strcpy(currentPatch,"Projects/");
				strcat(currentPatch,name);
			}
			copyPattern(currentProjectPatch, mtProject.mtProjectRemote.patternFile[i].name,currentPatch,mtProject.mtProjectRemote.patternFile[i].name);
		}
	}
	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,"Projects/");
	strcat(currentPatch,name);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);

}

void FileManager::saveProject()
{
	char currentPatch [PATCH_SIZE];


	for(int i=0; i< SAMPLES_COUNT ; i++)
	{
		if(mtProject.mtProjectRemote.sampleFile[i].index != - 1)
		{
			memcpy(mtProject.mtProjectRemote.sampleFile[i].name,mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].file_name, SAMPLE_NAME_SIZE);
			mtProject.mtProjectRemote.sampleFile[i].type=mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].type;
			mtProject.mtProjectRemote.sampleFile[i].wavetable_window_size=mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].wavetable_window_size;
		}
		else
		{
			memset(mtProject.mtProjectRemote.sampleFile[i].name,0,SAMPLE_NAME_SIZE);
		}
	}

	for(uint8_t i=0;i<INSTRUMENTS_COUNT;i++)
	{

		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{
			memset(currentPatch,0,PATCH_SIZE);
			strcpy(currentPatch,currentProjectPatch);
			strcat(currentPatch,"/instruments/");
			strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[i].name);

			writeInstrumentFile(currentPatch, &mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
		}
		else
		{
			memset(mtProject.mtProjectRemote.instrumentFile[i].name,0,INSTRUMENT_NAME_SIZE);
		}
	}

	for(uint8_t i=0; i< PATTERNS_COUNT; i++)
	{

		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
		{
			if(mtProject.mtProjectRemote.patternFile[i].index == currentPattern)
			{
				memset(currentPatch,0,PATCH_SIZE);
				strcpy(currentPatch,currentProjectPatch);
				strcat(currentPatch,"/patterns/");
				strcat(currentPatch,mtProject.mtProjectRemote.patternFile[i].name);

				writePatternFile(currentPatch);
			}
		}
		else
		{
			memset(mtProject.mtProjectRemote.patternFile[i].name,0,PATTERN_NAME_SIZE);
		}

	}
	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);

}


void FileManager::copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName)
{
	FsFile file;
	FsFile copy;
	char currentPatch [PATCH_SIZE];
	uint8_t currentBuffor[1024];
	uint16_t lengthData=0;

	strcpy(currentPatch,srcProjectPatch);
	strcat(currentPatch,"/samples/");
	strcat(currentPatch,srcName);

	if(!SD.exists(currentPatch)) return;
	file = SD.open(currentPatch);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,dstProjectPatch);
	strcat(currentPatch,"/samples/");
	strcat(currentPatch,dstName);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		lengthData=file.read(currentBuffor,1024);
		copy.write(currentBuffor,(size_t)lengthData);
	}
	file.close();
	copy.close();
}

void FileManager::copyPattern(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName)
{
	FsFile file;
	FsFile copy;
	char currentPatch [PATCH_SIZE];
	uint8_t currentBuffor[1024];
	uint16_t lengthData=0;

	strcpy(currentPatch,srcProjectPatch);
	strcat(currentPatch,"/patterns/");
	strcat(currentPatch,srcName);

	if(!SD.exists(currentPatch)) return;
	file = SD.open(currentPatch);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,dstProjectPatch);
	strcat(currentPatch,"/patterns/");
	strcat(currentPatch,dstName);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		lengthData=file.read(currentBuffor,1024);
		copy.write(currentBuffor,(size_t)lengthData);
	}
	file.close();
	copy.close();
}

void FileManager::formatSDCard()
{
	//SD.format();
}
void FileManager::createEmptyTemplateProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	memset(mtProject.mtProjectRemote.song,-1,SONG_MAX);
	mtProject.mtProjectRemote.song[0]=0;
	mtProject.mtProjectRemote.patternFile[0].index=0;
	strcpy(mtProject.mtProjectRemote.patternFile[0].name,"pattern_00.mtp");


	strcpy(currentProjectPatch,"Templates/");
	strcat(currentProjectPatch,name);

	strcpy(currentProjectName,name);

	if(!SD.exists("Projects")) SD.mkdir("Projects");
	if(!SD.exists("Templates")) SD.mkdir("Templates");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(currentProjectPatch);

	strcpy(patchFolder,"Templates/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/instruments");
	SD.mkdir(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Templates/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/patterns");
	SD.mkdir(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Templates/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/samples");
	SD.mkdir(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Templates/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/patterns/");
	strcat(patchFolder,mtProject.mtProjectRemote.patternFile[0].name);

	mtProject.patterns_count++;
	writePatternFile(patchFolder);

	memset(patchFolder,0,PATCH_SIZE);
	strcpy(patchFolder,"Templates/");
	strcat(patchFolder,name);
	strcat(patchFolder,"/project.bin");

	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);

//	addInstrumentToProject (0);
}

void FileManager:: addInstrumentToProject (int8_t index)
{
	char currentPatch[PATCH_SIZE];
	uint8_t cnt=0;

	if(mtProject.instrument[index].isActive)
	{
		//todo: nadpisac?
		while((mtProject.mtProjectRemote.instrumentFile[cnt].index != index) && (cnt < INSTRUMENTS_COUNT) )
		{
			cnt++;
		}
		if(cnt != INSTRUMENTS_COUNT)
		{
			mtProject.mtProjectRemote.instrumentFile[cnt].index=-1;
			memset(mtProject.mtProjectRemote.instrumentFile[cnt].name,0,INSTRUMENT_NAME_SIZE);
		}
		cnt=0;
	}

	while((mtProject.mtProjectRemote.instrumentFile[cnt].index != -1) && (cnt < INSTRUMENTS_COUNT) )
	{
			cnt++;
	}
	mtProject.mtProjectRemote.instrumentFile[cnt].index=index;
	strcpy(mtProject.mtProjectRemote.instrumentFile[cnt].name,"instrument_00.mti");
	mtProject.mtProjectRemote.instrumentFile[cnt].name[11] = ((index-index%10)/10) + 48;
	mtProject.mtProjectRemote.instrumentFile[cnt].name[12] = index%10 + 48;


	if(index<9) mtProject.instrument[index].name[0] = (index+1)%10 + 48;
	else
	{
		mtProject.instrument[index].name[0] = (((index+1)-(index+1)%10)/10) + 48;
		mtProject.instrument[index].name[1] = (index+1)%10 + 48;
	}
	mtProject.instrument[index].isActive=1;
	mtProject.instrument[index].startPoint=0;
	mtProject.instrument[index].loopPoint1=0;
	mtProject.instrument[index].loopPoint2=MAX_16BIT;
	mtProject.instrument[index].endPoint=MAX_16BIT;

	mtProject.instrument[index].wavetableCurrentWindow = 0;
	mtProject.instrument[index].playMode = 1;

	mtProject.instrument[index].envelope[envAmp].delay = 0;
	mtProject.instrument[index].envelope[envAmp].attack = 1000;
	mtProject.instrument[index].envelope[envAmp].hold = 0;
	mtProject.instrument[index].envelope[envAmp].decay = 0;
	mtProject.instrument[index].envelope[envAmp].sustain = 1.0;
	mtProject.instrument[index].envelope[envAmp].release = 1000;
	mtProject.instrument[index].envelope[envAmp].amount = 1.0;
	mtProject.instrument[index].envelope[envAmp].enable = envelopeOn;

	mtProject.instrument[index].envelope[envFilter].delay = 0;
	mtProject.instrument[index].envelope[envFilter].attack = 3000;
	mtProject.instrument[index].envelope[envFilter].hold = 0;
	mtProject.instrument[index].envelope[envFilter].decay = 0;
	mtProject.instrument[index].envelope[envFilter].sustain = 1.0;
	mtProject.instrument[index].envelope[envFilter].release = 1000;
	mtProject.instrument[index].envelope[envFilter].amount = 1.0;
	mtProject.instrument[index].envelope[envFilter].enable = envelopeOff;

	mtProject.instrument[index].cutOff = 1.0;
	mtProject.instrument[index].filterEnable = filterOff;
	mtProject.instrument[index].filterType = lowPass;
	mtProject.instrument[index].resonance = 0;
	mtProject.instrument[index].panning = 0;
	mtProject.instrument[index].glide = 0;
	mtProject.instrument[index].volume = 100;
	mtProject.instrument[index].tune = 0;
	mtProject.instrument[index].fineTune = 0;

	mtProject.instrument[index].reverbSend = 0;

	mtProject.instruments_count++;

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/instruments/");
	strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[cnt].name);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	writeInstrumentFile(currentPatch,&mtProject.instrument[index]);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager:: addPatternToProject (int8_t index)
{
	char currentPatch[PATCH_SIZE];
	uint8_t cnt=0;
	while((mtProject.mtProjectRemote.patternFile[cnt].index != index) && (cnt < PATTERNS_COUNT) )
	{
		cnt++;
	}
	if(cnt != PATTERNS_COUNT)
	{
		mtProject.mtProjectRemote.patternFile[cnt].index=-1;
		memset(mtProject.mtProjectRemote.patternFile[cnt].name,0,PATTERN_NAME_SIZE);
	}
	cnt=0;
	while((mtProject.mtProjectRemote.patternFile[cnt].index != -1) && (cnt < PATTERNS_COUNT) )
	{
		cnt++;
	}
	mtProject.mtProjectRemote.patternFile[cnt].index=index;
	strcpy(mtProject.mtProjectRemote.patternFile[cnt].name,"pattern_00.mti");
	mtProject.mtProjectRemote.patternFile[cnt].name[8] = ((index-index%10)/10) + 48;
	mtProject.mtProjectRemote.patternFile[cnt].name[9] = index%10 + 48;

	mtProject.patterns_count++;

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/patterns/");
	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	writePatternFile(currentPatch);

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::deleteSample(int8_t index)
{
	char currentPatch[PATCH_SIZE];

	uint8_t cnt=0;

	while( (mtProject.mtProjectRemote.sampleFile[cnt].index != index) && (cnt < SAMPLES_COUNT) )
	{
		cnt++;
	}
	if(cnt == SAMPLES_COUNT) return;

	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/samples/");
	strcat(currentPatch,mtProject.mtProjectRemote.sampleFile[cnt].name);
	if(SD.exists(currentPatch)) SD.remove(currentPatch);


	mtProject.mtProjectRemote.sampleFile[cnt].index=-1;
	mtProject.mtProjectRemote.sampleFile[cnt].type=0;
	memset(mtProject.mtProjectRemote.sampleFile[cnt].name,0,SAMPLE_NAME_SIZE);

	mtProject.sampleBank.sample[index].type=0;

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::deleteInstrument(int8_t index)
{
	char currentPatch[PATCH_SIZE];

	uint8_t cnt=0;

	while((mtProject.mtProjectRemote.instrumentFile[cnt].index != index) && (cnt < INSTRUMENTS_COUNT) )
	{
			cnt++;
	}
	if((cnt == INSTRUMENTS_COUNT) && (!mtProject.instrument[index].isActive)) return;

	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/instruments/");
	strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[cnt].name);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);


	mtProject.mtProjectRemote.instrumentFile[cnt].index=-1;
	memset(mtProject.mtProjectRemote.instrumentFile[cnt].name,0,INSTRUMENT_NAME_SIZE);
	memset(&mtProject.instrument[index],0,sizeof(mtProject.instrument[index]));

	mtProject.instruments_count--;

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

}

void FileManager::deletePattern(int8_t index)
{
	char currentPatch[PATCH_SIZE];
	uint8_t cnt=0;

	while((mtProject.mtProjectRemote.patternFile[cnt].index != index) && (cnt < PATTERNS_COUNT) )
	{
		cnt++;
	}
	if(cnt == PATTERNS_COUNT) return;

	for(uint8_t i=0;i<SONG_MAX; i++)
	{
		if(i == index) mtProject.mtProjectRemote.song[i] = -1;
	}


	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/patterns/");
	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	mtProject.mtProjectRemote.patternFile[cnt].index=-1;
	memset(mtProject.mtProjectRemote.patternFile[cnt].name,0,PATTERN_NAME_SIZE);

	mtProject.patterns_count--;

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}
