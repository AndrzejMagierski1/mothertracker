#include "mtPatternExporter.h"
#include "mtAudioEngine.h"
#include "mtSequencer.h"
//#include "mtFileManager.h"
#include "fileManager.h"
#include "debugLog.h"
void setOnLastExportStep();

void mtPatternExporter::setOnLastStep()
{
	if(status != exportStatus::exportFinished)
	{
		sequencer.stop();
		lastStep=1;
	}

}

void mtPatternExporter::finishReceiving()
{
	if(status != exportStatus::exportFinished)
	{
		while ((exportL.available() >= 1) && (exportR.available() >= 1 ))
		{
			refreshReceiving();
		}
		exportL.end();
		exportR.end();

		status = exportStatus::exportFinishedReceiving;
	}
}
void mtPatternExporter::finishSave()
{
	if((position != 0) && (status == exportStatus::exportFinishedReceiving))
	{
		uint32_t saveLenBytes = 2 * position;
		position = 0;
		switchBuffer();
		byteRecorded += wavExport.write((int16_t*)sendBuf,saveLenBytes);
	}

	if(headerIsNotSaved && (status == exportStatus::exportFinishedReceiving))
	{
		headerIsNotSaved = false;

		header.chunkId = 0x46464952; 																// "RIFF"
		header.chunkSize = byteRecorded + 36;
		header.format = 0x45564157;																	// "WAVE"
		header.subchunk1Id = 0x20746d66;															// "fmt "
		header.subchunk1Size = 16;
		header.AudioFormat = 1;
		header.numChannels = 2;
		header.sampleRate = 44100;
		header.bitsPerSample = 16;
		header.byteRate = header.sampleRate * header.numChannels * (header.bitsPerSample/8);
		header.blockAlign = header.numChannels * (header.bitsPerSample/8);
		header.subchunk2Id = 0x61746164;															// "data"
		header.subchunk2Size = byteRecorded;


		wavExport.seek(0);
		wavExport.write(&header,sizeof(header));
		wavExport.close();

		status = exportStatus::exportFinished;
	}



}

char currentSongExportPath[PATCH_SIZE];

void mtPatternExporter::start(char * path)
{
	//__disable_irq();
	lastStep = 0;
	recBuf = buf1;
	sendBuf = buf2;
	lastSendBuf = sendBuf;
	requiredSave = false;
	strcpy(currentSongExportPath, path);

	if(SD.exists(currentSongExportPath)) SD.remove(currentSongExportPath);
	wavExport = SD.open(currentSongExportPath, FILE_WRITE);

	if(wavExport)
	{
		wavExport.write((int16_t*)recBuf,44); // wpisanie losowych danych zeby przesunac plik na pozycje za naglowkiem - potem zostana one nadpisane przez naglowek
		byteRecorded=0;
		status = exportStatus::exportDuring;
		headerIsNotSaved = true;

		exportL.begin();
		exportR.begin();
		sequencer.play();

	}



	//__enable_irq();

}

//odswiezanie zapisu danych do bufora z audio streamu
void mtPatternExporter::refreshReceiving()
{
	if(status == exportStatus::exportDuring)
	{
		if((recBuf == nullptr) || (sendBuf == nullptr)) return;


		if((requiredSave) && (position == SEND_BUF_SIZE - 256  ))
		{
			debugLog.addLine("Wait for save");
			return;
		}
		if ((exportL.available() >= 1) || (exportR.available() >= 1 ))
		{
			int16_t * srcL = exportL.readBuffer();
			int16_t * srcR = exportR.readBuffer();
			uint32_t * dest = (uint32_t*)(recBuf + position);

			for(uint8_t i = 0 ; i< 128 ; i++)
			{
				if(srcL == nullptr) *packageL = 0;
				else *packageL = *srcL++;

				if(srcR == nullptr) *packageR = 0;
				else *packageR = *srcR++;
				*dest++ = packageLR;
			}
			position += 256;

			exportL.freeBuffer();
			exportR.freeBuffer();

			if(position == SEND_BUF_SIZE)
			{
				__disable_irq();
				position = 0;
				switchBuffer();
				requiredSave = true;
				__enable_irq();
			}
		}
	}
}
// odswiezanie zapisu exportu na karte w petli glownej.
void mtPatternExporter::refreshSave()
{
	if(status == exportStatus::exportDuring)
	{
		if((recBuf == nullptr) || (sendBuf == nullptr)) return;

		if(requiredSave)
		{
			if(sendBuf == lastSendBuf) return;
			byteRecorded += wavExport.write(sendBuf,2 * SEND_BUF_SIZE);
			requiredSave = false;
			lastSendBuf = sendBuf;
		}
	}
}
void mtPatternExporter::updateReceiving()
{
	if(duringUpdateReceive) return;
	duringUpdateReceive = true;
	refreshReceiving();
	if(lastStep)
	{
		float rmsL = 0 , rmsR = 0;

		if(exportRmsL.available()  && exportRmsR.available() )
		{
			rmsL = exportRmsL.read();
			rmsR = exportRmsR.read();

			if((rmsL < 0.0001f) && (rmsR < 0.0001f))
			{
				lastStep = 0;
				finishReceiving();
			}
		}
	}
	duringUpdateReceive = false;
}
void mtPatternExporter::updateSave()
{
	refreshSave();
	finishSave();
}

void mtPatternExporter::switchBuffer()
{
	swapBuffer(&recBuf, &sendBuf);

}

void mtPatternExporter::swapBuffer(int16_t ** a, int16_t ** b)
{
	int16_t * tmp = *a;

	*a = *b;
	*b = tmp;
}


void mtPatternExporter::cancel()
{
	finishReceiving();
	finishSave();
	sequencer.stop();

	if(SD.exists(currentSongExportPath)) SD.remove(currentSongExportPath);
}

uint8_t mtPatternExporter::getStatus()
{
	return (uint8_t)status;
}

uint8_t mtPatternExporter::getProgress()
{
	return (sequencer.getActualPos() * 100) / (sequencer.getPatternLength() - 1);
}

