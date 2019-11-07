#include "mtFileManager.h"
#include "sdram.h"
#include "interfacePopups.h"
#include "mtCommonBuffer.h"


uint8_t patternToLoad = 0;
//__NOINIT(EXTERNAL_RAM) uint8_t undo_Bank[1024*1024];
#define UNDO_CAPACITY 20
__NOINIT(EXTERNAL_RAM) Sequencer::strPattern undoPatternBuffer[UNDO_CAPACITY] { 0 };
__NOINIT(EXTERNAL_RAM) uint8_t undoPatternBufferIndexes[UNDO_CAPACITY] { 0 };

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

uint8_t FileManager::loadPattern(uint8_t index)
{

	char patternToLoad[PATCH_SIZE] { 0 };


	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", index);
	mtProject.values.actualPattern = index;

	uint8_t status = readPatternFile(patternToLoad);
	return status;
}
uint8_t FileManager::loadTrack(uint8_t pattIndex, uint8_t trackIndex)
{
	trackIndex = constrain(trackIndex, 0, 7);

	char patternToLoad[PATCH_SIZE] { 0 };
	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", pattIndex);
	readPatternFile(patternToLoad);

	Sequencer::strPattern * patternFrom = (Sequencer::strPattern*) sequencer.getPatternToLoadFromFile();
	Sequencer::strPattern * patternTo = sequencer.getActualPattern();

	patternTo->track[trackIndex] = patternFrom->track[trackIndex];

	return 1;
}

void FileManager::setLoadPattern(uint8_t index)
{
	patternToLoad = index;
}

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

	fileManager.patternIsChangedFlag[index] = 0;

	sprintf(patternToSave, "Workspace/patterns/pattern_%02d.mtp", index);
	mtProject.values.actualPattern = index;// to raczej nie jest potrzebne
	return writePatternFile(patternToSave);
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
void FileManager::undoPattern()
{
	bool doUndo = 0;
	uint8_t oldIndex = undo.actualIndex;
	if (undo.actualIndex > 0)
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
		setPatternChangeFlag();

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
		setPatternChangeFlag();

//		Serial.printf(
//				">>>pattern redo\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undo.actualIndex,
//				undo.storedCount,
//				undo.redoPossibility);
	}

}

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

	sprintf(currentPatch, "%s/project.bin", currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}


void FileManager::copyPattern(char* srcProjectPatch, uint8_t src_idx, char * dstProjectPatch, uint8_t dst_idx)
{
	FsFile file;
	FsFile copy;
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
//	strcat(currentPatch,"/project.bin");
//
//	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::deletePattern(int8_t index)
{
	char currentPatch[PATCH_SIZE];

	for (uint8_t i = 0; i < SONG_MAX; i++)
	{
		if (i == index) mtProject.mtProjectRemote.song.playlist[i] = 0;
	}


	sprintf(currentPatch,"Workspace/patterns/pattern_%02d.mtp",index);


	if (SD.exists(currentPatch)) SD.remove(currentPatch);


	mtProject.patterns_count--;


	strcpy(currentPatch,"Workspace/project.bin");


	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

uint8_t FileManager::getNextSongPattern()
{
	strMtProjectRemote::strSong *song = &mtProject.mtProjectRemote.song;
	return song->playlist[song->playlistPos + 1] > 0 ? song->playlist[song->playlistPos + 1] : song->playlist[0];
}
uint8_t FileManager::getSongPattern(uint8_t pos)
{
	strMtProjectRemote::strSong *song = &mtProject.mtProjectRemote.song;
	return song->playlist[pos];
}
void FileManager::setSongPos(uint8_t pos)
{
	strMtProjectRemote::strSong *song = &mtProject.mtProjectRemote.song;
	song->playlistPos = pos;
}

uint8_t FileManager::resetToFirstSongPattern()
{
	strMtProjectRemote::strSong *song = &mtProject.mtProjectRemote.song;
	song->playlistPos = 0;
	return song->playlist[0];
}

void FileManager::switchNextPatternInSong()
{
	strMtProjectRemote::strSong *song = &mtProject.mtProjectRemote.song;
	if (song->playlist[++(song->playlistPos)] != PLAYLIST_EMPTY_SLOT)
	{

	}
	else
	{
		song->playlistPos = 0;
	}
}


void FileManager::setPatternChangeFlag()
{
	fileManager.patternRefresh = 0;
	fileManager.patternIsChangedFlag[mtProject.values.actualPattern] = 1;
	mtProject.values.projectNotSavedFlag = 1;
}

/*void FileManager::setInstrumentChangeFlag()
{
	fileManager.instrumentIsChangedFlag[mtProject.values.a] = 1;
	mtProject.values.projectNotSavedFlag = 1;
}*/


