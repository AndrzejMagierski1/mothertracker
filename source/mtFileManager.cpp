#include "mtFileManager.h"

FileManager fileManager;

void FileManager::writeInstrumentFile(char * name, strInstrument * instr)
{
	if(SD.exists(name)) return; //todo: do ustalenia czy nadpisujemy czy nie

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
	if(SD.exists(name)) return; //todo: do ustalenia czy nadpisujemy czy nie

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
		return 0;
		sequencer.loadFromFileERROR();
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
		return 1;
	}
	else return 0;
}

uint8_t FileManager::openProject(char * name)
{
	uint8_t status;

	//SD.open(currentDirectoryOpen); todo: do przerobienia cala funkcja
	status = readProjectFile(name, &mtProject.mtProjectRemote);
	if(!status) return status;

	SD.open("/instruments");
	for(int i=0; i < INSTRUMENTS_COUNT; i++)
	{
		 if(mtProject.mtProjectRemote.instrumentFile[i].index != - 1)
		 {
			 status=readInstrumentFile(mtProject.mtProjectRemote.instrumentFile[i].name,&mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
			 if(!status) return status;
		 }
	}
	SD.open("/patterns");
	for(int i=0; i< PATTERNS_COUNT;i++)
	{
		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
		{
			status=readPatternFile(mtProject.mtProjectRemote.patternFile[i].name);
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

	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		mtProject.mtProjectRemote.instrumentFile[i].index = -1;
	}
	for(uint8_t i=0; i < SAMPLES_COUNT; i++)
	{
		mtProject.mtProjectRemote.sampleFile[i].index = -1;
	}
	for(uint8_t i=0; i < PATTERNS_COUNT; i++)
	{
		mtProject.mtProjectRemote.patternFile[i].index = -1;
	}

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

void FileManager::importSampleToProject(char* filePatch, char* name, int8_t index, uint8_t type)
{
	char currentPatch[PATCH_SIZE];
	FsFile file;
	FsFile copy;
	uint16_t lengthData=0;
	uint8_t currentBuffor[1024];
	uint8_t cnt=0;
	while(mtProject.mtProjectRemote.sampleFile[cnt].index != -1)
	{
		cnt++;
	}

	mtProject.mtProjectRemote.sampleFile[cnt].index=index;
	mtProject.mtProjectRemote.sampleFile[cnt].type=type;
	strcpy(mtProject.mtProjectRemote.sampleFile[cnt].name,name);

	mtProject.sampleBank.sample[index].type=type;
	strcpy(mtProject.sampleBank.sample[index].file_name,name);

	if(filePatch!= NULL)
	{
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
	strcat(currentPatch,"/");
	strcat(currentPatch,"samples");
	strcat(currentPatch,"/");
	strcat(currentPatch,name);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		lengthData=file.read(currentBuffor,1024);
		copy.write(currentBuffor,(size_t)lengthData);
	}
	file.close();
	copy.close();

	memset(currentPatch,0,PATCH_SIZE);
	strcpy(currentPatch,currentProjectPatch);
	strcat(currentPatch,"/project.bin");

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::importInstrumentToProject(char* filePatch, char* name, int8_t index)
{
	char currentPatch[PATCH_SIZE];
	FsFile file;
	uint16_t lengthData=0;
	uint8_t cnt=0;

	while(mtProject.mtProjectRemote.instrumentFile[cnt].index != -1)
	{
		cnt++;
	}
	mtProject.mtProjectRemote.instrumentFile[cnt].index=index;
	strcpy(mtProject.mtProjectRemote.instrumentFile[cnt].name,name);

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
	strcat(currentPatch,name);

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


	while(mtProject.mtProjectRemote.patternFile[cnt].index != -1)
	{
		cnt++;
	}
	mtProject.mtProjectRemote.patternFile[cnt].index=index;
	strcpy(mtProject.mtProjectRemote.patternFile[cnt].name,name);

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
	strcat(currentPatch,name);
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

