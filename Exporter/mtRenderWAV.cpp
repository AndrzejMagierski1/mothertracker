#include "mtRenderWAV.h"
#include "mtAudioEngine.h"
//#include "mtFileManager.h"
#include "fileManager.h"
#include "mtExporterWAV.h"

char currentRenderExportPath[PATCH_SIZE];

void sendSelection();

void mtRenderWAV::start(char * path)
{
	//__disable_irq();
	lastStep = 0;
	recBuf = buf1;
	sendBuf = buf2;

	strcpy(currentRenderExportPath, path);


	if(SD.exists(currentRenderExportPath)) SD.remove(currentRenderExportPath);
	wavExport = SD.open(currentRenderExportPath, FILE_WRITE);

	if(wavExport)
	{
		wavExport.write(recBuf, 44); // wpisanie losowych danych zeby przesunac plik na pozycje za naglowkiem - potem zostana one nadpisane przez naglowek
		byteRecorded=0;
		status = exportStatus::exportDuring;

		exportL.begin();
		exportR.begin();

		sendSelection();
		sequencer.playSelection();
	}




	//__enable_irq();
}

void mtRenderWAV::cancel()
{
	finishReceiving();
	sequencer.stop();

	if(SD.exists(currentRenderExportPath)) SD.remove(currentRenderExportPath);
}

uint8_t mtRenderWAV::getProgress()
{
	return sequencer.getPlaySelectionProgress();
}

