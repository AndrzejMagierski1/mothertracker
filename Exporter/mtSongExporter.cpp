#include "mtSongExporter.h"
#include "mtAudioEngine.h"
#include "mtSequencer.h"
//#include "mtFileManager.h"
#include "fileManager.h"

extern char currentSongExportPath[PATCH_SIZE];

void mtSongExporter::start(char * path)
{
	//__disable_irq();
	songLength = 0;

	lastStep = 0;
	recBuf = buf1;
	sendBuf = buf2;
	requiredSave = false;

	songLength = 0;
	while(mtProject.song.playlist[songLength] && songLength  < SONG_MAX) songLength++;

	strcpy(currentSongExportPath, path);

	if(SD.exists(currentSongExportPath)) SD.remove(currentSongExportPath);
	wavExport = SD.open(currentSongExportPath, FILE_WRITE);

	if(wavExport)
	{
		wavExport.write(recBuf,44); // wpisanie losowych danych zeby przesunac plik na pozycje za naglowkiem - potem zostana one nadpisane przez naglowek
		byteRecorded=0;
		status = exportStatus::exportDuring;

		headerIsNotSaved = true;
		exportL.begin();
		exportR.begin();
		sequencer.playSong(0);
	}



	//__enable_irq();
}

uint8_t mtSongExporter::getProgress()
{
	return map(sequencer.getActualPos(),0,sequencer.getPatternLength() - 1, (mtProject.song.playlistPos * 100) / (songLength),( (mtProject.song.playlistPos + 1) * 100) / (songLength) );
}

