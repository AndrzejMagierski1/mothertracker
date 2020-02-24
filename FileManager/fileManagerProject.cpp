

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

#include "projectEditor/projectEditor.h"

#include "fileTransfer.h"
#include "fileManager.h"


__NOINIT(EXTERNAL_RAM) strProjectFile fileManagerProjectBuffer  {0};




//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     LOAD     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::loadProjectFileFromWorkspace()
{
	uint8_t loadStatus = fileTransfer.loadFileToMemory(cProjectFileNameInWorkspace, (uint8_t*)&fileManagerProjectBuffer, sizeof(strProjectFile), fileWholeOnce); // fileDivIntoParts

	if(loadStatus == fileTransferEnd)
	{
		if(loadProjectFileFormFileStruct(&mtProject, &fileManagerProjectBuffer))
		{
			projectEditor.loadProjectValues();
			moveToNextOperationStep();
		}
		else
		{
			throwError(0);
		}
	}
	else if(loadStatus >= fileTransferError)
	{
		throwError(1);
	}
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     COPY     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::copyProjectFile()
{
	if(currentOperation == fmCopyWorkspaceToProjects)
	{
		strcpy(currentCopySrcPath, cProjectFileNameInWorkspace);
		sprintf(currentCopyDestPath, cProjectFileInProjectsFormat, currentProjectName);
	}
	else
	{
		sprintf(currentCopySrcPath, cProjectFileInProjectsFormat, currentProjectName);
		strcpy(currentCopyDestPath, cProjectFileNameInWorkspace);
	}

	uint8_t loadStatus = fileTransfer.copyFile(currentCopySrcPath, currentCopyDestPath);

	if(loadStatus == fileTransferEnd)
	{
		moveToNextOperationStep();
	}
	else if(loadStatus >= fileTransferError)
	{
		throwError(0);
	}
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     SAVE     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void cFileManager::saveProjectFileToWorkspace()
{
	if(chengesFlags.project == 0) //tylko jesli flaga zmian ustawiona
	{
		moveToNextOperationStep();
		return;
	}

	if(!writeProjectFileToFileStruct(&mtProject, &fileManagerProjectBuffer))
	{
		throwError(0);
	}

	uint8_t saveStatus = fileTransfer.saveMemoryToFile((uint8_t*)&fileManagerProjectBuffer, cProjectFileNameInWorkspace, sizeof(strProjectFile));

	if(saveStatus == fileTransferEnd)
	{
		moveToNextOperationStep();
	}
	else// if(saveStatus >= fileTransferError)
	{
		throwError(1);
	}
}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------     xxxx     -----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool cFileManager::loadProjectFileFormFileStruct(strMtProject* project, strProjectFile* pFile)
{
	FastCRC32 crcCalc;

	if (pFile->projectDataAndHeader.projectHeader.type != fileTypeProject)
		return false;

	uint32_t checkCRC = crcCalc.crc32((uint8_t *) &pFile->projectDataAndHeader,
								sizeof(pFile->projectDataAndHeader));

	if (FILEMANAGER_DEBUGLOG)
	{
		char line[44];
		sprintf(line,
				"opened project fw.ver: %u.%u.%u, crc %s",
				pFile->projectDataAndHeader.projectHeader.fwVersion[0],
				pFile->projectDataAndHeader.projectHeader.fwVersion[1],
				pFile->projectDataAndHeader.projectHeader.fwVersion[2],
				checkCRC == pFile->crc ? "ok" : "err"
						);
		debugLog.addLine(line);
	}

	if(pFile->projectDataAndHeader.projectHeader.fileStructureVersion[0] < PROJECT_FILE_VERSION)
	{
		debugLog.setMaxLineCount(2);
		debugLog.addLine("older version of project file opened");
	}
	else if(pFile->projectDataAndHeader.projectHeader.fileStructureVersion[0] > PROJECT_FILE_VERSION)
	{
		debugLog.setMaxLineCount(2);
		debugLog.addLine("newer version of project file opened");
	}


	if(1)
	{
		if(pFile->projectDataAndHeader.project.projectName[0] == 0)
		{
			// brak nazwy w pliku projektu
			// - jesli projekt czytany z workspace to nie mozna powiazac projektu z projektem w folderze Projects
			// - jeslie projekt czytany z workpace ale po kopiowaniu z folderu Projects
			//   to nazwa zostanie mu w dalszej czasci ladowania wpisana
			//

			projectNamefromProjectFile[0] = 0;
		}
		else
		{
			// tu przepisuje nazwe projektu odczytana z pliku projektu, pozniej jesy to itak weryfikowane
			pFile->projectDataAndHeader.project.projectName[PROJECT_NAME_SIZE-1] = 0;
			strcpy(projectNamefromProjectFile, pFile->projectDataAndHeader.project.projectName);
		}

		memcpy(&project->song, &pFile->projectDataAndHeader.project.song, sizeof(strSong));
		memcpy(&project->values, &pFile->projectDataAndHeader.project.values, sizeof(strMtValues));
		//*proj=pFile.projectDataAndHeader.project;
		//mtProject.values=pFile.projectDataAndHeader.project.values;
		return true;
	}

	return false;
}


bool cFileManager::writeProjectFileToFileStruct(strMtProject* project, strProjectFile* pFile)
{
	FastCRC32 crcCalc;

	memcpy(&pFile->projectDataAndHeader.project.song, &project->song, sizeof(strSong));
	memcpy(&pFile->projectDataAndHeader.project.values, &project->values, sizeof(strMtValues));
	strcpy(pFile->projectDataAndHeader.project.projectName, currentProjectName);

	pFile->projectDataAndHeader.projectHeader.id_file[0]='M';
	pFile->projectDataAndHeader.projectHeader.id_file[1]='T';

	pFile->projectDataAndHeader.projectHeader.type = fileTypeProject;

	pFile->projectDataAndHeader.projectHeader.fwVersion[0] = FV_VER_1;
	pFile->projectDataAndHeader.projectHeader.fwVersion[1] = FV_VER_2;
	pFile->projectDataAndHeader.projectHeader.fwVersion[2] = FV_VER_3;
	pFile->projectDataAndHeader.projectHeader.fwVersion[3] = FV_VER_1;

	pFile->projectDataAndHeader.projectHeader.fileStructureVersion[0] = PROJECT_FILE_VERSION;
	pFile->projectDataAndHeader.projectHeader.fileStructureVersion[1] = PROJECT_FILE_VERSION;
	pFile->projectDataAndHeader.projectHeader.fileStructureVersion[2] = PROJECT_FILE_VERSION;
	pFile->projectDataAndHeader.projectHeader.fileStructureVersion[3] = PROJECT_FILE_VERSION;

	pFile->projectDataAndHeader.projectHeader.size = sizeof(strProjectFile);

	pFile->crc = crcCalc.crc32((uint8_t *)&pFile->projectDataAndHeader, sizeof(pFile->projectDataAndHeader));

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cFileManager::writeProjectFile(const char * name, strMtProject* proj)
{
	SD.remove(name);

	SdFile file;
	FastCRC32 crcCalc;
	strProjectFile projectFile;

	memcpy(&projectFile.projectDataAndHeader.project.song, &proj->song, sizeof(strSong));
	memcpy(&projectFile.projectDataAndHeader.project.values, &proj->values, sizeof(strMtValues));
	strcpy(projectFile.projectDataAndHeader.project.projectName, currentProjectName);

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

	file.open(name, FILE_WRITE);
	file.write((uint8_t *)&projectFile, sizeof(projectFile));
	file.close();
}


bool cFileManager::readProjectFile(const char * name, strMtProject * proj)
{
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
		return false;

	checkCRC = crcCalc.crc32((uint8_t *) &projectFile.projectDataAndHeader,
								sizeof(projectFile.projectDataAndHeader));

	if (FILEMANAGER_DEBUGLOG)
	{
		char line[44];
		sprintf(line,
				"opened project fw.ver: %u.%u.%u, crc %s",
				projectFile.projectDataAndHeader.projectHeader.fwVersion[0],
				projectFile.projectDataAndHeader.projectHeader.fwVersion[1],
				projectFile.projectDataAndHeader.projectHeader.fwVersion[2],
				checkCRC == projectFile.crc ? "ok" : "err"
						);
		debugLog.addLine(line);
	}


	if(1)
	{
		memcpy(&proj->song, &projectFile.projectDataAndHeader.project.song, sizeof(strSong));
		memcpy(&proj->values, &projectFile.projectDataAndHeader.project.values, sizeof(strMtValues));
		//*proj=projectFile.projectDataAndHeader.project;
		//mtProject.values=projectFile.projectDataAndHeader.project.values;
		return false;
	}
	else return true;
}






void cFileManager::getDefaultProject(struct strMtProject *source)
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


void cFileManager::getDefaultSong(struct strSong *source)
{
	source->playlistPos = 0;

	memset(source->playlist, 0, SONG_MAX);

	source->playlist[0] = 1;

}

void cFileManager::getDefaultInstrument(struct strInstrument *source)
{
	memset((uint8_t*)source, 0, sizeof(strInstrument));

	source->sample.wavetable_window_size = 2048;
}

void cFileManager::getDefaultValues(struct strMtValues *source)
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
