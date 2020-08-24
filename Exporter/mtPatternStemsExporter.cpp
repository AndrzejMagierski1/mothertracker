#include "mtPatternStemsExporter.h"
#include "mtAudioEngine.h"
//#include "mtFileManager.h"
#include "fileManager.h"
#include "mtSequencer.h"



void mtPatternTrackExporter::start(char * path, uint8_t track_n)
{
	if(track_n < 10)
	{
		currentTrack = track_n;
		setSoloTrack(track_n);
	}

	if((track_n == 8) || (track_n == 10)) localPatternExporter.clearIgnoreDelay();
	else localPatternExporter.ignoreDelay();

	localPatternExporter.start(path);
}

void mtPatternTrackExporter::updateReceiving()
{
	localPatternExporter.updateReceiving();

	currentStatus = localPatternExporter.getStatus();
	if((currentStatus == 0) && (currentStatus != lastStatus)) clearSoloTrack(currentTrack);
	lastStatus = currentStatus;
}
void mtPatternTrackExporter::updateSave()
{
	localPatternExporter.updateSave();
}

uint8_t mtPatternTrackExporter::getStatus()
{
	return localPatternExporter.getStatus();
}


void  mtPatternTrackExporter::setSoloTrack(uint8_t n)
{
	if(n < 8) engine.soloTrack(n, 1);
	else if(n == 8) engine.soloDelaySend(1);
	else if(n == 9) engine.soloReverbSend(1);

}
void  mtPatternTrackExporter::clearSoloTrack(uint8_t n)
{
	if(n < 8) engine.soloTrack(n, 0);
	else if(n == 8) engine.soloDelaySend(0);
	else if(n == 9) engine.soloReverbSend(0);
}


void mtPatternStemsExporter::start(char * path)
{
	char currentPath[PATCH_SIZE];

	strcpy(folderPath, path);

	currentTrack = 0;
	for(uint8_t i = 0; i < 8 ; i++)
	{
		if(mtProject.values.trackMute[i]) currentTrack++;
		else break;
	}
	sprintf(currentPath,"%s/track%d.wav",folderPath, currentTrack + 1);

	trackExporter.start(currentPath, currentTrack); // tablica żyje podczas korzystania z tego wskaznika
	status = 1;

}
void mtPatternStemsExporter::updateReceiving()
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
void mtPatternStemsExporter::updateSave()
{
	trackExporter.updateSave();
}

void mtPatternStemsExporter::cancel()
{
	trackExporter.localPatternExporter.finishReceiving();
	trackExporter.localPatternExporter.finishSave();
	status = 0;
	sequencer.stop();
	char currentPath[PATCH_SIZE];

	for(uint8_t i = 0; i < 11; i ++ )
	{
		if(i == 8)
		{
			sprintf(currentPath,"%s/delay.wav",folderPath);
			if(SD.exists(currentPath)) SD.remove(currentPath);
		}
		else if(i == 9)
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
uint8_t mtPatternStemsExporter::getStatus()
{
	return status;
}

uint8_t mtPatternStemsExporter::getProgress()
{
	uint16_t patternLength =  sequencer.getPatternLength();
	return (currentTrack * patternLength  + sequencer.getActualPos()) * 100 / (11*patternLength);
}
uint8_t mtPatternStemsExporter::getTrack()
{
	return currentTrack;
}
