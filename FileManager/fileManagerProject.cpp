

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
#include "mtFileManagerOldVersionRecovery.h"

__NOINIT(EXTERNAL_RAM) strProjectFile fileManagerProjectBuffer {0};




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
	// sprawdza czy zaktualizowac patternBitmask
	if(changesFlags.pattern[mtProject.values.actualPattern] == 1)
	{
		if(updatePatternBitmask(mtProject.values.actualPattern))
		{
			// jesli struktura bitow sie zminila wymus zawsze aktualizcje projektu
			changesFlags.project = 1;
		}
	}

	if(changesFlags.project == 0) //tylko jesli flaga zmian ustawiona
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
		clearProjectChangedFlag();
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
		debugLog.addLine("older version of project file opened");

		versionRecovery.translateProject(pFile->projectDataAndHeader.projectHeader.fileStructureVersion[0],
				PROJECT_FILE_VERSION, &pFile->projectDataAndHeader.project);

	}
	else if(pFile->projectDataAndHeader.projectHeader.fileStructureVersion[0] > PROJECT_FILE_VERSION)
	{
		//debugLog.setMaxLineCount(2);
		debugLog.addLine("newer version of project file opened");
		//todo: informacja do interfejsu zeby wyswietlic popup
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
			// tu przepisuje nazwe projektu odczytana z pliku projektu, pozniej jest to itak weryfikowane
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






void cFileManager::getDefaultProject(struct strMtProject *source)
{
	source->instruments_count = 0;
	source->patterns_count = 0;
	source->max_memory = SAMPLE_MEMORY_MAX;
	source->used_memory = 0;

	for(uint32_t i = 0; i < INSTRUMENTS_COUNT; i++)
	{
		getEmptyInstrument(&source->instrument[i]);
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

//	source->reverbRoomSize = 80;
//	source->reverbDamping = 25;
//	source->reverbPanning = 0;

	source->delayFeedback = 50;
	source->delayTime = 500;
	source->delayParams = 0;

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
