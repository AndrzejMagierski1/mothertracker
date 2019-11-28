#include "mtSongStemsExporter.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"

void mtSongTrackExporter::start(char * path, uint8_t track_n)
{
	if(track_n < 9)
	{
		currentTrack = track_n;
		setSoloTrack(track_n);
	}

	localSongExporter.start(path);
}
void mtSongTrackExporter::update()
{
	localSongExporter.update();
	//*********************** wykrywanie końca
	currentStatus = localSongExporter.getStatus();
	if((currentStatus == 0) && (currentStatus != lastStatus)) clearSoloTrack(currentTrack);
	lastStatus = currentStatus;
	//***********************
}
uint8_t mtSongTrackExporter::getStatus()
{
	return localSongExporter.getStatus();
}
/*********************************************************************************************************/
void mtSongStemsExporter::start(char * path)
{
	if(!SD.exists("Export")) SD.mkdir(0,"Export");

	songLength = 0;
	while(mtProject.song.playlist[songLength]) songLength++;

	char currentPath[PATCH_SIZE];

	sprintf(folderPath,"Export/%s",path);


	if(!SD.exists(folderPath)) SD.mkdir(0,folderPath);

	currentTrack = 0;
	sprintf(currentPath,"%s/track%d",folderPath, currentTrack + 1);

	status = 1;
	trackExporter.start(currentPath, currentTrack); // tablica żyje podczas korzystania z tego wskaznika

}

void mtSongStemsExporter::update()
{
	if(status == 1)
	{
		trackExporter.update();
		currentTrackState = trackExporter.getStatus();

		if((currentTrackState == 0) && (currentTrackState != lastTrackState))
		{
			currentTrack++;
			if(currentTrack > 9 ) status = 0;
			else if(currentTrack == 8)
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/reverb",folderPath);
				trackExporter.start(currentPath, currentTrack);
			}
			else if(currentTrack == 9)
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/mix",folderPath);
				trackExporter.start(currentPath, currentTrack);
			}
			else
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/track%d",folderPath, currentTrack + 1);
				trackExporter.start(currentPath, currentTrack);

			}
		}

		lastTrackState = currentTrackState;
	}

}

void mtSongStemsExporter::cancel()
{
	trackExporter.localSongExporter.finish();
	status = 0;
	sequencer.stop();
	char currentPath[PATCH_SIZE];

	for(uint8_t i = 0; i < 10; i ++ )
	{
		if(i == 8)
		{
			sprintf(currentPath,"%s/reverb.wav",folderPath);
			if(SD.exists(currentPath)) SD.remove(currentPath);
		}
		else if(i == 9)
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

}
uint8_t mtSongStemsExporter::getStatus()
{
	return status;
}

uint8_t mtSongStemsExporter::getProgress()
{
	uint8_t currentSongProgress = map(sequencer.getActualPos(),0,sequencer.getPatternLength() - 1,(mtProject.song.playlistPos * 100) / (songLength),( (mtProject.song.playlistPos + 1) * 100)/ (songLength));
	return 	(((currentTrack * 100) + currentSongProgress) * 100) / (10*100);
}
uint8_t mtSongStemsExporter::getTrack()
{
	return currentTrack;
}
