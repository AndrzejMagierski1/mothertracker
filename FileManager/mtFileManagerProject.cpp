#include "mtFileManager.h"
#include "mtConfig.h"

uint8_t FileManager::openProject(char * name , uint8_t type)
{
	uint8_t status;
	char currentPatch[PATCH_SIZE];

	if(type == projectTypeExample)
	{
		sprintf(currentPatch,"Templates/%s",name);
		strcpy(currentProjectPatch,currentPatch);
	}
	else if( type == projectTypeUserMade)
	{
		sprintf(currentPatch,"Projects/%s",name);
		strcpy(currentProjectPatch,currentPatch);
	}


	strcpy(currentProjectName, name);
	strcpy(mtConfig.startup.lastProjectName, currentProjectName);

	forceSaveConfig();

//	strcpy(mtProject.path, currentPatch); // aktualna ścieżka projektu z nazwą
//	strcat(mtProject.path, "/"); // aktualna ścieżka projektu z nazwą

	strcat(currentPatch,"/project.bin");

	status = readProjectFile(currentPatch, &mtProject.mtProjectRemote);
	if(!status) return status;


	mtProject.instruments_count=0;
	for(int i=0; i < INSTRUMENTS_COUNT; i++)
	{
		 if(mtProject.mtProjectRemote.instrumentFile[i].index != - 1)
		 {
			 sprintf(currentPatch,"%s/instruments/%s",currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[i].name);
			 status=readInstrumentFile(currentPatch,&mtProject.instrument[i]);
			 if(!status) return status;

			 memcpy(mtProject.instrument[i].sample.file_name,mtProject.mtProjectRemote.instrumentFile[i].sample.name, SAMPLE_NAME_SIZE);
			 mtProject.instrument[i].sample.type=mtProject.mtProjectRemote.instrumentFile[i].sample.type;
			 mtProject.instrument[i].isActive = 1;
			 mtProject.instruments_count++;
		 }
		 else
		 {
			 memset(mtProject.instrument[i].sample.file_name,0, SAMPLE_NAME_SIZE);
			 mtProject.instrument[i].sample.loaded = 0;
			 mtProject.instrument[i].sample.length = 0;
			 mtProject.instrument[i].isActive = 0;
		 }
	}

//	for(int i=0; i< PATTERNS_COUNT;i++)
//	{
//		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
//		{
//			memset(currentPatch,0,PATCH_SIZE);
//			strcpy(currentPatch,currentProjectPatch);
//			strcat(currentPatch,"/patterns/");
//			strcat(currentPatch,mtProject.mtProjectRemote.patternFile[i].name);
//			status=readPatternFile(currentPatch);
//			if(!status) return status;
//			else break;
//		}
//	}


	for (int i = PATTERN_INDEX_MIN; i < PATTERN_INDEX_MAX; i++)
	{
		if (loadPattern(i))
		{
			mtProject.mtProjectRemote.patternFile[i].index = i;
			sequencer.switchNextPatternNow();
			break;
		}
	}

	if(status) fileManager.samplesLoader.start(0,currentProjectPatch,1);
	return status;
}

uint8_t FileManager::createNewProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	sprintf(currentProjectPatch,"Projects/%s",name);

	if(SD.exists(currentProjectPatch)) return 2;
	strcpy(currentProjectName,name);

	if(!SD.exists("Projects")) SD.mkdir("Projects");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(currentProjectPatch);

	sprintf(patchFolder,"Projects/%s/instruments", name);
	SD.mkdir(patchFolder);

	sprintf(patchFolder,"Projects/%s/patterns", name);
	SD.mkdir(patchFolder);

	sprintf(patchFolder,"Projects/%s/samples", name);
	SD.mkdir(patchFolder);

	sprintf(patchFolder,"Projects/%s/project.bin", name);
	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);

	return 1;
}




void FileManager::importProject(char* sourceProjectPatch,char* name, char* newName)
{
	char currentPatch [PATCH_SIZE];
	createNewProject(name);

	if(!SD.exists("Projects")) SD.mkdir("Projects"); //todo: możliwe że to nie jest konieczne bo jest w createNewProject

	sprintf(currentProjectPatch, "Projects/%s",newName);
	strcpy(currentProjectName,newName);


	sprintf(currentPatch,"%s/project.bin",sourceProjectPatch);
	readProjectFile(currentPatch,&mtProject.mtProjectRemote);

	sprintf(currentPatch,"%s/samples",sourceProjectPatch);

	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{
			samplesImporter.start(currentPatch,mtProject.mtProjectRemote.instrumentFile[i].sample.name,currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[i].index ,mtProject.mtProjectRemote.instrumentFile[i].sample.type);
		}
	}

	sprintf(currentPatch,"%s/instruments",sourceProjectPatch);
	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{
			mtProject.instruments_count++;
			importInstrumentToProject(currentPatch,mtProject.mtProjectRemote.instrumentFile[i].name,mtProject.mtProjectRemote.instrumentFile[i].index);
		}
	}
	sprintf(currentPatch,"%s/patterns",sourceProjectPatch);
	for(uint8_t i=0; i < PATTERN_INDEX_MAX; i++)
	{
		if(mtProject.mtProjectRemote.patternFile[i].index != -1)
		{
			mtProject.patterns_count++;
//			importInstrumentToProject(currentPatch,mtProject.mtProjectRemote.patternFile[i].name,mtProject.mtProjectRemote.patternFile[i].index);
		}
	}
}

uint8_t FileManager::saveAsProject(char* name)
{
//	TODO: saveAs i save muszą się wywoływać wzajemnie
//	return 0;
	char currentPatch [PATCH_SIZE];
	if(createNewProject(name) == 2) return 2;

	for(uint8_t i=0;i<INSTRUMENTS_COUNT;i++)
	{

		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{
			sprintf(currentPatch,"Projects/%s/instruments/%s",name,mtProject.mtProjectRemote.instrumentFile[i].name);

			memcpy(mtProject.mtProjectRemote.instrumentFile[i].sample.name,mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index].sample.file_name, SAMPLE_NAME_SIZE);
			mtProject.mtProjectRemote.instrumentFile[i].sample.type=mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index].sample.type;

			sprintf(currentPatch,"Projects/%s", name);

			copySample(currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[i].sample.name,currentPatch,mtProject.mtProjectRemote.instrumentFile[i].sample.name);


			writeInstrumentFile(currentPatch, &mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
		}
	}
	for(uint8_t i=0; i< PATTERN_INDEX_MAX; i++)
	{

		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
		{

			if(i == mtProject.values.actualPattern)
			{
				savePattern(i);

				sprintf(currentPatch,"Projects/%s", name);
			}
//			TODO:
//			copyPattern(currentProjectPatch, mtProject.mtProjectRemote.patternFile[i].name,currentPatch,mtProject.mtProjectRemote.patternFile[i].name);
		}
	}
	sprintf(currentPatch,"Projects/%s/project.bin", name);

	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);
	return 1;

}

void FileManager::saveProject()
{
	char currentPatch [PATCH_SIZE];




	for(uint8_t i=0;i<INSTRUMENTS_COUNT;i++)
	{

		if(mtProject.mtProjectRemote.instrumentFile[i].index != -1)
		{

			sprintf(currentPatch,"%s/instruments/%s",currentProjectPatch,mtProject.mtProjectRemote.instrumentFile[i].name);

			memcpy(mtProject.mtProjectRemote.instrumentFile[i].sample.name,mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index].sample.file_name, SAMPLE_NAME_SIZE);
			mtProject.mtProjectRemote.instrumentFile[i].sample.type=mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index].sample.type;
			writeInstrumentFile(currentPatch, &mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
		}
		else
		{
			memset(mtProject.mtProjectRemote.instrumentFile[i].name,0,INSTRUMENT_NAME_SIZE);
			memset(mtProject.mtProjectRemote.instrumentFile[i].sample.name,0,SAMPLE_NAME_SIZE);
		}
	}

	savePattern(mtProject.values.actualPattern);
//	for(uint8_t i=0; i< PATTERNS_COUNT; i++)
//	{
//
//		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
//		{
//			if(mtProject.mtProjectRemote.patternFile[i].index == mtProject.values.actualPattern)
//			{
//
//
//			}
//		}
//		else
//		{
//			memset(mtProject.mtProjectRemote.patternFile[i].name,0,PATTERN_NAME_SIZE);
//		}
//
//	}
	sprintf(currentPatch,"%s/project.bin", currentProjectPatch);

	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);


	// TODO
	// dodany zapis ostatnio uzywanego projektu do configu
	//if(strcmp(currentProjectName, mtConfig.startup.lastProjectName) != 0)
	//{


		strcpy(mtConfig.startup.lastProjectName, currentProjectName);

		forceSaveConfig();
	//}

}

void FileManager::createEmptyTemplateProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	memset(mtProject.mtProjectRemote.song.playlist,0,SONG_MAX);
	mtProject.mtProjectRemote.song.playlist[0]=0;
	mtProject.mtProjectRemote.patternFile[0].index=0;
//	strcpy(mtProject.mtProjectRemote.patternFile[0].name,"pattern_00.mtp");


	sprintf(currentProjectPatch,"Templates/%s",name);

	strcpy(currentProjectName,name);

	if(!SD.exists("Projects")) SD.mkdir("Projects");
	if(!SD.exists("Templates")) SD.mkdir("Templates");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(currentProjectPatch);

	sprintf(patchFolder,"Templates/%s/instruments", name);
	SD.mkdir(patchFolder);

	sprintf(patchFolder,"Templates/%s/patterns", name);
	SD.mkdir(patchFolder);

	sprintf(patchFolder,"Templates/%s/samples", name);
	SD.mkdir(patchFolder);

	savePattern(0);

//	memset(patchFolder,0,PATCH_SIZE);
//	strcpy(patchFolder,"Templates/");
//	strcat(patchFolder,name);
//	strcat(patchFolder,"/patterns/");
//	strcat(patchFolder,mtProject.mtProjectRemote.patternFile[0].name);

	mtProject.patterns_count++;
	writePatternFile(patchFolder);

	sprintf(patchFolder,"Templates/%s/project.bin", name);

	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);

//	addInstrumentToProject (0);
}

