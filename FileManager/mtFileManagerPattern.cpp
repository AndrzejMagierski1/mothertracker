#include "mtFileManager.h"

uint8_t patternToLoad = 0;
uint8_t FileManager::loadPattern(uint8_t index)
{

	char patternToLoad[PATCH_SIZE] { 0 };


	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", index);
	mtProject.values.actualPattern = index;

	uint8_t status = readPatternFile(patternToLoad);
	return status;
}


uint8_t FileManager::setLoadPattern(uint8_t index)
{
	patternToLoad = index;
}

uint8_t FileManager::handleLoadPattern()
{
	if (patternToLoad)
	{
		loadPattern(patternToLoad);
		patternToLoad = 0;
	}
}


uint8_t FileManager::savePattern(uint8_t index)
{
	char patternToSave[PATCH_SIZE] { 0 };

	sprintf(patternToSave, "Workspace/patterns/pattern_%02d.mtp", index);
	mtProject.values.actualPattern = index;
	return writePatternFile(patternToSave);
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
	uint8_t currentBuffor[1024];
	uint16_t lengthData = 0;


	sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",srcProjectPatch,src_idx);

	if (!SD.exists(currentPatch)) return;
	file = SD.open(currentPatch);


	sprintf(currentPatch,"%s/patterns/pattern_%02d.mtp",dstProjectPatch,dst_idx);

	if (SD.exists(currentPatch)) SD.remove(currentPatch);
	copy = SD.open(currentPatch, FILE_WRITE);

	while (file.available())
	{
		lengthData = file.read(currentBuffor, 1024);
		copy.write(currentBuffor, (size_t) lengthData);
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

