#include "mtSongStemsExporter.h"

#include "mtAudioEngine.h"

extern mtExporter exporter;

void mtSongTrackExporter::start(char * path, uint8_t track_n)
{
	currentTrack = track_n;
	setSoloTrack(track_n);
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


void  mtSongTrackExporter::setSoloTrack(uint8_t n)
{
	if(n < 8) engine.soloTrack(n, 1);
	else if(n == 8) engine.soloReverbSend(0, 1);
	else if(n == 9) engine.soloReverbSend(1, 1);

}
void  mtSongTrackExporter::clearSoloTrack(uint8_t n)
{
	if(n < 8) engine.soloTrack(n, 0);
	else if(n == 8) engine.soloReverbSend(0, 0);
	else if(n == 9) engine.soloReverbSend(1, 0);
}


void mtSongStemsExporter::start(char * path)
{
	if(!SD.exists("Export")) SD.mkdir(0,"Export");

	char currentPath[PATCH_SIZE];

	sprintf(folderPath,"Export/%s",path);


	if(!SD.exists(folderPath)) SD.mkdir(0,folderPath);

	currentTrack = 0;
	sprintf(currentPath,"%s/track%d",folderPath, currentTrack + 1);

	status = 1;
	trackExporter.start(currentPath, currentTrack);

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
				sprintf(currentPath,"%s/reverbL%d",folderPath, currentTrack + 1);
				trackExporter.start(currentPath, currentTrack);
			}
			else if(currentTrack == 9)
			{
				char currentPath[PATCH_SIZE];
				sprintf(currentPath,"%s/reverbR%d",folderPath, currentTrack + 1);
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
uint8_t mtSongStemsExporter::getStatus()
{
	return status;
}
