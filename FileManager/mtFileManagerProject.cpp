#include "mtFileManager.h"
#include "mtConfig.h"

uint8_t FileManager::createNewProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	sprintf(currentProjectPatch,"Projects/%s",name);

	if(SD.exists(currentProjectPatch)) return 2;
	strcpy(currentProjectName,name);

//****************************************************************************
// Glówny folder projektu
//****************************************************************************
	if(!SD.exists("Projects")) SD.mkdir(0,"Projects");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(0,currentProjectPatch);

	sprintf(patchFolder,"Projects/%s/instruments", name);
	SD.mkdir(0,patchFolder);

	sprintf(patchFolder,"Projects/%s/patterns", name);
	SD.mkdir(0,patchFolder);

	sprintf(patchFolder,"Projects/%s/samples", name);
	SD.mkdir(0,patchFolder);

	sprintf(patchFolder,"Projects/%s/project.bin", name);
	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);
//****************************************************************************
// Ukryty folder projektu
//****************************************************************************
	if(SD.exists("Workspace")) SD.remove("Workspace");

	SD.mkdir(1,"Workspace");

	sprintf(patchFolder,"Workspace/%s/instruments", name);
	SD.mkdir(1,patchFolder);

	sprintf(patchFolder,"Workspace/%s/patterns", name);
	SD.mkdir(1,patchFolder);

	sprintf(patchFolder,"Workspace/%s/samples", name);
	SD.mkdir(1,patchFolder);

	sprintf(patchFolder,"Workspace/%s/project.bin", name);
	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);
	return 1;
}

uint8_t FileManager::openTemplateBasedProject(char* projectName, char* templateName)
{
	uint8_t status = 0;
	openTemplateBasedProjectFlag = 1;
	sprintf(currentProjectNameOpenTemplate,projectName);
	status = fileManager.openProject(templateName,projectTypeExample);
	sprintf(currentProjectName,projectName);



	return status;
}

uint8_t FileManager::openProject(char * name , uint8_t type)
{
	uint8_t status;
	char currentPatch[PATCH_SIZE];
	char workspacePatch[PATCH_SIZE];

//**************************************************************************
//Zapamietanie nazwy projektu
//Skasowanie starego workspace
//Utworzenie nowych folderow do workspace
//**************************************************************************
	strcpy(currentProjectName,name);

	if(type == projectTypeExample)
	{
		sprintf(currentProjectPatch,"Templates/%s",name);
	}
	else if( type == projectTypeUserMade)
	{
		sprintf(currentProjectPatch,"Projects/%s",name);
	}

	strcpy(currentProjectName, name);
	strcpy(mtConfig.startup.lastProjectName, currentProjectName);

	forceSaveConfig();

	if(SD.exists("Workspace")) SD.remove("Workspace");

	SD.mkdir(1,"Workspace");

	strcpy(currentPatch,"Workspace/instruments");
	SD.mkdir(1,currentPatch);

	strcpy(currentPatch,"Workspace/patterns");
	SD.mkdir(1,currentPatch);

	strcpy(currentPatch,"Workspace/samples");
	SD.mkdir(1,currentPatch);

//**************************************************************************
//Wczytanie pliku projektu do pamięci i zapisanie go z pamięci do workspace
//**************************************************************************
	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);

	status = readProjectFile(currentPatch, &mtProject.mtProjectRemote);
	if(!status) return status;
	strcpy(currentPatch,"Workspace/project.bin");
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
//**************************************************************************
//Wczytanie pilków instrumentów do pamięci i zapisanie ich do workspace
//**************************************************************************
	for(int i=0; i < INSTRUMENTS_COUNT; i++)
	{

		 sprintf(currentPatch,"%s/instruments/instrument_%02d.mti",currentProjectPatch,i);

		 if(SD.exists(currentPatch))
		 {
			 status=readInstrumentFile(currentPatch,&mtProject.instrument[i]);
			 if(!status) return status;

			 sprintf(currentPatch,"Workspace/instruments/instrument_%02d.mti",i);
			 writeInstrumentFile(currentPatch,&mtProject.instrument[i]);
		 }
		 else
		 {
			 sprintf(currentPatch,"Workspace/instruments/instrument_%02d.mti",i);
			 if(SD.exists(currentPatch)) SD.remove(currentPatch);
			 sprintf(currentPatch,"Workspace/samples/instr%02d.wav",i);
			 if(SD.exists(currentPatch)) SD.remove(currentPatch);
			 memset(mtProject.instrument[i].sample.file_name,0, SAMPLE_NAME_SIZE);
			 mtProject.instrument[i].isActive = 0;
		 }
	}
//**************************************************************************
//Skopiowanie istniejących patternów do workspace i załadowanie pierwszego do pamięci
//**************************************************************************
	for (int i = PATTERN_INDEX_MIN; i < PATTERN_INDEX_MAX; i++)
	{
		 sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",currentProjectPatch,i);

		 if(SD.exists(currentPatch))
		 {
			 copyPattern(currentProjectPatch,i,(char*)"Workspace",i);
		 }
		 else
		 {
			 sprintf(currentPatch,"Workspace/patterns/pattern_%02d.mtp",i);
			 if(SD.exists(currentPatch)) SD.remove(currentPatch);
		 }
	}

	for (int i = PATTERN_INDEX_MIN; i < PATTERN_INDEX_MAX; i++)
	{
		if (loadPattern(i))
		{
			sequencer.switchNextPatternNow();
			break;
		}
	}
//**************************************************************************
//Wyszukanie pierwszego aktywnego instrumentu, żeby odpalić sekwencyjne kopiowanie do workspace
//**************************************************************************
	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		currentSaveWave = i+1;
		if(mtProject.instrument[i].isActive == 1)
		{
			sprintf(currentPatch,"%s/samples/instr%02d.wav",currentProjectPatch,i);
			sprintf(workspacePatch,"Workspace/samples/instr%02d.wav",i);

			samplesCopyier.start(workspacePatch,currentPatch);
			break;
		}
	}
	if(currentSaveWave == (INSTRUMENTS_COUNT))
	{
		if(openTemplateBasedProjectFlag)
		{
			openTemplateBasedProjectFlag = 0;
			startSaveAsProject(currentProjectNameOpenTemplate);
		}
	}
	else openWorkspaceCreateFlag = 1;

	mtProject.used_memory = 0;

	return status;
}

//uint8_t FileManager::createNewProject(char * name)
//{
//	char patchFolder[PATCH_SIZE];
//
//	sprintf(currentProjectPatch,"Projects/%s",name);
//
//	if(SD.exists(currentProjectPatch)) return 2;
//	strcpy(currentProjectName,name);
//
//	if(!SD.exists("Projects")) SD.mkdir("Projects");
//	if(!SD.exists(currentProjectPatch)) SD.mkdir(currentProjectPatch);
//
//	sprintf(patchFolder,"Projects/%s/instruments", name);
//	SD.mkdir(patchFolder);
//
//	sprintf(patchFolder,"Projects/%s/patterns", name);
//	SD.mkdir(patchFolder);
//
//	sprintf(patchFolder,"Projects/%s/samples", name);
//	SD.mkdir(patchFolder);
//
//	sprintf(patchFolder,"Projects/%s/project.bin", name);
//	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);
//
//	return 1;
//}




void FileManager::importProject(char* sourceProjectPatch,char* name, char* newName)
{
	char currentPatch [PATCH_SIZE];
	char currentName [SAMPLE_NAME_SIZE];
	createNewProject(name);

	if(!SD.exists("Projects")) SD.mkdir(0,"Projects"); //todo: możliwe że to nie jest konieczne bo jest w createNewProject

	sprintf(currentProjectPatch, "Projects/%s",newName);
	strcpy(currentProjectName,newName);


	sprintf(currentPatch,"%s/project.bin",sourceProjectPatch);
	readProjectFile(currentPatch,&mtProject.mtProjectRemote);



	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(currentPatch, "%s/instruments/instrument_%02d.mti", sourceProjectPatch,i);

		if(SD.exists(currentPatch))
		{
			sprintf(currentName,"instr%02d.wav",i);
			sprintf(currentPatch,"%s/samples",sourceProjectPatch);
			//todo: to nie ma sensu -> przerobic gdy bedzie potrzebne
			samplesImporter.start(currentPatch,currentName,currentProjectPatch,i,mtProject.mtProjectRemote.instrumentFile[i].sampleType);
		}
	}

	sprintf(currentPatch,"%s/instruments",sourceProjectPatch);
	for(uint8_t i=0; i < INSTRUMENTS_COUNT; i++)
	{
		sprintf(currentPatch, "%s/instruments/instrument_%02d.mti", sourceProjectPatch,i);
		if(SD.exists(currentPatch))
		{
			sprintf(currentName,"instrument_%02d.mti",i);
			importInstrumentToProject(sourceProjectPatch,currentName,i);
		}
	}

	for(uint8_t i=0; i < PATTERN_INDEX_MAX; i++)
	{
		sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",sourceProjectPatch,i);
		if(SD.exists(currentPatch))
		{
			mtProject.patterns_count++;
		}
	}
}

//uint8_t FileManager::saveAsProject(char* name)
//{
////	TODO: saveAs i save muszą się wywoływać wzajemnie
////	return 0;
//	char currentPatch [PATCH_SIZE];
//	char currentName[SAMPLE_NAME_SIZE];
//	if(createNewProject(name) == 2) return 2;
//
//	for(uint8_t i=0;i<INSTRUMENTS_COUNT;i++)
//	{
//
//		if(mtProject.instrument[i].isActive == 1)
//		{
//			mtProject.mtProjectRemote.instrumentFile[i].sampleType=mtProject.instrument[i].sample.type;
//
//			sprintf(currentPatch,"Projects/%s", name);
//
//			if(i < 10 ) sprintf(currentName,"instr0%d.wav",i);
//			else sprintf(currentName,"instr%d.wav",i);
//			copySample(currentProjectPatch,currentName,currentPatch,currentName);
//
//			if(i<10) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,i);
//			else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,i);
//			writeInstrumentFile(currentPatch, &mtProject.instrument[i]);
//		}
//	}
//	for(uint8_t i=0; i< PATTERN_INDEX_MAX; i++)
//	{
//		if(i<10) sprintf(currentPatch,"%s/patterns/pattern_0%d.mtp", currentProjectPatch,i);
//		else sprintf(currentPatch,"%s/patterns/pattern_%d.mtp", currentProjectPatch,i);
//		if(SD.exists(currentPatch))
//		{
//
//			if(i == mtProject.values.actualPattern)
//			{
//				savePattern(i);
//
//				sprintf(currentPatch,"Projects/%s", name);
//			}
////			TODO:
////			copyPattern(currentProjectPatch, mtProject.mtProjectRemote.patternFile[i].name,currentPatch,mtProject.mtProjectRemote.patternFile[i].name);
//		}
//	}
//	sprintf(currentPatch,"Projects/%s/project.bin", name);
//
//	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);
//	return 1;
//
//}

void FileManager::startSaveAsProject(char *name)
{
	sprintf(currentProjectPatch,"Projects/%s",name);
	strcpy(currentProjectName,name);
	startSaveProject();

	saveAsFlag = 1;
}
//void FileManager::saveProject()
//{
//	char currentPatch [PATCH_SIZE];
//
//	for(uint8_t i=0;i<INSTRUMENTS_COUNT;i++)
//	{
//
//		if(mtProject.instrument[i].isActive == 1)
//		{
//
//			if(i<10) sprintf(currentPatch,"%s/instruments/instrument_0%d.mti",currentProjectPatch,i);
//			else sprintf(currentPatch,"%s/instruments/instrument_%d.mti",currentProjectPatch,i);
//
//			mtProject.mtProjectRemote.instrumentFile[i].sampleType=mtProject.instrument[i].sample.type;
//			writeInstrumentFile(currentPatch, &mtProject.instrument[i]);
//		}
//	}
//
//	savePattern(mtProject.values.actualPattern);
////	for(uint8_t i=0; i< PATTERNS_COUNT; i++)
////	{
////
////		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
////		{
////			if(mtProject.mtProjectRemote.patternFile[i].index == mtProject.values.actualPattern)
////			{
////
////
////			}
////		}
////		else
////		{
////			memset(mtProject.mtProjectRemote.patternFile[i].name,0,PATTERN_NAME_SIZE);
////		}
////
////	}
//	sprintf(currentPatch,"%s/project.bin", currentProjectPatch);
//
//	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);
//
//
//	// TODO
//	// dodany zapis ostatnio uzywanego projektu do configu
//	//if(strcmp(currentProjectName, mtConfig.startup.lastProjectName) != 0)
//	//{
//
//
//		strcpy(mtConfig.startup.lastProjectName, currentProjectName);
//
//		forceSaveConfig();
//	//}
//
//}

uint8_t FileManager::getSaveProjectState()
{
	return saveProjectFlag;
}
uint8_t FileManager::getOpenProjectState()
{
	return openWorkspaceCreateFlag;
}

void FileManager::startSaveProject()
{
	char currentPatch[PATCH_SIZE];

	SD.remove(currentProjectPatch);

	//****************************************************************************
	// Glówny folder projektu
	//****************************************************************************
	if(!SD.exists("Projects")) SD.mkdir(0,"Projects");
	SD.mkdir(0,currentProjectPatch);

	sprintf(currentPatch,"%s/instruments", currentProjectPatch);
	SD.mkdir(0,currentPatch);

	sprintf(currentPatch,"%s/patterns", currentProjectPatch);
	SD.mkdir(0,currentPatch);

	sprintf(currentPatch,"%s/samples", currentProjectPatch);
	SD.mkdir(0,currentPatch);

	sprintf(currentPatch,"%s/project.bin", currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

	savePattern(mtProject.values.actualPattern);

	for(uint8_t i = PATTERN_INDEX_MIN; i< PATTERN_INDEX_MAX ; i++)
	{
		sprintf(currentPatch,"Workspace/patterns/pattern_%02d.mtp",i);

		if(SD.exists(currentPatch))
		{
			copyPattern((char*)"Workspace",i,currentProjectPatch,i);
		}
		else
		{
			sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",currentProjectPatch,i);
			if(SD.exists(currentPatch)) SD.remove(currentPatch);
		}
	}

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		if(mtProject.instrument[i].isActive == 1)
		{
			sprintf(currentPatch,"%s/instruments/instrument_%02d.mti",currentProjectPatch,i);
			writeInstrumentFile(currentPatch, &mtProject.instrument[i]);
		}
		else
		{
			sprintf(currentPatch,"%s/instruments/instrument_%02d.mti",currentProjectPatch,i);
			if(SD.exists(currentPatch)) SD.remove(currentPatch);
			sprintf(currentPatch,"%s/samples/instr%02d.wav",currentProjectPatch,i);
			if(SD.exists(currentPatch)) SD.remove(currentPatch);
		}
	}

	sprintf(currentPatch,"%s/project.bin", currentProjectPatch);
	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);

	for(uint8_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		if(mtProject.instrument[i].isActive == 1)
		{
			char workspacePatch[PATCH_SIZE];
			sprintf(currentPatch,"%s/samples/instr%02d.wav",currentProjectPatch,i);
			sprintf(workspacePatch,"Workspace/samples/instr%02d.wav",i);

			samplesCopyier.start(currentPatch,workspacePatch);
			saveProjectFlag = 1;

			currentSaveWave = i+1;
			break;
		}
	}

	strcpy(mtConfig.startup.lastProjectName, currentProjectName);

	forceSaveConfig();
}

void FileManager::createEmptyTemplateProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	memset(mtProject.mtProjectRemote.song.playlist,0,SONG_MAX);
	mtProject.mtProjectRemote.song.playlist[0]=0;

	sprintf(currentProjectPatch,"Templates/%s",name);

	strcpy(currentProjectName,name);

	if(!SD.exists("Projects")) SD.mkdir(0,"Projects");
	if(!SD.exists("Templates")) SD.mkdir(0,"Templates");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(0,currentProjectPatch);

	sprintf(patchFolder,"Templates/%s/instruments", name);
	SD.mkdir(0,patchFolder);

	sprintf(patchFolder,"Templates/%s/patterns", name);
	SD.mkdir(0,patchFolder);

	sprintf(patchFolder,"Templates/%s/samples", name);
	SD.mkdir(0,patchFolder);

	mtProject.patterns_count++;
	writePatternFile(patchFolder);

	sprintf(patchFolder,"Templates/%s/project.bin", name);

	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);
}

