

#include "mtStructs.h"
#include "mtConfig.h"

#include <FastCRC.h>
#include <FastCRC_cpu.h>
#include <FastCRC_tables.h>

#include "mtFileManager.h"


FileManager fileManager;

void FileManager::update()
{
	samplesLoader.update();
	samplesImporter.update();

//******************************************************************************************************
// SAMPLES COPYIER	- kopiuje sample  z patcha do patcha
//******************************************************************************************************
	samplesCopyierCurrentState = fileManager.samplesCopyier.getState();
	if( (!samplesCopyierCurrentState ) && (lastCopyierCurrentState) )
	{
		if(saveProjectFlag)
		{
			for(uint8_t i = currentSaveWave; i < INSTRUMENTS_COUNT; i++)
			{

				if(mtProject.instrument[i].isActive == 1)
				{
					char currentPatch[PATCH_SIZE];
					char workspacePatch[PATCH_SIZE];
					sprintf(currentPatch,"%s/samples/instr%02d.wav",currentProjectPatch,i);
					sprintf(workspacePatch,"Workspace/samples/instr%02d.wav",i);

					samplesCopyier.start(currentPatch,workspacePatch);

					currentSaveWave = i+1;

					if(i == (INSTRUMENTS_COUNT - 1))
					{
						saveProjectFlag = 0;
						if(saveAsFlag)
						{
							saveAsFlag = 0;
							openProject(currentProjectName, projectTypeUserMade);
						}
					}
					break;
				}
				if(i == (INSTRUMENTS_COUNT - 1))
				{
					saveProjectFlag = 0;

					if(saveAsFlag)
					{
						saveAsFlag = 0;
						openProject(currentProjectName, projectTypeUserMade);
					}

				}

			}
		}

		if(openWorkspaceCreateFlag)
		{
			for(uint8_t i = currentSaveWave; i < INSTRUMENTS_COUNT; i++)
			{
				if(mtProject.instrument[i].isActive == 1)
				{
					char currentPatch[PATCH_SIZE];
					char workspacePatch[PATCH_SIZE];
					sprintf(currentPatch,"%s/samples/instr%02d.wav",currentProjectPatch,i);
					sprintf(workspacePatch,"Workspace/samples/instr%02d.wav",i);

					samplesCopyier.start(workspacePatch,currentPatch);

					currentSaveWave = i+1;

					if(i == (INSTRUMENTS_COUNT - 1))
					{
						if(openTemplateBasedProjectFlag)
						{
							openWorkspaceCreateFlag = 0;
							openTemplateBasedProjectFlag = 0;
							startSaveAsProject(currentProjectNameOpenTemplate);
						}
						else
						{
							samplesLoader.start(0,(char*)"Workspace");
						}
					}
					break;
				}
				if(i == (INSTRUMENTS_COUNT - 1))
				{
					openWorkspaceCreateFlag = 0;

					if(openTemplateBasedProjectFlag)
					{
						openTemplateBasedProjectFlag = 0;
						startSaveAsProject(currentProjectName);
					}
					else
					{
						samplesLoader.start(0,(char*)"Workspace");
					}

				}
			}
		}
	}

	lastCopyierCurrentState = fileManager.samplesCopyier.getState();

//******************************************************************************************************
// SAMPLES IMPORTER - kopiuje pliki do projektu
//******************************************************************************************************
	currentCopyStatusFlag = fileManager.samplesImporter.getState();


	if( (!currentCopyStatusFlag ) && (lastCopyStatusFlag) )
	{
		endImportSampleFlag = 1;

		if(autoLoadFlag)
		{
			uint8_t startIndex = samplesImporter.getCurrentStartIndex();
			samplesLoader.setFilesToLoad(loadLength);

			if(startIndex != -1) samplesLoader.start(startIndex,currentProjectPatch);

			loadLength=1;

		}
	}

	lastCopyStatusFlag = currentCopyStatusFlag;

}

uint8_t FileManager::getEndImportSampleFlag()
{
	return endImportSampleFlag;
}
void FileManager::clearEndImportSampleFlag()
{
	endImportSampleFlag = 0;
}
void FileManager::setAutoLoadFlag()
{
	if(saveProjectFlag == 0) autoLoadFlag = 1;
}
void FileManager::clearAutoLoadFlag()
{
	autoLoadFlag = 0;
}

void FileManager::setLoadLength(uint8_t filesNum)
{
	loadLength = filesNum;
}

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

uint8_t FileManager::writePatternFile(char * name)
{

	// todo: może do wyjebania jeśli .open załatwi sprawę
	if (SD.exists(name)) SD.remove(name);

	FsFile file;
	FastCRC32 crcCalc;

	uint8_t * sourcePattern;
	sourcePattern = sequencer.getPatternToSaveToFile();

	((Sequencer::strPattern*) sourcePattern)->crc =
			crcCalc.crc32(sourcePattern,
							sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	file = SD.open(name, FILE_WRITE);
	file.write(sourcePattern,
				sizeof(Sequencer::strPattern));
	file.close();

	sequencer.saveToFileDone();
	return 1;
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


//	strPatternFile patternFile;

	uint8_t * patternDest = sequencer.getPatternToLoadFromFile();


	// na końcu struktury jest crc
	file = SD.open(name);
	file.read(patternDest, sizeof(Sequencer::strPattern));
	file.close();

	checkCRC = crcCalc.crc32(
			patternDest, sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	if (checkCRC == (((Sequencer::strPattern *) patternDest)->crc))
	{
		// ok
		sequencer.loadFromFileOK();
		return 1;
	}
	else
	{
		// not ok
		sequencer.loadFromFileERROR();
		return 0;
	}

	return 1;
}




uint8_t FileManager::readProjectFile(char * name, strMtProjectRemote * proj)
{
	if(!SD.exists(name)) return 0;
	FsFile file;
	FastCRC32 crcCalc;
//	uint32_t checkCRC=0;

	strProjectFile projectFile;

	file=SD.open(name);
	file.read((uint8_t*)&projectFile, sizeof(projectFile));
	file.close();

	if(projectFile.projectDataAndHeader.projectHeader.type != fileTypeProject) return 0;

//	checkCRC=crcCalc.crc32((uint8_t *)&projectFile.projectDataAndHeader,sizeof(projectFile.projectDataAndHeader));
//TODO:	if(checkCRC == projectFile.crc) // wylaczone sprawdzanie crc pliku projektu
	if(1)
	{
		*proj=projectFile.projectDataAndHeader.project;
		mtProject.values=projectFile.projectDataAndHeader.project.values;
		return 1;
	}
	else return 0;
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



void FileManager::formatSDCard()
{
	//SD.format();
}



//void FileManager:: addInstrumentToProject (int8_t index)
//{
//	char currentPatch[PATCH_SIZE];
//	uint8_t cnt=0;
//
//	if(mtProject.instrument[index].isActive)
//	{
//		//todo: nadpisac?
//		while((mtProject.mtProjectRemote.instrumentFile[cnt].index != index) && (cnt < INSTRUMENTS_COUNT) )
//		{
//			cnt++;
//		}
//		if(cnt != INSTRUMENTS_COUNT)
//		{
//			mtProject.mtProjectRemote.instrumentFile[cnt].index=-1;
//			memset(mtProject.mtProjectRemote.instrumentFile[cnt].name,0,INSTRUMENT_NAME_SIZE);
//		}
//		cnt=0;
//	}
//
//	while((mtProject.mtProjectRemote.instrumentFile[cnt].index != -1) && (cnt < INSTRUMENTS_COUNT) )
//	{
//			cnt++;
//	}
//	mtProject.mtProjectRemote.instrumentFile[cnt].index=index;
//	strcpy(mtProject.mtProjectRemote.instrumentFile[cnt].name,"instrument_00.mti");
//	mtProject.mtProjectRemote.instrumentFile[cnt].name[11] = ((index-index%10)/10) + 48;
//	mtProject.mtProjectRemote.instrumentFile[cnt].name[12] = index%10 + 48;
//
//
//	if(index<9) mtProject.instrument[index].name[0] = (index+1)%10 + 48;
//	else
//	{
//		mtProject.instrument[index].name[0] = (((index+1)-(index+1)%10)/10) + 48;
//		mtProject.instrument[index].name[1] = (index+1)%10 + 48;
//	}
//	mtProject.instrument[index].isActive=1;
//	mtProject.instrument[index].startPoint=0;
//	mtProject.instrument[index].loopPoint1=0;
//	mtProject.instrument[index].loopPoint2=MAX_16BIT;
//	mtProject.instrument[index].endPoint=MAX_16BIT;
//
//	mtProject.instrument[index].wavetableCurrentWindow = 0;
//	mtProject.instrument[index].playMode = 1;
//
//	mtProject.instrument[index].envelope[envAmp].delay = 0;
//	mtProject.instrument[index].envelope[envAmp].attack = 1000;
//	mtProject.instrument[index].envelope[envAmp].hold = 0;
//	mtProject.instrument[index].envelope[envAmp].decay = 0;
//	mtProject.instrument[index].envelope[envAmp].sustain = 1.0;
//	mtProject.instrument[index].envelope[envAmp].release = 1000;
//	mtProject.instrument[index].envelope[envAmp].amount = 1.0;
//	mtProject.instrument[index].envelope[envAmp].enable = envelopeOn;
//
//	mtProject.instrument[index].envelope[envFilter].delay = 0;
//	mtProject.instrument[index].envelope[envFilter].attack = 3000;
//	mtProject.instrument[index].envelope[envFilter].hold = 0;
//	mtProject.instrument[index].envelope[envFilter].decay = 0;
//	mtProject.instrument[index].envelope[envFilter].sustain = 1.0;
//	mtProject.instrument[index].envelope[envFilter].release = 1000;
//	mtProject.instrument[index].envelope[envFilter].amount = 1.0;
//	mtProject.instrument[index].envelope[envFilter].enable = envelopeOff;
//
//	mtProject.instrument[index].cutOff = 1.0;
//	mtProject.instrument[index].filterEnable = filterOff;
//	mtProject.instrument[index].filterType = lowPass;
//	mtProject.instrument[index].resonance = 0;
//	mtProject.instrument[index].panning = 0;
//	mtProject.instrument[index].glide = 0;
//	mtProject.instrument[index].volume = 100;
//	mtProject.instrument[index].tune = 0;
//	mtProject.instrument[index].fineTune = 0;
//
//	mtProject.instrument[index].reverbSend = 0;
//
//	mtProject.instruments_count++;
//
//	memset(currentPatch,0,PATCH_SIZE);
//	strcpy(currentPatch,currentProjectPatch);
//	strcat(currentPatch,"/instruments/");
//	strcat(currentPatch,mtProject.mtProjectRemote.instrumentFile[cnt].name);
//
//	if(SD.exists(currentPatch)) SD.remove(currentPatch);
//
//	writeInstrumentFile(currentPatch,&mtProject.instrument[index]);
//
//	memset(currentPatch,0,PATCH_SIZE);
//	strcpy(currentPatch,currentProjectPatch);
//	strcat(currentPatch,"/project.bin");
//
//	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
//}


