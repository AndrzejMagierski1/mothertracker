#include "mtExporterWAV.h"
#include "mtAudioEngine.h"

exportStatus status;
uint32_t byteRecorded;
FsFile wavExport;
int16_t sendBuf[SEND_BUF_SIZE];
uint16_t position;

void finishExport()
{
	__disable_irq();
	if(status != exportStatus::exportFinished)
	{
		uint32_t ChunkSize = 0L;
		uint32_t Subchunk1Size = 16;
		uint32_t AudioFormat = 1;
		uint32_t numChannels = 2;
		uint32_t sampleRate = 44100;
		uint32_t bitsPerSample = 16;
		uint32_t byteRate = sampleRate*numChannels*(bitsPerSample/8);
		uint32_t blockAlign = numChannels*bitsPerSample/8;
		uint32_t Subchunk2Size = 0;
		uint8_t byte1, byte2, byte3, byte4;

		while ((exportL.available() >= 1) && (exportR.available() >= 1 ))
		{
			updateExport();
		}
		sequencer.stop();
		exportL.end();
		exportR.end();
		engine.wavExportDisconnect();
		status = exportStatus::exportFinished;

		Subchunk2Size = byteRecorded;
		ChunkSize = Subchunk2Size + 36;
		wavExport.seek(0);
		wavExport.write("RIFF",4);
		byte1 = ChunkSize & 0xff;
		byte2 = (ChunkSize >> 8) & 0xff;
		byte3 = (ChunkSize >> 16) & 0xff;
		byte4 = (ChunkSize >> 24) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);  wavExport.write(byte3);  wavExport.write(byte4);
		wavExport.write("WAVE",4);
		wavExport.write("fmt ",4);
		byte1 = Subchunk1Size & 0xff;
		byte2 = (Subchunk1Size >> 8) & 0xff;
		byte3 = (Subchunk1Size >> 16) & 0xff;
		byte4 = (Subchunk1Size >> 24) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);  wavExport.write(byte3);  wavExport.write(byte4);
		byte1 = AudioFormat & 0xff;
		byte2 = (AudioFormat >> 8) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);
		byte1 = numChannels & 0xff;
		byte2 = (numChannels >> 8) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);
		byte1 = sampleRate & 0xff;
		byte2 = (sampleRate >> 8) & 0xff;
		byte3 = (sampleRate >> 16) & 0xff;
		byte4 = (sampleRate >> 24) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);  wavExport.write(byte3);  wavExport.write(byte4);
		byte1 = byteRate & 0xff;
		byte2 = (byteRate >> 8) & 0xff;
		byte3 = (byteRate >> 16) & 0xff;
		byte4 = (byteRate >> 24) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);  wavExport.write(byte3);  wavExport.write(byte4);
		byte1 = blockAlign & 0xff;
		byte2 = (blockAlign >> 8) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);
		byte1 = bitsPerSample & 0xff;
		byte2 = (bitsPerSample >> 8) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);
		wavExport.write("data",4);
		byte1 = Subchunk2Size & 0xff;
		byte2 = (Subchunk2Size >> 8) & 0xff;
		byte3 = (Subchunk2Size >> 16) & 0xff;
		byte4 = (Subchunk2Size >> 24) & 0xff;
		wavExport.write(byte1);  wavExport.write(byte2);  wavExport.write(byte3);  wavExport.write(byte4);
		wavExport.close();
		Serial.println("exportFinished");
	}
	__enable_irq()
}

void startExport(char * patch)
{
	__disable_irq();

	if(SD.exists(patch)) SD.remove(patch);
	wavExport = SD.open(patch,FILE_WRITE);
	if(wavExport)
	{
		sequencer.setOnPatternEnd(finishExport);
		byteRecorded=0;
		status = exportStatus::exportDuring;

		engine.wavExportConnect();

		exportL.begin();
		exportR.begin();
		sequencer.play();
		Serial.println("exportStarted");
	}



	__enable_irq();

}

void updateExport()
{
	if(status == exportStatus::exportDuring)
	{
		if ((exportL.available() >= 1) && (exportR.available() >= 1 ))
		{
			elapsedMicros apoloniuszTajmer=0;
			int16_t bufL[128];
			int16_t bufR[128];
			int16_t * srcL = bufL;
			int16_t * srcR = bufR;
			int16_t * dest = sendBuf+position;
			memcpy(bufL,exportL.readBuffer(),256);
			exportL.freeBuffer();
			memcpy(bufR,exportR.readBuffer(),256);
			exportR.freeBuffer();

			for(uint8_t i = 0 ; i< 128 ; i++)
			{
				*dest++=*srcL++;
				*dest++=*srcR++;
			}
			position+=256;

			if(position == SEND_BUF_SIZE)
			{
				__disable_irq();
				wavExport.write(sendBuf,SEND_BUF_SIZE*2);
				__enable_irq();
				byteRecorded+=SEND_BUF_SIZE*2;
				position=0;
			}

			Serial.println(apoloniuszTajmer);
		}
	}
}



uint8_t getExportStatus()
{
	return (uint8_t)status;
}

