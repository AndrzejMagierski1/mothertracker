#include "mtSongStemsExporter.h"
#include "mtAudioEngine.h"
//#include "mtFileManager.h"
#include "fileManager.h"

void mtSongTrackExporter::start(char * path, uint8_t track_n)
{
	if(track_n < 10)
	{
		currentTrack = track_n;
		setSoloTrack(track_n);
	}

	localSongExporter.start(path);
}

void mtSongTrackExporter::updateReceiving()
{
	localSongExporter.updateReceiving();
	currentStatus = localSongExporter.getStatus();
	if((currentStatus == 0) && (currentStatus != lastStatus)) clearSoloTrack(currentTrack);
	lastStatus = currentStatus;
}
void mtSongTrackExporter::updateSave()
{
	localSongExporter.updateSave();
}

uint8_t mtSongTrackExporter::getStatus()
{
	return localSongExporter.getStatus();
}
/*********************************************************************************************************/
void mtSongStemsExporter::start(char * path)
{
	songLength = 0;
	while(mtProject.song.playlist[songLength] && songLength  < SONG_MAX) songLength++;

	char currentPath[PATCH_SIZE];

	strcpy(folderPath, path);
	currentTrack = 0;
	for(uint8_t i = 0; i < 8 ; i++)
	{
		if(mtProject.values.trackMute[i]) currentTrack++;
		else break;
	}

	sprintf(currentPath,"%s/track%d.wav", path, currentTrack + 1);


	trackExporter.start(currentPath, currentTrack); // tablica żyje podczas korzystania z tego wskaznika
	status = 1;

}

void mtSongStemsExporter::updateReceiving()
{
	if(status == 1)
	{
		trackExporter.updateReceiving();
		currentTrackState = trackExporter.getStatus();

		if((currentTrackState == 0) && (currentTrackState != lastTrackState))
		{
			currentTrack++;

			for(uint8_t i = currentTrack; i < 8 ; i++)
			{
				if(mtProject.values.trackMute[i]) currentTrack++;
				else break;
			}

			if(currentTrack > 10 ) status = 0;
			else if(currentTrack == 8)
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/delay.wav",folderPath);
				trackExporter.start(currentPath, currentTrack);
			}
			else if(currentTrack == 9)
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/reverb.wav",folderPath);
				trackExporter.start(currentPath, currentTrack);
			}
			else if(currentTrack == 10)
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/mix.wav",folderPath);
				trackExporter.start(currentPath, currentTrack);
			}
			else
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/track%d.wav",folderPath, currentTrack + 1);
				trackExporter.start(currentPath, currentTrack);

			}
		}

		lastTrackState = currentTrackState;
	}
}
void mtSongStemsExporter::updateSave()
{
	trackExporter.updateSave();
}

void mtSongStemsExporter::cancel()
{

	status = 0;
	sequencer.stop();
	trackExporter.localSongExporter.finishReceiving();
	trackExporter.localSongExporter.finishSave();
	char currentPath[PATCH_SIZE];

	for(uint8_t i = 0; i < 11; i ++ )
	{
		if(i == 8)
		{
			sprintf(currentPath,"%s/delay.wav",folderPath);
			if(SD.exists(currentPath)) SD.remove(currentPath);
		}
		if(i == 9)
		{
			sprintf(currentPath,"%s/reverb.wav",folderPath);
			if(SD.exists(currentPath)) SD.remove(currentPath);
		}
		else if(i == 10)
		{
			sprintf(currentPath,"%s/mix.wav",folderPath);
			if(SD.exists(currentPath)) SD.remove(currentPath);
		}
		else
		{
			sprintf(currentPath,"%s/track%d.wav",folderPath, i + 1);
			if(SD.exists(currentPath)) SD.remove(currentPath);
		}
	}
	if(SD.exists(folderPath)) SD.rmdir(folderPath);
	trackExporter.clearSoloTrack(currentTrack);

	currentTrackState = 0;
	lastTrackState = 0;
}
uint8_t mtSongStemsExporter::getStatus()
{
	return status;
}

uint8_t mtSongStemsExporter::getProgress()
{
	uint8_t currentSongProgress = map(sequencer.getActualPos(),0,sequencer.getPatternLength() - 1,(mtProject.song.playlistPos * 100) / (songLength),( (mtProject.song.playlistPos + 1) * 100)/ (songLength));
	return 	(((currentTrack * 100) + currentSongProgress) * 100) / (11*100);
}
uint8_t mtSongStemsExporter::getTrack()
{
	return currentTrack;
}
