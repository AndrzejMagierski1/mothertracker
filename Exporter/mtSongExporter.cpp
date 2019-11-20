#include "mtSongExporter.h"
#include "mtAudioEngine.h"
#include "mtSequencer.h"
#include "mtFileManager.h"

extern char currentSongExportPath[PATCH_SIZE];

void mtSongExporter::start(char * path)
{
	__disable_irq();
	songLength = 0;

	lastStep = 0;
	recBuf = buf1;
	sendBuf = buf2;
	songLength = 0;

	while(mtProject.song.playlist[songLength]) songLength++;

	if(!SD.exists("Export")) SD.mkdir(0,"Export");

	sprintf(currentSongExportPath,"Export/%s",fileManager.currentProjectName);
	if(!SD.exists(currentSongExportPath)) SD.mkdir(0,currentSongExportPath);

	sprintf(currentSongExportPath,"%s.wav",path);

	if(SD.exists(currentSongExportPath)) SD.remove(currentSongExportPath);
	wavExport = SD.open(currentSongExportPath,FILE_WRITE);
	wavExport.seek(44);
	if(wavExport)
	{
		byteRecorded=0;
		status = exportStatus::exportDuring;

		exportL.begin();
		exportR.begin();
		sequencer.playSong(0);

		Serial.println("exportStarted");
	}



	__enable_irq();
}

uint8_t mtSongExporter::getProgress()
{
	return map(sequencer.getActualPos(),0,sequencer.getPatternLength() - 1, (mtProject.song.playlistPos * 100) / (songLength),( (mtProject.song.playlistPos + 1) * 100) / (songLength) );
}
