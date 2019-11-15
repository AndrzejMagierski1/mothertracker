#include "mtPatternStemsExporter.h"
#include "mtAudioEngine.h"
#include "mtFileManager.h"
extern mtExporter exporter;

void mtPatternTrackExporter::start(char * path, uint8_t track_n)
{
	if(track_n < 9)
	{
		currentTrack = track_n;
		setSoloTrack(track_n);
	}


	localPatternExporter.start(path);
}

void mtPatternTrackExporter::update()
{
	localPatternExporter.update();
	//*********************** wykrywanie końca
	currentStatus = localPatternExporter.getStatus();
	if((currentStatus == 0) && (currentStatus != lastStatus)) clearSoloTrack(currentTrack);
	lastStatus = currentStatus;
	//***********************
}

uint8_t mtPatternTrackExporter::getStatus()
{
	return localPatternExporter.getStatus();
}


void  mtPatternTrackExporter::setSoloTrack(uint8_t n)
{
	if(n < 8) engine.soloTrack(n, 1);
	else if(n == 8) engine.soloReverbSend(1);

}
void  mtPatternTrackExporter::clearSoloTrack(uint8_t n)
{
	if(n < 8) engine.soloTrack(n, 0);
	else if(n == 8) engine.soloReverbSend(0);
}


void mtPatternStemsExporter::start(char * path)
{
	if(!SD.exists("Export")) SD.mkdir(0,"Export");

	char currentPath[PATCH_SIZE];

	sprintf(folderPath,"Export/%s",path);


	if(!SD.exists(folderPath)) SD.mkdir(0,folderPath);

	currentTrack = 0;
	sprintf(currentPath,"%s/track%d",folderPath, currentTrack + 1);

	status = 1;
	trackExporter.start(currentPath, currentTrack); // tablica żyje podczas korzystania z tego wskaznika

}
void mtPatternStemsExporter::update()
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
				sprintf(currentPath,"%s/song",folderPath);
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
uint8_t mtPatternStemsExporter::getStatus()
{
	return status;
}
