

#include "mtStructs.h"
#include "mtConfig.h"

#include <FastCRC.h>
#include <FastCRC_cpu.h>
#include <FastCRC_tables.h>

#include "mtFileManager.h"

#include "debugLog.h"

FileManager fileManager;

/*void FileManager::setPatternBit(uint8_t trackIdx, uint8_t patternIdx, uint8_t step)
{
	//pattBitmask[trackIdx][patternIdx] =;
}*/

elapsedMicros sd_time_test;




void FileManager::update()
{

	samplesLoader.update();
	samplesImporter.update();
	samplesCopyier.update();

	autoSaveWorkspace(0);

//******************************************************************************************************
// SAMPLES IMPORTER - kopiuje pliki do projektu
//******************************************************************************************************
	currentCopyStatusFlag = fileManager.samplesImporter.getState();

	handleLoadPattern();


	if( (!currentCopyStatusFlag ) && (lastCopyStatusFlag) )
	{
		endImportSampleFlag = 1;

		if(autoLoadFlag)
		{
			int8_t startIndex = samplesImporter.getCurrentStartIndex();

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
	autoLoadFlag = 1;
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
	//sd_time_test;
	//Serial.print(" zapis pliku instrumentu ");

	SD.remove(name);

	SdFile file;
	FastCRC32 crcCalc;

	strInstrumentFile instrumentFile;


	instrumentFile.instrumentDataAndHeader.instrument = * instr;


	instrumentFile.instrumentDataAndHeader.instrHeader.id_file[0]='T';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_file[1]='I';
	instrumentFile.instrumentDataAndHeader.instrHeader.type = fileTypeInstrument;
	instrumentFile.instrumentDataAndHeader.instrHeader.fwVersion[0] = FV_VER_1;
	instrumentFile.instrumentDataAndHeader.instrHeader.fwVersion[1] = FV_VER_2;
	instrumentFile.instrumentDataAndHeader.instrHeader.fwVersion[2] = FV_VER_3;
	instrumentFile.instrumentDataAndHeader.instrHeader.fwVersion[3] = FV_VER_1;
	instrumentFile.instrumentDataAndHeader.instrHeader.fileStructureVersion[0] = INSTRUMENT_FILE_VERSION;
	instrumentFile.instrumentDataAndHeader.instrHeader.fileStructureVersion[1] = INSTRUMENT_FILE_VERSION;
	instrumentFile.instrumentDataAndHeader.instrHeader.fileStructureVersion[2] = INSTRUMENT_FILE_VERSION;
	instrumentFile.instrumentDataAndHeader.instrHeader.fileStructureVersion[3] = INSTRUMENT_FILE_VERSION;
	instrumentFile.instrumentDataAndHeader.instrHeader.size = sizeof(*instr);

	instrumentFile.crc = crcCalc.crc32((uint8_t *)&instrumentFile.instrumentDataAndHeader,sizeof(instrumentFile.instrumentDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&instrumentFile,sizeof(instrumentFile));
	file.close();

	//Serial.print(" t: ");
	//Serial.print(sd_time_test);
}

uint8_t FileManager::writePatternFile(char * name, uint8_t *sourcePattern)
{
	//sd_time_test;
	//Serial.print("zapis pliku patternu ");

	SD.remove(name);

	SdFile file;
	FastCRC32 crcCalc;

	sequencer.setPatternHeader((Sequencer::strPattern*)sourcePattern);

	((Sequencer::strPattern*) sourcePattern)->crc =
			crcCalc.crc32(sourcePattern,
							sizeof(Sequencer::strPattern) - sizeof(uint32_t));

	file = SD.open(name, FILE_WRITE);
	file.write(sourcePattern,
				sizeof(Sequencer::strPattern));
	file.close();

	//Serial.print(" t: ");
	//Serial.print(sd_time_test);

	return 1;
}

void FileManager::writeProjectFile(char * name, strMtProject *proj)
{
	//sd_time_test;
	//Serial.println(" zapis pliku projektu ");

	SD.remove(name);

	//Serial.print(" remove: ");
	//Serial.println(sd_time_test);

	//sd_time_test;

	SdFile file;
	FastCRC32 crcCalc;
	strProjectFile projectFile;

	memcpy(&projectFile.projectDataAndHeader.project.song, &proj->song, sizeof(strSong));
	memcpy(&projectFile.projectDataAndHeader.project.values, &proj->values, sizeof(strMtValues));

	projectFile.projectDataAndHeader.projectHeader.id_file[0]='M';
	projectFile.projectDataAndHeader.projectHeader.id_file[1]='T';

	projectFile.projectDataAndHeader.projectHeader.type = fileTypeProject;

	projectFile.projectDataAndHeader.projectHeader.fwVersion[0] = FV_VER_1;
	projectFile.projectDataAndHeader.projectHeader.fwVersion[1] = FV_VER_2;
	projectFile.projectDataAndHeader.projectHeader.fwVersion[2] = FV_VER_3;
	projectFile.projectDataAndHeader.projectHeader.fwVersion[3] = FV_VER_1;

//	char line[15];
//	sprintf(line,
//			"fw.ver: %u.%u.%u",
//			projectFile.projectDataAndHeader.projectHeader.fwVersion[0],
//			projectFile.projectDataAndHeader.projectHeader.fwVersion[1],
//			projectFile.projectDataAndHeader.projectHeader.fwVersion[2]
//			);
//	debugLog.addLine(line);

	projectFile.projectDataAndHeader.projectHeader.fileStructureVersion[0] = PROJECT_FILE_VERSION;
	projectFile.projectDataAndHeader.projectHeader.fileStructureVersion[1] = PROJECT_FILE_VERSION;
	projectFile.projectDataAndHeader.projectHeader.fileStructureVersion[2] = PROJECT_FILE_VERSION;
	projectFile.projectDataAndHeader.projectHeader.fileStructureVersion[3] = PROJECT_FILE_VERSION;

	projectFile.projectDataAndHeader.projectHeader.size = sizeof(projectFile);

	projectFile.crc = crcCalc.crc32((uint8_t *)&projectFile.projectDataAndHeader,sizeof(projectFile.projectDataAndHeader));

	//Serial.print(" memcpy-crc: ");
	//Serial.println(sd_time_test);
	//sd_time_test;

	file=SD.open(name, FILE_WRITE);

	//Serial.print(" open: ");
	//Serial.println(sd_time_test);
	//sd_time_test;

	file.write((uint8_t *)&projectFile, sizeof(projectFile));

	//Serial.print(" write: ");
	//Serial.println(sd_time_test);

	//file.write((uint8_t *)&pattBitmask, sizeof(strPatternsBitmask));

	//sd_time_test;
	file.close();
	//Serial.print(" close: ");
	//Serial.println(sd_time_test);
}

uint8_t FileManager::readInstrumentFile(char * name, strInstrument * instr)
{
	//sd_time_test;
	//Serial.print(" odczyt pliku instrumentu ");

	if(!SD.exists(name)) return 0;
	SdFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strInstrumentFile instrumentFile;

	file=SD.open(name);
	file.read((uint8_t*)&instrumentFile, sizeof(instrumentFile));
	file.close();

	if(instrumentFile.instrumentDataAndHeader.instrHeader.type != fileTypeInstrument) return 0;

	checkCRC=crcCalc.crc32((uint8_t *)&instrumentFile.instrumentDataAndHeader,sizeof(instrumentFile.instrumentDataAndHeader));

	//Serial.print(" t: ");
	//Serial.println(sd_time_test);

	if(checkCRC == instrumentFile.crc)
	{
		*instr=instrumentFile.instrumentDataAndHeader.instrument;

		return 1;
	}
	else return 0;
}

uint8_t FileManager::readPatternFile(char * name, uint8_t *patternDest)
{
	SdFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;
	uint8_t loadStatus = 0;

	//sd_time_test;
	//Serial.print(" odczyt pliku patternu ");

	// na końcu struktury jest crc
	file = SD.open(name);

	if(file)
	{
		file.read(patternDest, sizeof(Sequencer::strPattern));
		file.close();

		checkCRC = crcCalc.crc32(
				patternDest, sizeof(Sequencer::strPattern) - sizeof(uint32_t));

		if (checkCRC == (((Sequencer::strPattern *) patternDest)->crc))
		{
			loadStatus = 1;
			// ok
		}
		else
		{
			loadStatus = 0;
			// not ok
		}
	}

	//Serial.print(" t: ");
	//Serial.println(sd_time_test);


	return loadStatus;
}




uint8_t FileManager::readProjectFile(char * name, strMtProject * proj)
{
	//sd_time_test;
	//Serial.print(" odczyt pliku projktu ");

	if(!SD.exists(name)) return 0;
	SdFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strProjectFile projectFile;

	file=SD.open(name);
	file.read((uint8_t*)&projectFile, sizeof(projectFile));
	//file.read((uint8_t*)&pattBitmask, sizeof(strPatternsBitmask));
	file.close();

	if (projectFile.projectDataAndHeader.projectHeader.type != fileTypeProject)
		return 0;

	checkCRC = crcCalc.crc32((uint8_t *) &projectFile.projectDataAndHeader,
								sizeof(projectFile.projectDataAndHeader));

	if (FILEMANAGER_CONSOLELOG)
	{
		char line[40];
		sprintf(line,
				"opened project fw.ver: %u.%u.%u, crc %s",
				projectFile.projectDataAndHeader.projectHeader.fwVersion[0],
				projectFile.projectDataAndHeader.projectHeader.fwVersion[1],
				projectFile.projectDataAndHeader.projectHeader.fwVersion[2],
				checkCRC == projectFile.crc ? "ok" : "err"
						);
		debugLog.addLine(line);
	}


	//Serial.print(" t: ");
	//Serial.println(sd_time_test);

	if(1)
	{
		memcpy(&proj->song, &projectFile.projectDataAndHeader.project.song, sizeof(strSong));
		memcpy(&proj->values, &projectFile.projectDataAndHeader.project.values, sizeof(strMtValues));
		//*proj=projectFile.projectDataAndHeader.project;
		//mtProject.values=projectFile.projectDataAndHeader.project.values;
		return 1;
	}
	else return 0;
}




void FileManager::copySample(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName)
{
	SdFile file;
	SdFile copy;
	char currentPatch [PATCH_SIZE];
	uint8_t currentBuffor[1024];
	uint16_t lengthData=0;

	//sd_time_test;
	//Serial.print(" kopiowanie sampli ");

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


	//Serial.print(" t: ");
	//Serial.println(sd_time_test);
}



void FileManager::formatSDCard()
{
	//SD.format();
}


void FileManager::delayAutoSave(uint16_t ms)
{
	uint32_t temp_time = 10000-ms;

	if(projectChangeRefresh > temp_time) projectChangeRefresh = temp_time;
	if(instrumentRefresh > temp_time) instrumentRefresh = temp_time;
	if(patternRefresh > temp_time) patternRefresh = temp_time;

}

void FileManager::autoSaveWorkspace(uint8_t forcedWorkspaceSave)
{
	if(projectChangeRefresh > 10000 || forcedWorkspaceSave)
	{
		projectChangeRefresh = 0;

		if(savingInProgress == 0 && loadingInProgress == 0 && deletingInProgress == 0)
		{
			if(projectChangeFlag == 1 && sequencer.isStop())
			{
				fileManager.projectChangeFlag = 0;

				debugLog.addLine("project autosave: ");
				sd_time_test = 0;

				autoSaveProject();

				debugLog.addValue(sd_time_test);
				//debugLog.forceRefresh();
			}
		}
	}

	if((instrumentRefresh > 10000) || forcedWorkspaceSave)
	{
		instrumentRefresh = 0;

		if(savingInProgress == 0 && loadingInProgress == 0 && deletingInProgress == 0)
		{
			for(uint8_t i = 0; i< INSTRUMENTS_COUNT; i++)
			{
				if(instrumentIsChangedFlag[i] == 1 && sequencer.isStop())
				{
					debugLog.addLine("instrument autosave: ");
					sd_time_test = 0;

					saveInstrument(i);

					debugLog.addValue(sd_time_test);
					//debugLog.forceRefresh();
				}
			}
		}
	}

	if(patternRefresh > 10000 || forcedWorkspaceSave)
	{
		patternRefresh = 0;

		if(savingInProgress == 0 && loadingInProgress == 0 && deletingInProgress == 0)
		{
			if(patternIsChangedFlag[mtProject.values.actualPattern] == 1 && sequencer.isStop())
			{
				debugLog.addLine("pattern autosave: ");
				sd_time_test = 0;

				savePattern(mtProject.values.actualPattern);

				debugLog.addValue(sd_time_test);
				//debugLog.forceRefresh();
			}
		}
	}
}

void FileManager::getDefaultSong(struct strSong *source)
{
	source->playlistPos = 0;

	memset(source->playlist, 0, SONG_MAX);


	source->playlist[0] = 1;

}

void FileManager::getDefaultInstrument(struct strInstrument *source)
{
	memset((uint8_t*)source, 0, sizeof(strInstrument));

	source->sample.wavetable_window_size = 2048;
}

void FileManager::getDefaultProject(struct strMtProject *source)
{
	source->instruments_count = 0;
	source->patterns_count = 0;
	source->max_memory = SAMPLE_MEMORY_MAX;
	source->used_memory = 0;

	for(uint32_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		getDefaultInstrument(&source->instrument[i]);
	}

	getDefaultSong(&source->song);
	getDefaultValues(&source->values);
}

void FileManager::getDefaultValues(struct strMtValues *source)
{
	source->lastUsedNote = 0;
	source->lastUsedInstrument = 0;
	source->lastUsedVolume = 0;
	source->lastUsedFx = 0;

	source->actualPattern = 1;

	source->padBoardScale = 0;
	source->padBoardNoteOffset = 12;
	source->padBoardRootNote = 36;
	source->padBoardMaxVoices = 8;

	source->volume = 50;

	source->reverbRoomSize = 80;
	source->reverbDamping = 25;
	source->reverbPanning = 0;

	source->limiterAttack = 100;
	source->limiterRelease = 0.512;
	source->limiterTreshold = 16384;

	source->bitDepth = 16;

	source->patternEditStep = 1;

	for(uint32_t i = 0; i < 8; i++)
	{
		source->trackMute[i] = 0;
	}

//********************************* radio
	source->source = 0;
	source->gainLineIn = 0;
	source->gainMicLow = 0;
	source->gainMicHigh = 0;
	source->gainRadio = 0;
	source->monitor = 0;
	source->radioFreq = 87.5;

	source->projectNotSavedFlag = 0;

// performance
	for(uint32_t i = 0; i < 8; i++)
	{
		source->perfTracksPatterns[i] = 1;
		source->perfTracksState[i] = 0;
	}

	for(uint32_t i = 0; i < 12; i++)
	{
		source->perfSelectedValues[i] = 0;
		source->perfFxPlaces[i] = (i+1);
		source->perfFxValues[i][0] = 0;
		source->perfFxValues[i][1] = 0;
		source->perfFxValues[i][2] = 0;
		source->perfFxValues[i][3] = 0;
	}

// song
	source->globalTempo = DEFAULT_TEMPO;
	source->patternLength = 32;

	memset(source->instrumentsToSave, 0, INSTRUMENTS_COUNT);
	memset(source->patternsToSave, 0, PATTERN_INDEX_MAX);

	//instrumenty midi

	for(uint8_t channel = 0; channel<16; channel++)
	{
		source->midiInstrument[channel].velocity = DEFAULT_MIDI_VELOCITY;
	}

	memset(source->allPatternsBitmask, 0, sizeof(source->allPatternsBitmask));
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
//	strcat(currentPatch,"/project.mt");
//
//	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
//}


