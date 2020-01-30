#include "mtFileManager.h"
#include "sdram.h"
#include "core/interfacePopups.h"
#include "mtCommonBuffer.h"


uint8_t patternToLoad = 0;
//__NOINIT(EXTERNAL_RAM) uint8_t undo_Bank[1024*1024];
#define UNDO_CAPACITY 20
#define UNDO_SONG_CAPACITY 5
__NOINIT(EXTERNAL_RAM) Sequencer::strPattern undoPatternBuffer[UNDO_CAPACITY] { 0 };
__NOINIT(EXTERNAL_RAM) uint8_t undoPatternBufferIndexes[UNDO_CAPACITY] { 0 };

__NOINIT(EXTERNAL_RAM) Sequencer::strPattern undoSongBuffer[UNDO_SONG_CAPACITY] { 0 };
__NOINIT(EXTERNAL_RAM) uint8_t undoSongBufferIndexes[UNDO_SONG_CAPACITY] { 0 };

__NOINIT(EXTERNAL_RAM) Sequencer::strPattern songTrackCopy[2] {0};

struct strUndo
{
	uint8_t actualIndex = 0; // gdzie jesteśmy w tablicy
	uint8_t storedCount = 0; // ile razy zrzuciliśmy
	uint8_t redoPossibility = 0; // ile razy zrzuciliśmy
	strPopupStyleConfig popupConfig {
			2,					// time
			800 / 2 - 150,		// x
			480 / 2 - 50,		// y
			300,				// w
			100,				// h
			0xffffff,			// lineColor[4];
			0xffffff,
			0xffffff,
			0xffffff,
			controlStyleCenterX,//lineStyle[4];
			controlStyleCenterX,
			controlStyleCenterX,
			controlStyleCenterX };

} undo;

struct strUndoSong
{
	uint8_t actualIndex = 0; // gdzie jesteśmy w tablicy
	uint8_t storedCount = 0; // ile razy zrzuciliśmy
	uint8_t redoPossibility = 0; // ile razy zrzuciliśmy


} undoSong;

uint8_t FileManager::loadPattern(uint8_t index)
{
	uint8_t status = 0;
	char patternToLoad[PATCH_SIZE] { 0 };

	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", index);
	mtProject.values.actualPattern = index;

	if(!SD.exists(patternToLoad))
	{
		status = 0;
	}
	else
	{
		status = readPatternFile(patternToLoad, sequencer.getPatternToLoadFromFile());
	}

	if(status)
	{
		sequencer.loadFromFileOK();
	}
	else
	{
		sequencer.loadFromFileERROR();
	}

	return status;
}

uint8_t FileManager::loadTrack(uint8_t pattIndex, uint8_t trackIndex)
{
	trackIndex = constrain(trackIndex, 0, 7);

	char patternToLoad[PATCH_SIZE] { 0 };
	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", pattIndex);



	Sequencer::strPattern * patternFrom = (Sequencer::strPattern*) sequencer.getPatternToLoadFromFile();
	Sequencer::strPattern * patternTo = sequencer.getActualPattern();



	if (SD.exists(patternToLoad))
	{
		readPatternFile(patternToLoad, sequencer.getPatternToLoadFromFile());
	}
	else
	{
		sequencer.clearPattern(patternFrom);
	}



	patternTo->track[trackIndex] = patternFrom->track[trackIndex];

	mtProject.values.perfTracksPatterns[trackIndex] = pattIndex;

	return 1;
}

void FileManager::setLoadPattern(uint8_t index)
{
	patternToLoad = index;
}

//void FileManager::setTrackToLoad(uint8_t pattIndex, uint8_t trackIndex)
//{
//
//}

void FileManager::handleLoadPattern()
{
	if (patternToLoad)
	{
		savePattern(mtProject.values.actualPattern);
		loadPattern(patternToLoad);
		patternToLoad = 0;
	}
}


uint8_t FileManager::savePattern(uint8_t index)
{
	char patternToSave[PATCH_SIZE] { 0 };
	uint8_t status = 0;
	if(fileManager.patternIsChangedFlag[index] == 1)
	{
		fileManager.patternIsChangedFlag[index] = 0;
		sprintf(patternToSave, "Workspace/patterns/pattern_%02d.mtp", index);
		status = writePatternFile(patternToSave, sequencer.getPatternToSaveToFile());
		sequencer.saveToFileDone();
	}

	return status;
}

void FileManager::storePatternUndoRevision()
{
	undoPatternBuffer[undo.actualIndex] = *sequencer.getActualPattern();
	undoPatternBufferIndexes[undo.actualIndex] = mtProject.values.actualPattern;


	undo.redoPossibility = 0;
	if (undo.actualIndex >= UNDO_CAPACITY)
	{
		undo.actualIndex = 0;
		undo.storedCount++;
	}
	else
	{
		undo.actualIndex++;
		undo.storedCount++;
	}
	if (undo.storedCount > UNDO_CAPACITY) undo.storedCount = UNDO_CAPACITY;

//	Serial.printf(
//			">>>pattern stored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//			undo.actualIndex,
//			undo.storedCount,
//			undo.redoPossibility);

}

void FileManager::storeSongUndoRevision(uint8_t index)
{
//	undoSongBuffer[undoSong.actualIndex] = *sequencer.getActualPattern(); //todo: pobrać z pliku o indexie




	char patternToLoad[PATCH_SIZE] { 0 };
	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", index);
	fileManager.readPatternFile(
			patternToLoad, (uint8_t*) &undoSongBuffer[undoSong.actualIndex]);

	undoSongBufferIndexes[undoSong.actualIndex] = index;





	undoSong.redoPossibility = 0;
	if (undoSong.actualIndex >= UNDO_SONG_CAPACITY)
	{
		undoSong.actualIndex = 0;
		undoSong.storedCount++;
	}
	else
	{
		undoSong.actualIndex++;
		undoSong.storedCount++;
	}
	if (undoSong.storedCount > UNDO_SONG_CAPACITY) undoSong.storedCount = UNDO_SONG_CAPACITY;

//	Serial.printf(
//			">>>pattern stored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//			undoSong.actualIndex,
//			undoSong.storedCount,
//			undoSong.redoPossibility);

}



void FileManager::undoSongPattern()
{
	bool doUndo = 0;
//	uint8_t oldIndex = undoSong.actualIndex;
	if (undoSong.actualIndex > 0 && undoSong.storedCount > 0)
	{
		undoSong.actualIndex--;
		undoSong.storedCount--;
		doUndo = 1;
	}
	else if (undoSong.actualIndex == 0 && undoSong.storedCount > 0)
	{
		undoSong.actualIndex = UNDO_SONG_CAPACITY - 1;
		doUndo = 1;
	}
	else
	{
		mtPopups.config(0, &undo.popupConfig);
		mtPopups.show(0, "No undo data :(");
	}

	if (doUndo)
	{

		char filePath[PATCH_SIZE] { 0 };
		uint8_t bufferedPatternNumber = undoSongBufferIndexes[undoSong.actualIndex];
		sprintf(filePath, "Workspace/patterns/pattern_%02d.mtp", bufferedPatternNumber);

		fileManager.writePatternFile(
				filePath,
				(uint8_t*) &undoSongBuffer[undoSong.actualIndex]);


		updatePatternBitmask(bufferedPatternNumber-1, &undoSongBuffer[undoSong.actualIndex]);

		undoSong.redoPossibility++;
//		Serial.printf(
//				"<<<pattern restored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undoSong.actualIndex,
//				undoSong.storedCount,
//				undoSong.redoPossibility);
	}
}
void FileManager::undoPattern()
{
	bool doUndo = 0;
	uint8_t oldIndex = undo.actualIndex;
	if (undo.actualIndex > 0 && undo.storedCount > 0)
	{
		undo.actualIndex--;
		undo.storedCount--;
		doUndo = 1;
	}
	else if (undo.actualIndex == 0 && undo.storedCount > 0)
	{
		undo.actualIndex = UNDO_CAPACITY - 1;
		doUndo = 1;
	}
	else
	{
//		Serial.printf("no more undo data\n");

		mtPopups.config(0, &undo.popupConfig);
		mtPopups.show(0, "No undo data :(");
	}

	if (doUndo)
	{

		undoPatternBuffer[oldIndex] = *sequencer.getActualPattern();

		*sequencer.getActualPattern() = undoPatternBuffer[undo.actualIndex];
		mtProject.values.actualPattern = undoPatternBufferIndexes[undo.actualIndex];
		setPatternChangeFlag(mtProject.values.actualPattern);

		undo.redoPossibility++;
//		Serial.printf(
//				"<<<pattern restored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undo.actualIndex,
//				undo.storedCount,
//				undo.redoPossibility);
	}
}
void FileManager::redoPattern()
{
	bool doRedo = 0;
	if (undo.actualIndex < UNDO_CAPACITY - 1 && undo.redoPossibility > 0)
	{
		undo.actualIndex++;
		undo.storedCount++;
		doRedo = 1;
	}
	else if (undo.actualIndex == UNDO_CAPACITY - 1 && undo.redoPossibility > 0)
	{
		undo.actualIndex = 0;
		doRedo = 1;
	}
	else
	{
//		Serial.printf("no more redo possibility\n");
		mtPopups.config(0, &undo.popupConfig);
		mtPopups.show(0, "No redo data :(");
	}

	if (doRedo)
	{
		undo.redoPossibility--;
		*sequencer.getActualPattern() = undoPatternBuffer[undo.actualIndex];
		mtProject.values.actualPattern = undoPatternBufferIndexes[undo.actualIndex];
		setPatternChangeFlag(mtProject.values.actualPattern);

//		Serial.printf(
//				">>>pattern redo\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undo.actualIndex,
//				undo.storedCount,
//				undo.redoPossibility);
	}

}

#if 0
void FileManager::importPatternToProject(char* filePatch, char* name,
											int8_t index)
{
//	TODO: funkcja do przebudowy albo wywalenia
	return;

	char currentPatch[PATCH_SIZE];
	FsFile file;
	FsFile copy;
	uint8_t currentBuf[1024];
	uint16_t lengthData = 0;


//	strcpy(mtProject.mtProjectRemote.patternFile[cnt].name,"pattern_00.mtp");
//	mtProject.mtProjectRemote.patternFile[cnt].name[11] = ((index-index%10)/10) + 48;
//	mtProject.mtProjectRemote.patternFile[cnt].name[12] = index%10 + 48;

	if (filePatch != NULL)
	{
		sprintf(currentPatch, "%s/%s", filePatch, name);
	}
	else
		strcpy(currentPatch, name);

	file = SD.open(currentPatch);

	sprintf(currentPatch, "%s/patterns/", currentProjectPatch);
//	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);
	if (SD.exists(currentPatch)) SD.remove(currentPatch);
	copy = SD.open(currentPatch, FILE_WRITE);

	while (file.available())
	{
		lengthData = file.read(currentBuf, 1024);
		copy.write(currentBuf, (size_t) lengthData);
	}
	file.close();
	copy.close();

	sprintf(currentPatch, "%s/project.mt", currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}
#endif


void FileManager::copyPattern(char* srcProjectPatch, uint8_t src_idx, char * dstProjectPatch, uint8_t dst_idx)
{
	SdFile file;
	SdFile copy;
	char currentPatch[PATCH_SIZE];
	uint8_t *buf = getWriteLoadBufferPointer();
	uint16_t lengthData = 0;


	sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",srcProjectPatch,src_idx);

	if (!SD.exists(currentPatch)) return;
	file = SD.open(currentPatch);


	sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",dstProjectPatch,dst_idx);

	if (SD.exists(currentPatch)) SD.remove(currentPatch);
	copy = SD.open(currentPatch, FILE_WRITE);

	while (file.available())
	{
		lengthData = file.read(buf, COMMON_BUFFER_SIZE);
		copy.write(buf, (size_t) lengthData);
	}
	file.close();
	copy.close();
}

void FileManager::addPatternToProject(int8_t index)
{
//	char currentPatch[PATCH_SIZE];
//	uint8_t cnt=0;
//	while((mtProject.mtProjectRemote.patternFile[cnt].index != index) && (cnt < PATTERNS_COUNT) )
//	{
//		cnt++;
//	}
//	if(cnt != PATTERNS_COUNT)
//	{
//		mtProject.mtProjectRemote.patternFile[cnt].index=-1;
////		memset(mtProject.mtProjectRemote.patternFile[cnt].name,0,PATTERN_NAME_SIZE);
//	}
//	cnt=0;
//	while((mtProject.mtProjectRemote.patternFile[cnt].index != -1) && (cnt < PATTERNS_COUNT) )
//	{
//		cnt++;
//	}
//	mtProject.mtProjectRemote.patternFile[cnt].index=index;
//	strcpy(mtProject.mtProjectRemote.patternFile[cnt].name,"pattern_00.mti");
//	mtProject.mtProjectRemote.patternFile[cnt].name[8] = ((index-index%10)/10) + 48;
//	mtProject.mtProjectRemote.patternFile[cnt].name[9] = index%10 + 48;
//
//	mtProject.patterns_count++;
//
//	memset(currentPatch,0,PATCH_SIZE);
//	strcpy(currentPatch,currentProjectPatch);
//	strcat(currentPatch,"/patterns/");
//	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);
//
//	if(SD.exists(currentPatch)) SD.remove(currentPatch);
//
//	writePatternFile(currentPatch);
//
//	memset(currentPatch,0,PATCH_SIZE);
//	strcpy(currentPatch,currentProjectPatch);
//	strcat(currentPatch,"/project.mt");
//
//	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::deletePattern(int8_t index)
{
	char currentPatch[PATCH_SIZE];

	for (uint8_t i = 0; i < SONG_MAX; i++)
	{
		if (i == index) mtProject.song.playlist[i] = 0;
	}


	sprintf(currentPatch,"Workspace/patterns/pattern_%02d.mtp",index);


	if (SD.exists(currentPatch)) SD.remove(currentPatch);


	mtProject.patterns_count--;


	strcpy(currentPatch,"Workspace/project.mt");


	writeProjectFile(currentPatch, &mtProject);
}

uint8_t FileManager::getNextSongPattern()
{
	strSong *song = &mtProject.song;
	return song->playlist[song->playlistPos + 1] > 0 ? song->playlist[song->playlistPos + 1] : song->playlist[0];
}
uint8_t FileManager::getSongPattern(uint8_t pos)
{
	strSong *song = &mtProject.song;
	return song->playlist[pos];
}
void FileManager::setSongPos(uint8_t pos)
{
	strSong *song = &mtProject.song;
	song->playlistPos = pos;
}

uint8_t FileManager::resetToFirstSongPattern()
{
	strSong *song = &mtProject.song;
	song->playlistPos = 0;
	return song->playlist[0];
}

bool FileManager::switchNextPatternInSong()
{
	strSong *song = &mtProject.song;
	if (song->playlist[++(song->playlistPos)] != PLAYLIST_EMPTY_SLOT)
	{
		return 1;
	}
	else
	{
		song->playlistPos = 0;
		return 0;
	}

	return 0;
}

void FileManager::copySongTracks(char *currentProjectPath, uint8_t src, uint8_t dest, uint8_t trackStartSrc, uint8_t trackStartDest, uint8_t tracksNum)
{
	uint8_t status = 0;
	SdFile srcFile;
	SdFile destFile;
	char currentPath[PATCH_SIZE];

	sprintf(currentPath, "%s/patterns/pattern_%02d.mtp", currentProjectPath, src);
	status = readPatternFile(currentPath, (uint8_t*)&songTrackCopy[0]);

	if(status)
	{
		sprintf(currentPath, "%s/patterns/pattern_%02d.mtp", currentProjectPath, dest);
		status = readPatternFile(currentPath, (uint8_t*)&songTrackCopy[1]);
		if(status == 0)
		{
			sequencer.clearPattern(&songTrackCopy[1]);
		}

		while((trackStartSrc + tracksNum) > 8 ||(trackStartDest + tracksNum) > 8 )
		{
			tracksNum--;
		}

		for(uint8_t track = 0; track < tracksNum; track++)
		{
			if(&songTrackCopy[1].track[trackStartDest + track] != &songTrackCopy[0].track[trackStartSrc + track]) // memcpy restrict protection
			{
				memcpy(&songTrackCopy[1].track[trackStartDest + track], &songTrackCopy[0].track[trackStartSrc + track], sizeof(songTrackCopy[0].track[0]));
			}
		}

		writePatternFile(currentPath, (uint8_t*)&songTrackCopy[1]);
	}

	srcFile.close();
	destFile.close();
}

void FileManager::deleteTracks(char *currentProjectPath, uint8_t src, uint8_t trackStartSrc, uint8_t tracksNum)
{
	uint8_t status = 0;
	SdFile srcFile;
	char currentPath[PATCH_SIZE];

	sprintf(currentPath, "%s/patterns/pattern_%02d.mtp", currentProjectPath, src);
	status = readPatternFile(currentPath, (uint8_t*)&songTrackCopy[0]);

	if(status)
	{
		for(uint8_t track = 0; track < tracksNum; track++)
		{
			sequencer.clearSingleTrack(&songTrackCopy[0].track[track+trackStartSrc]);
		}

		writePatternFile(currentPath, (uint8_t*)&songTrackCopy[0]);
	}

	srcFile.close();
}

void FileManager::updatePatternBitmask(uint8_t patternNum)
{

	updatePatternBitmask(
			patternNum - 1,
			(Sequencer::strPattern*) sequencer.getPatternToSaveToFile());

}
void FileManager::updatePatternBitmask(uint8_t index, Sequencer::strPattern *sourcePattern)
{
	/*Update pattern usage bitmask*/

	for(uint8_t trackNum = 0; trackNum < 8; trackNum++)
	{
		uint8_t isAnyActive = 0;
		uint8_t length = sourcePattern->track[trackNum].length;

		for(uint8_t step = 0; step < length; step++)
		{
			if(sourcePattern->track[trackNum].step[step].note >= 0)
			{
				isAnyActive = 1;
				mtProject.values.allPatternsBitmask[index] |= (1 << trackNum);
				break;
			}
		}

		if(isAnyActive == 0)
		{
			mtProject.values.allPatternsBitmask[index] &= ~(1 << trackNum);
		}
	}
}

void FileManager::setPatternChangeFlag(uint8_t num)
{
	patternIsChangedFlag[num] = 1;
	mtProject.values.projectNotSavedFlag = 1;
	mtProject.values.patternsToSave[num] = 1;
	configIsChangedFlag = 1;

	updatePatternBitmask(num);
}

void FileManager::setInstrumentChangeFlag(uint8_t num)
{

	instrumentIsChangedFlag[num] = 1;
	mtProject.values.projectNotSavedFlag = 1;
	mtProject.values.instrumentsToSave[num] = 1;
	configIsChangedFlag = 1;

}

/*void FileManager::setInstrumentChangeFlag()
{
	fileManager.instrumentIsChangedFlag[mtProject.values.a] = 1;
	mtProject.values.projectNotSavedFlag = 1;
}*/


