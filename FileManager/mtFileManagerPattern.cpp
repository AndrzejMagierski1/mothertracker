#include "mtFileManager.h"


uint8_t FileManager::loadPattern(uint8_t index)
{

	char patternToLoad[PATCH_SIZE] { 0 };

	sprintf(patternToLoad, "%.200s%spattern_%02d.mtp", currentProjectPatch, "/patterns/",
			index);
	mtProject.values.actualPattern = index;

	uint8_t status = readPatternFile(patternToLoad);
	if (status)
	{
		mtProject.mtProjectRemote.patternFile[index].isActive = 1;
	}
	return status;
}

uint8_t FileManager::savePattern(uint8_t index)
{

	char patternToSave[PATCH_SIZE] { 0 };

	sprintf(patternToSave, "%.200s%spattern_%02d.mtp", currentProjectPatch, "/patterns/",
			index);
	mtProject.values.actualPattern = index;
	return writePatternFile(patternToSave);
}

void FileManager::importPatternToProject(char* filePatch, char* name, int8_t index)
{
//	TODO: funkcja do przebudowy albo wywalenia
	return;

	char currentPatch[PATCH_SIZE];
	FsFile file;
	FsFile copy;
	uint8_t currentBuf[1024];
	uint16_t lengthData=0;



	mtProject.mtProjectRemote.patternFile[index].isActive=1;
//	strcpy(mtProject.mtProjectRemote.patternFile[cnt].name,"pattern_00.mtp");
//	mtProject.mtProjectRemote.patternFile[cnt].name[11] = ((index-index%10)/10) + 48;
//	mtProject.mtProjectRemote.patternFile[cnt].name[12] = index%10 + 48;

	if(filePatch!= NULL)
	{
		sprintf(currentPatch,"%s/%s",filePatch,name);
	}
	else strcpy(currentPatch,name);

	file = SD.open(currentPatch);

	sprintf(currentPatch,"%s/patterns/",currentProjectPatch);
//	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);
	if(SD.exists(currentPatch)) SD.remove(currentPatch);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		lengthData=file.read(currentBuf,1024);
		copy.write(currentBuf,(size_t)lengthData);
	}
	file.close();
	copy.close();

	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);
	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

void FileManager::copyPattern(char* srcProjectPatch, char* srcName, char * dstProjectPatch, char* dstName)
{
	FsFile file;
	FsFile copy;
	char currentPatch [PATCH_SIZE];
	uint8_t currentBuffor[1024];
	uint16_t lengthData=0;

	sprintf(currentPatch,"%s/patterns/%s",srcProjectPatch,srcName);

	if(!SD.exists(currentPatch)) return;
	file = SD.open(currentPatch);

	sprintf(currentPatch,"%s/patterns/%s",dstProjectPatch,dstName);

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

void FileManager:: addPatternToProject (int8_t index)
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


	for(uint8_t i=0;i<SONG_MAX; i++)
	{
		if(i == index) mtProject.mtProjectRemote.song.playlist[i] = 0;
	}

	sprintf(currentPatch,"%s/patterns/",currentProjectPatch);
//	strcat(currentPatch,mtProject.mtProjectRemote.patternFile[cnt].name);

	if(SD.exists(currentPatch)) SD.remove(currentPatch);

	mtProject.mtProjectRemote.patternFile[index].isActive=-1;
//	memset(mtProject.mtProjectRemote.patternFile[cnt].name,0,PATTERN_NAME_SIZE);

	mtProject.patterns_count--;

	sprintf(currentPatch,"%s/project.bin",currentProjectPatch);

	writeProjectFile(currentPatch, &mtProject.mtProjectRemote);
}

uint8_t FileManager::getNextSongPattern()
{
	strMtProjectRemote::strSong *song = &mtProject.mtProjectRemote.song;
	return song->playlist[song->playlistPos + 1] > 0 ? song->playlist[song->playlistPos + 1] : song->playlist[0];
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




