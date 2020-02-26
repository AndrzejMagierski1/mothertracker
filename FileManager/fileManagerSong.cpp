#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"
#include "core/interfacePopups.h"

#include "fileManager.h"


//uint8_t patternToLoad = 0;

//__NOINIT(EXTERNAL_RAM) uint8_t undo_Bank[1024*1024];
__NOINIT(EXTERNAL_RAM) Sequencer::strPattern songTrackCopy[2] {0};


uint8_t cFileManager::getNextSongPattern()
{
	strSong *song = &mtProject.song;
	return song->playlist[song->playlistPos + 1] > 0 ? song->playlist[song->playlistPos + 1] : song->playlist[0];
}
uint8_t cFileManager::getSongPattern(uint8_t pos)
{
	strSong *song = &mtProject.song;
	return song->playlist[pos];
}
void cFileManager::setSongPos(uint8_t pos)
{
	strSong *song = &mtProject.song;
	song->playlistPos = pos;
}

uint8_t cFileManager::resetToFirstSongPattern()
{
	strSong *song = &mtProject.song;
	song->playlistPos = 0;
	return song->playlist[0];
}

bool cFileManager::switchNextPatternInSong()
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


void cFileManager::copySongTracks(char *currentProjectPath, uint8_t src, uint8_t dest, uint8_t trackStartSrc, uint8_t trackStartDest, uint8_t tracksNum)
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

void cFileManager::deleteTracks(char *currentProjectPath, uint8_t src, uint8_t trackStartSrc, uint8_t tracksNum)
{
	uint8_t status = 0;
	SdFile srcFile;
	char currentPath[PATCH_SIZE];

	sprintf(currentPath, "%s/patterns/pattern_%02d.mtp", currentProjectPath, src);
	status = readPatternFile(currentPath, (uint8_t*)&songTrackCopy[0]);



	if(status)
	{
		storeSongUndoRevision(src);

		for(uint8_t track = 0; track < tracksNum; track++)
		{
			sequencer.clearSingleTrack(&songTrackCopy[0].track[track+trackStartSrc]);
		}

		writePatternFile(currentPath, (uint8_t*)&songTrackCopy[0]);
	}

	srcFile.close();
}

void cFileManager::updatePatternBitmask(uint8_t patternNum)
{

	updatePatternBitmask(patternNum - 1,sequencer.getPatternToSaveToFile());

}

void cFileManager::updatePatternBitmask(uint8_t index, uint8_t* sourcePattern)
{
	/*Update pattern usage bitmask*/

	for(uint8_t trackNum = 0; trackNum < 8; trackNum++)
	{
		uint8_t isAnyActive = 0;
		uint8_t length =  ((Sequencer::strPattern*)sourcePattern)->track[trackNum].length;

		for(uint8_t step = 0; step < length; step++)
		{
			if(((Sequencer::strPattern*)sourcePattern)->track[trackNum].step[step].note >= 0)
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




