#include "mtPatternExporter.h"
#include "mtAudioEngine.h"
#include "mtSequencer.h"
#include "mtFileManager.h"
void setOnLastExportStep();

void mtPatternExporter::setOnLastStep()
{
	if(status != exportStatus::exportFinished)
	{
		sequencer.stop();
		lastStep=1;
	}

}

void mtPatternExporter::finish()
{
	__disable_irq();
	if(status != exportStatus::exportFinished)
	{
		while ((exportL.available() >= 1) && (exportR.available() >= 1 ))
		{
			refresh();
		}
		exportL.end();
		exportR.end();

		if(position != 0)
		{
			switchBuffer();
			__disable_irq();
			byteRecorded += wavExport.write(sendBuf,2 * position);
			__enable_irq();

			position=0;
		}
		status = exportStatus::exportFinished;


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

	}
	__enable_irq()
}
char currentSongExportPath[PATCH_SIZE];

void mtPatternExporter::start(char * path)
{
	__disable_irq();
	lastStep = 0;
	recBuf = buf1;
	sendBuf = buf2;
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
		sequencer.play();

	}



	__enable_irq();

}

void mtPatternExporter::refresh()
{

	if(status == exportStatus::exportDuring)
	{
		if((recBuf == nullptr) || (sendBuf == nullptr)) return;
		if ((exportL.available() >= 1) && (exportR.available() >= 1 ))
		{

			int16_t * srcL = exportL.readBuffer();
			int16_t * srcR = exportR.readBuffer();
			uint32_t * dest = (uint32_t*)(recBuf + position);

			for(uint8_t i = 0 ; i< 128 ; i++)
			{
				*packageL = *srcL++;
				*packageR = *srcR++;
				*dest++ = packageLR;
			}
			position+=256;

			exportL.freeBuffer();
			exportR.freeBuffer();



			if(position == SEND_BUF_SIZE)
			{
				switchBuffer();
				__disable_irq();
				byteRecorded += wavExport.write(sendBuf,2 * SEND_BUF_SIZE);
				__enable_irq();

				position=0;
			}
		}
	}

}


void mtPatternExporter::update()
{
	refresh();
	if(lastStep)
	{
		float rmsL = 0 , rmsR = 0;

		if(exportRmsL.available()  && exportRmsR.available() )
		{
			rmsL = exportRmsL.read();
			rmsR = exportRmsR.read();

			if((rmsL < 0.001f) && (rmsR < 0.001f))
			{
				lastStep = 0;
				finish();
			}
		}




	}
}

void mtPatternExporter::switchBuffer()
{
	int16_t * tmp = recBuf;

	recBuf = sendBuf;
	sendBuf = tmp;
}


uint8_t mtPatternExporter::getStatus()
{
	return (uint8_t)status;
}

uint8_t mtPatternExporter::getProgress()
{
	return (sequencer.getActualPos() * 100) / (sequencer.getPatternLength() - 1);
}

