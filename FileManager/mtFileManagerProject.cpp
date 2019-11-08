#include "mtFileManager.h"
#include "mtConfig.h"

void FileManager::moveToNextStage(save_load_handle_t *handle)
{
	handle->stage++;
}

uint8_t FileManager::getLoadingStatus()
{
	return loadingInProgress;
}

uint8_t FileManager::getSavingStatus()
{
	return savingInProgress;
}

//<! Loading whole project from file functions
void FileManager::refreshLoadingSamples()
{
	uint8_t skip = 0;

	if(loadHandle.nextSampleFlag)
	{
		if(mtProject.instrument[loadHandle.currSampleIdx].isActive == 1)
		{
			char currentPatch[PATCH_SIZE];
			char workspacePatch[PATCH_SIZE];

			sprintf(currentPatch, "%s/samples/instr%02d.wav", currentProjectPatch, loadHandle.currSampleIdx);
			sprintf(workspacePatch," Workspace/samples/instr%02d.wav", loadHandle.currSampleIdx);

			samplesCopyier.start(workspacePatch,currentPatch);
		}
		else
		{
			skip = 1;
		}

		loadHandle.nextSampleFlag = 0;
	}

	if((samplesCopyier.getState() == 0) || skip)
	{
		loadHandle.nextSampleFlag = 1;
		loadHandle.currSampleIdx++;
	}

	if(loadHandle.currSampleIdx >= INSTRUMENTS_COUNT)
	{
		moveToNextStage(&loadHandle);
	}
}

void FileManager::refreshLoadingPatterns()
{
	char currentPatch[PATCH_SIZE];

	sprintf(currentPatch, "%s/patterns/pattern_%02d.mtp", currentProjectPatch, loadHandle.currPatternIdx + 1);

	if(SD.exists(currentPatch))
	{
		copyPattern(currentProjectPatch, loadHandle.currPatternIdx + 1, (char*)"Workspace", loadHandle.currPatternIdx + 1);
	}

	loadHandle.currPatternIdx++;

	if(loadHandle.currPatternIdx >= PATTERN_INDEX_MAX)
	{
		if(loadPattern(mtProject.values.actualPattern))
		{
			sequencer.switchRamPatternsNow();
		}
		else
		{
			mtProject.values.actualPattern = 1;
			savePattern(mtProject.values.actualPattern);
		}

		moveToNextStage(&loadHandle);
	}
}

void FileManager::refreshLoadingInstrumentFiles()
{
	char currentPatch[PATCH_SIZE];

	sprintf(currentPatch, "%s/instruments/instrument_%02d.mti", currentProjectPatch, loadHandle.currInstrumentFileIdx);

	if(SD.exists(currentPatch))
	{
		readInstrumentFile(currentPatch, &mtProject.instrument[loadHandle.currInstrumentFileIdx]);

		sprintf(currentPatch, "Workspace/instruments/instrument_%02d.mti", loadHandle.currInstrumentFileIdx);
		writeInstrumentFile(currentPatch, &mtProject.instrument[loadHandle.currInstrumentFileIdx]);
	}

	loadHandle.currInstrumentFileIdx++;
	if(loadHandle.currInstrumentFileIdx >= INSTRUMENTS_COUNT)
	{
		moveToNextStage(&loadHandle);
	}
}

void FileManager::refreshLoadingInstrToMem()
{
	if(loadHandle.forcedInstrReload == 0)
	{
		samplesLoader.start(0,(char*)"Workspace");
		loadHandle.forcedInstrReload = 1;
	}

	if(samplesLoader.getStateFlag() != loaderStateTypeInProgress)
	{
		moveToNextStage(&loadHandle);
	}
}

void FileManager::refreshProjectOpening()
{
	switch(loadHandle.stage)
	{
	case sLoadClearWorkspace:
		refreshClearDir("Workspace", &loadHandle);
		break;
	case sLoadInstrumentFiles:
		refreshLoadingInstrumentFiles();
		break;
	case sLoadPatterns:
		refreshLoadingPatterns();
		break;
	case sLoadSamples:
		refreshLoadingSamples();
		break;
	case sLoadInstrumentsToMem:
		refreshLoadingInstrToMem();
		break;
	case sLoadDone:
		loadingInProgress = 0;
		break;
	default:
		break;
	}
}

//Ta funkcja czysci tylko wszystkie pliki projektowe/ nie usuwa folderow!
void FileManager::refreshClearDir(const char* dirToClear, save_load_handle_t *handle)
{
	char currentPatch[PATCH_SIZE];

	if(handle->currInstrumentFileIdx < INSTRUMENTS_COUNT)
	{
		sprintf(currentPatch, "%s/instruments/instrument_%02d.mti", dirToClear, handle->currInstrumentFileIdx);

		if(SD.exists(currentPatch))
		{
			SD.remove(currentPatch);
		}

		handle->currInstrumentFileIdx++;
	}
	else if(handle->currPatternIdx < PATTERN_INDEX_MAX )
	{
		sprintf(currentPatch, "%s/patterns/pattern_%02d.mtp", dirToClear, handle->currPatternIdx + 1);

		if(SD.exists(currentPatch))
		{
			SD.remove(currentPatch);
		}

		handle->currPatternIdx++;
	}
	else if(handle->currSampleIdx < INSTRUMENTS_COUNT)
	{
		sprintf(currentPatch, "%s/samples/instr%02d.wav", dirToClear, handle->currSampleIdx);

		if(SD.exists(currentPatch))
		{
			SD.remove(currentPatch);
		}

		handle->currSampleIdx++;
	}
	else
	{
		handle->currPatternIdx = 0;
		handle->currInstrumentFileIdx = 0;
		handle->currSampleIdx = 0;

		moveToNextStage(handle);
	}
}

uint8_t FileManager::openProjectStart(char * name , uint8_t type)
{
	memset(&loadHandle, 0, sizeof(save_load_handle_t));
	loadingInProgress = 1;
	loadHandle.nextSampleFlag = 1;

	uint8_t status;
	char currentPatch[PATCH_SIZE];

	if(type == projectTypeExample)
	{
		sprintf(currentProjectPatch,"Templates/%s",name);
	}
	else if(type == projectTypeUserMade)
	{
		strcpy(currentProjectName,name);
		strcpy(mtConfig.startup.lastProjectName, currentProjectName);
		sprintf(currentProjectPatch,"Projects/%s",name);
	}

	//**************************************************************************
	//Wczytanie pliku projektu do pamięci i zapisanie go z pamięci do workspace
	//**************************************************************************
	sprintf(currentPatch, "%s/project.bin", currentProjectPatch);

	status = readProjectFile(currentPatch, &mtProject.mtProjectRemote);
	if(status)
	{
		sprintf(currentPatch, "Workspace/project.bin");

		if(SD.exists(currentPatch))
		{
			SD.remove(currentPatch);
		}

		writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
	}
	else
	{
		return status;
	}

	forceSaveConfig();

	if(!SD.exists("Workspace"))
	{
		SD.mkdir(1,"Workspace");
	}

	if(!SD.exists("Workspace/instruments"))
	{
		SD.mkdir(1,"Workspace/instruments");
	}

	if(!SD.exists("Workspace/patterns"))
	{
		SD.mkdir(1,"Workspace/patterns");
	}

	if(!SD.exists("Workspace/samples"))
	{
		SD.mkdir(1,"Workspace/samples");
	}

	mtProject.used_memory = 0;

	moveToNextStage(&loadHandle);
	return status;
}

//<! Saving whole project from file functions
uint8_t FileManager::prepareSaveAs(char *name, uint8_t type)
{
	sprintf(currentProjectPatch,"Projects/%s",name);
	strcpy(currentProjectName,name);

	for(uint32_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		mtProject.values.instrumentsToSave[i] = 1;
	}

	for(uint32_t i = PATTERN_INDEX_MIN; i < PATTERN_INDEX_MAX; i++)
	{
		mtProject.values.patternsToSave[i] = 1;
	}

	if(SD.exists(currentProjectPatch))
	{
		if(type == saveAsChecking) return 0;
	}

	return 1;
}

void FileManager::refreshSavePatterns()
{
	char currentPatch[PATCH_SIZE];

	if(mtProject.values.patternsToSave[saveHandle.currPatternIdx+1])
	{
		mtProject.values.patternsToSave[saveHandle.currPatternIdx+1] = 0;

		sprintf(currentPatch, "Workspace/patterns/pattern_%02d.mtp", saveHandle.currPatternIdx+1);

		if(SD.exists(currentPatch))
		{
			copyPattern((char*)"Workspace", saveHandle.currPatternIdx+1, currentProjectPatch, saveHandle.currPatternIdx+1);
		}
		else
		{
			sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp", currentProjectPatch, saveHandle.currPatternIdx+1);
			if(SD.exists(currentPatch))
			{
				SD.remove(currentPatch);
			}
		}
	}

	saveHandle.currPatternIdx++;

	if(saveHandle.currPatternIdx == PATTERN_INDEX_MAX-1)
	{
		moveToNextStage(&saveHandle);
	}
}

void FileManager::refreshSaveInstrumentFiles()
{
	char currentPatch[PATCH_SIZE];

	if(mtProject.values.instrumentsToSave[saveHandle.currInstrumentFileIdx])
	{
		if(mtProject.instrument[saveHandle.currInstrumentFileIdx].isActive == 1)
		{
			sprintf(currentPatch,"%s/instruments/instrument_%02d.mti",currentProjectPatch,saveHandle.currInstrumentFileIdx);
			writeInstrumentFile(currentPatch, &mtProject.instrument[saveHandle.currInstrumentFileIdx]);

			sprintf(currentPatch,"Workspace/instruments/instrument_%02d.mti",saveHandle.currInstrumentFileIdx);
			writeInstrumentFile(currentPatch, &mtProject.instrument[saveHandle.currInstrumentFileIdx]);
		}
		else
		{
			sprintf(currentPatch,"%s/instruments/instrument_%02d.mti",currentProjectPatch,saveHandle.currInstrumentFileIdx);
			if(SD.exists(currentPatch)) SD.remove(currentPatch);
			sprintf(currentPatch,"%s/samples/instr%02d.wav", currentProjectPatch, saveHandle.currInstrumentFileIdx);
			if(SD.exists(currentPatch)) SD.remove(currentPatch);
		}
	}

	saveHandle.currInstrumentFileIdx++;

	if(saveHandle.currInstrumentFileIdx == INSTRUMENTS_COUNT)
	{
		moveToNextStage(&saveHandle);
	}
}

void FileManager::refreshSaveSamples()
{
	uint8_t skip = 0;

	if(saveHandle.nextSampleFlag)
	{
		if(mtProject.values.instrumentsToSave[saveHandle.currSampleIdx])
		{
			mtProject.values.instrumentsToSave[saveHandle.currSampleIdx] = 0;

			if(mtProject.instrument[saveHandle.currSampleIdx].isActive == 1)
			{
				char workspacePatch[PATCH_SIZE];
				char currentPatch[PATCH_SIZE];
				sprintf(currentPatch,"%s/samples/instr%02d.wav",currentProjectPatch, saveHandle.currSampleIdx);
				sprintf(workspacePatch,"Workspace/samples/instr%02d.wav", saveHandle.currSampleIdx);

				samplesCopyier.start(currentPatch,workspacePatch);
			}
			else
			{
				skip = 1;
			}

			saveHandle.nextSampleFlag = 0;
		}
		else
		{
			skip =1;
		}
	}

	if((samplesCopyier.getState() == 0) || skip)
	{
		saveHandle.nextSampleFlag = 1;
		saveHandle.currSampleIdx++;
	}

	if(saveHandle.currSampleIdx == INSTRUMENTS_COUNT)
	{
		moveToNextStage(&saveHandle);
	}
}

void FileManager::saveEeprom()
{
	strcpy(mtConfig.startup.lastProjectName, currentProjectName);
	forceSaveConfig();
	moveToNextStage(&saveHandle);
}

void FileManager::finishSaving()
{
	char currentPatch[PATCH_SIZE];
	savingInProgress = 0;

	sprintf(currentPatch,"%s/project.bin", currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);

	strcpy(currentPatch,"Workspace/project.bin");
	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);
}

void FileManager::refreshSaveProject()
{
	switch(saveHandle.stage)
	{
	case sSavePatterns:
		refreshSavePatterns();
		break;
	case sSaveInstrumentFiles:
		refreshSaveInstrumentFiles();
		break;
	case sSaveSamples:
		refreshSaveSamples();
		break;
	case sSaveEppromConfig:
		saveEeprom();
		break;
	case sSavingDone:
		finishSaving();
		break;
	default:
		break;
	}
}

void FileManager::startSaveProject()
{
	memset(&saveHandle,0,sizeof(save_load_handle_t));
	savingInProgress = 1;
	saveHandle.nextSampleFlag = 1;
	char currentPatch[PATCH_SIZE];

	if(fileManager.patternIsChangedFlag[mtProject.values.actualPattern] == 1)
	{
		savePattern(mtProject.values.actualPattern);// zapisanie workspace
	}

	if(!SD.exists("Projects"))
	{
		SD.mkdir(0,"Projects");
	}

	if(!SD.exists(currentProjectPatch))
	{
		SD.mkdir(0,currentProjectPatch);
	}

	sprintf(currentPatch,"%s/instruments", currentProjectPatch);

	if(!SD.exists(currentPatch))
	{
		SD.mkdir(0,currentPatch);
	}

	sprintf(currentPatch,"%s/patterns", currentProjectPatch);
	if(!SD.exists(currentPatch))
	{
		SD.mkdir(0,currentPatch);
	}

	sprintf(currentPatch,"%s/samples", currentProjectPatch);
	if(!SD.exists(currentPatch))
	{
		SD.mkdir(0,currentPatch);
	}

	moveToNextStage(&saveHandle);
}

void FileManager::createEmptyTemplateProject(char * name)
{
	char patchFolder[PATCH_SIZE];

	memset(mtProject.mtProjectRemote.song.playlist,0,SONG_MAX);
	mtProject.mtProjectRemote.song.playlist[0]=0;

	sprintf(currentProjectPatch,"Templates/%s",name);

	if(!SD.exists("Projects")) SD.mkdir(0,"Projects");
	if(!SD.exists("Templates")) SD.mkdir(1,"Templates");
	if(!SD.exists(currentProjectPatch)) SD.mkdir(0,currentProjectPatch);

	sprintf(patchFolder,"Templates/%s/instruments", name);
	SD.mkdir(1,patchFolder);

	sprintf(patchFolder,"Templates/%s/patterns", name);
	SD.mkdir(1,patchFolder);

	sprintf(patchFolder,"Templates/%s/samples", name);
	SD.mkdir(1,patchFolder);

	mtProject.patterns_count++;
	mtProject.values.actualPattern = 1;
	savePattern(mtProject.values.actualPattern);

	sprintf(patchFolder,"Templates/%s/project.bin", name);

	writeProjectFile(patchFolder, &mtProject.mtProjectRemote);
}

//<! Loading only neccessary files from workspace functions
void FileManager::refreshLoadingInstrToMemFromWorkspace()
{
	if(loadFromWorkspaceHandle.forcedInstrReload == 0)
	{
		samplesLoader.start(0,(char*)"Workspace");
		loadFromWorkspaceHandle.forcedInstrReload = 1;
	}

	if(samplesLoader.getStateFlag() != loaderStateTypeInProgress)
	{
		moveToNextStage(&loadFromWorkspaceHandle);
	}
}

void FileManager::refreshLoadInstrFilesFromWorkspace()
{
	char currentPatch[PATCH_SIZE];
	uint8_t index = loadFromWorkspaceHandle.currInstrumentFileIdx;
	sprintf(currentPatch, "Workspace/instruments/instrument_%02d.mti", index);

	if(SD.exists(currentPatch))
	{
		readInstrumentFile(currentPatch,&mtProject.instrument[index]);
	}
	else
	{
		memset(mtProject.instrument[index].sample.file_name,0, SAMPLE_NAME_SIZE);
		mtProject.instrument[index].isActive = 0;
	}

	loadFromWorkspaceHandle.currInstrumentFileIdx++;

	if(loadFromWorkspaceHandle.currInstrumentFileIdx == INSTRUMENTS_COUNT)
	{
		moveToNextStage(&loadFromWorkspaceHandle);
	}
}

void FileManager::refreshLoadProjectFromWorkspace()
{
	switch(loadFromWorkspaceHandle.stage)
	{
	case sWspLoadInstumentFiles:
		refreshLoadInstrFilesFromWorkspace();
		break;
	case sWspLoadInstrumentsToMem:
		refreshLoadingInstrToMemFromWorkspace();
		break;
	case sWspLoadDone:
		loadingInProgress = 0;
		break;
	default:
		break;
	}
}

uint8_t FileManager::loadProjectFromWorkspaceStart()
{
	uint8_t status;
	char currentPatch[PATCH_SIZE];

	strcpy(currentPatch,"Workspace/project.bin");

	status = readProjectFile(currentPatch, &mtProject.mtProjectRemote);
	if(!status) return status;

	if(loadPattern(mtProject.values.actualPattern))
	{
		sequencer.switchRamPatternsNow();
	}

	memset(&loadFromWorkspaceHandle, 0, sizeof(save_load_handle_t));
	moveToNextStage(&loadFromWorkspaceHandle);
	loadingInProgress = 1;

	mtProject.used_memory = 0;

	return status;
}

void FileManager::autoSaveProject()
{
	char currentPatch[PATCH_SIZE];
	strcpy(currentPatch,"Workspace/project.bin");

	writeProjectFile(currentPatch,&mtProject.mtProjectRemote);
}
